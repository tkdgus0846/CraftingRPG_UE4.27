
#include "Facility.h"
#include "Components/WidgetComponent.h"
#include "MyRPG/UI/InteractWidget.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "MyRPG/UI/DragableWindow.h"
#include <Kismet/GameplayStatics.h>
#include <MyRPG/UI/PlayerHUD.h>
#include "MyRPG/Player/PlayerCharacter.h"
#include "MyRPG/Player/Components/InteractComponent.h"
#include "MyRPG/Player/Components/Inventory.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

AFacility::AFacility()
{
	PrimaryActorTick.bCanEverTick = true;

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

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>("LootWidget");
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetFind(TEXT("/Game/Blueprints/UI/InteractWidget/WBP_BasicInteractWidget.WBP_BasicInteractWidget_C"));
	if (InteractWidget && WidgetFind.Succeeded())
	{
		InteractWidget->SetupAttachment(RootComponent);

		InteractWidget->SetWidgetClass(WidgetFind.Class);
		InteractWidget->SetWidgetSpace(EWidgetSpace::Screen);
		InteractWidget->SetDrawSize(FVector2D(200.f, 500.f));
	}

	InputInventory = CreateDefaultSubobject<UInventory>("InputInventory");
	OutputInventory = CreateDefaultSubobject<UInventory>("OutputInventory");
	
	static ConstructorHelpers::FObjectFinder<USoundCue> InteractSoundFind(TEXT("/Game/Blueprints/SoundCue/Facility/FacilityInteract_Cue.FacilityInteract_Cue"));
	if (InteractSoundFind.Succeeded())
	{
		InteractSound = InteractSoundFind.Object;
	}
}

void AFacility::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(InteractWidget))
	{
		UInteractWidget* Widget = Cast<UInteractWidget>(InteractWidget->GetWidget());
		if (IsValid(Widget))
		{
			Widget->SetInteractText(FacilityName);
		}
		InteractWidget->SetVisibility(false);
	}

	if (IsValid(DetectSphere))
	{
		DetectSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &AFacility::OnOverlapBegin);
		DetectSphere->OnComponentEndOverlap.AddUniqueDynamic(this, &AFacility::OnOverlapEnd);
	}
}

void AFacility::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsWorking)
	{
		RemainedTime -= DeltaTime;

		if (RemainedTime <= 0.f)
		{
			RemainedTime = 0.f;
		}

		FacilityWorkingDelegate.Broadcast(RemainedTime);

		if (RemainedTime == 0.f)
		{
			WorkEnd();
		}
	}
}

void AFacility::Interact_Implementation()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APlayerHUD* PlayerHUD = nullptr;

	if (IsValid(PlayerController))
	{
		PlayerHUD = PlayerController->GetHUD<APlayerHUD>();
	}
	
	if (IsValid(FacilityWindowClass) && IsValid(FacilityWindow) == false)
	{
		FacilityWindow = CreateWidget<UDragableWindow>(PlayerController, FacilityWindowClass);
		

		if (IsValid(PlayerHUD))
		{
			PlayerHUD->AddDragableWindowInCharacterOverlay(FacilityWindow);
		}
		
		if (IsValid(FacilityWindow))
		{
			FacilityWindow->SetOwnerFacility(this);
			FacilityWindow->DragableWindow_Initialize();
		}
			
	}

	if (IsValid(PlayerHUD))
	{
		PlayerHUD->RegisterFacilityWindow(FacilityWindow);

		PlayerHUD->ShowDragableWindow(EDragableWindow::EDW_Inventory, false);
		PlayerHUD->ShowDragableWindow(EDragableWindow::EDW_Facility, false);
	}

	bIsPlayerUsing = true;
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), InteractSound, GetActorLocation());
}

void AFacility::SetInteractWidgetVisibility_Implementation(bool bVisibility)
{
	if (IsValid(InteractWidget))
	{
		InteractWidget->SetVisibility(bVisibility);
	}
}

void AFacility::WorkStart_Implementation(float Duration)
{
	bIsWorking = true;
	RemainedTime = Duration;
	ActivateInteractVFX();

	if (IsValid(CurAudioComponent) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Audio?"));
		CurAudioComponent = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WorkSound, GetActorLocation());
		if (IsValid(CurAudioComponent))
		{
			CurAudioComponent->FadeIn(0.2f);
		}
	}
}

void AFacility::WorkEnd_Implementation()
{
	bIsWorking = false;
	DeactivateInteractVFX();
	FacilityWorkEndDelegate.Broadcast();

	GetWorldTimerManager().SetTimerForNextTick(this, &AFacility::FadeOutSound);
}

void AFacility::FinishFacilityInteract(bool bHideAllWindow)
{
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->GetInteractComponent()->ClearCurrentFacility();
		PlayerCharacter->SetWeaponVisiblity(true);
		PlayerCharacter->StopAnimMontage();
	}



	bIsPlayerUsing = false;
	PlayerCharacter = nullptr;

	if (bHideAllWindow)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		APlayerHUD* PlayerHUD = nullptr;

		if (IsValid(PlayerController))
		{
			PlayerHUD = PlayerController->GetHUD<APlayerHUD>();
		}

		if (IsValid(PlayerHUD))
		{
			PlayerHUD->HideAllDragableWindow();
		}
	}
	
}

void AFacility::HideFacilityWindow()
{
	if (IsValid(FacilityWindow))
	{
		FacilityWindow->HideWindow();
	}
}

UNiagaraComponent* AFacility::InitInteractVFXComponents(UNiagaraComponent* VFX1, UNiagaraComponent* VFX2)
{
	if (InteractVFXComponents.Find(VFX1) == -1)
	{
		InteractVFXComponents.Add(VFX1);
	}

	if (VFX2 == nullptr) return VFX1;

	InteractVFXComponents.Add(VFX2);

	return VFX2;
}

void AFacility::ActivateInteractVFX()
{
	for (UNiagaraComponent* Item : InteractVFXComponents)
	{
		if (IsValid(Item))
		{
			Item->SetVisibility(true);
			//Item->Activate();
		}
	}
}

void AFacility::DeactivateInteractVFX()
{
	for (UNiagaraComponent* Item : InteractVFXComponents)
	{
		if (IsValid(Item))
		{
			Item->SetVisibility(false);
			//Item->Deactivate();
		}
	}
}

void AFacility::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void AFacility::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (PlayerCharacter == OtherActor)
	{
		FinishFacilityInteract(true);
	}
}

void AFacility::FadeOutSound()
{
	if (bIsWorking == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("FadeOutFailed!"));
		return;
	}

	if (IsValid(CurAudioComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("FadeOutSucceed!"));
		CurAudioComponent->FadeOut(0.4f, 0.f);
		CurAudioComponent = nullptr;
	}
}

