
#include "PaladinBoss.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <MyRPG/GameInstance/MyGameInstance.h>
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "MyRPG/AttackActor/AttackActor.h"
#include <NiagaraFunctionLibrary.h>
#include <MyRPG/AttackActor/ActionActor.h>
#include "MyRPG/Data/DamageInfo.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "MyRPG/AttackActor/EnemyAttack/PaladinBoss/PaladinBossKneelAttack.h"
#include "MyRPG/Enemy/Boss/HolyAngel.h"
#include "MyRPG/Types/ChannelType.h"
#include "MyRPG/AttackActor/EnemyAttack/HolyAngel/HolyAngelProjectileAttack.h"
#include "MyRPG/Gimmick/BGMTriggerVolume.h"

void APaladinBoss::BeginPlay()
{
	Super::BeginPlay();

	SetIgnoreCommandState(EDamageActionCommandTag::EDACT_CrowdControl, true);
	SetCanHitReact(false);

	if (BGMTrigger.IsValid())
	{
		OnDead.AddDynamic(BGMTrigger.Get(), &ABGMTriggerVolume::PlayEndTriggerSound);
	}
}

void APaladinBoss::Damaged(FDamageInfo* DamageInfo)
{
	Super::Damaged(DamageInfo);

	if (GetDead() || Phase == 2) return;

	float Percent = (float)GetHP() / (float)GetMaxHP();
	if (Percent <= Phase2HPPercent)
	{
		Phase = 2;
	}
}

void APaladinBoss::SetDead()
{
	Super::SetDead();

	if (Angel.IsValid())
	{
		Angel->SetDead();
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActionActor::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (Actor->GetOwner() == this) // 소유자가 현재 액터인지 확인
		{
			AActionActor* ActionActor = Cast<AActionActor>(Actor);
			if (IsValid(ActionActor))
			{
				UE_LOG(LogTemp, Warning, TEXT("Found ActionActor owned by this: %s"), *ActionActor->GetName());
				ActionActor->Destroy();
			}
		}
	}
}

bool APaladinBoss::IsSkillAvailable(int SkillNum)
{
	if (bOnlyUseOneSkill)
	{
		if (OnlySkillNum != SkillNum)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	bool bResult = false;

	switch (SkillNum)
	{
	case 0:
		bResult = CanDownThrust();
		break;
	case 1:
		bResult = CanDashChargeAttack();
		break;
	case 2:
		bResult = CanJumpAttack();
		break;
	case 3:
		bResult = CanSideShock();
		break;
	case 4:
		bResult = CanVerticalSlash();
		break;
	case 5:
		bResult = CanKneel();
		break;
	case 6:
		bResult = CanChargeSwing();
		break;
	default:
		break;
	}

	return bResult;
}

bool APaladinBoss::Skill(int SkillNum)
{
	if (Phase == 2 && bSpawnAngelFinished == false)
	{
		SpawnAngel(0.5f);
		bSpawnAngelFinished = true;
		return false;
	}
	

	if (Super::Skill(SkillNum) == false)
	{
		return false;
	}

	switch (SkillNum)
	{
	case 0:
	{
		int RandNum = FMath::Rand() % 2;
		if (RandNum == 0)
			WalkToCenter([this]() {DownThrust(); });
		else
			DownThrust();
		break;
	}
	case 1:
		DashChargeAttack();
		break;
	case 2:
		JumpAttack();
		break;
	case 3:
		WalkToCenter([this]() {SideShock();});
		break;
	case 4:
		VerticalSlash();
		break;
	case 5:
		WalkToCenter([this]() {Kneel();});
		break;
	case 6:
		ChargeSwing();
		break;
	default:
		break;
	}

	return true;
}

void APaladinBoss::ActionNotify_Implementation(int ActionNum, float Duration)
{
	switch (ActionNum)
	{
	case 20:
		ActionDashCharge(Duration);
		break;
	case 21:
		ActionDashChargeDash(Duration);
		break;
	case 30:
		ActionJump(Duration);
		break;
	default:
		break;
	}
}

void APaladinBoss::AnimEventNotify_Implementation(int AnimNum)
{
	switch (AnimNum)
	{
	case 10:
	{
		FTimerDelegate TimerDele;
		TimerDele.BindUFunction(this, "SpawnDownThrustAttack", SpawnDownThurstAttackInitAngle);
		GetWorldTimerManager().SetTimer(SpawnDownThrustAttackTimer, TimerDele, 0.0001f, false);
		/*UGameplayStatics::SpawnSoundAtLocation(this, DownThrustAttackTravelSoundCue, GetActorLocation(), FRotator::ZeroRotator,)*/
		break;
	}
	case 11:
	{
		IsSkillFinishedToTrue();
		break;
	}
	case 20:
	{
		// AttackActor 소환
		FTimerDelegate TimerDele;
		TimerDele.BindUFunction(this, "SpawnDashChargeAttack", 0.f);
		GetWorldTimerManager().SetTimer(SpawnDashChargeAttackTimer, TimerDele, 0.0001f, false);
		break;
	}
	case 21:
	{
		IsSkillFinishedToTrue();
		break;
	}

	/* 스킬3 점프어택 올라가고나서 */
	case 30:
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, DownAttackPredictFX, DownAttackLocation - FVector(0.f, 0.f, GetDefaultHalfHeight()) + FVector(0.f,0.f, 1.f), GetActorRotation());
		break;
	}
	/* 내려찍기*/
	case 31:
	{
		FActorSpawnParameters SpawnParam;
		SpawnParam.Owner = this;
		FVector Location = DownAttackLocation - FVector(0.f, 0.f, GetDefaultHalfHeight());
		GetWorld()->SpawnActor<AAttackActor>(GetSkillAttackClass(2), FTransform(Location), SpawnParam);
		break;
	}
	case 32:
	{
		IsSkillFinishedToTrue();
		break;
	}
	// Side Shock
	case 40:
	{
		for (int SpawnPointIndex = 0; SpawnPointIndex < 2; ++SpawnPointIndex)
		{
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SideShockSpawnFX, SpawnSideShockAttackPoint[SpawnPointIndex]);
			if (IsValid(NiagaraComp))
			{
				NiagaraComp->SetVariableFloat("Direction", ((SpawnPointIndex == 0) ? -1.f : 1.f));
			}
		}
		FTimerDelegate TimerDele;
		TimerDele.BindUFunction(this, "SpawnSideShockAttack", 0.f);
		GetWorldTimerManager().SetTimer(SpawnSideShockAttackTimer, TimerDele, 0.0001f, false);
		break;
	}
	case 41:
	{
		IsSkillFinishedToTrue();
		break;
	}
	// Vertical Slash AttackActor 소환
	case 50:
	{
		SpawnVerticalSlash();
		break;
	}
	// End
	case 51:
	{
		IsSkillFinishedToTrue();
		break;
	}

	case 60:
	{
		SpawnKneelAttack(4);
		break;
	}
	case 61:
	{
		IsSkillFinishedToTrue();
		break;
	}
	// 스윙 휘두를때 딱 그 타이밍
	case 70:
	{
		SpawnChargeSwingAttack();
		break;
	}
	case 71:
	{
		IsSkillFinishedToTrue();
		break;
	}
	// 앤젤 소환~
	case 7:
	{
		FVector Direction = (GetCurrentTargetLocation() - SpawnAngelPos).GetSafeNormal2D();
		Angel = GetWorld()->SpawnActor<AHolyAngel>(AngelClass, FTransform(Direction.Rotation(), SpawnAngelPos + FVector(0.f, 0.f, 100.f)));
		if (Angel.IsValid())
		{
			Angel->SetAngelPoints(AngelPoints);
			Angel->SetPaladin(this);
		}
		break;
	}
	default:
		break;
	}
}

void APaladinBoss::WalkToCenter(TFunction<void()> Func)
{
	FVector StartPoint, EndPoint;

	StartPoint = GetActorLocation();
	EndPoint = CenterPos;
	EndPoint.Z = GetActorLocation().Z;

	float Distance = FVector::Dist2D(EndPoint, StartPoint);
	
	FActionInfo ActionInfo;

	ActionInfo.ActionTime = Distance / (GetMaxWalkSpeed()+100.f);
	ActionInfo.bSweep = true;
	ActionInfo.TeleportType = ETeleportType::TeleportPhysics;
	ActionInfo.AddMovePoint(StartPoint);
	ActionInfo.AddMovePoint(EndPoint);

	AddAction(ActionInfo);
	PlayAction();

	if (ActionInfo.ActionTime < 0.01f)
	{
		Func();
	}
	else
	{
		TWeakObjectPtr<APaladinBoss> ThisWeakPtr = this;
		GetWorld()->GetTimerManager().SetTimer(
			WalkToCenterTimer, 
			[ThisWeakPtr, Func]()
			{
				if (ThisWeakPtr.IsValid() == false)
				{
					return;
				}
				if (ThisWeakPtr->GetDead() == false)
				{
					Func();
				}
			}, 
			ActionInfo.ActionTime, 
			false);
	}
	
}

void APaladinBoss::SpawnAngel(float DelayTime)
{
	if (IsValid(SpawnAngelMontage) == false)
	{
		return;
	}

	StopBehaviorTree(DelayTime + SpawnAngelMontage->GetPlayLength());

	FTimerDelegate TimerDele;
	TimerDele.BindLambda([this]() 
		{
			FVector Direction = (GetCurrentTargetLocation() - GetActorLocation()).GetSafeNormal2D();
			FHitResult HitResult;
			FVector StartPoint, EndPoint;

			for (int i = 0; i < 2; i++)
			{
				StartPoint = GetActorLocation() + ((i == 0) ? FVector::CrossProduct(FVector::UpVector, Direction) : FVector::CrossProduct(Direction, FVector::UpVector)) * SpawnAngelDistance;
				EndPoint = StartPoint - FVector(0.f, 0.f, 500.f);

				bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_AimTrace);

				if (bHit)
				{
					SpawnAngelPos = HitResult.Location;
					break;
				}
			}
			
			PlayAnimMontage(SpawnAngelMontage); 
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SpawnAngelPredictFX, SpawnAngelPos + FVector(0.f, 0.f, 1.f));
		});

	SpawnAngelMontage->GetPlayLength();
	GetWorldTimerManager().SetTimer(SpawnAngelTimer, TimerDele, DelayTime, false);
}

bool APaladinBoss::CanDownThrust()
{
	return FVector::Dist(GetActorLocation(), CenterPos) > 500.f;
}

bool APaladinBoss::CanDashChargeAttack()
{
	AActor* Player = Cast<AActor>(GetValueAsObject("Target"));
	if (IsValid(Player) == false) return false;

	FVector DirectionToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	FVector TargetPos = GetActorLocation() + DirectionToPlayer * 1000.f;

	float Distance = FVector::Dist(TargetPos, CenterPos);
	
	return Distance < 1200.f;
}

bool APaladinBoss::CanSideShock()
{
	return FVector::Dist(GetActorLocation(), CenterPos) > 500.f;
}

bool APaladinBoss::CanJumpAttack()
{
	AActor* Player = Cast<AActor>(GetValueAsObject("Target"));
	if (IsValid(Player) == false) return false;

	float Distance = FVector::Dist(Player->GetActorLocation(), GetActorLocation());
	return Distance > 700.f;
}

bool APaladinBoss::CanVerticalSlash()
{
	AActor* Player = Cast<AActor>(GetValueAsObject("Target"));
	if (IsValid(Player) == false) return false;

	float Distance = FVector::Dist(Player->GetActorLocation(), GetActorLocation());
	return Distance < 1000.f && Distance > 300.f;
}

bool APaladinBoss::CanKneel()
{
	return FVector::Dist(GetActorLocation(), CenterPos) > 500.f;
}

bool APaladinBoss::CanChargeSwing()
{
	AActor* Player = Cast<AActor>(GetValueAsObject("Target"));
	if (IsValid(Player) == false) return false;

	float Distance = FVector::Dist(Player->GetActorLocation(), GetActorLocation());
	return Distance < 600.f && Phase == 2;
}

void APaladinBoss::DownThrust()
{
	PlaySkillMontageAfterInterpRotationForcibly(SkillSectionName(0), 0.5f);
}

void APaladinBoss::SpawnDownThrustAttack(float Angle)
{
	if (Angle > 150.f)
	{
		GetWorldTimerManager().ClearTimer(SpawnDownThrustAttackTimer);
		GetAnimInstance()->Montage_Play(GetSkillMontage());
		GetAnimInstance()->Montage_JumpToSection(SkillSectionName(0, "Finish"), GetSkillMontage());
		return;
	}

	FVector StartLocation, MiddleLocation, MiddleLocation2, EndLocation, ToStartLocationDirection, AngleDirection;

	FVector ActorLocation = GetActorLocation() - FVector(0.f, 0.f, GetDefaultHalfHeight());
	FVector StandardDirection = GetActorRightVector();
	
	FActionInfo ActionInfo;
	ActionInfo.ActionTime = SpawnDownThrustAttackActionTime;
	ActionInfo.CurveType = ECurveFloat::ECF_EaseInSlowly;

	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;

	Angle = (Angle == 90.f) ? Angle + SpawnDownThurstAttackAngleInterval : Angle;
	float TempAngle = Angle;
	

	for (int i=0; i<2; ++i)
	{
		ActionInfo.ClearMovePoint();
		TempAngle = (i == 1) ? (180.f - TempAngle) : TempAngle;

		AngleDirection = StandardDirection.RotateAngleAxis(TempAngle, FVector(0.f, 0.f, 1.f));
		StartLocation = ActorLocation + AngleDirection * SpawnDownThurstAttackRadius;
		AActionActor* ActionActor = GetWorld()->SpawnActor<AActionActor>(GetSkillAttackClass(0), FTransform(StartLocation), SpawnParam);

		if (IsValid(ActionActor) == false)
		{
			return;
		}

		FVector SideDirection = (FVector::CrossProduct(GetActorForwardVector(), AngleDirection).Z > 0.f ) ? StandardDirection : -StandardDirection;

		MiddleLocation2 = ActorLocation + (GetActorForwardVector() * SpawnDownThurstAttackMiddle2ForwardDistance) + SideDirection * SpawnDownThurstAttackMiddle2SideDistance;
		MiddleLocation = FMath::Lerp(StartLocation, MiddleLocation2, 0.5f);
		ToStartLocationDirection = (StartLocation - ActorLocation).GetSafeNormal();
		MiddleLocation = MiddleLocation + ToStartLocationDirection * SpawnDownThurstAttackMiddleDistance;
		EndLocation = MiddleLocation2 + (-ToStartLocationDirection * SpawnDownThurstAttackEndDistance);


		ActionInfo.AddMovePoint(StartLocation);
		ActionInfo.AddMovePoint(MiddleLocation);
		ActionInfo.AddMovePoint(MiddleLocation2);
		ActionInfo.AddMovePoint(EndLocation);

		ActionActor->AddAction(ActionInfo);
		ActionActor->PlayAction();
	}


	FTimerDelegate TimerDele;
	TimerDele.BindUFunction(this, "SpawnDownThrustAttack", Angle+ SpawnDownThurstAttackAngleInterval);
	GetWorldTimerManager().SetTimer(SpawnDownThrustAttackTimer, TimerDele, SpawnDownThurstAttackRate, false);
}

void APaladinBoss::DashChargeAttack()
{
	PlaySkillMontageAfterInterpRotationForcibly(SkillSectionName(1), DashChargeWaitTime);
}

void APaladinBoss::ActionDashCharge(float Duration)
{
	AActor* Player = Cast<AActor>(GetValueAsObject("Target"));
	if (IsValid(Player) == false) return;

	FVector StartLocation, EndLocation;
	StartLocation = GetActorLocation();
	DashChargeStepBackLocation = EndLocation = GetActorLocation() + (-GetActorForwardVector() * 200.f);

	const float ActionTime = Duration + 0.5f;

	FActionInfo ActionInfo1;
	ActionInfo1.ActionTime = ActionTime;
	ActionInfo1.CurveType = ECurveFloat::ECF_EaseOut;
	ActionInfo1.AddMovePoint(StartLocation);
	ActionInfo1.AddMovePoint(EndLocation);


	AddAction(ActionInfo1);
	PlayAction();

	PlaySkillMontageAfterInterpRotationForcibly(SkillSectionName(1, "Dash"), ActionTime + 0.4f);
	//PlaySkillMontage(SkillSectionName(1, "Dash"), ActionTime + 0.2f);
}

void APaladinBoss::ActionDashChargeDash(float Duration)
{
	FActionInfo ActionInfo2;

	FVector StartLocation = DashChargeStepBackLocation;
	FVector EndLocation = GetActorLocation() + (GetActorForwardVector() * 1400.f);

	float ActionTime = Duration + 0.7f;
	SpawnDashChargeAttackDuration = ActionTime;

	ActionInfo2.ActionTime = ActionTime;
	ActionInfo2.CurveType = ECurveFloat::ECF_Default;
	ActionInfo2.AddMovePoint(StartLocation);
	ActionInfo2.AddMovePoint(EndLocation);

	AddAction(ActionInfo2);
	PlayAction();

	PlaySkillMontage(SkillSectionName(1, "DashFinish"), ActionTime);
}

void APaladinBoss::SpawnDashChargeAttack(float TotalTime)
{
	if (TotalTime > SpawnDashChargeAttackDuration)
	{
		GetWorldTimerManager().ClearTimer(SpawnDashChargeAttackTimer);
		return;
	}

	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;

	FActionInfo ActionInfo;
	ActionInfo.ActionTime = SpawnDashChargeAttackActionTime;
	ActionInfo.CurveType = ECurveFloat::ECF_Default;
	
	FVector OriginLocation = GetActorLocation() - FVector(0.f, 0.f, GetDefaultHalfHeight());
	FVector OriginRightVector = GetActorRightVector();
	for (int i = 0; i < 2; i++)
	{
		FVector MoveDirection = ((i == 0) ? (OriginRightVector) : (-OriginRightVector));
		
		FVector StartLocation = OriginLocation + MoveDirection * SpawnDashChargeAttackSideInitDistance;

		MoveDirection = FMath::Lerp(MoveDirection, GetActorForwardVector(), 0.5f).GetSafeNormal2D();

		FVector EndLocation = StartLocation + MoveDirection * SpawnDashChargeAttackSideTargetDistance;

		AActionActor* ActionActor = GetWorld()->SpawnActor<AActionActor>(GetSkillAttackClass(1), FTransform(StartLocation), SpawnParam);
		
		if (IsValid(ActionActor) == false)
		{
			continue;
		}

		ActionInfo.ClearMovePoint();
		ActionInfo.AddMovePoint(StartLocation);
		ActionInfo.AddMovePoint(EndLocation);
		ActionActor->AddAction(ActionInfo);
		ActionActor->PlayAction();
	}

	FTimerDelegate TimerDele;
	TimerDele.BindUFunction(this, "SpawnDashChargeAttack", TotalTime + SpawnDashChargeAttackRate);
	GetWorldTimerManager().SetTimer(SpawnDashChargeAttackTimer, TimerDele, SpawnDashChargeAttackRate, false);
}

void APaladinBoss::JumpAttack()
{
	ACharacter* Player = Cast<ACharacter>(GetValueAsObject("Target"));
	if (IsValid(Player) == false) return;

	PlaySkillMontageAfterInterpRotationForcibly(SkillSectionName(2), JumpAttackWaitTime,
		[=]()
		{
			DownAttackLocation = Player->GetActorLocation() - FVector(0.f, 0.f, Player->GetDefaultHalfHeight()) + FVector(0.f, 0.f, GetDefaultHalfHeight());
			SetActorRotation((DownAttackLocation - GetActorLocation()).GetSafeNormal2D().Rotation());
		});
}

void APaladinBoss::ActionJump(float Duration)
{
	AActor* Player = Cast<AActor>(GetValueAsObject("Target"));
	if (IsValid(Player) == false) return;

	FVector StartLocation, MiddleLocation;
	StartLocation = GetActorLocation();
	MiddleLocation = DownAttackLocation + FVector(0.f, 0.f, 500.f);
	const float JumpUpTime = 0.9f;

	FActionInfo ActionInfo1;
	ActionInfo1.ActionTime = JumpUpTime;
	ActionInfo1.CurveType = ECurveFloat::ECF_EaseOut;
	ActionInfo1.AddMovePoint(StartLocation);
	ActionInfo1.AddMovePoint(MiddleLocation);

	FActionInfo ActionInfo2;
	ActionInfo2.ActionTime = Duration - JumpUpTime;
	ActionInfo2.CurveType = ECurveFloat::ECF_Default;
	ActionInfo2.AddMovePoint(MiddleLocation);
	ActionInfo2.AddMovePoint(DownAttackLocation);

	AddAction(ActionInfo1);
	AddAction(ActionInfo2);
	PlayAction();
}

void APaladinBoss::SideShock()
{
	PlaySkillMontage(SkillSectionName(3));


	SpawnSideShockAttackPoint[0] = GetActorLocation() + GetActorRightVector() * 400.f - FVector(0.f,0.f,GetDefaultHalfHeight() - 20.f);
	SpawnSideShockAttackPoint[1] = GetActorLocation() - GetActorRightVector() * 400.f - FVector(0.f, 0.f, GetDefaultHalfHeight() - 20.f);

	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SideShockPredictFX, SpawnSideShockAttackPoint[0]);
	if (IsValid(NiagaraComp))
	{
		NiagaraComp->SetVariableFloat("Direction", -1.f);
	}
	NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SideShockPredictFX, SpawnSideShockAttackPoint[1]);
	if (IsValid(NiagaraComp))
	{
		NiagaraComp->SetVariableFloat("Direction", 1.f);
	}
}

void APaladinBoss::SpawnSideShockAttack(float Angle)
{
	// 종료 조건 다시기입
	if (Angle >= 450.f)
	{
		GetWorldTimerManager().ClearTimer(SpawnSideShockAttackTimer);
		GetAnimInstance()->Montage_Play(GetSkillMontage());
		GetAnimInstance()->Montage_JumpToSection(SkillSectionName(3, "Finish"), GetSkillMontage());
		return;
	}

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SideShockSpawnSound, GetActorLocation());

	FVector StartLocation, EndLocation, AngleDirection;

	FActionInfo ActionInfo;
	ActionInfo.ActionTime = SpawnSideShockAttackActionTime;
	ActionInfo.CurveType = ECurveFloat::ECF_Default;

	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;

	for (int SpawnPointIndex = 0; SpawnPointIndex < 2; ++SpawnPointIndex)
	{
		for (int AttackActorIndex = 0; AttackActorIndex < 3; ++AttackActorIndex)
		{
			ActionInfo.ClearMovePoint();

			//(SpawnPointIndex == 0) ? -1.f : 1.f 
			float AttackAngle = Angle * ((SpawnPointIndex == 0) ? -1.f : 1.f);
			float Radian = FMath::DegreesToRadians(AttackAngle + 120.f * AttackActorIndex);
			StartLocation = SpawnSideShockAttackPoint[SpawnPointIndex] + FVector((SpawnSideShockAttackRadius * FMath::Cos(Radian)), (SpawnSideShockAttackRadius * FMath::Sin(Radian)), 0.f);

			FVector Direction = (StartLocation - SpawnSideShockAttackPoint[SpawnPointIndex]).GetSafeNormal2D();
			EndLocation = StartLocation + Direction * SpawnSideShockAttackActionDistance;

			AActionActor* ActionActor = GetWorld()->SpawnActor<AActionActor>(GetSkillAttackClass(3), FTransform(StartLocation), SpawnParam);
			
			if (IsValid(ActionActor))
			{
				ActionInfo.AddMovePoint(StartLocation);
				ActionInfo.AddMovePoint(EndLocation);
				
				ActionActor->AddAction(ActionInfo);
				ActionActor->PlayAction();

				ActionActor->SetLifeSpan(SpawnSideShockAttackActionTime);
			}
			
		}
	}
	
	FTimerDelegate TimerDele;
	TimerDele.BindUFunction(this, "SpawnSideShockAttack", Angle + SpawnSideShockAttackAngleInterval);
	GetWorldTimerManager().SetTimer(SpawnSideShockAttackTimer, TimerDele, SpawnSideShockAttackRate, false);
}

void APaladinBoss::VerticalSlash()
{
	SetInterpRotationForcibly(true);
	SetAutoInterpRotationToTarget(true);
	PlaySkillMontage(SkillSectionName(4));
}

void APaladinBoss::SpawnVerticalSlash()
{
	SetInterpRotationForcibly(false);
	SetAutoInterpRotationToTarget(false);

	FVector StartLocation;
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;

	FRotator PlayerRotation = GetActorRotation();

	FRotator RightRotation = FRotator(0.f, PlayerRotation.Yaw + SpawnVerticalSlashAttackAngle, 0.f);
	FRotator LeftRotation = FRotator(0.f, PlayerRotation.Yaw - SpawnVerticalSlashAttackAngle, 0.f);

	FVector RightDirection = RightRotation.Vector();
	FVector LeftDirection = LeftRotation.Vector();

	VerticalSlashAttackMiddlePoint = GetActorLocation() + GetActorForwardVector() * SpawnVerticalSlashAttackMiddleDistance - FVector(0.f, 0.f, 100.f);

	FActionInfo ActionInfo;
	ActionInfo.ActionTime = SpawnVerticalSlashAttackActionTime;
	//ActionInfo.CurveType = ECurveFloat::ECF_EaseInOut;

	/* 오른쪽부터*/
	for (int i = 0; i < 2; i++)
	{
		FVector Direction = ((i == 0) ? RightDirection : LeftDirection);

		StartLocation = GetActorLocation() + Direction * SpawnVerticalSlashAttackStartDistance - FVector(0.f, 0.f, 100.f);
		VerticalSlashAttackFinishPoint[i] = StartLocation + Direction * SpawnVerticalSlashAttackDistance;

		AActionActor* ActionActor = GetWorld()->SpawnActor<AActionActor>(GetSkillAttackClass(4), FTransform(RightRotation, StartLocation), SpawnParam);
		if (IsValid(ActionActor) == false)
		{
			return;
		}
		VerticalSlashAttackArray[i] = ActionActor;

		ActionInfo.ClearMovePoint();
		ActionInfo.AddMovePoint(StartLocation);
		ActionInfo.AddMovePoint(VerticalSlashAttackFinishPoint[i]);

		ActionActor->AddAction(ActionInfo);
		ActionActor->PlayAction();
		ActionActor->SetLifeSpan(SpawnVerticalSlashAttackActionTime);
	}

	GetWorld()->GetTimerManager().SetTimer(SpawnSecondVerticalSlashAttackTimer, this, &APaladinBoss::SpawnSecondVerticalSlash, SpawnVerticalSlashAttackActionTime);
}

void APaladinBoss::SpawnSecondVerticalSlash()
{
	FVector SecondVerticalSlashAttackFinishPoint[4];
	FVector StartLocation, MiddleLocation, EndLocation;

	FActionInfo ActionInfo;
	ActionInfo.ActionTime = SpawnSecondVerticalSlashAttackActionTime;
	//ActionInfo.CurveType = ECurveFloat::ECF_EaseInOut;

	for (int i = 0; i < 2; i++)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, VerticalSlashEndFX, VerticalSlashAttackFinishPoint[i] + FVector(0.f, 0.f, 100.f), GetActorRotation());

		StartLocation = VerticalSlashAttackFinishPoint[i];

		FVector SideVector = ((i == 0) ? GetActorRightVector() : -GetActorRightVector());
		// 0은 바깥쪽으로 도는거 1은 안쪽으로 도는거
		for (int j = 0; j < 2; j++)
		{
			if (j == 0)
			{
				MiddleLocation = VerticalSlashAttackMiddlePoint + SideVector * SpawnSecondVerticalSlashAttackSideFarDistance;
			}
			else
			{
				MiddleLocation = VerticalSlashAttackMiddlePoint + (-SideVector) * SpawnSecondVerticalSlashAttackSideNearDistance;
			}

			EndLocation = MiddleLocation + (GetActorLocation() - MiddleLocation).GetSafeNormal2D() * SpawnSecondVerticalSlashAttackDistance;

			FActorSpawnParameters SpawnParam;
			SpawnParam.Owner = this;
			AActionActor* ActionActor = GetWorld()->SpawnActor<AActionActor>(GetSkillAttackClass(4), FTransform(StartLocation), SpawnParam);

			ActionInfo.ClearMovePoint();
			ActionInfo.AddMovePoint(StartLocation);
			ActionInfo.AddMovePoint(MiddleLocation);
			ActionInfo.AddMovePoint(EndLocation);

			if (IsValid(ActionActor))
			{
				ActionActor->AddAction(ActionInfo);
				ActionActor->PlayAction();
				ActionActor->SetLifeSpan(SpawnSecondVerticalSlashAttackActionTime);
			}

			SecondVerticalSlashAttackFinishPoint[i * 2 + j] = EndLocation;
		}
	}

	FTimerDelegate TimerDele;
	TimerDele.BindLambda(
		[=]()
		{
			for (int i = 0; i < 4; i++)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, VerticalSlashEndFX, SecondVerticalSlashAttackFinishPoint[i] + FVector(0.f, 0.f, 100.f), GetActorRotation());
			}
		}
	);
	

	GetWorld()->GetTimerManager().SetTimer(SpawnSecondVerticalSlashAttackTimer, TimerDele, SpawnSecondVerticalSlashAttackActionTime - 0.1f, false);
	
}

void APaladinBoss::Kneel()
{
	PlaySkillMontage(SkillSectionName(5));
}

void APaladinBoss::SpawnKneelAttack(int Count)
{
	// 지름 대충 2700으로 잡고하면됨
	// 움직이는 방향은 보스의 반지름 100정도의 원에서 랜덤

	FActionInfo ActionInfo;
	ActionInfo.FirstDelay = SpawnKneelAttackFirstDelay;
	ActionInfo.ActionTime = SpawnKneelAttackSeconds;

	for (int i = 0; i < 2; ++i)
	{
		FVector RandSpawnPos = GetActorLocation() + FVector(FMath::RandPointInCircle(1350.f), 0.f);
		FVector RandDirectionPos = GetActorLocation() + FVector(FMath::RandPointInCircle(50.f), 0.f);

		RandSpawnPos.Z = GetActorLocation().Z - GetDefaultHalfHeight() + 1.f;
		RandDirectionPos.Z = GetActorLocation().Z - GetDefaultHalfHeight() + 1.f;

		FVector StartPos = RandSpawnPos;
		FVector EndPos = StartPos + (RandDirectionPos - RandSpawnPos).GetSafeNormal() * SpawnKneelAttackDistance;

		APaladinBossKneelAttack* ActionActor = GetWorld()->SpawnActorDeferred<APaladinBossKneelAttack>(GetSkillAttackClass(5), FTransform(RandSpawnPos), this);
		ActionActor->SetWaitTime(SpawnKneelAttackFirstDelay);
		UGameplayStatics::FinishSpawningActor(ActionActor, FTransform(RandSpawnPos));

		if (IsValid(ActionActor))
		{
			ActionInfo.ClearMovePoint();
			ActionInfo.AddMovePoint(StartPos);
			ActionInfo.AddMovePoint(EndPos);

			ActionActor->AddAction(ActionInfo);
			ActionActor->PlayAction();
			ActionActor->SetLifeSpan(SpawnKneelAttackSeconds);
		}
	}
	
	if (Count > 0)
	{
		FTimerDelegate TimerDele;
		TimerDele.BindUFunction(this, "SpawnKneelAttack", Count - 1);
		GetWorld()->GetTimerManager().SetTimer(SpawnKneelAttackTimer, TimerDele, SpawnKneelAttackInterval, false);
	}
	else
	{
		PlaySkillMontage(SkillSectionName(5, "Finish"));
	}
	
}

void APaladinBoss::ChargeSwing()
{
	if (Angel.IsValid() == false)
	{
		return;
	}
	SetInterpRotationForcibly(true);
	SetAutoInterpRotationToTarget(true);

	FProjectileAttackDelegate& Dele = Angel->PlayProjectileAttack();
	Dele.AddUniqueDynamic(this, &APaladinBoss::PlayChargeSwingAttackMontage);

	// exec까지 1.2초걸림
}

void APaladinBoss::SpawnChargeSwingAttack()
{
	if (Angel.IsValid() == false)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Angel.Get();

	FVector StandardDirection = GetActorForwardVector();
	float Distance = 50.f;
	float Angle = -30.f;
	for (int i = 0; i < 5; ++i)
	{
		FVector Direction = StandardDirection.RotateAngleAxis(Angle + i * 15, FVector::UpVector);
		FVector SpawnPos = GetActorLocation() + Direction * Distance;

		AHolyAngelProjectileAttack* AttackActor = GetWorld()->SpawnActor<AHolyAngelProjectileAttack>(Angel->GetProjectileAttackActorClass(), FTransform(SpawnPos), SpawnParams);

		FActionInfo ActionInfo;
		ActionInfo.ActionTime = 1.1f;
		ActionInfo.AddMovePoint(SpawnPos);
		ActionInfo.AddMovePoint(SpawnPos + Direction * 3000.f);

		AttackActor->AddAction(ActionInfo);
		AttackActor->PlayAction();
		AttackActor->SetLifeSpan(ActionInfo.ActionTime);
	}

	SetInterpRotationForcibly(false);
	SetAutoInterpRotationToTarget(false);
}

void APaladinBoss::PlayChargeSwingAttackMontage()
{
	PlaySkillMontage(SkillSectionName(6));
}