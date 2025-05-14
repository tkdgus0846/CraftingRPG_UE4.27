#pragma once

UENUM(BlueprintType)
enum class ECameraShake : uint8
{
	ECS_None UMETA(DisplayName = "None"),
	ECS_AttackActor UMETA(DisplayName = "AttackActor"),
	ECS_Explosion UMETA(DisplayName = "Explosion"),

	ECS_MAX UMETA(DisplayName = "Default MAX")
};