#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle,
	Walk,
	Run,
	Attack,
	Death,
};

UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	Phase1,
	Phase2,
	Phase3
};

UENUM(BlueprintType)
enum class EBossAttackMode : uint8
{
	Shooting,
	Laser,
	Rolling,
	Slam,
	Bomb,
	AdvancedLaser
};