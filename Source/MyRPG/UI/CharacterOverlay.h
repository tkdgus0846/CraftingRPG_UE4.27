// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(Meta = (BindWidget))
	class ULevelHpBar* EnemyNameLevelHP;

	UPROPERTY(Meta = (BindWidget))
	class UCanvasPanel* RootCanvas;

	void ShowEnemyNameLevelHP(bool bState, class AEnemy* Enemy);

	void AddDragableWindow(class UDragableWindow* DragableWindow);

protected:


private:
	float InterpingRenderOpactiy_EnemyNameLevelHP = 0.f;
	float TargetRenderOpactiy_EnemyNameLevelHP = 0.f;
	FTimerHandle InterpingRenderOpactiy_EnemyNameLevelHPTimer;

	void SetRenderOpacity_EnemyNameLevelHP(float NewTargetOpacity, float FirstDelay);
	void InterpRenderOpacity_EnemyNameLevelHP();
	
};
