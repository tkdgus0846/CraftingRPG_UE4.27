
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ImpactEffect.generated.h"

class UNiagaraSystem;
class USoundCue;

UCLASS(Abstract, Blueprintable)
class MYRPG_API AImpactEffect : public AActor
{
	GENERATED_BODY()

public:
	AImpactEffect();
	virtual void BeginPlay();
	virtual void PostInitializeComponents() override;

private:
	/* Visual FX */
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	UNiagaraSystem* DefaultFX;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UNiagaraSystem* BodyDamageFX;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UNiagaraSystem* ConcreteFX;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UNiagaraSystem* DirtFX;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UNiagaraSystem* WaterFX;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UNiagaraSystem* MetalFX;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UNiagaraSystem* WoodFX;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UNiagaraSystem* GlassFX;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UNiagaraSystem* GrassFX;

	/* Sounds */
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	USoundCue* DefaultSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* BodyDamageSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* ConcreteSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* DirtSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* WaterSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* MetalSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* WoodSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* GlassSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* GrassSound;

	/* Hit ��� */
	UPROPERTY(BlueprintReadOnly, Category = Surface, meta = (AllowPrivateAccess = true))
	FHitResult SurfaceHit;

	/*
		�׽�Ʈ��. ���߿� ������ ��Ƽ���� �����Ű�� �ٲ����~~
	*/
	UPROPERTY()
	class UNiagaraComponent* DefaultNiagaraComponent;
	bool bAutoActive = true;

protected:
	UNiagaraSystem* GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const;
	USoundCue* GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const;
	void ShowEffects(EPhysicalSurface SurfaceType);

public:
	FORCEINLINE void SetHitResult(const FHitResult& HitResult) { SurfaceHit = HitResult; }
	FORCEINLINE class UNiagaraComponent* GetDefaultNiagaraComponent() { return DefaultNiagaraComponent;}
	FORCEINLINE void SetAutoActive(bool bNewState) { bAutoActive = bNewState; }
};
