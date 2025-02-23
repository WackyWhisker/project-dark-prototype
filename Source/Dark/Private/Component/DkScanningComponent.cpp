// DkScanningComponent.cpp
#include "Component/DkScanningComponent.h"
#include "Player/DkPlayerControllerInterface.h"
#include "Kismet/GameplayStatics.h"

UDkScanningComponent::UDkScanningComponent()
{
   PrimaryComponentTick.bCanEverTick = false; // No tick needed yet
}

void UDkScanningComponent::BeginPlay()
{
   Super::BeginPlay();

   // Get interface reference
   if (!PlayerControllerInterface)
   {
       PlayerControllerInterface = Cast<IDkPlayerControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
   }

   // Bind delegates
   if (PlayerControllerInterface)
   {
       PlayerControllerInterface->GetScanModeStartDelegate()->AddUObject(this, &UDkScanningComponent::OnScanModeStart);
       PlayerControllerInterface->GetScanModeEndDelegate()->AddUObject(this, &UDkScanningComponent::OnScanModeEnd);
       PlayerControllerInterface->GetScanExecuteStartDelegate()->AddUObject(this, &UDkScanningComponent::OnScanExecuteStart);
       PlayerControllerInterface->GetScanExecuteEndDelegate()->AddUObject(this, &UDkScanningComponent::OnScanExecuteEnd);
   }
}

void UDkScanningComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay(EndPlayReason);
   
   // Clean up delegates if needed
   if (PlayerControllerInterface)
   {
       PlayerControllerInterface->GetScanModeStartDelegate()->RemoveAll(this);
       PlayerControllerInterface->GetScanModeEndDelegate()->RemoveAll(this);
       PlayerControllerInterface->GetScanExecuteStartDelegate()->RemoveAll(this);
       PlayerControllerInterface->GetScanExecuteEndDelegate()->RemoveAll(this);
   }
}

void UDkScanningComponent::OnScanModeStart()
{
   UE_LOG(LogTemp, Log, TEXT("Scan Mode Started"));
}

void UDkScanningComponent::OnScanModeEnd()
{
   UE_LOG(LogTemp, Log, TEXT("Scan Mode Ended"));
}

void UDkScanningComponent::OnScanExecuteStart()
{
   UE_LOG(LogTemp, Log, TEXT("Scan Execute Started"));
}

void UDkScanningComponent::OnScanExecuteEnd()
{
   UE_LOG(LogTemp, Log, TEXT("Scan Execute Ended"));
}