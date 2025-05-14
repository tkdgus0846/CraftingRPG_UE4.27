
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EscapeWidget.generated.h"

class UMainScreenButton;

UCLASS()
class MYRPG_API UEscapeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void FadeIn();
	UFUNCTION()
	void FadeOut();


	UPROPERTY(META = (BindWidget))
	UMainScreenButton* Resume;

	UPROPERTY(META = (BindWidget))
	UMainScreenButton* Save;

	UPROPERTY(META = (BindWidget))
	UMainScreenButton* Load;

	UPROPERTY(META = (BindWidget))
	UMainScreenButton* Quit;

	UPROPERTY(META = (BindWidget))
	UMainScreenButton* SaveAndQuit;

private:
	UPROPERTY()
	class AMyRPGGameModeBase* Gamemode = nullptr;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "Animations", Meta = (AllowPrivateAccess = true, BindWidgetAnim))
	class UWidgetAnimation* FadeInAnimation;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Animations", Meta = (AllowPrivateAccess = true, BindWidgetAnim))
	class UWidgetAnimation* FadeOutAnimation;

	UFUNCTION()
	void FadeOutAnimationFinished();

	FWidgetAnimationDynamicEvent FadeOutFinishedEvent;

	UFUNCTION()
	void OnSaveButtonClicked();
	UFUNCTION()
	void OnLoadButtonClicked();
	UFUNCTION()
	void OnQuitButtonClicked();
	UFUNCTION()
	void OnSaveAndQuitButtonClicked();
};
