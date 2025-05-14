
#include "MyGameInstance.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Materials/MaterialParameterCollection.h"
#include "MyRPG/Effect/PostProcessAnimator.h"
#include "MyRPG/Effect/MaterialParameterAnimator.h"
#include <Kismet/GameplayStatics.h>
#include <Engine/DecalActor.h>
#include "Components/DecalComponent.h"
#include "Curves/CurveLinearColor.h"

#include "MyRPG/Command/CommandPoolManager.h"
#include "MyRPG/Manager/LoadingManager.h"



void UMyGameInstance::Init()
{
	Super::Init();
}

void UMyGameInstance::Shutdown()
{
	Super::Shutdown();

	UCommandPoolManager::Shutdown();
	ULoadingManager::Shutdown();

	UE_LOG(LogTemp, Warning, TEXT("Gameinstance Shutdown"));
}

void UMyGameInstance::ClientPlayCameraShake(ECameraShake WhichShake, float Scale)
{
	TSubclassOf<UCameraShakeBase> Shake = GetCameraShake(WhichShake);
	if (IsValid(Shake))
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(Shake, Scale);
	}
}

UCurveFloat* UMyGameInstance::GetCurveFloat(ECurveFloat Type)
{
	UCurveFloat** FindCurve = CurveFloat_Map.Find(Type);
	if (FindCurve == nullptr)
		return nullptr;

	return *FindCurve;
}

void UMyGameInstance::SlowMotion(ECurveFloat Type, float Duration, float FirstDelay)
{
	UCurveFloat* Curve = GetCurveFloat(Type);
	if (Curve == nullptr) return;

	StopSlowMotion();

	FOnTimelineFloat CurveFloatCallback;
	CurveFloatCallback.BindUFunction(this, FName("CurveFloatSlowMotion"));
	SlowMotionTimeline.AddInterpFloat(Curve, CurveFloatCallback);

	FOnTimelineEvent TimelineFinishedCallback;
	TimelineFinishedCallback.BindUFunction(this, FName("StopSlowMotion"));
	SlowMotionTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);

	SlowMotionTimeline.SetPlayRate(1.f / Duration);

	GetWorld()->GetTimerManager().SetTimer(SlowMotionTimer, this, &UMyGameInstance::PlaySlowMotionTimeline, 0.0001f, false, FirstDelay);
}

void UMyGameInstance::StopSlowMotion()
{
	GetWorld()->GetTimerManager().ClearTimer(SlowMotionTimer);
	GetWorld()->GetWorldSettings()->SetTimeDilation(1.f);
	SlowMotionTimeline.Stop();
}

void UMyGameInstance::TimeDilation(float Dilation)
{
	GetWorld()->GetWorldSettings()->SetTimeDilation(Dilation);
	
}

//void UMyGameInstance::SetMaterialScalarParameter(FName ParameterName, float ParameterValue, EMaterialParameterCollectionType Type)
//{
//	UMaterialParameterCollection** FindValue = MaterialParameterCollection_Map.Find(Type);
//	if (FindValue == nullptr) return;
//
//	UMaterialParameterCollectionInstance* Instance = GetWorld()->GetParameterCollectionInstance(*FindValue);
//	if (IsValid(Instance) == false) return;
//
//	Instance->SetScalarParameterValue(ParameterName, ParameterValue);
//}

void UMyGameInstance::Play_PostProcessAnimation(EPostProcessAnimationType Type, float Duration)
{
	TSubclassOf<class APostProcessAnimator>* FindValue = PostProcessAnimator_Map.Find(Type);
	if (FindValue == nullptr) return;

	APostProcessAnimator* PostProcessAnimator = GetWorld()->SpawnActorDeferred<APostProcessAnimator>(FindValue->Get(), FTransform());

	PostProcessAnimator->Initialize(Duration);

	UGameplayStatics::FinishSpawningActor(PostProcessAnimator, FTransform::Identity);
}


void UMyGameInstance::Play_MaterialParamAnimation(const FName& Name, USceneComponent* Component, TArray<FPlayMaterialParameterAnimationInfo>& PlayInfo)
{
	if (IsValid(Component) == false) return;

	TArray<FMaterialParameterAnimationInfo> InfoArray;
	
	for (auto& Info : PlayInfo)
	{
		UMaterialInstanceDynamic* Instance = nullptr;

		UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Component);
		UDecalComponent* DecalComponent = nullptr;

		if (PrimitiveComponent)
		{
			Instance = Cast<UMaterialInstanceDynamic>(PrimitiveComponent->GetMaterial(Info.MaterialIndex));
		}
		else
		{
			DecalComponent = Cast<UDecalComponent>(Component);
			if (DecalComponent)
				Instance = Cast<UMaterialInstanceDynamic>(DecalComponent->GetDecalMaterial());
		}

		bool bShouldCreateDynamicMaterialInterface = (IsValid(Instance) == false) || (IsValid(Instance) == true && Info.Material && Instance->Parent != Info.Material);

		if (bShouldCreateDynamicMaterialInterface)
		{
			Instance = (PrimitiveComponent) ? PrimitiveComponent->CreateDynamicMaterialInstance(Info.MaterialIndex, Info.Material) : (DecalComponent ? DecalComponent->CreateDynamicMaterialInstance() : nullptr);
			if (IsValid(Instance) == false) return;
		}

		InfoArray.Add(FMaterialParameterAnimationInfo(Instance, Info));
	}

	AMaterialParameterAnimator* Animator = GetWorld()->SpawnActorDeferred<AMaterialParameterAnimator>(AMaterialParameterAnimator::StaticClass(), FTransform::Identity);
	if (IsValid(Animator))
	{
		Animator->Initialize(InfoArray);
	}
	UGameplayStatics::FinishSpawningActor(Animator, FTransform::Identity);

	
	auto Find = MaterialAnimationInfo.Find(Component);
	if (Find == nullptr)
	{
		MaterialAnimationInfo.Add(Component, FMaterialParameterAnimatorMap(Name, Animator));
	}
	else
	{
		Find->Add(Name, Animator);
	}
	
	
	if (GetWorld()->GetTimerManager().IsTimerActive(MaterialAnimationInfo_CleanUpTimer) == false)
		GetWorld()->GetTimerManager().SetTimer(MaterialAnimationInfo_CleanUpTimer, this, &UMyGameInstance::CleanUp_MaterialParamAnimation,  10.f, true);
	
}

void UMyGameInstance::Clear_MaterialParamAnimation(USceneComponent* Key, const FName& Name)
{
	auto Value = MaterialAnimationInfo.Find(Key);
	if (Value == nullptr) return;

	Value->Clear(Name);
}

void UMyGameInstance::CleanUp_MaterialParamAnimation() 
{
	// Component가 Valid하지 않을때 -> 안에 있는 모든놈들을 Clear
	// Component안의 AMaterialParameterAnimator의 요소들을 순회하면서 IsValid하지 않다면 그놈들을 Clear

	for (auto It = MaterialAnimationInfo.CreateIterator(); It; ++It)
	{
		TWeakObjectPtr<USceneComponent> Key = It.Key();
		if (Key.IsValid() == false)
		{
			UE_LOG(LogTemp, Error, TEXT("CleanUp_MaterialParamAnimation"));
			It.Value().ClearAll();

			It.RemoveCurrent();
		}
	}
}

ADecalActor* UMyGameInstance::SpawnDecal(TSubclassOf<ADecalActor> Decal, const FTransform& Transform, FVector DecalSize)
{
	if (IsValid(Decal) == false) return nullptr;

	ADecalActor* DecalActor = GetWorld()->SpawnActorDeferred<ADecalActor>(Decal, Transform);

	if (IsValid(DecalActor) == false || IsValid(DecalActor->GetDecal()) == false) return nullptr;

	if (DecalSize != FVector::ZeroVector)
	{
		FVector Size = DecalSize;
		// x y 교체
		float Temp = Size.Y;
		Size.Y = Size.X;
		Size.X = Temp;

		// x z 교체
		Temp = Size.Z;
		Size.Z = Size.X;
		Size.X = Temp;

		DecalActor->GetDecal()->DecalSize = Size;
	}

	UGameplayStatics::FinishSpawningActor(DecalActor, Transform);

	return DecalActor;
}

TSubclassOf<UCameraShakeBase> UMyGameInstance::GetCameraShake(ECameraShake WhichShake) 
{
	TSubclassOf<UCameraShakeBase>* FindShake = CS_Map.Find(WhichShake);
	if (FindShake == nullptr)
		return TSubclassOf<UCameraShakeBase>();

	return *FindShake;
}

void UMyGameInstance::CurveFloatSlowMotion(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Curve!!"));
	Value = FMath::Clamp(Value, 0.f, 1.f);

	GetWorld()->GetWorldSettings()->SetTimeDilation(Value);
}

void UMyGameInstance::PlaySlowMotionTimeline()
{
	SlowMotionTimeline.PlayFromStart();

	//UE_LOG(LogTemp, Warning, TEXT("MyTimeline!!"));
	GetWorld()->GetTimerManager().SetTimer(SlowMotionTimer, this, &UMyGameInstance::TickSlowMotionTimeline, SlowmotionTickDeltaTime = GetWorld()->DeltaTimeSeconds, true);
}

void UMyGameInstance::TickSlowMotionTimeline()
{
	//UE_LOG(LogTemp, Warning, TEXT("Tick!! %f"), GetWorld()->DeltaTimeSeconds);
	SlowMotionTimeline.TickTimeline(SlowmotionTickDeltaTime);
	/*GetWorld()->GetTimerManager().SetTimer(SlowMotionTimer, this, &UMyGameInstance::TickSlowMotionTimeline, 0.001f, false, GetWorld()->DeltaTimeSeconds);*/
}


