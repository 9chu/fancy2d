#include "fuiStyle.h"

#include "fuiFactory.h"

#include <fcyParser/fcyXml.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiStyle::fuiStyle()
{}

fuiStyle::~fuiStyle()
{}

void fuiStyle::RegisterRes(const std::wstring Name, fuiRes* pRes)
{
	if(m_ResList.find(Name) != m_ResList.end())
		throw fcyException("fuiStyle::RegisterRes", "Res has already existed.");

	m_ResList[Name] = pRes;
}

void fuiStyle::UnregisterRes(const std::wstring Name)
{
	unordered_map<std::wstring, fcyRefPointer<fuiRes>>::iterator i;

	if((i = m_ResList.find(Name)) == m_ResList.end())
		throw fcyException("fuiStyle::UnregisterRes", "Res not found.");

	m_ResList.erase(i);
}

fuiRes* fuiStyle::QueryRes(const std::wstring Name)
{
	unordered_map<std::wstring, fcyRefPointer<fuiRes>>::iterator i;

	if((i = m_ResList.find(Name)) == m_ResList.end())
		return NULL;
	return i->second;
}

void fuiStyle::LoadResFromFile(f2dStream* pStream, fuiResProvider* pProvider)
{
	if(!pStream)
		throw fcyException("fuiStyle::LoadResFromFile", "Param 'pStream' is null.");

	pStream->SetPosition(FCYSEEKORIGIN_BEG, 0);

	fcyXml tXml(pStream);
	fcyXmlNode* pRootNode = tXml.GetRoot();
	for(fuInt i = 0; i<pRootNode->GetNodeCount(); ++i)
	{
		fcyXmlNode* pSubNode = pRootNode->GetNode(i);

		if(!pSubNode->HasAttribute(L"Name"))
			throw fcyException("fuiStyle::LoadResFromFile", "Property 'Name' not found.");
		wstring tName = pSubNode->GetAttribute(L"Name");

		fcyRefPointer<fuiRes> tRes = fuiResFactory::GetInstace()->CreateRes(pSubNode->GetName(), tName);
		
		// 设置属性
		fcyXmlNode::AttributeIterator tIter = pSubNode->GetFirstAttribute();
		while(tIter != pSubNode->GetLastAttribute())
		{
			if(wcscmp(tIter.GetName(), L"Name") != 0)
				tRes->RawSetProperty(tIter.GetName(), tIter.GetContent());

			++tIter;
		}

		// 构造资源
		tRes->ConstructRes(pProvider);
		
		// 注册
		RegisterRes(tName, tRes);
	}
}
