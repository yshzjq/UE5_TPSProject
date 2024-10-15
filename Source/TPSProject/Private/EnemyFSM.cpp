// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "TPSPlayer.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "EnemyAnim.h"

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// ���忡�� ATPSPlayer Ÿ�� ã�ƿ���
	auto Actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());
	//ATPSPlayer Ÿ������ ĳ����
	Target = Cast<ATPSPlayer>(Actor);
	// �ҿ� ��ü ��������
	Me = Cast<AEnemy>(GetOwner());

	// ��� ���� UEnemyAnim ��������
	Anim = Cast<UEnemyAnim>(Me->GetMesh()->GetAnimInstance());
	
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ���� â�� ���� �޼��� ���
	FString LogMsg = UEnum::GetValueAsString(mState);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, LogMsg);

	switch (mState)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	}
	
}

void UEnemyFSM::IdleState()
{
	// ���� �ð� ��ٷȴٰ� �̵� ���·� ��ȯ�ϰ� �ʹ�.
	//1. �ð��� ������ϱ�
	CurrentTime += GetWorld()->DeltaTimeSeconds;
	// 2. ���� ��� �ð��� ��� �ð��� �ʰ��ߴٸ�
	if (CurrentTime > IdleDelayTime)
	{
		//3. �̵� ���·� ��ȯ�ϰ� �ʹ�.
		mState = EEnemyState::Move;
		// ��� �ð� �ʱ�ȭ
		CurrentTime = 0;

		// �ִϸ��̼� ���� ����ȭ
		Anim->AnimState = mState;
	}
}

void UEnemyFSM::MoveState()
{
	// Ÿ���� �������� �̵��ϰ� �ʹ�.
	// 1. Ÿ���� �������� �ʿ�
	FVector Destination = Target->GetActorLocation();
	// 2. ������ �ʿ�
	FVector Dir = Destination - Me->GetActorLocation();
	// 3. �������� �̵��ϰ� �ʹ�.
	Me->AddMovementInput(Dir.GetSafeNormal());

	// Ÿ��� ��������� ���� ���·� ��ȯ�ϰ� �ʹ�.
	// 1. ���� �Ÿ��� ���� ���� �ȿ� ������
	if (Dir.Size() < AttackRange)
	{
		// 2. ���� ���·� ��ȯ�ϰ� �ʹ�.
		mState = EEnemyState::Attack;
		// �ִϸ��̼� ���� ����ȭ
		Anim->AnimState = mState;
		// ���� �ִϸ��̼� ��� Ȱ��ȭ
		Anim->bAttackPlay = true;
		// ���� ���� ��ȯ �� ��� �ð��� �ٷ� ����
		CurrentTime = AttackDelayTime;
	}
}

void UEnemyFSM::AttackState()
{
	// ���� �ð��� �� ���� �����ϰ� �ʹ�.
	// 1. �ð��� �귯�� �Ѵ�.
	CurrentTime += GetWorld()->DeltaTimeSeconds;
	// 2. ���� �ð��� �����ϱ�
	if (CurrentTime > AttackDelayTime)
	{
		// 3. �����ϰ� �ʹ�.
		UE_LOG(LogTemp, Warning, TEXT("Attack"));
	}
	// Ÿ���� ���� ������ ����� ���¸� �̵����� ��ȯ�ϰ� �ʹ�.
	// 1. Ÿ����� �Ÿ��� �ʿ��ϴ�
	float Distance = FVector::Distance(Target->GetActorLocation(), Me->GetActorLocation());
	// 2. Ÿ����� �Ÿ��� ���� ������ �������
	if (Distance > AttackRange)
	{
		// 3. ���¸� �̵����� ��ȯ�ϰ� �ʹ�.
		mState = EEnemyState::Move;
	}
}

void UEnemyFSM::DamageState()
{
	// ���� �ð� ��ٷȴٰ� ��븦 ���� �����ϰ� �ʹ�.
	// 1. �ð��� ������ϱ�
	CurrentTime += GetWorld()->DeltaTimeSeconds;
	// 2. ���� ��� �ð��� ��� �ð��� �ʰ��ߴٸ�
	if (CurrentTime > DamageDelayTime)
	{
		// 3. ��� ���·� ��ȯ�ϰ� �ʹ�.
		mState = EEnemyState::Idle;
		// ��� �ð� �ʱ�ȭ
		CurrentTime = 0;

		//Anim->AnimState = mState;
	}
}

void UEnemyFSM::DieState()
{
	// ���� ���� �ִϸ��̼��� ������ �ʾҴٸ�
	// �ٴ����� �������� �ʵ��� ó��
	if (Anim->bDieDone == false)
	{
		return;
	}

	// ��� �Ʒ��� ��������.
	// ��� � ���� P = P0 + VT
	FVector P0 = Me->GetActorLocation();
	FVector VT = FVector::DownVector * DieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector P = P0 + VT;
	Me->SetActorLocation(P);

	// ���� 2���� �̻� �����Դٸ�
	if (P.Z < -200.0f)
	{
		// ���Ž�Ų��.
		Me->Destroy();
	}
}

void UEnemyFSM::OnDamageProcess()
{
	
	// 
	//ü�� ����
	HP--;
	// ���� ü���� �����ִٸ�
	if (HP > 0)
	{
		// ��븦 �ǰ����� ��ȯ
		mState = EEnemyState::Damage;
		CurrentTime = 0;
		// �ǰ� �ִϸ��̼� ���
		int32 index = FMath::RandRange(0, 1);
		FString SectionName = FString::Printf(TEXT("Damage & d"), index);
		Anim->PlayDamageAnim(*SectionName);
	}
	// �׷��� �ʴٸ�
	else
	{
		//���¸� �������� ��ȯ
		mState = EEnemyState::Die;

		// ĸ�� ������Ʈ �浹ü ��Ȱ��ȭ
		Me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// ���� �ִϸ��̼� ���
		Anim->PlayDamageAnim(TEXT("Die"));
	}
	// �ִϸ��̼� ���� ����ȭ
	Anim->AnimState = mState;
}



