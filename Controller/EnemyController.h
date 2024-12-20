// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

struct FAIStimulus;
class UAISenseConfig_Sight;
class UAISenseConfig_CloseSight;

UCLASS()
class CRAZY6_API AEnemyController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyController();

protected:
	void BeginPlay() override;
	void OnPossess(APawn* InPawn) override;
	void OnUnPossess() override;

public:
	void Tick(float DeltaTime) override;

	AActor* GetTargetActor() const;
	FORCEINLINE TArray<AActor*> GetTargetCollection() { return mTargetCollection; }
	FORCEINLINE void PushTargetCollection(AActor* Target) { mTargetCollection.Push(Target); }

	// AI Section
	UFUNCTION()
	void OnTargetDetect(AActor* Target, FAIStimulus Stimulus);

private:
	AActor* GetClosestTarget();

private:
	TObjectPtr<UBehaviorTree> mAITree = nullptr;
	TObjectPtr<UBlackboardData> mBlackboardData = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Sense")
	TObjectPtr<UAIPerceptionComponent> mAIPerception = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Sense")
	TObjectPtr<UAISenseConfig_Sight> mSightConfig = nullptr;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> mTargetCollection;

//TeamInterface
public:
	template<typename EnumType>
	FORCEINLINE void SetTeamID(EnumType ID) { mTeamID = static_cast<uint8>(ID); }

	FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(mTeamID); }

protected:
	/* for judge team */
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

private:
	uint8 mTeamID = 0;

};
