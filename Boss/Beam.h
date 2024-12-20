// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Beam.generated.h"


class UParticleSystem;
class USoundBase;
class UNiagaraComponent;
class UNiagaraSystem;
class UCapsuleComponent;

UCLASS()
class CRAZY6_API UBeam : public USceneComponent
{
	GENERATED_BODY()

public:
	UBeam();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UFUNCTION()
	void OnLaserChargeEnd(UNiagaraComponent* FinishedComponent);
	UFUNCTION()
	void OnLaserEnd(UNiagaraComponent* FinishedComponent);

	void ApplyBeamDamage(AActor* OtherActor);

	UFUNCTION()
	void OnBegineOverlap_Beam(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap_Beam(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> mBeamHitBox;

	FTimerHandle mContinuousDamageTimer;
	float mDamageInterval = 0.2f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UParticleSystem> mDotHitParticle = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USoundBase> mBeamSound = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USoundBase> mDotHitSound = nullptr;

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> mBeamHead = nullptr;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> mBeamBody = nullptr;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> mBeamTail = nullptr;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> mBeamIndicator = nullptr;
};
