# TurnBasedSample Document
In this project, I Modify Unreal's GAS plugin to support turn-based games.

## How to use

### Enable turn based effect

In *AbilitySystemComponent* default settings, set property *bTurnBased* to true, and the GEs applyed by ASC will will effect based on the number of turns instead of the world time.
```
	UPROPERTY(EditAnywhere, Category=TurnBased)
	bool bTurnBased = false;
```

If you want to switch between turn-based and real-time modes during runtime, call the function *AbilitySystemComponent::SetTurnBasedEnabled*.
```
	UFUNCTION(BlueprintCallable)
	void SetTurnBasedEnabled(bool bEnabled = false) { bTurnBased = bEnabled; }
```

### Tick Turn

If turn-based mode is enabled, the duration and other effects of gameplay effects (GEs) will not progress with the world time's tick. To make them take effect, you must manually call the function *UAbilitySystemBlueprintLibrary::TickTurn*.
```
	/* Tick given AbilitySystemComponent's turn */
	UFUNCTION(BlueprintCallable, Category = "Ability|TurnBased")
	static void TickTurn(UAbilitySystemComponent* AbilitySystemComponent, int32 Delta = 1);
```

## Advance

If you want to review the modified code, you can search comment **TurnBased Support**—all changes made to the GAS plugin related to this feature are marked with corresponding comments.