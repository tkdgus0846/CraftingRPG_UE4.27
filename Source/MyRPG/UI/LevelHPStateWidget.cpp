// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelHPStateWidget.h"
#include "MyRPG/UI/StateWidget.h"
#include "MyRPG/UI/LevelHpBar.h"

void ULevelHPStateWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (StateWidget)
	{
		StateWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ULevelHPStateWidget::UpdateCreatureCharacter(ACreatureCharacter* NewCreature)
{
	if (LevelHPBar)
	{
		LevelHPBar->UpdateCreatureCharacter(NewCreature);
	}
}

void ULevelHPStateWidget::ChangeState(EStateMaterialType NewState, float Duration)
{
	if (StateWidget)
	{
		StateWidget->ChangeState(NewState, Duration);
	}
}

void ULevelHPStateWidget::SetLevelHpBarVisibility(bool bState)
{
	if (LevelHPBar)
	{
		LevelHPBar->SetVisibility((bState) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
}

void ULevelHPStateWidget::SetStateWidgetVisibility(bool bState)
{
	if (StateWidget)
	{
		StateWidget->SetVisibility((bState) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
}

void ULevelHPStateWidget::SetWidgetVisibility(bool bState)
{
	SetLevelHpBarVisibility(bState);
	SetStateWidgetVisibility(bState);
}
