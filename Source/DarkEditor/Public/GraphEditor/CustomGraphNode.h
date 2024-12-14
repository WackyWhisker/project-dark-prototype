// Copyright @ Christian Reichel
#pragma once

#if WITH_EDITOR
#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "SGraphPin.h"
#include "Widgets/SBoxPanel.h"

class UCustomNode;

// Custom Graph Pin for bidirectional visualization
class SCustomGraphPin : public SGraphPin
{
public:
    SLATE_BEGIN_ARGS(SCustomGraphPin) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
    {
        this->SGraphPin::Construct(SGraphPin::FArguments(), InPin);
    }

    // Override mouse handling to add debug info
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
    {
        if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
        {
            if (UEdGraphPin* Pin = GetPinObj())
            {
                if (UEdGraphNode* Node = Pin->GetOwningNode())
                {
                    UE_LOG(LogTemp, Warning, TEXT("Pin MouseDown - Pin: %s, Node: %s (%p)"), 
                        *Pin->PinName.ToString(),
                        *Node->GetName(),
                        Node);
                }
            }
        }
        return SGraphPin::OnMouseButtonDown(MyGeometry, MouseEvent);
    }

    virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
    {
        if (UEdGraphPin* Pin = GetPinObj())
        {
            if (UEdGraphNode* Node = Pin->GetOwningNode())
            {
                UE_LOG(LogTemp, Warning, TEXT("Pin DragDetected - Pin: %s, Node: %s (%p)"), 
                    *Pin->PinName.ToString(),
                    *Node->GetName(),
                    Node);
            }
        }
        return SGraphPin::OnDragDetected(MyGeometry, MouseEvent);
    }

    // Override to provide custom pin appearance
    virtual FSlateColor GetPinColor() const override 
    { 
        return FSlateColor(FLinearColor(0.8f, 0.8f, 0.2f)); 
    }
    
    virtual const FSlateBrush* GetPinIcon() const override
    {
        return IsConnected() ? 
            FAppStyle::GetBrush("Graph.Pin.Connected") : 
            FAppStyle::GetBrush("Graph.Pin.Disconnected");
    }
};

class DARKEDITOR_API SCustomGraphNode : public SGraphNode
{
public:
    SLATE_BEGIN_ARGS(SCustomGraphNode) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UCustomNode* InNode);

    // Override appearance functions
    virtual void UpdateGraphNode() override;
    virtual void CreatePinWidgets() override;
    virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

private:
    // Node Colors
    const FLinearColor NodeBorderColor = FLinearColor(0.5f, 0.5f, 0.5f);    // Outer frame color
    const FLinearColor NodeBodyColor = FLinearColor(0.5f, 0.5f, 0.5f);    // Main body fill color
    const FLinearColor TitleBorderColor = FLinearColor(0.5f, 0.5f, 0.5f);    // Title section border
    const FLinearColor TitleFillColor = FLinearColor(0.5f, 0.5f, 0.5f);     // Title section fill
    const FLinearColor TitleTextColor = FLinearColor::White;

    // Node boxes for pin placement
    TSharedPtr<SHorizontalBox> TopNodeBox;
    TSharedPtr<SHorizontalBox> BottomNodeBox;
    TSharedPtr<SVerticalBox> LeftNodeBox;
    TSharedPtr<SVerticalBox> RightNodeBox;
};

#endif