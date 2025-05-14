
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LoadingManager.generated.h"

/*
	레벨을 로드하는 역할.
*/
UCLASS()
class MYRPG_API ULoadingManager : public UObject
{
	GENERATED_BODY()

public:
	static void LoadLevel(TWeakObjectPtr<UObject> WorldContextObject, FName LevelName);
	static void UnloadLastLevel();
	static void Shutdown();

private:
	ULoadingManager();
	virtual void BeginDestroy() override;
	static ULoadingManager* GetInstance();
	static ULoadingManager* Instance;

	void Initialize();

	/* Loading UI */
	void ShowLoadingScreen();
	void HideLoadingScreen();

	UFUNCTION()
	void LoadStreamLevel();
	UFUNCTION()
	void OnLevelStreamLoaded();

	UPROPERTY()
	TWeakObjectPtr<UObject> WorldObject;

	FTimerHandle LoadingProgressTimer;


	UPROPERTY()
	TSubclassOf<class ULoadingScreen> LoadingScreenClass;
	UPROPERTY()
	class ULoadingScreen* LoadingScreen;

	FString PrevLevelPathString = "";
	FString CurLevelPathString = "";
};
