////////////////////////////////////////////////////////////////////////////////
/// @file  f2dRenderDevice.h
/// @brief fancy2D��Ⱦ�豸�ӿڶ���
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "f2dInterface.h"

/// @addtogroup f2d��Ⱦϵͳ
/// @brief fancy2d�����е���Ⱦϵͳ���ṩ��2Dͼ�ε���Ⱦ֧�֡�
/// @{

// =============================== �����װ ===================================

// �����ʽ��

////////////////////////////////////////////////////////////////////////////////
/// @brief ����ҳ���ʽ
////////////////////////////////////////////////////////////////////////////////
enum F2DSURFACEFMT
{
	F2DSURFACEFMT_UNKNOWN,  ///< @brief δ֪��ʽ
	F2DSURFACEFMT_A8R8G8B8  ///< @brief ARGB��ʽ
};

// ����ӿڣ�

////////////////////////////////////////////////////////////////////////////////
/// @brief ����ӿ�
////////////////////////////////////////////////////////////////////////////////
struct f2dTexture :
	public f2dInterface
{
	/// @brief   ����ڲ��������
	/// @warning �߼�����������������Ҫ����ʹ��
	/// @return  ����IDirect3DTexutreBase9�ӿ�
	virtual void* GetHandle()=0;

	virtual fuInt GetDimension()=0;   ///< @brief ��������ά��, ��������Tex1D,Tex2D,Tex3D
	virtual fBool IsDynamic()=0;      ///< @brief �Ƿ���޸�
	virtual fBool IsRenderTarget()=0; ///< @brief �Ƿ�����ȾĿ��
};

////////////////////////////////////////////////////////////////////////////////
/// @brief ��ά����ӿ�
////////////////////////////////////////////////////////////////////////////////
struct f2dTexture2D :
	public f2dTexture
{
	virtual fuInt GetWidth()=0;    ///< @brief ���ؿ��
	virtual fuInt GetHeight()=0;   ///< @brief ���ظ߶�

	/// @brief      ��������
	/// @note       ֻ�ж�̬������Ա����������������ͽ���������ԣ���δ����ǰֻ������һ�Ρ�
	/// @param[in]  pLockRect ��������������ΪNULL��������������
	/// @param[in]  Discard   ���ԭʼ�������ڱ�ʹ��������ԭʼ����
	/// @param[out] Pitch     �ж����ֽ�
	/// @param[out] pOut      ���ص��ڴ�ָ�룬�Զ����㵽��������ֽ�
	virtual fResult Lock(fcyRect* pLockRect, fBool Discard, fuInt* Pitch, fData* pOut)=0;

	/// @brief ��������
	virtual fResult Unlock()=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief ���ģ�建����
////////////////////////////////////////////////////////////////////////////////
struct f2dDepthStencilSurface :
	public f2dInterface
{
	virtual void* GetHandle()=0;   ///< @brief ����IDirect3DSurface���󣬸߼�����
	virtual fuInt GetWidth()=0;    ///< @brief ���ؿ��
	virtual fuInt GetHeight()=0;   ///< @brief ���ظ߶�
};

// =============================== Ч����װ ===================================

// Ч��������

////////////////////////////////////////////////////////////////////////////////
/// @brief Ч����������
////////////////////////////////////////////////////////////////////////////////
enum F2DEPTYPE
{
	F2DEPTYPE_UNKNOWN,  ///< @brief δ֪��������
	F2DEPTYPE_VALUE,    ///< @brief ֵ����
	F2DEPTYPE_ARRAY,    ///< @brief ����
	F2DEPTYPE_STRUCT    ///< @brief �ṹ��
};

////////////////////////////////////////////////////////////////////////////////
/// @brief Ч������ֵ����
////////////////////////////////////////////////////////////////////////////////
enum F2DEPVTYPE
{
	F2DEPVTYPE_UNKNOWN,   ///< @brief δ֪����
	F2DEPVTYPE_VOID,      ///< @brief ������
	F2DEPVTYPE_BOOL,      ///< @brief Bool��
	F2DEPVTYPE_FLOAT,     ///< @brief Float��
	F2DEPVTYPE_INT,       ///< @brief Int��
	F2DEPVTYPE_VECTOR,    ///< @brief ����
	F2DEPVTYPE_MATRIX,    ///< @brief ����
	F2DEPVTYPE_TEXTURE1D, ///< @brief 1D����
	F2DEPVTYPE_TEXTURE2D, ///< @brief 2D����
	F2DEPVTYPE_TEXTURE3D, ///< @brief 3D����
	F2DEPVTYPE_STRING     ///< @brief �ַ���
};

// Ч����������

////////////////////////////////////////////////////////////////////////////////
/// @brief Ч������
/// @note  ע��һ��Effect�����ͷţ��ýӿڽ���Ч��
////////////////////////////////////////////////////////////////////////////////
struct f2dEffectParam
{
	virtual fcStr GetName()=0;                 ///< @brief ���ز�������
	virtual F2DEPTYPE GetType()=0;             ///< @brief ���ز�������

	virtual fcStr GetSemantic()=0;                        ///< @brief ��ò�������
	virtual f2dEffectParam* GetAnnotation(fcStr Name)=0;  ///< @brief ����ע�Ͷ���
	virtual f2dEffectParam* GetAnnotation(fuInt Index)=0; ///< @brief ����ע�Ͷ���
	virtual fuInt GetAnnotationCount()=0;                 ///< @brief ����ע�Ͷ������
};

////////////////////////////////////////////////////////////////////////////////
/// @brief Ч����������
/// @note  ע��һ��Effect�����ͷţ��ýӿڽ���Ч��
////////////////////////////////////////////////////////////////////////////////
struct f2dEffectParamValue :
	public f2dEffectParam
{
	virtual F2DEPVTYPE GetValueType()=0;      ///< @brief �������

	virtual fuInt GetRow()=0;                 ///< @brief �������/��������
	virtual fuInt GetColumn()=0;              ///< @brief �������/��������

	virtual fBool IsRowFirstMatrix()=0;       ///< @brief �Ƿ�Ϊ�����Ⱦ���

	virtual fBool GetBool()=0;                ///< @brief ����Bool��
	virtual fFloat GetFloat()=0;              ///< @brief ����Float��
	virtual fInt GetInt()=0;                  ///< @brief ����Int��
	virtual fcyVec4 GetVector()=0;            ///< @brief ��������
	virtual fcyMatrix4 GetMatrix()=0;         ///< @brief ���ؾ���
	virtual f2dTexture* GetTexture()=0;       ///< @brief ����������
	virtual fcStr GetString()=0;              ///< @brief �����ַ�������

	virtual fResult SetBool(fBool Value)=0;   ///< @brief ����Bool��
	virtual fResult SetFloat(fFloat Value)=0; ///< @brief ����Float��
	virtual fResult SetInt(fInt Value)=0;     ///< @brief ����Int��
	virtual fResult SetVector(const fcyVec4& Value)=0;    ///< @brief ��������
	virtual fResult SetMatrix(const fcyMatrix4& Value)=0; ///< @brief ���þ���
	virtual fResult SetTexture(f2dTexture* pTex)=0;       ///< @brief ��������
	virtual fResult SetString(fcStr Str)=0;               ///< @brief �����ַ���
};

////////////////////////////////////////////////////////////////////////////////
/// @brief �������Ͳ���
/// @note  ע��һ��Effect�����ͷţ��ýӿڽ���Ч��
////////////////////////////////////////////////////////////////////////////////
struct f2dEffectParamArr :
	public f2dEffectParam
{
	/// @brief ����Ԫ�ظ���
	virtual fuInt GetElementCount()=0;

	/// @brief ͨ��Ԫ��Index��ȡԪ��
	virtual f2dEffectParam* GetElement(fuInt Index)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief �ṹ���Ͳ���
/// @note  ע��һ��Effect�����ͷţ��ýӿڽ���Ч��
////////////////////////////////////////////////////////////////////////////////
struct f2dEffectParamStruct :
	public f2dEffectParam
{
	///< @brief ��ó�Ա��Ŀ
	virtual fuInt GetElementCount()=0;

	///< @brief ͨ����Ա����ó�Ա
	virtual f2dEffectParam* GetElement(fcStr Name)=0;
	///< @brief ͨ����Ա�±��ó�Ա
	virtual f2dEffectParam* GetElement(fuInt Index)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief Ч����ں���
/// @note  ע��һ��Effect�����ͷţ��ýӿڽ���Ч��
////////////////////////////////////////////////////////////////////////////////
struct f2dEffectFunction
{
	virtual fcStr GetName()=0;                            ///< @brief �����������
	virtual f2dEffectParam* GetAnnotation(fcStr Name)=0;  ///< @brief ����ע�Ͷ���
	virtual f2dEffectParam* GetAnnotation(fuInt Index)=0; ///< @brief ����ע�Ͷ���
	virtual fuInt GetAnnotationCount()=0;                 ///< @brief ����ע�Ͷ������
};

////////////////////////////////////////////////////////////////////////////////
/// @brief Ч������
/// @note  ע��һ��Effect�����ͷţ��ýӿڽ���Ч��
////////////////////////////////////////////////////////////////////////////////
struct f2dEffectPass
{
	virtual fuInt GetIndex()=0;                           ///< @brief ����Pass��Index
	virtual fcStr GetName()=0;                            ///< @brief ��������
	virtual f2dEffectParam* GetAnnotation(fcStr Name)=0;  ///< @brief ����ע�Ͷ���
	virtual f2dEffectParam* GetAnnotation(fuInt Index)=0; ///< @brief ����ע�Ͷ���
	virtual fuInt GetAnnotationCount()=0;                 ///< @brief ����ע�Ͷ������
};

////////////////////////////////////////////////////////////////////////////////
/// @brief Ч������
////////////////////////////////////////////////////////////////////////////////
struct f2dEffectTechnique
{
	virtual fcStr GetName()=0;                            ///< @brief ��������
	virtual f2dEffectParam* GetAnnotation(fcStr Name)=0;  ///< @brief ����ע�Ͷ���
	virtual f2dEffectParam* GetAnnotation(fuInt Index)=0; ///< @brief ����ע�Ͷ���
	virtual fuInt GetAnnotationCount()=0;                 ///< @brief ����ע�Ͷ������

	virtual fBool IsValidate()=0;                 ///< @brief ����豸�Ƿ�֧�ָ�Ч��
	virtual fuInt GetPassCount()=0;               ///< @brief ����Ч�����θ���
	virtual f2dEffectPass* GetPass(fuInt Index)=0;///< @brief �����Ⱦ����
	virtual f2dEffectPass* GetPass(fcStr Name)=0; ///< @brief �����Ⱦ����
};

////////////////////////////////////////////////////////////////////////////////
/// @brief Ч������
/// @note  ���������ȼ� = 16
////////////////////////////////////////////////////////////////////////////////
struct f2dEffect :
	public f2dInterface
{
	/// @brief   ���ID3DXEffect����
	/// @warning �߼�����
	virtual void* GetHandle()=0;

	virtual fuInt GetParamCount()=0;                  ///< @brief ��ò�������
	virtual f2dEffectParam* GetParam(fuInt Index)=0;  ///< @brief ��ò���
	virtual f2dEffectParam* GetParam(fcStr Name)=0;   ///< @brief ��ò���

	virtual fuInt GetTechniqueCount()=0;                     ///< @brief ��ü�������
	virtual f2dEffectTechnique* GetTechnique(fuInt Index)=0; ///< @brief ��ü���
	virtual f2dEffectTechnique* GetTechnique(fcStr Name)=0;  ///< @brief ��ü���

	virtual fuInt GetFunctionCount()=0;                     ///< @brief ��ú�������
	virtual f2dEffectFunction* GetFunction(fuInt Index)=0;  ///< @brief ��ú���
	virtual f2dEffectFunction* GetFunction(fcStr Name)=0;   ///< @brief ��ú���
	
	virtual f2dEffectTechnique* GetCurrentTechnique()=0;              ///< @brief ��õ�ǰ��Ⱦ����
	virtual fResult SetCurrentTechnique(f2dEffectTechnique* pTech)=0; ///< @brief ���õ�ǰ��Ⱦ����

	virtual fuInt GetCurrentPassCount()=0; ///< @brief ���ص�ǰ��������Ⱦ���θ���
};

// ============================= ��Ⱦ����װ ===================================

// ���ѡ�

////////////////////////////////////////////////////////////////////////////////
/// @brief ��������
////////////////////////////////////////////////////////////////////////////////
enum F2DBLENDOPERATOR
{
	F2DBLENDOPERATOR_ADD         = 1, ///< @brief ��Ͻ�� = Դ���� + Ŀ������
	F2DBLENDOPERATOR_SUBTRACT    = 2, ///< @brief ��Ͻ�� = Դ���� - Ŀ������
	F2DBLENDOPERATOR_REVSUBTRACT = 3, ///< @brief ��Ͻ�� = Ŀ������ - Դ����
	F2DBLENDOPERATOR_MIN         = 4, ///< @brief ��Ͻ�� = MIN(Դ����, Ŀ������)
	F2DBLENDOPERATOR_MAX         = 5  ///< @brief ��Ͻ�� = MAX(Դ����, Ŀ������)
};

////////////////////////////////////////////////////////////////////////////////
/// @brief �������
/// @note  ��ɫ��Ϲ�ʽ��Color = SrcColor * SrcBlend + DestColor * DestBlend
////////////////////////////////////////////////////////////////////////////////
enum F2DBLENDFACTOR
{
    F2DBLENDFACTOR_ZERO               = 1,   ///< @brief �������(0, 0, 0, 0)
    F2DBLENDFACTOR_ONE                = 2,   ///< @brief �������(1, 1, 1, 1)
    F2DBLENDFACTOR_SRCCOLOR           = 3,   ///< @brief �������(Rs, Gs, Bs, As)
    F2DBLENDFACTOR_INVSRCCOLOR        = 4,   ///< @brief �������(1 - Rs, 1 - Gs, 1 - Bs, 1 - As)
    F2DBLENDFACTOR_SRCALPHA           = 5,   ///< @brief �������(As, As, As, As)
    F2DBLENDFACTOR_INVSRCALPHA        = 6,   ///< @brief �������(1 - As, 1 - As, 1 - As, 1 - As)
    F2DBLENDFACTOR_DESTALPHA          = 7,   ///< @brief �������(Ad, Ad, Ad, Ad)
    F2DBLENDFACTOR_INVDESTALPHA       = 8,   ///< @brief �������(1 - Ad, 1 - Ad, 1 - Ad, 1 - Ad)
    F2DBLENDFACTOR_DESTCOLOR          = 9,   ///< @brief �������(Rd, Gd, Bd, Ad)
    F2DBLENDFACTOR_INVDESTCOLOR       = 10,  ///< @brief �������(1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad)
    F2DBLENDFACTOR_SRCALPHASAT        = 11   ///< @brief �������(f, f, f, 1)������f = min(As, 1 - Ad)
};

////////////////////////////////////////////////////////////////////////////////
/// @brief ���״̬
////////////////////////////////////////////////////////////////////////////////
struct f2dBlendState
{
	F2DBLENDFACTOR SrcBlend;        ///< @brief Դ��ϲ���
	F2DBLENDFACTOR DestBlend;       ///< @brief Ŀ�Ļ�ϲ���
	F2DBLENDOPERATOR BlendOp;       ///< @brief ��������
	F2DBLENDFACTOR AlphaSrcBlend;   ///< @brief AlphaԴ��ϲ���������ʹ��RGBAѡ��
	F2DBLENDFACTOR AlphaDestBlend;  ///< @brief AlphaĿ�Ļ�ϲ���������ʹ��RGBAѡ��
	F2DBLENDOPERATOR AlphaBlendOp;  ///< @brief Alpha��������
};

// ��Ⱦ����

////////////////////////////////////////////////////////////////////////////////
/// @brief ��Ⱦ���ӿ�
////////////////////////////////////////////////////////////////////////////////
struct f2dGraphics :
	public f2dInterface
{
	virtual fBool IsGraphics3D()=0;  ///< @brief �Ƿ�Ϊ3D��Ⱦ��������Ϊ2D��Ⱦ��

	// === ��Ⱦ���̿��� ===
	virtual fBool IsInRender()=0;    ///< @brief ����Ƿ���Begin/End����
	virtual fResult Begin()=0;       ///< @brief ������Ⱦ�������״̬ͬ��
	virtual fResult Flush()=0;       ///< @brief �����ݽ���Ⱦ����
	virtual fResult End()=0;         ///< @brief ������Ⱦ���̲��ݽ���Ⱦ����

	// === ��Ⱦ״̬���� ===
	virtual const fcyMatrix4& GetWorldTransform()=0;  ///< @brief ��������任����
	virtual const fcyMatrix4& GetViewTransform()=0; ///< @brief ���ع۲����
	virtual const fcyMatrix4& GetProjTransform()=0;   ///< @brief ����ͶӰ����

	virtual void SetWorldTransform(const fcyMatrix4& Mat)=0;  ///< @brief ��������任����
	virtual void SetViewTransform(const fcyMatrix4& Mat)=0; ///< @brief ���ù۲����
	virtual void SetProjTransform(const fcyMatrix4& Mat)=0;   ///< @brief ����ͶӰ����

	// === ���״̬���� ===
	virtual const f2dBlendState& GetBlendState()=0;           ///< @brief �������ػ�ϲ���
	virtual void SetBlendState(const f2dBlendState& State)=0; ///< @brief �������ػ�ϲ���
};

// 2D��Ⱦ����

////////////////////////////////////////////////////////////////////////////////
/// @brief 2D��Ⱦ�����ѡ��
////////////////////////////////////////////////////////////////////////////////
enum F2DGRAPH2DBLENDTYPE
{
	F2DGRAPH2DBLENDTYPE_DISABLE,          ///< @brief ����
	F2DGRAPH2DBLENDTYPE_SELECTCOLOR,      ///< @brief ѡ����ɫ(final=color)
	F2DGRAPH2DBLENDTYPE_SELECTTEXTURE,    ///< @brief ѡ������(final=texture)
	F2DGRAPH2DBLENDTYPE_ADD,              ///< @brief ��ɫ���(final=color+texture)
	F2DGRAPH2DBLENDTYPE_SUBTRACT,         ///< @brief ��ɫ���(final=color-texture)
	F2DGRAPH2DBLENDTYPE_MODULATE,         ///< @brief ��ɫ���(final=color*texture)
	F2DGRAPH2DBLENDTYPE_MODULATE2X,       ///< @brief ��ɫ���x2(final=color*texture*2)
	F2DGRAPH2DBLENDTYPE_MODULATE4X,       ///< @brief ��ɫ���x4(final=color*texture*4)
	F2DGRAPH2DBLENDTYPE_ADDSIGNED,        ///< @brief ��ɫ��Ӳ�ƫ��(final=color+texture-0.5)
	F2DGRAPH2DBLENDTYPE_ADDSIGNED2X,      ///< @brief ��ɫ��Ӳ�ƫ��x2(final=(color+texture-0.5)x2)
	F2DGRAPH2DBLENDTYPE_ADDSMOOTH         ///< @brief ƽ�����(final=color+texture-color*texture)
};

////////////////////////////////////////////////////////////////////////////////
/// @brief ��ά��Ⱦ������
////////////////////////////////////////////////////////////////////////////////
struct f2dGraphics2DVertex
{
	fFloat x;      ///< @brief x����
	fFloat y;      ///< @brief y����
	fFloat z;      ///< @brief z����
	fuInt color;   ///< @brief ��������ɫ
	fFloat u;      ///< @brief ����u����
	fFloat v;      ///< @brief ����v����
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 2D��Ⱦ���ӿ�
////////////////////////////////////////////////////////////////////////////////
struct f2dGraphics2D :
	public f2dGraphics
{
	// === ���ѡ�� ===
	/// @brief ������ɫ���״̬
	virtual F2DGRAPH2DBLENDTYPE GetColorBlendType()=0;

	/// @brief ������ɫ���״̬
	virtual fResult SetColorBlendType(F2DGRAPH2DBLENDTYPE Type)=0;

	// === 2D���ƺ��� ===
	/// @brief ���ƾ���
	virtual fResult DrawQuad(
		f2dTexture2D* pTex,
		const f2dGraphics2DVertex& v1, 
		const f2dGraphics2DVertex& v2, 
		const f2dGraphics2DVertex& v3, 
		const f2dGraphics2DVertex& v4,
		fBool bAutoFixCoord = true
		)=0;

	/// @brief ���ƾ���
	virtual fResult DrawQuad(f2dTexture2D* pTex, const f2dGraphics2DVertex* arr, fBool bAutoFixCoord = true)=0;

	/// @brief ԭʼ���ƺ���
	virtual fResult DrawRaw(f2dTexture2D* pTex, fuInt VertCount, fuInt IndexCount, const f2dGraphics2DVertex* VertArr, const fuShort* IndexArr, fBool bAutoFixCoord = true)=0;
};

// 3D��Ⱦ����

////////////////////////////////////////////////////////////////////////////////
/// @brief ����Ԫ����;
////////////////////////////////////////////////////////////////////////////////
enum F2DVDUSAGE
{
    F2DVDUSAGE_POSITION = 0,    ///< @brief ����
    F2DVDUSAGE_BLENDWEIGHT,     ///< @brief ���Ȩ��
    F2DVDUSAGE_BLENDINDICES,    ///< @brief �������
    F2DVDUSAGE_NORMAL,          ///< @brief ����
    F2DVDUSAGE_PSIZE,           ///< @brief ���Ӵ�С
    F2DVDUSAGE_TEXCOORD,        ///< @brief ��������
    F2DVDUSAGE_TANGENT,         ///< @brief ����
    F2DVDUSAGE_BINORMAL,        ///< @brief �η���
    F2DVDUSAGE_TESSFACTOR,      ///< @brief ϸ������
    F2DVDUSAGE_POSITIONT,       ///< @brief 
    F2DVDUSAGE_COLOR,           ///< @brief ��ɫ
    F2DVDUSAGE_FOG,             ///< @brief ��
    F2DVDUSAGE_DEPTH,           ///< @brief ���
    F2DVDUSAGE_SAMPLE           ///< @brief ����
};

////////////////////////////////////////////////////////////////////////////////
/// @brief ����Ԫ����������
////////////////////////////////////////////////////////////////////////////////
enum F2DVDTYPE
{
    F2DVDTYPE_FLOAT1 =  0,  ///< @brief 1*4�ֽڸ���
	                        ///< @note  GPU����չ�� (value, 0., 0., 1.)
    F2DVDTYPE_FLOAT2 =  1,  ///< @brief 2*4�ֽڸ���
	                        ///< @note  GPU����չ�� (value, value, 0., 1.)
    F2DVDTYPE_FLOAT3 =  2,  ///< @brief 3*4�ֽڸ���
	                        ///< @note  GPU����չ�� (value, value, value, 1.)
    F2DVDTYPE_FLOAT4 =  3,  ///< @brief 4*4�ֽڸ���
    F2DVDTYPE_COLOR  =  4,  ///< @brief 4*1�ֽ�
	                        ///< @note  GPU����չ����������[0,1]���Ų� (R, G, B, A)

    F2DVDTYPE_UBYTE4 =  5,  ///< @brief 4*2�ֽ��޷��Ŷ�����
    F2DVDTYPE_SHORT2 =  6,  ///< @brief 2*2�ֽ��з��Ŷ�����
					        ///< @note  GPU����չ�� (value, value, 0., 1.)
    F2DVDTYPE_SHORT4 =  7   ///< @brief 4*2�ֽ��з��Ŷ�����
};

////////////////////////////////////////////////////////////////////////////////
/// @brief ����Ԫ������
////////////////////////////////////////////////////////////////////////////////
struct f2dVertexElement
{
	F2DVDTYPE Type;   ///< @brief ����
	F2DVDUSAGE Usage; ///< @brief ��;
	fByte UsageIndex; ///< @brief ��;����
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 3D��Ⱦ���ӿ�
////////////////////////////////////////////////////////////////////////////////
struct f2dGraphics3D :
	public f2dGraphics
{
	// === FX״̬ ===
	/// @brief ���ص�ǰ��Ⱦ����
	virtual f2dEffect* GetEffect()=0;

	/// @brief ���õ�ǰ��Ⱦ����
	virtual fResult SetEffect(f2dEffect* Effect)=0;

	/// @brief     ��ʼһ����Ⱦ��
	/// @param[in] PassIndex ��Ⱦ���±�
	virtual fResult BeginPass(fuInt PassIndex)=0;

	/// @brief ����һ����Ⱦ��
	virtual fResult EndPass()=0;

	/// @brief ˢ��Effect״̬
	/// @note  ��pass�иı�״̬��Ҫ�ֶ����ñ�����
	virtual fResult CommitState()=0;

	// === PostEffect ===
	/// @brief Ϊִ��PostEffect������Ⱦ
	virtual fResult RenderPostEffect()=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief ͼԪ����
////////////////////////////////////////////////////////////////////////////////
enum F2DPRIMITIVETYPE 
{
	F2DPT_NULL          = 0,  ///< @brief ��Чֵ
    F2DPT_POINTLIST     = 1,  ///< @brief �㼯
    F2DPT_LINELIST      = 2,  ///< @brief �߼�
    F2DPT_LINESTRIP     = 3,  ///< @brief �ߴ���
    F2DPT_TRIANGLELIST  = 4,  ///< @brief �����μ�
    F2DPT_TRIANGLESTRIP = 5,  ///< @brief �����δ�
    F2DPT_TRIANGLEFAN   = 6   ///< @brief ��������
};

////////////////////////////////////////////////////////////////////////////////
/// @brief �����Ӽ�����
////////////////////////////////////////////////////////////////////////////////
struct f2dMeshSubsetInfo
{
	F2DPRIMITIVETYPE Type;  ///< @brief ͼԪ����
	fuInt StartIndex;       ///< @brief ��ʼ����
	fuInt PrimitiveCount;   ///< @brief ͼԪ����
};

////////////////////////////////////////////////////////////////////////////////
/// @brief ����
////////////////////////////////////////////////////////////////////////////////
struct f2dMeshData :
	public f2dInterface
{
	/// @brief      ������������
	/// @param[in]  StartVert ��ʼ�����Ķ���
	/// @param[in]  VertCount Ҫ�����Ķ���������0��ʾȫ������
	/// @param[out] pOut      ����Ļ�����ָ��
	virtual fResult LockVertexData(fuInt StartVert, fuInt VertCount, void** pOut)=0;

	/// @brief �������㻺����
	virtual fResult UnlockVertexData()=0;

	/// @brief      ������������
	/// @param[in]  StartIndex ��ʼ����������
	/// @param[in]  IndexCount Ҫ����������������0��ʾȫ������
	/// @param[out] pOut       ����Ļ�����ָ��
	virtual fResult LockIndexData(fuInt StartIndex, fuInt IndexCount, void** pOut)=0;
	
	/// @brief ��������������
	virtual fResult UnlockIndexData()=0;

	/// @brief      �����Ӽ�
	/// @param[in]  ID   �Ӽ�ID
	/// @param[out] pOut ������Ӽ�����
	virtual fResult GetSubset(fInt ID, f2dMeshSubsetInfo* pOut)=0;

	/// @brief     �����Ӽ�
	/// @param[in] ID             �Ӽ�ID
	/// @param[in] Type           ͼԪ����
	/// @param[in] StartIndex     ��ʼ����
	/// @param[in] PrimitiveCount ͼԪ����
	virtual fResult SetSubset(fInt ID, F2DPRIMITIVETYPE Type, fuInt StartIndex, fuInt PrimitiveCount)=0;

	/// @brief �����Ӽ�����
	virtual fuInt GetSubsetCount()=0;

	/// @brief     ��Ⱦ�Ӽ�
	/// @param[in] ID �Ӽ�ID
	virtual fResult Render(fInt ID)=0;
};

// ============================= ��Ⱦ�豸 ===================================
////////////////////////////////////////////////////////////////////////////////
/// @brief ��Ⱦ�豸����ݵȼ�
////////////////////////////////////////////////////////////////////////////////
enum F2DAALEVEL
{
	F2DAALEVEL_NONE = 0,     ///< @brief �رտ����
	F2DAALEVEL_2    = 1,     ///< @brief 2x�����
	F2DAALEVEL_4    = 2,     ///< @brief 4x�����
	F2DAALEVEL_8    = 3,     ///< @brief 8x�����
	F2DAALEVEL_16   = 4      ///< @brief 16x�����
};

////////////////////////////////////////////////////////////////////////////////
/// @brief ��Ⱦ�豸�¼�������
/// @note  ���豸��ʧ��������ʱ�������¼�
////////////////////////////////////////////////////////////////////////////////
struct f2dRenderDeviceEventListener
{
	/// @brief �豸��ʧ�¼�
	virtual void OnRenderDeviceLost(){}

	/// @brief �豸�����¼�
	virtual void OnRenderDeviceReset(){}
};

////////////////////////////////////////////////////////////////////////////////
/// @brief ��Ⱦ�豸
/// @note  ������Ⱦ����Ĵ������豸����
////////////////////////////////////////////////////////////////////////////////
struct f2dRenderDevice
{
	// --- ��Ⱦ������ ---
	/// @brief   ����D3D�豸ָ��
	/// @warning �߼�����������������Ҫ��Ӧ������ʹ�ø÷���
	/// @return  ����IDirect3DDevice9�ӿ�ָ��
	virtual void* GetHandle()=0;
	
	/// @brief �����Կ�����
	virtual fcStr GetDeviceName()=0;

	/// @brief ���ػ��������
	virtual fuInt GetBufferWidth()=0;

	/// @brief ���ػ������߶�
	virtual fuInt GetBufferHeight()=0;

	/// @brief �Ƿ񴰿ڻ�
	virtual fBool IsWindowed()=0;

	/// @brief ��ÿ���ݵȼ�
	virtual F2DAALEVEL GetAALevel()=0;

	/// @brief ��鿹����Ƿ�֧��
	/// @param[in] AALevel  Ҫ���Ŀ���ݵȼ�
	/// @param[in] Windowed �Ƿ񴰿�ģʽ
	virtual fBool CheckMultiSample(F2DAALEVEL AALevel, fBool Windowed)=0;

	/// @brief ����֧�ֵ�ȫ���ֱ�������
	virtual fuInt GetSupportResolutionCount()=0;

	/// @brief ö��֧�ֵķֱ���
	virtual fcyVec2 EnumSupportResolution(fuInt Index)=0;

	/// @brief     ���軺����
	/// @note      �ò���������Ҫ�ֶ�����Graphics�ľ����Viewport��С
	/// @param[in] Width    ���������
	/// @param[in] Height   �������߶�
	/// @param[in] Windowed ���ڻ�
	/// @param[in] VSync    ��ֱͬ��
	/// @param[in] AALevel  ����ݵȼ�
	virtual fResult SetBufferSize(fuInt Width, fuInt Height, fBool Windowed, fBool VSync, F2DAALEVEL AALevel)=0;

	// --- �¼������� ---
	/// @brief     �ҽ�һ����Ϣ������
	/// @param[in] Listener Ҫ�󶨵ļ�����
	/// @param[in] Priority ���������ȼ���Խ���ֵԽ�����
	virtual fResult AttachListener(f2dRenderDeviceEventListener* Listener, fInt Priority=0)=0;
	
	/// @brief     �Ƴ�һ����Ϣ������
	/// @param[in] Listener Ҫ�Ƴ��ļ�����
	virtual fResult RemoveListener(f2dRenderDeviceEventListener* Listener)=0;

	// --- ��Դ���� ---
	/// @brief      ����������������
	/// @note       ����ѡ���Դ�����̬/��̬����
	/// @param[in]  pStream   ������������ȡ����������
	/// @param[in]  Width     ��ȣ���Ϊ0��ʹ����������ͼ��Ĭ�Ͽ��
	/// @param[in]  Height    �߶ȣ���Ϊ0��ʹ����������ͼ��Ĭ�ϸ߶�
	/// @param[in]  IsDynamic �Ƿ�Ϊ��̬����
	/// @param[in]  HasMipmap ����Mipmap�������ڼӿ�ͼ����Ⱦ���Զ�̬�������ȾĿ����Ч���Ƽ���Ϊtrue
	/// @param[out] pOut      ���������ָ��
	virtual fResult CreateTextureFromStream(f2dStream* pStream, fuInt Width, fuInt Height, fBool IsDynamic, fBool HasMipmap, f2dTexture2D** pOut)=0;
	
	/// @brief      ���ڴ����򴴽�����
	/// @note       ����ѡ���Դ�����̬/��̬����
	/// @param[in]  pMemory   �ڴ�����
	/// @param[in]  Size      �ڴ������С
	/// @param[in]  Width     ��ȣ���Ϊ0��ʹ����������ͼ��Ĭ�Ͽ��
	/// @param[in]  Height    �߶ȣ���Ϊ0��ʹ����������ͼ��Ĭ�ϸ߶�
	/// @param[in]  IsDynamic �Ƿ�Ϊ��̬����
	/// @param[in]  HasMipmap ����Mipmap�������ڼӿ�ͼ����Ⱦ���Զ�̬�������ȾĿ����Ч���Ƽ���Ϊtrue
	/// @param[out] pOut      ���������ָ��
	virtual fResult CreateTextureFromMemory(fcData pMemory, fLen Size, fuInt Width, fuInt Height, fBool IsDynamic, fBool HasMipmap, f2dTexture2D** pOut) = 0;

	/// @brief      ����һ����̬����
	/// @note       ��̬����֮�е����ݻ����豸��ʧ֮����ʧ��
	/// @param[in]  Width  ���
	/// @param[in]  Height �߶�
	/// @param[out] pOut   ���������ָ��
	virtual fResult CreateDynamicTexture(fuInt Width, fuInt Height, f2dTexture2D** pOut)=0;

	/// @brief      ����һ����ȾĿ��
	/// @note       ��ȾĿ�����ڴ����Ⱦ���ݣ���������
	/// @warning    ��ȾĿ���СӦ�����ģ�建������Сһ�£����򽫻���ɲ���Ԥ�ϵĽ����
	/// @param[in]  Width      ���
	/// @param[in]  Height     �߶�
	/// @param[in]  AutoResize ����ʱ�Զ���Ϊ��Ļ�ֱ���
	/// @param[out] pOut       ���������ָ��
	virtual fResult CreateRenderTarget(fuInt Width, fuInt Height, fBool AutoResize, f2dTexture2D** pOut)=0;

	/// @brief      ����һ�����ģ�建����
	/// @warning    ��ȾĿ���СӦ�����ģ�建������Сһ�£����򽫻���ɲ���Ԥ�ϵĽ����
	/// @param[in]  Width      ���
	/// @param[in]  Height     �߶�
	/// @param[in]  Discard    ����������
	/// @param[in]  AutoResize ����ʱ�Զ���Ϊ��Ļ�ֱ���
	/// @param[out] pOut       ����Ļ�����ָ��
	virtual fResult CreateDepthStencilSurface(fuInt Width, fuInt Height, fBool Discard, fBool AutoResize, f2dDepthStencilSurface** pOut)=0;

	/// @brief      ����һ��2D��Ⱦ��
	/// @param[in]  VertexBufferSize �ڲ����㻺������С����Ϊ0��Ĭ��
	/// @param[in]  IndexBufferSize  �ڲ�������������С����Ϊ0��Ĭ��
	/// @param[out] pOut             ���ָ��
	virtual fResult CreateGraphics2D(fuInt VertexBufferSize, fuInt IndexBufferSize, f2dGraphics2D** pOut)=0;

	/// @brief      ����һ��3D��Ⱦ��������fx�ģ�
	/// @param[in]  pDefaultEffect Ĭ��Ч�����ɿ�
	/// @param[out] pOut           �������Ⱦ��ָ��
	virtual fResult CreateGraphics3D(f2dEffect* pDefaultEffect, f2dGraphics3D** pOut)=0;

	/// @brief      ����һ��Ч��
	/// @param[in]  pStream    Ч������(fx�ļ�)������ȡ������
	/// @param[in]  bAutoState �Ƿ��Զ�����״̬������Graphics3D����ʱ�����Զ��������й�������Ⱦ������
	/// @param[out] pOut       �����Ч��
	/// @param[out] pErrOut    ������Ϣ���ɿգ���������ʧ��ʱ����
	virtual fResult CreateEffect(f2dStream* pStream, fBool bAutoState, f2dEffect** pOut)=0;

	/// @brief      ����һ����������
	/// @param[in]  pVertElement ����Ԫ������
	/// @param[in]  ElementCount ����Ԫ�ظ���
	/// @param[in]  VertCount    ��������
	/// @param[in]  IndexCount   ��������
	/// @param[in]  Int32Index   �Ƿ�ʹ��Int��Ϊ������λ������ʹ��Short
	/// @param[out] pOut         �����ģ�����ݶ���
	virtual fResult CreateMeshData(f2dVertexElement* pVertElement, fuInt ElementCount, fuInt VertCount, fuInt IndexCount, fBool Int32Index, f2dMeshData** pOut)=0;

	// --- ��ͼ״̬ ---
	/// @brief     ���Z�������ȾĿ��
	/// @param[in] BackBufferColor ������ɫ
	/// @param[in] ZValue          Z���ֵ�����������Ӧȡ1.0f
	virtual fResult Clear(const fcyColor& BackBufferColor = 0, fFloat ZValue = 1.0f)=0;

	/// @brief     ���Z���塢��ȾĿ�ꡢģ�建����
	/// @param[in] BackBufferColor ������ɫ
	/// @param[in] ZValue          Z���ֵ�����������Ӧȡ1.0f
	/// @param[in] StencilValue    ģ�建����ֵ
	virtual fResult Clear(const fcyColor& BackBufferColor, fFloat ZValue, fuInt StencilValue)=0;

	/// @brief     �����ȾĿ��
	/// @param[in] BackBufferColor ������ɫ
	virtual fResult ClearColor(const fcyColor& BackBufferColor = 0)=0;

	/// @brief     ���Z����
	/// @param[in] Value Z���ֵ�����������Ӧȡ1.0f
	virtual fResult ClearZBuffer(fFloat Value=1.0f)=0;

	/// @brief     ���ģ�建��
	/// @param[in] StencilValue ģ�建��ֵ
	virtual fResult ClearStencilBuffer(fuInt StencilValue=0)=0;

	/// @brief   ����Ŀǰʹ���е���ȾĿ��
	/// @warning ��ȾĿ�����һ����Ⱦ������ָ���Ĭ�Ϻ�̨������
	/// @note    ���ΪĬ�Ϻ�̨������������NULL���ú������������ü���
	virtual f2dTexture2D* GetRenderTarget()=0;

	/// @brief ������ȾĿ��
	/// @note  ��������ΪĬ�Ϻ�̨������������ΪNULL
	virtual fResult SetRenderTarget(f2dTexture2D* pTex)=0;

	/// @brief   ����Ŀǰʹ���е����ģ�建����
	/// @warning ���ģ�建��������һ����Ⱦ������ָ���Ĭ�Ϻ�̨������
	/// @note    ��ΪĬ�Ϻ�̨����������NULL���ú������������ü���
	virtual f2dDepthStencilSurface* GetDepthStencilSurface()=0;

	/// @brief �������ģ�建����
	/// @note  ��������ΪĬ��ģ�建����������ΪNULL
	virtual fResult SetDepthStencilSurface(f2dDepthStencilSurface* pSurface)=0;

	/// @brief ����Ŀǰ�Ĳü�����
	virtual fcyRect GetScissorRect()=0;

	/// @brief   ���òü�����
	/// @warning ����Ӧ��С��RenderTarget��С
	/// @note    �������û�������Сʱ��ֵ�ᱻ����
	virtual fResult SetScissorRect(const fcyRect& pRect)=0;

	/// @brief ����Ŀǰ��Viewport
	virtual fcyRect GetViewport()=0;

	/// @brief     ����Ŀǰ��Viewport
	/// @param[in] vp ����������
	virtual fResult SetViewport(fcyRect vp)=0;

	/// @brief �����Ƿ���Z-buffer����
	virtual fBool IsZBufferEnabled()=0;

	/// @brief �����Ƿ���Z-buffer����
	virtual fResult SetZBufferEnable(fBool v)=0;

	// --- �߼� ---
	/// @brief     ����
	/// @note      ��PNG��ʽ����
	/// @param[in] pStream ��������������ĵ�ǰλ�ÿ�ʼд��
	virtual fResult SaveScreen(f2dStream* pStream)=0;

	/// @brief     ��������
	/// @note      ��PNG��ʽ����
	/// @param[in] pStream ��������������ĵ�ǰλ�ÿ�ʼд��
	/// @param[in] pTex    Ҫ���������
	virtual fResult SaveTexture(f2dStream* pStream, f2dTexture2D* pTex)=0;

	/// @brief     ����UpdateLayeredWindow���·ֲ㴰��
	/// @note      Ҫ�������ڱ�����Ϊ�ֲ㴰��
	/// @param[in] KeyColor �ؼ�ɫ
	/// @param[in] Alpha    ͸����
	virtual fResult UpdateScreenToWindow(fcyColor KeyColor, fByte Alpha)=0;
};

/// @}
