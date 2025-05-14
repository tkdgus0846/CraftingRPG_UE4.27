// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelHpBar.generated.h"


UCLASS()
class MYRPG_API ULevelHpBar : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(Meta = (BindWidget), BlueprintReadWrite)
	class UProgressBar* HPBar;

	UPROPERTY(Meta = (BindWidget), BlueprintReadWrite)
	class UProgressBar* DamageBar;

	UPROPERTY(Meta = (BindWidget), BlueprintReadWrite)
	class UTextBlock* LevelText;

	UFUNCTION(BlueprintNativeEvent)
	void UpdateCreatureCharacter(class ACreatureCharacter* NewCreature);
	UFUNCTION()
	void UpdateCreatureCharacter_Implementation(class ACreatureCharacter* NewCreature);

protected:
	UFUNCTION()
	void OnDamaged();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDamagedHPZero();

private:
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class ACreatureCharacter* Creature;

	bool IsUpdateEqualCreature(class ACreatureCharacter* NewCreature);

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool bUpdateFailed = false;
	
	float TargetHP;
	float InterpingHP;
	float InterpingDamagedHP;

	void InterpHP();
	FTimerHandle InterpHPTimer;
	void InterpDamage();
	FTimerHandle InterpDamageTimer;

	void ChangeHPBar();
	void ChangeDamageBar();
};
