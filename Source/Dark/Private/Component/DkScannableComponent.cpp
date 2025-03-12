// Copyright @ Christian Reichel

#include "Component/DkScannableComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

UDkScannableComponent::UDkScannableComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UDkScannableComponent::BeginPlay()
{
    Super::BeginPlay();
    if (AActor* Owner = GetOwner())
    {
        OriginalScale = Owner->GetActorScale3D();
        
        // Create and setup the sphere
        DetectionSphere = NewObject<USphereComponent>(Owner, TEXT("DetectionSphere"));
        DetectionSphere->RegisterComponent();
        DetectionSphere->SetCollisionProfileName(TEXT("Scannable")); 
        DetectionSphere->SetSphereRadius(DetectionRadius);
        DetectionSphere->SetHiddenInGame(true);
        DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        DetectionSphere->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
        
        // Optionally disable collision on the main mesh for traces
        if (UStaticMeshComponent* MeshComp = Owner->FindComponentByClass<UStaticMeshComponent>())
        {
            MeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
        }
    }
}

void UDkScannableComponent::OnScanStart()
{
    bIsBeingScanned = true;
    CurrentScanProgress = 0.0f;
    if (AActor* Owner = GetOwner())
    {
        this->OriginalLocation = Owner->GetActorLocation();
    }
}

void UDkScannableComponent::OnScanProgress(float Progress)
{
    CurrentScanProgress = Progress;
    
    if (AActor* Owner = GetOwner())
    {
        // Amplitude increases exponentially with progress
        float ProgressFactor = FMath::Pow(Progress, 2.0f);
        float CurrentAmplitude = BaseAmplitude + (MaxAdditionalAmplitude * ProgressFactor);
        
        float Time = GetWorld()->GetTimeSeconds();
        float HorizontalWave = FMath::Sin(Time * VibrationFrequency);
        float VerticalWave = FMath::Cos(Time * VibrationFrequency);
        
        FVector RightVector = Owner->GetActorRightVector();
        FVector UpVector = Owner->GetActorUpVector();
        
        FVector NewLocation = this->OriginalLocation + 
            (RightVector * HorizontalWave * CurrentAmplitude) +
            (UpVector * VerticalWave * CurrentAmplitude);
            
        Owner->SetActorLocation(NewLocation);
    }
}

void UDkScannableComponent::OnScanComplete()
{
    bIsBeingScanned = false;
    CurrentScanProgress = 1.0f;
    
    if (AActor* Owner = GetOwner())
    {
        Owner->SetActorLocation(this->OriginalLocation);//TODO: superflous
        Owner->Destroy();
    }
}

void UDkScannableComponent::OnScanAborted()
{
    bIsBeingScanned = false;
    if (bResetOnAbort)
    {
        CurrentScanProgress = 0.0f;
    }
    if (AActor* Owner = GetOwner())
    {
        Owner->SetActorLocation(this->OriginalLocation);
    }
}

void UDkScannableComponent::OnScanModeEntered()
{
    if (AActor* Owner = GetOwner())
    {
        if (UStaticMeshComponent* MeshComponent = Owner->FindComponentByClass<UStaticMeshComponent>())
        {
            // Store original material if we haven't already
            if (!OriginalMaterial)
            {
                OriginalMaterial = MeshComponent->GetMaterial(0);
            }
            
            if (HighlightMaterial)
            {
                MeshComponent->SetMaterial(0, HighlightMaterial);
            }
            else
            {
                // Create dynamic material instance for orange color
                UMaterialInterface* CurrentMat = MeshComponent->GetMaterial(0);
                if (CurrentMat)
                {
                    UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(CurrentMat, this);
                    DynMaterial->SetVectorParameterValue(FName("BaseColor"), FLinearColor(1.0f, 0.5f, 0.0f, 1.0f));
                    MeshComponent->SetMaterial(0, DynMaterial);
                }
            }
        }
    }
    //UE_LOG(LogTemp, Log, TEXT("Scannable %s entered scan mode"), *GetOwner()->GetName());
}

void UDkScannableComponent::OnScanModeExited()
{
    if (AActor* Owner = GetOwner())
    {
        if (UStaticMeshComponent* MeshComponent = Owner->FindComponentByClass<UStaticMeshComponent>())
        {
            if (OriginalMaterial)
            {
                MeshComponent->SetMaterial(0, OriginalMaterial);
            }
        }
        UnhighlightAsTarget();
    }
    //UE_LOG(LogTemp, Log, TEXT("Scannable %s exited scan mode"), *GetOwner()->GetName());
}

void UDkScannableComponent::HighlightAsTarget()
{
    if (AActor* Owner = GetOwner())
    {
        Owner->SetActorScale3D(OriginalScale * HighlightScaleMultiplier);
    }
}

void UDkScannableComponent::UnhighlightAsTarget()
{
    if (AActor* Owner = GetOwner())
    {
        Owner->SetActorScale3D(OriginalScale);
    }
}