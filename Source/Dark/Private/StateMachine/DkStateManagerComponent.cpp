// Copyright @ Christian Reichel

#include "StateMachine/DkStateManagerComponent.h"
#include "StateMachine/States/DkStateBase.h"

UDkStateManagerComponent::UDkStateManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDkStateManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeStates()	;
}

void UDkStateManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (CurrentState && bCanStateTick)
	{
		CurrentState->TickState();
	}

	if (bDebug)
	{
		// Start with oldest states at the top
		int32 NumStatesToShow = FMath::Min(StateHistory.Num(), StateHistoryLength);
		for (int32 i = NumStatesToShow - 1; i >= 0; i--)
		{
			FColor MessageColor = (i == 0) ? FColor::Orange : FColor::Yellow;
			FString StateNum = FString::Printf(TEXT("previous state %d: "), i + 1);
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, MessageColor, 
				this->GetOwner()->GetName() + "'s " + StateNum + 
				StateHistory[i]->StateDisplayName.GetPlainNameString());
		}
    
		// Current state displayed last (at the bottom)
		if (CurrentState)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, 
				this->GetOwner()->GetName() + "'s current state: " + 
				CurrentState->StateDisplayName.GetPlainNameString());
		}
	}
}

void UDkStateManagerComponent::SwitchStateByKey(FString StateKey)
{
	//See if a state with that key exists in the state map
	UDkStateBase* NewState = StateMap.FindRef(StateKey);

	//if yes valid state
	if (NewState->IsValidLowLevel())
	{
		//If there is no current state yet(begin), new state should be the first current state
		if(!CurrentState)
		{
			CurrentState = NewState;
			NewState->PreviousState = nullptr;
		}
		else
		{
			//if same state and not repeatle
			if (CurrentState->GetClass() == NewState->GetClass() && CurrentState->bCanStateRepeat == false)
			{
				if (bDebug)
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, this->GetOwner()->GetName() + "'s state switch failed, because " + CurrentState->StateDisplayName.GetPlainNameString() + " is not set to repeatable!", true);
				}
			}
			//else state is repeatable or it's a different state alltogether
			else
			{
				NewState->PreviousState = CurrentState;
				
				//stop ticking of current state
				CurrentState->bCanStateTick = false;
				//call exit on current state
				CurrentState->OnStateExit();

				//if there is space in state history, add, if not remove oldest and then push
				if(StateHistory.Num()<StateHistoryLength)
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
		}

		if (CurrentState)
		{
			CurrentState->OnStateEnter(GetOwner());
			bCanStateTick = true;
		}
	}
	//if no valid state
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, this->GetOwner()->GetName() + "'s state switch failed, because the state is invalid!");
	}
}

void UDkStateManagerComponent::SwitchStateByObject(UDkStateBase* StateObject)
{
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

