// Fill out your copyright notice in the Description page of Project Settings.


#include "BossController.h"
#include "Crazy6/DebugMacros.h"
#include "Perception/AIPerceptionTypes.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ABossController::ABossController()
{// <=========== Sense Setting ===========>
	mAIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	
	mSightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	mSightConfig->SightRadius = 5000.f;
	mSightConfig->LoseSightRadius = 10000.f;
	mSightConfig->PeripheralVisionAngleDegrees = 180.f;
	mSightConfig->DetectionByAffiliation.bDetectEnemies = true;
	mSightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	mSightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	mAIPerception->ConfigureSense(*mSightConfig);
	SetPerceptionComponent(*mAIPerception);
	
	// <=========== AI Setting ===========>
	static ConstructorHelpers::FObjectFinder<UBehaviorTree>
		BehaviorTreeAsset(TEXT("/Script/AIModule.BehaviorTree'/Game/Dev/AI/Enemy/Boss/BT_Boss.BT_Boss'"));
	if (true == BehaviorTreeAsset.Succeeded())
	{
		mAITree = BehaviorTreeAsset.Object;
	}
	else
	{
		LOG_ERROR(TEXT("Failed to load behavior tree asset"));
	}

	static ConstructorHelpers::FObjectFinder<UBlackboardData>
		BlackBoardAsset(TEXT("/Script/AIModule.BlackboardData'/Game/Dev/AI/Enemy/Boss/BB_Boss.BB_Boss'"));
	if (true == BlackBoardAsset.Succeeded())
	{
		mBlackboardData = BlackBoardAsset.Object;
	}
	else
	{
		LOG_ERROR(TEXT("Failed to load blackboad asset"));
	}
}

void ABossController::BeginPlay()
{
	Super::BeginPlay();

	if (true == IsValid(mAIPerception))
	{
		mAIPerception->OnTargetPerceptionUpdated.AddDynamic(this , &ThisClass::OnTargetDetect);
	}
}

void ABossController::OnPossess(APawn* InPawn)
{
	if (true == IsValid(mAITree) && true == IsValid(mBlackboardData))
	{
		UBlackboardComponent* BlackboardComponent = nullptr;
		if (true == UseBlackboard(mBlackboardData, BlackboardComponent))
		{
			RunBehaviorTree(mAITree);
		}
	}

	Super::OnPossess(InPawn);
}

void ABossController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ABossController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABossController::OnTargetDetect(AActor* Target, FAIStimulus Stimulus)
{
	if (true == Stimulus.WasSuccessfullySensed())
	{
		Blackboard->SetValueAsObject("Target", Target);
	}
}

AActor* ABossController::GetTarget()
{
	return Cast<AActor>(Blackboard->GetValueAsObject("Target"));
}

ETeamAttitude::Type ABossController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(&Other);
	if (nullptr == OtherTeamAgent)
	{
		return ETeamAttitude::Neutral;
	}
	else if (FGenericTeamId::NoTeam == OtherTeamAgent->GetGenericTeamId())
	{
		return ETeamAttitude::Neutral;
	}
	else if (GetGenericTeamId() == OtherTeamAgent->GetGenericTeamId())
	{
		return ETeamAttitude::Friendly;
	}

	return ETeamAttitude::Hostile;
}
