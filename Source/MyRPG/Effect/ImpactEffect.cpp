

#include "ImpactEffect.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "MyRPG/Types/ChannelType.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

AImpactEffect::AImpactEffect()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SetAutoDestroyWhenFinished(true);
}

void AImpactEffect::BeginPlay()
{
	Super::BeginPlay();

	UPhysicalMaterial* HitPhysMat = SurfaceHit.PhysMaterial.Get();
	
	TEnumAsByte<EPhysicalSurface> SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

	ShowEffects(SurfaceType);

	/*if (DefaultDecal.DecalMaterial)
	{
		FRotator RandomDecalRotation = SurfaceHit.ImpactNormal.Rotation();
		RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);

		UGameplayStatics::SpawnDecalAttached(DefaultDecal.DecalMaterial, FVector(1.0f, DefaultDecal.DecalSize, DefaultDecal.DecalSize),
			SurfaceHit.Component.Get(), SurfaceHit.BoneName,
			SurfaceHit.ImpactPoint, RandomDecalRotation, EAttachLocation::KeepWorldPosition,
			DefaultDecal.LifeSpan);
	}*/
}

void AImpactEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

UNiagaraSystem* AImpactEffect::GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	UNiagaraSystem* ImpactFX = nullptr;

	switch (SurfaceType)
	{
	case SURFACE_BodyDamage:	ImpactFX = BodyDamageFX; break;
	case SURFACE_Concrete:		ImpactFX = ConcreteFX; break;
	case SURFACE_Dirt:			ImpactFX = DirtFX; break;
	case SURFACE_Water:			ImpactFX = WaterFX; break;
	case SURFACE_Metal:			ImpactFX = MetalFX; break;
	case SURFACE_Wood:			ImpactFX = WoodFX; break;
	case SURFACE_Glass:			ImpactFX = GlassFX; break;
	case SURFACE_Grass:			ImpactFX = GrassFX; break;
	case SURFACE_Default:		ImpactFX = DefaultFX; break;
	}

	return ImpactFX;
}

USoundCue* AImpactEffect::GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	USoundCue* ImpactSound = nullptr;

	switch (SurfaceType)
	{
	case SURFACE_BodyDamage:	ImpactSound = BodyDamageSound; break;
	case SURFACE_Concrete:		ImpactSound = ConcreteSound; break;
	case SURFACE_Dirt:			ImpactSound = DirtSound; break;
	case SURFACE_Water:			ImpactSound = WaterSound; break;
	case SURFACE_Metal:			ImpactSound = MetalSound; break;
	case SURFACE_Wood:			ImpactSound = WoodSound; break;
	case SURFACE_Glass:			ImpactSound = GlassSound; break;
	case SURFACE_Grass:			ImpactSound = GrassSound; break;
	case SURFACE_Default:		ImpactSound = DefaultSound; break;
	}

	return ImpactSound;
}

void AImpactEffect::ShowEffects(EPhysicalSurface SurfaceType)
{
	UNiagaraSystem* ImpactFX;
	USoundCue* ImpactSound;

	// show default particles
	ImpactFX = GetImpactFX(SURFACE_Default);
	if (IsValid(ImpactFX))
	{
		DefaultNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactFX, GetActorLocation(), GetActorRotation(), ((FVector)(1.f)), true, bAutoActive);
	}

	// play default sound
	ImpactSound = GetImpactSound(SURFACE_Default);
	if (IsValid(ImpactSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (SurfaceType == SURFACE_Default) return;

	// show particles
	ImpactFX = GetImpactFX(SurfaceType);
	if (IsValid(ImpactFX))
	{
		UE_LOG(LogTemp, Warning, TEXT("ShowEffectsCalled"));
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactFX, GetActorLocation(), GetActorRotation());
	}

	// play sound
	ImpactSound = GetImpactSound(SurfaceType);
	if (IsValid(ImpactSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
}

