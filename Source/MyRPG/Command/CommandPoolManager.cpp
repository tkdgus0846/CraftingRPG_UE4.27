
#include "CommandPoolManager.h"

UCommandPoolManager* UCommandPoolManager::Instance = nullptr;

UCommandPoolManager* UCommandPoolManager::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = NewObject<UCommandPoolManager>();
		Instance->AddToRoot();
		Instance->Initialize();
	}
	return Instance;
}

void UCommandPoolManager::Shutdown()
{
	if (IsValid(Instance))
	{
		Instance->RemoveFromRoot();
	}
	Instance = nullptr;
}

void UCommandPoolManager::ReturnCommand(ICommand* Command)
{
	if (Command && Command->IsInProgress() == false)
	{
		TScriptInterface<ICommand> RetCommand;
		RetCommand.SetObject(Cast<UObject>(Command));
		RetCommand.SetInterface(Command);

		if (IsValid(RetCommand.GetObject()))
		{
			Command->Reset();
			UClass* CommandClass = RetCommand.GetObject()->GetClass();
			GetInstance()->CommandPools.FindOrAdd(CommandClass).Add(RetCommand);
		}		
	}
}

void UCommandPoolManager::Initialize()
{
}
