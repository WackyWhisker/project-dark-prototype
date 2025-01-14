// Copyright @ Christian Reichel


#include "Npc/DkNpcBase.h"
#include "Components/WidgetComponent.h"

ADkNpcBase::ADkNpcBase()
{
	PrimaryActorTick.bCanEverTick = true;
	//Dialogue UI
	// Create the widget component
	DialoguePromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DialoguePromptWidget"));
	DialoguePromptWidget->SetupAttachment(RootComponent);
	DialoguePromptWidget->SetVisibility(false);
    
	// Configure the widget
	DialoguePromptWidget->SetWidgetSpace(EWidgetSpace::Screen); // Makes it face the camera
	DialoguePromptWidget->SetRelativeLocation(FVector(0, 0, 100)); // Adjust position above npc
}

void ADkNpcBase::ToggleDialoguePrompt(bool bShowDialoguePrompt)
{
	DialoguePromptWidget->SetVisibility(bShowDialoguePrompt);
}

void ADkNpcBase::BeginPlay()
{
	Super::BeginPlay();
	if (DialoguePromptWidget)
	{
		DialoguePromptWidget->SetWidgetClass(DialoguePromptWidgetClass);
		DialoguePromptWidget->SetVisibility(false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Something not set correctly on the target reticle widget class"));
	}
	
}

void ADkNpcBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

