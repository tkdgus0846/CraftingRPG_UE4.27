
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyRPG/Data/DamageInfo.h"
#include "AttackActor.generated.h"

#define SettingDamageInfo(...) FDamageInfo DamageInfo = FDamageInfo(__VA_ARGS__)				
#define AddDamageCommand(Type, ...) DamageInfo.AddCommand(UCommandPoolManager::GetCommand<Type>(__VA_ARGS__))

#define Damage(Target) Target->Damaged(&DamageInfo)

															

UENUM(BlueprintType)
enum class EAttackCollisionMode : uint8
{
	ACM_BOX		UMETA(DisplayName = "BoxCollision"),
	ACM_SPHERE	UMETA(DisplayName = "SphereCollision"),
	ACM_BOTH	UMETA(DisplayName = "BothCollision"),
};

UCLASS()
class MYRPG_API AAttackActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAttackActor();

	virtual void BeginPlay() override;	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;

	void SetCollsionEnabled(ECollisionEnabled::Type ColType);

	void EmptyOverlapFinishedCreatures();

	void SetBoxSize(const FVector& NewSize);
	FVector GetBoxSize();
	void SetSphereRadius(const float NewRadius);
	float GetSphereRadius();

	void SetDamage(int Damage);
	int GetDamage() const;

	void Initialize(const FName& InitAttachSocketName);

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnOverlapBegin(AActor* OtherActor);
	UFUNCTION()
	virtual void OnOverlapBegin_Implementation(AActor* OtherActor);

	UFUNCTION(BlueprintNativeEvent)
	void OnOverlapEnd(AActor* OtherActor);
	UFUNCTION()
	virtual void OnOverlapEnd_Implementation(AActor * OtherActor);

	UFUNCTION(BlueprintNativeEvent)
	void OnOverlapping(float DeltaTime);
	UFUNCTION()
	virtual void OnOverlapping_Implementation(float DeltaTime);

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> OverlappingCreatures;
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> OverlapFinishedCreatures;

	void RemoveDeadCreatures();

	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* RootScene;

	/*
		충돌 감지 관련 변수들
	*/
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* CollisionSphere;

	UPROPERTY(EditDefaultsOnly)
	EAttackCollisionMode AttackCollisionMode = EAttackCollisionMode::ACM_BOX;

	UPROPERTY(EditDefaultsOnly)
	bool bAttachToOwner = true;

	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bAttachToOwner"))
	bool bRotateInherit = true;

	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bAttachToOwner"))
	bool bLocationInherit = true;

	UPROPERTY(EditDefaultsOnly)
	int Damage = 10;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AImpactEffect> ImpactEffectClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AImpactEffect> SpawnEffectClass;

	FORCEINLINE void SetGenerateOverlapEventOnce(bool bNewState) { bGenerateOverlapEventOnce = bNewState; }

private:
	UFUNCTION()
	void SetLocationToOriginal(class UShapeComponent* Comp, const FVector& OriginalLocation);
	void UpdateOverlapOneTick();

	UPROPERTY(EditDefaultsOnly)
	bool bGenerateOverlapEventOnce = true;

	UFUNCTION()
	void CollisionOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void CollisionOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/* Owner 기준이 아닌 일정 소켓에 붙여서 처리해야할 경우.*/
	UPROPERTY()
	FName AttachSocketName = NAME_None;

	void SetCollsionEnabled(class UShapeComponent* Comp, ECollisionEnabled::Type ColType);

public:
};
