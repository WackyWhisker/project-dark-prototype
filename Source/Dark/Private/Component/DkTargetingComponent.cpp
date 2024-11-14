// Copyright @ Christian Reichel


#include "Component/DkTargetingComponent.h"

#include "Camera/CameraComponent.h"
#include "Component/DkHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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
		PlayerControllerInterface->GetTargetCycleLeftDelegate()->AddUObject(this, &UDkTargetingComponent::OnTargetCycleLeft);
		PlayerControllerInterface->GetTargetCycleRightDelegate()->AddUObject(this, &UDkTargetingComponent::OnTargetCycleRight);
		
	}
}

void UDkTargetingComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

bool UDkTargetingComponent::FindBestTarget(AActor*& OutBestTarget, float SphereRadius, float MaxAngleInDegrees)
{
	TArray<AActor*> TargetsInRange;
	TArray<AActor*> TargetsInCone;
    
	// Early out if no targets in range
	if (!GetTargetableActorsInRange(TargetsInRange, SphereRadius))
	{
		return false;
	}
    
	// Early out if no targets in cone
	if (!FilterTargetsByViewCone(TargetsInRange, TargetsInCone, MaxAngleInDegrees))
	{
		return false;
	}

	OutBestTarget = TargetsInCone[0];
	return true;
}

bool UDkTargetingComponent::CycleTarget(bool bGoingRight)
{
	if (!CurrentActiveTarget) {return false;}
	TArray<AActor*> TargetsInRange;
	TArray<AActor*> TargetsInCone;

	if (!GetTargetableActorsInRange(TargetsInRange, TargetableActorsScanRange) ||
		!FilterTargetsByViewCone(TargetsInRange, TargetsInCone, MaxViewConeAngle))
	{
		UE_LOG(LogTemp, Log, TEXT("There are no further targets to target at all"));
		return false;
	}

	FVector CameraLocation = PlayerCameraRef->GetComponentLocation();
	FVector CameraForward = PlayerCameraRef->GetForwardVector();
	FVector CameraRight = PlayerCameraRef->GetRightVector();

	// Get current target's direction and side
	FVector DirToCurrent = (CurrentActiveTarget->GetActorLocation() - CameraLocation).GetSafeNormal();
	float CurrentSide = FVector::DotProduct(DirToCurrent, CameraRight);

	AActor* BestNextTarget = nullptr;
	float BestDot = -1.0f;

	for (AActor* Target : TargetsInCone)
	{
		if (Target == CurrentActiveTarget) continue;
        
		FVector DirToTarget = (Target->GetActorLocation() - CameraLocation).GetSafeNormal();
		float TargetSide = FVector::DotProduct(DirToTarget, CameraRight);
        
		if ((bGoingRight && TargetSide > CurrentSide) || (!bGoingRight && TargetSide < CurrentSide))
		{
			float DotWithForward = FVector::DotProduct(DirToTarget, CameraForward);
			if (DotWithForward > BestDot)
			{
				BestDot = DotWithForward;
				BestNextTarget = Target;
			}
		}
	}

	if (BestNextTarget)
	{
		//If there is a current active one, first run the untarget logic, and then after set the new current one.
		if (CurrentActiveTarget && CurrentActiveTarget->Implements<UDkTargetableInterface>())
		{
			IDkTargetableInterface::Execute_OnUntargeted(CurrentActiveTarget);
		}

		//Just another sanity null check
		CurrentActiveTarget = BestNextTarget;
		if (CurrentActiveTarget && CurrentActiveTarget->Implements<UDkTargetableInterface>())
		{
			IDkTargetableInterface::Execute_OnTargeted(CurrentActiveTarget);
			//Subscribe to active target health comp
			HealthComponent = CurrentActiveTarget->FindComponentByClass<UDkHealthComponent>();
			if (HealthComponent)
			{
				// Remove any existing binding first
				HealthComponent->OnHealthDepleted.RemoveDynamic(this, &UDkTargetingComponent::OnCurrentTargetHealthDepleted);
				// Then add the new binding
				HealthComponent->OnHealthDepleted.AddDynamic(this, &UDkTargetingComponent::OnCurrentTargetHealthDepleted);
			}
		}
		return true;
	}
	return false;
	
}

bool UDkTargetingComponent::GetTargetableActorsInRange(TArray<AActor*>& OutTargetableActors, float SphereRadius)
{
	if (!PlayerSpringArmRef) { return false;}

	//Start of sphere to search within
	FVector StartLocation = PlayerRef->GetActorLocation();

	// Temp array to store all overlapped actors
	TArray<FOverlapResult> OverlapResults;

	// Configure query parameters
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActor(PlayerRef);

	// Set up shape collision parameters
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(SphereRadius);

	// Perform overlap check
	bool bSuccess = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		StartLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_Visibility,
		CollisionShape,
		QueryParams
	);

	// Standard usage with sensible defaults
	DrawDebugSphere(
		GetWorld(),              // World context
		StartLocation,            // Location in world space
		SphereRadius,            // Radius in Unreal units (1 unit = 1cm)
		12,                // Number of segments (more = smoother sphere)
		FColor::Red,       // Color of the sphere
		true,             // Persistent lines
		-1.0f,             // Lifetime (-1 = infinite)
		0,                 // Depth priority
		1.0f              // Thickness of lines
	);

	if (bSuccess)
	{
		// Filter results for ITargetable interface
		for (const FOverlapResult& Result : OverlapResults)
		{
			if (AActor* OverlappedActor = Result.GetActor())
			{
				// Check if actor implements ITargetable
				if (OverlappedActor->Implements<UDkTargetableInterface>())
				{
					OutTargetableActors.Add(OverlappedActor);
					DrawDebugSphere(
		GetWorld(),              
		OverlappedActor->GetActorLocation(),            
		100.0f,            
		12,                
		FColor::Yellow,       
		true,             
		-1.0f,             
		0,                 
		1.0f              
	);
				}
			}
		}
	}
    
	return OutTargetableActors.Num() > 0;
	
}

bool UDkTargetingComponent::FilterTargetsByViewCone(const TArray<AActor*>& PotentialTargets,
	TArray<AActor*>& OutValidTargets, float MaxAngleInDegrees)
{
	if (!PlayerCameraRef) { return  false;}
	
	
	// Get camera forward vector and location
	FVector CameraLocation;
	FVector CameraForward;
	
	CameraLocation = PlayerCameraRef->GetComponentLocation();
	CameraForward = PlayerCameraRef->GetForwardVector();
    
	// Convert max angle to cosine for comparison
	float CosMaxAngle = FMath::Cos(FMath::DegreesToRadians(MaxAngleInDegrees));

	// Struct to store actor and its dot product for sorting
	struct FTargetDot {                                    // 1
		AActor* Actor;                                     // 2
		float DotProduct;                                  // 3
    
		FTargetDot(AActor* InActor, float InDot)          // 4
			: Actor(InActor), DotProduct(InDot) {}        // 5
	};

	// Temporary array to store valid targets with their dot products
	TArray<FTargetDot> ValidTargetsWithDot;
	
	// Check each potential target
	for (AActor* Target : PotentialTargets)
	{
		if (!Target) continue;
        
		// Get direction to target (normalized)
		FVector DirectionToTarget = (Target->GetActorLocation() - CameraLocation).GetSafeNormal();
        
		// Calculate dot product (gives us cosine of angle between vectors)
		float DotProduct = FVector::DotProduct(CameraForward, DirectionToTarget);
        
		// If dot product is greater than cosine of our max angle, target is in cone
		if (DotProduct >= CosMaxAngle)
		{
			ValidTargetsWithDot.Add(FTargetDot(Target, DotProduct));
			DrawDebugSphere(
		GetWorld(),              
		Target->GetActorLocation(),            
		120.0f,            
		12,                
		FColor::Green,       
		true,             
		-1.0f,             
		0,                 
		2.0f              
	);
		}
	}
    
	// Sort by dot product (highest/most centered first)
	ValidTargetsWithDot.Sort([](const FTargetDot& A, const FTargetDot& B) {
		return A.DotProduct > B.DotProduct;
	});
    
	// Fill output array with sorted actors
	OutValidTargets.Empty();
	for (const FTargetDot& TargetDot : ValidTargetsWithDot)
	{
		OutValidTargets.Add(TargetDot.Actor);
	}
    
	return OutValidTargets.Num() > 0;
}

void UDkTargetingComponent::OnTargetStart()
{
	UE_LOG(LogTemp, Log, TEXT("Targeting component executing TargetStart"));
	bIsTargeting = true;
	ToggleLetterboxWidget(bIsTargeting);
	TogglePlayerDefaults(bIsTargeting);
	ToggleSpringArmDefaults(bIsTargeting);
	InitiateSweepForTargets();
}

void UDkTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!bIsTargeting) {return;} 

	if (CurrentActiveTarget)
	{
		HandleEnemyTrackingWhileTargeting(DeltaTime);
	}
	else
	{
		UpdateCameraWithoutActiveTarget(DeltaTime);
	}
	
}

void UDkTargetingComponent::OnTargetEnd()
{
	UE_LOG(LogTemp, Log, TEXT("Targeting component executing TargetEnd"));
	bIsTargeting = false;
	ToggleLetterboxWidget(bIsTargeting);
	TogglePlayerDefaults(bIsTargeting);
	ToggleSpringArmDefaults(bIsTargeting);
	RestoreCameraPositionOnEnd();
	ClearTarget(bIsTargeting);

	// This clears everything - including spheres, lines, boxes, etc.
	FlushPersistentDebugLines(GetWorld());
	
}

void UDkTargetingComponent::OnTargetCycleLeft()
{
	UE_LOG(LogTemp, Log, TEXT("Targeting component executing TargetCycleLeft"));
	if (!CycleTarget(false))
	{
		UE_LOG(LogTemp, Log, TEXT("There is no more target to the left"));
	}
}

void UDkTargetingComponent::OnTargetCycleRight()
{
	UE_LOG(LogTemp, Log, TEXT("Targeting component executing TargetCycleRight"));
	if (!CycleTarget(true))
	{
		UE_LOG(LogTemp, Log, TEXT("There is no more target to the right"));
	}
}

//Do always when starting or ending targeting
void UDkTargetingComponent::ToggleLetterboxWidget(bool IsTargeting)
{
	if (PlayerControllerRef)
	{
		PlayerControllerRef->ToggleLetterboxUI(IsTargeting);
	}
}


void UDkTargetingComponent::ToggleSpringArmDefaults(bool IsTargeting)
{
	if (!PlayerSpringArmRef) {return;}
	if (IsTargeting)
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
	}
}

//Do always when starting or ending targeting
void UDkTargetingComponent::TogglePlayerDefaults(bool IsTargeting)
{
	if (PlayerRef)
	{
		PlayerRef->GetCharacterMovement()->bOrientRotationToMovement = !bIsTargeting;
	}
}

void UDkTargetingComponent::StoreCameraPositionOnStart()
{
	
}

void UDkTargetingComponent::RestoreCameraPositionOnEnd()
{
	if (!PlayerSpringArmRef || !PlayerControllerRef) {	return;}
	
	FRotator RestoredRotation = PlayerRef->GetActorRotation();
	RestoredRotation.Pitch = LastUsedTargetRotation.Pitch;
	PlayerControllerRef->SetControlRotation(RestoredRotation);
}

void UDkTargetingComponent::ClearTarget(bool IsTargeting)
{
	if (CurrentActiveTarget && CurrentActiveTarget->Implements<UDkTargetableInterface>())
	{
		IDkTargetableInterface::Execute_OnUntargeted(CurrentActiveTarget);
		CurrentActiveTarget = nullptr;
	}
}

void UDkTargetingComponent::HandleEnemyTrackingWhileTargeting(float DeltaTime)
{
	UpdatePlayerRotationWhileTargeting(DeltaTime);
	UpdateCameraWithActiveTarget(DeltaTime);
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

void UDkTargetingComponent::UpdateCameraWithActiveTarget(float DeltaTime)
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

void UDkTargetingComponent::UpdateCameraWithoutActiveTarget(float DeltaTime)
{
	if (!PlayerRef || !PlayerSpringArmRef || !PlayerCameraRef) {return;}
	// Calculate ideal rotation to look at target
	
	static FRotator TargetRotation = FRotator(-14.0f, 0.0f, 0.0f);
    
	// Smoothly interpolate rotation
	FRotator NewRotation = FMath::RInterpTo(
		PlayerSpringArmRef->GetRelativeRotation(),
		TargetRotation,
		DeltaTime,
		InterpSpeed
	);
	PlayerSpringArmRef->SetRelativeRotation(NewRotation);
	LastUsedTargetRotation = NewRotation;
	
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
	FindBestTarget(CurrentActiveTarget, TargetableActorsScanRange, MaxViewConeAngle);
	if (CurrentActiveTarget && CurrentActiveTarget->Implements<UDkTargetableInterface>())
	{
		IDkTargetableInterface::Execute_OnTargeted(CurrentActiveTarget);
		//Subscribe to active target health comp
		HealthComponent = CurrentActiveTarget->FindComponentByClass<UDkHealthComponent>();
		if (HealthComponent)
		{
			// Remove any existing binding first
			HealthComponent->OnHealthDepleted.RemoveDynamic(this, &UDkTargetingComponent::OnCurrentTargetHealthDepleted);
			// Then add the new binding
			HealthComponent->OnHealthDepleted.AddDynamic(this, &UDkTargetingComponent::OnCurrentTargetHealthDepleted);
		}
	}
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


