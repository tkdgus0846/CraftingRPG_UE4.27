
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "MyRPG/Types/CurveType.h"
#include "MyRPG/SplineActor/SplineActor.h"
#include "ActionComponent.generated.h"

#define DEFAULT_NAME FName("Default")

USTRUCT(BlueprintType)
struct FActionInfo
{
	GENERATED_BODY()

	friend class UActionComponent;

public:
	UPROPERTY(BlueprintReadWrite)
	FRotator OriginRotation = FRotator::ZeroRotator;
	FRotator TargetRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite)
	ECurveFloat CurveType = ECurveFloat::ECF_Default;

	UPROPERTY(BlueprintReadWrite)
	bool bAttachToGround = false;

	UPROPERTY(BlueprintReadWrite)
	bool bSweep = true;

	UPROPERTY(BlueprintReadWrite)
	class UAnimMontage* AnimMontage = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FName SectionName = DEFAULT_NAME;

	UPROPERTY(BlueprintReadWrite)
	float MontagePlayRate = 1.f;

	// 값을 안받을경우 몽타주의 시간으로 액션타임이 정해지게함.
	UPROPERTY(BlueprintReadWrite)
	float ActionTime = -1.f;

	UPROPERTY(BlueprintReadWrite)
	float FirstDelay = 0.f;

	// 액션이 실행되기 몇 초 전에 애니메이션 몽타주를 미리 재생할 시간을 설정합니다. 애니메이션 자체의 시간보다 길어지면 오류가 납니다.
	UPROPERTY(BlueprintReadWrite)
	float PreActionMontagePlayTime = 0.f;

	UPROPERTY(BlueprintReadWrite)
	ETeleportType TeleportType = ETeleportType::None;

	void AddMovePoint(const FVector& Location, ESplinePointType::Type PointType = ESplinePointType::Curve, bool bIsLocal = false)
	{
		SplinePoint.Add({Location, PointType, bIsLocal});
	}

	void ClearMovePoint()
	{
		SplinePoint.Empty();
	}

	ASplineActor* GetSplineActor() const
	{
		return SplineActor;
	}

private:
	UPROPERTY()
	ASplineActor* SplineActor = nullptr;
	TArray<FSplineInfo> SplinePoint;
	bool bMontagePlayed = false;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYRPG_API UActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UActionComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// bForceAdd가 true이면 진행하던 액션을 지워버리고 새로 실행  Ret: Action Total Length
	float AddAction(const FActionInfo& ActionInfoParam, bool bForceAdd = false);

	void AddMovePointToAction(FActionInfo& ActionInfo, const FVector& Location, ESplinePointType::Type PointType = ESplinePointType::Curve, bool bIsLocal = false);

	void PlayAction();
	void PauseAction();
	void ResumeAction();

	// 액션을 맨처음 인덱스로 되돌린다.
	void ResetAction();

	// 액션 리스트들을 아예 비워버린다
	void ClearAction();

	virtual void DestroyComponent(bool bPromoteChildren = false) override;
	void EnableActionStop();
	

protected:

private:
	UPROPERTY()
	class ACreatureCharacter* Character = nullptr;
	UPROPERTY()
	class AActor* MyActor = nullptr;

	UPROPERTY()
	TArray<FActionInfo> ActionQueue;
	int CurIndex = 0;
	FVector OriginPos = FVector::ZeroVector;

	UFUNCTION()
	void NextAction();
	UFUNCTION()
	void ActionFunc();

	
	void PlayFromStart(); 
	FActionInfo& GetCurActionInfo();
	FActionInfo* GetNextActionInfo();
	const bool IsCurIndexValid();

	// Ret: 애니메이션 플레이에 성공했는지 실패했는지
	const bool PlayActionAnimation(FActionInfo& ActionInfo);

	bool bMontagePlayed = false;
	bool bFirstTick = true;

	/* 한번이라도 블록된다면 그에 대한 처리를 멈춘다. */
	bool bEvenOneTickBlocked = false;

	/* 땅에 붙어서 행하는 액션의 경우에 어느정도 높이까지 올라갈수있는지*/
	const float MaxActionStepHeight = 50.f;
	const float MinActionStepHeight = 20.f;

	FTimerHandle FirstDelayTimer;

	UPROPERTY()
	TSubclassOf<ASplineActor> SplineActorClass;

	void ClearActionQueue();

	/* 액션이 실행중일때 키 입력을 막아뒀는데 이 입력을 받을수도 있게 하기위함*/
	bool bEnableActionStop = false;

	void AttachToGround(FVector& Pos, bool& bSweep);
	

public:
	const bool IsActionPlaying();
};
