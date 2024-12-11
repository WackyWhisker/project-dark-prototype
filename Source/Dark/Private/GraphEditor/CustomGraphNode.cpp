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
    LeftNodeBox.Reset();
    RightNodeBox.Reset();

    this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
    this->GetOrAddSlot(ENodeZone::Center)
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Center)
        [
            SNew(SBorder)
            .BorderImage(FAppStyle::GetBrush("Graph.Node.Body"))
            .Padding(FMargin(4.0f))
            .BorderBackgroundColor(NodeBorderColor)     // Border/frame color
            .ColorAndOpacity(NodeBodyColor)            // Body fill color
            [
                SNew(SVerticalBox)
                // Title section with its own background
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(FMargin(2.0f))
                [
                    SNew(SBorder)
                    .BorderImage(FAppStyle::GetBrush("Graph.Node.TitleBackground"))
                    .BorderBackgroundColor(TitleBorderColor)  // Title border color
                    .ColorAndOpacity(TitleFillColor)         // Title fill color
                    .Padding(FMargin(8.0f))
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(GraphNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString()))
                        .ColorAndOpacity(TitleTextColor)
                        .TextStyle(FAppStyle::Get(), "Graph.Node.NodeTitle")
                    ]
                ]
                // Pins section
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(FMargin(0.0f, 4.0f))
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .HAlign(HAlign_Left)
                    [
                        SAssignNew(LeftNodeBox, SVerticalBox)
                    ]
                    + SHorizontalBox::Slot()
                    .HAlign(HAlign_Right)
                    [
                        SAssignNew(RightNodeBox, SVerticalBox)
                    ]
                ]
            ]
        ];

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

