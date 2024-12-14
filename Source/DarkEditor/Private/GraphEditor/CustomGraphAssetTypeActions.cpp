// Copyright @ Christian Reichel

#if WITH_EDITOR

#include "GraphEditor/CustomGraphAssetTypeActions.h"
#include "GraphEditor/CustomGraphAsset.h"
#include "GraphEditor/CustomGraphAssetEditor.h"

UClass* FCustomGraphAssetTypeActions::GetSupportedClass() const
{
	return UCustomGraphAsset::StaticClass();
}

void FCustomGraphAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UCustomGraphAsset* Graph = Cast<UCustomGraphAsset>(*ObjIt))
		{
			TSharedRef<FCustomGraphAssetEditor> NewGraphEditor(new FCustomGraphAssetEditor());
			NewGraphEditor->InitCustomGraphAssetEditor(Mode, EditWithinLevelEditor, Graph);
		}
	}
}
#endif
