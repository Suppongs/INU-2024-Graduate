// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_CheckEnemyClass.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Crazy6/Actor/Enemy/Epic/EpicCrunch.h"
#include "Crazy6/Actor/Enemy/Humanoid/HumanoidGunMan.h"
#include "Crazy6/Actor/Enemy/Humanoid/HumanoidSwordMan.h"


bool UBTD_CheckEnemyClass::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool Result = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	AHumanoidGunMan* ControlledGunMan = Cast<AHumanoidGunMan>(OwnerComp.GetAIOwner()->GetPawn());
	if (true == IsValid(ControlledGunMan))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsClass("EnemyClass", ControlledGunMan->StaticClass());
	}
	else
	{
		AHumanoidSwordMan* ControlledSwordMan = Cast<AHumanoidSwordMan>(OwnerComp.GetAIOwner()->GetPawn());
		if (true == IsValid(ControlledSwordMan))
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsClass("EnemyClass", ControlledSwordMan->StaticClass());
		}
		else
		{
			AEpicCrunch* ControlledEpic = Cast<AEpicCrunch>(OwnerComp.GetAIOwner()->GetPawn());
			if (true == IsValid(ControlledEpic))
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsClass("EnemyClass", ControlledEpic->StaticClass());
			}
		}
	}
	
	Result = mConfiguredEnemyClass == OwnerComp.GetBlackboardComponent()->GetValueAsClass("EnemyClass");
	
	return Result;
}
