// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"
#include "Enemy.h"

void UEnemyAnim::OnEndAttackAnimation()
{
	bAttackPlay = false;
}

void UEnemyAnim::OnDamageEnd()
{
	// Enemy�� �����ͼ� EnemyFSM �� �Լ��� ȣ���ϰ� �ʹ�.
	AEnemy* Enemy = Cast<AEnemy>(TryGetPawnOwner());
	if (Enemy)
	{
		Enemy->EnemyFSM->OnChangeMoveState();
	}
}
