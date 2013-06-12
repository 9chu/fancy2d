////////////////////////////////////////////////////////////////////////////////
/// @file  fuiFactory.h
/// @brief fancyUI 工厂
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "fuiControl.h"

#include <unordered_map>

class fuiPage;

////////////////////////////////////////////////////////////////////////////////
/// @brief UI工厂
////////////////////////////////////////////////////////////////////////////////
class fuiFactory
{
public:
	static fuiFactory* GetInstace()
	{
		static fuiFactory s_pFactory;

		return &s_pFactory;
	}
private:
	typedef fcyRefPointer<fuiControl> (*lpCreateFunc)(fuiPage*, const std::wstring&);

	template<typename T>
	struct CreationHelper
	{
		static fcyRefPointer<fuiControl> CreateControl(fuiPage* pContext, const std::wstring& Name)
		{
			fcyRefPointer<T> tRet;
			tRet.DirectSet(new T(pContext, Name));

			return tRet;
		}
	};
protected:
	std::unordered_map<std::wstring, lpCreateFunc> m_Creator;
public:
	template<typename T>
	void RegisterControlCreator(const std::wstring& ControlTypeName)
	{
		m_Creator[ControlTypeName] = CreationHelper<T>::CreateControl;
	}

	fcyRefPointer<fuiControl> CreateControl(const std::wstring& ControlTypeName, fuiPage* pRootPage, const std::wstring& Name);
public:
	fuiFactory();
	~fuiFactory();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief UI资源工厂
////////////////////////////////////////////////////////////////////////////////
class fuiResFactory
{
public:
	static fuiResFactory* GetInstace()
	{
		static fuiResFactory s_pFactory;

		return &s_pFactory;
	}
private:
	typedef fcyRefPointer<fuiRes> (*lpCreateFunc)(const std::wstring&);

	template<typename T>
	struct CreationHelper
	{
		static fcyRefPointer<fuiRes> CreateRes(const std::wstring& Name)
		{
			fcyRefPointer<T> tRet;
			tRet.DirectSet(new T(Name));

			return tRet;
		}
	};
protected:
	std::unordered_map<std::wstring, lpCreateFunc> m_Creator;
public:
	template<typename T>
	void RegisterResCreator(const std::wstring& ResTypeName)
	{
		m_Creator[ResTypeName] = CreationHelper<T>::CreateRes;
	}

	fcyRefPointer<fuiRes> CreateRes(const std::wstring& ResTypeName, const std::wstring& Name);
public:
	fuiResFactory();
	~fuiResFactory();
};

