// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayLoopSound.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"

void UPlayLoopSound::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	AudioComponent = UGameplayStatics::SpawnSoundAttached(Sound, MeshComp );
	if (IsValid(AudioComponent))
	{
		AudioComponent->FadeIn(0.5f);
	}
}

void UPlayLoopSound::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void UPlayLoopSound::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (IsValid(AudioComponent) && AudioComponent->IsPlaying())
		AudioComponent->FadeOut(1.f, 0.f);
}
