// Copyright @ Christian Reichel


#include "Component/DkTargetingComponent.h"

#include "Camera/CameraComponent.h"
#include "Component/DkHealthComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/DkPlayerControllerInterface.h"
#include "TargetSystem/DkTargetableInterface.h"

UDkTargetingComponent::UDkTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDkTargetingComponent::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("Targeting component initialized"));

	if (!PlayerRef)
	{
		PlayerRef = Cast<ADkCharacter>(GetOwner());
	}

	if (PlayerRef)
	{
		if (!PlayerControllerRef)
		{
			PlayerControllerRef = Cast<ADkPlayerController>(PlayerRef->GetController());
		}
    
		if (!PlayerSpringArmRef)
		{
			PlayerSpringArmRef = Cast<USpringArmComponent>(PlayerRef->GetComponentByClass(USpringArmComponent::StaticClass()));
		}

		if (PlayerSpringArmRef)
		{
			PlayerCameraRef = Cast<UCameraComponent>(PlayerSpringArmRef->GetChildComponent(0));
		}
	}
	
	if (!PlayerControllerInterface)
	{
		PlayerControllerInterface = Cast<IDkPlayerControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
	}

	//Bind all relevant delegates
	//TODO: Consider which of these should be in the child states only
	if(PlayerControllerInterface)
	{
		PlayerControllerInterface->GetTargetStartDelegate()->AddUObject(this, &UDkTargetingComponent::OnTargetStart);
		PlayerControllerInterface->GetTargetEndDelegate()->AddUObject(this, &UDkTargetingComponent::OnTargetEnd);
	}
}

void UDkTargetingComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UDkTargetingComponent::OnTargetStart()
{
	UE_LOG(LogTemp, Log, TEXT("Targeting component executing TargetStart"));
	bIsTargeting = true;
	HandlePlayerLocomotion(bIsTargeting); 
	HandleLetterboxWidget(bIsTargeting);

	InitiateSweepForTargets();
}

void UDkTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!bIsTargeting) {return;} //TODO: Consider disabling tick all together instead of driving by bool

	HandleEnemyTrackingWhileTargeting(DeltaTime);
}

void UDkTargetingComponent::OnTargetEnd()
{
	UE_LOG(LogTemp, Log, TEXT("Targeting component executing TargetEnd"));
	bIsTargeting = false;
	HandlePlayerLocomotion(bIsTargeting);
	HandleLetterboxWidget(bIsTargeting);
	HandleSpringArmDefaults(bIsTargeting);
	HandleTargetClearing(bIsTargeting);
}

void UDkTargetingComponent::HandlePlayerLocomotion(bool IsTargeting)
{
}

void UDkTargetingComponent::HandleLetterboxWidget(bool IsTargeting)
{
	if (PlayerControllerRef)
	{
		PlayerControllerRef->ToggleLetterboxUI(IsTargeting);
	}
}

void UDkTargetingComponent::HandleTargetClearing(bool IsTargeting)
{
	if (CurrentActiveTarget && CurrentActiveTarget->Implements<UDkTargetableInterface>())
	{
		IDkTargetableInterface::Execute_OnUntargeted(CurrentActiveTarget);
		CurrentActiveTarget = nullptr;
	}
}

void UDkTargetingComponent::HandleSpringArmDefaults(bool IsTargeting)
{
	if (!PlayerSpringArmRef) {return;}
	if (IsTargeting && CurrentActiveTarget)
	{
		PlayerSpringArmRef->bUsePawnControlRotation = false;
		PlayerSpringArmRef->bEnableCameraLag = true;
		PlayerSpringArmRef->bEnableCameraRotationLag = true;
	}
	else
	{
		PlayerSpringArmRef->bUsePawnControlRotation = true;
		PlayerSpringArmRef->bEnableCameraLag = false;
		PlayerSpringArmRef->bEnableCameraRotationLag = false;

		//restore camera value
		if (CurrentActiveTarget)
		{
			FRotator RestoredRotation = PlayerRef->GetActorRotation();
			RestoredRotation.Pitch = LastUsedTargetRotation.Pitch;
			PlayerSpringArmRef->SetRelativeRotation(RestoredRotation);
			PlayerControllerRef->SetControlRotation(RestoredRotation);
		}
	}
}

void UDkTargetingComponent::HandleEnemyTrackingWhileTargeting(float DeltaTime)
{
	UpdatePlayerRotationWhileTargeting(DeltaTime);
	UpdateCameraWhileTargeting(DeltaTime);
}

void UDkTargetingComponent::UpdatePlayerRotationWhileTargeting(float DeltaTime)
{
	if (!CurrentActiveTarget || !PlayerRef) {return;}
	FVector TargetLocation = CurrentActiveTarget->GetActorLocation();
	FVector PlayerLocation = PlayerRef->GetActorLocation();

	FRotator CurrentRotation = PlayerRef->GetActorRotation();
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, TargetLocation);

	TargetRotation.Pitch = CurrentRotation.Pitch;
	TargetRotation.Roll = CurrentRotation.Roll;

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 150.0f);
	PlayerRef->SetActorRotation(NewRotation);
}

void UDkTargetingComponent::UpdateCameraWhileTargeting(float DeltaTime)
{
	if (!PlayerRef || !CurrentActiveTarget || !PlayerSpringArmRef || !PlayerCameraRef) {return;}

	// Calculate ideal rotation to look at target
	FRotator TargetRotation = CalculateIdealSpringArmRotation();
    
	// Smoothly interpolate rotation
	FRotator NewRotation = FMath::RInterpTo(
		PlayerSpringArmRef->GetRelativeRotation(),
		TargetRotation,
		DeltaTime,
		InterpSpeed
	);
	PlayerSpringArmRef->SetRelativeRotation(NewRotation);
	LastUsedTargetRotation = NewRotation;

	// Update spring arm length
	float TargetLength = CalculateIdealSpringArmLength();
	float NewLength = FMath::FInterpTo(
		PlayerSpringArmRef->TargetArmLength,
		TargetLength,
		DeltaTime,
		InterpSpeed
	);
	//PlayerSpringArmRef->TargetArmLength = NewLength;

	// Update camera FOV based on distance
	if (PlayerCameraRef)
	{
		float Distance = FVector::Distance(PlayerRef->GetActorLocation(), CurrentActiveTarget->GetActorLocation());
		float NormalizedDistance = FMath::GetMappedRangeValueClamped(
			FVector2D(MinTargetDistance, TargetArmLength),
			FVector2D(0.0f, 1.0f),
			Distance
		);
		float TargetFOV = FMath::Lerp(MinFOV, MaxFOV, NormalizedDistance);
		float NewFOV = FMath::FInterpTo(PlayerCameraRef->FieldOfView, TargetFOV, DeltaTime, InterpSpeed);
		//PlayerCameraRef->SetFieldOfView(NewFOV);
	}
	
}

FRotator UDkTargetingComponent::CalculateIdealSpringArmRotation() const
{
	// Find the midpoint between player and target
	FVector MidPoint = (PlayerRef->GetActorLocation() + CurrentActiveTarget->GetActorLocation()) * 0.5f;
    
	// Calculate the direction from player to target
	FVector TargetDirection = (CurrentActiveTarget->GetActorLocation() - PlayerRef->GetActorLocation()).GetSafeNormal();
    
	// Calculate camera position that's perpendicular to the player-target line
	FVector CameraOffset = FVector::CrossProduct(TargetDirection, FVector::UpVector);
	CameraOffset.Z = CameraHeight / TargetArmLength; // Normalize the height based on arm length
	CameraOffset.Normalize();
    
	// Convert to relative rotation
	FVector CameraPosition = MidPoint + CameraOffset * TargetArmLength;
	FRotator WorldRotation = UKismetMathLibrary::FindLookAtRotation(CameraPosition, MidPoint);

	// Subtract 90 degrees from the Yaw to correct the spring arm's orientation
	WorldRotation.Yaw -= 90.0f;
	
	return (WorldRotation - PlayerRef->GetActorRotation()).GetNormalized();
	
}

float UDkTargetingComponent::CalculateIdealSpringArmLength() const
{
	// Calculate distance between characters
	float Distance = FVector::Distance(PlayerRef->GetActorLocation(), CurrentActiveTarget->GetActorLocation());
    
	// Scale arm length based on distance, but ensure it's never less than our minimum
	float IdealLength = FMath::Max(Distance * 0.75f, MinTargetDistance);
    
	// Clamp to our configured maximum
	return FMath::Min(IdealLength, TargetArmLength);
}

void UDkTargetingComponent::InitiateSweepForTargets()
{
	TArray<FHitResult> Hits;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(PlayerRef);
	SweepForPossibleTargets(PlayerRef->GetActorLocation(), 1000.0f, 45.0f, 1000.0f, Hits, ECC_Visibility, IgnoreActors, true);
}

void UDkTargetingComponent::OnCurrentTargetHealthDepleted()
{
	CurrentActiveTarget = nullptr;

	FTimerHandle TickTimerHandle;
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		// Code that will run next frame
		InitiateSweepForTargets();
	});
	
}


bool UDkTargetingComponent::SweepForPossibleTargets(const FVector& Start, const float Range, const float ConeAngle,
                                                    const float SphereRadius, TArray<FHitResult>& OutHits, ECollisionChannel TraceChannel,
                                                    const TArray<AActor*>& ActorsToIgnore, bool bDrawDebug)
{
	FRotator CameraRotation = PlayerControllerRef->PlayerCameraManager->GetCameraRotation();
	FVector CameraForward = CameraRotation.Vector();
	const FVector ForwardVector = CameraForward;
	const float HalfAngleRad = FMath::DegreesToRadians(ConeAngle * 0.5f);
	const float EndRadius = Range * FMath::Tan(HalfAngleRad);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(ActorsToIgnore);
	const FVector EndLocation = Start + (ForwardVector * Range);

	bool bHit = GetWorld()->SweepMultiByChannel(OutHits, Start, EndLocation, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(SphereRadius), QueryParams);

	for (int32 i = OutHits.Num() - 1; i >= 0; i--)
	{
		const FHitResult& Hit = OutHits[i];
    
		if (!Hit.GetActor() || !Hit.GetActor()->Implements<UDkTargetableInterface>())
		{
			OutHits.RemoveAt(i);
			continue;
		}

		FVector Origin;
		FVector Extend;
		Hit.GetActor()->GetActorBounds(false, Origin, Extend);

		// Check if actor is visible in viewport
		FVector2D ScreenLocation;
		bool bIsInViewport = PlayerControllerRef->ProjectWorldLocationToScreen(Origin, ScreenLocation);

		if (!bIsInViewport)
		{
			OutHits.RemoveAt(i);
		}
	}
	if (OutHits.Num() > 0)
	{
		CurrentActiveTarget = FindClosestTarget(OutHits);
		if (CurrentActiveTarget && CurrentActiveTarget->Implements<UDkTargetableInterface>())
		{
			IDkTargetableInterface::Execute_OnTargeted(CurrentActiveTarget);
			//Subscribe to active target health comp
			HealthComponent = CurrentActiveTarget->FindComponentByClass<UDkHealthComponent>();
			if (HealthComponent)
			{
				HealthComponent->OnHealthDepleted.AddDynamic(this, &UDkTargetingComponent::OnCurrentTargetHealthDepleted);
			}


			//Subscribe done
			HandleSpringArmDefaults(true);
		}
	}
	return bHit;
}

AActor* UDkTargetingComponent::FindClosestTarget(const TArray<FHitResult>& Hits)
{
	AActor* ClosestTarget = nullptr;
	float ClosestDistanceSq = TNumericLimits<float>::Max();

	for (const FHitResult& Hit : Hits)
	{
		AActor* Actor = Hit.GetActor();
		if (!Actor)
		{
			continue;
		}
		const float DistSq = FVector::DistSquared(Actor->GetActorLocation(), PlayerRef->GetActorLocation());
		if (DistSq < ClosestDistanceSq)
		{
			ClosestDistanceSq = DistSq;
			ClosestTarget = Actor;
		}
	}

	return ClosestTarget;
}



