// Fill out your copyright notice in the Description page of Project Settings.


#include "StateWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UStateWidget::ChangeState(EStateMaterialType NewState, float Duration)
{
	switch (NewState)
	{
	case EStateMaterialType::ESMT_Ice:
	{
		StateSetting(FText::FromString(TEXT("빙결")), FLinearColor(0.f, 0.79f, 0.93f));
		break;
	}
	case EStateMaterialType::ESMT_Block:
	{
		StateSetting(FText::FromString(TEXT("방어")), FLinearColor(0.82f, 0.0f, 0.93f));
		break;
	}
	case EStateMaterialType::ESMT_Channeling:
	{
		StateSetting(FText::FromString(TEXT("정신 집중")), FLinearColor(1.f, 0.1f, 0.1f));
		break;
	}
	default:
		GetWorld()->GetTimerManager().ClearTimer(StateDurationTimer);
		SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);

	bChangePercent = true;
	CurDuration = Duration;
	OriginDuration = Duration;
}

void UStateWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bChangePercent)
	{
		ChangePercent(InDeltaTime);
	}
}

void UStateWidget::ChangePercent(float DeltaSeconds)
{
	if (CurDuration < 0.f)
	{
		CurDuration = 0.f;
	}

	if (DurationRemainBar)
	{
		DurationRemainBar->SetPercent(CurDuration / OriginDuration);
	}


	if (CurDuration > 0.f)
	{
		CurDuration -= DeltaSeconds;
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
		bChangePercent = false;
	}
}

void UStateWidget::StateSetting(FText StateText, FLinearColor Color)
{
	if (StateName)
	{
		StateName->SetText(StateText);
	}

	if (BackgroundImage)
	{
		FLinearColor NewColor = FLinearColor(Color.R, Color.G, Color.B, 1.f);
		BackgroundImage->SetColorAndOpacity(NewColor);
	}

	if (DurationRemainBar)
	{
		FLinearColor NewColor = FLinearColor(Color.R, Color.G, Color.B, 1.f);
		DurationRemainBar->SetFillColorAndOpacity(NewColor);
	}
}

