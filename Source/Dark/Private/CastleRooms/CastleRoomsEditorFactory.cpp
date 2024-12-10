// Copyright @ Christian Reichel

#include "CastleRooms/CastleRoomsEditorFactory.h"
#include "CastleRooms/CastleRoomsGraph.h"
#include "CastleRooms/CastleRoomsGraphEditor.h"
#include "AssetTypeCategories.h"

UCastleRoomsEditorFactory::UCastleRoomsEditorFactory()
{
	SupportedClass = UCastleRoomsGraph::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UCastleRoomsEditorFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UCastleRoomsGraph* NewGraph = NewObject<UCastleRoomsGraph>(InParent, InClass, InName, Flags);
	return NewGraph;
}

FText FCastleRoomsAssetTypeActions::GetName() const
{
	return FText::FromString("Castle Rooms Graph");
}

UClass* FCastleRoomsAssetTypeActions::GetSupportedClass() const
{
	return UCastleRoomsGraph::StaticClass();
}

FColor FCastleRoomsAssetTypeActions::GetTypeColor() const
{
	return FColor(145, 97, 69); // Brown/castle-like color
}

uint32 FCastleRoomsAssetTypeActions::GetCategories()
{
	return EAssetTypeCategories::Gameplay;
}

void FCastleRoomsAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	for (UObject* Object : InObjects)
	{
		if (UCastleRoomsGraph* Graph = Cast<UCastleRoomsGraph>(Object))
		{
			TSharedRef<FCastleRoomsGraphEditor> NewGraphEditor(new FCastleRoomsGraphEditor());
			NewGraphEditor->InitCastleRoomsEditor(EditWithinLevelEditor, Graph);
		}
	}
}