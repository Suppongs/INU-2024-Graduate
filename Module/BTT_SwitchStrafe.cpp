// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SwitchStrafe.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTT_SwitchStrafe::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	
	OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsStrafe", bAllowStrafe);

	return Result;
}
