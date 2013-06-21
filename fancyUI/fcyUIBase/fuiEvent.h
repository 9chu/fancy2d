////////////////////////////////////////////////////////////////////////////////
/// @file  fuiEvent.h
/// @brief fancyUI 控件事件及委托
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <fcyRefObj.h>
#include <fcyException.h>
#include <fcyMisc/fcyFunctor.h>

#include <fcyMath.h>

#include <vector>
#include <unordered_map>

class fuiControl;
class fuiEventArgs;

////////////////////////////////////////////////////////////////////////////////
/// @brief 委托
////////////////////////////////////////////////////////////////////////////////
class fuiDelegate
{
public:
	typedef fcyFunctor<void(fuiControl*, fuiEventArgs*)> EventCallBack;
	static fuiEventArgs EmptyEventArgs;
protected:
	std::vector<EventCallBack> m_FunctorList;
public:
	fuiDelegate& operator=(const fuiDelegate& Right);
	void operator()(fuiControl* pControl, fuiEventArgs* pArgs);
	fuiDelegate& operator+=(const EventCallBack& CallBack);
public:
	void Append(const EventCallBack& CallBack);
	void Clear();
	void Exec(fuiControl* pControl, fuiEventArgs* pArgs = &EmptyEventArgs);
public:
	fuiDelegate();
	fuiDelegate(const fuiDelegate& Right);
	~fuiDelegate();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 事件集合
////////////////////////////////////////////////////////////////////////////////
class fuiEventSet
{
protected:
	/// @brief 事件列表
	std::unordered_map<std::wstring, fuiDelegate> m_EventList;

	/// @brief 事件防止重入标记
	std::unordered_map<std::wstring, fBool> m_EventFlag;
protected:
	/// @brief 注册事件
	void RegisterEvent(const std::wstring& Str);
	/// @brief 执行事件
	void ExecEvent(const std::wstring& Str, fuiControl* pControl, fuiEventArgs* pArgs);
public: // 事件操作
	fuiDelegate& GetEvent(const std::wstring& EventName);
public:
	fuiEventSet() {}
	~fuiEventSet() {}
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 回调事件参数
////////////////////////////////////////////////////////////////////////////////
class fuiEventArgs
{
public:
	/// @brief 事件类型
	enum EVENTTYPE
	{
		EVENTTYPE_USER,         ///< @brief 用户事件
		EVENTTYPE_CANCELABLE,   ///< @brief 可撤销事件
		EVENTTYPE_POSITION,     ///< @brief 位置事件
		EVENTTYPE_OBJECT,       ///< @brief UI对象事件
		EVENTTYPE_KEY,          ///< @brief 按键事件
		EVENTTYPE_CHAR,         ///< @brief 字符事件
		EVENTTYPE_STRING,       ///< @brief 字符串事件
		EVENTTYPE_IMECANDIDATE  ///< @brief IME候选词事件
	};
public:
	/// @brief     查询事件
	/// @param[in] Type 是否支持的事件
	/// @return    若支持事件，则返回相应的对象
	virtual void* QueryEvent(EVENTTYPE Type)
	{
		return NULL;
	}
public:
	fuiEventArgs() {}
	virtual ~fuiEventArgs() {}
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 可撤销事件
////////////////////////////////////////////////////////////////////////////////
class fuiCancelableEventArgs :
	public fuiEventArgs
{
protected:
	fBool m_bCancel;
public:
	fBool GetCancel()const
	{
		return m_bCancel;
	}
	void SetCancel(fBool V)
	{
		m_bCancel = V;
	}
public: // 接口实现
	void* QueryEvent(EVENTTYPE Type)
	{
		if(Type == EVENTTYPE_CANCELABLE)
			return this;
		else
			return NULL;
	}
public:
	fuiCancelableEventArgs()
		: m_bCancel(false) {}
	~fuiCancelableEventArgs() {}
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 位置事件
////////////////////////////////////////////////////////////////////////////////
class fuiPositionEventArgs :
	public fuiEventArgs
{
protected:
	fcyVec2 m_Pos;
public:
	const fcyVec2& GetPos()const
	{
		return m_Pos;
	}
	void SetPos(const fcyVec2& V)
	{
		m_Pos = V;
	}
public: // 接口实现
	void* QueryEvent(EVENTTYPE Type)
	{
		if(Type == EVENTTYPE_POSITION)
			return this;
		else
			return NULL;
	}
public:
	fuiPositionEventArgs() {}
	~fuiPositionEventArgs() {}
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 对象事件
////////////////////////////////////////////////////////////////////////////////
class fuiObjectEventArgs :
	public fuiEventArgs
{
protected:
	fuiControl* m_pObj;
public:
	fuiControl* GetObj()const
	{
		return m_pObj;
	}
	void SetObj(fuiControl* V)
	{
		m_pObj = V;
	}
public: // 接口实现
	void* QueryEvent(EVENTTYPE Type)
	{
		if(Type == EVENTTYPE_OBJECT)
			return this;
		else
			return NULL;
	}
public:
	fuiObjectEventArgs() {}
	~fuiObjectEventArgs() {}
};

enum F2DINPUTKEYCODE;

////////////////////////////////////////////////////////////////////////////////
/// @brief 按键事件
////////////////////////////////////////////////////////////////////////////////
class fuiKeyEventArgs :
	public fuiEventArgs
{
protected:
	F2DINPUTKEYCODE m_pObj;
public:
	F2DINPUTKEYCODE GetKeyCode()const
	{
		return m_pObj;
	}
	void SetKeyCode(F2DINPUTKEYCODE V)
	{
		m_pObj = V;
	}
public: // 接口实现
	void* QueryEvent(EVENTTYPE Type)
	{
		if(Type == EVENTTYPE_KEY)
			return this;
		else
			return NULL;
	}
public:
	fuiKeyEventArgs() {}
	~fuiKeyEventArgs() {}
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 字符事件
////////////////////////////////////////////////////////////////////////////////
class fuiCharEventArgs :
	public fuiEventArgs
{
protected:
	fCharW m_Value;
public:
	fCharW GetChar()const
	{
		return m_Value;
	}
	void SetChar(fCharW V)
	{
		m_Value = V;
	}
public: // 接口实现
	void* QueryEvent(EVENTTYPE Type)
	{
		if(Type == EVENTTYPE_CHAR)
			return this;
		else
			return NULL;
	}
public:
	fuiCharEventArgs() {}
	~fuiCharEventArgs() {}
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 字符串事件
////////////////////////////////////////////////////////////////////////////////
class fuiStringEventArgs :
	public fuiEventArgs
{
protected:
	std::wstring m_Str;
public:
	const std::wstring& GetString()const
	{
		return m_Str;
	}
	void SetString(const std::wstring& V)
	{
		m_Str = V;
	}
public: // 接口实现
	void* QueryEvent(EVENTTYPE Type)
	{
		if(Type == EVENTTYPE_STRING)
			return this;
		else
			return NULL;
	}
public:
	fuiStringEventArgs() {}
	~fuiStringEventArgs() {}
};

struct f2dIMECandidateList;

////////////////////////////////////////////////////////////////////////////////
/// @brief IME候选词事件
////////////////////////////////////////////////////////////////////////////////
class fuiIMECandidateEventArgs :
	public fuiEventArgs
{
protected:
	f2dIMECandidateList* m_pList;
public:
	f2dIMECandidateList* GetList()const
	{
		return m_pList;
	}
	void SetList(f2dIMECandidateList* pList)
	{
		m_pList = pList;
	}
public: // 接口实现
	void* QueryEvent(EVENTTYPE Type)
	{
		if(Type == EVENTTYPE_IMECANDIDATE)
			return this;
		else
			return NULL;
	}
public:
	fuiIMECandidateEventArgs() {}
	~fuiIMECandidateEventArgs() {}
};

