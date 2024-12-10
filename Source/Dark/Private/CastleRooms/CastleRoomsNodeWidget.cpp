// Copyright @ Christian Reichel

#include "CastleRooms/CastleRoomsNodeWidget.h"
#include "CastleRooms/CastleRoomsNode.h"
#include "SLevelOfDetailBranchNode.h"
#include "Widgets/SBoxPanel.h"
#include "SGraphPin.h"
#include "CastleRooms/CastleRoomsGraphPin.h"

void SCastleRoomsNodeWidget::Construct(const FArguments& InArgs, UCastleRoomsNode* InNode)
{
	GraphNode = InNode;
	SetCursor(EMouseCursor::CardinalCross);
	NodeTitle = SNew(SNodeTitle, GraphNode);

	// Create the input/output pin areas
	LeftNodeBox = SNew(SVerticalBox);
	RightNodeBox = SNew(SVerticalBox);

	// Main content area
	ContentScale.Bind(this, &SGraphNode::GetContentScale);
	GetOrAddSlot(ENodeZone::Center)
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Center)
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("Graph.Node.Body"))
		.Padding(5.0f)
		[
			SNew(SVerticalBox)
			// Title area
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(NodeTitle, SNodeTitle, GraphNode)
			]
			// Pin areas
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				[
					LeftNodeBox.ToSharedRef()
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				[
					RightNodeBox.ToSharedRef()
				]
			]
		]
	];

	CreatePinWidgets();
}

TSharedRef<SWidget> SCastleRoomsNodeWidget::CreateNodeContentArea()
{
	UCastleRoomsNode* CastleRoomsNode = Cast<UCastleRoomsNode>(GraphNode);

	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("Graph.Node.Body"))
		.Padding(5)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				CreateTitleWidget(NodeTitle)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				// Room description area
				SNew(STextBlock)
				.Text(CastleRoomsNode ? FText::FromString(CastleRoomsNode->RoomDescription.ToString()) : FText::GetEmpty())
				.WrapTextAt(200.0f)
			]
		];
}

TSharedRef<SWidget> SCastleRoomsNodeWidget::CreateTitleWidget(TSharedPtr<SNodeTitle> InNodeTitle)
{
	return SNew(STextBlock)
		.Text(GraphNode->GetNodeTitle(ENodeTitleType::FullTitle))
		.TextStyle(FAppStyle::Get(), "Graph.Node.NodeTitle")
		.WrapTextAt(200.0f);
}

void SCastleRoomsNodeWidget::CreatePinWidgets()
{
	for (UEdGraphPin* Pin : GraphNode->Pins)
	{
		TSharedPtr<SGraphPin> NewPin = SNew(SCastleRoomsGraphPin, Pin);
        
		if (Pin->Direction == EGPD_Input)
		{
			LeftNodeBox->AddSlot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.Padding(20.0f, 0.0f)
				[
					NewPin.ToSharedRef()
				];
		}
		else // EGPD_Output
		{
			RightNodeBox->AddSlot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(20.0f, 0.0f)
				[
					NewPin.ToSharedRef()
				];
		}
	}
}

void SCastleRoomsNodeWidget::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));
	LeftNodeBox->AddSlot()
	.HAlign(HAlign_Left)
	.VAlign(VAlign_Center)
	.Padding(20.0f, 0.0f)
	[
		PinToAdd
	];
}