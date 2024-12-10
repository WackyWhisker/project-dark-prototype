// Copyright @ Christian Reichel


#include "CastleRooms/CastleRoomsNode.h"

#include "CastleRooms/CastleRoomTypes.h"
#include "EdGraph/EdGraphPin.h"

UCastleRoomsNode::UCastleRoomsNode()
{
	RoomName = TEXT("New Room");
}

void UCastleRoomsNode::AllocateDefaultPins()
{
	// Create input pin for incoming connections
	CreatePin(EGPD_Input, CastleRoomsTypes::PK_RoomConnection, TEXT("From"));
    
	// Create output pin for outgoing connections
	CreatePin(EGPD_Output, CastleRoomsTypes::PK_RoomConnection, TEXT("To"));
}

FText UCastleRoomsNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(RoomName);
}

void UCastleRoomsNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	// We'll add room-specific actions here later
}