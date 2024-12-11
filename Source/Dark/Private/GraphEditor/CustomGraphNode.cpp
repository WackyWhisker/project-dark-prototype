// Copyright @ Christian Reichel


// SCustomGraphNode.cpp
#include "GraphEditor/CustomGraphNode.h"
#include "GraphEditor/CustomNode.h"
#include "SGraphPin.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Styling/SlateStyleRegistry.h"

void SCustomGraphNode::Construct(const FArguments& InArgs, UCustomNode* InNode)
{
    GraphNode = InNode;
    UpdateGraphNode();
}

void SCustomGraphNode::UpdateGraphNode()
{
    InputPins.Empty();
    OutputPins.Empty();

    RightNodeBox.Reset();
    LeftNodeBox.Reset();

    this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
    this->GetOrAddSlot(ENodeZone::Center)
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Center)
        [
            SNew(SOverlay)
            +SOverlay::Slot()
            [
                SNew(SImage)
                .Image(FAppStyle::GetBrush("Brushes.Header"))
                .ColorAndOpacity(NodeBodyColor)
            ]
            +SOverlay::Slot()
            [
                SNew(SVerticalBox)
                // Title area
                +SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SBorder)
                    .BorderImage(FAppStyle::GetBrush("Graph.Node.TitleBackground"))
                    .BorderBackgroundColor(TitleFillColor)
                    .Padding(FMargin(8.0f))
                    [
                        SNew(SHorizontalBox)
                        +SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(GraphNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString()))
                            .ColorAndOpacity(TitleTextColor)
                            .TextStyle(FAppStyle::Get(), "Graph.Node.NodeTitle")
                        ]
                        +SHorizontalBox::Slot()
                        .HAlign(HAlign_Right)
                        .VAlign(VAlign_Center)
                        .AutoWidth()
                        //[
                            // Add right-aligned widgets here
                        //]
                    ]
                ]
                // Pin area
                +SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                    +SHorizontalBox::Slot()
                    .HAlign(HAlign_Left)
                    .VAlign(VAlign_Center)
                    [
                        SAssignNew(LeftNodeBox, SVerticalBox)
                    ]
                    +SHorizontalBox::Slot()
                    .HAlign(HAlign_Right)
                    .VAlign(VAlign_Center)
                    [
                        SAssignNew(RightNodeBox, SVerticalBox)
                    ]
                ]
            ]
        ];

    // Create pin widgets
    CreatePinWidgets();
}

void SCustomGraphNode::CreatePinWidgets()
{
    UCustomNode* CustomNode = CastChecked<UCustomNode>(GraphNode);
    
    for (UEdGraphPin* Pin : CustomNode->Pins)
    {
        TSharedPtr<SGraphPin> NewPin = SNew(SGraphPin, Pin);
        AddPin(NewPin.ToSharedRef());
    }
}

void SCustomGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
    PinToAdd->SetOwner(SharedThis(this));

    const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
    
    if (PinObj && PinObj->Direction == EGPD_Input)
    {
        LeftNodeBox->AddSlot()
            .AutoHeight()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Center)
            .Padding(FMargin(2.0f))
            [
                PinToAdd
            ];
        InputPins.Add(PinToAdd);
    }
    else
    {
        RightNodeBox->AddSlot()
            .AutoHeight()
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Center)
            .Padding(FMargin(2.0f))
            [
                PinToAdd
            ];
        OutputPins.Add(PinToAdd);
    }
}

