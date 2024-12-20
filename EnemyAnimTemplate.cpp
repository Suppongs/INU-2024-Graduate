// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimTemplate.h"
#include "EnemyBase.h"
#include "EnemyInfo.h"
#include "EnemyDataManager.h"
#include "Boss/BossZero.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/MovementComponent.h"

UEnemyAnimTemplate::UEnemyAnimTemplate()
{
	
}

void UEnemyAnimTemplate::SetAnimData(const FName& _Key)
{
	const FEnemyAnimData* pAnimData = CEnemyDataManager::GetInst()->FindAnim(_Key);
	if (nullptr != pAnimData)
	{
		mSequenceMap = pAnimData->mSequenceMap;
		mBlendSpaceMap = pAnimData->mBlendSpaceMap;
		mMontageMap = pAnimData->mMontageMap;
	}
}

void UEnemyAnimTemplate::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	AEnemyBase* EnemyPawn = Cast<AEnemyBase>(TryGetPawnOwner());
	if (true == IsValid(EnemyPawn))
	{
		UFloatingPawnMovement* Movement = Cast<UFloatingPawnMovement>(EnemyPawn->GetMovementComponent());
		if (true == IsValid(Movement))
		{
			mMoveSpeed = Movement->Velocity.Length();
			mMoveDirection = EnemyPawn->GetServerMoveDirection();
		}
	}
}

void UEnemyAnimTemplate::PlayAttackMontage()
{
	Montage_Play(mAttackMontage);
}

void UEnemyAnimTemplate::AnimNotify_AttackHitBoxOn()
{
	switch (GetWorld()->GetNetMode())
	{
	case NM_Standalone:
	case NM_ListenServer:
	case NM_DedicatedServer:
		{
			AEnemyBase* EnemyPawn = Cast<AEnemyBase>(TryGetPawnOwner());
			EnemyPawn->AttackHitBoxOn();
		}
		break;
	default:
		break;
	}
}

void UEnemyAnimTemplate::AnimNotify_AttackHitBoxOff()
{
	switch (GetWorld()->GetNetMode())
	{
	case NM_Standalone:
	case NM_ListenServer:
	case NM_DedicatedServer:
		{
			AEnemyBase* EnemyPawn = Cast<AEnemyBase>(TryGetPawnOwner());
			EnemyPawn->AttackHitBoxOff();
		}
		break;
	default:
		break;
	}
}

void UEnemyAnimTemplate::AnimNotify_AttackEnd()
{
	switch (GetWorld()->GetNetMode())
	{
	case NM_Standalone:
	case NM_ListenServer:
	case NM_DedicatedServer:
		{
			AEnemyBase* EnemyPawn = Cast<AEnemyBase>(TryGetPawnOwner());
			EnemyPawn->AttackEnd();
		}
		break;
	default:
		break;
	}

	Montage_Stop(0.1, mAttackMontage);
}

void UEnemyAnimTemplate::AnimNotify_RushEnd()
{
	switch (GetWorld()->GetNetMode())
	{
	case NM_Standalone:
	case NM_ListenServer:
	case NM_DedicatedServer:
		{
			AEnemyBase* EnemyPawn = Cast<AEnemyBase>(TryGetPawnOwner());
			EnemyPawn->SetIsRushing(false);
		}
		break;
	default:
		break;
	}
}

void UEnemyAnimTemplate::AnimNotify_EnergyBallSpawn()
{
	ABossZero* BossPawn = Cast<ABossZero>(TryGetPawnOwner());
	BossPawn->Shooting();	
}

void UEnemyAnimTemplate::AnimNotify_ChargeStart()
{
	ABossZero* BossPawn = Cast<ABossZero>(TryGetPawnOwner());
	BossPawn->Laser();
}

void UEnemyAnimTemplate::AnimNotify_RollingMoveStart()
{
	ABossZero* BossPawn = Cast<ABossZero>(TryGetPawnOwner());
	BossPawn->Rolling();
}

void UEnemyAnimTemplate::AnimNotify_RollingMoveEnd()
{
	ABossZero* BossPawn = Cast<ABossZero>(TryGetPawnOwner());
	BossPawn->Rolling();
}

void UEnemyAnimTemplate::AnimNotify_SlamMoveStart()
{
	ABossZero* BossPawn = Cast<ABossZero>(TryGetPawnOwner());
	BossPawn->Slam();
}

void UEnemyAnimTemplate::AnimNotify_SlamMoveEnd()
{
	ABossZero* BossPawn = Cast<ABossZero>(TryGetPawnOwner());
	BossPawn->Slam();
}

void UEnemyAnimTemplate::AnimNotify_BombStart()
{
	ABossZero* BossPawn = Cast<ABossZero>(TryGetPawnOwner());
	BossPawn->Bomb();
}

void UEnemyAnimTemplate::AnimNotify_BombEnd()
{
	ABossZero* BossPawn = Cast<ABossZero>(TryGetPawnOwner());
	BossPawn->Bomb();
}

void UEnemyAnimTemplate::AnimNotify_AdvancedChargeStart()
{
	ABossZero* BossPawn = Cast<ABossZero>(TryGetPawnOwner());
	BossPawn->AdvancedLaser();
}

void UEnemyAnimTemplate::AnimNotify_HitEnd()
{
	switch (GetWorld()->GetNetMode())
	{
	case NM_Standalone:
	case NM_ListenServer:
	case NM_DedicatedServer:
		{
			IsHit = false;
			AEnemyBase* EnemyPawn = Cast<AEnemyBase>(TryGetPawnOwner());
			EnemyPawn->SetServerIsHit(IsHit);
		}
		break;
	default:
		break;
	}
}

void UEnemyAnimTemplate::AnimNotify_DeathEnd()
{
	AEnemyBase* EnemyPawn = Cast<AEnemyBase>(TryGetPawnOwner());

	EnemyPawn->StartRagdoll();
	
	switch (GetWorld()->GetNetMode())
	{
	case NM_Standalone:
	case NM_ListenServer:
	case NM_DedicatedServer:
		{
			EnemyPawn->SetDeathTimer(3.0f);
		}
		break;
	default:
		break;
	}
	
}

void UEnemyAnimTemplate::AnimNotify_FK_Off()
{
	IsUseFabrik = false;
}


