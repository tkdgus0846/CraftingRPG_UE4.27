
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyRPG/Interfaces/Command.h"
#include "CommandPoolManager.generated.h"

/* 외부에서 접근을 금지 */
USTRUCT(BlueprintType)
struct FCommandPool
{
	GENERATED_USTRUCT_BODY()

public:
	FCommandPool() {}

	int32 Num() 
	{
		return CommandPool.Num();
	}

	template<typename T>
	T* Pop()
	{
		return Cast<T>(CommandPool.Pop().GetObject());
	}

	void Add(TScriptInterface<ICommand> Command)
	{
		CommandPool.Add(Command);
	}


	UPROPERTY()
	TArray<TScriptInterface<ICommand>> CommandPool;
};

/*
	메모리풀, 싱글톤, 추상팩토리 개념을 사용함.
*/
UCLASS()
class MYRPG_API UCommandPoolManager : public UObject
{
	GENERATED_BODY()

public:
	static void Shutdown();

	template<typename T, typename... ArgTypes>
	static T* GetCommand(ArgTypes&&... Args)
	{
		UClass* CommandClass = T::StaticClass();
		FCommandPool& Pool = GetInstance()->CommandPools.FindOrAdd(CommandClass);

		T* Command = nullptr;

		if (Pool.Num() > 0)
		{
			Command = Pool.Pop<T>();
		}
		else
		{
			Command = NewObject<T>(GetInstance());
		}

		Command->Initialize(Forward<ArgTypes>(Args)...);
		return Command;
	}

	static void ReturnCommand(ICommand* Command);

private:
	UCommandPoolManager() {}
	static UCommandPoolManager* Instance;
	static UCommandPoolManager* GetInstance();

	void Initialize();

	UPROPERTY()
	TMap<UClass*, FCommandPool> CommandPools;
	
};
