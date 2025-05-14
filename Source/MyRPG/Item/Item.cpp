
#include "Item.h"
#include "UObject/ConstructorHelpers.h"
#include <Engine/DataTable.h>
#include "Components/StaticMeshComponent.h"
#include <MyRPG/Data/ItemDataTable.h>
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include <Kismet/KismetMathLibrary.h>
#include "MyRPG/UI/LootWidget.h"
#include "NiagaraComponent.h"
#include "MyRPG/BaseClasses/Components/ActionComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

#pragma region BasicComponents
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>("RootSceneComponent");

	if (RootSceneComponent)
	{
		SetRootComponent(RootSceneComponent);
	}

	DetectSphere = CreateDefaultSubobject<USphereComponent>("DetectSphere");

	if (DetectSphere)
	{
		DetectSphere->SetupAttachment(RootComponent);
		DetectSphere->SetSphereRadius(100.f);
		DetectSphere->SetCollisionProfileName("ItemDetection");
	}

	FXComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraFX");
	
	if (FXComponent)
	{
		FXComponent->SetupAttachment(RootComponent);
		FXComponent->SetActive(true);
	}

	LootWidget = CreateDefaultSubobject<UWidgetComponent>("LootWidget");
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetFind(TEXT("/Game/Blueprints/UI/InteractWidget/WBP_LootWidget.WBP_LootWidget_C"));
	if (LootWidget && WidgetFind.Succeeded())
	{
		LootWidget->SetupAttachment(RootComponent);
		
		LootWidget->SetWidgetClass(WidgetFind.Class);
		LootWidget->SetWidgetSpace(EWidgetSpace::Screen);
		LootWidget->SetDrawSize(FVector2D(200.f, 500.f));
	}

	DroppedMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("DroppedMeshComponent");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFind(TEXT("/Game/Assets/Mesh/Item/Sack.Sack"));
	if (DroppedMeshComponent && MeshFind.Succeeded())
	{
		DroppedMeshComponent->SetStaticMesh(MeshFind.Object);
		DroppedMeshComponent->SetupAttachment(RootComponent);
		DroppedMeshComponent->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
		DroppedMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}

	ActionComponent = CreateDefaultSubobject<UActionComponent>("ActionComponent");
#pragma endregion

#pragma region VFX
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> DroppedFXFinder(TEXT("/Game/Blueprints/FX/ItemFX/NS_DroppedItem.NS_DroppedItem"));
	if (DroppedFXFinder.Succeeded())
	{
		DroppedFX = DroppedFXFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> DroppingFXFinder(TEXT("/Game/Blueprints/FX/ItemFX/NS_DroppingItem.NS_DroppingItem"));
	if (DroppingFXFinder.Succeeded())
	{
		DroppingFX = DroppingFXFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> LootFXFinder(TEXT("/Game/Blueprints/FX/ItemFX/NS_LootItem.NS_LootItem"));
	if (LootFXFinder.Succeeded())
	{
		LootFX = LootFXFinder.Object;
	}
#pragma endregion

#pragma region SFX
	static ConstructorHelpers::FObjectFinder<USoundCue> DropHitSFXFinder(TEXT("/Game/Blueprints/SoundCue/Item/ItemDropHit_Cue.ItemDropHit_Cue"));
	if (DropHitSFXFinder.Succeeded())
	{
		DropHitSFX = DropHitSFXFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> DropSFXFinder(TEXT("/Game/Blueprints/SoundCue/Item/ItemDrop_Cue.ItemDrop_Cue"));
	if (DropSFXFinder.Succeeded())
	{
		DropSFX = DropSFXFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> LootSFXFinder(TEXT("/Game/Blueprints/SoundCue/Item/ItemPickup_Cue.ItemPickup_Cue"));
	if (LootSFXFinder.Succeeded())
	{
		LootSFX = LootSFXFinder.Object;
	}
#pragma endregion
	
	SetActorHiddenInGame(false);
}

void AItem::PushInInventory(bool bWasDropped, AActor* NewOwner)
{
	bInInventory = true;

	if (FXComponent)
		FXComponent->SetActive(false);

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	if (bWasDropped && IsValid(LootFX))
	{
		ItemOwner = NewOwner;

		if (LootSFX)
		{
			UGameplayStatics::PlaySoundAtLocation(this, LootSFX, GetActorLocation());
		}

		LootFXComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, LootFX, GetActorLocation());
		GetWorld()->GetTimerManager().SetTimer(SendLootedPosTimer, this, &AItem::SendLootedPos, 0.016f, true);
	}
}

void AItem::PopFromInventory(const FVector& PopPos, float PopRadius)
{
	ItemOwner = nullptr;

	bInInventory = false;
	SetActorLocation(PopPos);

	SetActorHiddenInGame(false);
	if (DroppedMeshComponent)
		DroppedMeshComponent->SetVisibility(false);

	if (FXComponent && IsValid(DroppingFX))
	{
		FXComponent->SetActive(true);
		FXComponent->SetAsset(DroppingFX);
	}

	if (DropSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DropSFX, PopPos);
	}
		
	float ActionTime = DroppingAction(PopRadius);
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AItem::DropFinished, ActionTime);
}

bool AItem::CombineItem(AItem* InItem)
{
	if (IsValid(InItem) == false || (InItem->GetClass() != GetClass()))
	{
		return false;
	}

	int NewItemCount = ItemCount + InItem->GetItemCount();

	if (NewItemCount > ItemMaxStackCount)
	{
		SetItemCount(ItemMaxStackCount);
		int RemainItemCount = NewItemCount - ItemMaxStackCount;
		InItem->SetItemCount(RemainItemCount);
	}
	else
	{
		ItemCount = NewItemCount;
		InItem->SetLifeSpan(1.f);
	}

	return true;
}

void AItem::SetInteractWidgetVisibility_Implementation(bool bVisibility)
{
	if (LootWidget)
	{
		LootWidget->SetVisibility(bVisibility);
	}
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	if (DetectSphere)
	{
		DetectSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &AItem::OnOverlapBegin);
		DetectSphere->OnComponentEndOverlap.AddUniqueDynamic(this, &AItem::OnOverlapEnd);

		OriginalLocation = DetectSphere->GetRelativeLocation();
		DetectSphere->SetWorldLocation({ 0.f,0.f,-100000.f });

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AItem::SetLocationToOriginal, 0.0001f, false, GetWorld()->GetDeltaSeconds());
	}

	if (LootWidget)
	{
		LootWidget->SetVisibility(false);
		UpdateLootWidget();
	}

	if (DroppedMeshComponent)
	{
		DroppedMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	BringDataTableDatas();

	if (FXComponent && IsValid(DroppedFX))
	{
		FXComponent->SetAsset(DroppedFX);
	}
}

void AItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*if (Cast<APlayerCharacter>(OtherActor))
	{
		if (LootWidget)
		{
			LootWidget->SetVisibility(true);
		}
	}*/
}

void AItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*if (Cast<APlayerCharacter>(OtherActor))
	{
		if (LootWidget)
		{
			LootWidget->SetVisibility(false);
		}
	}*/
}

void AItem::BringDataTableDatas()
{
	if (DataTable == nullptr) return;

	FItemDataTable* DataRow = DataTable->FindRow<FItemDataTable>(RowName, FString("ItemDataTable"));
	
	if (DataRow == nullptr) return;

	ItemID = DataRow->ItemID;
	PreviewTexture = DataRow->PreviewTexture;
	ItemMaxStackCount = DataRow->ItemMaxStackCount;
}

void AItem::UpdateLootWidget()
{
	ULootWidget* Widget = Cast<ULootWidget>(LootWidget->GetWidget());
	if (Widget)
	{

		FString NewString = ItemID.ToString() + " (" + FString::FromInt(ItemCount) + ")";
		Widget->SetItemText(FName(*NewString));
	}
}

void AItem::SetLocationToOriginal()
{
	if (DetectSphere == nullptr) return;
	DetectSphere->SetRelativeLocation(OriginalLocation);
}

float AItem::DroppingAction(float ActionRadius)
{
	const float RadiusMin = ActionRadius - 5.f;
	const float RadiusMax = ActionRadius + 5.f;

	const FVector Center = GetActorLocation();
	FHitResult HitResult;

	while (true)
	{
		float RandomAngle = FMath::FRandRange(0.f, 2.f * PI);
		float Radius = FMath::FRandRange(RadiusMin, RadiusMax);

		float X = Center.X + Radius * FMath::Cos(RandomAngle);
		float Y = Center.Y + Radius * FMath::Sin(RandomAngle);

		FVector StartPos = FVector(X, Y, GetActorLocation().Z);
		FVector EndPos = FVector(X, Y, GetActorLocation().Z - 400.f);

		bool bTraceSucceed = GetWorld()->LineTraceSingleByChannel(HitResult, StartPos, EndPos, ECollisionChannel::ECC_Visibility);

		if (bTraceSucceed)
			break;
	}
	
	FActionInfo Action;

	Action.ActionTime = 0.3f;
	Action.CurveType = ECurveFloat::ECF_Default;

	FVector DiffVec = HitResult.Location - Center;
	FVector RightVector = FVector::CrossProduct(FVector::UpVector, DiffVec.GetSafeNormal());

	Action.AddMovePoint(Center);
	Action.AddMovePoint(Center + (DiffVec/2.f) + (FVector::UpVector * 100.f));
	Action.AddMovePoint(HitResult.Location + (FVector::UpVector * 20.f));
	
	ActionComponent->AddAction(Action);
	ActionComponent->PlayAction();

	if (IsValid(DetectSphere))
	{
		DetectSphere->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}

	return Action.ActionTime;
}

void AItem::DropFinished()
{
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	if (DroppedMeshComponent)
		DroppedMeshComponent->SetVisibility(true);

	if (IsValid(DetectSphere))
	{
		DetectSphere->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	}

	if (FXComponent && IsValid(DroppedFX))
		FXComponent->SetAsset(DroppedFX);

	if (DropHitSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DropHitSFX, GetActorLocation());
	}

	UpdateLootWidget();
}

void AItem::SendLootedPos()
{
	if (IsValid(ItemOwner) && IsValid(LootFXComp))
	{
		LootFXComp->SetVectorParameter("LootedPos", ItemOwner->GetActorLocation());
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(SendLootedPosTimer);
	}
}
