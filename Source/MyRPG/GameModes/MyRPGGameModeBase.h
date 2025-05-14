
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyRPG/Types/LevelType.h"
#include "MyRPGGameModeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangedLevelType);

UCLASS()
class MYRPG_API AMyRPGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyRPGGameModeBase();

	UFUNCTION(Exec)
	void TimeDilation(float DelayTime);

	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot);

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UFUNCTION()
	void OnLevelLoaded(ULevel* NewLevel, UWorld* World);
	UFUNCTION()
	void OnLevelRemoved(ULevel* RemoveLevel, UWorld* World);

	UFUNCTION(BlueprintCallable)
	void LoadLevel(ELevelType LevelType);
	FName GetLevelName(ELevelType LevelType);

	UFUNCTION(BlueprintCallable)
	void PlayerInputSetting(bool bEnableInput);
	UFUNCTION(BlueprintCallable)
	void PlayerCursorSetting(bool bShowCursor, uint8 CursorType = 2);

	void SetAllActorsTimeDilation(float TimeDilation);

	void NewGame();
	bool LoadGame();
	void SaveGame();

	const bool IsLoading() const;
	void SetIsLoading(bool bInIsLoading) { bIsLoading = bInIsLoading; }

private:
	ELevelType PrevLevelType = ELevelType::ELT_None;
	ELevelType CurrentLevelType = ELevelType::ELT_None;	

	UFUNCTION(BlueprintCallable)
	void ChangeLevelType(ELevelType LevelType);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UMainScreen> MainScreenClass;
	UPROPERTY()
	class UMainScreen* MainScreen = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> BlackScreenClass;
	UPROPERTY()
	class UUserWidget* BlackScreen = nullptr;

	FString CurSaveSlotName = "SaveSlot1";

	/* 로딩 중인지 아닌지 */
	bool bIsLoading = false;

	/* 
		게임 플레이 시간 관련 프로퍼티들
	*/
	float LastGamePlayTime = 0.f;
	float StartGameTime = 0.f;

	/*
		미니맵을 그리기위한 컴포넌트
	*/
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	class USceneCaptureComponent2D* MinimapCapture;
	UPROPERTY(EditDefaultsOnly)
	class UTextureRenderTarget2D* MinimapRenderTarget;
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float MinimapCaptureZ = 8000.f;

	void FollowPlayerAndMinimapCapture();
	void InitMinimapCapture();
	
public:
	FORCEINLINE const ELevelType GetPrevLevelType() const { return PrevLevelType; }
	FORCEINLINE const ELevelType GetCurLevelType() const { return CurrentLevelType; }
	FORCEINLINE const FString GetCurSaveSlotName() const { return CurSaveSlotName; }
	FORCEINLINE void SetCurSaveSlotName(const FString& InSlotName) { CurSaveSlotName = InSlotName; }
};
