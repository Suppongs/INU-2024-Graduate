// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnergyBall.generated.h"

DECLARE_DELEGATE(FEnergyBallLaunch);

class USoundBase;
class UNiagaraSystem;
class UNiagaraComponent;
class ABossZero;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class CRAZY6_API AEnergyBall : public AActor
{
	GENERATED_BODY()
	
public:
	AEnergyBall();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	FORCEINLINE void SetEnergyBallOwner(ABossZero* _Owner){ mOwner = _Owner;}

private:
	UFUNCTION()
	void OnBegineOverlap_EnergyBall(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnProJectileStop_EnergyBall(const FHitResult& HitResult);

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> mMovement = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mDefaultRange = 10000.f;

	float mAccumulatedRange = 0.f;
	
	FVector mPrevLocation = FVector::ZeroVector;
	FVector mCurLocation = FVector::ZeroVector;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> mEnergyBallHitBox = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ABossZero> mOwner = nullptr;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> mEnergyBallVisual = nullptr;
	
	FEnergyBallLaunch OnEnergyBallLaunch;
	bool IsLaunched = false;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraSystem> mExplosionEffect = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USoundBase> mSpawnSound = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USoundBase> mExplosionSound = nullptr;
};
