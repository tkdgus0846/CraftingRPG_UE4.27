
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

	/* Hit 결과 */
	UPROPERTY(BlueprintReadOnly, Category = Surface, meta = (AllowPrivateAccess = true))
	FHitResult SurfaceHit;

	/*
		테스트용. 나중에 피지컬 머티리얼 적용시키면 바꿔야함~~
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
