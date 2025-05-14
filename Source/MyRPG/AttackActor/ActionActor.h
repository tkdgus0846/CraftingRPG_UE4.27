// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackActor.h"
#include "MyRPG/BaseClasses/Components/ActionComponent.h"
#include "ActionActor.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API AActionActor : public AAttackActor
{
	GENERATED_BODY()

public:
	AActionActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable)
	void AddAction(const FActionInfo& ActionInfo);

	UFUNCTION(BlueprintCallable)
	void PlayAction();

	void PauseAction();
	void ResumeAction();

	void ResetAction();
	void ClearAction();

	const bool IsActionPlaying();
	void Destroy();
	

protected:

private:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	UActionComponent* ActionComponent;
};
