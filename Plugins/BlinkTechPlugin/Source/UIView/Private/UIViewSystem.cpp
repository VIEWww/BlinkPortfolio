// Fill out your copyright notice in the Description page of Project Settings.
#include "UIViewSystem.h"

#include "UIView.h"
#include "UIViewBase.h"
#include "Blueprint/UserWidget.h"
#include "Engine/AssetManager.h"
#include "UIViewLayer.h"
#include "UIViewLayerPanel.h"
#include "UIViewSettings.h"


TWeakObjectPtr<UUIViewSystem> UUIViewSystem::ThisWeak = nullptr;

void UUIViewSystem::Init()
{
	Widget_UIViewLayer.Reset();

	WidgetCalssCacheMap.Reset();
	WidgetObjectCacheMap.Reset();
	
	WidgetLoadInfos.Reset();
	AsyncLoadHandler.Reset();

	ReservedWidgetLoadQueue.Empty();
}

void UUIViewSystem::ShutDown()
{
	Init();
}

void UUIViewSystem::UnInstall()
{
	if (ThisWeak.IsValid())
	{
		ThisWeak->RemoveFromRoot();
		ThisWeak->MarkAsGarbage();
		ThisWeak = nullptr;
	}
}

void UUIViewSystem::InitSettings()
{
	if (ThisWeak.IsValid() == false)
		return;
	
	const UUIViewSettings* UIViewSettings = GetDefault<UUIViewSettings>();
	if (IsValid(UIViewSettings) == false)
		return;

	ThisWeak->LayerClassPath = UIViewSettings->GetLayerWidgetClass();
	ThisWeak->ClassPathFormat = UIViewSettings->GetClassPathFormat();
}

void UUIViewSystem::OnPostInstall()
{
}

UUIViewSystem* UUIViewSystem::Get()
{
	if (ThisWeak.IsValid() == false)
		return nullptr;
	return ThisWeak.Get();
}

void UUIViewSystem::ShowInternal(UClass* InWidgetStaticClass)
{
	// ※ InWidgetStaticClass의 유효성은 ShowInternal을 호출하기 전에 검증하고 넘길 것이므로 IsValid 체크를 생략함. 
	FName InLayerWidgetName = InWidgetStaticClass->GetFName();
	if (WidgetCalssCacheMap.Contains(InLayerWidgetName))
	{
		// 이미 생성되어 캐싱된 UI인 경우.
		OnPostLoadWidget(WidgetCalssCacheMap.FindRef(InLayerWidgetName));
	}
	else
	{
		// 캐싱되어 있지 않아 새로 만들어야 하는 UI인 경우.
		TryLoadWidget(InWidgetStaticClass);
	}
}

void UUIViewSystem::Hide(TWeakObjectPtr<UUIViewBase> InHideWidget)
{
	if (ThisWeak.IsValid() == false || InHideWidget.IsValid() == false)
		return;
	
	ThisWeak->HideInternal(InHideWidget);
}

void UUIViewSystem::HideInternal(TWeakObjectPtr<UUIViewBase> InHideWidget)
{
	if (Widget_UIViewLayer.IsValid() == false)
		return;

	// Hide 처리가 불가능한 상태인 경우엔, 그대로 리턴.
	if (InHideWidget.IsValid() == false || InHideWidget->IsClosable() == false)
		return;

	Widget_UIViewLayer->HideWidgetInLayer(InHideWidget, [this, InHideWidget]()
	{
		TWeakObjectPtr<UUIViewSystem> ThisWeak = UUIViewSystem::Get();
		if (ThisWeak.IsValid())
		{
			Widget_UIViewLayer->RemoveWidgetInLayer(InHideWidget.Get());
		}
	});
}

void UUIViewSystem::CleanUp()
{
	TWeakObjectPtr<UUIViewSystem> ManagerPtr = UUIViewSystem::Get();
	if (ManagerPtr.IsValid() == false)
		return;

	ManagerPtr->CleanUpInternal();
}

void UUIViewSystem::CleanUpInternal()
{
	// 모든 레이어 CleanUp
	CleanUpLayer();

	// 모든 캐시 릴리즈.
	WidgetCalssCacheMap.Reset();
	WidgetObjectCacheMap.Reset();
	WidgetLoadInfos.Reset();

	// 비동기 로드 컨테이너 초기화.
	AsyncLoadHandler.Reset();
	ReservedWidgetLoadQueue.Empty();
}

void UUIViewSystem::CleanUpLayer(TFunction<bool(const FName& InLayerType)> InPredicate)
{
	if (Widget_UIViewLayer.IsValid() == false)
		return;
	
	for (auto &CurLayerInfo : AllLayerInfo)
	{
		FName CurLayerName = CurLayerInfo.Key;
		if (InPredicate != nullptr && InPredicate(CurLayerName) == false)
			continue;
		// 1. 해당 레이어에 속하는 비동기 로드가 진행중인 위젯들을 전부 취소처리.
		CleanUpLoadingWidget(CurLayerName);
		// 2. 해당 레이어에 속하는 위젯들을 전부 비활성 처리.
		Widget_UIViewLayer->CleanUpLayer(CurLayerName);
	}
}

void UUIViewSystem::CleanUpLayerByExcludingView(const TWeakObjectPtr<UUIViewBase> InExcludingView)
{
	if (InExcludingView.IsValid() == false || Widget_UIViewLayer.IsValid() == false)
		return;

	Widget_UIViewLayer->CleanUpLayerByExcludingView(InExcludingView);
}

void UUIViewSystem::TryLoadWidget(UClass* InWidgetStaticClass)
{
	if (IsAsyncLoading())
	{
		// 비동기 로드가 진행중인 경우. 예약하고 리턴.
		ReserveWidgetLoad(InWidgetStaticClass);
		return;
	}
	
	if (IsSyncLoadWidget(InWidgetStaticClass))
	{
		// 동기 로드
		LoadWidgetSyncronous(InWidgetStaticClass);
	}
	else
	{
		// 비동기 로드
		LoadWidgetAsync(InWidgetStaticClass);
	}
}

void UUIViewSystem::LoadWidgetAsync(UClass* InWidgetStaticClass)
{
	const FSoftClassPath InBPWidgetPath = DetermineLayerWidgetPath(InWidgetStaticClass);
	AsyncLoadHandler = UAssetManager::GetStreamableManager().RequestAsyncLoad(InBPWidgetPath, [=]()
	{
		TWeakObjectPtr<UClass> LoadedWidget = TSoftClassPtr<UUIViewBase>(InBPWidgetPath).Get();
		if (LoadedWidget.IsValid() == false)
		{
			FName InLayerWidgetName = InWidgetStaticClass->GetFName();
			ExcuteShowCancelWidget(InLayerWidgetName);
			ensure(TEXT("Error : Failed Async Laod Widget :: Invalid Widget Or Path !!!"));
			return;
		}

		// 로드 완료 후 처리 수행.
		OnPostLoadWidget(LoadedWidget.Get());

		// 비동기 로드 핸들 초기화.
		AsyncLoadHandler.Reset();

		if (IsReservedWidgetLoad())
		{
			// 예약 되어있는 위젯 로드가 있는 경우, 이어서 수행.
			PerformReservedWidgetLoad();
		}
	});
}

bool UUIViewSystem::IsAsyncLoading() const
{
	return AsyncLoadHandler.IsValid();
}

bool UUIViewSystem::IsReservedWidgetLoad() const
{
	return ReservedWidgetLoadQueue.IsEmpty() == false;
}

void UUIViewSystem::ReserveWidgetLoad(UClass* InWidgetStaticClass)
{
	ReservedWidgetLoadQueue.Enqueue(InWidgetStaticClass);
}

void UUIViewSystem::PerformReservedWidgetLoad()
{
	UClass* NextWidgetLoadClass = nullptr;
	ReservedWidgetLoadQueue.Dequeue(NextWidgetLoadClass);
	TryLoadWidget(NextWidgetLoadClass);
}

void UUIViewSystem::LoadWidgetSyncronous(UClass* InWidgetStaticClass)
{
	const FSoftClassPath InBPWidgetPath = DetermineLayerWidgetPath(InWidgetStaticClass);
	TWeakObjectPtr<UClass> LoadedWidget = UAssetManager::GetStreamableManager().LoadSynchronous<UClass>(InBPWidgetPath);
	if (LoadedWidget.IsValid() == false)
	{
		FName InLayerWidgetName = InWidgetStaticClass->GetFName();
		ExcuteShowCancelWidget(InLayerWidgetName);
		ensure(TEXT("Error : Failed Async Laod Widget :: Invalid Widget Or Path !!!"));
		return;
	}
	OnPostLoadWidget(LoadedWidget.Get());
}

void UUIViewSystem::OnPostLoadWidget(UClass* InLoadedView)
{
	UClass* InParentClass = GetParentNativeClass(InLoadedView);
	if (IsValid(InLoadedView) == false || IsValid(InParentClass) == false || IsValid(InLoadedView->StaticClass()) == false)
		return;

	// 1. 클래스 캐시 추가. (이미 캐싱되어 있는 경우엔 그대로 리턴됨)
	AddWidgetClassCache(InLoadedView);

	// 2. UI 레이어 위젯 생성. (이미 생성된 경우엔 그대로 리턴됨)
	CreateUIViewLayer();

	// 3. 위젯 비동기 로드 취소 여부에 따라 분기.
	FName WidgetName = InParentClass->GetFName();
	if (IsWidgetLoadCanceled(WidgetName))
	{
		// 비동기 로드 취소시 콜백 실행.
		ExcuteShowCancelWidget(WidgetName);
	}
	else if (Widget_UIViewLayer.IsValid())
	{
		// 레이어 위젯에 생성된 위젯 부착.
		
		Widget_UIViewLayer->ShowWidgetInLayer(InLoadedView, [WidgetName](TWeakObjectPtr<UUIViewBase> InCreatedLayerWidget)
		{
			// 위젯 Show 완료 시 콜백 실행.
			if (ThisWeak.IsValid() == false || InCreatedLayerWidget.IsValid() == false)
				return;
			
			InCreatedLayerWidget->OnShowFromViewLayer([]()
			{
				// Show 완료 시 콜백. 필요한 경우 처리 추가.
			});
			
			ThisWeak->ExcutePostShowWidget(WidgetName, InCreatedLayerWidget);
		});
	}
}

void UUIViewSystem::CleanUpLoadingWidget(const FName& InLayerName)
{
	TArray<FName> CleanUpWidgetNames;

	for (auto &CurLoadInfo : WidgetLoadInfos)
	{
		FName CurrentWidgetLayerName = CurLoadInfo.Value.LayerName;
		if (InLayerName == CurrentWidgetLayerName)
		{
			CleanUpWidgetNames.Add(CurLoadInfo.Key);
		}
	}
	
	for (auto &CurName : CleanUpWidgetNames)
	{
		UnRegistWidgetLoadDelegate(CurName);
	}
}

bool UUIViewSystem::IsStackedView(TWeakObjectPtr<const UUIViewBase> InCheckWidget)
{
	if (ThisWeak.IsValid() == false || InCheckWidget.IsValid() == false)
		return false;
	
	TWeakObjectPtr<UUIViewLayerPanel> LayerPanel = ThisWeak->GetUIViewLayerPanel(InCheckWidget);
	if (LayerPanel.IsValid() == false)
		return false;

	TArray<UUIViewBase*> StatckedWidgetList = LayerPanel->GetWidgetList();
	return StatckedWidgetList.Contains(InCheckWidget.Get());
}

TArray<TWeakObjectPtr<UUIViewBase>> UUIViewSystem::GetAllStackedViewsInternal(UClass* InWidgetClass) const
{
	TArray<TWeakObjectPtr<UUIViewBase>> RetArr;
	if (ThisWeak.IsValid() == false || IsValid(InWidgetClass) == false)
		return RetArr;
	
	if (ThisWeak->Widget_UIViewLayer.IsValid() == false)
		return RetArr;
		
	return ThisWeak->Widget_UIViewLayer->GetAllStackedViews(InWidgetClass);
}

void UUIViewSystem::AddLayerInfo(const FName& InLayerName)
{
	if (InLayerName == NAME_None)
		return;

	AllLayerInfo.Add(InLayerName, FUIViewLayerInfo(InLayerName));
}

void UUIViewSystem::AddLayerInfo(const FUIViewLayerInfo& InLayerInfo)
{
	if (InLayerInfo.LayerName == NAME_None)
		return;
	
	AllLayerInfo.Add(InLayerInfo.LayerName, InLayerInfo);
}

void UUIViewSystem::RemoveLayerInfo(const FName& InLayerName)
{
	AllLayerInfo.Remove(InLayerName);
}


bool UUIViewSystem::IsValidLayer(TWeakObjectPtr<UUIViewBase> InLayerWidget) const
{
	FName LayerName = DetermineLayerName(InLayerWidget);
	return IsValidLayer(LayerName);
}

bool UUIViewSystem::IsValidLayer(const FName& InLayerName) const
{
	return HasLayer(InLayerName);
}

void UUIViewSystem::OnDestructViewLayer()
{
	Widget_UIViewLayer.Reset();
}

FSoftClassPath UUIViewSystem::DetermineLayerWidgetPath(const UClass* InLayerWidgetClass) const
{
	FName LayerName = DetermineLayerName(InLayerWidgetClass);
	const FUIViewLayerInfo& LayerInfo = AllLayerInfo.FindRef(LayerName);
	
	FString LayerString = LayerInfo.LayerPath;
	FString WidgetNameString = InLayerWidgetClass->GetFName().ToString();

	TArray<FStringFormatArg> PathArgs;
	PathArgs.Add(LayerString);
	PathArgs.Add(WidgetNameString);
	FString RetPathString = FString::Format(*ClassPathFormat, PathArgs);

	FSoftClassPath RetWidgetPath(RetPathString);
	return RetWidgetPath;
}

FName UUIViewSystem::DetermineLayerName(const UClass* InLayerWidgetClass) const
{
	if (IsValid(InLayerWidgetClass) == false)
		return NAME_None;
	
	TWeakObjectPtr<UUIViewBase> ViewBase = Cast<UUIViewBase>(InLayerWidgetClass->ClassDefaultObject);
	if (ViewBase.IsValid() == false)
		return NAME_None;

	return ViewBase->GetLayerName();
}

FName UUIViewSystem::DetermineLayerName(TWeakObjectPtr<const UUIViewBase> InLayerWidget) const
{
	if (InLayerWidget.IsValid() == false)
    	return NAME_None;
    
    return InLayerWidget->GetLayerName();
}

UUIViewLayerPanel* UUIViewSystem::GetUIViewLayerPanel(TWeakObjectPtr<const UUIViewBase> InLayerWidget) const
{
	if (Widget_UIViewLayer.IsValid() == false)
		return nullptr;

	return Widget_UIViewLayer->GetLayerPanel(InLayerWidget->GetLayerName());
}

void UUIViewSystem::CreateUIViewLayer()
{
	// 이미 레이어 위젯이 생성되어 있다면 리턴.
	if (Widget_UIViewLayer.IsValid())
		return;
	
	// 레이어 위젯은 비동기 로드하지 않음. 즉시 로드.
	auto ViewLayerWidget = LayerClassPath.LoadSynchronous();
	Widget_UIViewLayer = CreateWidget<UUIViewLayer>(OwnerInstance.Get(), ViewLayerWidget);
	if (Widget_UIViewLayer.IsValid())
	{
		Widget_UIViewLayer->AddToViewport();
	}
}

UUIViewBase* UUIViewSystem::GetOrCreateUIViewWidget(const FName InWidgetName)
{
	UUIViewBase* RetUIView = GetCachedUIViewWidget(InWidgetName);
	if (IsValid(RetUIView))
		return RetUIView;
	
	return CreateUIViewWidget(InWidgetName);
}

UUIViewBase* UUIViewSystem::GetCachedUIViewWidget(const FName InWidgetName, bool bUnUsingOnly) const
{
	if (WidgetObjectCacheMap.Contains(InWidgetName) == false)
		return nullptr;
	
	// 이미 생성되어 캐싱된 위젯이 있는 경우, Hide 상태인 위젯을 찾아 리턴.
	TArray<UUIViewBase*> CachedLayerWidgets = WidgetObjectCacheMap.FindRef(InWidgetName).ViewArray;
	for (auto &iter : CachedLayerWidgets)
	{
		TWeakObjectPtr<UUIViewBase> CurWidgetPtr = iter;
		if (CurWidgetPtr.IsValid() && CurWidgetPtr->IsStackedWidget() == false)
		{
			return CurWidgetPtr.Get();
		}
	}
	
	return nullptr;
}

UUIViewBase* UUIViewSystem::CreateUIViewWidget(const FName InWidgetName)
{
	if (OwnerInstance.IsValid() == false)
		return nullptr;

	// 캐싱된 위젯 클래스가 없으면, 위젯 생성 불가. nullptr 리턴.
	if (WidgetCalssCacheMap.Contains(InWidgetName) == false)
		return nullptr;

	UUIViewBase* CreatedLayerWidget = CreateWidget<UUIViewBase>(OwnerInstance.Get(), WidgetCalssCacheMap[InWidgetName]);
	if (IsValid(CreatedLayerWidget))
	{
		AddWidgetObjectCache(InWidgetName, CreatedLayerWidget);
	}
	
	return CreatedLayerWidget;
}

void UUIViewSystem::RegistWidgetLoadInfo(UClass* InWidgetStaticClass, TFunction<void(TWeakObjectPtr<UUIViewBase>)> InShowFunction, TFunction<void()> InCancelFunction)
{
	FUIWidgetLoadInfo WidgetLoadInfo;
	// 레이어 타입 설정.
	// WidgetLoadInfo.ViewLayerType = DetermineLayerType(InWidgetStaticClass);
	// Show / Cancel시 수행할 콜백 함수 Bind.
	WidgetLoadInfo.OnPostShowWidget = InShowFunction;
	WidgetLoadInfo.OnShowCancelWidget = InCancelFunction;

	FName InLayerWidgetName = InWidgetStaticClass->GetFName();
	WidgetLoadInfos.Add(InLayerWidgetName, WidgetLoadInfo);
}

void UUIViewSystem::MarkForWidgetLoadCancel(const FName& InWidgetName)
{
	auto LoadInfo = WidgetLoadInfos.Find(InWidgetName);
	if (LoadInfo)
	{
		LoadInfo->MarkForLoadCancel();
	}
}

bool UUIViewSystem::IsWidgetLoadCanceled(const FName& InWidgetName) const
{
	// 위젯 로드 정보가 없는 경우에도 취소로 간주.
	auto LoadInfo = WidgetLoadInfos.Find(InWidgetName);
	if (LoadInfo == nullptr)
		return true;
	
	return LoadInfo->IsLoadCanceled();
}

void UUIViewSystem::UnRegistWidgetLoadDelegate(const FName& InWidgetName)
{
	if (WidgetLoadInfos.Contains(InWidgetName) == false)
		return;
	
	WidgetLoadInfos.FindRef(InWidgetName).UnRegistAllDelegates();
	WidgetLoadInfos.Remove(InWidgetName);
}

void UUIViewSystem::ExcutePostShowWidget(const FName& InWidgetName, TWeakObjectPtr<UUIViewBase> InWidgetPtr)
{
	auto WidgetLoadInfo = WidgetLoadInfos.Find(InWidgetName);
	if (WidgetLoadInfo == nullptr)
		return;

	WidgetLoadInfo->ExcuteOnPostShowWidget(InWidgetPtr);
}

void UUIViewSystem::ExcuteShowCancelWidget(const FName& InWidgetName)
{
	auto WidgetLoadInfo = WidgetLoadInfos.Find(InWidgetName);
	if (WidgetLoadInfo == nullptr)
		return;

	WidgetLoadInfo->ExcuteOnShowCancelWidget();
}

void UUIViewSystem::AddWidgetClassCache(UClass* InWidget)
{
	// 클래스 캐시는 하나만 존재하면 됨. Contains == false면 리턴.
	FName WidgetName = GetParentNativeClass(InWidget)->GetFName();
	if (WidgetCalssCacheMap.Contains(WidgetName) == false)
	{
		WidgetCalssCacheMap.Add(WidgetName, InWidget);
	}
}

UClass* UUIViewSystem::FindCachedWidgetClass(const FName& InWidgetName)
{
	return WidgetCalssCacheMap.FindRef(InWidgetName);
}

void UUIViewSystem::AddWidgetObjectCache(const FName& InWidgetName, UUIViewBase* InLayerWidget)
{
	TArray<UUIViewBase*>& CacheArray = WidgetObjectCacheMap.FindOrAdd(InWidgetName).ViewArray;
	CacheArray.Add(InLayerWidget);
}

bool UUIViewSystem::IsSyncLoadWidget(UClass* InWidgetStaticClass) const
{
	return InWidgetStaticClass->ImplementsInterface(UIsSyncLoadUIWidget::StaticClass());
}
