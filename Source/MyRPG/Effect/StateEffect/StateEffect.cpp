
#include "StateEffect.h"
#include "MyRPG/GameInstance/MyGameInstance.h"
#include "MyRPG/BaseClasses/VFXCreatureCharacter.h"

AStateEffect::AStateEffect()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AStateEffect::Initialize(const FName& NewEffectName, float NewDuration)
{
	StateName = NewEffectName;
	StateDuration = NewDuration;
}

void AStateEffect::Play()
{
	if (IsValid(Character) == false) return;
	if (IsValid(GameInstance) == false) return;

	Stop();
	StartState();
}

void AStateEffect::Stop()
{
	GetWorld()->GetTimerManager().ClearTimer(StateMaterialAnimTimer);
	Character->EndVFX(StateName, false);
}

void AStateEffect::BeginPlay()
{
	Super::BeginPlay();
	
	Character = Cast<AVFXCreatureCharacter>(GetOwner());
	GameInstance = Cast<UMyGameInstance>(Character->GetGameInstance());

	Play();
}

void AStateEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStateEffect::EndMaterialAnim()
{
	if (IsValid(Character) == false) return;
	if (IsValid(GameInstance) == false) return;

	Character->EndVFX(StateName, false);
}

void AStateEffect::ReserveEndMaterialAnim(float ReserveDuration)
{
	GetWorld()->GetTimerManager().SetTimer(StateMaterialAnimTimer, this, &AStateEffect::EndMaterialAnim, ReserveDuration, false);
}

