// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyEnum.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimTemplate.generated.h"

UCLASS()
class CRAZY6_API UEnemyAnimTemplate : public UAnimInstance
{
	GENERATED_BODY()

public:
	UEnemyAnimTemplate();

public:	
	void ChangeAnimation(EAIState _Anim)
	{
		mAnimationType = _Anim;
	}

	void SetAnimData(const FName& _Key);

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackMontage();
	
	FORCEINLINE void SetIsHit(bool _IsHit) {IsHit = _IsHit;}
	FORCEINLINE void SetIsMoving(bool _IsMoving) {IsMovingAttack = _IsMoving;}
	FORCEINLINE void SetIsRushAttackReady(bool _IsReady) {IsRushAttackReady = _IsReady;}
	FORCEINLINE void SetAttackMode(EBossAttackMode _Mode) {mAttackMode = _Mode;}

private:
	UFUNCTION()
	void AnimNotify_AttackHitBoxOn();
	
	UFUNCTION()
	void AnimNotify_AttackHitBoxOff();
	
	UFUNCTION()
	void AnimNotify_AttackEnd();

	UFUNCTION()
	void AnimNotify_RushEnd();

	UFUNCTION()
	void AnimNotify_EnergyBallSpawn();
	
	UFUNCTION()
	void AnimNotify_ChargeStart();

	UFUNCTION()
	void AnimNotify_RollingMoveStart();
	
	UFUNCTION()
	void AnimNotify_RollingMoveEnd();

	UFUNCTION()
	void AnimNotify_SlamMoveStart();

	UFUNCTION()
	void AnimNotify_SlamMoveEnd();
	
	UFUNCTION()
	void AnimNotify_BombStart();

	UFUNCTION()
	void AnimNotify_BombEnd();

	UFUNCTION()
	void AnimNotify_AdvancedChargeStart();

	UFUNCTION()
	void AnimNotify_HitEnd();

	UFUNCTION()
	void AnimNotify_DeathEnd();

	UFUNCTION()
	void AnimNotify_FK_Off();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> mAttackMontage = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAIState mAnimationType = EAIState::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FString, UAnimSequence*> mSequenceMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FString, UBlendSpace*> mBlendSpaceMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FString, UAnimMontage*> mMontageMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mMoveDirection = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mMoveSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsMovingAttack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsRushAttackReady = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBossAttackMode mAttackMode = EBossAttackMode::Shooting;

	// == IK (Fabrik) ==
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform mLeftHandSocketTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsUseFabrik = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mFabrikAlpha = 1.0f; // 0 ~ 1

	
};