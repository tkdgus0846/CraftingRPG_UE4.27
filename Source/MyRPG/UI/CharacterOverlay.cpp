// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterOverlay.h"
#include <MyRPG/Enemy/Enemy.h>
#include "MyRPG/UI/LevelHpBar.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "MyRPG/UI/DragableWindow.h"

void UCharacterOverlay::ShowEnemyNameLevelHP(bool bState, AEnemy* Enemy)
{
	if ((IsValid(EnemyNameLevelHP) == false) || (IsValid(Enemy) && Enemy->GetIsUseHPBar() == false))
	{
		return;
	}
	
	EnemyNameLevelHP->UpdateCreatureCharacter(Enemy);
	
	if (bState)
		SetRenderOpacity_EnemyNameLevelHP(1.f, GetWorld()->GetDeltaSeconds());
	else
		SetRenderOpacity_EnemyNameLevelHP(0.f, 1.8f);

}

void UCharacterOverlay::AddDragableWindow(UDragableWindow* DragableWindow)
{
	if (IsValid(RootCanvas) == false || IsValid(DragableWindow) == false || RootCanvas->HasChild(DragableWindow))
	{
		return;
	}

	// 요놈을 가져와서 초기화해야한다.
	UCanvasPanelSlot* CanvasSlot = RootCanvas->AddChildToCanvas(DragableWindow);
	DragableWindow->SetWindowPanelSlot(CanvasSlot);
	CanvasSlot->SetSize(DragableWindow->GetInitSize());
	CanvasSlot->SetPosition(DragableWindow->GetInitPos());
	DragableWindow->SetVisibility(ESlateVisibility::Collapsed);
	//CanvasSlot->SetAnchors(FAnchors())
}

void UCharacterOverlay::SetRenderOpacity_EnemyNameLevelHP(float NewTargetOpacity, float FirstDelay)
{
	if (NewTargetOpacity == TargetRenderOpactiy_EnemyNameLevelHP) return;

	TargetRenderOpactiy_EnemyNameLevelHP = NewTargetOpacity;
	/*if (InterpingRenderOpactiy_EnemyNameLevelHP == TargetRenderOpactiy_EnemyNameLevelHP)
	{
		return;
	}*/	

	GetWorld()->GetTimerManager().SetTimer(InterpingRenderOpactiy_EnemyNameLevelHPTimer, this, &UCharacterOverlay::InterpRenderOpacity_EnemyNameLevelHP, FirstDelay, false);
}

void UCharacterOverlay::InterpRenderOpacity_EnemyNameLevelHP()
{
	if (IsValid(EnemyNameLevelHP) == false) return;
	if (FMath::IsNearlyEqual(InterpingRenderOpactiy_EnemyNameLevelHP, TargetRenderOpactiy_EnemyNameLevelHP, 0.1f))
	{
		InterpingRenderOpactiy_EnemyNameLevelHP = TargetRenderOpactiy_EnemyNameLevelHP;
		EnemyNameLevelHP->SetRenderOpacity(InterpingRenderOpactiy_EnemyNameLevelHP);
		return;
	}

	InterpingRenderOpactiy_EnemyNameLevelHP = FMath::FInterpTo(InterpingRenderOpactiy_EnemyNameLevelHP, TargetRenderOpactiy_EnemyNameLevelHP, GetWorld()->GetDeltaSeconds(), 20.f);
	EnemyNameLevelHP->SetRenderOpacity(InterpingRenderOpactiy_EnemyNameLevelHP);

	GetWorld()->GetTimerManager().SetTimer(InterpingRenderOpactiy_EnemyNameLevelHPTimer, this, &UCharacterOverlay::InterpRenderOpacity_EnemyNameLevelHP, GetWorld()->GetDeltaSeconds());
}
