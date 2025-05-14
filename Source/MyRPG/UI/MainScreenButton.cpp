// Fill out your copyright notice in the Description page of Project Settings.


#include "MainScreenButton.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void UMainScreenButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IsValid(Button))
	{
		Button->OnHovered.AddDynamic(this, &UMainScreenButton::OnButtonHovered);
		Button->OnUnhovered.AddDynamic(this, &UMainScreenButton::OnButtonUnhovered);
	}
}

void UMainScreenButton::NativeConstruct()
{
	Super::NativeConstruct();

	
}

void UMainScreenButton::NativeDestruct()
{
	Super::NativeDestruct();
}

void UMainScreenButton::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (CurHoveredState == EButtonHoveredState::None)
	{
		return;
	}

	if (CurHoveredState == EButtonHoveredState::Hovered)
	{
		ButtonFadeIn(InDeltaTime);
	}
	else if (CurHoveredState == EButtonHoveredState::UnHovered)
	{
		ButtonFadeOut(InDeltaTime);
	}
	
}

FOnButtonClickedEvent& UMainScreenButton::GetButtonClickedEvent()
{
	return Button->OnClicked;
}

void UMainScreenButton::SetText(const FString& ButtonString, int32 FontSize)
{
	if (IsValid(InteractText) == false)
	{
		return;
	}
	InteractText->SetText(FText::FromString(ButtonString));

	FSlateFontInfo FontInfo = InteractText->Font;
	FontInfo.Size = FontSize;
	InteractText->SetFont(FontInfo);
}

void UMainScreenButton::OnButtonHovered()
{
	CurHoveredState = EButtonHoveredState::Hovered;
}

void UMainScreenButton::OnButtonUnhovered()
{
	CurHoveredState = EButtonHoveredState::UnHovered;
}

void UMainScreenButton::ButtonFadeIn(float DeltaSeconds)
{
	CurrentAlpha += DeltaSeconds * 4.f;
	if (CurrentAlpha >= 1.f)
	{
		CurrentAlpha = 1.f;
		CurHoveredState = EButtonHoveredState::None;
	}
	SetImageAndTextOpacity();
}

void UMainScreenButton::ButtonFadeOut(float DeltaSeconds)
{
	CurrentAlpha -= DeltaSeconds * 4.f;
	if (CurrentAlpha <= 0.f)
	{
		CurrentAlpha = 0.f;
		CurHoveredState = EButtonHoveredState::None;
	}
	SetImageAndTextOpacity();
}

void UMainScreenButton::SetImageAndTextOpacity()
{
	if (IsValid(BackgroundImage) == false || IsValid(InteractText) == false)
	{
		return;
	}

	FLinearColor Brush = BackgroundImage->ColorAndOpacity;
	Brush.A = CurrentAlpha;
	BackgroundImage->SetColorAndOpacity(Brush);

	FSlateFontInfo FontInfo = InteractText->Font;
	FLinearColor FontColor = FontInfo.OutlineSettings.OutlineColor;
	FontColor.A = CurrentAlpha;
	FontInfo.OutlineSettings.OutlineColor = FontColor;
	InteractText->SetFont(FontInfo);
}
