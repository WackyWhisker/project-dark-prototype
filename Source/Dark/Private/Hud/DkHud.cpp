// Copyright @ Christian Reichel

#include "Hud/DkHUD.h"
#include "Hud/DkPauseMenuStack.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DkPlayerController.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

ADkHUD::ADkHUD()
{
}

void ADkHUD::BeginPlay()
{
    Super::BeginPlay();

    // Create the pause menu stack widget
    if (BaseMenuStackClass)
    {
       BaseMenuStack = CreateWidget<UDkBaseMenuStack>(GetOwningPlayerController(), BaseMenuStackClass);
       BaseMenuStack->AddToViewport();  // We add the stack container immediately but it starts empty

       // Bind to the pause menu delegate
       if (ADkPlayerController* PC = Cast<ADkPlayerController>(GetOwningPlayerController()))
       {
          if (!PlayerControllerInterface)
          {
             PlayerControllerInterface = Cast<IDkPlayerControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
          }
          if (PlayerControllerInterface)
          {
             PlayerControllerInterface->GetTogglePauseMenuDelegate()->AddUObject(this, &ADkHUD::HandleTogglePauseMenu);
          }
      }
    }
}

void ADkHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
   if(PlayerControllerInterface)
   {
      PlayerControllerInterface->GetTogglePauseMenuDelegate()->RemoveAll(this);
   }
    Super::EndPlay(EndPlayReason);
}

void ADkHUD::HandleTogglePauseMenu()
{
   if (bIsPauseMenuVisible)
   {
      BaseMenuStack->OnHidePauseMenuWidget();
      bIsPauseMenuVisible = false;
   }
   else
   {
      BaseMenuStack->OnShowPauseMenuWidget();
      bIsPauseMenuVisible = true;
   }
}

void ADkHUD::HandleToggleEndScreenMenu()
{
   if (bIsEndScreenMenuVisible)
   {
      BaseMenuStack->OnHideEndScreenMenuWidget();
      bIsEndScreenMenuVisible = false;
      UE_LOG(LogTemp, Warning, TEXT("Hide Endscreen menu"));
   }
   else
   {
      BaseMenuStack->OnShowEndScreenMenuWidget();
      bIsEndScreenMenuVisible = true;
      UE_LOG(LogTemp, Warning, TEXT("Show Endscreen menu"));
   }
}
