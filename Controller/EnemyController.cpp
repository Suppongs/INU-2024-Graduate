// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../EnemyBase.h"
#include "Crazy6/DebugMacros.h"

AEnemyController::AEnemyController()
{
	// <=========== Sense Setting ===========>
	mAIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	
	mSightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	mSightConfig->SightRadius = 2000.f;
	mSightConfig->LoseSightRadius = 2500.f;
	mSightConfig->PeripheralVisionAngleDegrees = 80.f;
	mSightConfig->DetectionByAffiliation.bDetectEnemies = true;
	mSightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	mSightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	mAIPerception->ConfigureSense(*mSightConfig);
	SetPerceptionComponent(*mAIPerception);

	// <=========== AI Setting ===========>
	static ConstructorHelpers::FObjectFinder<UBehaviorTree>
		BehaviorTreeAsset(TEXT("/Script/AIModule.BehaviorTree'/Game/Dev/AI/Enemy/BT_Enemy.BT_Enemy'"));
	if (true == BehaviorTreeAsset.Succeeded())
	{
		mAITree = BehaviorTreeAsset.Object;
	}
	else
	{
		LOG_ERROR(TEXT("Failed to load behavior tree asset"));
	}

	static ConstructorHelpers::FObjectFinder<UBlackboardData>
		BlackBoardAsset(TEXT("/Script/AIModule.BlackboardData'/Game/Dev/AI/Enemy/BB_Enemy.BB_Enemy'"));
	if (true == BlackBoardAsset.Succeeded())
	{
		mBlackboardData = BlackBoardAsset.Object;
	}
	else
	{
		LOG_ERROR(TEXT("Failed to load blackboad asset"));
	}
}

void AEnemyController::BeginPlay()
{
	Super::BeginPlay();

	if (true == IsValid(mAIPerception))
	{
		mAIPerception->OnTargetPerceptionUpdated.AddDynamic(this , &ThisClass::OnTargetDetect);
	}
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	// On AI
	if (true == IsValid(mAITree)
		&& true == IsValid(mBlackboardData))
	{
		UBlackboardComponent* BlackboardComponent = nullptr;
		if (true == UseBlackboard(mBlackboardData, BlackboardComponent))
		{
			RunBehaviorTree(mAITree);
		}
	}

	Super::OnPossess(InPawn);
}

void AEnemyController::OnUnPossess()
{
	Super::OnUnPossess();
}

void AEnemyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	AEnemyBase* ControlledPawn = Cast<AEnemyBase>(GetPawn());

	//Strafe Setting
	if (true == Blackboard->GetValueAsBool("IsStrafe"))
	{
		APawn* Target = Cast<APawn>(Blackboard->GetValueAsObject("Target"));
		if (true == IsValid(ControlledPawn) && true == IsValid(Target))
		{
			FVector Direction = Target->GetActorLocation() - ControlledPawn->GetActorLocation();
			Direction.Z = 0;
			FRotator TargetRotation = Direction.Rotation();
		
			FRotator CurrentRotation = ControlledPawn->GetActorRotation();
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 500.f);
			ControlledPawn->SetActorRotation(NewRotation);
		}
	}

	//AnimInstance MoveDirection Setting
	FVector EnemyLocation = ControlledPawn->GetActorLocation();
	FVector Destination = Blackboard->GetValueAsVector("MovePoint");
	FVector DirectionToDestination = Destination - EnemyLocation;

	ControlledPawn->ChangeMovementDirection(ControlledPawn->GetActorForwardVector(), DirectionToDestination);

	//Closest Target Decision
	AActor* ClosestTarget = GetClosestTarget();
	if (true == IsValid(ClosestTarget))
	{
		Blackboard->SetValueAsObject("Target", ClosestTarget);
	}
}

AActor* AEnemyController::GetTargetActor() const
{
	AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject("Target"));
	return Target;
}

void AEnemyController::OnTargetDetect(AActor* Target, FAIStimulus Stimulus)
{
	if (true == Stimulus.WasSuccessfullySensed())
	{
		if (false == mTargetCollection.Contains(Target))
		{
			mTargetCollection.Push(Target);
		}
	}
}

AActor* AEnemyController::GetClosestTarget()
{
	if (mTargetCollection.Num() == 0)
	{
		return nullptr;
	}

	AActor* ClosestTarget = nullptr;
	float MinDistance = FLT_MAX;
	FVector SelfLocation = GetPawn()->GetActorLocation();

	for (AActor* Target : mTargetCollection)
	{
		if (true == IsValid(Target))
		{
			float Distance = FVector::Dist(SelfLocation, Target->GetActorLocation());
			
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				ClosestTarget = Target;
			}
		}
	}

	return ClosestTarget;
}

ETeamAttitude::Type AEnemyController::GetTeamAttitudeTowards(const AActor& Other) const
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
