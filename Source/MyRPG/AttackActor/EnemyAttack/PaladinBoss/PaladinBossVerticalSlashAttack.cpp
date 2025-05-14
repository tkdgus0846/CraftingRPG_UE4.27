
#include "PaladinBossVerticalSlashAttack.h"
#include <MyRPG/Interfaces/Creature.h>
#include <MyRPG/GameInstance/MyGameInstance.h>
#include "MyRPG/Effect/ImpactEffect.h"

#include "MyRPG/Command/CommandPoolManager.h"
#include "MyRPG/Command/DamageActionCommand/CameraShakeCommand.h"
#include "MyRPG/Command/DamageActionCommand/DamageOnceCommand.h"
#include "MyRPG/Command/DamageActionCommand/SpawnImpactEffectCommand.h"
#include <NiagaraFunctionLibrary.h>
#include "Kismet/GameplayStatics.h"

#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

APaladinBossVerticalSlashAttack::APaladinBossVerticalSlashAttack()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APaladinBossVerticalSlashAttack::BeginPlay()
{
	Super::BeginPlay();

	SetGenerateOverlapEventOnce(false);
	PrevPos = GetActorLocation() - (GetActorRotation().Vector());

	SpinSoundComponent = UGameplayStatics::SpawnSoundAttached(SpinSound, RootScene, FName("None"), ((FVector)(ForceInit)), FRotator::ZeroRotator);
	if (SpinSoundComponent.IsValid())
	{
		SpinSoundComponent->FadeIn(0.5f, 0.4f);
	}
}

void APaladinBossVerticalSlashAttack::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (SpinSoundComponent.IsValid())
	{
		SpinSoundComponent->FadeOut(0.5f, 0.f);
	}
}

void APaladinBossVerticalSlashAttack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (bFirstTick)
	{
		bFirstTick = false;
		return;
	}*/

	InterpRotation(DeltaTime);
}

void APaladinBossVerticalSlashAttack::OnOverlapBegin_Implementation(AActor* OtherActor)
{
	ICreature* Creature = Cast<ICreature>(OtherActor);
	if (Creature)
	{
		SettingDamageInfo(GetOwner());

		AddDamageCommand(UCameraShakeCommand, ECameraShake::ECS_AttackActor, 1.f);
		AddDamageCommand(UDamageOnceCommand, Creature, GetDamage());
		AddDamageCommand(USpawnImpactEffectCommand, ImpactEffectClass.Get(), FTransform(GetActorForwardVector().Rotation(), OtherActor->GetActorLocation()));

		Damage(Creature);
	}
}

void APaladinBossVerticalSlashAttack::OnOverlapEnd_Implementation(AActor* OtherActor)
{
}

void APaladinBossVerticalSlashAttack::OnOverlapping_Implementation(float DeltaTime)
{
}

void APaladinBossVerticalSlashAttack::InterpRotation(float DeltaTime)
{
	FVector Direction = (GetActorLocation() - PrevPos).GetSafeNormal();
	TargetRotation = Direction.Rotation();

	FRotator CurRotation = GetActorRotation();
	CurRotation = FMath::RInterpTo(CurRotation, TargetRotation, DeltaTime, 10.f);
	SetActorRotation(CurRotation);

	PrevPos = GetActorLocation();
}
