// Fill out your copyright notice in the Description page of Project Settings.


#include "AxeESkill.h"

void AAxeESkill::BeginPlay()
{
	Super::BeginPlay();
}

void AAxeESkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator CurRotation = GetActorRotation();

	if (bLeftRotateAxe)
		CurRotation.Yaw -= DeltaTime * 1700.f;
	else
		CurRotation.Yaw += DeltaTime * 1700.f;

	SetActorRotation(CurRotation);
}

void AAxeESkill::OnOverlapBegin_Implementation(AActor* OtherActor)
{
}

void AAxeESkill::OnOverlapEnd_Implementation(AActor* OtherActor)
{
}

void AAxeESkill::OnOverlapping_Implementation(float DeltaTime)
{
}

void AAxeESkill::SetLeftRotateAxe(bool bLeftRotate)
{
	bLeftRotateAxe = bLeftRotate;
}
