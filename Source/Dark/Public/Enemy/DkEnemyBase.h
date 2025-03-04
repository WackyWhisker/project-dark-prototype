﻿// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TargetSystem/DkTargetableInterface.h"
#include "DkEnemyBase.generated.h"

class UWidgetComponent;
class UUserWidget;
class UDkDamageFlashComponent;
class UDkEnemyHealthWidget;

UCLASS()
class DARK_API ADkEnemyBase : public ACharacter, public IDkTargetableInterface
{
	GENERATED_BODY()

public:
	ADkEnemyBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	//Target UI
	UPROPERTY(EditDefaultsOnly, Category = UI_Targeting)
	UWidgetComponent* TargetReticleWidget;

	UPROPERTY(EditDefaultsOnly, Category = UI_Targeting)
	TSubclassOf<UUserWidget> TargetReticleWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthWidgetClass;

	UPROPERTY()
	UDkEnemyHealthWidget* HealthWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UWidgetComponent* HealthWidgetComponent;

	UFUNCTION(BlueprintCallable, Category = UI_Targeting)
	void ToggleTargetReticle(bool bShowTargetReticle);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
	UDkDamageFlashComponent* FlashComponent;

	//IDkTargetableInterface functions
	virtual bool CanBeTargeted_Implementation() const override;
	virtual void OnTargeted_Implementation() override;
	virtual void OnUntargeted_Implementation() override;
	virtual FVector GetTargetLocation_Implementation() const override;

private:
	UFUNCTION()
	void OnHealthChangedHandler(float Health, float HealthDelta, float MaxHealth);

	UFUNCTION()
	void OnBindingChangedHandler(float CurrentBinding, float MaxBinding);

	void UpdateHealthWidget();
};