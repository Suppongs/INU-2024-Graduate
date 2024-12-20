// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyStatComponent.h"
#include "../Enemy/EnemyDataManager.h"
#include "Net/UnrealNetwork.h"


UEnemyStatComponent::UEnemyStatComponent()
{
	SetIsReplicatedByDefault(true);
}

void UEnemyStatComponent::SetStat(const FEnemyInfoTable* DataTable)
{
	check(DataTable)

	mAttack = DataTable->mAttack;
	mHP = DataTable->mHP;
	mHPMax = DataTable->mHP;
	mAttackRange = DataTable->mAttackRange;
	mStatInfoByState = DataTable->mStatInfoByState;

	SetHp(mHPMax);
}

void UEnemyStatComponent::ChangeStatInfoByState(EAIState State)
{
	const FAIStatInfoByState* StatInfoByState = FindStatInfoByState(State);
	if (nullptr == StatInfoByState)
	{
		return;
	}

	mMoveSpeed = StatInfoByState->mConfiguredMoveSpeed;
	mTurnSpeed = StatInfoByState->mConfiguredTurnSpeed;
}

float UEnemyStatComponent::ApplyDamage(float Damage)
{
	const float fPrevHp = mHP;
	const bool IsDeath = (mHP <= Damage);
	const float CurHp = mHP - Damage;

	if (true == OnHitDamageDelegate.IsBound())
	{
		const float fDamage = IsDeath ? mHP : Damage;
		OnHitDamageDelegate.Broadcast(fDamage);
	}

	SetHp(CurHp);

	if (true == IsDeath)
	{
		if (true == OnHpZERODelegate.IsBound())
		{
			OnHpZERODelegate.Broadcast();
		}
	}

	return CurHp;
}

const FAIStatInfoByState* UEnemyStatComponent::FindStatInfoByState(EAIState State)
{
	if (true == mStatInfoByState.IsEmpty())
	{
		return nullptr;
	}
	
	const FAIStatInfoByState* pResult = mStatInfoByState.Find(State);
	return pResult;
}

void UEnemyStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UEnemyStatComponent, mMoveSpeed);
	DOREPLIFETIME(UEnemyStatComponent, mTurnSpeed);
}

void UEnemyStatComponent::SetHp(float CurHp)
{
	mHP = FMath::Clamp(CurHp, 0.0f, mHPMax);
	if (true == OnChangedHpDelegate.IsBound())
	{
		OnChangedHpDelegate.Broadcast(mHP);
	}
}
