
#include "BGMTriggerVolume.h"
#include "Components/BoxComponent.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"
#include "Components/AudioComponent.h"
#include "MyRPG/Types/ChannelType.h"

ABGMTriggerVolume::ABGMTriggerVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("BGMTriggerBox");
	if (CollisionBox)
	{
		CollisionBox->SetupAttachment(RootComponent);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECC_Creature, ECollisionResponse::ECR_Overlap);
	}
}

void ABGMTriggerVolume::PlayEndTriggerSound()
{
	USoundWave** FindSound = SoundMap.Find(EBGMTriggerTiming::EBTT_End);
	if (FindSound == nullptr)
	{
		return;
	}
	float* FindVolume = SoundVolmueMap.Find(EBGMTriggerTiming::EBTT_End);
	if (FindVolume == nullptr)
	{
		return;
	}

	if (CurAudioComponent.IsValid())
	{
		CurAudioComponent->Stop();
		CurAudioComponent->DestroyComponent();
	}

	UAudioComponent* EndAudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), *FindSound);
	if (IsValid(EndAudioComponent))
	{
		EndAudioComponent->FadeIn(FadeInTime, *FindVolume);
	}
}

void ABGMTriggerVolume::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABGMTriggerVolume::CollisionOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ABGMTriggerVolume::CollisionOverlapEnd);
}

void ABGMTriggerVolume::SpawnPreceduralSound(EBGMTriggerTiming BGMTiming)
{
	//CurrentBGMTiming = EBGMTriggerTiming::EBTT_Start;
	USoundWave** FindSound = SoundMap.Find(BGMTiming);
	if (FindSound == nullptr)
	{
		return;
	}
	float* FindVolume = SoundVolmueMap.Find(BGMTiming);
	if (FindVolume == nullptr)
	{
		return;
	}
	
	CurAudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), *FindSound);
	if (CurAudioComponent.IsValid())
	{
		CurAudioComponent->FadeIn(FadeInTime, *FindVolume);
	}

	// 다음 사운드 예약

	uint8 NextBGMTiming = (uint8)BGMTiming + 1;
	if (NextBGMTiming > (uint8)EBGMTriggerTiming::EBTT_End || SoundFadeInDelayTimeMap.Find((EBGMTriggerTiming)NextBGMTiming) == nullptr)
	{
		return;
	}


	FTimerDelegate TimerDele;
	TimerDele.BindUFunction(this, "SpawnPreceduralSound", (EBGMTriggerTiming)NextBGMTiming);

	float WaitTime = (SoundFadeInDelayTimeMap[(EBGMTriggerTiming)NextBGMTiming] == 0.f) ? 0.01f : SoundFadeInDelayTimeMap[(EBGMTriggerTiming)NextBGMTiming];
	GetWorld()->GetTimerManager().SetTimer(SpawnPreceduralSoundTimer, TimerDele, (*FindSound)->GetDuration() + WaitTime, false);
}

void ABGMTriggerVolume::CollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<APlayerCharacter>(OtherActor) == nullptr)
	{
		return;
	}

	if (bProcedural == false)
	{
		CurAudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), Sound);

		if (CurAudioComponent.IsValid())
		{
			CurAudioComponent->FadeIn(FadeInTime, Volume);
		}
	}
	else
	{
		FTimerDelegate TimerDele;
		TimerDele.BindUFunction(this, "SpawnPreceduralSound", EBGMTriggerTiming::EBTT_Start);

		float WaitTime = (SoundFadeInDelayTimeMap[EBGMTriggerTiming::EBTT_Start] <= 0.f) ? 0.01f : SoundFadeInDelayTimeMap[EBGMTriggerTiming::EBTT_Start];
		GetWorld()->GetTimerManager().SetTimer(SpawnPreceduralSoundTimer, TimerDele, WaitTime, false);
	}
}

void ABGMTriggerVolume::CollisionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<APlayerCharacter>(OtherActor) == nullptr)
	{
		return;
	}

	if (CurAudioComponent.IsValid())
	{
		CurAudioComponent->FadeOut(FadeOutTime, 0.f);
	}
}

