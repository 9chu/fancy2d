////////////////////////////////////////////////////////////////////////////////
/// @file  fcyRefObj.h
/// @brief 描述并实现了引用计数接口
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "fcyType.h"

#include <Windows.h>

#define FCYREFOBJ

////////////////////////////////////////////////////////////////////////////////
/// @brief 引用计数接口定义
/// @note  AddRef和Release函数应当加锁
////////////////////////////////////////////////////////////////////////////////
struct fcyRefObj
{
	virtual void AddRef()=0;  ///< @brief 增加接口的引用计数
	virtual void Release()=0; ///< @brief 减少接口的引用计数
	                          ///< @note  当计数器置0时销毁对象
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 引用计数接口实现
/// @note  使用模版进行代码复用防止出现菱形继承
////////////////////////////////////////////////////////////////////////////////
template<class T>
class fcyRefObjImpl : 
	public T
{
private:
	fuInt m_cRef;
public:
	/// @brief AddRef实现
	virtual void AddRef()
	{
		InterlockedIncrement(&m_cRef);
	}

	/// @brief Release实现
	virtual void Release()
	{
		fuInt tRet = InterlockedDecrement(&m_cRef);
		if(tRet == 0)
			delete this;
	}
public:
	fcyRefObjImpl()
		: m_cRef(1) {}
	virtual ~fcyRefObjImpl() {}
};
