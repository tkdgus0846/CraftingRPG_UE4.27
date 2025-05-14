
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlayerSave.generated.h"

USTRUCT()
struct FItemSaveData
{
	GENERATED_USTRUCT_BODY()
	FItemSaveData() {}
	FItemSaveData(TSubclassOf<class AItem> InItemClass, uint32 InItemNum)
	{
		ItemClass = InItemClass;
		ItemNum = InItemNum;
	}

	UPROPERTY()
	TSubclassOf<class AItem> ItemClass = nullptr;

	UPROPERTY()
	uint32 ItemNum = 0;
};

UCLASS()
class MYRPG_API UPlayerSave : public USaveGame
{
	GENERATED_BODY()

public:
	void SaveGame(class APlayerCharacter* Player, const FString SlotName, float GamePlayTime);

	UPROPERTY()
	int32 HP;

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	uint8 LastLevelType;

	UPROPERTY()
	TArray<FItemSaveData> ItemData;

	UPROPERTY()
	float LastPlayGameTime;

	UPROPERTY()
	float LastInterpZoomLength;

	UPROPERTY()
	FRotator LastSpringArmRelativeRotation;
};
