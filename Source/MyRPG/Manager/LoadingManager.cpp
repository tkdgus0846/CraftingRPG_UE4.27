
#include "LoadingManager.h"
#include "Kismet/GameplayStatics.h"
#include <Engine/StreamableManager.h>
#include "Engine/AssetManager.h"
#include "Engine/LevelStreaming.h"
#include "MyRPG/UI/LoadingScreen.h"

ULoadingManager* ULoadingManager::Instance = nullptr;

ULoadingManager::ULoadingManager()
{
	static ConstructorHelpers::FClassFinder<ULoadingScreen> LoadingScreenFind(TEXT("/Game/Blueprints/UI/LoadingScreen/WBP_LoadingScreen.WBP_LoadingScreen_C"));
	if (LoadingScreenFind.Succeeded())
	{
		LoadingScreenClass = LoadingScreenFind.Class;
	}
}

void ULoadingManager::BeginDestroy()
{
	Super::BeginDestroy();

	if (WorldObject.IsValid())
	{
		WorldObject->GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}
}

ULoadingManager* ULoadingManager::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = NewObject<ULoadingManager>();
		Instance->AddToRoot();
		Instance->Initialize();
	}
	return Instance;
}

void ULoadingManager::Initialize()
{
	
}

void ULoadingManager::LoadLevel(TWeakObjectPtr<UObject> WorldContextObject, FName LevelName)
{
	if (WorldContextObject.IsValid() == false)
	{
		return;
	}

	GetInstance()->WorldObject = WorldContextObject;
	GetInstance()->ShowLoadingScreen();

	FString LevelPathString = FString::Printf(TEXT("/Game/Blueprints/Maps/Map_Game/%s"), *LevelName.ToString());
	GetInstance()->CurLevelPathString = LevelPathString;
}

void ULoadingManager::UnloadLastLevel()
{
	if (GetInstance()->PrevLevelPathString.IsEmpty() || GetInstance()->WorldObject.IsValid() == false)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("LoadingManager UnloadlastLevel : %s"), *GetInstance()->PrevLevelPathString);

	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = GetInstance();
	LatentInfo.ExecutionFunction = NAME_None;
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = __LINE__;

	UGameplayStatics::UnloadStreamLevel(GetInstance()->WorldObject.Get(), FName(*GetInstance()->PrevLevelPathString), LatentInfo, false);
}

void ULoadingManager::Shutdown()
{
	if (IsValid(Instance))
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadingManager Shutdown"));
		UnloadLastLevel();
		Instance->RemoveFromRoot();
	}
	Instance = nullptr;
}

void ULoadingManager::ShowLoadingScreen()
{
	if (IsValid(LoadingScreen) == false)
	{
		LoadingScreen = CreateWidget<ULoadingScreen>(WorldObject->GetWorld(), LoadingScreenClass);
		LoadingScreen->GetOnLoadingFadeInFinished().AddDynamic(this, &ULoadingManager::LoadStreamLevel);
	}
	LoadingScreen->FadeIn();
}

void ULoadingManager::HideLoadingScreen()
{
	if (IsValid(LoadingScreen) == false)
	{
		return;
	}

	LoadingScreen->Finishloading();
	LoadingScreen->FadeOut();
}

void ULoadingManager::LoadStreamLevel()
{
	UnloadLastLevel();
	PrevLevelPathString = CurLevelPathString;

	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = "OnLevelStreamLoaded";
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = __LINE__;

	UGameplayStatics::LoadStreamLevel(WorldObject.Get(), FName(*CurLevelPathString), true, false, LatentInfo);
}

void ULoadingManager::OnLevelStreamLoaded()
{
	HideLoadingScreen();
}
