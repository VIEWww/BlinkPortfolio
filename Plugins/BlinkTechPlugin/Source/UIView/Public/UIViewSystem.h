// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/LruCache.h"
#include "UIViewSystem.generated.h"

class UUIViewLayerPanel;
struct FStreamableHandle;

class UUIViewLayer;
class UUIViewBase;

USTRUCT(BlueprintType)
struct FUIViewLayerInfo
{
	GENERATED_BODY()

	FUIViewLayerInfo() {}
	FUIViewLayerInfo(const FName& InLayerName) : LayerName(InLayerName), LayerPath(InLayerName.ToString()) {}
	FUIViewLayerInfo(const FName& InLayerName, const FString& InLayerPath) : LayerName(InLayerName), LayerPath(InLayerPath) {}
	~FUIViewLayerInfo() {}

	UPROPERTY(EditAnywhere)
	FName LayerName = NAME_None;
	
	UPROPERTY(EditAnywhere)
	FString LayerPath = TEXT("");
};

USTRUCT()
struct FUIWidgetObjectCache
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<UUIViewBase*> ViewArray;
};

USTRUCT()
struct FUIWidgetLoadInfo
{
	GENERATED_BODY()

private:
	bool bMarkedForLoadCancel = false;

public:
	FName LayerName = NAME_None;
	TFunction<void(TWeakObjectPtr<UUIViewBase>)> OnPostShowWidget;
	TFunction<void()> OnShowCancelWidget;

	void ExcuteOnPostShowWidget(TWeakObjectPtr<UUIViewBase> InPostShowWidget)
	{
		if (OnPostShowWidget)
			OnPostShowWidget(InPostShowWidget);

		// Show완료 콜백을 호출했단 얘기는, 로드 도중 Cancel이 발생하지 않은 것. 모든 델리게이트 릴리즈.
		UnRegistAllDelegates();
	}

	void ExcuteOnShowCancelWidget()
	{
		if (OnShowCancelWidget)
			OnShowCancelWidget();

		// Show가 Cancel 되었으니, OnPostShowWidget 콜백도 필요가 없음. 모든 델리게이트 릴리즈.
		UnRegistAllDelegates();
	}
	
	void UnRegistAllDelegates()
	{
		OnPostShowWidget.Reset();
		OnShowCancelWidget.Reset();
	}

	bool IsLoadCanceled() const { return bMarkedForLoadCancel; }
	void MarkForLoadCancel() { bMarkedForLoadCancel = true; }
};


USTRUCT()
struct FUILayerViewInfo
{
	GENERATED_BODY()

	FUILayerViewInfo() {}
	FUILayerViewInfo(const FName& InViewName, const TWeakObjectPtr<UUIViewBase> InViewWidgetPtr) : ViewName(InViewName), ViewWidgetPtr(InViewWidgetPtr)
	{
	}

	bool IsValidInfo() const
	{
		return ViewName.IsNone() == false && ViewWidgetPtr.IsValid();
	}

public:
	UPROPERTY()
	FName ViewName = TEXT("");
	UPROPERTY()
	TWeakObjectPtr<UUIViewBase> ViewWidgetPtr = nullptr;
};

UCLASS()
class UIVIEW_API UUIViewSystem : public UObject
{
	GENERATED_BODY()

public:
	void Init();
	void ShutDown();

	static UUIViewSystem* Get();
	
	template <typename T, typename = typename TEnableIf<TIsDerivedFrom<T, UUIViewSystem>::Value>::Type>
	static void Install(TWeakObjectPtr<UGameInstance> InGameInstance)
	{
		UnInstall();
	
		if (InGameInstance.IsValid() == false)
			return;
	
		ThisWeak = NewObject<T>(InGameInstance.Get(), TEXT("UIViewLayerSystem"));
		if (ThisWeak.IsValid() == false)
		{
			ensure(TEXT("Error!!! : Failed Create UIViewSystem!!!"));
			return;
		}
		
		ThisWeak->OwnerInstance = InGameInstance;
		ThisWeak->InitSettings();
		ThisWeak->OnPostInstall();
	}
	static void UnInstall();
	
	virtual void InitSettings();

	virtual void OnPostInstall();
	

	// [1]. Show ======================================================================================================================================================================
	
	// Layer View Widget에 원하는 UI를 출력(Show)하고자 할 때 사용.
	// InShowFunction : UI Show가 정상 진행된 이후 호출될 콜백
	// InCancelFunction : 모종의 사유(비동기 로드중 레벨 변경 등)로 취소 되었을 때 호출될 콜백
	template <typename T, typename = typename TEnableIf<TPointerIsConvertibleFromTo<T, UUIViewBase>::Value>::Type>
	static void Show(TFunction<void(TWeakObjectPtr<UUIViewBase>)> InShowFunction = nullptr, TFunction<void()> InCancelFunction = nullptr)
	{
		UClass* InWidgetStaticClass = T::StaticClass();
		if (ThisWeak.IsValid() == false || IsValid(InWidgetStaticClass) == false)
			return;

		ThisWeak->RegistWidgetLoadInfo(InWidgetStaticClass, InShowFunction, InCancelFunction);
		ThisWeak->ShowInternal(InWidgetStaticClass);
	}
	

private:
	// Show 호출 시 내부 처리.
	void ShowInternal(UClass* InWidgetStaticClass);
	

public:
	// [2]. Hide ======================================================================================================================================================================

	// Layer View Widget에 출력중인 UI를 가리(Hide)고자 할 때 사용.

	// Hide 1. 이미 생성되어 존재하는 위젯을 파라미터로 직접 전달할 수 있는 경우에 사용.
	// ※ 오버로딩된 모든 Hide 함수는 최종적으로 이 함수를 호출하여 내부 처리가 진행됨. ※
	static void Hide(TWeakObjectPtr<UUIViewBase> InHideWidget);

	// Hide 2. 위젯 포인터를 직접 전달하기 어려운 경우에 사용. Hide 하고자 하는 Class를 template 인자로 전달하여 사용.
	// ※ 여러개의 위젯이 동시에 출력되고 있는 경우엔, 가장 앞쪽에 출력중인 위젯이 Hide 됨. ※
	template <typename T, typename = typename TEnableIf<TPointerIsConvertibleFromTo<T, UUIViewBase>::Value>::Type>
	static void Hide()
	{
		if (ThisWeak.IsValid() == false)
			return;
		
		TWeakObjectPtr<T> TopUIViewWidget = ThisWeak->GetTopUIView<T>();
		if (TopUIViewWidget.IsValid() == false)
		{
			// 현 시점에 활성화 중인 위젯이 없는 경우의 처리.
			
			UClass* InWidgetStaticClass = T::StaticClass();
			if (IsValid(InWidgetStaticClass) == false)
				return;

			FName WidgetName = InWidgetStaticClass->GetFName();
			if (ThisWeak->IsLoadingWidget(WidgetName) == false)
				return;

			// 현 시점에 활성화 중인 위젯은 없지만, 비동기 로드에 의해 로딩중인 위젯이 있는 경우.
			// 위젯 로드 취소 플래그를 ON 해주어, 비동기로드 완료 후, OnPostLoadWidget()에서 취소 루틴을 수행하도록 유도후 리턴.
			ThisWeak->MarkForWidgetLoadCancel(WidgetName);
			return;
		}
		
		ThisWeak->Hide(TopUIViewWidget);
	}

private:
	// Hide 호출 시 내부 처리.
	void HideInternal(TWeakObjectPtr<UUIViewBase> InHideWidget);


public:
	// [3]. Replace & Toggle ======================================================================================================================================================================

	// ReplaceView : Layer View Widget에 출력중인 UI를 그대로 재출력 하고자 할 때 사용. - 출력중인 UI가 없는 경우엔 Show 루틴을 타게 됨.
	template <typename T, typename = typename TEnableIf<TPointerIsConvertibleFromTo<T, UUIViewBase>::Value>::Type>
	static void ReplaceView(TFunction<void(TWeakObjectPtr<UUIViewBase>)> InShowFunction = nullptr, TFunction<void()> InCancelFunction = nullptr)
	{
		UClass* InWidgetStaticClass = T::StaticClass();
		if (ThisWeak.IsValid() == false || IsValid(InWidgetStaticClass) == false)
			return;

		TArray<TWeakObjectPtr<T>> ActivatedViews = ThisWeak->GetAllStackedViews<T>();
		if (ActivatedViews.IsValidIndex(0) && ActivatedViews[0].IsValid())
			ThisWeak->Hide(ActivatedViews[0]); // 스택 최상단에 출력중인 위젯을 찾아 Hide -> Pool로 되돌림.

		Show<T>(InShowFunction, InCancelFunction);
	}

	// ToggleView : 현재 출력중인 UI가 있다면 Hide, 없다면 Show를 호출 하여 토글링.
	template <typename T, typename = typename TEnableIf<TPointerIsConvertibleFromTo<T, UUIViewBase>::Value>::Type>
	static void ToggleView(TFunction<void(TWeakObjectPtr<UUIViewBase>)> InShowFunction = nullptr, TFunction<void()> InCancelFunction = nullptr)
	{
		UClass* InWidgetStaticClass = T::StaticClass();
		if (ThisWeak.IsValid() == false || IsValid(InWidgetStaticClass) == false)
			return;
		
		if (ThisWeak->GetTopUIView<T>() == false)
		{
			ReplaceView<T>(InShowFunction);
		}
		else
		{
			// 활성화 된 위젯은 없지만, 로딩 중인 위젯이 있는 경우라면, 아무것도 하지 않고 리턴.
			FName InWidgetName = InWidgetStaticClass->GetFName();
			if (ThisWeak->IsLoadingWidget(InWidgetName))
				return;
			
			Hide<T>();
		}
	}
	
	// [4]. CleanUp ======================================================================================================================================================================

	// 레이어 위젯을 비롯한 모든 관련 위젯 제거.
	static void CleanUp();
	void CleanUpInternal();

	// 조건에 부합하는 레이어만 CleanUp - Predicate를 전달하지 않는 경우엔 모든 레이어를 CleanUp.
	void CleanUpLayer(TFunction<bool(const FName& InLayerType)> InPredicate = nullptr);

	// 해당 위젯을 제외한 모든 위젯을 CleanUp.
	void CleanUpLayerByExcludingView(const TWeakObjectPtr<UUIViewBase> InExcludingView);

	// 해당 레이어에 속하는 비동기 로드가 진행중인 위젯들을 전부 취소처리.
	void CleanUpLoadingWidget(const FName& InLayerName);



	// Layer Stack 관련 함수 ======================================================================================================================================================================

	// 출력되어 있는 해당 위젯들 중 [가장 앞쪽에 출력중인] 위젯을 찾아 반환.
	template <typename T, typename = typename TEnableIf<TPointerIsConvertibleFromTo<T, UUIViewBase>::Value>::Type>
	TWeakObjectPtr<T> GetTopUIView()
	{
		UClass* InWidgetStaticClass = T::StaticClass();
		if (ThisWeak.IsValid() == false || IsValid(InWidgetStaticClass) == false)
			return nullptr;
		
		TArray<TWeakObjectPtr<T>> AllStackedViews = GetAllStackedViews<T>();
		TWeakObjectPtr<T> StackedView = AllStackedViews.Last();
		if (StackedView.IsValid() == false)
			return nullptr;
		
		return AllStackedViews.Last();
	}

	
	// 현재 레이어 스택에 존재하는 [모든] 위젯을 찾아 반환. - 복수 개체 체크 시 사용.
	template <typename T, typename = typename TEnableIf<TPointerIsConvertibleFromTo<T, UUIViewBase>::Value>::Type>
	TArray<TWeakObjectPtr<T>> GetAllStackedViews()
	{
		TArray<TWeakObjectPtr<T>> RetActiveViews;
		
		UClass* InWidgetStaticClass = T::StaticClass();
		if (IsValid(InWidgetStaticClass) == false)
			return RetActiveViews;

		TArray<TWeakObjectPtr<UUIViewBase>> AllActiveViews = GetAllStackedViewsInternal(InWidgetStaticClass);
		for (auto CurView : AllActiveViews)
		{
			TWeakObjectPtr<T> CastedView = Cast<T>(CurView);
			if (CastedView.IsValid())
				RetActiveViews.Add(CastedView);
		}
		
		return RetActiveViews;
	}
private:
	TArray<TWeakObjectPtr<UUIViewBase>> GetAllStackedViewsInternal(UClass* InWidgetClass) const;


public:
	// 해당 위젯이 레이어 스택에 존재하는가? - 단일 개체 체크 시 사용.
	static bool IsStackedView(TWeakObjectPtr<const UUIViewBase> InCheckWidget);
	
	// 해당 위젯이 레이어 스택에 존재하는가? - 복수 개체 체크 시 사용.
	template <typename T, typename = typename TEnableIf<TPointerIsConvertibleFromTo<T, UUIViewBase>::Value>::Type>
	bool IsStackedView()
	{
		return GetAllStackedViews<T>().Num() > 0;
	}

	// UI 레이어 관련 함수 ======================================================================================================================================================================
protected:
	void SetAllLayerInfo(const TMap<FName, FUIViewLayerInfo>& InLayerInfo) { AllLayerInfo = InLayerInfo; }
	void AddLayerInfo(const FName& InLayerName);
	void AddLayerInfo(const FUIViewLayerInfo& InLayerInfo);
	void RemoveLayerInfo(const FName& InLayerName);
	void ClearAllLayerInfo() { AllLayerInfo.Reset(); }
	
public:
	const TMap<FName, FUIViewLayerInfo>& GetAllLayerInfo() const { return AllLayerInfo; }

	bool HasLayer(const FName& InLayerName) const { return AllLayerInfo.Contains(InLayerName); }
	
	virtual bool IsValidLayer(TWeakObjectPtr<UUIViewBase> InLayerWidget) const;
	virtual bool IsValidLayer(const FName& InLayerName) const;
	
	// 뷰 레이어 위젯 삭제 발생.
	void OnDestructViewLayer();
	
	
	// 위젯 관련 편의성 함수 ======================================================================================================================================================================
	
	// 해당 위젯의 경로 알아내기.
	FSoftClassPath DetermineLayerWidgetPath(const UClass* InLayerWidgetClass) const;
	// // 해당 위젯의 레이어 타입 알아내기.
	FName DetermineLayerName(const UClass* InLayerWidgetClass) const;
	FName DetermineLayerName(TWeakObjectPtr<const UUIViewBase> InLayerWidget) const;

	// 해당 위젯이 속한 레이어의 LayerPanel Get.
	UUIViewLayerPanel* GetUIViewLayerPanel(TWeakObjectPtr<const UUIViewBase> InLayerWidget) const;

private:

	// 위젯 로드 ======================================================================================================================================================================

	// 현재 위젯 로드가 진행중인가?
	bool IsLoadingWidget() const { return WidgetLoadInfos.Num() > 0; };
	bool IsLoadingWidget(const FName& InWidgetName) const { return WidgetLoadInfos.Contains(InWidgetName); };

	// 위젯 로드 시도 - 동기 / 비동기에 따라 다른 처리를 수행. 
	void TryLoadWidget(UClass* InWidgetStaticClass);

	// 동기 로드 -----------------------------------------------------------------------------
	void LoadWidgetSyncronous(UClass* InWidgetStaticClass);
	// 비동기 로드 ---------------------------------------------------------------------------
	void LoadWidgetAsync(UClass* InWidgetStaticClass);
	// 현재 비동기 로드가 진행중인가?
	bool IsAsyncLoading() const;

	// 로드 완료 후처리 -----------------------------------------------------------------------
	void OnPostLoadWidget(UClass* InLoadedView);
	// 뷰포트에 뷰레이어 추가.
	void CreateUIViewLayer();

	// 위젯 로드 예약 ------------------------------------------------------------------------
	// 현재 로드가 예약된 위젯이 있는가?
	bool IsReservedWidgetLoad() const;
	// 위젯 로드 예약.
	void ReserveWidgetLoad(UClass* InWidgetStaticClass);
	// 예약된 위젯 로드 수행.
	void PerformReservedWidgetLoad();

	// 위젯 로드 취소 ------------------------------------------------------------------------
	// 위젯 로드 취소 플래그 ON
	void MarkForWidgetLoadCancel(const FName& InWidgetName);
	// 위젯 로드가 취소되었는가? (위젯 로드 정보가 없는 경우에도 취소로 간주.)
	bool IsWidgetLoadCanceled(const FName& InWidgetName) const;

public:
	// 캐싱 되어있고 사용 가능한 위젯을 반환하거나, 새로 생성하여 반환.
	UUIViewBase* GetOrCreateUIViewWidget(const FName InWidgetName);

	//
	UUIViewBase* GetCachedUIViewWidget(const FName InWidgetName, bool bUnUsingOnly = true) const;
	UUIViewBase* CreateUIViewWidget(const FName InWidgetName);

public:
	// 위젯 로드 정보 ======================================================================================================================================================================
	
	// 위젯 로드 과정에 사용할 정보 등록.
	void RegistWidgetLoadInfo(UClass* InWidgetStaticClass, TFunction<void(TWeakObjectPtr<UUIViewBase>)> InShowFunction = nullptr, TFunction<void()> InCancelFunction = nullptr);
	
	// 위젯 Show / Cancel시 수행할 콜백 함수 UnBind.
	void UnRegistWidgetLoadDelegate(const FName& InWidgetName);

	// 위젯 Show 후처리 콜백 실행. 
	void ExcutePostShowWidget(const FName& InWidgetName, TWeakObjectPtr<UUIViewBase> InWidgetPtr);
	// 위젯 Show Cancel 시 콜백 실행.
	void ExcuteShowCancelWidget(const FName& InWidgetName);
	


	// 위젯 캐시 관련 ======================================================================================================================================================================
	
	// 위젯 클래스 캐싱.
	void AddWidgetClassCache(UClass* InWidget);
	UClass* FindCachedWidgetClass(const FName& InWidgetName);

	// 위젯 오브젝트 캐싱.
	void AddWidgetObjectCache(const FName& InWidgetName, UUIViewBase* InLayerWidget);
	
	

	// 위젯 상태 관련 ======================================================================================================================================================================

	// 동기 로드를 해야하는 위젯 인가?
	bool IsSyncLoadWidget(UClass* InWidgetStaticClass) const;

	
private:
	static TWeakObjectPtr<UUIViewSystem> ThisWeak;

	//# UIViewSettings 관련 ==========================================================================
	UPROPERTY()
	TSoftClassPtr<UUIViewLayer> LayerClassPath = nullptr;
	
	UPROPERTY()
	FString ClassPathFormat = TEXT("/Game/UI/UIView/{0}/bp_{1}.bp_{1}_C");
	// ==============================================================================================

	UPROPERTY()
	TMap<FName, FUIViewLayerInfo> AllLayerInfo;

	// 시스템 Owner Instance
	UPROPERTY()
	TWeakObjectPtr<UGameInstance> OwnerInstance = nullptr;
	
	// UI 레이어 위젯.
	UPROPERTY()
	TWeakObjectPtr<UUIViewLayer> Widget_UIViewLayer = nullptr;
	
	// 위젯 생성에 필요한 클래스 캐시.
	UPROPERTY()
	TMap<FName, UClass*> WidgetCalssCacheMap;

	// CreateWiget으로 생성한 위젯 오브젝트 캐시.
	UPROPERTY()
	TMap<FName, FUIWidgetObjectCache> WidgetObjectCacheMap;
	
	// 위젯 생성에 사용할 정보.
	UPROPERTY()
	TMap<FName, FUIWidgetLoadInfo> WidgetLoadInfos;

	// 위젯 비동기로드용 핸들.
	TSharedPtr<FStreamableHandle> AsyncLoadHandler = nullptr;

	// 위젯 로드 예약용 큐.
	TQueue<UClass*> ReservedWidgetLoadQueue;
};