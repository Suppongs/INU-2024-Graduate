// Fill out your copyright notice in the Description page of Project Settings.


#include "BeginPlayAISpawner.h"


void ABeginPlayAISpawner::BeginPlay()
{
	Super::BeginPlay();

	switch (GetWorld()->GetNetMode())
	{
	case NM_Standalone:
	case NM_ListenServer:
	case NM_DedicatedServer:
		Spawn();
		break;
	}


}