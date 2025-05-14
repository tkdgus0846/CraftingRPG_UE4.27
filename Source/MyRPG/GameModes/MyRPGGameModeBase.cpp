
#include "MyRPGGameModeBase.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "MyRPG/Controller/MyPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "MyRPG/Manager/LoadingManager.h"
#include <Kismet/GameplayStatics.h>
#include <MyRPG/UI/PlayerHUD.h>
#include "Engine/LevelScriptActor.h"
#include "MyRPG/UI/MainScreen.h"
#include "MyRPG/SaveGame/PlayerSave.h"
#include <Kismet/KismetRenderingLibrary.h>
#include "MyRPG/FunctionLibrary/RenderTargetFunctionLibrary.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/SceneCapture2D.h"

AMyRPGGameModeBase::AMyRPGGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	MinimapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MinimapCapture"));
	if (IsValid(MinimapCapture))
	{
		MinimapCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	}
}

void AMyRPGGameModeBase::TimeDilation(float DelayTime)
{
	GetWorld()->GetWorldSettings()->SetTimeDilation(DelayTime);
}

APawn* AMyRPGGameModeBase::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	APawn* NewPawn = Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);

	return NewPawn;
}

void AMyRPGGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &AMyRPGGameModeBase::OnLevelLoaded);
	FWorldDelegates::LevelRemovedFromWorld.AddUObject(this, &AMyRPGGameModeBase::OnLevelRemoved);

	BlackScreen = CreateWidget<UUserWidget>(GetWorld(), BlackScreenClass);
	BlackScreen->AddToViewport(1001);

	PlayerInputSetting(false);
	InitMinimapCapture();
}

void AMyRPGGameModeBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AMyRPGGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FollowPlayerAndMinimapCapture();
}

AActor* AMyRPGGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	return Super::ChoosePlayerStart_Implementation(Player);
}

void AMyRPGGameModeBase::ChangeLevelType(ELevelType LevelType)
{
	PrevLevelType = CurrentLevelType;
	CurrentLevelType = LevelType;	
}

void AMyRPGGameModeBase::FollowPlayerAndMinimapCapture()
{
	if (IsValid(MinimapCapture) == false)
	{
		return;
	}

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (IsValid(PlayerCharacter) == false)
	{
		return;
	}

	FVector Location = PlayerCharacter->GetActorLocation();
	Location.Z = 8000.f;
	
	MinimapCapture->SetWorldLocation(Location);
	MinimapCapture->OrthoWidth = MinimapCaptureZ;
	MinimapCapture->CaptureScene();
}

void AMyRPGGameModeBase::InitMinimapCapture()
{
	if (IsValid(MinimapCapture))
	{
		MinimapCapture->TextureTarget = MinimapRenderTarget;
		FRotator Rotation = FVector(0.f, 0.f, -1.f).Rotation();
		MinimapCapture->SetWorldRotation(Rotation);
		MinimapCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;

		TArray<AActor*> FindActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACreatureCharacter::StaticClass(), FindActors);

		MinimapCapture->HiddenActors.Empty();
		for (AActor* Actor : FindActors)
		{
			if (IsValid(Actor))
			{
				MinimapCapture->HiddenActors.Add(Actor);
			}
		}

		MinimapCapture->bCaptureEveryFrame = false;

		MinimapCapture->ShowFlags.SetParticles(false);           // 파티클 비활성화
		MinimapCapture->ShowFlags.SetTranslucency(false);        // 투명 렌더링 비활성화
		MinimapCapture->ShowFlags.SetFog(false);                 // 안개 비활성화
		MinimapCapture->ShowFlags.SetDecals(false);
	}
}

void AMyRPGGameModeBase::OnLevelLoaded(ULevel* NewLevel, UWorld* World)
{
	// 최초 게임을 시작했을때 로딩화면이 끝나면 블랙스크린을 제거
	if (PrevLevelType == ELevelType::ELT_None)
	{
		if (IsValid(BlackScreen))
		{
			BlackScreen->RemoveFromParent();
		}
	}

	if (CurrentLevelType == ELevelType::ELT_MainLevel)
	{
		if (IsValid(MainScreen) == false)
		{
			MainScreen = CreateWidget<UMainScreen>(World, MainScreenClass);
		}
		MainScreen->AddToViewport(10);
	}
	else
	{
		// 플레이어스폰위치?
		/*ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (PlayerCharacter)
		{
			PlayerCharacter->SetActorLocation(FVector(0.f, 0.f, 1000.f));
		}*/

		InitMinimapCapture();
	}
}

void AMyRPGGameModeBase::OnLevelRemoved(ULevel* RemoveLevel, UWorld* World)
{
	if (PrevLevelType == ELevelType::ELT_MainLevel)
	{
		if (IsValid(MainScreen))
		{
			MainScreen->RemoveFromParent();
		}
	}
}

void AMyRPGGameModeBase::LoadLevel(ELevelType LevelType)
{
	ChangeLevelType(LevelType);
	ULoadingManager::LoadLevel(this, GetLevelName(LevelType));
}

FName AMyRPGGameModeBase::GetLevelName(ELevelType LevelType)
{
	FName LevelName;
	// 이것도 함수화
	switch (LevelType)
	{
	case ELevelType::ELT_MainLevel:
		LevelName = "MainLevel";
		break;
	case ELevelType::ELT_FieldLevel:
		LevelName = "FieldLevel";
		break;
	case ELevelType::ELT_PaladinBossLevel:
		LevelName = "PaladinBossLevel";
		break;
	default:
		break;
	}
	return LevelName;
}

void AMyRPGGameModeBase::PlayerInputSetting(bool bEnableInput)
{
	/*APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));*/
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if (IsValid(PlayerController))
	{
		PlayerController->PlayerInputSetting(bEnableInput);
	}
}

void AMyRPGGameModeBase::PlayerCursorSetting(bool bShowCursor, uint8 CursorType)
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	APlayerHUD* PlayerHUD = PlayerController->GetHUD<APlayerHUD>();

	if (IsValid(PlayerCharacter) && IsValid(PlayerHUD))
	{
		if (bShowCursor)
		{
			PlayerHUD->ChangeCursorType((ECursorType)CursorType);
		}
		else
		{
			PlayerHUD->HideCursor();
		}
	}
}

void AMyRPGGameModeBase::SetAllActorsTimeDilation(float TimeDilation)
{
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
	for (AActor* Actor : AllActors)
	{
		if (IsValid(Actor))
		{
			Actor->CustomTimeDilation = TimeDilation;
		}
	}
}

void AMyRPGGameModeBase::NewGame()
{
	StartGameTime = GetWorld()->GetTimeSeconds();
	LastGamePlayTime = 0.f;

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (IsValid(PlayerCharacter))
	{
		FVector InitLocation = FVector(0.f, 0.f, PlayerCharacter->GetDefaultHalfHeight());
		PlayerCharacter->ClearPlayer(InitLocation);
		LoadLevel(ELevelType::ELT_FieldLevel);
	}
}

bool AMyRPGGameModeBase::LoadGame()
{
	StartGameTime = GetWorld()->GetTimeSeconds();

	UPlayerSave* PlayerSave = Cast<UPlayerSave>(UGameplayStatics::LoadGameFromSlot(CurSaveSlotName, 0));
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (IsValid(PlayerSave) && IsValid(PlayerCharacter))
	{
		LastGamePlayTime = PlayerSave->LastPlayGameTime;
		PlayerCharacter->LoadPlayer(PlayerSave);
		LoadLevel((ELevelType)PlayerSave->LastLevelType);
		return true;
	}
	else
	{
		return false;
	}
}

void AMyRPGGameModeBase::SaveGame()
{
	float SaveGameTime = GetWorld()->GetTimeSeconds();

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (IsValid(PlayerCharacter) == false)
	{
		return;
	}

	UTextureRenderTarget2D* RenderTarget = URenderTargetFunctionLibrary::ExportCaptureSceneToRenderTarget2D(GetWorld(), "/Game/Assets/Texture2D/RenderTarget", CurSaveSlotName, PlayerCharacter->GetCameraTransform());
	URenderTargetFunctionLibrary::ExportRenderTargetToHDR(GetWorld(), RenderTarget, CurSaveSlotName);	

	UPlayerSave* PlayerSave = NewObject<UPlayerSave>();
	if (IsValid(PlayerSave))
	{
		PlayerSave->SaveGame(PlayerCharacter, CurSaveSlotName, LastGamePlayTime + (SaveGameTime - StartGameTime));
	}
}

const bool AMyRPGGameModeBase::IsLoading() const
{
	return bIsLoading;
}

//void AMyRPGGameModeBase::UnloadLastLevel()
//{
//	ULoadingManager
//}
