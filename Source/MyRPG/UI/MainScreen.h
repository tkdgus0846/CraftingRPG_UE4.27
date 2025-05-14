
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainScreen.generated.h"

UCLASS()
class MYRPG_API UMainScreen : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(Meta = (BindWidget), EditDefaultsOnly, BlueprintReadWrite)
	class UMainScreenButton* Play;

	UPROPERTY(Meta = (BindWidget), EditDefaultsOnly, BlueprintReadWrite)
	class UMainScreenButton* Load;

	UPROPERTY(Meta = (BindWidget), EditDefaultsOnly, BlueprintReadWrite)
	class UMainScreenButton* Exit;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* NewOrLoadText;

	UPROPERTY(Meta = (BindWidget), EditDefaultsOnly, BlueprintReadWrite)
	class UMainScreenButton* OK;

	UPROPERTY(Meta = (BindWidget), EditDefaultsOnly, BlueprintReadWrite)
	class UMainScreenButton* Cancel;

	UPROPERTY(Meta = (BindWidget))
	class USizeBox* SelectSaveSlot;

	UPROPERTY(META = (BindWidget))
	class USaveSlotWidget* SaveSlotWidget1;
	UPROPERTY(META = (BindWidget))
	class USaveSlotWidget* SaveSlotWidget2;
	UPROPERTY(META = (BindWidget))
	class USaveSlotWidget* SaveSlotWidget3;

protected:

private:
	UFUNCTION()
	void PlayGame();

	UFUNCTION()
	void LoadGame();

	UFUNCTION()
	void ExitGame();

	UFUNCTION()
	void OnOKButtonClicked();
	UFUNCTION()
	void OnCancelButtonClicked();

	UPROPERTY(Transient, BlueprintReadWrite, Category = "Animations", Meta = (AllowPrivateAccess = true, BindWidgetAnim))
	class UWidgetAnimation* SelectSaveSlotFadeIn;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "Animations", Meta = (AllowPrivateAccess = true, BindWidgetAnim))
	class UWidgetAnimation* SelectSaveSlotFadeOut;

	void FadeIn();
	void FadeOut();
	UFUNCTION()
	void FadeInAnimationFinished();
	UFUNCTION()
	void FadeOutAnimationFinished();
	FWidgetAnimationDynamicEvent FadeInFinishedEvent;
	FWidgetAnimationDynamicEvent FadeOutFinishedEvent;

	UPROPERTY()
	TArray<TWeakObjectPtr<class USaveSlotWidget>> SaveSlotWidgetArray;
	void SetVisibilityNotHitableToEmptySlots();
	void SetVisibilityVisibleToSlots();

	UFUNCTION()
	void ClickedSaveSlot(const FString& ClickedSlotName);
	void ClearSelectSlot();
};
