////////////////////////////////////////////////////////////////////////////////
/// @file  f2dRenderDevice.h
/// @brief fancy2D渲染设备接口定义
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "f2dInterface.h"

/// @addtogroup f2d渲染系统
/// @brief fancy2d引擎中的渲染系统，提供对2D图形的渲染支持。
/// @{

// =============================== 纹理包装 ===================================

// 纹理格式：

////////////////////////////////////////////////////////////////////////////////
/// @brief 纹理页面格式
////////////////////////////////////////////////////////////////////////////////
enum F2DSURFACEFMT
{
	F2DSURFACEFMT_UNKNOWN,  ///< @brief 未知格式
	F2DSURFACEFMT_A8R8G8B8  ///< @brief ARGB格式
};

// 纹理接口：

////////////////////////////////////////////////////////////////////////////////
/// @brief 纹理接口
////////////////////////////////////////////////////////////////////////////////
struct f2dTexture :
	public f2dInterface
{
	/// @brief   获得内部纹理对象
	/// @warning 高级方法，若无特殊需要请勿使用
	/// @return  返回IDirect3DTexutreBase9接口
	virtual void* GetHandle()=0;

	virtual fuInt GetDimension()=0;   ///< @brief 返回纹理维数, 用以区分Tex1D,Tex2D,Tex3D
	virtual fBool IsDynamic()=0;      ///< @brief 是否可修改
	virtual fBool IsRenderTarget()=0; ///< @brief 是否是渲染目标
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 二维纹理接口
////////////////////////////////////////////////////////////////////////////////
struct f2dTexture2D :
	public f2dTexture
{
	virtual fuInt GetWidth()=0;    ///< @brief 返回宽度
	virtual fuInt GetHeight()=0;   ///< @brief 返回高度

	/// @brief      锁定纹理
	/// @note       只有动态纹理可以被锁定。锁定操作和解锁操作配对，在未解锁前只能锁定一次。
	/// @param[in]  pLockRect 被锁定的区域，设为NULL则锁定整个纹理
	/// @param[in]  Discard   如果原始数据正在被使用则抛弃原始数据
	/// @param[out] Pitch     行对齐字节
	/// @param[out] pOut      返回的内存指针，自动计算到区域的首字节
	virtual fResult Lock(fcyRect* pLockRect, fBool Discard, fuInt* Pitch, fData* pOut)=0;

	/// @brief 解锁纹理
	virtual fResult Unlock()=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 深度模板缓冲区
////////////////////////////////////////////////////////////////////////////////
struct f2dDepthStencilSurface :
	public f2dInterface
{
	virtual void* GetHandle()=0;   ///< @brief 返回IDirect3DSurface对象，高级方法
	virtual fuInt GetWidth()=0;    ///< @brief 返回宽度
	virtual fuInt GetHeight()=0;   ///< @brief 返回高度
};

// =============================== 效果包装 ===================================

// 效果参数：

////////////////////////////////////////////////////////////////////////////////
/// @brief 效果参数类型
////////////////////////////////////////////////////////////////////////////////
enum F2DEPTYPE
{
	F2DEPTYPE_UNKNOWN,  ///< @brief 未知参数类型
	F2DEPTYPE_VALUE,    ///< @brief 值类型
	F2DEPTYPE_OBJECT,   ///< @brief 内置对象
	F2DEPTYPE_ARRAY,    ///< @brief 数组
	F2DEPTYPE_STRUCT    ///< @brief 结构体
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 效果参数值类型
////////////////////////////////////////////////////////////////////////////////
enum F2DEPVTYPE
{
	F2DEPVTYPE_UNKNOWN, ///< @brief 未知类型
	F2DEPVTYPE_VOID,    ///< @brief 无类型
	F2DEPVTYPE_BOOL,    ///< @brief Bool型
	F2DEPVTYPE_FLOAT,   ///< @brief Float型
	F2DEPVTYPE_INT      ///< @brief Int型
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 效果参数内置类型
////////////////////////////////////////////////////////////////////////////////
enum F2DEPOTYPE
{
	F2DEPOTYPE_UNKNOWN,   ///< @brief 未知类型
	F2DEPOTYPE_STRING,    ///< @brief 字符串
	F2DEPOTYPE_TEXTURE,   ///< @brief 纹理
	F2DEPOTYPE_VECTOR     ///< @brief 向量
};

// 效果参数对象：

////////////////////////////////////////////////////////////////////////////////
/// @brief 效果参数
/// @note  注意一旦Effect对象被释放，该接口将无效。
////////////////////////////////////////////////////////////////////////////////
struct f2dEffectParam
{
	virtual fcStr GetName()=0;                 ///< @brief 返回参数名称
	virtual F2DEPTYPE GetType()=0;             ///< @brief 返回参数类型

	virtual fcStr GetSemantic()=0;                        ///< @brief 获得参数语义
	virtual f2dEffectParam* GetAnnotation(fcStr Name)=0;  ///< @brief 返回注释对象
	virtual f2dEffectParam* GetAnnotation(fuInt Index)=0; ///< @brief 返回注释对象
	virtual fuInt GetAnnotationCount()=0;                 ///< @brief 返回注释对象个数
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 值类型参数
/// @note  注意一旦Effect对象被释放，该接口将无效。
////////////////////////////////////////////////////////////////////////////////
struct f2dEffectParamValue :
	public f2dEffectParam
{
	virtual F2DEPVTYPE GetValueType()=0;      ///< @brief 获得值类型

	virtual fBool GetBool()=0;                ///< @brief 返回Bool型
	virtual fFloat GetFloat()=0;              ///< @brief 返回Float型
	virtual fInt GetInt()=0;                  ///< @brief 返回Int型

	virtual fResult SetBool(fBool Value)=0;   ///< @brief 设置Bool型
	virtual fResult SetFloat(fFloat Value)=0; ///< @brief 设置Float型
	virtual fResult SetInt(fInt Value)=0;     ///< @brief 设置Int型
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 内置类型参数
/// @note  注意一旦Effect对象被释放，该接口将无效。
////////////////////////////////////////////////////////////////////////////////
struct f2dEffectParamObj :
	public f2dEffectParam
{
	virtual F2DEPOTYPE GetObjType()=0;               ///< @brief 获得对象类型

	virtual fcStr GetString()=0;                     ///< @brief 返回字符串数据
	virtual fResult GetTexture(f2dTexture** pOut)=0; ///< @brief 返回纹理对象
	virtual fcyVec4 GetVector()=0;                   ///< @brief 返回向量对象

	virtual fResult SetString(fcStr Str)=0;             ///< @brief 设置字符串数据
	virtual fResult SetTexture(f2dTexture* pTex)=0;     ///< @brief 设置纹理对象
	virtual fResult SetVector(const fcyVec4& Vec)=0;    ///< @brief 设置向量对象
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 数组类型参数
/// @note  注意一旦Effect对象被释放，该接口将无效。
////////////////////////////////////////////////////////////////////////////////
struct f2dEffectParamArr :
	public f2dEffectParam
{
	virtual fBool IsRowFirst()=0;          ///< @brief 是否行优先储存
	virtual fuInt GetRowColumnCount()=0;   ///< @brief 行数或者列数

	/// @brief 通过元素Index获取元素
	virtual f2dEffectParam* GetElement(fuInt Index)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 结构类型参数
/// @note  注意一旦Effect对象被释放，该接口将无效。
////////////////////////////////////////////////////////////////////////////////
struct f2dEffectParamStruct :
	public f2dEffectParam
{
	virtual fuInt GetElementCount()=0;                 ///< @brief 获得成员数目
	virtual f2dEffectParam* GetElement(fcStr Name)=0;  ///< @brief 通过成员名获得成员
	virtual f2dEffectParam* GetElement(fuInt Index)=0; ///< @brief 通过成员下标获得成员
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 效果入口函数
/// @note  注意一旦Effect对象被释放，该接口将无效。
////////////////////////////////////////////////////////////////////////////////
struct f2dEffectFunction
{
	virtual fcStr GetName()=0;                 ///< @brief 返回入口名称
	virtual f2dEffectParam* GetAnnotation()=0; ///< @brief 返回注释对象
	virtual fuInt GetAnnotationCount()=0;      ///< @brief 返回注释对象个数
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 效果批次
/// @note  注意一旦Effect对象被释放，该接口将无效。
////////////////////////////////////////////////////////////////////////////////
struct f2dEffectPass
{
	virtual fcStr GetName()=0;                 ///< @brief 返回入口名称
	virtual f2dEffectParam* GetAnnotation()=0; ///< @brief 返回注释对象
	virtual fuInt GetAnnotationCount()=0;      ///< @brief 返回注释对象个数
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 效果技术
////////////////////////////////////////////////////////////////////////////////
struct f2dEffectTechnique
{
	virtual fcStr GetName()=0;                 ///< @brief 返回入口名称
	virtual f2dEffectParam* GetAnnotation()=0; ///< @brief 返回注释对象
	virtual fuInt GetAnnotationCount()=0;      ///< @brief 返回注释对象个数

	virtual fBool IsValidate()=0;                 ///< @brief 检查设备是否支持该效果
	virtual fuInt GetPassCount()=0;               ///< @brief 返回效果批次个数
	virtual f2dEffectPass* GetPass(fuInt Index)=0;///< @brief 获得渲染批次
	virtual f2dEffectPass* GetPass(fcStr Name)=0; ///< @brief 获得渲染批次
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 效果对象
////////////////////////////////////////////////////////////////////////////////
struct f2dEffect :
	public f2dInterface
{
	/// @brief   获得ID3DXEffect对象
	/// @warning 高级方法
	virtual void* GetHandle()=0;

	virtual fuInt GetParamCount()=0;                  ///< @brief 获得参数个数
	virtual f2dEffectParam* GetParam(fuInt Index)=0;  ///< @brief 获得参数
	virtual f2dEffectParam* GetParam(fcStr Name)=0;   ///< @brief 获得参数

	virtual fuInt GetTechniqueCount()=0;                     ///< @brief 获得技术个数
	virtual f2dEffectTechnique* GetTechnique(fuInt Index)=0; ///< @brief 获得技术
	virtual f2dEffectTechnique* GetTechnique(fcStr Name)=0;  ///< @brief 获得技术

	virtual fuInt GetFunctionCount()=0;                     ///< @brief 获得函数个数
	virtual f2dEffectFunction* GetFunction(fuInt Index)=0;  ///< @brief 获得函数
	virtual f2dEffectFunction* GetFunction(fcStr Name)=0;   ///< @brief 获得函数
	
	virtual f2dEffectTechnique* GetCurrentTechnique()=0;              ///< @brief 获得当前渲染技术
	virtual fResult SetCurrentTechnique(f2dEffectTechnique* pTech)=0; ///< @brief 设置当前渲染技术
};

// ============================= 渲染器包装 ===================================

// 混合选项：

////////////////////////////////////////////////////////////////////////////////
/// @brief 混合运算符
////////////////////////////////////////////////////////////////////////////////
enum F2DBLENDOPERATOR
{
	F2DBLENDOPERATOR_ADD         = 1, ///< @brief 混合结果 = 源像素 + 目标像素
	F2DBLENDOPERATOR_SUBTRACT    = 2, ///< @brief 混合结果 = 源像素 - 目标像素
	F2DBLENDOPERATOR_REVSUBTRACT = 3, ///< @brief 混合结果 = 目标像素 - 源像素
	F2DBLENDOPERATOR_MIN         = 4, ///< @brief 混合结果 = MIN(源像素, 目标像素)
	F2DBLENDOPERATOR_MAX         = 5  ///< @brief 混合结果 = MAX(源像素, 目标像素)
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 混合因素
/// @note  颜色混合公式：Color = SrcColor * SrcBlend + DestColor * DestBlend
////////////////////////////////////////////////////////////////////////////////
enum F2DBLENDFACTOR
{
    F2DBLENDFACTOR_ZERO               = 1,   ///< @brief 混合因素(0, 0, 0, 0)
    F2DBLENDFACTOR_ONE                = 2,   ///< @brief 混合因素(1, 1, 1, 1)
    F2DBLENDFACTOR_SRCCOLOR           = 3,   ///< @brief 混合因素(Rs, Gs, Bs, As)
    F2DBLENDFACTOR_INVSRCCOLOR        = 4,   ///< @brief 混合因素(1 - Rs, 1 - Gs, 1 - Bs, 1 - As)
    F2DBLENDFACTOR_SRCALPHA           = 5,   ///< @brief 混合因素(As, As, As, As)
    F2DBLENDFACTOR_INVSRCALPHA        = 6,   ///< @brief 混合因素(1 - As, 1 - As, 1 - As, 1 - As)
    F2DBLENDFACTOR_DESTALPHA          = 7,   ///< @brief 混合因素(Ad, Ad, Ad, Ad)
    F2DBLENDFACTOR_INVDESTALPHA       = 8,   ///< @brief 混合因素(1 - Ad, 1 - Ad, 1 - Ad, 1 - Ad)
    F2DBLENDFACTOR_DESTCOLOR          = 9,   ///< @brief 混合因素(Rd, Gd, Bd, Ad)
    F2DBLENDFACTOR_INVDESTCOLOR       = 10,  ///< @brief 混合因素(1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad)
    F2DBLENDFACTOR_SRCALPHASAT        = 11   ///< @brief 混合因素(f, f, f, 1)，其中f = min(As, 1 - Ad)
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 混合状态
////////////////////////////////////////////////////////////////////////////////
struct f2dBlendState
{
	F2DBLENDFACTOR SrcBlend;        ///< @brief 源混合参数
	F2DBLENDFACTOR DestBlend;       ///< @brief 目的混合参数
	F2DBLENDOPERATOR BlendOp;       ///< @brief 混合运算符
	F2DBLENDFACTOR AlphaSrcBlend;   ///< @brief Alpha源混合参数，不可使用RGBA选项
	F2DBLENDFACTOR AlphaDestBlend;  ///< @brief Alpha目的混合参数，不可使用RGBA选项
	F2DBLENDOPERATOR AlphaBlendOp;  ///< @brief Alpha混合运算符
};

// 渲染器：

////////////////////////////////////////////////////////////////////////////////
/// @brief 渲染器接口
////////////////////////////////////////////////////////////////////////////////
struct f2dGraphics :
	public f2dInterface
{
	virtual fBool IsGraphics3D()=0;  ///< @brief 是否为3D渲染器，否则为2D渲染器

	// === 渲染过程控制 ===
	virtual fBool IsInRender()=0;    ///< @brief 检查是否处于Begin/End区段
	virtual fResult Begin()=0;       ///< @brief 启动渲染器并完成状态同步
	virtual fResult Flush()=0;       ///< @brief 立即递交渲染队列
	virtual fResult End()=0;         ///< @brief 结束渲染过程并递交渲染队列

	// === 渲染状态控制 ===
	virtual const fcyMatrix4& GetWorldTransform()=0;  ///< @brief 返回世界变换矩阵
	virtual const fcyMatrix4& GetLookatTransform()=0; ///< @brief 返回观察矩阵
	virtual const fcyMatrix4& GetProjTransform()=0;   ///< @brief 返回投影矩阵

	virtual void SetWorldTransform(const fcyMatrix4& Mat)=0;  ///< @brief 设置世界变换矩阵
	virtual void SetLookatTransform(const fcyMatrix4& Mat)=0; ///< @brief 设置观察矩阵
	virtual void SetProjTransform(const fcyMatrix4& Mat)=0;   ///< @brief 设置投影矩阵

	// === 混合状态控制 ===
	virtual const f2dBlendState& GetBlendState()=0;           ///< @brief 返回像素混合参数
	virtual void SetBlendState(const f2dBlendState& State)=0; ///< @brief 设置像素混合参数
};

// 2D渲染器：

////////////////////////////////////////////////////////////////////////////////
/// @brief 二维渲染器顶点
////////////////////////////////////////////////////////////////////////////////
struct f2dGraphics2DVertex
{
	fFloat x;      ///< @brief x坐标
	fFloat y;      ///< @brief y坐标
	fFloat z;      ///< @brief z坐标
	fuInt color;   ///< @brief 漫反射颜色
	fFloat u;      ///< @brief 纹理u坐标
	fFloat v;      ///< @brief 纹理v坐标
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 2D渲染器接口
////////////////////////////////////////////////////////////////////////////////
struct f2dGraphics2D :
	public f2dGraphics
{
	// === 2D绘制函数 ===
	/// @brief 绘制矩形
	virtual fResult DrawQuad(
		f2dTexture2D* pTex,
		const f2dGraphics2DVertex& v1, 
		const f2dGraphics2DVertex& v2, 
		const f2dGraphics2DVertex& v3, 
		const f2dGraphics2DVertex& v4,
		fBool bAutoFixCoord = true
		)=0;

	/// @brief 绘制矩形
	virtual fResult DrawQuad(f2dTexture2D* pTex, const f2dGraphics2DVertex* arr, fBool bAutoFixCoord = true)=0;

	/// @brief 原始绘制函数
	virtual fResult DrawRaw(f2dTexture2D* pTex, fuInt VertCount, fuInt IndexCount, const f2dGraphics2DVertex* VertArr, const fuShort* IndexArr, fBool bAutoFixCoord = true)=0;
};

// 3D渲染器：

////////////////////////////////////////////////////////////////////////////////
/// @brief 3D渲染器接口
////////////////////////////////////////////////////////////////////////////////
struct f2dGraphics3D :
	public f2dGraphics
{
	// === FX状态 ===
	/// @brief 返回当前渲染程序
	virtual f2dEffect* GetEffect()=0;

	/// @brief 设置当前渲染程序
	virtual fResult SetEffect(f2dEffect* Effect)=0;
};

// ============================= 渲染设备 ===================================
////////////////////////////////////////////////////////////////////////////////
/// @brief 渲染设备抗锯齿等级
////////////////////////////////////////////////////////////////////////////////
enum F2DAALEVEL
{
	F2DAALEVEL_NONE = 0,     ///< @brief 关闭抗锯齿
	F2DAALEVEL_2    = 1,     ///< @brief 2x抗锯齿
	F2DAALEVEL_4    = 2,     ///< @brief 4x抗锯齿
	F2DAALEVEL_8    = 3,     ///< @brief 8x抗锯齿
	F2DAALEVEL_16   = 4      ///< @brief 16x抗锯齿
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 渲染设备事件监听器
/// @note  当设备丢失或者重置时引发该事件
////////////////////////////////////////////////////////////////////////////////
struct f2dRenderDeviceEventListener
{
	/// @brief 设备丢失事件
	virtual void OnRenderDeviceLost(){}

	/// @brief 设备重置事件
	virtual void OnRenderDeviceReset(){}
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 渲染设备
/// @note  负责渲染对象的创建和设备管理。
////////////////////////////////////////////////////////////////////////////////
struct f2dRenderDevice
{
	// --- 渲染器属性 ---
	/// @brief   返回D3D设备指针
	/// @warning 高级方法。如无特殊需要，应当避免使用该方法
	/// @return  返回IDirect3DDevice9接口指针
	virtual void* GetHandle()=0;
	
	/// @brief 返回显卡名称
	virtual fcStr GetDeviceName()=0;

	/// @brief 返回缓冲区宽度
	virtual fuInt GetBufferWidth()=0;

	/// @brief 返回缓冲区高度
	virtual fuInt GetBufferHeight()=0;

	/// @brief 是否窗口化
	virtual fBool IsWindowed()=0;

	/// @brief 获得抗锯齿等级
	virtual F2DAALEVEL GetAALevel()=0;

	/// @brief 检查抗锯齿是否被支持
	/// @param[in] AALevel  要检测的抗锯齿等级
	/// @param[in] Windowed 是否窗口模式
	virtual fBool CheckMultiSample(F2DAALEVEL AALevel, fBool Windowed)=0;

	/// @brief 返回支持的全屏分辨率数量
	virtual fuInt GetSupportResolutionCount()=0;

	/// @brief 枚举支持的分辨率
	virtual fcyVec2 EnumSupportResolution(fuInt Index)=0;

	/// @brief     重设缓冲区
	/// @param[in] Width    缓冲区宽度
	/// @param[in] Height   缓冲区高度
	/// @param[in] Windowed 窗口化
	/// @param[in] VSync    垂直同步
	/// @param[in] AALevel  抗锯齿等级
	virtual fResult SetBufferSize(fuInt Width, fuInt Height, fBool Windowed, fBool VSync, F2DAALEVEL AALevel)=0;

	// --- 事件监听器 ---
	/// @brief     挂接一个消息监听器
	/// @param[in] Listener 要绑定的监听器
	virtual fResult AttachListener(f2dRenderDeviceEventListener* Listener)=0;
	
	/// @brief     移除一个消息监听器
	/// @param[in] Listener 要移除的监听器
	virtual fResult RemoveListener(f2dRenderDeviceEventListener* Listener)=0;

	// --- 资源创建 ---
	/// @brief      从数据流创建纹理
	/// @note       可以选择性创建动态/静态纹理
	/// @param[in]  pStream   数据流，将读取整个数据流
	/// @param[in]  Width     宽度，设为0将使用数据流中图像默认宽度
	/// @param[in]  Height    高度，设为0将使用数据流中图像默认高度
	/// @param[in]  IsDynamic 是否为动态纹理
	/// @param[in]  HasMipmap 创建Mipmap链，用于加快图像渲染，对动态纹理和渲染目标无效。推荐设为true
	/// @param[out] pOut      输出的纹理指针
	virtual fResult CreateTextureFromStream(f2dStream* pStream, fuInt Width, fuInt Height, fBool IsDynamic, fBool HasMipmap, f2dTexture2D** pOut)=0;
	
	/// @brief      创建一个动态纹理
	/// @note       动态纹理之中的数据会在设备丢失之后消失。
	/// @param[in]  Width  宽度
	/// @param[in]  Height 高度
	/// @param[out] pOut   输出的纹理指针
	virtual fResult CreateDynamicTexture(fuInt Width, fuInt Height, f2dTexture2D** pOut)=0;

	/// @brief      创建一个渲染目标
	/// @note       渲染目标用于存放渲染数据，不可锁定
	/// @warning    渲染目标大小应和深度模板缓冲区大小一致，否则将会造成不可预料的结果。
	/// @param[in]  Width      宽度
	/// @param[in]  Height     高度
	/// @param[in]  AutoResize 重置时自动设为屏幕分辨率
	/// @param[out] pOut       输出的纹理指针
	virtual fResult CreateRenderTarget(fuInt Width, fuInt Height, fBool AutoResize, f2dTexture2D** pOut)=0;

	/// @brief      创建一个深度模板缓冲区
	/// @warning    渲染目标大小应和深度模板缓冲区大小一致，否则将会造成不可预料的结果。
	/// @param[in]  Width      宽度
	/// @param[in]  Height     高度
	/// @param[in]  Discard    不保留数据
	/// @param[in]  AutoResize 重置时自动设为屏幕分辨率
	/// @param[out] pOut       输出的缓冲区指针
	virtual fResult CreateDepthStencilSurface(fuInt Width, fuInt Height, fBool Discard, fBool AutoResize, f2dDepthStencilSurface** pOut)=0;

	/// @brief      创建一个2D渲染器
	/// @param[in]  VertexBufferSize 内部顶点缓冲区大小，置为0则默认
	/// @param[in]  IndexBufferSize  内部索引缓冲区大小，置为0则默认
	/// @param[out] pOut             输出指针
	virtual fResult CreateGraphics2D(fuInt VertexBufferSize, fuInt IndexBufferSize, f2dGraphics2D** pOut)=0;

	/// @brief      创建一个3D渲染器（基于fx的）
	/// @param[in]  pDefaultEffect 默认效果
	/// @param[out] pOut           输出的渲染器指针
	virtual fResult CreateGraphics3D(f2dEffect* pDefaultEffect, f2dGraphics3D** pOut)=0;

	/// @brief      创建一个效果
	/// @param[in]  pStream 效果代码(fx文件)，将读取整个流
	/// @param[out] pOut    输出的效果
	/// @param[out] pErrOut 错误信息，可空，仅当创建失败时设置
	virtual fResult CreateEffect(f2dStream* pStream, f2dEffect** pOut, f2dStream** pErrOut = NULL)=0;

	// --- 绘图状态 ---
	/// @brief     清空Z缓冲和渲染目标
	/// @param[in] BackBufferColor 背景颜色
	/// @param[in] ZValue          Z深度值，无特殊情况应取1.0f
	virtual fResult Clear(const fcyColor& BackBufferColor = 0, fFloat ZValue = 1.0f)=0;

	/// @brief     清空Z缓冲、渲染目标、模板缓冲区
	/// @param[in] BackBufferColor 背景颜色
	/// @param[in] ZValue          Z深度值，无特殊情况应取1.0f
	/// @param[in] StencilValue    模板缓冲区值
	virtual fResult Clear(const fcyColor& BackBufferColor, fFloat ZValue, fuInt StencilValue)=0;

	/// @brief     清空渲染目标
	/// @param[in] BackBufferColor 背景颜色
	virtual fResult ClearColor(const fcyColor& BackBufferColor = 0)=0;

	/// @brief     清空Z缓冲
	/// @param[in] Value Z深度值，无特殊情况应取1.0f
	virtual fResult ClearZBuffer(fFloat Value=1.0f)=0;

	/// @brief     清空模板缓冲
	/// @param[in] StencilValue 模板缓冲值
	virtual fResult ClearStencilBuffer(fuInt StencilValue=0)=0;

	/// @brief   返回目前使用中的渲染目标
	/// @warning 渲染目标会在一轮渲染结束后恢复到默认后台缓冲区
	/// @note    如果为默认后台缓冲区将返回NULL；该函数不增加引用计数
	virtual f2dTexture2D* GetRenderTarget()=0;

	/// @brief 设置渲染目标
	/// @note  若需设置为默认后台缓冲区，请置为NULL
	virtual fResult SetRenderTarget(f2dTexture2D* pTex)=0;

	/// @brief   返回目前使用中的深度模板缓冲区
	/// @warning 深度模板缓冲区会在一轮渲染结束后恢复到默认后台缓冲区
	/// @note    若为默认后台缓冲区返回NULL；该函数不增加引用计数
	virtual f2dDepthStencilSurface* GetDepthStencilSurface()=0;

	/// @brief 设置深度模板缓冲区
	/// @note  若需设置为默认模板缓冲区，请置为NULL
	virtual fResult SetDepthStencilSurface(f2dDepthStencilSurface* pSurface)=0;

	/// @brief 返回目前的裁剪矩形
	virtual fcyRect GetScissorRect()=0;

	/// @brief   设置裁剪矩形
	/// @warning 矩形应当小于RenderTarget大小
	/// @note    重新设置缓冲区大小时该值会被覆盖
	virtual fResult SetScissorRect(const fcyRect& pRect)=0;

	// --- 高级 ---
	/// @brief     截屏
	/// @note      以PNG形式保存
	/// @param[in] pStream 输出的流，从流的当前位置开始写入
	virtual fResult SaveScreen(f2dStream* pStream)=0;

	/// @brief     保存纹理
	/// @note      以PNG形式保存
	/// @param[in] pStream 输出的流，从流的当前位置开始写入
	/// @param[in] pTex    要保存的纹理
	virtual fResult SaveTexture(f2dStream* pStream, f2dTexture2D* pTex)=0;

	/// @brief     调用UpdateLayeredWindow更新分层窗口
	/// @note      要求主窗口被设置为分层窗口
	/// @param[in] KeyColor 关键色
	/// @param[in] Alpha    透明度
	virtual fResult UpdateScreenToWindow(fcyColor KeyColor, fByte Alpha)=0;
};

/// @}
