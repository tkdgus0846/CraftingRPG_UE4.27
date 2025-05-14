
#include "CreatureActor.h"
#include "Components/WidgetComponent.h"

#include <Engine/DataTable.h>
#include <NiagaraFunctionLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "Sound/SoundCue.h"
#include "NiagaraSystem.h"

#include "MyRPG/Interfaces/Command.h"
#include "MyRPG/Command/CommandPoolManager.h"
#include "MyRPG/Data/DamageInfo.h"

ACreatureActor::ACreatureActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	if (RootSceneComponent)
	{
		RootComponent = RootSceneComponent;
	}

	GatherWidgetComponent.Add(CreateDefaultSubobject<UWidgetComponent>(TEXT("GatherWidget1")));
	if (GatherWidgetComponent[0])
	{
		GatherWidgetComponent[0]->SetupAttachment(RootComponent);
	}

	GatherWidgetComponent.Add(CreateDefaultSubobject<UWidgetComponent>(TEXT("GatherWidget2")));
	if (GatherWidgetComponent[1])
	{
		GatherWidgetComponent[1]->SetupAttachment(RootComponent);
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> DropItemDataTableFind(TEXT("/Game/Blueprints/DataTable/DropItemDataTable.DropItemDataTable"));
	if (DropItemDataTableFind.Succeeded())
	{
		DropItemDataTable = DropItemDataTableFind.Object;
	}
}

void ACreatureActor::BeginPlay()
{
	Super::BeginPlay();
	
	OnDamaged.AddDynamic(this, &ACreatureActor::OnDamage);

	for (int i = 0; i < GatherWidgetComponent.Num(); i++)
	{
		GatherWidgetComponent[i]->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ACreatureActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnDamaged.RemoveDynamic(this, &ACreatureActor::OnDamage);
	GetWorldTimerManager().ClearAllTimersForObject(this);

	if (EndPlayReason == EEndPlayReason::Type::Destroyed)
	{
		// 액터가 Destroyed에 대한 내용을 처리하는게 좋아보임.
	}
}

void ACreatureActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACreatureActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	BringDataTableDatas();
}

void ACreatureActor::Damaged(FDamageInfo* DamageInfo)
{
	if (GetDead() || DamageInfo == nullptr) return;

	LastHitCharacter = DamageInfo->DamageCauser;

	TSortedMap<EDamageActionCommandTag, FCommandArray>& Commands = DamageInfo->GetDamageActionCommands();

	for (TSortedMap<EDamageActionCommandTag, FCommandArray>::TIterator It(Commands); It; ++It)
	{
		FCommandArray& CommandArray = It.Value();
		for (ICommand* Command : CommandArray.Get())
		{
			Command->Execute(this);
			UCommandPoolManager::ReturnCommand(Command);
		}
	}
}

FDamagedDelegate& ACreatureActor::GetDamagedDelegate()
{
	return OnDamaged;
}

FDeadDelegate& ACreatureActor::GetDeadDelegate()
{
	return OnDead;
}

void ACreatureActor::SetDead_Implementation()
{
	SpawnDeathFX();
	bDead = true;

	if (bAutoDestroy)
		SetLifeSpan(DestroyTime);

	OnDead.Broadcast();
}

void ACreatureActor::OnDamage_Implementation()
{
	SpawnHitFX();
	TargetWhiteness += 0.1f;

	if (MaxWhiteness < TargetWhiteness)
	{
		TargetWhiteness = MaxWhiteness;
	}

	bIsDamaged = true;
	WhitenessInterping();

	
}

void ACreatureActor::WhitenessInterping()
{
	InterpWhiteness = FMath::FInterpTo(InterpWhiteness, (bIsDamaged) ? TargetWhiteness : 0.f, GetWorld()->GetDeltaSeconds(), (bIsDamaged) ? WhitenessUpSpeed : WhitenessDownSpeed);

	for (auto Item : GetMaterialInstances())
	{
		Item->SetScalarParameterValue("Whiteness", InterpWhiteness);
	}

	if (bIsDamaged)
	{
		bIsDamaged = !FMath::IsNearlyEqual(InterpWhiteness, TargetWhiteness, 0.1f);
	}
	else
	{
		if (FMath::IsNearlyEqual(InterpWhiteness, 0.f, 0.001f)) return;
	}
	


	GetWorld()->GetTimerManager().SetTimer(InterpingWhitenessTimer, this, &ACreatureActor::WhitenessInterping, 0.0001f, false, GetWorld()->GetDeltaSeconds());
}

void ACreatureActor::BringDataTableDatas()
{
	if (FindDropItemName.IsNone()) return;
	if (DropItemDataTable == nullptr) return;

	FDropItemDataTable* DataRow = DropItemDataTable->FindRow<FDropItemDataTable>(FindDropItemName, FString("ItemDataTable"));

	if (DataRow == nullptr) return;

	DropItems = DataRow->DropItems;
}

void ACreatureActor::SpawnHitFX()
{
	FVector SpawnLocation = GetActorLocation() + HitFXOffset;
	if (IsValid(HitVFX))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitVFX, SpawnLocation);
	}

	if (IsValid(HitSFX))
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSFX, SpawnLocation);
	}
}

void ACreatureActor::SpawnDeathFX()
{
	FVector SpawnLocation = GetActorLocation() + DeathFXOffset;
	if (IsValid(DeathVFX))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, DeathVFX, SpawnLocation);
	}

	if (IsValid(DeathSFX))
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSFX, SpawnLocation);
	}
}

bool ACreatureActor::GetDead()
{
	return bDead;
}

int ACreatureActor::GetHP() const
{
	return HP;
}

void ACreatureActor::GainHP(int HPAmount)
{
	HP += HPAmount;
	if (HP > MaxHP)
		HP = MaxHP;
}

void ACreatureActor::SetMaxHP(int NewMaxHP, bool bHPUpdate)
{
	MaxHP = NewMaxHP;
	if (bHPUpdate)
		HP = MaxHP;
}

int ACreatureActor::GetMaxHP() const
{
	return MaxHP;
}

void ACreatureActor::SetHP(int NewHP)
{
	HP = NewHP;
	if (HP > MaxHP)
	{
		HP = MaxHP;
	}
	else if (HP < 0)
	{
		HP = 0;
	}
}

