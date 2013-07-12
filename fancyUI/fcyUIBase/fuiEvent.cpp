#include "fuiEvent.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiEventArgs fuiDelegate::EmptyEventArgs;

fuiDelegate::fuiDelegate()
{}

fuiDelegate::fuiDelegate(const fuiDelegate& Right)
	: m_FunctorList(Right.m_FunctorList)
{}

fuiDelegate::~fuiDelegate()
{}

fuiDelegate& fuiDelegate::operator=(const fuiDelegate& Right)
{
	m_FunctorList = Right.m_FunctorList;

	return *this;
}

void fuiDelegate::operator()(fuiControl* pControl, fuiEventArgs* pArgs)
{
	Exec(pControl, pArgs);
}

fuiDelegate& fuiDelegate::operator+=(const EventCallBack& CallBack)
{
	Append(CallBack);

	return *this;
}

void fuiDelegate::Append(const EventCallBack& CallBack)
{
	m_FunctorList.push_back(CallBack);
}

void fuiDelegate::Clear()
{
	m_FunctorList.clear();
}

void fuiDelegate::Exec(fuiControl* pControl, fuiEventArgs* pArgs)
{
	for(vector<EventCallBack>::iterator i = m_FunctorList.begin();
		i != m_FunctorList.end();
		++i)
	{
		(*i)(pControl, pArgs);
	}
}

////////////////////////////////////////////////////////////////////////////////

void fuiEventSet::RegisterEvent(const std::wstring& Str)
{
	m_EventList[Str] = fuiDelegate();
}

void fuiEventSet::ExecEvent(const std::wstring& Str, fuiControl* pControl, fuiEventArgs* pArgs)
{
	if(!m_EventFlag[Str])
	{
		m_EventFlag[Str] = true;
		try
		{
			m_EventList[Str](pControl, pArgs);
		}
		catch(...)
		{
			m_EventFlag[Str] = false;

			throw;
		}
		m_EventFlag[Str] = false;
	}
}

fuiDelegate& fuiEventSet::GetEvent(const std::wstring& EventName)
{
	unordered_map<std::wstring, fuiDelegate>::iterator i = m_EventList.find(EventName);
	if(i == m_EventList.end())
		throw fcyException("fuiEventSet::GetEvent", "Event is not exsit.");
	else
		return i->second;
}
