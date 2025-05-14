
#include "ScheduledCommand.h"
#include "CommandPoolManager.h"

void UScheduledCommand::Execute(UObject* WorldContext)
{
	if (IsValid(WorldContext) == false || IsValid(CurWorld) == false)
	{
		return;
	}
	CurWorld->GetTimerManager().SetTimer(TimerHandle, TimerDele, Rate, bLoop, FirstDelay);
	CurWorld->GetTimerManager().SetTimer(ClearTimerHandle, this, &UScheduledCommand::Clear, Duration);
}

void UScheduledCommand::Reset()
{
	TimerDele.Unbind();
	if (IsValid(CurWorld))
	{
		CurWorld->GetTimerManager().ClearTimer(TimerHandle);
	}
	CurWorld = nullptr;
}

const bool UScheduledCommand::IsInProgress()
{
	if (IsValid(CurWorld) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UScheduledCommand�� �ڽĿ��� World������ ������Ʈ ���� �ʾҽ��ϴ�."));
		return false;
	}
	return CurWorld->GetTimerManager().IsTimerActive(TimerHandle);
}

void UScheduledCommand::Clear()
{
	if (IsValid(CurWorld))
	{
		CurWorld->GetTimerManager().ClearTimer(TimerHandle);
		CurWorld->GetTimerManager().ClearTimer(ClearTimerHandle);
	}
	UCommandPoolManager::ReturnCommand(this);
}