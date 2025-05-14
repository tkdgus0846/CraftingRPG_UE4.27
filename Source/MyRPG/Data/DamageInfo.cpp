// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageInfo.h"
#include "MyRPG/Interfaces/Command.h"

void FDamageInfo::AddCommand(ICommand* Command)
{
	FCommandArray& CommandArray = DamageActionCommands.FindOrAdd(Command->GetTag());
	CommandArray.Add(Command);

	CommandArray.Sort([](ICommand& A, ICommand& B)
		{
			return A.GetPriority() > B.GetPriority();
		}
	);
}
