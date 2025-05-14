
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MyRPG/Types/WeaponType.h"
#include "ItemDataTable.generated.h"

USTRUCT(BlueprintType)
struct MYRPG_API FItemDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* PreviewTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ItemMaxStackCount = 1;
};

USTRUCT(BlueprintType)
struct MYRPG_API FConsumableDataTable : public FItemDataTable
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* ConsumeSound;
};

USTRUCT(BlueprintType)
struct MYRPG_API FEquipmentDataTable : public FItemDataTable
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMesh* Mesh;
};

USTRUCT(BlueprintType)
struct MYRPG_API FDamageRange
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MinDamage = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxDamage = 1;
};

USTRUCT(BlueprintType)
struct MYRPG_API FWeaponDataTable : public FEquipmentDataTable
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDamageRange AttackDamage;
};

USTRUCT(BlueprintType)
struct MYRPG_API FDropCountRange
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DropCountMin = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DropCountMax = 1;
};

USTRUCT(BlueprintType)
struct MYRPG_API FDropItemInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AItem> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDropCountRange DropCountRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DropProbability = 1.f;
};

USTRUCT(BlueprintType)
struct MYRPG_API FRecipeInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AItem> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RequireNum;
};

USTRUCT(BlueprintType)
struct MYRPG_API FRecipeDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AItem> ResultItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CraftDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRecipeInfo> Recipe;
};

USTRUCT(BlueprintType)
struct MYRPG_API FDropItemDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDropItemInfo> DropItems;
};
