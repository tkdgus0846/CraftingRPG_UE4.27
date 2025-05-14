

#include "BasicDashCaster.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "MyRPG/Player/PlayerAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "MyRPG/Player/Components/CombatComponent.h"
#include "MyRPG/Player/Components/MouseComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyRPG/SplineActor/SplineActor.h"
#include <Kismet/GameplayStatics.h>
#include <MyRPG/GameInstance/MyGameInstance.h>
#include "MyRPG/AttackActor/AttackActor.h"
#include "MyRPG/Controller/MyPlayerController.h"
#include <NiagaraSystem.h>
#include <NiagaraFunctionLibrary.h>
#include "NiagaraComponent.h"

ABasicDashCaster::ABasicDashCaster()
{
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemFinder(TEXT("/Game/Blueprints/FX/PlayerFX/Dash/NS_PlayerBasicDash.NS_PlayerBasicDash"));
	
	if (NiagaraSystemFinder.Succeeded())
	{
		BasicDashVFX = NiagaraSystemFinder.Object;
	}
}

void ABasicDashCaster::SkillPressed()
{
	if (PlayerCharacter == nullptr || SkillMontage == nullptr) return;

	FVector Velocity = PlayerCharacter->GetLastMovementInputVector();
	Direction = (Velocity.Size2D() == 0.f) ? PlayerCharacter->GetActorForwardVector() : Velocity.GetSafeNormal2D();

	//{
	//	FActionInfo Action1;
	//	Action1.AnimMontage = SkillMontage;
	//	Action1.OriginRotation = Direction.Rotation();
	//	Action1.CurveType = ECurveFloat::ECF_EaseOut;
	//	Action1.bAttachToGround = true;
	//	Action1.bSweep = true;
	//	Action1.FirstDelay = 0.4f;
	//	Action1.ActionTime = 0.85f;

	//	/*Action1.AddMovePoint(PlayerCharacter->GetActorLocation() + FVector(0.f,0.f,500.f));
	//	Action1.AddMovePoint(PlayerCharacter->GetActorLocation() + FVector(0.f, 0.f, 500.f) + Direction * 700.f);*/

	//	Action1.AddMovePoint(PlayerCharacter->GetActorLocation(), ESplinePointType::Linear);
	//	Action1.AddMovePoint(PlayerCharacter->GetActorLocation() + Direction * 500.f, ESplinePointType::Linear);

	//	PlayerCharacter->AddAction(Action1);
	//	PlayerCharacter->PlayAction();

	//	PlayerCharacter->GetCombatComponent()->OnSpaceSkill.Broadcast();

	//	//bDashing = true;
	//}

	PlayerCharacter->PlayAnimMontage(SkillMontage);
	CurrentDashTime = 0.f;
	PlayerCharacter->DisableInput(PlayerCharacter->GetPlayerController());
	UPlayerAnimInstance* AnimInstance = Cast<UPlayerAnimInstance>(PlayerCharacter->GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance))
	{
		AnimInstance->SetPlayMontageOnly(true);
	}

	GetWorld()->GetTimerManager().SetTimer(Timer, this, &ABasicDashCaster::Dash, GetWorld()->DeltaTimeSeconds, true);
	PlayerCharacter->ChangeState(EStateMaterialType::ESMT_Channeling, DashDuration);
	PlayerCharacter->GetCombatComponent()->OnSpaceSkill.Broadcast();
	PlayerCharacter->SetIgnoreDamage(true);
	//if (IsValid(BasicDashVFX))
	//{
	//	BasicDashVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
	//		GetWorld(),
	//		BasicDashVFX,
	//		PlayerCharacter->GetActorLocation(),
	//		PlayerCharacter->GetActorRotation()
	//	);
	//	//UE_LOG(LogTemp, Display, TEXT(""));
	//}
	
}

void ABasicDashCaster::SkillPressing(float DeltaTime)
{
}

void ABasicDashCaster::SkillReleased()
{
}

void ABasicDashCaster::Dash()
{
	if (IsValid(PlayerCharacter) == false)
	{
		return;
	}

	/*if (IsValid(BasicDashVFXComponent))
	{
		BasicDashVFXComponent->SetVectorParameter("PlayerPos", PlayerCharacter->GetActorLocation());
	}*/
	
	PlayerCharacter->SetMaxWalkSpeed(FMath::Lerp(2000.f, 100.f, CurrentDashTime / DashDuration));
	PlayerCharacter->SetMaxAcceleration(100000.f);
	Direction = PlayerCharacter->GetActorRotation().Vector();

	PlayerCharacter->AddMovementInput(Direction, 1.f);
	CurrentDashTime += GetWorld()->DeltaTimeSeconds;

	if (CurrentDashTime >= DashDuration)
	{
		UPlayerAnimInstance* AnimInstance = Cast<UPlayerAnimInstance>(PlayerCharacter->GetMesh()->GetAnimInstance());
		if (IsValid(AnimInstance))
		{
			AnimInstance->SetPlayMontageOnly(false);
			AnimInstance->StopAllMontages(0.2f);
		}

		PlayerCharacter->EnableInput(PlayerCharacter->GetPlayerController());
		PlayerCharacter->WalkSpeedToOrigin();
		PlayerCharacter->AccelerationToOrigin();
		PlayerCharacter->SetIgnoreDamage(false);
		GetWorld()->GetTimerManager().ClearTimer(Timer);
	}
}
