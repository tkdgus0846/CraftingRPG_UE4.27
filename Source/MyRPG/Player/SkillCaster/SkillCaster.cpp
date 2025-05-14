
#include "SkillCaster.h"
#include "kismet/GameplayStatics.h"
#include "MyRPG/Player/PlayerCharacter.h"

ASkillCaster::ASkillCaster()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ASkillCaster::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerCharacter = GetPlayer();
}

void ASkillCaster::BindSkillMontage(UAnimMontage* Montage)
{
	SkillMontage = Montage;
}

void ASkillCaster::BindSkillActor(TSubclassOf<class AAttackActor> AttackActor)
{
	SkillActorClass = AttackActor;
}

APlayerCharacter* ASkillCaster::GetPlayer()
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);

	return Cast<APlayerCharacter>(Player);
}

void ASkillCaster::RerserveSpawnSkillActor(float WaitTime)
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASkillCaster::SpawnSkillActor, WaitTime);
}

