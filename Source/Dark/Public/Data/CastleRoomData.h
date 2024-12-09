// CastleRoomData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CastleRoomData.generated.h"

UCLASS(BlueprintType)
class DARK_API UCastleRoomData : public UDataAsset
{
	GENERATED_BODY()

public:
	// List of available room level names
	UPROPERTY(EditDefaultsOnly, Category = "Castle Rooms")
	TArray<FName> AvailableRooms;

	// Later we can add things like:
	// - Room requirements
	// - Room difficulty
	// - Room type/category
	// - Connection points
};