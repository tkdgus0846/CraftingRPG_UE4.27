#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ModifyLandscapeVolume.generated.h"

UCLASS()
class MYRPG_API AModifyLandscapeVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	AModifyLandscapeVolume();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:

private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* ModifyBound;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditInstanceOnly)
	class ALandscape* TargetLandscape;

	UPROPERTY()
	class ULandscapeInfo* LandscapeInfo;

public:
};
