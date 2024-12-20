// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BossController.generated.h"

class UAISenseConfig_Sight;
struct FAIStimulus;

UCLASS()
class CRAZY6_API ABossController : public AAIController
{
	GENERATED_BODY()

public:
	ABossController();

protected:
	void BeginPlay() override;
	void OnPossess(APawn* InPawn) override;
	void OnUnPossess() override;

public:
	void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnTargetDetect(AActor* Target, FAIStimulus Stimulus);
	
	AActor* GetTarget();

private:
	TObjectPtr<UBehaviorTree> mAITree = nullptr;
	TObjectPtr<UBlackboardData> mBlackboardData = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Sense")
	TObjectPtr<UAIPerceptionComponent> mAIPerception = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Sense")
	TObjectPtr<UAISenseConfig_Sight> mSightConfig = nullptr;

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
