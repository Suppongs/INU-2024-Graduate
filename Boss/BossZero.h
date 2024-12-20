// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../EnemyBase.h"
#include "BossZero.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class UParticleSystem;
class USoundBase;
class ABomb;
class UBoxComponent;
class USphereComponent;
class UBeam;
class AEnergyBall;
/**
 * 
 */
UCLASS()
class CRAZY6_API ABossZero : public AEnemyBase
{
	GENERATED_BODY()

public:
	ABossZero();

protected:
	void BeginPlay() override;

public:
	void PossessedBy(AController* NewController) override;
	void Tick(float DeltaTime) override;

public:
	void SetAIState(EAIState _eType) override;

	void Attack() override;
	void CallBack_Hit(float Damage) override;
	void CallBack_HpZERO() override;

	void ResumeMontage();
	
	void Shooting();
	void LaunchEnergyBall(AEnergyBall* EnergyBall);
	void Laser();
	void Rolling();
	void Slam();
	void Bomb();
	void SpawnBomb();
	void AdvancedLaser();

private:
	void SwitchAttackMode();

	UFUNCTION()
	void OnBegineOverlap_Rolling(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnBegineOverlap_Slam(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void OnRep_mServerAttackMode();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> mBossHitBox = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> mArmorHitBox = nullptr;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> mRollingHitBox = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> mSlamHitBox = nullptr;
	
	UPROPERTY(EditAnywhere) //replicateUsing
	EBossAttackMode mServerAttackMode = EBossAttackMode::Shooting;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AActor> mTarget = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnergyBall> mEnergyBallClass = nullptr;
	UPROPERTY(EditAnywhere)
	float mEnergyBallLaunchSpeed = 3000.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBeam> mBeam;

	bool IsRollingMove = false;
	
	bool IsSlamMove = false;
	FVector mSlamDestination = FVector::ZeroVector;

	FTimerHandle mBombTimer;
	float mBombTime = 0.5f;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABomb> mBombClass = nullptr;

	FTimerHandle mAdvancedLaserMoveTimer;
	float mAdvancedLaserMovingTime = 2.f;
	bool IsAdvancedLaserMove = false;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USoundBase> mLaserChargeSound = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UParticleSystem> mRollingHitParticle = nullptr;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USoundBase> mRollingSound = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USoundBase> mRollingHitSound = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraSystem> mSlamFX = nullptr;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USoundBase> mSlamSound = nullptr;

public:
	float CloseProb = 0.f;
	float FarProb = 0.f;
	float MidProb = 0.f;
	
	float InitialHitRate = 0.5f;
	float ShootingProb = 0.f;
	float SlamProb = 0.f;
	float BombProb = 0.f;

	int ShootingSuccessCount = 0;
	int ShootingAttemptCount = 0;

	int SlamSuccessCount = 0;
	int SlamAttemptCount = 0;

	int BombSuccessCount = 0;
	int BombAttemptCount = 0;

	FTimerHandle mPlayTimer;
	float mPlayTime = 600.f;
};
