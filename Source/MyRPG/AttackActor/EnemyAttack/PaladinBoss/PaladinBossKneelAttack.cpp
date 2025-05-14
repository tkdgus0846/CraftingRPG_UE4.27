
#include "PaladinBossKneelAttack.h"
#include "MyRPG/Effect/ImpactEffect.h"
#include <MyRPG/Interfaces/Creature.h>
#include "MyRPG/Command/CommandPoolManager.h"
#include "MyRPG/Command/DamageActionCommand/CameraShakeCommand.h"
#include "MyRPG/Command/DamageActionCommand/DamageOnceCommand.h"
#include "MyRPG/Command/DamageActionCommand/SpawnImpactEffectCommand.h"
#include <NiagaraFunctionLibrary.h>
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

void APaladinBossKneelAttack::BeginPlay()
{
	Super::BeginPlay();

	SetGenerateOverlapEventOnce(false);

	WaitVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(WaitVFX, RootScene, "None", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::KeepRelativeOffset, true);
	UAudioComponent* AudioComp = UGameplayStatics::SpawnSoundAttached(LoopSound, RootScene, "None", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::KeepRelativeOffset, true);
	if (IsValid(AudioComp))
	{
		AudioComp->FadeIn(1.f);
	}

	SetCollsionEnabled(ECollisionEnabled::Type::NoCollision);
	FTimerDelegate TimerDele;
	TimerDele.BindLambda([this]() 
		{
			if (WaitVFXComponent.IsValid())
			{
				WaitVFXComponent->DestroyComponent();
			}
			UNiagaraFunctionLibrary::SpawnSystemAttached(AttackVFX, RootScene, "None", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::KeepRelativeOffset, true);
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WaitFinishSound, GetActorLocation());

			SetCollsionEnabled(ECollisionEnabled::Type::QueryOnly);
		});
	GetWorld()->GetTimerManager().SetTimer(VFXWaitTimerHandle, TimerDele, WaitTime, false);
	
}

void APaladinBossKneelAttack::OnOverlapBegin_Implementation(AActor* OtherActor)
{
}

void APaladinBossKneelAttack::OnOverlapEnd_Implementation(AActor* OtherActor)
{
}

void APaladinBossKneelAttack::OnOverlapping_Implementation(float DeltaTime)
{
	CalcDamageTime += DeltaTime;
	if (DamageInterval > CalcDamageTime)
	{
		return;
	}

	CalcDamageTime = 0.f;
	for (int i = 0; i < OverlappingCreatures.Num(); ++i)
	{
		ICreature* Creature = Cast<ICreature>(OverlappingCreatures[i]);
		if (Creature)
		{
			SettingDamageInfo(GetOwner());

			AddDamageCommand(UCameraShakeCommand, ECameraShake::ECS_AttackActor, 1.f);
			AddDamageCommand(UDamageOnceCommand, Creature, GetDamage());
			AddDamageCommand(USpawnImpactEffectCommand, ImpactEffectClass.Get(), FTransform(GetActorForwardVector().Rotation(), OverlappingCreatures[i]->GetActorLocation()));

			Damage(Creature);
		}
	}
	
}
