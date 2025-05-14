// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyRPG/Item/NonEquipment/NonEquipment.h"
#include "Consumable.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API AConsumable : public ANonEquipment
{
	GENERATED_BODY()

public:
	AConsumable();
	virtual void BringDataTableDatas() override;

	UFUNCTION(BlueprintNativeEvent)
	void Consume();
	UFUNCTION()
	void Consume_Implementation(); 

private:
	UPROPERTY(VisibleInstanceOnly, Meta = (AllowPrivateAccess = true))
	class USoundCue* ConsumeSound;
	
};
