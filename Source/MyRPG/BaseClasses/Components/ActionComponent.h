
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

	// ���� �ȹ������ ��Ÿ���� �ð����� �׼�Ÿ���� ����������.
	UPROPERTY(BlueprintReadWrite)
	float ActionTime = -1.f;

	UPROPERTY(BlueprintReadWrite)
	float FirstDelay = 0.f;

	// �׼��� ����Ǳ� �� �� ���� �ִϸ��̼� ��Ÿ�ָ� �̸� ����� �ð��� �����մϴ�. �ִϸ��̼� ��ü�� �ð����� ������� ������ ���ϴ�.
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

	// bForceAdd�� true�̸� �����ϴ� �׼��� ���������� ���� ����  Ret: Action Total Length
	float AddAction(const FActionInfo& ActionInfoParam, bool bForceAdd = false);

	void AddMovePointToAction(FActionInfo& ActionInfo, const FVector& Location, ESplinePointType::Type PointType = ESplinePointType::Curve, bool bIsLocal = false);

	void PlayAction();
	void PauseAction();
	void ResumeAction();

	// �׼��� ��ó�� �ε����� �ǵ�����.
	void ResetAction();

	// �׼� ����Ʈ���� �ƿ� ���������
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

	// Ret: �ִϸ��̼� �÷��̿� �����ߴ��� �����ߴ���
	const bool PlayActionAnimation(FActionInfo& ActionInfo);

	bool bMontagePlayed = false;
	bool bFirstTick = true;

	/* �ѹ��̶� ��ϵȴٸ� �׿� ���� ó���� �����. */
	bool bEvenOneTickBlocked = false;

	/* ���� �پ ���ϴ� �׼��� ��쿡 ������� ���̱��� �ö󰥼��ִ���*/
	const float MaxActionStepHeight = 50.f;
	const float MinActionStepHeight = 20.f;

	FTimerHandle FirstDelayTimer;

	UPROPERTY()
	TSubclassOf<ASplineActor> SplineActorClass;

	void ClearActionQueue();

	/* �׼��� �������϶� Ű �Է��� ���Ƶ״µ� �� �Է��� �������� �ְ� �ϱ�����*/
	bool bEnableActionStop = false;

	void AttachToGround(FVector& Pos, bool& bSweep);
	

public:
	const bool IsActionPlaying();
};
