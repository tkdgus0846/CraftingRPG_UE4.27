// Fill out your copyright notice in the Description page of Project Settings.


#include "LootWidget.h"
#include "Components/TextBlock.h"

void ULootWidget::SetItemText(FName Text)
{
	if (ItemText)
	{
		ItemText->SetText(FText::FromName(Text));
	}
}
