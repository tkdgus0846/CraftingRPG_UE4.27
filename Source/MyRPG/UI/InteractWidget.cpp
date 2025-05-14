// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractWidget.h"
#include "Components/TextBlock.h"

void UInteractWidget::SetInteractText(FName Text)
{
	if (InteractText)
	{
		InteractText->SetText(FText::FromName(Text));
	}
}
