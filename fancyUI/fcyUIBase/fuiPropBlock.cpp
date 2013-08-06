#include "fuiPropBlock.h"

#include "fuiControl.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiPropBlock::fuiPropBlock()
	: m_pControl(NULL)
{}

fuiPropBlock::fuiPropBlock(fuiControl* pControl, fBool CaptureImmediate)
	: m_pControl(pControl)
{
	if(!pControl)
		throw fcyException("fuiPropBlock::fuiPropBlock", "Param 'pControl' is null.");

	if(CaptureImmediate)
		Get();
}

fuiPropBlock::fuiPropBlock(const fuiPropBlock& Org)
	: m_pControl(Org.m_pControl), m_ControlProp(Org.m_ControlProp), m_SubControlProp(Org.m_SubControlProp)
{}

fuiPropBlock::~fuiPropBlock()
{}

void fuiPropBlock::getAllProp(fuiPropBlock& Block, fuiControl* p)
{
	Block.m_ControlProp.clear();
	Block.m_SubControlProp.clear();

	// 遍历所有属性
	fuiPropertySet::Iterator i = p->GetPropertyBegin();
	fuiPropertySet::Iterator iEnd = p->GetPropertyEnd();
	while(i != iEnd)
	{
		Block.m_ControlProp[i.GetPropName()] = i.GetPropInterface()->Get();

		++i;
	}
	
	// 遍历所有子控件
	fuInt iSubControlCount = p->GetSubControlCount();
	for(fuInt i = 0; i<iSubControlCount; i++)
	{
		fuiControl* pSub = p->GetSubControl(i);

		Block.m_SubControlProp.push_back(fuiPropBlock(pSub, false));

		getAllProp(Block.m_SubControlProp.back(), pSub);
	}
}

void fuiPropBlock::setAllProp(fuiPropBlock& Block)
{
	fuiControl* pControl = Block.GetBindControl();

	// 遍历所有属性
	unordered_map<wstring, wstring>::iterator i = Block.m_ControlProp.begin();
	while(i != Block.m_ControlProp.end())
	{
		try
		{
			pControl->RawSetProperty(i->first, i->second);
		}
		catch(const fcyException& e)
		{
			if(strcmp(e.GetDesc(), "Property is not writable.")!=0)
				throw;
		}

		++i;
	}

	// 遍历所有子控件属性
	vector<fuiPropBlock>::iterator j = Block.m_SubControlProp.begin();
	while(j != Block.m_SubControlProp.end())
	{
		setAllProp(*j);

		++j;
	}
}

fuiControl* fuiPropBlock::GetBindControl()
{
	return m_pControl;
}

void fuiPropBlock::Get()
{
	getAllProp(*this, m_pControl);
}

void fuiPropBlock::Set()
{
	setAllProp(*this);
}
