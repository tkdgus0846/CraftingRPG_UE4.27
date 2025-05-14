
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

	/** ���𰡰� ��ü ������Ʈ�� �� �� ȣ�� */
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** ���𰡰� ��ü ������Ʈ�� ���� �� ȣ�� */
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

	/* ���� ����� ��ȣ�ۿ� ������Ʈ�� ã�� ���� */
	TArray<AActor*> CurrentOverlapInteractables;

	int CurrentNearestInteractableIndex = -1;
	void CheckNearestInteractable();
	FTimerHandle CheckNearestInteractableTimer;

public:
	FORCEINLINE void ClearCurrentFacility() { CurrentFacility = nullptr; }

};
