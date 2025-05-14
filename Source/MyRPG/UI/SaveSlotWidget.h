
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveSlotWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveSlotClicked, const FString&, ClickedSlotName);

UCLASS()
class MYRPG_API USaveSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(META = (BindWidget))
	class UButton* SlotButton;

	UPROPERTY(META = (BindWidget), BlueprintReadWrite)
	class UImage* GamePlayPreview;
	UPROPERTY(META = (BindWidget))
	class UTextBlock* GamePlayTimeText;
	UPROPERTY(META = (BindWidget))
	class UWidgetSwitcher* Switcher;

	void SetPlayTimeText(float GameplayTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SlotName;

	const bool IsSlotDataEmpty();

	UPROPERTY()
	FOnSaveSlotClicked OnSaveSlotClicked;
	void SelectAppearanceSlot(bool bSelect);

protected:

private:
	FString CalcTime(const float GameplayTime);
	
	UFUNCTION()
	void OnSlotButtonClicked();
};
