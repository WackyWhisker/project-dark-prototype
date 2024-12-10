// Copyright @ Christian Reichel

#include "CastleRooms/CastleRoomsGraphPin.h"

#include "CastleRooms/CastleRoomsGraphPin.h"
#include "CastleRooms/CastleRoomTypes.h"
#include "SGraphPanel.h"

void SCastleRoomsGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InPin);
}

FSlateColor SCastleRoomsGraphPin::GetPinColor() const
{
	return FLinearColor(0.7f, 0.4f, 0.0f); // Bronze/brown color for castle theme
}

const FSlateBrush* SCastleRoomsGraphPin::GetPinIcon() const
{
	return FAppStyle::GetBrush(TEXT("Graph.Pin.Connected"));
}