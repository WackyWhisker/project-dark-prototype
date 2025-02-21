#include "LevelObject/RoomSocketActor.h"

ARoomSocketActor::ARoomSocketActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // Create and set up components
    SocketMesh = CreateDefaultSubobject<UStaticMeshComponent>("SocketMesh");
    SetRootComponent(SocketMesh);
    
    DirectionArrow = CreateDefaultSubobject<UArrowComponent>("DirectionArrow");
    DirectionArrow->SetupAttachment(RootComponent);
    DirectionArrow->SetArrowSize(100.0f);
    DirectionArrow->SetArrowLength(100.0f);

    SocketTypeText = CreateDefaultSubobject<UTextRenderComponent>("SocketTypeText");
    SocketTypeText->SetupAttachment(RootComponent);
    SocketTypeText->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
    SocketTypeText->SetWorldSize(75.0f);
    SocketTypeText->SetTextRenderColor(FColor::White);
    SocketTypeText->SetHorizontalAlignment(EHTA_Center);
    SocketTypeText->SetVerticalAlignment(EVRTA_TextCenter);

    // Set default socket type
    SocketType = ERoomSocketType::Socket_Entry;
    SocketTypeText->SetText(FText::FromString(TEXT("Entry")));

    // Only show in editor, hide in game
    SocketMesh->SetVisibility(false);
#if WITH_EDITOR
    DirectionArrow->SetVisibility(true);
    SocketTypeText->SetVisibility(true);
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
void ARoomSocketActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // Update visuals when properties change in editor
    if (PropertyChangedEvent.Property)
    {
        const FName PropertyName = PropertyChangedEvent.Property->GetFName();
        if (PropertyName == GET_MEMBER_NAME_CHECKED(ARoomSocketActor, SocketType))
        {
            FString DisplayText = (SocketType == ERoomSocketType::Socket_Entry) ? TEXT("Entry") : TEXT("Exit");
            SocketTypeText->SetText(FText::FromString(DisplayText));
        }
    }
}
#endif