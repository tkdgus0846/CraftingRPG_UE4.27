// Fill out your copyright notice in the Description page of Project Settings.


#include "HPBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include <Kismet/GameplayStatics.h>
#include "MyRPG/BaseClasses/CreatureCharacter.h"

void UHPBar::NativeConstruct()
{
	UUserWidget::NativeConstruct();
	
	Creature = Cast<ACreatureCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	Creature->OnDamaged.AddDynamic(this, &UHPBar::OnDamaged);

	InterpingHP = Creature->GetHP();
	InterpingDamagedHP = Creature->GetHP();

	ChangeHPBarAndText();
	ChangeDamageBar();
}

void UHPBar::OnDamaged_Implementation()
{
	if (Creature == nullptr || GetWorld() == nullptr) return;

	TargetHP = Creature->GetHP();

	GetWorld()->GetTimerManager().SetTimer(InterpHPTimer, this, &UHPBar::InterpHP, GetWorld()->DeltaTimeSeconds);
	GetWorld()->GetTimerManager().SetTimer(InterpDamageTimer, this, &UHPBar::InterpDamage, GetWorld()->DeltaTimeSeconds, false, 0.4f);
}

void UHPBar::InterpHP()
{
	if (GetWorld() == nullptr || 
		Creature == nullptr ||
		FMath::IsNearlyEqual(TargetHP, InterpingHP)) return;

	InterpingHP = FMath::FInterpTo(InterpingHP, TargetHP, GetWorld()->DeltaTimeSeconds, 30.f);
	ChangeHPBarAndText();
	

	GetWorld()->GetTimerManager().SetTimer(InterpHPTimer, this, &UHPBar::InterpHP, GetWorld()->DeltaTimeSeconds);
}

void UHPBar::InterpDamage()
{
	if (GetWorld() == nullptr ||
		Creature == nullptr ||
		FMath::IsNearlyEqual(TargetHP, InterpingDamagedHP, 0.1f))
	{
		return;
	}

	InterpingDamagedHP = FMath::FInterpTo(InterpingDamagedHP, TargetHP, GetWorld()->DeltaTimeSeconds, 20.f);
	ChangeDamageBar();


	GetWorld()->GetTimerManager().SetTimer(InterpDamageTimer, this, &UHPBar::InterpDamage, GetWorld()->DeltaTimeSeconds);
}

void UHPBar::ChangeDamageBar()
{
	if (DamageBar && Creature)
	{
		DamageBar->SetPercent((InterpingDamagedHP / Creature->GetMaxHP()));
	}
}

void UHPBar::ChangeHPBarAndText()
{
	if (HPBar)
	{
		HPBar->SetPercent((InterpingHP / Creature->GetMaxHP()));
	}

	if (HPText)
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), (int)InterpingHP, (int)Creature->GetMaxHP())));
	}
}
