////////////////////////////////////////////////////////////////////////////////
/// @file  fuiProperty.h
/// @brief fancyUI 控件属性
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <fcyRefObj.h>
#include <fcyException.h>
#include <fcyMisc/fcyFunctor.h>

#include <fcyMath.h>

#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////
/// @brief 属性访问接口
////////////////////////////////////////////////////////////////////////////////
struct fuiProperty
{
	virtual const std::wstring& Get()
	{
		throw fcyException("fuiProperty::Get", "Property is not readable.");
	}
	virtual void Set(const std::wstring& Str)
	{
		throw fcyException("fuiProperty::Get", "Property is not writable.");
	}
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 属性访问辅助
////////////////////////////////////////////////////////////////////////////////
template<class T>
struct fuiPropertyAccessorHelper
{
	static void DefaultGetter(std::wstring&, const T*);
	static void DefaultSetter(const std::wstring&, T*);
};

template<class T>
void fuiPropertyAccessorHelper<T>::DefaultGetter(std::wstring&, const T*)
{
	throw fcyException("fuiPropertyAccessorHelper<T>::DefaultGetter", "Property type is not support.");
}

template<class T>
void fuiPropertyAccessorHelper<T>::DefaultSetter(const std::wstring&, T*)
{
	throw fcyException("fuiPropertyAccessorHelper<T>::DefaultSetter", "Property type is not support.");
}

////////////////////////////////////////////////////////////////////////////////
/// @brief 属性访问辅助 - 特化
////////////////////////////////////////////////////////////////////////////////
template<>
struct fuiPropertyAccessorHelper<int>
{
	static void DefaultGetter(std::wstring&, const int*);
	static void DefaultSetter(const std::wstring&, int*);
};

template<>
struct fuiPropertyAccessorHelper<bool>
{
	static void DefaultGetter(std::wstring&, const bool*);
	static void DefaultSetter(const std::wstring&, bool*);
};

template<>
struct fuiPropertyAccessorHelper<float>
{
	static void DefaultGetter(std::wstring&, const float*);
	static void DefaultSetter(const std::wstring&, float*);
};

template<>
struct fuiPropertyAccessorHelper<fcyVec2>
{
	static void DefaultGetter(std::wstring&, const fcyVec2*);
	static void DefaultSetter(const std::wstring&, fcyVec2*);
};

template<>
struct fuiPropertyAccessorHelper<fcyVec3>
{
	static void DefaultGetter(std::wstring&, const fcyVec3*);
	static void DefaultSetter(const std::wstring&, fcyVec3*);
};

template<>
struct fuiPropertyAccessorHelper<fcyVec4>
{
	static void DefaultGetter(std::wstring&, const fcyVec4*);
	static void DefaultSetter(const std::wstring&, fcyVec4*);
};

template<>
struct fuiPropertyAccessorHelper<fcyRect>
{
	static void DefaultGetter(std::wstring&, const fcyRect*);
	static void DefaultSetter(const std::wstring&, fcyRect*);
};

template<>
struct fuiPropertyAccessorHelper<fcyColor>
{
	static void DefaultGetter(std::wstring&, const fcyColor*);
	static void DefaultSetter(const std::wstring&, fcyColor*);
};

template<>
struct fuiPropertyAccessorHelper<std::wstring>
{
	static void DefaultGetter(std::wstring&, const std::wstring*);
	static void DefaultSetter(const std::wstring&, std::wstring*);
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 属性访问器
////////////////////////////////////////////////////////////////////////////////
template<typename T>
class fuiPropertyAccessor :
	public fuiProperty
{
public:
	typedef fcyFunctor<void(std::wstring&, const T*)> PropGetter;
	typedef fcyFunctor<void(const std::wstring&, T*)> PropSetter;
protected:
	T* m_pObj;               ///< @brief 关联的属性成员
	std::wstring m_PropStr;  ///< @brief 临时储存返回值
	PropGetter m_Getter;     ///< @brief 读函数
	PropSetter m_Setter;     ///< @brief 写函数
public: // 接口实现
	const std::wstring& Get()
	{
		if(m_Getter.Empty())
			m_PropStr = fuiProperty::Get();
		else
			m_Getter(m_PropStr, m_pObj);

		return m_PropStr;
	}
	void Set(const std::wstring& Str)
	{
		if(m_Setter.Empty())
			fuiProperty::Set(Str);
		else
			m_Setter(Str, m_pObj);
	}
public:
	fuiPropertyAccessor& operator=(const fuiPropertyAccessor& Right)
	{
		m_pObj = Right.m_pObj;
		m_Getter = Right.m_Getter;
		m_Setter = Right.m_Setter;

		return *this;
	}
public:
	fuiPropertyAccessor()
		: m_pObj(NULL) {}
	fuiPropertyAccessor(
		T* pObj, 
		const PropGetter& Getter = fuiPropertyAccessorHelper<T>::DefaultGetter, 
		const PropSetter& Setter = fuiPropertyAccessorHelper<T>::DefaultSetter
		)
		: m_pObj(pObj), m_Getter(Getter), m_Setter(Setter) {}
	fuiPropertyAccessor(const fuiPropertyAccessor& Right)
		: m_pObj(Right.m_pObj), m_Getter(Right.m_Getter), m_Setter(Right.m_Setter) {}
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 属性集合
////////////////////////////////////////////////////////////////////////////////
class fuiPropertySet
{
protected:
	/// @brief 属性列表
	std::unordered_map<std::wstring, fuiProperty*> m_PropList;
protected:
	/// @brief 注册属性
	void RegisterProperty(const std::wstring& Str, fuiProperty* pProp);
public: // 属性操作
	/// @brief     返回属性接口
	/// @param[in] Prop 属性名
	fuiProperty* QueryPropertyInterface(const std::wstring& Prop);
	/// @brief     获得原始属性字符串
	/// @param[in] PropName 属性名称
	const std::wstring& RawGetProperty(const std::wstring& PropName)const;
	/// @brief     设置原始属性字符串
	/// @param[in] PropName 属性名称
	/// @param[in] Value    属性值
	void RawSetProperty(const std::wstring& PropName, const std::wstring& Value);
	/// @brief     获得属性
	/// @param[in] PropName 属性名称
	template<typename T>
	T GetProperty(const std::wstring& PropName)const
	{
		T tRet;
		const std::wstring& tStr = RawGetProperty(PropName);

		fuiPropertyAccessorHelper<T>::DefaultSetter(tStr, &tRet);

		return tRet;
	}
	template<>
	const std::wstring& GetProperty<const std::wstring&>(const std::wstring& PropName)const
	{
		const std::wstring& tStr = RawGetProperty(PropName);

		return tStr;
	}
	/// @brief     设置属性
	/// @param[in] PropName 属性名称
	/// @param[in] Value    属性值
	template<typename T>
	void SetProperty(const std::wstring& PropName, const T& Value)
	{
		std::wstring tStr;
		fuiPropertyAccessorHelper<T>::DefaultGetter(tStr, &Value);

		RawSetProperty(PropName, tStr);
	}
	template<>
	void SetProperty<std::wstring>(const std::wstring& PropName, const std::wstring& Value)
	{
		RawSetProperty(PropName, Value);
	}
public:
	fuiPropertySet() {}
	~fuiPropertySet() {}
};
