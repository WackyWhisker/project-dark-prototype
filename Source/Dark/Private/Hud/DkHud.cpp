// Copyright @ Christian Reichel

#include "Hud/DkHUD.h"
#include "Hud/DkPauseMenuStack.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DkPlayerController.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

ADkHUD::ADkHUD()
{
    bIsPauseMenuVisible = false;
    bIsEndScreenMenuVisible = false;
    bIsUpgradeMenuVisible = false;
    bIsRetreatScreenMenuVisible = false;
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
             PlayerControllerInterface->GetToggleUpgradeMenuDelegate()->AddUObject(this, &ADkHUD::HandleToggleUpgradeMenu);
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

void ADkHUD::HandleToggleUpgradeMenu()
{
   if (bIsUpgradeMenuVisible)
   {
      BaseMenuStack->OnHideUpgradeMenuWidget();
      bIsUpgradeMenuVisible = false;
   }
   else
   {
      BaseMenuStack->OnShowUpgradeMenuWidget();
      bIsUpgradeMenuVisible = true;
   }
}

void ADkHUD::HandleToggleEndScreenMenu()
{
   if (bIsEndScreenMenuVisible)
   {
      BaseMenuStack->OnHideEndScreenMenuWidget();
      bIsEndScreenMenuVisible = false;
   }
   else
   {
      BaseMenuStack->OnShowEndScreenMenuWidget();
      bIsEndScreenMenuVisible = true;
   }
}

void ADkHUD::HandleToggleRetreatScreenMenu()
{
   if (bIsRetreatScreenMenuVisible)
   {
      BaseMenuStack->OnHideRetreatScreenMenuWidget();
      bIsRetreatScreenMenuVisible = false;
   }
   else
   {
      BaseMenuStack->OnShowRetreatScreenMenuWidget();
      bIsRetreatScreenMenuVisible = true;
   }
}