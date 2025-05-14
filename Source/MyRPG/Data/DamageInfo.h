// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyRPG/Types/DamageActionCommandTag.h"
#include "DamageInfo.generated.h"

/* 외부에서 접근을 금지 */
USTRUCT(BlueprintType)
struct FCommandArray
{
	GENERATED_USTRUCT_BODY()

public:
	FCommandArray() {}

	void Add(class ICommand* Command)
	{
		CommandArray.Add(Command);
	}

	template <class PREDICATE_CLASS>
	void Sort(const PREDICATE_CLASS& Predicate)
	{
		CommandArray.Sort(Predicate);
	}

	TArray<class ICommand*>& Get()
	{
		return CommandArray;
	}


private:
	TArray<class ICommand*> CommandArray;
};

USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

public:
	FDamageInfo() { }
	FDamageInfo(const FVector& NewHitLocation) : HitLocation(NewHitLocation) {}
	FDamageInfo(class AActor* NewDamageCauser) : DamageCauser(NewDamageCauser) {}
	FDamageInfo(const FVector& NewHitLocation, class AActor* NewDamageCauser, class AController* NewEventInstigator) : HitLocation(NewHitLocation), DamageCauser(NewDamageCauser), EventInstigator(NewEventInstigator) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector HitLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AActor* DamageCauser = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AController* EventInstigator = nullptr;

	void AddCommand(class ICommand* Command);

	TSortedMap<EDamageActionCommandTag, FCommandArray>& GetDamageActionCommands()
	{
		return DamageActionCommands;
	}

private:
	TSortedMap<EDamageActionCommandTag, FCommandArray> DamageActionCommands;
};
