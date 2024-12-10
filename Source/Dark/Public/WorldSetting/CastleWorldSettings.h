// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "Data/CastleRoomData.h"
#include "CastleWorldSettings.generated.h"


UCLASS()
class DARK_API ACastleWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	/*-- Public Methods -------------------------------------------------*/

	/*-- Public Properties -----------------------------------------------*/

	// Reference to our room configuration data
	UPROPERTY(EditDefaultsOnly, Category = "Castle Configuration")
	TObjectPtr<UCastleRoomData> RoomData;

protected:

	/*-- Protected Methods -----------------------------------------------*/

	/*-- Protected Properties --------------------------------------------*/


private:

	/*-- Private Methods -------------------------------------------------*/

	/*-- Private Properties ----------------------------------------------*/
};