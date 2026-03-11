// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "Types/SlateEnums.h"

#include "IndicatorDescriptor.generated.h"

#define UE_API NIPCATBASICGAMEPLAY_API

class SWidget;
class UIndicatorDescriptor;
class UNipcatIndicatorManagerComponent;
class UUserWidget;
struct FFrame;
struct FSceneViewProjectionData;

struct FIndicatorProjection
{
	bool Project(const UIndicatorDescriptor& IndicatorDescriptor, const FSceneViewProjectionData& InProjectionData, const FVector2f& ScreenSize, FVector& ScreenPositionWithDepth);
};

UENUM(BlueprintType)
enum class EActorCanvasProjectionMode : uint8
{
	ComponentPoint,
	ComponentBoundingBox,
	ComponentScreenBoundingBox,
	ActorBoundingBox,
	ActorScreenBoundingBox
};

USTRUCT(BlueprintType)
struct FNipcatIndicatorConfig
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftClassPtr<UUserWidget> IndicatorWidgetClass;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAutoRemoveWhenIndicatorComponentIsNull;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EActorCanvasProjectionMode ProjectionMode = EActorCanvasProjectionMode::ComponentPoint;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EHorizontalAlignment> HAlignment = HAlign_Center;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EVerticalAlignment> VAlignment = VAlign_Center;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bClampToScreen;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bClampToScreen"))
	bool bShowClampToScreenArrow;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector WorldPositionOffset;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector2D ScreenSpaceOffset;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector BoundingBoxAnchor = FVector(0.5, 0.5, 0.5);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Priority = 0;

};

/**
 * Describes and controls an active indicator.  It is highly recommended that your widget implements
 * IActorIndicatorWidget so that it can 'bind' to the associated data.
 */
UCLASS(MinimalAPI, BlueprintType)
class UIndicatorDescriptor : public UObject
{
	GENERATED_BODY()
	
public:
	UIndicatorDescriptor() { }

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBindIndicator, UIndicatorDescriptor*, Descriptor, UUserWidget*, Widget);
	UPROPERTY(BlueprintAssignable)
	FOnBindIndicator OnBindIndicator;
	
	UPROPERTY(BlueprintAssignable)
	FOnBindIndicator OnUnbindIndicator;
	
	UFUNCTION(BlueprintCallable)
	UObject* GetDataObject() const { return DataObject; }
	UFUNCTION(BlueprintCallable)
	void SetDataObject(UObject* InDataObject) { DataObject = InDataObject; }
	
	UFUNCTION(BlueprintCallable)
	USceneComponent* GetSceneComponent() const { return Component; }
	UFUNCTION(BlueprintCallable)
	void SetSceneComponent(USceneComponent* InComponent) { Component = InComponent; }

	UFUNCTION(BlueprintCallable)
	FName GetComponentSocketName() const { return ComponentSocketName; }
	UFUNCTION(BlueprintCallable)
	void SetComponentSocketName(FName SocketName) { ComponentSocketName = SocketName; }

	UFUNCTION(BlueprintCallable)
	TSoftClassPtr<UUserWidget> GetIndicatorClass() const { return IndicatorWidgetClass; }
	UFUNCTION(BlueprintCallable)
	void SetIndicatorClass(TSoftClassPtr<UUserWidget> InIndicatorWidgetClass)
	{
		IndicatorWidgetClass = InIndicatorWidgetClass;
	}

public:
	// TODO Organize this better.
	TWeakObjectPtr<UUserWidget> IndicatorWidget;

public:
	UFUNCTION(BlueprintCallable)
	void SetAutoRemoveWhenIndicatorComponentIsNull(bool CanAutomaticallyRemove)
	{
		bAutoRemoveWhenIndicatorComponentIsNull = CanAutomaticallyRemove;
	}
	UFUNCTION(BlueprintCallable)
	bool GetAutoRemoveWhenIndicatorComponentIsNull() const { return bAutoRemoveWhenIndicatorComponentIsNull; }

	bool CanAutomaticallyRemove() const
	{
		return bAutoRemoveWhenIndicatorComponentIsNull && !IsValid(GetSceneComponent());
	}

public:
	// Layout Properties
	//=======================

	UFUNCTION(BlueprintCallable)
	bool GetIsVisible() const { return IsValid(GetSceneComponent()) && bVisible; }
	
	UFUNCTION(BlueprintCallable)
	void SetDesiredVisibility(bool InVisible)
	{
		bVisible = InVisible;
	}

	UFUNCTION(BlueprintCallable)
	EActorCanvasProjectionMode GetProjectionMode() const { return ProjectionMode; }
	UFUNCTION(BlueprintCallable)
	void SetProjectionMode(EActorCanvasProjectionMode InProjectionMode)
	{
		ProjectionMode = InProjectionMode;
	}

	// Horizontal alignment to the point in space to place the indicator at.
	UFUNCTION(BlueprintCallable)
	EHorizontalAlignment GetHAlign() const { return HAlignment; }
	UFUNCTION(BlueprintCallable)
	void SetHAlign(EHorizontalAlignment InHAlignment)
	{
		HAlignment = InHAlignment;
	}

	// Vertical alignment to the point in space to place the indicator at.
	UFUNCTION(BlueprintCallable)
	EVerticalAlignment GetVAlign() const { return VAlignment; }
	UFUNCTION(BlueprintCallable)
	void SetVAlign(EVerticalAlignment InVAlignment)
	{
		VAlignment = InVAlignment;
	}

	// Clamp the indicator to the edge of the screen?
	UFUNCTION(BlueprintCallable)
	bool GetClampToScreen() const { return bClampToScreen; }
	UFUNCTION(BlueprintCallable)
	void SetClampToScreen(bool bValue)
	{
		bClampToScreen = bValue;
	}

	// Show the arrow if clamping to the edge of the screen?
	UFUNCTION(BlueprintCallable)
	bool GetShowClampToScreenArrow() const { return bShowClampToScreenArrow; }
	UFUNCTION(BlueprintCallable)
	void SetShowClampToScreenArrow(bool bValue)
	{
		bShowClampToScreenArrow = bValue;
	}

	// The position offset for the indicator in world space.
	UFUNCTION(BlueprintCallable)
	FVector GetWorldPositionOffset() const { return WorldPositionOffset; }
	UFUNCTION(BlueprintCallable)
	void SetWorldPositionOffset(FVector Offset)
	{
		WorldPositionOffset = Offset;
	}

	// The position offset for the indicator in screen space.
	UFUNCTION(BlueprintCallable)
	FVector2D GetScreenSpaceOffset() const { return ScreenSpaceOffset; }
	UFUNCTION(BlueprintCallable)
	void SetScreenSpaceOffset(FVector2D Offset)
	{
		ScreenSpaceOffset = Offset;
	}

	UFUNCTION(BlueprintCallable)
	FVector GetBoundingBoxAnchor() const { return BoundingBoxAnchor; }
	UFUNCTION(BlueprintCallable)
	void SetBoundingBoxAnchor(FVector InBoundingBoxAnchor)
	{
		BoundingBoxAnchor = InBoundingBoxAnchor;
	}

public:
	// Sorting Properties
	//=======================

	// Allows sorting the indicators (after they are sorted by depth), to allow some group of indicators
	// to always be in front of others.
	UFUNCTION(BlueprintCallable)
	int32 GetPriority() const { return Priority; }
	UFUNCTION(BlueprintCallable)
	void SetPriority(int32 InPriority)
	{
		Priority = InPriority;
	}

public:
	UNipcatIndicatorManagerComponent* GetIndicatorManagerComponent() { return ManagerPtr.Get(); }
	UE_API void SetIndicatorManagerComponent(UNipcatIndicatorManagerComponent* InManager);
	
	UFUNCTION(BlueprintCallable)
	UE_API void UnregisterIndicator();

private:
	UPROPERTY(VisibleAnywhere)
	bool bVisible = true;
	UPROPERTY(VisibleAnywhere)
	bool bClampToScreen = false;
	UPROPERTY(VisibleAnywhere)
	bool bShowClampToScreenArrow = false;
	UPROPERTY(VisibleAnywhere)
	bool bOverrideScreenPosition = false;
	UPROPERTY(VisibleAnywhere)
	bool bAutoRemoveWhenIndicatorComponentIsNull = false;

	UPROPERTY(VisibleAnywhere)
	EActorCanvasProjectionMode ProjectionMode = EActorCanvasProjectionMode::ComponentPoint;
	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<EHorizontalAlignment> HAlignment = HAlign_Center;
	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<EVerticalAlignment> VAlignment = VAlign_Center;

	UPROPERTY(VisibleAnywhere)
	int32 Priority = 0;

	UPROPERTY(VisibleAnywhere)
	FVector BoundingBoxAnchor = FVector(0.5, 0.5, 0.5);
	UPROPERTY(VisibleAnywhere)
	FVector2D ScreenSpaceOffset = FVector2D(0, 0);
	UPROPERTY(VisibleAnywhere)
	FVector WorldPositionOffset = FVector(0, 0, 0);

private:
	friend class SActorCanvas;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UObject> DataObject;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Component;

	UPROPERTY(VisibleAnywhere)
	FName ComponentSocketName = NAME_None;

	UPROPERTY(VisibleAnywhere)
	TSoftClassPtr<UUserWidget> IndicatorWidgetClass;

	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<UNipcatIndicatorManagerComponent> ManagerPtr;

	TWeakPtr<SWidget> Content;
	TWeakPtr<SWidget> CanvasHost;
};

#undef UE_API
