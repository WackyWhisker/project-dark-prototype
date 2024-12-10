// Copyright @ Christian Reichel


#include "GraphEditor/CustomGraphAssetTypeActions.h"
#include "GraphEditor/CustomGraphAsset.h"

UClass* FCustomGraphAssetTypeActions::GetSupportedClass() const
{
	return UCustomGraphAsset::StaticClass();
}