////////////////////////////////////////////////////////////////////////////////
/// @file  fuiExceptionMacro.h
/// @brief 辅助异常宏
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <fcyMisc/fcyStringHelper.h>

/// @brief 获取资源并检查合法性
#define FUIGETRESANDCHECK(ResName, ResVar, ResType, ResCheck, ExceptionSrc) \
	ResVar = (ResType*)GetControlStyle()->QueryRes(ResName);             \
	if(!ResName.empty() && !ResVar)                                      \
		throw fcyException(                                              \
			ExceptionSrc,                                                \
			"in control '%s' : Res '%s' not found.",                     \
			fcyStringHelper::WideCharToMultiByte(GetName()).c_str(),     \
			fcyStringHelper::WideCharToMultiByte(ResName).c_str()        \
			);                                                           \
	else if(ResVar && ResVar->GetResType() != ResCheck)                  \
	{                                                                    \
		ResVar = NULL;                                                   \
		throw fcyException(                                              \
			ExceptionSrc,                                                \
			"in control '%s' : Res '%s' type error.",                    \
			fcyStringHelper::WideCharToMultiByte(GetName()).c_str(),     \
			fcyStringHelper::WideCharToMultiByte(ResName).c_str()        \
			);                                                           \
	}                                                                    
