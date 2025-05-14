
#include "AttackActor.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "MyRPG/Interfaces/Creature.h"
#include <MyRPG/Enemy/Enemy.h>
#include <MyRPG/Player/PlayerCharacter.h>
#include <MyRPG/BaseClasses/CreatureActor.h>

AAttackActor::AAttackActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>("RootScene");

	if (RootScene)
	{
		SetRootComponent(RootScene);
	}
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");

	if (CollisionBox)
	{
		if (RootScene)
		{
			CollisionBox->SetupAttachment(RootScene);
			CollisionBox->SetCollisionProfileName("CreatureDetection");
		}
	}

	CollisionSphere = CreateDefaultSubobject<USphereComponent>("CollisionSphere");

	if (CollisionSphere)
	{
		if (RootScene)
		{
			CollisionSphere->SetupAttachment(RootScene);
			CollisionSphere->SetCollisionProfileName("CreatureDetection");
		}
	}
	
}

void AAttackActor::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && bAttachToOwner == true)
	{
		FAttachmentTransformRules Rule(EAttachmentRule::KeepRelative, false);

		if (bRotateInherit)
			Rule.RotationRule = EAttachmentRule::KeepRelative;
		else
			Rule.RotationRule = EAttachmentRule::KeepWorld;

		if (bLocationInherit)
			Rule.LocationRule = EAttachmentRule::KeepRelative;
		else
			Rule.LocationRule = EAttachmentRule::KeepWorld;

		if (AttachSocketName.IsNone() == false)
		{
			Rule.RotationRule = EAttachmentRule::SnapToTarget;
			Rule.ScaleRule = EAttachmentRule::KeepWorld;

			ACharacter* Character = Cast<ACharacter>(GetOwner());
			if (IsValid(Character))
				AttachToComponent(Character->GetMesh(), Rule, AttachSocketName);
		}
		else
		{
			AttachToActor(GetOwner(), Rule);
		}
		
	}

	UpdateOverlapOneTick();
}

void AAttackActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (IsValid(CollisionBox))
	{
		CollisionBox->OnComponentBeginOverlap.RemoveDynamic(this, &AAttackActor::CollisionOverlapBegin);
		CollisionBox->OnComponentEndOverlap.RemoveDynamic(this, &AAttackActor::CollisionOverlapEnd);
	}

	if (IsValid(CollisionSphere))
	{
		CollisionSphere->OnComponentBeginOverlap.RemoveDynamic(this, &AAttackActor::CollisionOverlapBegin);
		CollisionSphere->OnComponentEndOverlap.RemoveDynamic(this, &AAttackActor::CollisionOverlapEnd);
	}
}

void AAttackActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (IsValid(CollisionBox) == false || IsValid(CollisionSphere) == false) return;

	CollisionBox->SetCollisionProfileName("CreatureDetection");
	CollisionSphere->SetCollisionProfileName("CreatureDetection");

	switch (AttackCollisionMode)
	{
	case EAttackCollisionMode::ACM_BOX:
	{
		SetCollsionEnabled(CollisionBox, ECollisionEnabled::QueryOnly);
		SetCollsionEnabled(CollisionSphere, ECollisionEnabled::NoCollision);
		break;
	}
	case EAttackCollisionMode::ACM_SPHERE:
	{
		SetCollsionEnabled(CollisionSphere, ECollisionEnabled::QueryOnly);
		SetCollsionEnabled(CollisionBox, ECollisionEnabled::NoCollision);
		break;
	}

	case EAttackCollisionMode::ACM_BOTH:
	{
		SetCollsionEnabled(ECollisionEnabled::QueryOnly);
		break;
	}
	}
		
}

void AAttackActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RemoveDeadCreatures();

	if (OverlappingCreatures.Num() > 0)
	{
		OnOverlapping(DeltaTime);
	}
}

void AAttackActor::SetCollsionEnabled(ECollisionEnabled::Type ColType)
{
	TArray<UShapeComponent*> Array;

	switch (AttackCollisionMode)
	{
	case EAttackCollisionMode::ACM_BOX:
		Array.Add(CollisionBox);
		break;
	case EAttackCollisionMode::ACM_SPHERE:
		Array.Add(CollisionSphere);
		break;
	case EAttackCollisionMode::ACM_BOTH:
		Array.Add(CollisionBox);
		Array.Add(CollisionSphere);
		break;
	}

	for (UShapeComponent* Comp : Array)
	{
		if (ColType == ECollisionEnabled::NoCollision)
			Comp->SetVisibility(false);
		else
			Comp->SetVisibility(true);

		Comp->SetCollisionEnabled(ColType);
	}
}

void AAttackActor::SetCollsionEnabled(UShapeComponent* Comp, ECollisionEnabled::Type ColType)
{
	if (ColType == ECollisionEnabled::NoCollision)
		Comp->SetVisibility(false);
	else
		Comp->SetVisibility(true);

	Comp->SetCollisionEnabled(ColType);
}

void AAttackActor::EmptyOverlapFinishedCreatures()
{
	OverlapFinishedCreatures.Empty();
}

void AAttackActor::SetBoxSize(const FVector& NewSize)
{
	if (CollisionBox)
	{
		CollisionBox->SetBoxExtent(NewSize);
	}
}

FVector AAttackActor::GetBoxSize()
{
	if (CollisionBox)
	{
		return CollisionBox->GetScaledBoxExtent();
	}
	return FVector();
}

void AAttackActor::SetSphereRadius(const float NewRadius)
{
	if (IsValid(CollisionSphere))
	{
		CollisionSphere->SetSphereRadius(NewRadius);
	}
}

float AAttackActor::GetSphereRadius()
{
	if (IsValid(CollisionSphere))
	{
		return CollisionSphere->GetScaledSphereRadius();
	}
	return -1.f;
}

void AAttackActor::SetLocationToOriginal(UShapeComponent* Comp, const FVector& OriginalLocation)
{
	if (IsValid(Comp) == false) return;
	Comp->SetRelativeLocation(OriginalLocation);
}

void AAttackActor::UpdateOverlapOneTick()
{
	if (IsValid(CollisionBox) && (AttackCollisionMode == EAttackCollisionMode::ACM_BOX || AttackCollisionMode == EAttackCollisionMode::ACM_BOTH))
	{
		CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AAttackActor::CollisionOverlapBegin);
		CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AAttackActor::CollisionOverlapEnd);


		FVector OriginalLocation = CollisionBox->GetRelativeLocation();
		CollisionBox->SetWorldLocation({ 0.f,0.f,-100000.f });

		FTimerHandle TimerHandle;
		FTimerDelegate TimerDele;
		TimerDele.BindUFunction(this, "SetLocationToOriginal", CollisionBox, OriginalLocation);

		float Rate = GetWorld()->GetDeltaSeconds();
		Rate = (Rate == 0.f) ? 0.00833f : Rate;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDele, Rate, false);
		/*GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAttackActor::SetLocationToOriginal, 0.0001f, false, GetWorld()->GetDeltaSeconds());*/
	}

	if (IsValid(CollisionSphere) && (AttackCollisionMode == EAttackCollisionMode::ACM_SPHERE || AttackCollisionMode == EAttackCollisionMode::ACM_BOTH))
	{
		CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAttackActor::CollisionOverlapBegin);
		CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AAttackActor::CollisionOverlapEnd);


		FVector OriginalLocation = CollisionSphere->GetRelativeLocation();
		CollisionSphere->SetWorldLocation({ 0.f,0.f,-100000.f });

		FTimerHandle TimerHandle;
		FTimerDelegate TimerDele;
		TimerDele.BindUFunction(this, "SetLocationToOriginal", CollisionSphere, OriginalLocation);

		float Rate = GetWorld()->GetDeltaSeconds();
		Rate = (Rate == 0.f) ? 0.00833f : Rate;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDele, Rate, false);
		/*GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAttackActor::SetLocationToOriginal, 0.0001f, false, GetWorld()->GetDeltaSeconds());*/
	}
}

void AAttackActor::CollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AEnemy>(GetOwner()) && Cast<AEnemy>(OtherActor)) return;
	if (Cast<APlayerCharacter>(GetOwner()) && Cast<APlayerCharacter>(OtherActor)) return;

	ICreature* Creature = Cast<ICreature>(OtherActor);
	if (Creature && Creature->GetDead() == false)
	{
		static int CallNum = 1;
		UE_LOG(LogTemp, Warning, TEXT("%d: Collision: %s"), CallNum++, *OtherComp->GetFName().ToString());
		// 오버랩 이벤트를 한번만 발생시키는 AttackActor이며, 이미 한번이라도 겹쳤던 크리쳐가 감지되면 그냥 이벤트를 Return 해버린다.
		if (bGenerateOverlapEventOnce && OverlapFinishedCreatures.Find(OtherActor) != INDEX_NONE)
		{
			return;
		}
		OverlappingCreatures.Add(OtherActor);
		OverlapFinishedCreatures.Add(OtherActor);
		OnOverlapBegin(OtherActor);
	}
}

void AAttackActor::CollisionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AEnemy>(GetOwner()) && Cast<AEnemy>(OtherActor)) return;
	if (Cast<APlayerCharacter>(GetOwner()) && Cast<APlayerCharacter>(OtherActor)) return;

	ICreature* Creature = Cast<ICreature>(OtherActor);
	if (Creature && Creature->GetDead() == false)
	{
		// 반대편 크리쳐의 콜리젼을 죽었을때 꺼버리면 될것같다.
		if (OverlappingCreatures.Find(OtherActor) != INDEX_NONE)
		{
			OverlappingCreatures.Remove(OtherActor);
			OnOverlapEnd(OtherActor);
		}
	}
}

void AAttackActor::RemoveDeadCreatures()
{
	OverlappingCreatures.RemoveAll
	(
		[](TWeakObjectPtr<AActor> CreatureActor)->bool
		{
			if (CreatureActor.IsValid())
			{
				ICreature* Creature = Cast<ICreature>(CreatureActor.Get());
				if (Creature)
				{
					return Creature->GetDead();
				}
			}
			else
			{
				return true;
			}

			return false;
		}
	);

	OverlapFinishedCreatures.RemoveAll
	(
		[](TWeakObjectPtr<AActor> CreatureActor)->bool
		{
			if (CreatureActor.IsValid())
			{
				ICreature* Creature = Cast<ICreature>(CreatureActor.Get());
				if (Creature)
				{
					return Creature->GetDead();
				}
			}
			else
			{
				return true;
			}

			return false;
		}
	);
}

void AAttackActor::SetDamage(int NewDamage)
{
	Damage = NewDamage;
}

int AAttackActor::GetDamage() const
{
	return Damage;
}

void AAttackActor::Initialize(const FName& InitAttachSocketName)
{
	AttachSocketName = InitAttachSocketName;
}

void AAttackActor::OnOverlapBegin_Implementation(AActor* OtherActor)
{
}

void AAttackActor::OnOverlapEnd_Implementation(AActor* OtherActor)
{
}

void AAttackActor::OnOverlapping_Implementation(float DeltaTime)
{
}

