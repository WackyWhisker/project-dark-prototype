// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DkStateManagerComponent.generated.h"

class UDkStateBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARK_API UDkStateManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDkStateManagerComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StateManager")
	FString InitialState;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StateManager")
	TMap<FString, TSubclassOf<UDkStateBase>> AvailableStates;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StateManager_Debug")
	bool bDebug = false;

	UPROPERTY(BlueprintReadOnly, Category = "StateManager")
	TArray<UDkStateBase*> StateHistory;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StateManager_Debug", meta = (ClampMin = 0, ClampMax = 10, UIMin = 0, UIMax = 10))
	int32 StateHistoryLength; 
	
	UPROPERTY(BlueprintReadOnly, Category = "StateManager")
	UDkStateBase* CurrentState = nullptr;
	
	UPROPERTY()
	TMap<FString, UDkStateBase*> StateMap;

	UFUNCTION(BlueprintCallable, Category = "StateManager")
	void SwitchStateByKey(FString StateKey);

	UFUNCTION(BlueprintCallable, Category = "StateManager")
	void SwitchStateByObject(UDkStateBase* StateObject);

	UFUNCTION(BlueprintCallable, Category = "StateManager")
	void InitializeStateManager();


private:
	bool bCanStateTick = true;
	void InitializeStates();
};
