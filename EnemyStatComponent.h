// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Crazy6/Actor/Enemy/EnemyInfo.h"
#include "EnemyStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZERODelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangedHpDelegate, float /* Health */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHitDamageDelegate, float /* Damage */);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CRAZY6_API UEnemyStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyStatComponent();
	
	/* call bind function, When Enemy Hp is below 0*/
	FOnHpZERODelegate OnHpZERODelegate;
	/* call bind function, When Enemy Hp changed 0*/
	FOnChangedHpDelegate OnChangedHpDelegate;
	/* call bind function, When Enemy Hp changed 
		@Param : float Daamage */
	FOnHitDamageDelegate OnHitDamageDelegate;

	void SetStat(const FEnemyInfoTable* DataTable);
	void ChangeStatInfoByState(EAIState State);

	FORCEINLINE float GetAttack() const { return mAttack; }
	FORCEINLINE float GetHP() const { return mHP; }
	FORCEINLINE float GetMaxHP() const { return mHPMax; }
	FORCEINLINE float GetHPPercentage() const {return mHP / mHPMax * 100;}
	FORCEINLINE float GetAttackRange() const { return mAttackRange; }
	FORCEINLINE void SetAttackRange(float _AttackRange) { mAttackRange = _AttackRange; }
	FORCEINLINE float GetMoveSpeed() const { return mMoveSpeed; }
	FORCEINLINE float GetTurnSpeed() const { return mTurnSpeed; }

	/* Change Enemy Hp by attacking or healing */
	void  SetHp(float CurHp);

	/* Calculate Take Damge and Call SetHp*/
	float ApplyDamage(float Damage);

private:
	const FAIStatInfoByState* FindStatInfoByState(EAIState State);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleAnywhere)
	float mAttack = 0.0f;
	UPROPERTY(VisibleAnywhere)
	float mHP = 0.0f;
	UPROPERTY(VisibleAnywhere)
	float mHPMax = 0.0f;
	UPROPERTY(VisibleAnywhere)
	float mAttackRange = 0.0f;
	UPROPERTY(VisibleAnywhere, Replicated)
	float mMoveSpeed = 0.0f;
	UPROPERTY(VisibleAnywhere, Replicated)
	float mTurnSpeed = 0.0f;
	UPROPERTY(VisibleAnywhere)
	TMap<EAIState, FAIStatInfoByState> mStatInfoByState;

};
