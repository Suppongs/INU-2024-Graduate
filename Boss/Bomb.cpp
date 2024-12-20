// Fill out your copyright notice in the Description page of Project Settings.


#include "Bomb.h"
#include "BossZero.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Components/SphereComponent.h"
#include "Crazy6/Global/ProfileInfo.h"
#include "Engine/DamageEvents.h"

ABomb::ABomb()
{
	PrimaryActorTick.bCanEverTick = true;

	// <======= HitBox Setting =======>
	mBombHitBox = CreateDefaultSubobject<USphereComponent>(TEXT("BombHitBox"));
	SetRootComponent(mBombHitBox);
	mBombHitBox->SetSphereRadius(240.f);
	mBombHitBox->SetCollisionProfileName(UE_COLLISION_PROFILE_ENEMYATTACK);
	mBombHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	mBombHitBox->SetCanEverAffectNavigation(false);

	// <======= Visual Setting =======>
	mBombVisual = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BombVisual"));
	mBombVisual->SetupAttachment(mBombHitBox);
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem>
			VisualParticleSystem(TEXT("/Script/Engine.ParticleSystem'/Game/Dev/AI/Enemy/Boss/BombFX/P_ConsGround_Bubble_Pop.P_ConsGround_Bubble_Pop'"));
	if (true == VisualParticleSystem.Succeeded())
	{
		mBombVisual->SetTemplate(VisualParticleSystem.Object);
	}
	mBombVisual->SetAutoActivate(false);

	// <======= Indicator Setting =======>
	mBombIndicator = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BombIndicator"));
	mBombIndicator->SetupAttachment(mBombHitBox);
	mBombIndicator->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
			IndicatorNiagaraSystem(TEXT("/Script/Niagara.NiagaraSystem'/Game/Dev/UI/AttackIndicator/NS_CircleIndicator.NS_CircleIndicator'"));
	if (true == IndicatorNiagaraSystem.Succeeded())
	{
		mBombIndicator->SetAsset(IndicatorNiagaraSystem.Object);
	}

	/* <=========== Delegate Setting ===========> */
	mBombIndicator->OnSystemFinished.AddDynamic(this, &ABomb::OnIndicatorEnd);
	mBombVisual->OnSystemFinished.AddDynamic(this, &ABomb::OnExplosionEnd);

	// <======= Bomb Overlap Setting =======>
	mBombHitBox->OnComponentBeginOverlap.AddDynamic(this, &ABomb::OnBegineOverlap_Bomb);

	// <======= FX Setting =======>
	static ConstructorHelpers::FObjectFinder<USoundBase>
		LaunchSoundBase(TEXT("/Script/Engine.SoundWave'/Game/Dev/AI/Enemy/Boss/Sound/Bomb_Launch.Bomb_Launch'"));
	if (LaunchSoundBase.Succeeded())
	{
		mLaunchSound = LaunchSoundBase.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase>
		ExplosionSoundBase(TEXT("/Script/Engine.SoundWave'/Game/Dev/AI/Enemy/Boss/Sound/Bomb_Explosion.Bomb_Explosion'"));
	if (ExplosionSoundBase.Succeeded())
	{
		mExplosionSound = ExplosionSoundBase.Object;
	}
}


void ABomb::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::PlaySound2D(GetWorld(), mLaunchSound, 1, 1, 0,
			nullptr, this, false);
}

void ABomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABomb::OnIndicatorEnd(UNiagaraComponent* FinishedComponent)
{
	mBombVisual->Activate();
	
	mBombHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	mBombHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UGameplayStatics::PlaySound2D(GetWorld(), mExplosionSound, 1, 1, 0,
			nullptr, this, false);
}

void ABomb::OnExplosionEnd(UParticleSystemComponent* FinishedComponent)
{
	this->Destroy();
}

void ABomb::OnBegineOverlap_Bomb(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FDamageEvent DmgEvent;
	OtherActor->TakeDamage(mOwner->GetAttack(), DmgEvent, mOwner->GetController(), this);

	mOwner->BombSuccessCount++;
}


