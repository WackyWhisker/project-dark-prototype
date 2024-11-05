// Copyright @ Christian Reichel


#include "Component/DkTargetingComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DkPlayerControllerInterface.h"

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
		if (!PlayerControllerRef)
		{
			PlayerControllerRef = Cast<ADkPlayerController>(PlayerRef->GetController());
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
}

void UDkTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bIsTargeting) {return;} //TODO: Consider disabling tick all together instead of driving by bool
}

void UDkTargetingComponent::OnTargetEnd()
{
	UE_LOG(LogTemp, Log, TEXT("Targeting component executing TargetEnd"));
	bIsTargeting = false;
	HandlePlayerLocomotion(bIsTargeting);
	HandleLetterboxWidget(bIsTargeting);
}

void UDkTargetingComponent::HandlePlayerLocomotion(bool IsTargeting)
{
	if (PlayerRef)
	{
		PlayerRef->GetCharacterMovement()->bOrientRotationToMovement = !IsTargeting;
	}
}

void UDkTargetingComponent::HandleLetterboxWidget(bool IsTargeting)
{
	if (PlayerControllerRef)
	{
		PlayerControllerRef->ToggleLetterboxUI(IsTargeting);
	}
}

