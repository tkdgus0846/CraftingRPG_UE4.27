// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../SkillCaster.h"
#include "BarrierCaster.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API ABarrierCaster : public ASkillCaster
{
	GENERATED_BODY()
	
public:
	ABarrierCaster();

	// ASkillCaster��(��) ���� ��ӵ�
	void SkillPressed() override;
	void SkillPressing(float DeltaTime) override;
	void SkillReleased() override;

	UFUNCTION()
	void ShowDamagedEffect();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

private:
	FTimerHandle BarrierTimer, StartChangeStateTimer;
	// �ӽ÷� �踮�� Ÿ�� ����Ʈ �׽�Ʈ�� �ϴٰ� ���� Ÿ�̸�
	FTimerHandle TestTimer;

	UFUNCTION()
	void StartChangeState();

	UFUNCTION()
	void EndState();

	UPROPERTY()
	class UNiagaraSystem* BarrierHitFX;

	UPROPERTY()
	class UBoxComponent* BarrierHitComponent;

	UFUNCTION()
	void CollisionOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void CollisionOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//bool CanBlock(AActor* OtherActor);

};
