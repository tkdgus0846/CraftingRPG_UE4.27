
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Components/SplineComponent.h"
#include "SplineActor.generated.h"

DECLARE_DELEGATE(FUpdateSplineDelegate);
DECLARE_DELEGATE(FFinishedSplineDelegate);

USTRUCT(BlueprintType)
struct FSplineInfo
{
	GENERATED_BODY()

	FVector PointLocation;
	ESplinePointType::Type PointType;
	bool bIsLocal;
};

UCLASS()
class MYRPG_API ASplineActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASplineActor();

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent)
	void AddPoint(int Key, const FVector& Location, ESplinePointType::Type SplineType = ESplinePointType::Curve, bool bIsLocal = false);

	UFUNCTION()
	void AddPoint_Implementation(int Key, const FVector& Location, ESplinePointType::Type SplineType = ESplinePointType::Curve, bool bIsLocal = false) {}

	UFUNCTION(BlueprintCallable)
	void BindInfoTimeline(UCurveFloat* CurveFloat, float CustomTimelineLength, bool bDestroyAfterFinished = true);

	void Play();
	const bool IsPlaying() const;
	void Stop();
	void Resume();
	void InitializePatrol();

	float GetRemainedTime();
	float GetElapsedTime();

	FUpdateSplineDelegate OnUpdateSpline;
	FFinishedSplineDelegate OnFinishedSpline;

protected:

private:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* RootSceneComponent;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Meta = (AllowPrivateAccess = true))
	USplineComponent* SplineComponent;
	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* DefaultCurveFloat = nullptr;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float TimelineLength = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bOneWay = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bLoop = false;

	bool bDestroyFinished = false;
	bool bReversing = false;
	bool bFinishedOneRound = false;
	bool bAttachToGround = false;
	
	UPROPERTY()
	FTimeline SplineTimeline;
	FVector CurrentPos = FVector::ZeroVector;
	FVector PrevPos = FVector::ZeroVector;
	FVector CurrentTangent = FVector::ZeroVector;

	UFUNCTION()
	void SplineActorDestroy();

	UFUNCTION()
	void CurveFloatFunc(float Value);

	float CurrentValue = -1.f;
	float ElapsedTime = 0.f;

public:
	FORCEINLINE FVector GetDeltaPos() const { return CurrentPos - PrevPos; }
	FORCEINLINE FVector GetCurrentPos() const { return CurrentPos; }
	FORCEINLINE FVector GetPrevPos() const { return PrevPos; }
	FORCEINLINE float GetCurrentValue() const { return CurrentValue; }
	FORCEINLINE FVector GetCurrentTangent() const { return CurrentTangent; }
	FORCEINLINE const bool IsFinishedPatrolOneRound() const { return bFinishedOneRound; }
	FORCEINLINE void SetAttachToGround(bool bState) { bAttachToGround = bState; }
	
	FVector GetNextTickPos(float TickAmount = -1.f) const;
};
