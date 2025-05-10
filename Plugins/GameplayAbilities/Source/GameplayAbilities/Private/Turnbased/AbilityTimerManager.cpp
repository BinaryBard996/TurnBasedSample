#include "Turnbased/AbilityTimerManager.h"
#include "TimerManager.h"
#include "Containers/StringConv.h"
#include "Engine/GameInstance.h"
#include "Logging/LogScopedCategoryAndVerbosityOverride.h"
#include "Misc/CoreDelegates.h"
#include "ProfilingDebugging/CsvProfiler.h"
#include "Stats/StatsTrace.h"
#include "UnrealEngine.h"
#include "Misc/TimeGuard.h"
#include "HAL/PlatformStackWalk.h"
#include "AbilitySystemComponent.h"

DEFINE_LOG_CATEGORY(LogAbilitySystemTurnbased)

void FAbilityTimerManager::TickTurn(UAbilitySystemComponent* AbilitySystemComponent, int Delta)
{
	if(!IsValid(AbilitySystemComponent))
	{
		UE_LOG(LogAbilitySystemTurnbased, Display, TEXT("TickTurn-No valid ability system component for ability timer manager!"))
		return;
	}

	FAbilityTimerContainer& AbilityTimerContainer = GetAbilityTimerContainer(AbilitySystemComponent);
	for(int32 Step = 0; Step < Delta; ++Step)
	{
		AbilityTimerContainer.Turn++;
		AbilitySystemComponent->TickTurn(1);

		TArray<FTimerHandle> RemoveTimerHandles;
		for(FTimerHandle& Handle: AbilityTimerContainer.TimerHandles)
		{
			FTimerData* TimerData = FindTimer(Handle);
			if(!TimerData)
			{
				RemoveTimerHandles.Emplace(Handle);
				continue;
			}

			// Expire Time
			if(TimerData->ExpireTime <= AbilityTimerContainer.Turn)
			{
				// Execute Timer
				// Because we only set TimeDelegate in AbilityTimerManager, so only check this type.
				if(TimerData->TimerDelegate.VariantDelegate.IsType<FTimerDelegate>())
				{
					const FTimerDelegate& FuncDelegate = TimerData->TimerDelegate.VariantDelegate.Get<FTimerDelegate>();
					if (FuncDelegate.IsBound())
					{
						FScopeCycleCounterUObject Context(FuncDelegate.GetUObject());
						FuncDelegate.Execute();
					}
				}

				if(TimerData->bLoop)
				{
					TimerData->ExpireTime = AbilityTimerContainer.Turn + TimerData->Rate;
				}
				else
				{
					RemoveTimerHandles.Emplace(Handle);
				}
			}
		}

		for(FTimerHandle& Handle: RemoveTimerHandles)
		{
			RemoveAbilityTimer(AbilitySystemComponent, Handle);
			ClearTimer(Handle);
		}
	}
}

float FAbilityTimerManager::GetAbilityTimerRemaining(UAbilitySystemComponent* AbilitySystemComponent,
	FTimerHandle& InHandle)
{
	if(!IsValid(AbilitySystemComponent))
	{
		UE_LOG(LogAbilitySystemTurnbased, Display, TEXT("GetAbilityTimerRemaining-No valid ability system component for ability timer manager!"))
		return 0.f;
	}
	
	FAbilityTimerContainer& AbilityTimerContainer = GetAbilityTimerContainer(AbilitySystemComponent);
	if(AbilityTimerContainer.TimerHandles.Find(InHandle) == INDEX_NONE)
	{
		return 0.f;
	}

	FTimerData* TimerData = FindTimer(InHandle);
	if(!TimerData)
	{
		return 0.f;
	}

	float RemainingTime = TimerData->ExpireTime - AbilityTimerContainer.Turn;
	return RemainingTime;
}

float FAbilityTimerManager::GetAbilityCurrentTurn(UAbilitySystemComponent* AbilitySystemComponent) const
{
	if(!IsValid(AbilitySystemComponent))
	{
		return 0.f;
	}

	const FAbilityTimerContainer* AbilityTimerContainer = GetAbilityTimerContainer(AbilitySystemComponent);
	if(!AbilityTimerContainer)
	{
		return 0.f;
	}

	return AbilityTimerContainer->Turn;
}

bool FAbilityTimerManager::AbilityTimerExists(UAbilitySystemComponent* AbilitySystemComponent,
                                              FTimerHandle Handle) const
{
	if(!IsValid(AbilitySystemComponent))
	{
		return false;
	}

	const FAbilityTimerContainer* AbilityTimerContainer = GetAbilityTimerContainer(AbilitySystemComponent);
	if(!AbilityTimerContainer)
	{
		return false;
	}

	return AbilityTimerContainer->TimerHandles.Contains(Handle);
}

void FAbilityTimerManager::SetAbilityTimer(UAbilitySystemComponent* AbilitySystemComponent, FTimerHandle& InOutHandle,
                                           FTimerDelegate const& InDelegate, float InRate, bool bInLoop, float InFirstDelay)
{
	if(!IsValid(AbilitySystemComponent))
	{
		UE_LOG(LogAbilitySystemTurnbased, Display, TEXT("SetAbilityTimer-No valid ability system component for ability timer manager!"))
		return;
	}

	ClearTimer(InOutHandle);
	
	if(InRate > 0.f)
	{
		// set up the new timer
		InOutHandle = SetTimerForNextTick(InDelegate);
		FTimerData* NewTimerData = FindTimer(InOutHandle);
		NewTimerData->Rate = InRate;
		NewTimerData->bLoop = bInLoop;
		FAbilityTimerContainer& AbilityTimerContainer = GetAbilityTimerContainer(AbilitySystemComponent);
		const float FirstDelay = (InFirstDelay >= 0.f) ? InFirstDelay : InRate;
		NewTimerData->ExpireTime = AbilityTimerContainer.Turn + FirstDelay;
		
		AddAbilityTimer(AbilitySystemComponent, InOutHandle);
	}
	else
	{
		RemoveAbilityTimer(AbilitySystemComponent, InOutHandle);
	}
}

void FAbilityTimerManager::ClearAbilityTimer(UAbilitySystemComponent* AbilitySystemComponent, FTimerHandle& InHandle)
{
	RemoveAbilityTimer(AbilitySystemComponent, InHandle);
	ClearTimer(InHandle);
}

void FAbilityTimerManager::SetAbilityTimerForNextTick(FTimerDelegate const& InDelegate)
{
	if(AbilityOwningGameInstance)
	{
		if(UWorld* World = AbilityOwningGameInstance->GetWorld())
		{
			World->GetTimerManager().SetTimerForNextTick(InDelegate);
		}
	}
}

void FAbilityTimerManager::ClearAbilityTimerContainer(UAbilitySystemComponent* AbilitySystemComponent)
{
	if(FAbilityTimerContainer* AbilityTimerContainer = AbilityTimerContainers.Find(AbilitySystemComponent))
	{
		for(auto& TimerHandle: AbilityTimerContainer->TimerHandles)
		{
			ClearTimer(TimerHandle);
		}
	}

	AbilityTimerContainers.Remove(AbilitySystemComponent);
}

void FAbilityTimerManager::ClearAllAbilityTimerContainers()
{
	for(auto& AbilityTimerContainer: AbilityTimerContainers)
	{
		if(AbilityTimerContainer.Key.IsValid())
		{
			AbilityTimerContainer.Key.Get()->ResetTurn();	
		}
		
		for(auto& TimerHandle: AbilityTimerContainer.Value.TimerHandles)
		{
			ClearTimer(TimerHandle);
		}
	}

	AbilityTimerContainers.Reset();
}

const FAbilityTimerContainer* FAbilityTimerManager::GetAbilityTimerContainer(
	UAbilitySystemComponent* AbilitySystemComponent) const
{
	return AbilityTimerContainers.Find(AbilitySystemComponent);
}

FAbilityTimerContainer& FAbilityTimerManager::GetAbilityTimerContainer(UAbilitySystemComponent* AbilitySystemComponent)
{
	check(IsValid(AbilitySystemComponent));
	return AbilityTimerContainers.FindOrAdd(AbilitySystemComponent);
}

void FAbilityTimerManager::AddAbilityTimer(UAbilitySystemComponent* AbilitySystemComponent,
                                           const FTimerHandle& TimerHandle)
{
	if(!IsValid(AbilitySystemComponent))
	{
		UE_LOG(LogAbilitySystemTurnbased, Display, TEXT("AddAbilityTimer-No valid ability system component for ability timer manager!"))
		return;
	}

	if(!TimerHandle.IsValid())
	{
		UE_LOG(LogAbilitySystemTurnbased, Display, TEXT("AddAbilityTimer-No valid timer handle for ability timer manager!"))
		return;
	}

	FAbilityTimerContainer& AbilityTimerContainer = GetAbilityTimerContainer(AbilitySystemComponent);
	AbilityTimerContainer.TimerHandles.Emplace(TimerHandle);
}

void FAbilityTimerManager::RemoveAbilityTimer(UAbilitySystemComponent* AbilitySystemComponent,
	const FTimerHandle& TimerHandle)
{
	if(!IsValid(AbilitySystemComponent))
	{
		UE_LOG(LogAbilitySystemTurnbased, Display, TEXT("RemoveAbilityTimer-No valid ability system component for ability timer manager!"))
		return;
	}

	FAbilityTimerContainer& AbilityTimerContainer = GetAbilityTimerContainer(AbilitySystemComponent);
	AbilityTimerContainer.TimerHandles.RemoveSwap(TimerHandle);
}
