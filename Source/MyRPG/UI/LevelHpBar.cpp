
#include "LevelHpBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include <Kismet/GameplayStatics.h>
#include "MyRPG/BaseClasses/CreatureCharacter.h"

void ULevelHpBar::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULevelHpBar::NativeDestruct()
{
	Super::NativeDestruct();

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	/*if (IsValid(Creature))
	{
		Creature->OnDamaged.RemoveDynamic(this, &ULevelHpBar::OnDamaged);
	}*/
}

void ULevelHpBar::OnDamaged()
{
	if (Creature == nullptr || GetWorld() == nullptr) return;

	TargetHP = Creature->GetHP();
	GetWorld()->GetTimerManager().SetTimer(InterpHPTimer, this, &ULevelHpBar::InterpHP, GetWorld()->DeltaTimeSeconds);
	GetWorld()->GetTimerManager().SetTimer(InterpDamageTimer, this, &ULevelHpBar::InterpDamage, GetWorld()->DeltaTimeSeconds, false, 0.4f);
}

bool ULevelHpBar::IsUpdateEqualCreature(ACreatureCharacter* NewCreature)
{
	return Creature == NewCreature;
}

void ULevelHpBar::InterpHP()
{
	if (GetWorld() == nullptr ||
		Creature == nullptr ||
		FMath::IsNearlyEqual(TargetHP, InterpingHP, 0.1f)) return;

	InterpingHP = FMath::FInterpTo(InterpingHP, TargetHP, GetWorld()->DeltaTimeSeconds, 30.f);
	ChangeHPBar();


	GetWorld()->GetTimerManager().SetTimer(InterpHPTimer, this, &ULevelHpBar::InterpHP, GetWorld()->DeltaTimeSeconds);
}

void ULevelHpBar::InterpDamage()
{
	if (GetWorld() == nullptr ||
		Creature == nullptr ||
		FMath::IsNearlyEqual(TargetHP, InterpingDamagedHP, 0.1f))
	{
		if (FMath::IsNearlyEqual(InterpingDamagedHP, 0.f, 0.1f))
			OnDamagedHPZero();
		return;
	}

	InterpingDamagedHP = FMath::FInterpTo(InterpingDamagedHP, TargetHP, GetWorld()->DeltaTimeSeconds, 20.f);
	ChangeDamageBar();


	GetWorld()->GetTimerManager().SetTimer(InterpDamageTimer, this, &ULevelHpBar::InterpDamage, GetWorld()->DeltaTimeSeconds);
}

void ULevelHpBar::ChangeHPBar()
{
	if (HPBar && Creature)
	{
		HPBar->SetPercent((InterpingHP / Creature->GetMaxHP()));
	}
}

void ULevelHpBar::ChangeDamageBar()
{
	if (DamageBar && Creature)
	{
		DamageBar->SetPercent((InterpingDamagedHP / Creature->GetMaxHP()));
	}
}

void ULevelHpBar::UpdateCreatureCharacter_Implementation(ACreatureCharacter* NewCreature)
{
	if (IsValid(NewCreature) == false || IsUpdateEqualCreature(NewCreature))
	{
		bUpdateFailed = true;
		return;
	}

	bUpdateFailed = false;

	if (IsValid(Creature))
	{
		Creature->OnDamaged.RemoveDynamic(this, &ULevelHpBar::OnDamaged);
	}
	
	Creature = NewCreature;

	if (IsValid(Creature))
	{
		Creature->OnDamaged.AddDynamic(this, &ULevelHpBar::OnDamaged);
	}
	

	InterpingHP = Creature->GetHP();
	InterpingDamagedHP = Creature->GetHP();

	ChangeHPBar();
	ChangeDamageBar();

	LevelText->SetText(FText::FromString(FString::FromInt(Creature->GetLevel())));
}