
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyRPG/Types/StateMaterialType.h"
#include "StateComponent.generated.h"

class AStateEffect;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStateChangedDelegate, EStateMaterialType, StateType, float, Duration);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYRPG_API UStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStateComponent();

	UFUNCTION(BlueprintCallable)
	void ChangeState(EStateMaterialType State, float Duration);

	UFUNCTION()
	void ResetState();

	FStateChangedDelegate StateChangedDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	EStateMaterialType CurState;
	EStateMaterialType PrevState;

	UPROPERTY(EditDefaultsOnly)
	TMap<EStateMaterialType, TSubclassOf<AStateEffect>> StateEffectClassMap;
	UPROPERTY()
	TMap<EStateMaterialType, AStateEffect*> StateEffectMap;

	bool SpawnStateEffect(EStateMaterialType State, float Duration);

	static TMap<EStateMaterialType, FName> StateNameMap;
	static const FName& GetStateName(EStateMaterialType State);
};
