// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

// ����� ���� ����
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	Move,
	Attack,
	Damage,
	Die,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSPROJECT_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// ���� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSM)
	EEnemyState mState = EEnemyState::Idle;
	
	// ��� ����
	void IdleState();
	// �̵� ����
	void MoveState();
	// ���� ����
	void AttackState();
	// �ǰ� ����
	void DamageState();
	// ���� ����
	void DieState();

	// ��� �ð�
	UPROPERTY(EditDefaultsOnly,Category = FSM)
	float IdleDelayTime = 2;
	// ��� �ð�
	float CurrentTime = 0;

	// Ÿ��
	UPROPERTY(VisibleAnywhere, Category = FSM)
	class ATPSPlayer* Target;
	// ���� ����
	UPROPERTY()
	class AEnemy* Me;

	//���� ����
	UPROPERTY(EditAnywhere, Category = FSM)
	float AttackRange = 250.0f;
	// ���� ��� �ð�
	UPROPERTY(EditAnywhere, Category = FSM)
	float AttackDelayTime = 2.0f;

	// �ǰ� �˸� �̺�Ʈ
	void OnDamageProcess();


	// ü��
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	int32 HP = 3;
	//�ǰ� ��� �ð�
	UPROPERTY(EditAnywhere,Category = FSM)
	float DamageDelayTime = 2.0f;
	// �Ʒ��� ������� �ӵ�
	UPROPERTY(EditAnywhere, Category = FSM)
	float DieSpeed = 50.0f;
		
	// ��� ���� �ִϸ��̼� ��������Ʈ
	UPROPERTY()
	class UEnemyAnim* Anim;
};