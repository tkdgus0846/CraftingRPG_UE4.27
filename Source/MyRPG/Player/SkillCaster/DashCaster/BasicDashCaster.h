
#pragma once

#include "CoreMinimal.h"
#include "../SkillCaster.h"
#include "BasicDashCaster.generated.h"

UCLASS()
class MYRPG_API ABasicDashCaster : public ASkillCaster
{
	GENERATED_BODY()
	
public:
	ABasicDashCaster();

	// ASkillCaster��(��) ���� ��ӵ�
	void SkillPressed() override;
	void SkillPressing(float DeltaTime) override;
	void SkillReleased() override;

private:
	FTimerHandle Timer;
	FVector Direction;
	const float DashDuration = 0.6f;
	float CurrentDashTime = 0.f;

	UFUNCTION()
	void Dash();

	UPROPERTY()
	class UNiagaraSystem* BasicDashVFX = nullptr;
	UPROPERTY()
	class UNiagaraComponent* BasicDashVFXComponent = nullptr;
};
