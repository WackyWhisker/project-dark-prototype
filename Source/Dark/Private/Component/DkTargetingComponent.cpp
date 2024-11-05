﻿// Copyright @ Christian Reichel


#include "Component/DkTargetingComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/DkPlayerControllerInterface.h"
#include "TargetSystem/DkTargetableInterface.h"

UDkTargetingComponent::UDkTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	if (!bIsTargeting) {return;}
	

}

void UDkTargetingComponent::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("Targeting component initialized"));

	if (!PlayerRef)
	{
		PlayerRef = Cast<ADkCharacter>(GetOwner());
		if (!PlayerControllerRef)
		{
			PlayerControllerRef = Cast<ADkPlayerController>(PlayerRef->GetController());
		}
		if (!PlayerCameraRef)
		{
			PlayerCameraRef = PlayerRef->FindComponentByClass<UCameraComponent>();
		}
		if (!PlayerSpringArmRef)
		{
			PlayerSpringArmRef = PlayerRef->FindComponentByClass<USpringArmComponent>();
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

	TArray<FHitResult> Hits;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(PlayerRef);
	SweepForPossibleTargets(PlayerRef->GetActorLocation(), 1000.0f, 45.0f, 1000.0f, Hits, ECC_Visibility, IgnoreActors, true);
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
	HandleTargetClearing(bIsTargeting);
}

void UDkTargetingComponent::HandlePlayerLocomotion(bool IsTargeting)
{
	if (PlayerRef)
	{
		PlayerRef->GetCharacterMovement()->bOrientRotationToMovement = !IsTargeting;
	}
	if (PlayerSpringArmRef)
	{
		//PlayerSpringArmRef->bUsePawnControlRotation = !IsTargeting;
	}
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

void UDkTargetingComponent::HandleEnemyTrackingWhileTargeting(float DeltaTime)
{
	UpdatePlayerRotationWhileTargeting(DeltaTime);
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

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 15.0f);
	PlayerRef->SetActorRotation(NewRotation);
}

void UDkTargetingComponent::UpdateCameraWhileTargeting(float DeltaTime)
{
	if (!PlayerRef || !CurrentActiveTarget || !PlayerControllerRef) {return;}

	FVector TargetLocation = CurrentActiveTarget->GetActorLocation();
	FVector CameraLocation = PlayerCameraRef->GetComponentLocation();

	FRotator CurrentRotation = PlayerCameraRef->GetComponentRotation();
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, TargetLocation);

	TargetRotation.Pitch = CurrentRotation.Pitch;
	TargetRotation.Roll = CurrentRotation.Roll;
	
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 15.0f);

	//PlayerControllerRef->SetControlRotation(NewRotation); //TODO: Figure out how to set correctly the spring arm rotation
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

