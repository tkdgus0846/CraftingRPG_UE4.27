
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyRPG/Interfaces/Interactable.h"
#include "MyRPG/Types/LevelType.h"
#include "WarpPortal.generated.h"


/*
	GameLevel에만 배치하기
*/
UCLASS()
class MYRPG_API AWarpPortal : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AWarpPortal();

	virtual void OnConstruction(const FTransform& Transform);
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation() override;
	virtual void SetInteractWidgetVisibility_Implementation(bool bVisibility) override;

protected:

private:
	UPROPERTY(VisibleDefaultsOnly)
	class UCapsuleComponent* WarpTriggerComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UWidgetComponent* InteractWidgetComponent;

	UPROPERTY(EditInstanceOnly, Category = "Default")
	ELevelType LevelType;
	UPROPERTY(EditInstanceOnly, Category = "Default")
	TWeakObjectPtr<AWarpPortal> SymmetricPortal = nullptr;

	FVector WarpPoint;
	FTimerHandle WarpTimer;

	UFUNCTION()
	void WarpPlayer();

};
