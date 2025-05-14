// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LootWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API ULootWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* ItemText;

	UFUNCTION(BlueprintCallable)
	void SetItemText(FName Text);

	
};
