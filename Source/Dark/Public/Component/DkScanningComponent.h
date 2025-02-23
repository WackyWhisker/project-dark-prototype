// DkScanningComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DkScanningComponent.generated.h"

class IDkPlayerControllerInterface;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARK_API UDkScanningComponent : public UActorComponent
{
	GENERATED_BODY()

public:    
	UDkScanningComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void OnScanModeStart();
    
	UFUNCTION()
	void OnScanModeEnd();
    
	UFUNCTION()
	void OnScanExecuteStart();
    
	UFUNCTION()
	void OnScanExecuteEnd();

	IDkPlayerControllerInterface* PlayerControllerInterface = nullptr;
};