// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DkNpcBase.generated.h"

class UWidgetComponent;
class UUserWidget;

UCLASS()
class DARK_API ADkNpcBase : public ACharacter
{
	GENERATED_BODY()

public:
	ADkNpcBase();

	virtual void Tick(float DeltaTime) override;

	//Dialogue UI
	UPROPERTY(BlueprintReadOnly, Category = UI_Targeting)
	UWidgetComponent* DialoguePromptWidget;

	UPROPERTY(EditDefaultsOnly, Category = UI_Targeting)
	TSubclassOf<UUserWidget> DialoguePromptWidgetClass;

	UFUNCTION(BlueprintCallable, Category = UI_Targeting)
	void ToggleDialoguePrompt(bool bShowDialoguePrompt);

protected:
	virtual void BeginPlay() override;

};
