// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "AssetTypeActions_Base.h"
#include "CastleRoomsEditorFactory.generated.h"

UCLASS()
class UCastleRoomsEditorFactory : public UFactory
{
	GENERATED_BODY()

public:
	// Public Methods
	UCastleRoomsEditorFactory();
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

public:
	// Public Properties

protected:
	// Protected Methods

protected:
	// Protected Properties

private:
	// Private Methods

private:
	// Private Properties

};

class FCastleRoomsAssetTypeActions : public FAssetTypeActions_Base
{
public:
	// Public Methods
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

public:
	// Public Properties

protected:
	// Protected Methods

protected:
	// Protected Properties

private:
	// Private Methods

private:
	// Private Properties

};