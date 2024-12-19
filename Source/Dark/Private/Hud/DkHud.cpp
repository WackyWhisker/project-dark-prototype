// Copyright @ Christian Reichel

#include "Hud/DkHUD.h"
#include "Hud/DkPauseMenuStack.h"
#include "Hud/DkBaseMenuWidget.h"
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
    if (PauseMenuStackClass)
    {
       PauseMenuStack = CreateWidget<UDkPauseMenuStack>(GetOwningPlayerController(), PauseMenuStackClass);
       PauseMenuStack->AddToViewport();  // We add the stack container immediately but it starts empty

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
  // if (!PauseMenuStack) {return;}
   if (!PlayerControllerInterface || !PlayerControllerInterface->GetTogglePauseMenuDelegate()->IsBoundToObject(this))
   {
      UE_LOG(LogTemp, Warning, TEXT("Delegate binding issue detected"));
   }
   UE_LOG(LogTemp, Warning, TEXT("Toggling shizzle ma nizzle"));
   
   if (bIsMenuVisible)
   {
     // PauseMenuStack->OnHideMenuWidget.Broadcast();
      PauseMenuStack->OnHideMenuWidget();
      bIsMenuVisible = false;
   }
   else
   {
      //PauseMenuStack->OnShowMenuWidget.Broadcast();
      PauseMenuStack->OnShowMenuWidget();
      bIsMenuVisible = true;
   }
}