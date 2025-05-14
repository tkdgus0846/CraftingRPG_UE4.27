// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MainScreenButton.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API UMainScreenButton : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(Meta = (BindWidget), BlueprintReadWrite)
	UButton* Button;

	UPROPERTY(Meta = (BindWidget), BlueprintReadWrite)
	class UImage* BackgroundImage;

	UPROPERTY(Meta = (BindWidget), BlueprintReadWrite)
	class UTextBlock* InteractText;

	FOnButtonClickedEvent& GetButtonClickedEvent();
	void SetText(const FString& ButtonString, int32 FontSize);	

private:
	enum class EButtonHoveredState
	{
		None,
		Hovered,
		UnHovered
	};

	UFUNCTION()
	void OnButtonHovered();
	UFUNCTION()
	void OnButtonUnhovered();

	UFUNCTION()
	void ButtonFadeIn(float DeltaSeconds);
	UFUNCTION()
	void ButtonFadeOut(float DeltaSeconds);

	void SetImageAndTextOpacity();

	float CurrentAlpha = 0.f;
	FTimerHandle HoverTimer;
	EButtonHoveredState CurHoveredState = EButtonHoveredState::None;
};
