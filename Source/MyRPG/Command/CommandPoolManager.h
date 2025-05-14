
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyRPG/Interfaces/Command.h"
#include "CommandPoolManager.generated.h"

/* �ܺο��� ������ ���� */
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
	�޸�Ǯ, �̱���, �߻����丮 ������ �����.
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
