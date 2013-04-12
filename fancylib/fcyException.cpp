#include "fcyException.h"

#include <Windows.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fcyException::fcyException(fcStr Src, fcStr Desc)
{
	m_Time = GetTickCount();
	m_ExcpSrc = Src;
	m_ExcpDesc = Desc;
}

fcyException::~fcyException(void)
{}

fuInt fcyException::GetTime()const
{
	return m_Time;
}

fcStr fcyException::GetSrc()const
{
	return m_ExcpSrc.c_str();
}

fcStr fcyException::GetDesc()const
{
	return m_ExcpDesc.c_str();
}

////////////////////////////////////////////////////////////////////////////////

fcyWin32Exception::fcyWin32Exception(fcStr Src, fcStr Info)
	: m_LastError(GetLastError()), fcyException(Src, Info)
{
	char tNumStr[16] = { 0 };
	_itoa_s((int)m_LastError, tNumStr, 16, 10);
	m_ExcpDesc = m_ExcpDesc + " ( LastErr = " + tNumStr + " )";
}

fcyWin32Exception::~fcyWin32Exception()
{}

fuInt fcyWin32Exception::GetErrorCode()const
{
	return m_LastError;
}

////////////////////////////////////////////////////////////////////////////////

fcyWin32COMException::fcyWin32COMException(fcStr Src, fcStr Info, fInt HRet)
	: m_HR(HRet), fcyException(Src, Info)
{
	char tNumStr[16] = { 0 };
	_itoa_s((int)m_HR, tNumStr, 16, 10);
	m_ExcpDesc = m_ExcpDesc + " ( HRESULT = " + tNumStr + " )";
}

fcyWin32COMException::~fcyWin32COMException()
{}

fInt fcyWin32COMException::GetResult()const
{
	return m_HR;
}