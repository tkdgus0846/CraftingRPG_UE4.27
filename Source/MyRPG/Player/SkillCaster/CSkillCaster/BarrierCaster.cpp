// Fill out your copyright notice in the Description page of Project Settings.


#include "BarrierCaster.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "MyRPG/Player/Components/CombatComponent.h"
#include <NiagaraSystem.h>
#include <NiagaraFunctionLibrary.h>
#include "Components/BoxComponent.h"
#include "MyRPG/Types/ChannelType.h"
#include "MyRPG/AttackActor/AttackActor.h"

ABarrierCaster::ABarrierCaster()
{
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemFinder(TEXT("/Game/Blueprints/FX/PlayerFX/RCSkills/ChaosBarrier/NS_ChaosBarrierHit.NS_ChaosBarrierHit"));

	if (NiagaraSystemFinder.Succeeded())
	{
		BarrierHitFX = NiagaraSystemFinder.Object;
	}

	BarrierHitComponent = CreateDefaultSubobject<UBoxComponent>("BarrierHitBox");
	if (IsValid(BarrierHitComponent) == false)
	{
		return;
	}

	RootComponent = BarrierHitComponent;

	BarrierHitComponent->SetBoxExtent(FVector(80.f, 100.f, 90.f));
	BarrierHitComponent->SetCollisionProfileName("Creature");
	BarrierHitComponent->SetCollisionResponseToChannel(ECC_Creature, ECollisionResponse::ECR_Ignore);
	BarrierHitComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	BarrierHitComponent->Rename(TEXT("Shield"));
	BarrierHitComponent->PrimaryComponentTick.bHighPriority = true;
}

void ABarrierCaster::SkillPressed()
{
	if (IsValid(PlayerCharacter) == false || IsValid(SkillMontage) == false) return;
}

void ABarrierCaster::SkillPressing(float DeltaTime)
{
	if (IsValid(PlayerCharacter) == false || IsValid(SkillMontage) == false) return;
}

void ABarrierCaster::SkillReleased()
{
	if (IsValid(PlayerCharacter) == false || IsValid(SkillMontage) == false) return;

	PlayerCharacter->PlayAnimMontage(SkillMontage);
	PlayerCharacter->GetCombatComponent()->OnCSkill.Broadcast();

	GetWorldTimerManager().SetTimer(StartChangeStateTimer, this, &ABarrierCaster::StartChangeState, 0.292f, false);
}

void ABarrierCaster::ShowDamagedEffect()
{
	UNiagaraFunctionLibrary::SpawnSystemAttached(BarrierHitFX, PlayerCharacter->GetMesh(), NAME_None, FVector(0.f, 123.952553f, 95.5625), FRotator(0.f, 90.f, 0.f), EAttachLocation::Type::KeepRelativeOffset, true);
}

void ABarrierCaster::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(BarrierHitComponent) && IsValid(PlayerCharacter))
	{
		FAttachmentTransformRules AttachRule = FAttachmentTransformRules::KeepRelativeTransform;
		AttachToActor(PlayerCharacter, AttachRule);
		SetActorRelativeLocation(FVector(40.f, 0.f, 15.f));

		BarrierHitComponent->OnComponentBeginOverlap.AddDynamic(this, &ABarrierCaster::CollisionOverlapBegin);
		BarrierHitComponent->OnComponentEndOverlap.AddDynamic(this, &ABarrierCaster::CollisionOverlapEnd);

	}
}

void ABarrierCaster::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ABarrierCaster::StartChangeState()
{
	if (IsValid(PlayerCharacter) == false) return;

	SetActorHiddenInGame(false);

	PlayerCharacter->SetMaxWalkSpeed(320.f);
	PlayerCharacter->RestrictChangeMaxWalkSpeed(true);

	PlayerCharacter->ChangeState(EStateMaterialType::ESMT_Block, 5.f);
	//PlayerCharacter->SetIgnoreDamage(true);

	GetWorldTimerManager().SetTimer(BarrierTimer, this, &ABarrierCaster::EndState, 5.f, false);

	if (IsValid(BarrierHitComponent))
	{
		// Barrier Hit Component¿¡ ºÎµúÇû´ÂÁö¸¦ ¸ÕÀú È®ÀÎÇÏ°í ºÎµúÇû´Ù¸é, 
		BarrierHitComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	}
}

void ABarrierCaster::EndState()
{
	if (IsValid(PlayerCharacter) == false || IsValid(SkillMontage) == false) return;

	SetActorHiddenInGame(true);

	PlayerCharacter->RestrictChangeMaxWalkSpeed(false);
	PlayerCharacter->WalkSpeedToOrigin();

	PlayerCharacter->SetWeaponVisiblity(true);
	PlayerCharacter->StopAnimMontage(SkillMontage);
	//PlayerCharacter->SetIgnoreDamage(false);

	if (IsValid(BarrierHitComponent))
	{
		BarrierHitComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}

// ¿©±â¼­ ForwardVecto
void ABarrierCaster::CollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(PlayerCharacter) == false || Cast<AAttackActor>(OtherActor) == nullptr) return;
	
	//UE_LOG(LogTemp, Warning, TEXT("CollisionBeginOverlap"));
	
	ShowDamagedEffect();

	//PlayerCharacter->SetAllIgnoreCommandState(true);

	/*FTimerDelegate TimerDele;
	TimerDele.BindUFunction(PlayerCharacter, "SetAllIgnoreCommandState", false);
	GetWorldTimerManager().SetTimerForNextTick(TimerDele);*/

	/*FVector ForwardVector = PlayerCharacter->GetActorForwardVector();

	FVector Direction = OtherComp->GetComponentLocation() - OverlappedComp->GetComponentLocation();
	Direction.Normalize();

	float DotValue = FVector::DotProduct(Direction, ForwardVector);*/
}

void ABarrierCaster::CollisionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValid(PlayerCharacter) == false) return;
	//UE_LOG(LogTemp, Warning, TEXT("CollisionEndOverlap"));
	//PlayerCharacter->SetAllIgnoreCommandState(false);
}
