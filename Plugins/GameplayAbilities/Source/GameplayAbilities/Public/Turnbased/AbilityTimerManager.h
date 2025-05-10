#pragma once
#include "TimerManager.h"

class UAbilitySystemComponent;

GAMEPLAYABILITIES_API DECLARE_LOG_CATEGORY_EXTERN(LogAbilitySystemTurnbased, Display, All)

struct FAbilityTimerContainer
{
public:
	int32 Turn = 0;
	TArray<FTimerHandle> TimerHandles;
};

class GAMEPLAYABILITIES_API FAbilityTimerManager: public FTimerManager
{
public:
	FAbilityTimerManager(UGameInstance* InGameInstance): FTimerManager(InGameInstance){ AbilityOwningGameInstance = InGameInstance; }
	
	void TickTurn(UAbilitySystemComponent* AbilitySystemComponent, int Delta = 1);
	float GetAbilityTimerRemaining(UAbilitySystemComponent* AbilitySystemComponent, FTimerHandle& InHandle);
	float GetAbilityCurrentTurn(UAbilitySystemComponent* AbilitySystemComponent) const;

	bool AbilityTimerExists(UAbilitySystemComponent* AbilitySystemComponent, FTimerHandle Handle) const;
	void SetAbilityTimer(UAbilitySystemComponent* AbilitySystemComponent, FTimerHandle& InOutHandle, FTimerDelegate const& InDelegate, float InRate, bool bInLoop, float InFirstDelay = -1.f);
	void ClearAbilityTimer(UAbilitySystemComponent* AbilitySystemComponent, FTimerHandle& InHandle);
	void SetAbilityTimerForNextTick(FTimerDelegate const& InDelegate);
	void ClearAbilityTimerContainer(UAbilitySystemComponent* AbilitySystemComponent);
	void ClearAllAbilityTimerContainers();
	
private:
	const FAbilityTimerContainer* GetAbilityTimerContainer(UAbilitySystemComponent* AbilitySystemComponent) const;
	FAbilityTimerContainer& GetAbilityTimerContainer(UAbilitySystemComponent* AbilitySystemComponent);
	void AddAbilityTimer(UAbilitySystemComponent* AbilitySystemComponent, const FTimerHandle& TimerHandle);
	void RemoveAbilityTimer(UAbilitySystemComponent* AbilitySystemComponent, const FTimerHandle& TimerHandle);

private:
	TMap<TWeakObjectPtr<UAbilitySystemComponent>, FAbilityTimerContainer> AbilityTimerContainers;
	UGameInstance* AbilityOwningGameInstance;
};
