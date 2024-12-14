// Copyright @ Christian Reichel

#if WITH_EDITOR

#include "GraphEditor/CustomGraphNode.h"
#include "GraphEditor/CustomNode.h"
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
    TopNodeBox.Reset();
    BottomNodeBox.Reset();

    this->ContentScale.Bind(this, &SGraphNode::GetContentScale);

    this->GetOrAddSlot(ENodeZone::Center)
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SNew(SBorder)
            .BorderImage(FAppStyle::GetBrush("Graph.Node.Body"))
            .BorderBackgroundColor(NodeBodyColor)
            .Padding(0.0f)
            [
                SNew(SVerticalBox)
                // Top pin
                +SVerticalBox::Slot()
                .AutoHeight()
                .HAlign(HAlign_Center)
                [
                    SAssignNew(TopNodeBox, SHorizontalBox)
                ]
                
                // Middle section with Left/Right pins and title
                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10.0f)
                [
                    SNew(SHorizontalBox)
                    
                    // Left pin
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SAssignNew(LeftNodeBox, SVerticalBox)
                    ]
                    
                    // Title area
                    +SHorizontalBox::Slot()
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                    .FillWidth(1.0f)
                    [
                        SNew(SBorder)
                        .BorderImage(FAppStyle::GetBrush("Graph.Node.TitleBackground"))
                        .BorderBackgroundColor(TitleFillColor)
                        .HAlign(HAlign_Center)
                        .VAlign(VAlign_Center)
                        .Padding(FMargin(10.0f))
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(GraphNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString()))
                            .ColorAndOpacity(TitleTextColor)
                            .TextStyle(FAppStyle::Get(), "Graph.Node.NodeTitle")
                        ]
                    ]
                    
                    // Right pin
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SAssignNew(RightNodeBox, SVerticalBox)
                    ]
                ]
                
                // Bottom pin
                +SVerticalBox::Slot()
                .AutoHeight()
                .HAlign(HAlign_Center)
                [
                    SAssignNew(BottomNodeBox, SHorizontalBox)
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
        TSharedPtr<SGraphPin> NewPin = SNew(SCustomGraphPin, Pin);
        NewPin->SetShowLabel(false);  // Hide the pin's built-in label
        AddPin(NewPin.ToSharedRef());
    }
}

void SCustomGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
    PinToAdd->SetOwner(SharedThis(this));
    const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
    
    if (PinObj)
    {
        FString PinName = PinObj->PinName.ToString();
        
        if (PinName == TEXT("Top"))
        {
            TopNodeBox->AddSlot()
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Top)
                [
                    SNew(SVerticalBox)
                    
                    // Pin first
                    +SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Top)
                    [
                        PinToAdd
                    ]
                    
                    // Label below pin
                    +SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                    .Padding(FMargin(0.0f, 4.0f, 0.0f, 0.0f))
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(PinName))
                    ]
                ];
            InputPins.Add(PinToAdd);
        }
        else if (PinName == TEXT("Bottom"))
        {
            BottomNodeBox->AddSlot()
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Bottom)
                [
                    SNew(SVerticalBox)
                    
                    // Label first
                    +SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                    .Padding(FMargin(0.0f, 0.0f, 0.0f, 4.0f))
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(PinName))
                    ]
                    
                    // Pin below label
                    +SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Bottom)
                    [
                        PinToAdd
                    ]
                ];
            InputPins.Add(PinToAdd);
        }
        else if (PinName == TEXT("Right"))
        {
            RightNodeBox->AddSlot()
                .AutoHeight()
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Center)
                [
                    SNew(SHorizontalBox)
                    
                    // Label first
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .HAlign(HAlign_Right)
                    .VAlign(VAlign_Center)
                    .Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(PinName))
                    ]
                    
                    // Pin second (at the edge)
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .HAlign(HAlign_Right)
                    .VAlign(VAlign_Center)
                    [
                        PinToAdd
                    ]
                ];
            InputPins.Add(PinToAdd);
        }
        else if (PinName == TEXT("Left"))
        {
            LeftNodeBox->AddSlot()
                .AutoHeight()
                .HAlign(HAlign_Left)
                .VAlign(VAlign_Center)
                [
                    SNew(SHorizontalBox)
                    
                    // Pin first (at the edge)
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .HAlign(HAlign_Left)
                    .VAlign(VAlign_Center)
                    [
                        PinToAdd
                    ]
                    
                    // Label second
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .HAlign(HAlign_Left)
                    .VAlign(VAlign_Center)
                    .Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(PinName))
                    ]
                ];
            InputPins.Add(PinToAdd);
        }
    }
}
#endif