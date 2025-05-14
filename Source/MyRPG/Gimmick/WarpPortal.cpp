
#include "WarpPortal.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "MyRPG/UI/InteractWidget.h"
#include "MyRPG/Types/ChannelType.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "MyRPG/GameModes/MyRPGGameModeBase.h"

AWarpPortal::AWarpPortal()
{
	PrimaryActorTick.bCanEverTick = true;

	WarpTriggerComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("WarpTrigger"));
	if (IsValid(WarpTriggerComponent))
	{
		RootComponent = WarpTriggerComponent;
		WarpTriggerComponent->SetCollisionProfileName(TEXT("ItemDetection"));
		WarpTriggerComponent->SetCapsuleHalfHeight(88.f);
		WarpTriggerComponent->SetCapsuleRadius(32.050377f);
	}

	ConstructorHelpers::FClassFinder<UInteractWidget> InteractWidgetFinder(TEXT("/Game/Blueprints/UI/InteractWidget/WBP_BasicInteractWidget.WBP_BasicInteractWidget_C"));
	TSubclassOf<UInteractWidget> InteractWidgetClass = nullptr;

	if (InteractWidgetFinder.Succeeded())
	{
		InteractWidgetClass = InteractWidgetFinder.Class;
	}

	InteractWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	if (IsValid(InteractWidgetComponent))
	{
		InteractWidgetComponent->SetupAttachment(RootComponent);
		InteractWidgetComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		InteractWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		InteractWidgetComponent->SetDrawSize(FVector2D(200.f, 200.f));
		InteractWidgetComponent->SetWidgetClass(InteractWidgetClass);

		if (IsValid(InteractWidgetComponent))
		{
			InteractWidgetComponent->SetVisibility(false);
		}
	}
}

void AWarpPortal::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (SymmetricPortal.IsValid())
	{
		SymmetricPortal->SymmetricPortal = this;
	}
}

void AWarpPortal::BeginPlay()
{
	Super::BeginPlay();

	/*FHitResult HitResult;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation - FVector(0.f,0.f,500.f);

	bool bTraceHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility);
	if (bTraceHit)
	{
		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		
		if (IsValid(Character))
		{
			PortalLocation = HitResult.ImpactPoint + FVector(0.f, 0.f, Character->GetDefaultHalfHeight());
		}
	}*/
	WarpPoint = GetActorLocation();
}

void AWarpPortal::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AWarpPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWarpPortal::Interact_Implementation()
{
	if (SymmetricPortal.IsValid())
	{
		AMyRPGGameModeBase* Gamemode = Cast<AMyRPGGameModeBase>(GetWorld()->GetAuthGameMode());
		

		if (IsValid(Gamemode))
		{
			Gamemode->LoadLevel(SymmetricPortal.Get()->LevelType);

			GetWorld()->GetTimerManager().SetTimer(WarpTimer, this, &AWarpPortal::WarpPlayer, 1.f);
		}
	}
}

void AWarpPortal::SetInteractWidgetVisibility_Implementation(bool bVisibility)
{
	if (IsValid(InteractWidgetComponent))
	{
		InteractWidgetComponent->SetVisibility(bVisibility);
	}
}

void AWarpPortal::WarpPlayer()
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (IsValid(Character))
	{
		Character->SetActorLocation(SymmetricPortal.Get()->WarpPoint);
	}
}

