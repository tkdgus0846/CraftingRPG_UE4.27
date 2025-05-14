
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPBar.generated.h"

UCLASS()
class MYRPG_API UHPBar : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(Meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(Meta = (BindWidget), BlueprintReadWrite)
	class UProgressBar* DamageBar;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* HPText;

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnDamaged();

	UFUNCTION()
	void OnDamaged_Implementation();

private:
	UPROPERTY()
	class ACreatureCharacter* Creature;

	float TargetHP;
	float InterpingHP;
	float InterpingDamagedHP;

	void InterpHP();
	FTimerHandle InterpHPTimer;
	void InterpDamage();
	FTimerHandle InterpDamageTimer;

	void ChangeDamageBar();
	void ChangeHPBarAndText();

};
