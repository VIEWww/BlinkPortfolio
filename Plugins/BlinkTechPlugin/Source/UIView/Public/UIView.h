// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#ifndef ENUM_TO_STRING
#define ENUM_TO_STRING(ENUM_TYPE, ENUM_VALUE) SJLib::TEnumToString<ENUM_TYPE>(#ENUM_TYPE, ENUM_VALUE, FString(TEXT("InValid")))
#endif

#ifndef STRING_TO_ENUM
#define STRING_TO_ENUM(ENUM_TYPE, STRING_VALUE) SJLib::TStringToEnum<ENUM_TYPE>(#ENUM_TYPE, STRING_VALUE)
#endif

#ifndef UI_BIND_BUTTON_CLICK_EVENT
#define UI_BIND_BUTTON_CLICK_EVENT(BUTTON, DELEGATE) if(IsValid(BUTTON)) { BUTTON->OnClicked.Clear(); BUTTON->OnClicked.AddDynamic(this, DELEGATE); }
#endif

#ifndef UI_BIND_COMMONBUTTON_CLICK_EVENT
#define UI_BIND_COMMONBUTTON_CLICK_EVENT(BUTTON, DELEGATE) if(IsValid(BUTTON)) { BUTTON->OnClicked().Clear(); BUTTON->OnClicked().AddUObject(this, DELEGATE); } 
#endif

#ifndef UI_BIND_ANIM_STRATED
#define UI_BIND_ANIM_STRATED(ANIM, DELEGATE) if(IsValid(ANIM)) \
{\
FWidgetAnimationDynamicEvent AnimStartEvent;\
AnimStartEvent.BindDynamic(this, DELEGATE);\
BindToAnimationStarted(ANIM, AnimStartEvent);\
}
#endif

#ifndef UI_BIND_ANIM_FINISHED
#define UI_BIND_ANIM_FINISHED(ANIM, DELEGATE) if(IsValid(ANIM)) \
{\
FWidgetAnimationDynamicEvent AnimFinishEvent;\
AnimFinishEvent.BindDynamic(this, DELEGATE);\
BindToAnimationFinished(ANIM, AnimFinishEvent);\
}
#endif


class FUIViewModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
