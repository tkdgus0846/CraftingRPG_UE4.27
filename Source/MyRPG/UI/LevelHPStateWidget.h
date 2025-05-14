// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelHPStateWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API ULevelHPStateWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(Meta = (BindWidget))
	class ULevelHpBar* LevelHPBar;
	UPROPERTY(Meta = (BindWidget))
	class UStateWidget* StateWidget;

	UFUNCTION()
	void UpdateCreatureCharacter(class ACreatureCharacter* NewCreature);
	UFUNCTION()
	void ChangeState(EStateMaterialType NewState, float Duration);

	void SetLevelHpBarVisibility(bool bState);
	void SetStateWidgetVisibility(bool bState);

	void SetWidgetVisibility(bool bState);

	
};
