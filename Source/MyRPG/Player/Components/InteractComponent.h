
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractComponent.generated.h"

class IInteractable;
class AActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYRPG_API UInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractComponent();

	virtual void BeginPlay() override;

	/** 무언가가 구체 컴포넌트에 들어설 때 호출 */
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** 무언가가 구체 컴포넌트를 나설 때 호출 */
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void Interact();
	void RemoveOverlapActor(AActor* OverlapActor);
	void AddOverlapActor(AActor* OverlapActor);

	bool IsAnyOverlapInteractables();

	void FinishFacilityInteract();
	
protected:


private:
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter = nullptr;
	UPROPERTY()
	TWeakObjectPtr<class AFacility> CurrentFacility = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* InteractMontage = nullptr;

	/* 가장 가까운 상호작용 오브젝트를 찾기 위함 */
	TArray<AActor*> CurrentOverlapInteractables;

	int CurrentNearestInteractableIndex = -1;
	void CheckNearestInteractable();
	FTimerHandle CheckNearestInteractableTimer;

public:
	FORCEINLINE void ClearCurrentFacility() { CurrentFacility = nullptr; }

};
