// Copyright @ Christian Reichel

#include "Subsystem/CastleSocketActor.h"

ACastleSocketActor::ACastleSocketActor()
{
	//bAllowDeferredSpawning = false;
    // Create and set up components
    SocketMesh = CreateDefaultSubobject<UStaticMeshComponent>("SocketMesh");
    SetRootComponent(SocketMesh);
    
    DirectionArrow = CreateDefaultSubobject<UArrowComponent>("DirectionArrow");
    DirectionArrow->SetupAttachment(RootComponent);
    DirectionArrow->SetArrowSize(100.0f);
	DirectionArrow->SetArrowLength(100.0f);

    SocketNameText = CreateDefaultSubobject<UTextRenderComponent>("SocketNameText");
    SocketNameText->SetupAttachment(RootComponent);
    SocketNameText->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	SocketNameText->SetWorldSize(75.0f);
    SocketNameText->SetTextRenderColor(FColor::White);
    SocketNameText->SetHorizontalAlignment(EHTA_Center);
    SocketNameText->SetVerticalAlignment(EVRTA_TextCenter);

    // Set default socket name
    SocketName = "socket_1";
    SocketNameText->SetText(FText::FromName(SocketName));

    // Only show in editor, hide in game
    SocketMesh->SetVisibility(false);
#if WITH_EDITOR
    DirectionArrow->SetVisibility(true);
    SocketNameText->SetVisibility(true);
#endif
    
#if WITH_EDITOR
    // Load editor-only visualization mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> 
       SphereMesh(TEXT("/Engine/BasicShapes/Sphere"));
    if(SphereMesh.Succeeded())
    {
       SocketMesh->SetStaticMesh(SphereMesh.Object);
       SocketMesh->SetRelativeScale3D(FVector(0.25f));
    }
#endif
}

#if WITH_EDITOR
void ACastleSocketActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // Update visuals when properties change in editor
    if (PropertyChangedEvent.Property)
    {
       const FName PropertyName = PropertyChangedEvent.Property->GetFName();
       if (PropertyName == GET_MEMBER_NAME_CHECKED(ACastleSocketActor, SocketName))
       {
          SocketNameText->SetText(FText::FromName(SocketName));
       }
       else if (PropertyName == GET_MEMBER_NAME_CHECKED(UTextRenderComponent, Text))
       {
          SocketName = FName(*SocketNameText->Text.ToString());
       }
    }
}
#endif