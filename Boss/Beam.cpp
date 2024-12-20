// Fill out your copyright notice in the Description page of Project Settings.


#include "Beam.h"
#include "BossZero.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Components/CapsuleComponent.h"
#include "Crazy6/Global/ProfileInfo.h"
#include "Engine/DamageEvents.h"

UBeam::UBeam()
{
	PrimaryComponentTick.bCanEverTick = true;

	// <======= HitBox Setting =======>
	mBeamHitBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BeamHitBox"));
	mBeamHitBox->SetCapsuleSize(2.3f, 15.f);
	mBeamHitBox->SetRelativeLocation(FVector(12.3, 0.0, -8.27));
	mBeamHitBox->SetRelativeRotation(FRotator(55.0, 0.0, 0.0));
	mBeamHitBox->SetupAttachment(this);
	mBeamHitBox->SetCollisionProfileName(UE_COLLISION_PROFILE_ENEMYATTACK);
	mBeamHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* <=========== Attack OverLap Setting ===========> */
	mBeamHitBox->OnComponentBeginOverlap.AddDynamic(this,
		&UBeam::OnBegineOverlap_Beam);
	mBeamHitBox->OnComponentEndOverlap.AddDynamic(this,
		&UBeam::OnEndOverlap_Beam);

	// <======= Visual Setting =======>
	mBeamHead = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BeamHead"));
	mBeamHead->SetRelativeScale3D(FVector(0.012, 0.012, 0.012));
	mBeamHead->SetRelativeLocation(FVector(0.624, 0.0, -0.15));
	mBeamHead->SetRelativeRotation(FRotator(-9.0, 3.8, -26.9));
	mBeamHead->SetupAttachment(this);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
			BeamHeadNiagara(TEXT("/Script/Niagara.NiagaraSystem'/Game/Dev/AI/Enemy/Boss/RayFX/Fx/NS_Energy_1.NS_Energy_1'"));
	if (true == BeamHeadNiagara.Succeeded())
	{
		mBeamHead->SetAsset(BeamHeadNiagara.Object);
	}
	mBeamHead->SetAutoActivate(false);

	mBeamBody = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BeamBody"));
	mBeamBody->SetRelativeScale3D(FVector(0.015, 0.02, 0.02));
	mBeamBody->SetRelativeLocation(FVector(1.1, 0.0, -0.2));
	mBeamBody->SetRelativeRotation(FRotator(-35.0, 0.0, 0.0));
	mBeamBody->SetupAttachment(this);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
			BeamBodyNiagara(TEXT("/Script/Niagara.NiagaraSystem'/Game/Dev/AI/Enemy/Boss/RayFX/Fx/NS_Ray_1.NS_Ray_1'"));
	if (true == BeamBodyNiagara.Succeeded())
	{
		mBeamBody->SetAsset(BeamBodyNiagara.Object);
	}
	mBeamBody->SetAutoActivate(false);

	mBeamTail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BeamTail"));
	mBeamTail->SetRelativeScale3D(FVector(0.014, 0.014, 0.014));
	mBeamTail->SetRelativeLocation(FVector(22.4, 0.0, -15.0));
	mBeamTail->SetRelativeRotation(FRotator(-30.0, 0.0, 0.0));
	mBeamTail->SetupAttachment(this);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
			BeamTailNiagara(TEXT("/Script/Niagara.NiagaraSystem'/Game/Dev/AI/Enemy/Boss/RayFX/Fx/NS_Hit_1.NS_Hit_1'"));
	if (true == BeamTailNiagara.Succeeded())
	{
		mBeamTail->SetAsset(BeamTailNiagara.Object);
	}
	mBeamTail->SetAutoActivate(false);

	/* <=========== Indicator Setting ===========> */
	mBeamIndicator = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BeamIndicator"));
	mBeamIndicator->SetupAttachment(this);
	mBeamIndicator->SetRelativeScale3D(FVector(0.01, 0.01, 0.01));
	mBeamIndicator->SetRelativeLocation(FVector(-22.85, 0.0, 13.5));
	mBeamIndicator->SetRelativeRotation(FRotator(0.0, -90.0, 35.1));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
			NiagaraParticle(TEXT("/Script/Niagara.NiagaraSystem'/Game/Dev/UI/AttackIndicator/NS_SquareSideIndicator.NS_SquareSideIndicator'"));
	if (true == NiagaraParticle.Succeeded())
	{
		mBeamIndicator->SetAsset(NiagaraParticle.Object);
	}
	mBeamIndicator->SetAutoActivate(false);

	/* <=========== Delegate Setting ===========> */
	mBeamIndicator->OnSystemFinished.AddDynamic(this, &UBeam::OnLaserChargeEnd);
	mBeamBody->OnSystemFinished.AddDynamic(this, &UBeam::OnLaserEnd);

	// <======= FX Setting =======>
	static ConstructorHelpers::FObjectFinder<UParticleSystem>
		HitParticle(TEXT("/Script/Engine.ParticleSystem'/Game/Dev/AI/Enemy/Humanoid/FX/P_Minion_Impact_Default.P_Minion_Impact_Default'"));
	if (HitParticle.Succeeded())
	{
		mDotHitParticle = HitParticle.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<USoundBase>
		BeamSoundBase(TEXT("/Script/Engine.SoundWave'/Game/Dev/AI/Enemy/Boss/Sound/Laser_Beam.Laser_Beam'"));
	if (BeamSoundBase.Succeeded())
	{
		mBeamSound = BeamSoundBase.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase>
		HitSoundBase(TEXT("/Script/Engine.SoundWave'/Game/Dev/AI/Enemy/Boss/Sound/Laser_DotHit.Laser_DotHit'"));
	if (HitSoundBase.Succeeded())
	{
		mDotHitSound = HitSoundBase.Object;
	}
}

void UBeam::BeginPlay()
{
	Super::BeginPlay();
}

void UBeam::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UBeam::OnLaserChargeEnd(UNiagaraComponent* FinishedComponent)
{
	mBeamBody->Activate();
	mBeamTail->Activate();
	mBeamHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	UGameplayStatics::PlaySound2D(GetWorld(), mBeamSound, 1, 1, 0,
			nullptr, GetOwner(), false);
}

void UBeam::OnLaserEnd(UNiagaraComponent* FinishedComponent)
{
	mBeamHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	ABossZero* BossPawn = Cast<ABossZero>(GetOwner());
	if (true == IsValid(BossPawn))
	{
		BossPawn->ResumeMontage();
	}
}

void UBeam::ApplyBeamDamage(AActor* OtherActor)
{
	FDamageEvent	DmgEvent;
	ABossZero* BossPawn = Cast<ABossZero>(GetOwner());
	if (true == IsValid(BossPawn))
	{
		OtherActor->TakeDamage(BossPawn->GetAttack() / 10.f, DmgEvent, BossPawn->GetController(), BossPawn);
	}

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mDotHitParticle,
			OtherActor->GetActorLocation(), OtherActor->GetActorRotation(), true);
	UGameplayStatics::PlaySound2D(GetWorld(), mDotHitSound, 1, 1, 0,
			nullptr, OtherActor, false);
}

void UBeam::OnBegineOverlap_Beam(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	GetWorld()->GetTimerManager().SetTimer(mContinuousDamageTimer, [this, OtherActor]()
	{
		ApplyBeamDamage(OtherActor);
	}, mDamageInterval, true);
}

void UBeam::OnEndOverlap_Beam(
	UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GetWorld()->GetTimerManager().ClearTimer(mContinuousDamageTimer);
}

