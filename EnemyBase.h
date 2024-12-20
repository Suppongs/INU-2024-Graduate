// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAnimTemplate.h"
#include "GameFramework/Pawn.h"
#include "EnemyEnum.h"
#include "EnemyStatComponent.h"
#include "Blueprint/UserWidget.h"
#include "Crazy6/Item/ItemBase.h"
#include "EnemyBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FAIDeathDelegate);
DECLARE_DELEGATE(FAIAttackEnd);

class UEnemyStatComponent;
class UCapsuleComponent;
class UFloatingPawnMovement;
class UWidgetComponent;
class UMonsterHUDWidget;
	
UCLASS()
class CRAZY6_API AEnemyBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyBase();

public:
	/* Getter and Setter */
	FORCEINLINE UCapsuleComponent* GetCapsule() const { return mCapsule; }
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return mMesh; }
	
	FORCEINLINE EAIState GetAIState() const { return mState; }

	FORCEINLINE UEnemyAnimTemplate* GetAnimInstance() const { return mAnimInstance; }

	FORCEINLINE void SetIsHeadShot(bool _IsHeadShot) {IsHeadShot = _IsHeadShot;}
	FORCEINLINE bool GetIsHeadShot() {return IsHeadShot;}
	
	FORCEINLINE float GetServerMoveDirection() {return mServerMoveDirection;}

	FORCEINLINE void SetServerIsHit(bool _IsHit) {mServerIsHit = _IsHit;}


protected:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void PossessedBy(AController* NewController) override;
	void Tick(float DeltaTime) override;

	// AI
public:
	/* Update current AI State use for enemy animation and movement variable */
	virtual void SetAIState(EAIState State);

	/* Get Enemy power */
	float GetAttack();
	/* Get Attack range, if out of range to player, enemy switch to trace state */
	float GetAttackRange();
	FORCEINLINE void SetAttackRange(float _AttackRange) {mStat->SetAttackRange(_AttackRange);}
	/* get turn speed for looking target */
	float GetTurnSpeed();
	
	/* This is virtual function for Attack Logic */
	virtual void Attack();
	
	/* set deletegation, when enemy done attack */
	void SetAttackEndDelegate(const FAIAttackEnd& OnAttackFinished);
	virtual void AttackHitBoxOn();
	virtual void AttackHitBoxOff();
	virtual void SetIsRushing(bool _IsRushing);
	virtual void AttackEnd();
	float TakeDamage(float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

	/* regist delegate, when AI Death */
	template<typename ObjectType>
	void AddDeathAIDelegate(ObjectType* pObject, void(ObjectType::* Func)())
	{
		mDeathDelegate.AddUObject(pObject, Func);
	}

	virtual void CallBack_Hit(float Damage) {};
	virtual void CallBack_HpZERO();

	/* Ragdoll Logic */
	void StartRagdoll();
	/* Use TimerManager for enemy dead */
	void SetDeathTimer(float DelayTime);
	/*Item Drop*/
	void ItemDrop();

	/* regist GC for release */
	void Death();

	void ChangeMovementDirection(const FVector& ForwardVec, const FVector& Velocity);

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_mServerAnimationType();

	UFUNCTION()
	void OnRep_mServerIsHit();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	EAIState mState = EAIState::Idle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName mEnemyKey = "";
	
	UPROPERTY(VisibleAnywhere, Replicated)
	TObjectPtr<USkeletalMeshComponent> mMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Replicated)
	TObjectPtr<UCapsuleComponent> mCapsule = nullptr;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UEnemyStatComponent> mStat = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UEnemyAnimTemplate> mAnimInstance = nullptr;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_mServerAnimationType)
	EAIState mServerAnimationType = EAIState::Idle;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_mServerIsHit)
	bool mServerIsHit = false;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFloatingPawnMovement> mMovement = nullptr;

	UPROPERTY(VisibleAnywhere, Replicated)
	float mServerMoveDirection = 0.f;

	FAIAttackEnd OnAttackFininsed;
	FAIDeathDelegate mDeathDelegate;

protected:
	// FK
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseFabrik  = true;

	//UI
public:
	//HP
	void CheckHPFunctionTime();
	UFUNCTION()
	void HPTriggerVisibleFalse();

	//Damage
	void CheckDamageFunctionTime();
	
	UFUNCTION()
	void DamageTriggerDeleteFunction();

private:
	UFUNCTION(NetMulticast, Reliable)
	void Callback_SetUIHp(float CurHp);

	UFUNCTION(NetMulticast, Reliable)
	void Callback_OutputDamageIndicator(float Damage);

private:
	//HP
	FTimerHandle CheckHPTimerHandle;
	float CheckHPTime = 3.0f;  // 3 seconds 

	//UI
	FTimerHandle CheckDamageTimerHandle;
	float CheckDamageTime = 1.2f;  //  seconds
	
	TArray<UWidgetComponent*> ActiveDamageWidgets;

	bool IsHeadShot = false;

protected:
	// HUD
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> mMonsterHUDWidgetComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMonsterHUDWidget> MonsterHPInstance;

	// DamageIndicator
	FWidgetAnimationDynamicEvent AnimationEndEvent;
	TObjectPtr<UWidgetAnimation> Animation;
	TObjectPtr<UWidgetAnimation> HeadshotAnimation;

public:
	bool bDamageUICheckKey = false;
};