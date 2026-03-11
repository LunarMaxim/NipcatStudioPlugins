// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask_TargetTrace.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#if ENABLE_DRAW_DEBUG
#include "KismetTraceUtils.h"
#endif
UAbilityTask_TargetTrace::UAbilityTask_TargetTrace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DrawDebugTrace = EDrawDebugTrace::None;
	TraceColor = FLinearColor::Green;
	TraceHitColor = FLinearColor::Red;
	DrawTime = 0.f;
	bTickingTask = true;
}

UAbilityTask_TargetTrace* UAbilityTask_TargetTrace::CreateTargetBoxTraceTask(UGameplayAbility* OwningAbility, FName TaskName, int32 InTraceKey, UMeshComponent* MeshComponent, FName InSocketName, FVector InPositionOffset, FRotator InRotationOffset, FVector BoxExtent, ECollisionChannel InTraceChannel,
                                                                             TArray<AActor*> IgnoreActors, bool IgnoreOwner,
                                                                             bool IgnoreMeshOwner)
{
	UAbilityTask_TargetTrace* MyTask = NewAbilityTask<UAbilityTask_TargetTrace>(OwningAbility, TaskName);
	MyTask->TraceKey = InTraceKey;
	MyTask->MeshComponent = MeshComponent;
	MyTask->SocketName = InSocketName;
	MyTask->PositionOffset = InPositionOffset;
	MyTask->RotationOffset = InRotationOffset;
	MyTask->QueryParams.AddIgnoredActors(IgnoreActors);
	MyTask->bIgnoreOwner = IgnoreOwner;
	MyTask->bIgnoreMeshOwner = IgnoreMeshOwner;
	MyTask->Shape = FCollisionShape::MakeBox(BoxExtent);
	MyTask->TraceChannel = InTraceChannel;
	return MyTask;
}

UAbilityTask_TargetTrace* UAbilityTask_TargetTrace::CreateTargetSphereTraceTask(UGameplayAbility* OwningAbility, FName TaskName, int32 InTraceKey, UMeshComponent* InMeshComponent, FName InSocketName, FVector InPositionOffset, FRotator InRotationOffset, float InRadius, ECollisionChannel InTraceChannel,
                                                                                TArray<AActor*> InIgnoreActors, bool IgnoreOwner, bool IgnoreMeshOwner)
{
	UAbilityTask_TargetTrace* MyTask = NewAbilityTask<UAbilityTask_TargetTrace>(OwningAbility, TaskName);
	MyTask->TraceKey = InTraceKey;
	MyTask->MeshComponent = InMeshComponent;
	MyTask->SocketName = InSocketName;
	MyTask->PositionOffset = InPositionOffset;
	MyTask->RotationOffset = InRotationOffset;
	MyTask->QueryParams.AddIgnoredActors(InIgnoreActors);
	MyTask->bIgnoreOwner = IgnoreOwner;
	MyTask->bIgnoreMeshOwner = IgnoreMeshOwner;
	MyTask->Shape = FCollisionShape::MakeSphere(InRadius);
	MyTask->TraceChannel = InTraceChannel;
	return MyTask;
}

UAbilityTask_TargetTrace* UAbilityTask_TargetTrace::CreateTargetTraceTask(UGameplayAbility* OwningAbility, FName TaskName, int32 InTraceKey, UMeshComponent* InMeshComponent, FName InSocketName, FVector InPositionOffset, FRotator InRotationOffset, FCollisionShape InShape, ECollisionChannel InTraceChannel, TArray<AActor*> InIgnoreActors, bool IgnoreOwner,
                                                                          bool IgnoreMeshOwner)
{
	UAbilityTask_TargetTrace* MyTask = NewAbilityTask<UAbilityTask_TargetTrace>(OwningAbility, TaskName);
	MyTask->TraceKey = InTraceKey;
	MyTask->MeshComponent = InMeshComponent;
	MyTask->SocketName = InSocketName;
	MyTask->PositionOffset = InPositionOffset;
	MyTask->RotationOffset = InRotationOffset;
	MyTask->QueryParams.AddIgnoredActors(InIgnoreActors);
	MyTask->bIgnoreOwner = IgnoreOwner;
	MyTask->bIgnoreMeshOwner = IgnoreMeshOwner;
	MyTask->Shape = InShape;
	MyTask->TraceChannel = InTraceChannel;
	return MyTask;
}


void UAbilityTask_TargetTrace::SetDebug(EDrawDebugTrace::Type InDrawDebugTrace, FLinearColor InTraceColor, FLinearColor InTraceHitColor, float InDrawTime)
{
	DrawDebugTrace = InDrawDebugTrace;
	TraceColor = InTraceColor;
	TraceHitColor = InTraceHitColor;
	DrawTime = InDrawTime;
}

void UAbilityTask_TargetTrace::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	if (TaskState == EGameplayTaskState::Active)
	{
		TArray<FHitResult> Hits;
		FTransform SocketTransform = MeshComponent->GetSocketTransform(SocketName);
		FVector StartPosition = SocketTransform.GetLocation();
		FQuat Quat = SocketTransform.GetRotation();
		bool bHit = GetWorld()->SweepMultiByChannel(Hits, StartPosition, StartPosition, Quat, TraceChannel, Shape, QueryParams);
#if ENABLE_DRAW_DEBUG
		switch (Shape.ShapeType)
		{
		case ECollisionShape::Line:
			break;
		case ECollisionShape::Box:
			DrawDebugBoxTraceMulti(GetWorld(), StartPosition, StartPosition, FVector(Shape.Box.HalfExtentX, Shape.Box.HalfExtentY, Shape.Box.HalfExtentZ), Quat.Rotator(), DrawDebugTrace, bHit, Hits, TraceColor, TraceHitColor, DrawTime);
			break;
		case ECollisionShape::Sphere:
			DrawDebugSphereTraceMulti(GetWorld(), StartPosition, StartPosition, Shape.Sphere.Radius, DrawDebugTrace, bHit, Hits, TraceColor, TraceHitColor, DrawTime);
			break;
		case ECollisionShape::Capsule:
			break;
		}
#endif
		for (FHitResult& Hit : Hits)
		{
			AActor* CurrentHitActor = Hit.GetActor();
			bool bFirstHit = true;
			for (auto HitActor : HitActors)
			{
				if (auto SavedActor = HitActor.Get())
				{
					if (SavedActor == CurrentHitActor)
					{
						bFirstHit = false;
						break;
					}
				}
			}
			if (bFirstHit)
			{
				HitActors.Add(CurrentHitActor);
				if (OnFirstHit.IsBound())
				{
					OnFirstHit.Broadcast(Hit, TraceKey);
				}
			}
		}
	}
}

void UAbilityTask_TargetTrace::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
	bTickingTask = false;
	// UE_LOG(LogTemp, Error, TEXT("结束目标检测"));
	// GetWorld()->GetTimerManager().ClearTimer(MaxDurationTimerHandle);
}

void UAbilityTask_TargetTrace::Activate()
{
	Super::Activate();
	if (Ability)
	{
		Ability->OnGameplayAbilityEnded.AddUObject(this, &ThisClass::OnGameplayAbilityEnded);
		if (!MeshComponent)
		{
			MeshComponent = Ability->GetOwningComponentFromActorInfo();
		}
		else
		{
			if (bIgnoreMeshOwner)
			{
				QueryParams.AddIgnoredActor(MeshComponent->GetOwner());
			}
		}
		if (!MeshComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("MeshComponent 为空"));
			EndTask();
			return;
		}
		if (bIgnoreOwner)
		{
			QueryParams.AddIgnoredActor(GetAvatarActor());
		}
		QueryParams.bTraceComplex = true;
		// GetWorld()->GetTimerManager().SetTimer(MaxDurationTimerHandle, this, &ThisClass::EndTask, MaxDuration, false);
		bTickingTask = 1;
		// TaskState = EGameplayTaskState::Active;
		// UE_LOG(LogTemp, Error, TEXT("激活目标检测 %s"), *GetTaskStateName());
	}
	else
	{
		EndTask();
	}
}

void UAbilityTask_TargetTrace::OnGameplayAbilityEnded(UGameplayAbility* GameplayAbility)
{
	EndTask();
}
