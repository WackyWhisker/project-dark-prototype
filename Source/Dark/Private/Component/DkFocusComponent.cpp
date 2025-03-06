// Copyright @ Christian Reichel

#include "Component/DkFocusComponent.h"
#include "Player/DkPlayerControllerInterface.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

UDkFocusComponent::UDkFocusComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bIsFocused = false;
    bUseFocusToggle = true;
    CurrentMode = EDkFocusMode::Firearm;
}

void UDkFocusComponent::BeginPlay()
{
    Super::BeginPlay();

    // Get player controller interface
    APawn* Pawn = Cast<APawn>(GetOwner());
    if (Pawn)
    {
        if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
        {
            PlayerControllerInterface = Cast<IDkPlayerControllerInterface>(PC);
            if (PlayerControllerInterface)
            {
                PlayerControllerInterface->GetFocusStartDelegate()->AddUObject(this, &UDkFocusComponent::HandleFocusStart);
                PlayerControllerInterface->GetFocusEndDelegate()->AddUObject(this, &UDkFocusComponent::HandleFocusEnd);
                PlayerControllerInterface->GetSwitchFocusModeDelegate()->AddUObject(this, &UDkFocusComponent::HandleFocusModeSwitch);
            }
        }
    }
}

void UDkFocusComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    
    if (PlayerControllerInterface)
    {
        PlayerControllerInterface->GetFocusStartDelegate()->RemoveAll(this);
        PlayerControllerInterface->GetFocusEndDelegate()->RemoveAll(this);
        PlayerControllerInterface->GetSwitchFocusModeDelegate()->RemoveAll(this);
    }
}

void UDkFocusComponent::HandleFocusStart()
{
    bIsFocused = true;
    // Broadcast initial focus mode
    OnFocusModeChanged.Broadcast(CurrentMode, EDkFocusMode::None);
    OnFocusStateChanged.Broadcast(true);
}

void UDkFocusComponent::HandleFocusEnd()
{
    bIsFocused = false;
    OnFocusStateChanged.Broadcast(false);
}

void UDkFocusComponent::HandleFocusModeSwitch()
{
    if (!bIsFocused)
        return;
        
    EDkFocusMode NewMode = (CurrentMode == EDkFocusMode::Firearm) ? 
        EDkFocusMode::Scanning : EDkFocusMode::Firearm;
        
    EDkFocusMode OldMode = CurrentMode;
    CurrentMode = NewMode;
    
    OnFocusModeChanged.Broadcast(NewMode, OldMode);
}

void UDkFocusComponent::SetFocusToggleMode(bool bNewToggleMode)
{
    if (bUseFocusToggle == bNewToggleMode)
        return;
        
    bUseFocusToggle = bNewToggleMode;
    
    if (!bUseFocusToggle && bIsFocused)
    {
        HandleFocusEnd();
    }
}