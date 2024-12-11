// Copyright @ Christian Reichel


#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class FCustomGraphAssetTypeActions : public FAssetTypeActions_Base
{
public:
	// Override required functions from FAssetTypeActions_Base
    
	// The name that appears in the editor
	virtual FText GetName() const override { return FText::FromString(TEXT("Custom Graph")); }
    
	// The color of the asset in the content browser
	virtual FColor GetTypeColor() const override { return FColor(129, 196, 115); }
    
	// The class this asset type is for
	virtual UClass* GetSupportedClass() const override;
    
	// Which category this asset should appear in when right-clicking in the content browser
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Misc; }

	//Handle opening the editor
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

};