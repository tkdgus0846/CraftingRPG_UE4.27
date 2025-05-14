
#include "HolyAngel.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyRPG/GameInstance/MyGameInstance.h"
#include "DrawDebugHelpers.h"
#include "MyRPG/Enemy/Boss/PaladinBoss.h"
#include "MyRPG/AttackActor/EnemyAttack/HolyAngel/HolyAngelProjectileAttack.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include <MyRPG/AttackActor/EnemyAttack/HolyAngel/HolyAngelMeteorAttack.h>

AHolyAngel::AHolyAngel()
{
	ShowHpBar(false);
	SetIsUseHPBar(false);
	SetIgnoreDamage(true);

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	GetCharacterMovement()->GravityScale = 0.f;

	MeteorAttackEachTimer.SetNum(SpawnMeteorNum);
}

void AHolyAngel::SetAngelPoints(TArray<USceneComponent*>& InPoints)
{
	for (auto Point : InPoints)
	{
		AngelPoints.Add(Point);

		/*DrawDebugSphere(GetWorld(), Point->GetComponentLocation(), 50.f, 32, FColor::Green, true);*/
	}
}

void AHolyAngel::BeginPlay()
{
	Super::BeginPlay();

	PlayAnimMontage(SpawnMontage);

	FadeOut(1.5f, 3.f);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &AHolyAngel::CanSkillToTrue, 0.0001f, false, GetCanSkillInterval());

	HideHpBar();
	
}

void AHolyAngel::SetDead()
{
	Super::SetDead();

	GetWorldTimerManager().ClearAllTimersForObject(this);
	SetActorLocation(FVector(0.f, 0.f, -999999.f));
	ClearAction();
	SetLifeSpan(1.f);
}

bool AHolyAngel::IsSkillAvailable(int SkillNum)
{
	return true;
}

bool AHolyAngel::Skill(int SkillNum)
{
	if (GetWorldTimerManager().IsTimerActive(ProjectileAttackTimer))
	{
		bCanProjectileAttack = true;
		return false;
	}


	if (Super::Skill(SkillNum) == false)
	{
		return false;
	}

	switch (SkillNum)
	{
	case 0:
		Dash();
		break;
	case 1:
		Meteor();
		break;
	default:
		break;
	}

	return true;
}

void AHolyAngel::ActionNotify_Implementation(int ActionNum, float Duration)
{
	switch (ActionNum)
	{
	case 10:
		DashAction(Duration);
		break;
	case 20:
		MeteorAction(Duration);
		break;
	default:
		break;
	}
}

void AHolyAngel::AnimEventNotify_Implementation(int AnimNum)
{
	switch (AnimNum)
	{
	case 10:
		FadeOut(1.5f, 1.f);
		ReserveSkillFinish(3.f);
		break;
	// Meteor Attack Start
	case 20:
		SpawnMeteorAttack();
		break;
	// Meteor Montage Finished
	case 21:
		SetInterpRotationForcibly(false);
		SetAutoInterpRotationToTarget(false);
		FadeOut(1.5f, 1.f);
		ReserveSkillFinish(3.f);
		break;
	case 74:
		SpawnProjectileAttack();
		break;
	case 75:
		FadeOut(1.5f);
		break;
	default:
		break;
	}
}

FProjectileAttackDelegate& AHolyAngel::PlayProjectileAttack()
{
	// 여기서 바로하지말고 타이머를 만들어서 현재 스킬이 끝나는 타이밍 체크,
	// 그동안 팔라딘은 대기하고 있어야함
	FTimerDelegate TimerDele;
	TimerDele.BindLambda(
		[this]()
		{
			if (bCanProjectileAttack)
			{
				FadeIn(1.f);
				StopBehaviorTree(4.f + SpawnProjectileMontage->GetPlayLength());
				ReserveMontage(SpawnProjectileMontage, "Default", 1.f);

				// 포인트랑 팔라딘 선상에 있는 곳에 위치시키고 팔라딘 방향으로 rotate
				FVector Direction = (GetCurrentTargetLocation() - Paladin->GetActorLocation()).GetSafeNormal2D();
				FVector Point = Paladin->GetActorLocation() + Direction * 1500.f + FVector(0.f, 0.f, -60.f);
				
				/*DrawDebugLine(GetWorld(), Paladin->GetActorLocation(), Point, FColor::Red, true);*/
				//DrawDebugSphere(GetWorld(), Point, 50.f, 32, FColor::Red, true);

				SetInterpRotationForcibly(false);
				SetAutoInterpRotationToTarget(false);

				SetActorLocation(Point);
				SetActorRotation((Paladin->GetActorLocation() - Point).GetSafeNormal2D().Rotation());

				bCanProjectileAttack = false;
				GetWorldTimerManager().ClearTimer(ProjectileAttackTimer);
			}
		}
	);

	GetWorldTimerManager().SetTimer(ProjectileAttackTimer, TimerDele, 0.01f, true);

	return ProjectileAttackDelegate;
}

void AHolyAngel::SpawnProjectileAttack()
{
	if (Paladin.IsValid() == false || IsValid(GetMesh()) == false)
	{
		return;
	}

	FVector StartPosition = GetMesh()->GetSocketLocation("WeaponSocket");

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	AHolyAngelProjectileAttack* ProjectileAttackActor = ProjectileAttackActor = GetWorld()->SpawnActor<AHolyAngelProjectileAttack>(ProjectileAttackClass, FTransform(StartPosition), SpawnParams);

	if (IsValid(ProjectileAttackActor) == false)
	{
		return;
	}

	const float Distance = FVector::Dist2D(Paladin->GetActorLocation(), StartPosition);
	FVector Direction = (Paladin->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();


	FActionInfo ActionInfo1;
	ActionInfo1.ActionTime = 1.4f;

	ActionInfo1.AddMovePoint(StartPosition);
	ActionInfo1.AddMovePoint(StartPosition + Direction * Distance);

	ProjectileAttackActor->AddAction(ActionInfo1);
	ProjectileAttackActor->PlayAction();
	ProjectileAttackActor->SetLifeSpan(ActionInfo1.ActionTime);

	ProjectileAttackDelegate.Broadcast();
}

void AHolyAngel::FadeOut(float FadeOutTime, float Delay)
{
	UMyGameInstance* GameInstance = GetGameInstance<UMyGameInstance>();
	if (IsValid(GameInstance) == false || IsValid(FadeFXMaterial) == false)
	{
		return;
	}

	TArray<FPlayMaterialParameterAnimationInfo> FadeOutAlphaAnim;
	FadeOutAlphaAnim.Add
	(
		FPlayMaterialParameterAnimationInfo
		(
			"FadeOutAlpha",
			ECurveValueType::ECVT_Scalar,
			0.2f,
			GameInstance->GetCurveFloat(ECurveFloat::ECF_Default),
			-1,
			1.f,
			true
		)
	);

	TArray<FPlayMaterialParameterAnimationInfo> GlowAnim;

	for (int i = 0; i < 5; i++)
	{
		GlowAnim.Add
		(
			FPlayMaterialParameterAnimationInfo
			(
				"FadeOutAlpha",
				ECurveValueType::ECVT_Scalar,
				FadeOutTime,
				GameInstance->GetCurveFloat(ECurveFloat::ECF_Sine),
				i,
				1.f,
				false,
				0.f,
				((i != 2) ? FadeFXMaterial : FadeWingsFXMaterial)
			)
		);
	}

	if (Delay != -1.f)
	{
		FTimerDelegate TimerDele;
		TimerDele.BindLambda([FadeOutAlphaAnim, GlowAnim, this]() 
			{
				PlayVFX("AngelFadeOut", true, FadeOutAlphaAnim); 
				PlayVFX("AngelFadeOutFX", false, GlowAnim);
			});

		GetWorldTimerManager().SetTimer(FadeTimer, TimerDele, Delay, false);
	}
	else
	{
		PlayVFX("AngelFadeOut", true, FadeOutAlphaAnim);
		PlayVFX("AngelFadeOutFX", false, GlowAnim);
	}
	
}

void AHolyAngel::FadeIn(float FadeInTime, float Delay)
{
	UMyGameInstance* GameInstance = GetGameInstance<UMyGameInstance>();
	if (IsValid(GameInstance) == false || IsValid(FadeFXMaterial) == false)
	{
		return;
	}

	TArray<FPlayMaterialParameterAnimationInfo> FadeOutAlphaAnim;
	FadeOutAlphaAnim.Add
	(
		FPlayMaterialParameterAnimationInfo
		(
			"FadeOutAlpha",
			ECurveValueType::ECVT_Scalar,
			0.5f,
			GameInstance->GetCurveFloat(ECurveFloat::ECF_Default),
			-1,
			1.f,
			false
		)
	);

	TArray<FPlayMaterialParameterAnimationInfo> GlowAnim;

	for (int i = 0; i < 5; i++)
	{
		GlowAnim.Add
		(
			FPlayMaterialParameterAnimationInfo
			(
				"FadeOutAlpha",
				ECurveValueType::ECVT_Scalar,
				FadeInTime,
				GameInstance->GetCurveFloat(ECurveFloat::ECF_Default),
				i,
				1.f,
				true,
				0.f,
				((i != 2) ? FadeFXMaterial : FadeWingsFXMaterial)
			)
		);
	}

	if (Delay != -1.f)
	{
		FTimerDelegate TimerDele;
		TimerDele.BindLambda([FadeOutAlphaAnim, GlowAnim, this]()
			{
				PlayVFX("AngelFadeOut", true, FadeOutAlphaAnim);
				PlayVFX("AngelFadeOutFX", false, GlowAnim);
			});

		GetWorldTimerManager().SetTimer(FadeTimer, TimerDele, Delay, false);
	}
	else
	{
		PlayVFX("AngelFadeOut", true, FadeOutAlphaAnim);
		PlayVFX("AngelFadeOutFX", false, GlowAnim);
	}
}

FVector AHolyAngel::GetRandomAngelPoint()
{
	if (AngelPoints.Num() == 0)
	{
		return FVector::ZeroVector;
	}

	return AngelPoints[FMath::Rand() % AngelPoints.Num()]->GetComponentLocation();
}

FVector AHolyAngel::GetToPlayerDirection(const FVector& PointLocation, bool bOnlyXY)
{
	if (bOnlyXY)
	{
		return (GetCurrentTargetLocation() - PointLocation).GetSafeNormal2D();
	}
	else
	{
		return (GetCurrentTargetLocation() - PointLocation).GetSafeNormal();
	}
}

const FVector AHolyAngel::RandomPosInArena()
{
	float RandX = FMath::FRandRange(-1.f, 1.f) * 1100.f;
	float RandY = FMath::FRandRange(-1.f, 1.f) * 1100.f;

	return CenterPos + FVector(RandX, RandY, 0.f);
}

void AHolyAngel::Dash()
{
	FadeIn(1.f);

	FVector SpawnPos;
	float Distance;

	while (true)
	{
		SpawnPos = GetCurrentTargetLocation() + FVector(1.f, 0.f, 0.f).RotateAngleAxis(FMath::FRandRange(0.f, 360.f), FVector::UpVector) * DashSpawnRadius;
		Distance = FVector::Dist2D(SpawnPos, CenterPos);

		if (Distance < 1300.f)
		{
			break;
		}
	}

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, SpawnPos, SpawnPos - FVector(0.f, 0.f, 300.f), ECC_Visibility);

	SetActorLocation(HitResult.Location + FVector(0.f, 0.f, 110.f));
	SetActorRotation(GetToPlayerDirection(GetActorLocation()).Rotation());
	SetRotationCurrentToInterp();
	
	PlaySkillMontage(SkillSectionName(0), 2.f);

	SetInterpRotationForcibly(true);
	SetAutoInterpRotationToTarget(true);
}

void AHolyAngel::DashAction(float Duration)
{
	SetInterpRotationForcibly(false);
	SetAutoInterpRotationToTarget(false);

	FActionInfo ActionInfo;

	ActionInfo.ActionTime = Duration;
	ActionInfo.CurveType = ECurveFloat::ECF_Default;
	
	ActionInfo.AddMovePoint(GetActorLocation());
	ActionInfo.AddMovePoint(GetActorLocation() + GetToPlayerDirection(GetActorLocation()) * DashDistance);

	AddAction(ActionInfo);
	PlayAction();
}

void AHolyAngel::SetPaladin(APaladinBoss* InPaladin)
{
	Paladin = InPaladin;
}

void AHolyAngel::Meteor()
{
	FadeIn(1.f);

	PlaySkillMontage(SkillSectionName(1), 0.5f);

	if (Paladin.IsValid())
	{
		float RandAngle = FMath::FRandRange(0.f, 360.f);
		float Distance = 500.f;
		FVector RandVector = FVector(FMath::Cos(FMath::DegreesToRadians(RandAngle)) * Distance, FMath::Sin(FMath::DegreesToRadians(RandAngle)) * Distance, 0.f);

		SetActorLocation(CenterPos + RandVector - FVector(0.f,0.f,30.f));
		SetActorRotation(GetToPlayerDirection(GetActorLocation()).Rotation());
	}
		
}

void AHolyAngel::SpawnMeteorAttack()
{
	SetRotationCurrentToInterp();
	SetInterpRotationForcibly(true);
	SetAutoInterpRotationToTarget(true);

	FTimerDelegate TimerDele;
	TimerDele.BindUFunction(this, "SpawnMeteorAttackTimerFunc", 0);

	GetWorldTimerManager().SetTimer(SpawnMeteorAttackTimer, TimerDele, 0.05f, false);
}

void AHolyAngel::SpawnMeteorAttackTimerFunc(int Count)
{
	if (Count == SpawnMeteorNum)
	{
		GetWorldTimerManager().ClearTimer(SpawnMeteorAttackTimer);
		PlaySkillMontage(SkillSectionName(1, "Finished"));
		return;
	}

	UMyGameInstance* GameInstance = GetGameInstance<UMyGameInstance>();
	if (IsValid(GameInstance) == false)
	{
		return;
	}

	FHitResult HitResult;
	FVector TraceStartPos = RandomPosInArena();
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartPos, TraceStartPos - FVector(0.f, 0.f, 300.f), ECC_Visibility);


	ADecalActor* Decal = GameInstance->SpawnDecal(GetDecalClass(0), FTransform(HitResult.Location));
	if (IsValid(Decal) == false)
	{
		return;
	}

	TArray<FPlayMaterialParameterAnimationInfo> Info;
	Info.Add(FPlayMaterialParameterAnimationInfo("FadeOutAlpha", ECurveValueType::ECVT_Scalar, 0.3f, GameInstance->GetCurveFloat(ECurveFloat::ECF_Default)));

	GameInstance->Play_MaterialParamAnimation("FadeOut1", Decal->GetDecal(), Info);

	// 타이머로 진행
	FTimerDelegate RadiusTimerDele;
	RadiusTimerDele.BindLambda
	(	
		[HitResult, this, GameInstance, Count, Decal]()
		{
			TArray<FPlayMaterialParameterAnimationInfo> Info;
			Info.Add(FPlayMaterialParameterAnimationInfo("Radius", ECurveValueType::ECVT_Scalar, 1.f, GameInstance->GetCurveFloat(ECurveFloat::ECF_Default)));

			GameInstance->Play_MaterialParamAnimation("Radius", Decal->GetDecal(), Info);

			FTimerDelegate TimerDele = FTimerDelegate::CreateLambda
			(
				[HitResult, this, GameInstance, Count, Decal]()
				{
					TArray<FPlayMaterialParameterAnimationInfo> Info;
					Info.Add(FPlayMaterialParameterAnimationInfo("FadeOutAlpha", ECurveValueType::ECVT_Scalar, 0.2f, GameInstance->GetCurveFloat(ECurveFloat::ECF_Default), 0, 1.f, true));

					GameInstance->Play_MaterialParamAnimation("FadeOut2", Decal->GetDecal(), Info);

					// Angel이 쳐다보는 방향에따라 위치를 정하고, AttackActor Spawn
					
					FVector StartPos = HitResult.Location + (-GetActorForwardVector()) * 500.f + FVector(0.f, 0.f, 700.f);
					FVector EndPos = HitResult.Location;

					FActorSpawnParameters SpawnParams;
					SpawnParams.Owner = this;

					AHolyAngelMeteorAttack* MeteorAttack = GetWorld()->SpawnActor<AHolyAngelMeteorAttack>(GetSkillAttackClass(1), FTransform(GetActorForwardVector().Rotation(), StartPos), SpawnParams);
					if (IsValid(MeteorAttack))
					{
						FActionInfo ActionInfo;
						ActionInfo.ActionTime = 0.2f;

						ActionInfo.AddMovePoint(StartPos);
						ActionInfo.AddMovePoint(EndPos);

						MeteorAttack->AddAction(ActionInfo);
						MeteorAttack->PlayAction();

						FTimerDelegate TimerDele;
						TimerDele.BindLambda(
							[MeteorAttack]()
							{
								MeteorAttack->PlayAttackVFX();
								MeteorAttack->Destroy();
							}
						);
						GetWorldTimerManager().SetTimer(MeteorAttackEachTimer[Count], TimerDele, ActionInfo.ActionTime, false);
					}
				}
			);
			GetWorldTimerManager().SetTimer(MeteorAttackEachTimer[Count], TimerDele, 0.8f, false);
		}
	);
	GetWorldTimerManager().SetTimer(MeteorAttackEachTimer[Count], RadiusTimerDele, 0.3f, false);

	Decal->SetLifeSpan(4.f);

	FTimerDelegate TimerDele;
	TimerDele.BindUFunction(this, "SpawnMeteorAttackTimerFunc", Count + 1);
	GetWorldTimerManager().SetTimer(SpawnMeteorAttackTimer, TimerDele, 0.05f, false);
}

void AHolyAngel::MeteorAction(float Duration)
{
	FActionInfo ActionInfo;

	ActionInfo.ActionTime = Duration;
	ActionInfo.AddMovePoint(GetActorLocation());
	ActionInfo.AddMovePoint(GetActorLocation() + FVector(0.f, 0.f, 100.f));

	AddAction(ActionInfo);
	PlayAction();
}
