// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DkTargetingComponent.generated.h"

class IDkPlayerControllerInterface;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARK_API UDkTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDkTargetingComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
	IDkPlayerControllerInterface* PlayerController = nullptr;

private:
	UFUNCTION()
	void OnTargetStart();
	UFUNCTION()
	void OnTargetEnd();

	UPROPERTY()
	bool bIsTargeting = false;

	
	
};
