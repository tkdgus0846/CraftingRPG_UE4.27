
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FadeOutActor.generated.h"

/* 
	특정 사물이 플레이어를 가릴때 FadeOut 해주는 기능을 위한 클래스 계층이다.
	아래 계층에 MIBaseActor라던지 그런 계층 하나 더 추가해줘서 처리하면 좋을거같다.
*/
UCLASS()
class MYRPG_API AFadeOutActor : public AActor
{
	GENERATED_BODY()
	
public:
	enum EFadeState
	{
		FS_FadeIn, FS_FadeOut, FS_None
	};

	AFadeOutActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void FadeOut(class UMeshComponent* Component);
	void FadeIn(class UMeshComponent* Component);

protected:
	UFUNCTION(BlueprintPure, meta = (CommutativeAssociativeBinaryOperator = true))
	class UMeshComponent* InitMeshes(class UMeshComponent* A, class UMeshComponent* B);
	
private:
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	TArray<class UMeshComponent*> Meshes;

	TArray<TTuple<FTimerHandle, EFadeState>> FadeTimers;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	TArray<class UMaterialInstanceDynamic*> MaterialInstances;

	void FadeOutFunc();
	void FadeInFunc();

	void FadeOutAlphaMesh(int Index, float Rate);
	void FadeInAlphaMesh(int Index, float Rate);

	const float FadeOutAlphaLimit = 0.1f;
	const float FadeOutPlayRate = 2.0f;
	const float FadeInAlphaLimit = 1.f;
	const float FadeInPlayRate = 2.0f;

public:
	TArray<class UMaterialInstanceDynamic*>& GetMaterialInstances() { return MaterialInstances; }
};
