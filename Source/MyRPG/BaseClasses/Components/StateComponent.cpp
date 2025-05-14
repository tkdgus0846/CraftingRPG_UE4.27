
#include "StateComponent.h"
#include "MyRPG/Effect/StateEffect/StateEffect.h"
#include <Kismet/GameplayStatics.h>

TMap<EStateMaterialType, FName> UStateComponent::StateNameMap;

UStateComponent::UStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	static ConstructorHelpers::FClassFinder<AStateEffect> IceEffectClass(TEXT("/Game/Blueprints/FX/StateEffect/BP_IceStateEffect"));
	if (IceEffectClass.Succeeded())
	{
		if (StateEffectClassMap.Find(EStateMaterialType::ESMT_Ice) == nullptr)
			StateEffectClassMap.Add(EStateMaterialType::ESMT_Ice, IceEffectClass.Class);
	}
}

void UStateComponent::ChangeState(EStateMaterialType State, float Duration)
{
	bool bChangeStateSucceed = SpawnStateEffect(State, Duration);
	CurState = (bChangeStateSucceed) ? State : PrevState;

	StateChangedDelegate.Broadcast(State, Duration);


	PrevState = CurState;
}

void UStateComponent::ResetState()
{
	AStateEffect** FindCurStateEffect = StateEffectMap.Find(CurState);
	if (FindCurStateEffect == nullptr)
	{
		return;
	}

	StateChangedDelegate.Broadcast(EStateMaterialType::ESMT_None, -1.f);
	(*FindCurStateEffect)->Stop();
}

bool UStateComponent::SpawnStateEffect(EStateMaterialType State, float Duration)
{
	AStateEffect** FindCurStateEffect = StateEffectMap.Find(State);

	if (FindCurStateEffect && State == PrevState)
	{
		(*FindCurStateEffect)->Play();
	}
	else if (FindCurStateEffect)
	{
		AStateEffect** FindPrevStateEffect = StateEffectMap.Find(PrevState);
		if (FindPrevStateEffect == nullptr)
		{
			return false;
		}
		(*FindPrevStateEffect)->Stop();
		(*FindCurStateEffect)->Play();
	}
	else if (FindCurStateEffect == nullptr)
	{
		TSubclassOf<AStateEffect>* FindEffectClass = StateEffectClassMap.Find(State);
		if (FindEffectClass == nullptr)
		{
			return false;
		}

		AStateEffect* NewStateEffect = GetWorld()->SpawnActorDeferred<AStateEffect>(*FindEffectClass, FTransform(GetOwner()->GetActorLocation()), GetOwner());
		if (IsValid(NewStateEffect) == false)
		{
			return false;
		}

		NewStateEffect->Initialize(GetStateName(State), Duration);

		UGameplayStatics::FinishSpawningActor(NewStateEffect, FTransform(GetOwner()->GetActorLocation()));

		StateEffectMap.Add(TTuple<EStateMaterialType, AStateEffect*>(State, NewStateEffect));
	}

	return true;
}


void UStateComponent::BeginPlay()
{
	Super::BeginPlay();

	if (StateNameMap.Num() == 0)
	{
		StateNameMap.Add(EStateMaterialType::ESMT_Ice, "IceState");
	}
	
}

void UStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

const FName& UStateComponent::GetStateName(EStateMaterialType State)
{
	FName* FindName = StateNameMap.Find(State);
	if (FindName == nullptr) FName("NotFound");
	return *FindName;
}



