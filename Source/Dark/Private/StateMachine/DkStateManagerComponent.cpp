// Copyright @ Christian Reichel

#include "StateMachine/DkStateManagerComponent.h"
#include "StateMachine/States/DkStateBase.h"


UDkStateManagerComponent::UDkStateManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UDkStateManagerComponent::BeginPlay()
{
    Super::BeginPlay();
    //InitializeStates();
}

void UDkStateManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    // Handle any pending state transitions before ticking
    if (PendingStateTransition.IsValidTransition())
    {
        ExecuteStateTransition();
    }
    
    if (CurrentState && bCanStateTick)
    {
        CurrentState->TickState();
    }

    if (bDebug)
    {
        // Current state displayed first (at the top)
        if (CurrentState)
        {
            GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, 
                this->GetOwner()->GetName() + "'s current state: " + 
                CurrentState->StateDisplayName.GetPlainNameString());
        }

        // Show previous states newest to oldest
        int32 NumStatesToShow = FMath::Min(StateHistory.Num(), StateHistoryLength);
        for (int32 i = 0; i < NumStatesToShow; i++)
        {
            FColor MessageColor = FColor::Yellow;
            FString StateNum = FString::Printf(TEXT("previous state %d: "), i + 1);
            GEngine->AddOnScreenDebugMessage(-1, 0.0f, MessageColor, 
                this->GetOwner()->GetName() + "'s " + StateNum + 
                StateHistory[i]->StateDisplayName.GetPlainNameString());
        }
    }
}

void UDkStateManagerComponent::SwitchStateByKey(FString StateKey)
{
    UDkStateBase* NewState = StateMap.FindRef(StateKey);
    
    if (NewState && NewState->IsValidLowLevel())
    {
        // Instead of immediate transition, queue it
        PendingStateTransition.NewState = NewState;
        PendingStateTransition.bIsValid = true;
        
        // Optionally immediately stop ticking current state to prevent further logic execution
        if (CurrentState)
        {
            CurrentState->bCanStateTick = false;
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, 
            this->GetOwner()->GetName() + "'s state switch failed, because the state is invalid!");
    }
}

void UDkStateManagerComponent::ExecuteStateTransition()
{
    if (!PendingStateTransition.IsValidTransition())
    {
        return;
    }

    UDkStateBase* NewState = PendingStateTransition.NewState;

    // Handle first state initialization
    if (!CurrentState)
    {
        CurrentState = NewState;
        NewState->PreviousState = nullptr;
    }
    else
    {
        // Find current state's key
        FString CurrentStateKey;
        for (const auto& StatePair : StateMap)
        {
            if (StatePair.Value == CurrentState)
            {
                CurrentStateKey = StatePair.Key;
                break;
            }
        }

        // Check if transition is restricted
        if (NewState->RestrictedFromStates.Contains(CurrentStateKey))
        {
            if (bDebug)
            {
                GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, 
                    this->GetOwner()->GetName() + "'s state switch failed, because transition from " + 
                    CurrentStateKey + " is restricted!", true);
            }
            PendingStateTransition.ResetTransition();
            return;
        }

        // Check for non-repeatable state
        if (CurrentState->GetClass() == NewState->GetClass() && !CurrentState->bCanStateRepeat)
        {
            if (bDebug)
            {
                GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, 
                    this->GetOwner()->GetName() + "'s state switch failed, because " + 
                    CurrentState->StateDisplayName.GetPlainNameString() + " is not set to repeatable!", true);
            }
            PendingStateTransition.ResetTransition();
            return;
        }

        NewState->PreviousState = CurrentState;
        CurrentState->OnStateExit();

        // Update state history
        if (StateHistory.Num() < StateHistoryLength)
        {
            StateHistory.Push(CurrentState);
        }
        else
        {
            StateHistory.RemoveAt(0);
            StateHistory.Push(CurrentState);
        }

        CurrentState = NewState;
    }

    if (CurrentState)
    {
        CurrentState->OnStateEnter(GetOwner());
        bCanStateTick = true;
    }

    // Clear the pending transition
    PendingStateTransition.ResetTransition();
}

void UDkStateManagerComponent::SwitchStateByObject(UDkStateBase* StateObject)
{
    // You can implement this if needed
}

void UDkStateManagerComponent::InitializeStateManager()
{
    SwitchStateByKey(InitialState);
}

void UDkStateManagerComponent::InitializeStates()
{
    //Create all states from available states as objects and add them to the state map one by one
    for (auto It = AvailableStates.CreateConstIterator(); It; ++It)
    {
        UDkStateBase* State = NewObject<UDkStateBase>(this, It->Value);
        StateMap.Add(It->Key, State);
    }
}

FPendingStateTransition::FPendingStateTransition() 
    : NewState(nullptr)
    , bIsValid(false)
{
}

void FPendingStateTransition::ResetTransition()
{
    NewState = nullptr;
    bIsValid = false;
}

bool FPendingStateTransition::IsValidTransition() const 
{ 
    return bIsValid && NewState != nullptr; 
}