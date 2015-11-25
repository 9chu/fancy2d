////////////////////////////////////////////////////////////////////////////////
/// @file   f2dRenderer.h
/// @brief  fancy2D������Ⱦ���ӿڶ���
/// @note   ������f2D����Ⱦ���ӿ�
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "f2dInterface.h"

#include "f2dRenderDevice.h"

/// @addtogroup f2d��Ⱦϵͳ
/// @brief fancy2d�����е���Ⱦϵͳ���ṩ��2Dͼ�ε���Ⱦ֧�֡�
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D ���鷭תѡ��
////////////////////////////////////////////////////////////////////////////////
enum F2DSPRITEFLIP
{
	F2DSPRITEFLIP_NONE,  ///< @brief �޷�ת
	F2DSPRITEFLIP_H,     ///< @brief ˮƽ��ת
	F2DSPRITEFLIP_V,     ///< @brief ��ֱ��ת
	F2DSPRITEFLIP_HV     ///< @brief ˮƽ��ֱ��ת
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D ����
////////////////////////////////////////////////////////////////////////////////
struct f2dSprite : 
	public f2dInterface
{
	/// @brief ��þ���󶨵�����
	virtual f2dTexture2D* GetTexture()=0;

	/// @brief     ����һ���µ�����
	/// @param[in] pTex ����
	virtual fResult SetTexture(f2dTexture2D* pTex)=0;

	/// @brief ���ض�Ӧ����ľ���
	virtual const fcyRect& GetTexRect()const=0;

	/// @brief ���ö�Ӧ����ľ���
	virtual fResult SetTexRect(const fcyRect& Rect)=0;

	/// @brief �����ȵ�
	virtual const fcyVec2& GetHotSpot()const=0;

	/// @brief     �����ȵ�
	/// @param[in] Point �ȵ�
	virtual fResult SetHotSpot(const fcyVec2& Point)=0;

	/// @brief ���ط�ת��ʽ
	virtual F2DSPRITEFLIP GetFlipType()const=0;
	
	/// @brief     ���÷�ת��ʽ
	/// @param[in] Type ��ת��ʽ
	virtual fResult SetFlipType(F2DSPRITEFLIP Type)=0;

	/// @brief ����Zֵ
	virtual fFloat GetZ()const=0;

	/// @brief     ����Zֵ
	/// @param[in] Value �µ�Zֵ
	virtual fResult SetZ(fFloat Value)=0;

	/// @brief     ���ض������ɫ
	/// @param[in] Index ��������[0~3]������˳ʱ�뷽��λ
	virtual fcyColor GetColor(fuInt Index)const=0;

	/// @brief     ���ض������ɫ
	/// @param[in] pOut ��СΪ4������
	virtual void GetColor(fcyColor* pOut)const=0;

	/// @brief     �������ж������ɫ
	/// @param[in] Color �����ɫ
	virtual fResult SetColor(fcyColor Color)=0;

	/// @brief ���ö�����ɫ
	/// @param[in] Index ��������[0~3]������˳ʱ�뷽��λ
	/// @param[in] Color �����ɫ
	virtual fResult SetColor(fuInt Index, fcyColor Color)=0;

	/// @brief     �������ж������ɫ
	/// @param[in] pArr ��СΪ4������
	virtual fResult SetColor(fcyColor* pArr)=0;

	/// @brief ���ƾ���
	/// @param[in] pGraph ��ͼ����ָ��
	/// @param[in] Dest   Ŀ�����
	virtual fResult Draw(f2dGraphics2D* pGraph, const fcyRect& Dest, fBool bAutoFixCoord = true) = 0;

	/// @brief ���ƾ���
	/// @param[in] pGraph ��ͼ����ָ��
	/// @param[in] Dest   Ŀ�����
	/// @param[in] SubTex ����������[0~1, 0~1]
	virtual fResult Draw(f2dGraphics2D* pGraph, const fcyRect& Dest, const fcyRect& SubTex)=0;

	/// @brief ���ƾ���
	/// @param[in] pGraph ��ͼ����ָ��
	/// @param[in] v1     ����1
	/// @param[in] v2     ����2
	/// @param[in] v3     ����3
	/// @param[in] v4     ����4
	virtual fResult Draw(f2dGraphics2D* pGraph, fcyVec3 v1, fcyVec3 v2, fcyVec3 v3, fcyVec3 v4, fBool bAutoFixCoord = true) = 0;

	/// @brief ���ƾ���
	/// @param[in] pGraph ��ͼ����ָ��
	/// @param[in] Center ������ʾ����
	virtual fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center)=0;

	/// @brief ���ƾ���
	/// @param[in] pGraph ��ͼ����ָ��
	/// @param[in] Center ������ʾ����
	/// @param[in] Scale  ����
	virtual fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale)=0;

	/// @brief ���ƾ���
	/// @param[in] pGraph ��ͼ����ָ��
	/// @param[in] Center ������ʾ����
	/// @param[in] Scale  ����
	/// @param[in] SubTex ����������[0~1, 0~1]
	virtual fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, const fcyRect& SubTex)=0;

	/// @brief ���ƾ���
	/// @param[in] pGraph   ��ͼ����ָ��
	/// @param[in] Center   ������ʾ����
	/// @param[in] Scale    ����
	/// @param[in] Rotation ˳ʱ����ת�Ƕȣ������ơ�
	virtual fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, fFloat Rotation, fBool bAutoFixCoord = true) = 0;
	
	/// @brief ���ƾ���
	/// @param[in] pGraph   ��ͼ����ָ��
	/// @param[in] Center   ������ʾ����
	/// @param[in] Scale    ����
	/// @param[in] Rotation ˳ʱ����ת�Ƕȣ������ơ�
	/// @param[in] SubTex   ����������[0~1, 0~1]
	virtual fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, fFloat Rotation, const fcyRect& SubTex)=0;

	/// @brief ���ƾ���
	/// @note  �÷�����Ӧy�ᷭת�����
	/// @param[in] pGraph ��ͼ����ָ��
	/// @param[in] Center ������ʾ����
	virtual fResult Draw2(f2dGraphics2D* pGraph, const fcyVec2& Center)=0;

	/// @brief ���ƾ���
	/// @note  �÷�����Ӧy�ᷭת�����
	/// @param[in] pGraph ��ͼ����ָ��
	/// @param[in] Center ������ʾ����
	/// @param[in] Scale  ����
	virtual fResult Draw2(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale)=0;

	/// @brief ���ƾ���
	/// @note  �÷�����Ӧy�ᷭת�����
	/// @param[in] pGraph ��ͼ����ָ��
	/// @param[in] Center ������ʾ����
	/// @param[in] Scale  ����
	/// @param[in] SubTex ����������[0~1, 0~1]
	virtual fResult Draw2(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, const fcyRect& SubTex)=0;

	/// @brief ���ƾ���
	/// @note  �÷�����Ӧy�ᷭת�����
	/// @param[in] pGraph   ��ͼ����ָ��
	/// @param[in] Center   ������ʾ����
	/// @param[in] Scale    ����
	/// @param[in] Rotation ˳ʱ����ת�Ƕȣ������ơ�
	virtual fResult Draw2(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, fFloat Rotation, fBool bAutoFixCoord = true) = 0;

	/// @brief ���ƾ���
	/// @note  �÷�����Ӧy�ᷭת�����
	/// @param[in] pGraph   ��ͼ����ָ��
	/// @param[in] Center   ������ʾ����
	/// @param[in] Scale    ����
	/// @param[in] Rotation ˳ʱ����ת�Ƕȣ������ơ�
	/// @param[in] SubTex   ����������[0~1, 0~1]
	virtual fResult Draw2(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, fFloat Rotation, const fcyRect& SubTex)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D�򵥼�������Ⱦ��
////////////////////////////////////////////////////////////////////////////////
struct f2dGeometryRenderer : 
	public f2dInterface
{
	/// @brief ���Zֵ
	virtual fFloat GetZ()=0;

	/// @brief     ����Zֵ
	/// @param[in] ZValue Zֵ
	virtual void SetZ(fFloat ZValue)=0;

	/// @brief     ��û��ʵ���ɫ
	/// @param[in] Index ����[0~2]��0=�����ɫ��1=�м���ɫ��2=�ڲ���ɫ
	virtual fcyColor GetPenColor(fuInt Index)=0;

	/// @brief     ���û�����ɫ
	/// @param[in] Index ����[0~2]��0=�����ɫ��1=�м���ɫ��2=�ڲ���ɫ
	/// @param[in] Color ��ɫֵ
	virtual void SetPenColor(fuInt Index, const fcyColor& Color)=0;

	/// @brief ��û��ʴ�С
	virtual fFloat GetPenSize()=0;

	/// @brief     ���û��ʴ�С
	/// @param[in] Size ���ʴ�С
	virtual void SetPenSize(fFloat Size)=0;

	/// @brief     ����
	/// @param[in] pGraph ��ͼָ��
	/// @param[in] Begin  ���
	/// @param[in] End    �յ�
	virtual fResult DrawLine(f2dGraphics2D* pGraph, const fcyVec2& Begin, const fcyVec2& End)=0;

	/// @brief     ���ƾ���
	/// @param[in] pGraph ��ͼָ��
	/// @param[in] Rect   ����
	virtual fResult DrawRectangle(f2dGraphics2D* pGraph, const fcyRect& Rect)=0;

	/// @brief     ����Բ
	/// @param[in] pGraph ��ͼָ��
	/// @param[in] Center ����
	/// @param[in] R      �뾶
	/// @param[in] Divide ϸ��
	virtual fResult DrawCircle(f2dGraphics2D* pGraph, const fcyVec2& Center, float R, fuInt Divide=50)=0;

	/// @brief     ������
	/// @param[in] pGraph    ��ͼָ��
	/// @param[in] Rect      �������
	/// @param[in] pColorArr ��ɫ����[0~3]��˳ʱ���ĸ��ǵ���ɫ
	virtual fResult FillRectangle(f2dGraphics2D* pGraph, const fcyRect& Rect, const fcyColor* pColorArr)=0;

	/// @brief     ���Բ
	/// @param[in] pGraph      ��ͼָ��
	/// @param[in] Center      ����
	/// @param[in] R           �뾶
	/// @param[in] CenterColor ������ɫ
	/// @param[in] BoundColor  �߽���ɫ
	/// @param[in] Divide      ϸ��
	virtual fResult FillCircle(f2dGraphics2D* pGraph, const fcyVec2& Center, float R, const fcyColor& CenterColor, const fcyColor& BoundColor, fuInt Divide=50)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D ��������
////////////////////////////////////////////////////////////////////////////////
struct f2dGlyphInfo
{
	fcyRect GlyphPos;    ///< @brief �����������ϵ�uv����
	fcyVec2 GlyphSize;   ///< @brief ���δ�С
	fcyVec2 BrushPos;    ///< @brief �ʴ������������Ͻ�����
	fcyVec2 Advance;     ///< @brief �ʴ���ǰ����(����)
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D �����ṩ��
////////////////////////////////////////////////////////////////////////////////
struct f2dFontProvider :
	public f2dInterface
{
	/// @brief ���Ĭ���и�
	virtual fFloat GetLineHeight()=0;

	/// @brief ��û��ߵ������ߵľ���
	virtual fFloat GetAscender()=0;

	/// @brief ��Ascender�෴
	virtual fFloat GetDescender()=0;

	/// @brief ���ػ�������
	virtual f2dTexture2D* GetCacheTexture()=0;

	/// @breif     �����ַ���
	/// @note      ��Զ�̬�������ֵ��ض�����
	/// @param[in] String �ַ���
	virtual fResult CacheString(fcStrW String)=0;

	/// @brief      ����һ������
	/// @param[in]  pGraph    ����ʹ�õ���Ⱦ�����ձ�ʾֻ��ѯ��������
	/// @param[in]  Character ������ַ�
	/// @param[out] InfoOut   ���ص���������
	virtual fResult QueryGlyph(f2dGraphics* pGraph, fCharW Character, f2dGlyphInfo* InfoOut)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D ����ѡ��
////////////////////////////////////////////////////////////////////////////////
enum F2DFONTFLAG
{
	F2DFONTFLAG_NONE,    ///< @brief ��ѡ��
	F2DFONTFLAG_OUTLINE  ///< @brief [��δʵ��] �������
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D ������Ƽ�����
////////////////////////////////////////////////////////////////////////////////
struct f2dFontRendererListener
{
	/// @brief          ��׼����������
	/// @param[in]      Index     ���ַ����ı����е�����
	/// @param[in]      Character �ַ�
	/// @param[in][out] DrawPos   ��ʼ����λ��
	/// @param[in][out] Adv       ǰ����
	/// @return         �����Ƿ񱻻��ƣ�ע�������Ƿ���ƣ�ǰ�����������á�
	virtual fBool OnGlyphBeginDraw(fuInt Index, fCharW Character, fcyVec2& DrawPos, fcyVec2& Adv) { return true; }

	/// @brief          ��������
	/// @param[in][out] pVerts �������飬��СΪ4
	virtual void OnGlyphCalcuCoord(f2dGraphics2DVertex pVerts[]) { }
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D ������Ⱦ��
////////////////////////////////////////////////////////////////////////////////
struct f2dFontRenderer :
	public f2dInterface
{
	/// @brief ���ؼ�����
	virtual f2dFontRendererListener* GetListener()=0;
	
	/// @brief ���ü�����
	virtual void SetListener(f2dFontRendererListener* pListener)=0;

	/// @brief ���������ṩ��
	virtual f2dFontProvider* GetFontProvider()=0;

	/// @brief     ���������ṩ��
	/// @param[in] pProvider �����ṩ�ߣ��ǿ�
	virtual fResult SetFontProvider(f2dFontProvider* pProvider)=0;

	/// @brief ���Zֵ
	virtual fFloat GetZ()=0;

	/// @brief     ����Zֵ
	/// @param[in] ZValue Zֵ
	virtual void SetZ(fFloat ZValue)=0;

	/// @brief     ��ö�����ɫ
	/// @param[in] Index ����[0~3]
	virtual fcyColor GetColor(fuInt Index)=0;

	/// @brief     ����������ɫ
	/// @param[in] Color ������ɫ
	virtual void SetColor(const fcyColor& Color)=0;

	/// @brief     ���ö�����ɫ
	/// @param[in] Index ����[0~3]
	/// @param[in] Color ��ɫ
	virtual void SetColor(fuInt Index, const fcyColor& Color)=0;

	/// @brief ��÷�ת��ʽ
	virtual F2DSPRITEFLIP GetFlipType()=0;

	/// @brief     ���÷�ת��ʽ
	/// @param[in] Type ��ת��ʽ
	virtual void SetFlipType(F2DSPRITEFLIP Type)=0;

	/// @brief ��ȡ��������
	virtual fcyVec2 GetScale()=0;

	/// @brief ������������
	/// @note  �÷�����Ӱ��MeasureString����
	virtual void SetScale(fcyVec2 Scale)=0;

	/// @brief     ����һ���ַ������ջ��ƵĴ�С
	/// @param[in] String �ַ���
	/// @param[in] bStrictWidth �ϸ���
	virtual fcyRect MeasureString(fcStrW String, bool bStrictWidth=true)=0;

	/// @brief     ����һ���ַ������ջ��ƵĿ��
	/// @param[in] String �ַ���
	virtual fFloat MeasureStringWidth(fcStrW String)=0;

	/// @brief     ��������
	/// @param[in] pGraph   ��Ⱦ��
	/// @param[in] Text     ����
	/// @param[in] StartPos ������ʼ��׼
	virtual fResult DrawTextW(f2dGraphics2D* pGraph, fcStrW Text, const fcyVec2& StartPos)=0;

	/// @brief     ��������
	/// @param[in]  pGraph   ��Ⱦ��
	/// @param[in]  Text     ����
	/// @param[in]  Count    ��������Ϊ-1��Ϊȫ������
	/// @param[in]  StartPos ������ʼ��׼
	/// @param[out] PosOut   ������ֹλ�ã��ɿ�
	virtual fResult DrawTextW(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, const fcyVec2& StartPos, fcyVec2* PosOut)=0;

	/// @brief      ��������
	/// @param[in]  pGraph   ��Ⱦ��
	/// @param[in]  Text     ����
	/// @param[in]  Count    ��������Ϊ-1��Ϊȫ������
	/// @param[in]  Bias     ��б�ǣ�0Ϊ����б
	/// @param[in]  StartPos ������ʼ��׼
	/// @param[out] PosOut   ������ֹλ�ã��ɿ�
	virtual fResult DrawTextW(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, fFloat Bias, const fcyVec2& StartPos, fcyVec2* PosOut) = 0;

	/// @brief     ��������
	/// @brief      �÷�����Ӧy�����ϵ����
	/// @param[in] pGraph   ��Ⱦ��
	/// @param[in] Text     ����
	/// @param[in] StartPos ������ʼ��׼
	virtual fResult DrawTextW2(f2dGraphics2D* pGraph, fcStrW Text, const fcyVec2& StartPos) = 0;

	/// @brief     ��������
	/// @brief      �÷�����Ӧy�����ϵ����
	/// @param[in]  pGraph   ��Ⱦ��
	/// @param[in]  Text     ����
	/// @param[in]  Count    ��������Ϊ-1��Ϊȫ������
	/// @param[in]  StartPos ������ʼ��׼
	/// @param[out] PosOut   ������ֹλ�ã��ɿ�
	virtual fResult DrawTextW2(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, const fcyVec2& StartPos, fcyVec2* PosOut) = 0;

	/// @brief      ��������
	/// @brief      �÷�����Ӧy�����ϵ����
	/// @param[in]  pGraph   ��Ⱦ��
	/// @param[in]  Text     ����
	/// @param[in]  Count    ��������Ϊ-1��Ϊȫ������
	/// @param[in]  Bias     ��б�ǣ�0Ϊ����б
	/// @param[in]  StartPos ������ʼ��׼
	/// @param[out] PosOut   ������ֹλ�ã��ɿ�
	virtual fResult DrawTextW2(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, fFloat Bias, const fcyVec2& StartPos, fcyVec2* PosOut) = 0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D���鶯��ʵ��
////////////////////////////////////////////////////////////////////////////////
struct f2dSpriteAnimationInstance
{
	fuInt KeyFrameIndex;
	fuInt KeyFrameElapsedTime;
	F2DSPRITEFLIP FlipType;
	fcyColor BlendColor;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D���鶯��
////////////////////////////////////////////////////////////////////////////////
struct f2dSpriteAnimation : 
	public f2dInterface
{
	/// @brief ����֡��Ӧ�ľ���
	virtual f2dSprite* GetSprite(fuInt Index)=0;
	/// @brief ����֡��Ӧ��ʱ��
	virtual fInt GetKeyFrameTime(fuInt Index)const=0;
	/// @brief ����֡��Ӧ�ľ���
	virtual fResult SetSprite(fuInt Index, f2dSprite* pSprite)=0;
	/// @brief ����֡��Ӧ��ʱ��
	virtual fResult SetKeyFrameTime(fuInt Index, fuInt Time)=0;

	/// @brief ������֡��
	virtual fuInt GetFrameCount()const=0;
	/// @brief ���عؼ�֡����
	virtual fuInt GetKeyFrameCount()const=0;

	/// @brief ׷�ӹؼ�֡
	virtual fuInt Append(fuInt FrameTime, f2dTexture2D* FrameTex)=0;
	/// @brief ׷�ӹؼ�֡
	virtual fuInt Append(fuInt FrameTime, f2dTexture2D* FrameTex, const fcyRect& TexRect)=0;
	/// @brief ׷�ӹؼ�֡
	virtual fuInt Append(fuInt FrameTime, f2dTexture2D* FrameTex, const fcyRect& TexRect, const fcyVec2& HotPos)=0;
	/// @brief �ָ����
	virtual void SplitAndAppend(fuInt FrameTime, f2dTexture2D* FrameTex, const fcyRect& TexRect, fuInt SplitCount)=0;
	/// @brief �ָ����
	virtual void SplitAndAppend(fuInt* FrameTimeArr, f2dTexture2D* FrameTex, const fcyRect& TexRect, fuInt SplitCount)=0;
	/// @brief ����ָ����
	virtual void ReverseSplitAndAppend(fuInt FrameTime, f2dTexture2D* FrameTex, const fcyRect& TexRect, fuInt SplitCount)=0;
	/// @brief ����ָ����
	virtual void ReverseSplitAndAppend(fuInt* FrameTimeArr, f2dTexture2D* FrameTex, const fcyRect& TexRect, fuInt SplitCount)=0;
	/// @brief ���
	virtual void Clear()=0;

	/// @brief ����ѭ�����
	virtual fInt GetLoopStart()const=0;
	/// @brief ����ѭ�����
	virtual void SetLoopStart(fInt Index)=0;
	/// @brief ����ѭ���յ�
	virtual fInt GetLoopEnd()const=0;
	/// @brief ����ѭ���յ�
	virtual void SetLoopEnd(fInt Index)=0;
	/// @brief �Ƿ�ѭ��
	virtual fBool IsLoop()const=0;
	/// @brief ����ѭ��
	virtual void SetLoop(fBool bLoop)=0;

	/// @brief     ��ʼ��ʵ��
	/// @param[in] Instance ʵ������
	virtual void InitInstance(f2dSpriteAnimationInstance& Instance, bool bResetBlendColor = true, bool bResetFlipType = true)const=0;
	/// @brief     ��ʼ��ʵ����������β
	/// @param[in] Instance ʵ������
	virtual void InitInstanceToEnd(f2dSpriteAnimationInstance& Instance, bool bResetBlendColor = true, bool bResetFlipType = true)const=0;
	/// @brief  ����ʵ��
	/// @return ��������������false
	virtual fBool StepInstance(f2dSpriteAnimationInstance& Instance)const=0;
	/// @brief  ����һ��
	/// @return ��������������false
	virtual fBool StepbackInstance(f2dSpriteAnimationInstance& Instance)const=0;
	/// @brief  ����֡
	virtual fResult JumpTo(f2dSpriteAnimationInstance& Instance, fuInt FrameIndex)const=0;
	/// @brief  �����ؼ�֡
	virtual fResult JumpToKeyFrame(f2dSpriteAnimationInstance& Instance, fuInt FrameIndex)const=0;

	/// @brief ���ƾ��鶯��
	/// @param[in] Instance ����ʵ��
	/// @param[in] pGraph   ��ͼ����ָ��
	/// @param[in] Dest     Ŀ�����
	virtual fResult DrawInstance(const f2dSpriteAnimationInstance& Instance, f2dGraphics2D* pGraph, const fcyRect& Dest)=0;
	/// @brief     ���ƾ��鶯��
	/// @param[in] Instance ����ʵ��
	/// @param[in] pGraph   ��ͼ����ָ��
	/// @param[in] Dest     Ŀ�����
	/// @param[in] SubTex   ����������[0~1, 0~1]
	virtual fResult DrawInstance(const f2dSpriteAnimationInstance& Instance, f2dGraphics2D* pGraph, const fcyRect& Dest, const fcyRect& SubTex)=0;
	/// @brief     ���ƾ��鶯��
	/// @param[in] Instance ����ʵ��
	/// @param[in] pGraph   ��ͼ����ָ��
	/// @param[in] Center   ������ʾ����
	virtual fResult DrawInstance(const f2dSpriteAnimationInstance& Instance, f2dGraphics2D* pGraph, const fcyVec2& Center)=0;
	/// @brief     ���ƾ��鶯��
	/// @param[in] Instance ����ʵ��
	/// @param[in] pGraph   ��ͼ����ָ��
	/// @param[in] Center   ������ʾ����
	/// @param[in] Scale    ����
	virtual fResult DrawInstance(const f2dSpriteAnimationInstance& Instance, f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale)=0;
	/// @brief     ���ƾ��鶯��
	/// @param[in] Instance ����ʵ��
	/// @param[in] pGraph   ��ͼ����ָ��
	/// @param[in] Center   ������ʾ����
	/// @param[in] Scale    ����
	/// @param[in] SubTex   ����������[0~1, 0~1]
	virtual fResult DrawInstance(const f2dSpriteAnimationInstance& Instance, f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, const fcyRect& SubTex)=0;
	/// @brief     ���ƾ���
	/// @param[in] Instance ����ʵ��
	/// @param[in] pGraph   ��ͼ����ָ��
	/// @param[in] Center   ������ʾ����
	/// @param[in] Scale    ����
	/// @param[in] Rotation ˳ʱ����ת�Ƕȣ������ơ�
	virtual fResult DrawInstance(const f2dSpriteAnimationInstance& Instance, f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, fFloat Rotation)=0;
	/// @brief     ���ƾ���
	/// @param[in] Instance ����ʵ��
	/// @param[in] pGraph   ��ͼ����ָ��
	/// @param[in] Center   ������ʾ����
	/// @param[in] Scale    ����
	/// @param[in] Rotation ˳ʱ����ת�Ƕȣ������ơ�
	/// @param[in] SubTex   ����������[0~1, 0~1]
	virtual fResult DrawInstance(const f2dSpriteAnimationInstance& Instance, f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, fFloat Rotation, const fcyRect& SubTex)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D 2D���ӳ� ���Ӳ���
////////////////////////////////////////////////////////////////////////////////
struct f2dParticleCreationDesc
{
	fcyRect PosRange;      ///< @brief ������Χƫ��
	fcyVec2 VRange;        ///< @brief ���ٶȷ�Χ
	fcyVec2 VAngleRange;   ///< @brief ���ٶȷ���Χ
	fcyVec2 ARRange;       ///< @brief ��������ٶȷ�Χ
	fcyVec2 ATRange;       ///< @brief ��������ٶȷ�Χ
	fcyVec2 InitialAngle;  ///< @brief ��ʼ��ƫת�Ƕȷ�Χ
	fcyVec2 SpinRange;     ///< @brief �����ٶȷ�Χ
	fcyVec2 LifeTimeRange; ///< @brief ����ʱ�䷶Χ

	fcyColor StartColor;      ///< @brief ��ʼ��ɫ
	fcyVec2  StartColorRange; ///< @brief ��ʼ��ɫ������Χ
	                          ///< @note  StartColor+=random(StartColorRange.x, StartColorRange.y)
	fcyColor EndColor;        ///< @brief ��ֹ��ɫ

	fcyVec2 StartScale;       ///< @brief ��ʼ����
	fcyVec2 StartScaleRange;  ///< @brief ��ʼ���Ŷ�����Χ
	                          ///< note   ����ͬ��
	fcyVec2 EndScale;         ///< @brief ��ֹ����
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D 2D���ӳ� ����
////////////////////////////////////////////////////////////////////////////////
struct f2dParticleForce
{
	virtual void OnUpdate(fFloat ElapsedTime, fcyVec2& Pos, fcyVec2& V, float& Angle)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D 2D���ӳ�
////////////////////////////////////////////////////////////////////////////////
struct f2dParticlePool : 
	public f2dInterface
{
	/// @brief �������������
	virtual fuInt GetRandomSeed()=0;
	/// @brief     �������������
	/// @param[in] Seed ����
	virtual void SetRandomSeed(fuInt Seed)=0;
	/// @brief ���������
	virtual fFloat RandFloat(fFloat Min, fFloat Max)=0;

	/// @brief ������������
	virtual fuInt GetCount()=0;

	/// @brief ������������
	virtual fuInt GetCapacity()=0;

	/// @brief �������
	virtual void Clear()=0;

	/// @brief     ׷��һ����
	virtual fResult AppendForce(f2dParticleForce* pForce)=0;
	
	/// @brief     �Ƴ�һ����
	/// @note      ���ж����ͬ�Ķ����Ƴ���һ������
	virtual bool RemoveForce(f2dParticleForce* pForce)=0;

	/// @brief     �������
	virtual void ClearForce()=0;

	/// @brief     ����һ������
	/// @param[in] pSprite           ����ľ���
	/// @param[in] Center            ��������
	/// @param[in] EmittedCountRange ����������Χ
	/// @param[in] Desc              ������ϸ����
	virtual fResult Emitted(f2dSprite* pSprite, const fcyVec2& Center, const fcyVec2& EmittedCountRange, const f2dParticleCreationDesc& Desc)=0;

	/// @brief     ��������״̬
	/// @param[in] ElapsedTime ����ʱ��
	virtual void Update(fFloat ElapsedTime)=0;
	/// @brief     ��Ⱦ����
	/// @param[in] pGraph ��ͼ�ӿ�
	virtual void Render(f2dGraphics2D* pGraph)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D��Ⱦ��
////////////////////////////////////////////////////////////////////////////////
struct f2dRenderer
{
	virtual f2dRenderDevice* GetDevice()=0;  ///< @brief �����Ⱦ�豸ָ��

	// === ���󴴽� ===
	/// @brief      ����Sprite2D
	/// @param[in]  pTex �󶨵�����
	/// @param[out] pOut ������Sprite
	virtual fResult CreateSprite2D(f2dTexture2D* pTex, f2dSprite** pOut)=0;

	/// @brief      ����Sprite2D
	/// @param[in]  pTex �󶨵�����
	/// @param[in]  Org  ԭʼ�������
	/// @param[out] pOut ������Sprite
	virtual fResult CreateSprite2D(f2dTexture2D* pTex, const fcyRect& Org, f2dSprite** pOut)=0;

	/// @brief      ����Sprite2D
	/// @param[in]  pTex    �󶨵�����
	/// @param[in]  Org     ԭʼ�������
	/// @param[in]  HotSopt �ȵ�
	/// @param[out] pOut    ������Sprite
	virtual fResult CreateSprite2D(f2dTexture2D* pTex, const fcyRect& Org, const fcyVec2& HotSopt, f2dSprite** pOut)=0;

	/// @brief      ������������Ⱦ��
	/// @param[out] pOut �������Ⱦ������
	virtual fResult CreateGeometryRenderer(f2dGeometryRenderer** pOut)=0;

	/// @brief      ����������Ⱦ��
	/// @param[in]  pProvider ����Դ
	/// @param[out] pOut      ������������
	virtual fResult CreateFontRenderer(f2dFontProvider* pProvider, f2dFontRenderer** pOut)=0;

	/// @brief      �������ļ���������
	/// @param[in]  pStream   �����ļ�����ȡ�����ļ�
	/// @param[in]  FaceIndex ���ж��Face������ָ��������0������Чֵ��
	/// @param[in]  FontSize  �����С
	/// @param[in]  Flag      ����ѡ��
	/// @param[out] pOut      ���ص��������
	virtual fResult CreateFontFromFile(f2dStream* pStream, fuInt FaceIndex, const fcyVec2& FontSize, F2DFONTFLAG Flag, f2dFontProvider** pOut)=0;

	/// @brief      ��ϵͳ��������
	/// @param[in]  FaceName  ������ע����е�����
	/// @param[in]  FaceIndex ���ж��Face������ָ��������0������Чֵ��
	/// @param[in]  FontSize  �����С
	/// @param[in]  Flag      ����ѡ��
	/// @param[out] pOut      ���ص��������
	virtual fResult CreateSystemFont(fcStrW FaceName, fuInt FaceIndex, const fcyVec2& FontSize, F2DFONTFLAG Flag, f2dFontProvider** pOut)=0;

	/// @brief      �������������
	/// @param[in]  pDefineFile ���嶨���ļ�
	/// @param[in]  pTex        ����
	/// @param[out] pOut        ���ص��������
	virtual fResult CreateFontFromTex(f2dStream* pDefineFile, f2dTexture2D* pTex, f2dFontProvider** pOut)=0;
	
	/// @brief      �������������
	/// @param[in]  pDefineFile ���嶨��
	/// @param[in]  pTex        ����
	/// @param[out] pOut        ���ص��������
	virtual fResult CreateFontFromTex(fcStrW pDefineText, f2dTexture2D* pTex, f2dFontProvider** pOut)=0;

	/// @brief      �������鶯��
	/// @param[out] pOut        ���صĶ���
	virtual fResult CreateSpriteAnimation(f2dSpriteAnimation** pOut)=0;

	/// @brief      �������ӳ�
	/// @param[out] pOut        ���صĶ���
	virtual fResult CreateParticlePool(f2dParticlePool** pOut)=0;
};

/// @}
