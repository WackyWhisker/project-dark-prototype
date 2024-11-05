// Copyright @ Christian Reichel


#include "Component/DkTargetingComponent.h"
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
	if (!PlayerController)
	{
		PlayerController = Cast<IDkPlayerControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
	}

	//Bind all relevant delegates
	//TODO: Consider which of these should be in the child states only
	if(PlayerController)
	{
		PlayerController->GetTargetStartDelegate()->AddUObject(this, &UDkTargetingComponent::OnTargetStart);
		PlayerController->GetTargetEndDelegate()->AddUObject(this, &UDkTargetingComponent::OnTargetEnd);
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
}

