// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyBase.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "EnemyDataManager.h"
#include "EnemyStatComponent.h"
#include "Controller/EnemyController.h"
#include "Components/WidgetComponent.h"
#include "Crazy6/Widget/MonsterHPWidget.h"
#include "Crazy6/Widget/MonsterHUDWidget.h"
#include "../../../Crazy6/Widget/DamageWidgetData.h"
#include "Crazy6/Global/ProfileInfo.h"
#include "Crazy6/Actor/TeamEnum.h"
#include "Net/UnrealNetwork.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = true;
	
	mCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	mCapsule->SetCollisionProfileName(UE_COLLISION_PROFILE_ENEMY);
	
	mCapsule->SetCanEverAffectNavigation(false);

	mCapsule->BodyInstance.bLockRotation = true;
	mCapsule->BodyInstance.bLockXRotation = true;
	mCapsule->BodyInstance.bLockYRotation = true;
	mCapsule->BodyInstance.bLockZRotation = false;
	SetRootComponent(mCapsule);

	// <======= Mesh Setting =======>
	mMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	mMesh->SetupAttachment(mCapsule);
	mMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// <======= AI Setting =======>
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AEnemyController::StaticClass();

	// <======== Movement Setting ==========>
	mMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	mMovement->SetUpdatedComponent(RootComponent);
	mMovement->MaxSpeed = 600.f;
	mMovement->Acceleration = 1000.f;

	// <======== Stat Setting ==========>
	mStat = CreateDefaultSubobject<UEnemyStatComponent>(TEXT("AIStatComponent"));

	/*Monster HP HUD Setting*/
	mMonsterHUDWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("MonsterHUDWidget"));

	{
		static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Dev/UI/Widget/MonsterHUDWidget.MonsterHUDWidget_C'"));
		if (WidgetClass.Succeeded())
		{
			mMonsterHUDWidgetComponent->SetWidgetClass(WidgetClass.Class);
		}

		mMonsterHUDWidgetComponent->SetupAttachment(mCapsule);
		mMonsterHUDWidgetComponent->SetDrawSize(FVector2D(173.0f, 36.0f));
		mMonsterHUDWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		mMonsterHUDWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
		mMonsterHUDWidgetComponent->SetVisibility(false);

	}

	
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	bReplicates = true;
	mCapsule->SetIsReplicated(true);
	mMesh->SetIsReplicated(true);

	const FEnemyInfoTable* pEnemyInfo = CEnemyDataManager::GetInst()->FindInfo(mEnemyKey);
	check(pEnemyInfo);
	mStat->SetStat(pEnemyInfo);

	mStat->OnChangedHpDelegate.AddUObject(this, &ThisClass::CallBack_Hit);
	mStat->OnChangedHpDelegate.AddUObject(this, &ThisClass::Callback_SetUIHp);
	mStat->OnHitDamageDelegate.AddUObject(this, &ThisClass::Callback_OutputDamageIndicator);
	mStat->OnHpZERODelegate.AddUObject(this, &ThisClass::CallBack_HpZERO);

	// HUD view
	if (nullptr != mMonsterHUDWidgetComponent
		&& mMonsterHUDWidgetComponent->GetWidget())
	{
		MonsterHPInstance = Cast<UMonsterHUDWidget>(mMonsterHUDWidgetComponent->GetWidget());
	}
}

void AEnemyBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (true == mDeathDelegate.IsBound())
	{
		mDeathDelegate.Broadcast();
	}
}

void AEnemyBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AEnemyController* pAIController = Cast<AEnemyController>(NewController);
	if (true == IsValid(pAIController))
	{
		pAIController->SetTeamID(ETeamID::ENEMY);
	}
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch(mState)
	{
	case EAIState::Walk:
	case EAIState::Run:
	case EAIState::Attack:
		{
			FHitResult HitResult;
			FVector Start = GetActorLocation();
			FVector End = Start - FVector(0.0f, 0.0f, 1000.0f);
			bool Hit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel7);
			if (true == Hit)
			{
				FVector GroundLocation = GetActorLocation();
				GroundLocation.Z = HitResult.Location.Z + mCapsule->GetScaledCapsuleHalfHeight();
				SetActorLocation(GroundLocation);
			}
		}
	}
}

void AEnemyBase::SetAIState(EAIState State)
{
	mState = State;
	mStat->ChangeStatInfoByState(State);

	mMovement->MaxSpeed = mStat->GetMoveSpeed();
}

float AEnemyBase::GetAttack()
{
	return mStat->GetAttack();
}

float AEnemyBase::GetAttackRange()
{
	const float fAtttackRange = mStat->GetAttackRange();
	const float HalfCapsuleRadius = mCapsule->GetScaledCapsuleRadius();
	const float fResult = fAtttackRange + HalfCapsuleRadius;
	return fResult;
}

float AEnemyBase::GetTurnSpeed()
{
	return mStat->GetTurnSpeed();
}

void AEnemyBase::Attack()
{
	mCapsule->SetCanEverAffectNavigation(true);
}

void AEnemyBase::SetAttackEndDelegate(const FAIAttackEnd& InOnAttackFinished)
{
	OnAttackFininsed = InOnAttackFinished;
}

void AEnemyBase::AttackHitBoxOn()
{
}

void AEnemyBase::AttackHitBoxOff()
{
}

void AEnemyBase::SetIsRushing(bool _IsRushing)
{
}

void AEnemyBase::AttackEnd()
{
	mCapsule->SetCanEverAffectNavigation(false);
	OnAttackFininsed.ExecuteIfBound();
}

float AEnemyBase::TakeDamage(float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent,
	                                 EventInstigator, DamageCauser);
	mStat->ApplyDamage(DamageAmount);

	AEnemyController* AIController = Cast<AEnemyController>(GetController());
	if (true == IsValid(AIController))
	{
		if (false == AIController->GetTargetCollection().Contains(DamageCauser))
		{
			AIController->PushTargetCollection(DamageCauser);
		}
	}

	return DamageAmount;
}

void AEnemyBase::CallBack_HpZERO()
{
	ensure(mCapsule);
	mCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyBase::StartRagdoll()
{
	//AddActorWorldOffset(FVector::UpVector * 3.0f);
	
	mCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//mCapsule->SetCollisionProfileName(UE_COLLISION_PROFILE_RAGDOLL);
	
	mMesh->SetCollisionProfileName(UE_COLLISION_PROFILE_RAGDOLL);
	mMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	mMesh->SetSimulatePhysics(true);

	if (true == HasAuthority())
	{
		ItemDrop();
	}
}

void AEnemyBase::SetDeathTimer(float DelayTime)
{
	FTimerHandle DeathTimerHandle;

	GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &AEnemyBase::Death, DelayTime, false);
}

void AEnemyBase::ItemDrop()
{
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = FRotator::ZeroRotator;

	// Linetrace 
	FVector TraceEnd = SpawnLocation - FVector(0, 0, 1000);
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, SpawnLocation, TraceEnd, ECC_Visibility, Params))
	{
		FVector GroundLocation = HitResult.Location;
		// spawn
		AItemBase* DroppedItem = GetWorld()->SpawnActor<AItemBase>(AItemBase::StaticClass(), GroundLocation, SpawnRotation);
	}
}

void AEnemyBase::Death()
{
	Destroy();
}

void AEnemyBase::ChangeMovementDirection(const FVector& ForwardVec, const FVector& Velocity)
{
	FVector ForwardVector = ForwardVec;
	FVector VelocityVector = Velocity;
	if (!Velocity.IsNearlyZero())
	{
		FVector NormalForwardVector = ForwardVector.GetSafeNormal();
		FVector NormalVelocityVector = VelocityVector.GetSafeNormal();

		float AngleInRadians = FMath::Atan2(NormalVelocityVector.Y, NormalVelocityVector.X) - FMath::Atan2(NormalForwardVector.Y, NormalForwardVector.X);
		float AngleInDegrees = FMath::RadiansToDegrees(AngleInRadians);

		float FinalAngle = FMath::Fmod(AngleInDegrees + 360.0f, 360.0f);
		if (FinalAngle > 180.0f)
		{
			FinalAngle -= 360.0f;
		}
		
		mServerMoveDirection = FinalAngle;
	}
}

void AEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyBase, mState);
	DOREPLIFETIME(AEnemyBase, mMesh);
	DOREPLIFETIME(AEnemyBase, mCapsule);
	DOREPLIFETIME(AEnemyBase, mServerAnimationType);
	DOREPLIFETIME(AEnemyBase, mServerMoveDirection);
	DOREPLIFETIME(AEnemyBase, mServerIsHit);
}

void AEnemyBase::OnRep_mServerAnimationType()
{
	if (true == IsValid(mAnimInstance))
	{
		mAnimInstance->ChangeAnimation(mServerAnimationType);
	}

	mMovement->MaxSpeed = mStat->GetMoveSpeed();
}

void AEnemyBase::OnRep_mServerIsHit()
{
	mAnimInstance->SetIsHit(mServerIsHit);
}

void AEnemyBase::CheckHPFunctionTime()
{
	GetWorld()->GetTimerManager().ClearTimer(CheckHPTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(CheckHPTimerHandle, this, &AEnemyBase::HPTriggerVisibleFalse, CheckHPTime, false);
}

void AEnemyBase::HPTriggerVisibleFalse()
{
	mMonsterHUDWidgetComponent->SetVisibility(false);
	bDamageUICheckKey = false;
}

void AEnemyBase::CheckDamageFunctionTime()
{
	GetWorld()->GetTimerManager().ClearTimer(CheckDamageTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(CheckDamageTimerHandle, this, &AEnemyBase::DamageTriggerDeleteFunction, CheckDamageTime, false);
}

void AEnemyBase::DamageTriggerDeleteFunction()
{
	if (ActiveDamageWidgets.Num() > 0)
	{
		UWidgetComponent* FinishedWidget = ActiveDamageWidgets[0];
		if (FinishedWidget)
		{
			FinishedWidget->UnregisterComponent();
			FinishedWidget->DestroyComponent();
		}
		ActiveDamageWidgets.RemoveAt(0);
	}
}

void AEnemyBase::Callback_SetUIHp_Implementation(float CurHp)
{
	if (true == bDamageUICheckKey)
	{
		CheckHPFunctionTime();
		const float fMaxHp = mStat->GetMaxHP();
		const float fPercent = CurHp / fMaxHp;
		if (true == IsValid(MonsterHPInstance))
		{
			MonsterHPInstance->SetHPPercent(fPercent);
		}
		if (false == mMonsterHUDWidgetComponent->IsVisible())
		{
			mMonsterHUDWidgetComponent->SetVisibility(true);
		}
		if (0 == CurHp)
		{
			HPTriggerVisibleFalse();
		}
	}
}

void AEnemyBase::Callback_OutputDamageIndicator_Implementation(float Damage)
{
	if (true == bDamageUICheckKey)
	{
		if (0.0f == Damage)
		{
			return;
		}
		/*Monster Damage Setting*/
		UWidgetComponent* NewDamageWidgetComponent = NewObject<UWidgetComponent>(this);
		if (NewDamageWidgetComponent)
		{
			const UDamageWidgetData* InputData = GetDefault<UDamageWidgetData>();
			//make Widget
			UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), InputData->mWidgetComponent);
			NewDamageWidgetComponent->SetWidget(WidgetInstance);
		}
		// actor attach
		NewDamageWidgetComponent->SetupAttachment(mCapsule);
		NewDamageWidgetComponent->SetDrawSize(FVector2D(192.0f, 108.0f));
		NewDamageWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		NewDamageWidgetComponent->SetRelativeLocation(FVector(0.0f, FMath::RandRange(50, 80), FMath::RandRange(100,110)));
		NewDamageWidgetComponent->RegisterComponent();

		UMonsterHPWidget* DamageInstance = Cast<UMonsterHPWidget>(NewDamageWidgetComponent->GetWidget());
		if (DamageInstance)
		{
			if (false == GetIsHeadShot())
			{  
				DamageInstance->SetText(FText::AsNumber(Damage));
				Animation = DamageInstance->mTextUp;
				if (Animation)
				{
					DamageInstance->PlayAnimation(Animation);
					AnimationEndEvent.BindDynamic(this, &AEnemyBase::DamageTriggerDeleteFunction);
					DamageInstance->BindToAnimationFinished(Animation, AnimationEndEvent);
					NewDamageWidgetComponent->SetVisibility(true);
					ActiveDamageWidgets.Add(NewDamageWidgetComponent);

				}
			}
			if (true == GetIsHeadShot())
			{
				DamageInstance->SetText(FText::AsNumber(Damage));
				HeadshotAnimation = DamageInstance->mTextUPRED;
				if (HeadshotAnimation)
				{
					NewDamageWidgetComponent->SetDrawSize(FVector2D(307.2f, 172.8f));
					DamageInstance->PlayAnimation(HeadshotAnimation);
					AnimationEndEvent.BindDynamic(this, &AEnemyBase::DamageTriggerDeleteFunction);
					DamageInstance->BindToAnimationFinished(HeadshotAnimation, AnimationEndEvent);
					NewDamageWidgetComponent->SetVisibility(true);
					ActiveDamageWidgets.Add(NewDamageWidgetComponent);
				}
			}
		}
	}
}


