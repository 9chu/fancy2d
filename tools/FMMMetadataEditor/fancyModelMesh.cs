using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.ComponentModel;

namespace fancyModelMesh
{
    /// <summary>
    /// 序列化接口
    /// </summary>
    public interface Serializable
    {
        /// <summary>
        /// 创建空的对象
        /// </summary>
        void New();
        /// <summary>
        /// 从流加载数据
        /// </summary>
        /// <remarks>若加载失败则调用New</remarks>
        /// <param name="Reader">读入的对象</param>
        void Load(BinaryReader Reader);
        /// <summary>
        /// 保存数据到流
        /// </summary>
        /// <param name="Writer">写出的对象</param>
        void Save(BinaryWriter Writer);
    }

    /// <summary>
    /// 字符串数据
    /// </summary>
    public class StringData : Serializable
    {
        private string _Content = String.Empty;

        /// <summary>
        /// 获取或设置字符串内容
        /// </summary>
        public string Content
        {
            get
            {
                return _Content;
            }
            set
            {
                _Content = value;
            }
        }

        public int GetSaveSize()
        {
            byte[] tBuf = Encoding.UTF8.GetBytes(_Content);
            return tBuf.Length + 4;
        }

        public void New()
        {
            _Content = String.Empty;
        }

        public void Load(BinaryReader Reader)
        {
            try
            {
                UInt32 tLen = Reader.ReadUInt32();
                byte[] tBuf = Reader.ReadBytes((int)tLen);
                _Content = Encoding.UTF8.GetString(tBuf);
            }
            catch
            {
                New();

                throw;
            }
        }

        public void Save(BinaryWriter Writer)
        {
            if (_Content == null)
                Writer.Write((UInt32)0);
            else
            {
                byte[] tBuf = Encoding.UTF8.GetBytes(_Content);
                Writer.Write((UInt32)tBuf.Length);
                Writer.Write(tBuf);
            }
        }

        public StringData Clone()
        {
            StringData tRet = new StringData();
            tRet._Content = (string)_Content.Clone();
            return tRet;
        }
    }

    /// <summary>
    /// 数据标签
    /// </summary>
    /// <remarks>数据标签长度固定且只能为ASCII字符</remarks>
    public class Label : Serializable
    {
        private int _FixedLength = 0;
        private string _Content = String.Empty;

        /// <summary>
        /// 获取标签的长度
        /// </summary>
        public int FixedLength
        {
            get
            {
                return _FixedLength;
            }
        }

        /// <summary>
        /// 获取或设置内容
        /// </summary>
        /// <remarks>对于超出长度的部分将会在保存时截断</remarks>
        public string Content
        {
            get
            {
                return _Content;
            }
            set
            {
                _Content = value;
            }
        }

        public void New()
        {
            _Content = String.Empty;
        }

        public void Load(BinaryReader Reader)
        {
            try
            {
                byte[] tBuf = new byte[_FixedLength + 1];
                for (int i = 0; i < _FixedLength; ++i)
                {
                    tBuf[i] = Reader.ReadByte();
                }
                tBuf[_FixedLength] = 0;

                _Content = Encoding.ASCII.GetString(tBuf);

                // 去掉末尾多余的'\0'
                int tPos = _Content.IndexOf('\0');
                if (tPos >= 0)
                {
                    _Content = _Content.Substring(0, tPos);
                }
            }
            catch
            {
                New();

                throw;
            }
        }

        public void Save(BinaryWriter Writer)
        {
            byte[] tBuf = new byte[_FixedLength];
            byte[] tContentBuf = null;
            if (_Content != null)
                tContentBuf = Encoding.ASCII.GetBytes(_Content);
            for (int i = 0; i < _FixedLength; ++i)
            {
                if (tContentBuf != null && i < tContentBuf.Length)
                {
                    tBuf[i] = tContentBuf[i];
                }
            }
            Writer.Write(tBuf);
        }

        public Label(int FixedLength)
        {
            _FixedLength = FixedLength;
        }
    }

    /// <summary>
    /// 二维向量
    /// </summary>
    public class Vector2 : Serializable
    {
        public float X { get; set; }
        public float Y { get; set; }

        public void New()
        {
            X = Y = 0;
        }

        public void Load(BinaryReader Reader)
        {
            try
            {
                X = Reader.ReadSingle();
                Y = Reader.ReadSingle();
            }
            catch
            {
                New();

                throw;
            }
        }

        public void Save(BinaryWriter Writer)
        {
            Writer.Write(X);
            Writer.Write(Y);
        }

        public Vector2()
        {
            X = 0; Y = 0;
        }

        public Vector2(float _X, float _Y)
        {
            X = _X; Y = _Y;
        }
    }

    /// <summary>
    /// 三维向量
    /// </summary>
    public class Vector3 : Serializable
    {
        public float X { get; set; }
        public float Y { get; set; }
        public float Z { get; set; }

        public void New()
        {
            X = Y = Z = 0;
        }

        public void Load(BinaryReader Reader)
        {
            try
            {
                X = Reader.ReadSingle();
                Y = Reader.ReadSingle();
                Z = Reader.ReadSingle();
            }
            catch
            {
                New();

                throw;
            }
        }

        public void Save(BinaryWriter Writer)
        {
            Writer.Write(X);
            Writer.Write(Y);
            Writer.Write(Z);
        }

        public Vector3()
        {
            X = 0; Y = 0; Z = 0;
        }

        public Vector3(float _X, float _Y, float _Z)
        {
            X = _X; Y = _Y; Z = _Z;
        }
    }

    /// <summary>
    /// 四维向量
    /// </summary>
    public class Vector4 : Serializable
    {
        public float X { get; set; }
        public float Y { get; set; }
        public float Z { get; set; }
        public float W { get; set; }

        public void New()
        {
            X = Y = Z = W = 0;
        }

        public void Load(BinaryReader Reader)
        {
            try
            {
                X = Reader.ReadSingle();
                Y = Reader.ReadSingle();
                Z = Reader.ReadSingle();
                W = Reader.ReadSingle();
            }
            catch
            {
                New();

                throw;
            }
        }

        public void Save(BinaryWriter Writer)
        {
            Writer.Write(X);
            Writer.Write(Y);
            Writer.Write(Z);
            Writer.Write(W);
        }

        public Vector4()
        {
            X = 0; Y = 0; Z = 0; W = 0;
        }

        public Vector4(float _X, float _Y, float _Z, float _W)
        {
            X = _X; Y = _Y; Z = _Z; W = _W;
        }
    }

    /// <summary>
    /// 4x4矩阵
    /// </summary>
    public class Matrix4 : Serializable
    {
        private float[,] _Data = new float[4, 4];

        /// <summary>
        /// 获取矩阵对象
        /// </summary>
        public float[,] Data
        {
            get
            {
                return _Data;
            }
        }

        public void New()
        {
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    _Data[i, j] = 0;
                }
            }
        }

        public void Load(BinaryReader Reader)
        {
            try
            {
                for (int j = 0; j < 4; ++j)
                {
                    for (int i = 0; i < 4; ++i)
                    {
                        _Data[i, j] = Reader.ReadSingle();
                    }
                }
            }
            catch
            {
                New();

                throw;
            }
        }

        public void Save(BinaryWriter Writer)
        {
            for (int j = 0; j < 4; ++j)
            {
                for (int i = 0; i < 4; ++i)
                {
                    Writer.Write(_Data[i, j]);
                }
            }
        }

        public Matrix4()
        {
            New();
        }
    }

    /// <summary>
    /// 区块基类
    /// </summary>
    /// <remark>在覆写Load时注意不读取Label并且需要手动处理BlockSize</remark>
    public class Block : Serializable
    {
        private Label _BlockName = new Label(8);

        /// <summary>
        /// 获取区块名称
        /// </summary>
        [Browsable(false)]
        public string BlockName
        {
            get
            {
                return _BlockName.Content;
            }
        }

        /// <summary>
        /// 获取区块大小
        /// </summary>
        /// <returns></returns>
        public virtual UInt32 GetBlockSize()
        {
            throw new NotImplementedException();
        }

        public virtual void New()
        {
            throw new NotImplementedException();
        }

        public virtual void Load(BinaryReader Reader)
        {
            throw new NotImplementedException();
        }

        public virtual void Save(BinaryWriter Writer)
        {
            _BlockName.Save(Writer);
            Writer.Write(GetBlockSize());
        }

        protected Block(string BlockName)
        {
            _BlockName.Content = BlockName;
        }
    }

    /// <summary>
    /// 顶点元素
    /// </summary>
    public class VertexElement : Serializable
    {
        /// <summary>
        /// 顶点元素类型
        /// </summary>
        public enum VertexElementType
        {
            Float,   // 4字节浮点
            Float2,  // 2*4字节浮点
            Float3,  // 3*4字节浮点
            Float4,  // 4*4字节浮点
            Color,   // 4*1字节BGRA颜色分量
            UByte4,  // 4*1字节
            Short2,  // 2*2有符号短整数
            Short4   // 4*2有符号短整数
        }

        private VertexElementType _Type = VertexElementType.Float4;
        private Label _Usage = new Label(8);
        private byte _UsageIndex = 0;

        /// <summary>
        /// 获取或设置顶点元素类型
        /// </summary>
        public VertexElementType Type
        {
            get
            {
                return _Type;
            }
            set
            {
                _Type = value;
            }
        }

        /// <summary>
        /// 获取或设置用途
        /// </summary>
        public string Usage
        {
            get
            {
                return _Usage.Content;
            }
            set
            {
                _Usage.Content = value;
            }
        }

        /// <summary>
        /// 获取或设置用途索引
        /// </summary>
        public byte UsageIndex
        {
            get
            {
                return _UsageIndex;
            }
            set
            {
                _UsageIndex = value;
            }
        }

        /// <summary>
        /// 返回元素大小
        /// </summary>
        public int ElementSize
        {
            get
            {
                switch (Type)
                {
                    case VertexElementType.Float:
                        return 4;
                    case VertexElementType.Float2:
                        return 8;
                    case VertexElementType.Float3:
                        return 12;
                    case VertexElementType.Float4:
                        return 16;
                    case VertexElementType.Color:
                        return 4;
                    case VertexElementType.UByte4:
                        return 4;
                    case VertexElementType.Short2:
                        return 4;
                    case VertexElementType.Short4:
                        return 8;
                    default:
                        return 0;
                }
            }
        }

        public void New()
        {
            _Type = VertexElementType.Float4;
            _Usage.Content = String.Empty;
            _UsageIndex = 0;
        }

        public void Load(BinaryReader Reader)
        {
            try
            {
                _Type = (VertexElementType)Reader.ReadInt32();
                _Usage.Load(Reader);
                _UsageIndex = Reader.ReadByte();
            }
            catch
            {
                New();

                throw;
            }
        }

        public void Save(BinaryWriter Writer)
        {
            Writer.Write((int)_Type);
            _Usage.Save(Writer);
            Writer.Write(_UsageIndex);
        }

        public VertexElement()
        {
            New();
        }
    }

    /// <summary>
    /// 顶点区块
    /// </summary>
    public class VertexBlock : Block
    {
        private List<VertexElement> _ElementList;
        private List<byte[]> _VertexList;

        /// <summary>
        /// 获取元素列表
        /// </summary>
        public List<VertexElement> ElementList
        {
            get
            {
                return _ElementList;
            }
        }

        /// <summary>
        /// 获取顶点列表
        /// </summary>
        public List<byte[]> VertexList
        {
            get
            {
                return _VertexList;
            }
        }

        /// <summary>
        /// 计算顶点大小
        /// </summary>
        /// <returns></returns>
        public UInt32 GetVertexSize()
        {
            UInt32 tRet = 0;
            foreach (VertexElement i in _ElementList)
            {
                tRet += (UInt32)i.ElementSize;
            }
            return tRet;
        }

        /// <summary>
        /// 检查顶点元素大小是否符合要求
        /// </summary>
        public bool CheckFormat()
        {
            // 顶点大小
            UInt32 tVertexSize = GetVertexSize();

            for (int i = 0; i < _VertexList.Count; ++i)
            {
                if (_VertexList[i].Length != tVertexSize)
                    return false;
            }

            return true;
        }

        public override UInt32 GetBlockSize()
        {
            UInt32 tRet = 9;

            for (UInt32 i = 0; i < _ElementList.Count; ++i)
            {
                tRet += 13;
            }

            // 顶点大小
            UInt32 tVertexSize = GetVertexSize();

            for (UInt32 i = 0; i < _VertexList.Count; ++i)
            {
                tRet += tVertexSize;
            }

            return tRet;
        }

        public override void New()
        {
            _ElementList = new List<VertexElement>();
            _VertexList = new List<byte[]>();
        }

        public override void Load(BinaryReader Reader)
        {
            New();

            try
            {
                UInt32 tBlockLen = Reader.ReadUInt32();
                UInt32 tBlockSizeCheck = 0;
                UInt32 tVertexCount = Reader.ReadUInt32();
                UInt32 tVertexElementCount = Reader.ReadUInt32();
                byte tStreamFlag = Reader.ReadByte();
                if (tStreamFlag != 0)
                    throw new NotImplementedException("Informal format not supported.");

                tBlockSizeCheck += 9;

                // 读取顶点定义数组
                for (UInt32 i = 0; i < tVertexElementCount; ++i)
                {
                    VertexElement tElement = new VertexElement();
                    tElement.Load(Reader);
                    _ElementList.Add(tElement);

                    tBlockSizeCheck += 13;
                }

                // 顶点大小
                UInt32 tVertexSize = GetVertexSize();

                // 读取顶点数组
                for (UInt32 i = 0; i < tVertexCount; ++i)
                {
                    _VertexList.Add(Reader.ReadBytes((int)tVertexSize));

                    tBlockSizeCheck += tVertexSize;
                }

                // 检查大小
                if (tBlockSizeCheck != tBlockLen)
                    throw new FormatException("Vertex block format error.");
            }
            catch
            {
                New();

                throw;
            }
        }

        public override void Save(BinaryWriter Writer)
        {
            if (false == CheckFormat())
                throw new FormatException("Vertex size error.");

            base.Save(Writer);

            Writer.Write((UInt32)_VertexList.Count);
            Writer.Write((UInt32)_ElementList.Count);
            Writer.Write((byte)0);

            for (int i = 0; i < _ElementList.Count; ++i)
            {
                _ElementList[i].Save(Writer);
            }

            for (int i = 0; i < _VertexList.Count; ++i)
            {
                Writer.Write(_VertexList[i]);
            }
        }

        public VertexBlock()
            : base("VERTEX")
        {
            New();
        }
    }

    /// <summary>
    /// 索引区块
    /// </summary>
    public class IndexBlock : Block
    {
        private bool _UseInt32Index;
        private List<UInt32> _IndexList;

        /// <summary>
        /// 获取或设置是否使用Int32索引
        /// </summary>
        public bool UseInt32Index
        {
            get
            {
                return _UseInt32Index;
            }
            set
            {
                _UseInt32Index = value;
            }
        }

        /// <summary>
        /// 获取索引列表
        /// </summary>
        public List<UInt32> IndexList
        {
            get
            {
                return _IndexList;
            }
        }

        /// <summary>
        /// 检查索引元素大小是否符合要求
        /// </summary>
        public bool CheckFormat()
        {
            for (int i = 0; i < _IndexList.Count; ++i)
            {
                if (_IndexList[i] > UInt16.MaxValue)
                    return false;
            }

            return true;
        }

        public override UInt32 GetBlockSize()
        {
            UInt32 tRet = 6 + (UInt32)(UseInt32Index ? _IndexList.Count * 4 : _IndexList.Count * 2);
            return tRet;
        }

        public override void New()
        {
            _UseInt32Index = false;
            _IndexList = new List<uint>();
        }

        public override void Load(BinaryReader Reader)
        {
            New();

            try
            {
                UInt32 tBlockLen = Reader.ReadUInt32();
                UInt32 tBlockSizeCheck = 0;
                UInt32 tIndexCount = Reader.ReadUInt32();
                bool tUseInt32Index = Reader.ReadBoolean();
                byte tStreamFlag = Reader.ReadByte();
                if (tStreamFlag != 0)
                    throw new NotImplementedException("Informal format not supported.");

                tBlockSizeCheck += 6;

                for (UInt32 i = 0; i < tIndexCount; ++i)
                {
                    if (tUseInt32Index)
                    {
                        _IndexList.Add(Reader.ReadUInt32());

                        tBlockSizeCheck += 4;
                    }
                    else
                    {
                        _IndexList.Add(Reader.ReadUInt16());

                        tBlockSizeCheck += 2;
                    }
                }

                // 检查大小
                if (tBlockSizeCheck != tBlockLen)
                    throw new FormatException("Index block format error.");
            }
            catch
            {
                New();

                throw;
            }
        }

        public override void Save(BinaryWriter Writer)
        {
            base.Save(Writer);

            Writer.Write((UInt32)_IndexList.Count);
            Writer.Write(UseInt32Index);
            Writer.Write((byte)0);

            for (int i = 0; i < _IndexList.Count; ++i)
            {
                if (UseInt32Index)
                {
                    Writer.Write((UInt32)_IndexList[i]);
                }
                else
                {
                    Writer.Write((UInt16)_IndexList[i]);
                }
            }
        }

        public IndexBlock()
            : base("INDEX")
        {
            New();
        }
    }

    /// <summary>
    /// 子集区块
    /// </summary>
    public class SubsetBlock : Block
    {
        public enum PrimitiveType
        {
            PointList = 1,  // 点集
            LineList = 2,  // 线集
            LineStrip = 3,  // 线带集
            TriangleList = 4,  // 三角形集
            TriangleStrip = 5,  // 三角形带
            TriangleFan = 6   // 三角形扇
        }

        private StringData _SubsetName = new StringData();
        private StringData _MaterialName = new StringData();
        private PrimitiveType _PrimType;
        private UInt32 _StartIndex;
        private UInt32 _PrimitiveCount;

        /// <summary>
        /// 获取或设置子集名字
        /// </summary>
        [DisplayName("子集名称")]
        [Description("获取或设置子集的名称")]
        public string SubsetName
        {
            get
            {
                return _SubsetName.Content;
            }
            set
            {
                _SubsetName.Content = value;
            }
        }

        /// <summary>
        /// 获取或设置材质名字
        /// </summary>
        [DisplayName("材质名称")]
        [Description("获取或设置材质的名称")]
        public string MaterialName
        {
            get
            {
                return _MaterialName.Content;
            }
            set
            {
                _MaterialName.Content = value;
            }
        }

        /// <summary>
        /// 获取或设置子集图元类型
        /// </summary>
        [DisplayName("图元类型")]
        [Description("获取或设置子集定义的图元类型")]
        public PrimitiveType SubsetPrimitiveType
        {
            get
            {
                return _PrimType;
            }
            set
            {
                _PrimType = value;
            }
        }

        /// <summary>
        /// 开始索引
        /// </summary>
        [DisplayName("开始索引")]
        [Description("获取或设置图元数据在索引缓冲中的起始位置")]
        public UInt32 StartIndex
        {
            get
            {
                return _StartIndex;
            }
            set
            {
                _StartIndex = value;
            }
        }

        /// <summary>
        /// 图元数量
        /// </summary>
        [DisplayName("图元数量")]
        [Description("获取或设置子集中的图元数量")]
        public UInt32 PrimitiveCount
        {
            get
            {
                return _PrimitiveCount;
            }
            set
            {
                _PrimitiveCount = value;
            }
        }

        public override UInt32 GetBlockSize()
        {
            return (UInt32)(_SubsetName.GetSaveSize() + _MaterialName.GetSaveSize() + 4 * 3);
        }

        public override void New()
        {
            _SubsetName.New();
            _MaterialName.New();
            _PrimType = PrimitiveType.TriangleList;
            _StartIndex = 0;
            _PrimitiveCount = 0;
        }

        public override void Load(BinaryReader Reader)
        {
            New();

            try
            {
                UInt32 tBlockLen = Reader.ReadUInt32();
                long tPos = Reader.BaseStream.Position;
                _SubsetName.Load(Reader);
                _MaterialName.Load(Reader);
                _PrimType = (PrimitiveType)Reader.ReadUInt32();
                _StartIndex = Reader.ReadUInt32();
                _PrimitiveCount = Reader.ReadUInt32();

                // 检查大小
                if (Reader.BaseStream.Position - tPos != tBlockLen)
                    throw new FormatException("Subset block format error.");
            }
            catch
            {
                New();

                throw;
            }
        }

        public override void Save(BinaryWriter Writer)
        {
            base.Save(Writer);

            _SubsetName.Save(Writer);
            _MaterialName.Save(Writer);
            Writer.Write((UInt32)_PrimType);
            Writer.Write(StartIndex);
            Writer.Write(PrimitiveCount);
        }

        public SubsetBlock Clone()
        {
            SubsetBlock tRet = new SubsetBlock();
            tRet._SubsetName = _SubsetName.Clone();
            tRet._MaterialName = _MaterialName.Clone();
            tRet._PrimType = _PrimType;
            tRet._StartIndex = _StartIndex;
            tRet._PrimitiveCount = _PrimitiveCount;
            return tRet;
        }

        public SubsetBlock()
            : base("SUBSET")
        {
            New();
        }
    }

    /// <summary>
    /// 材质区块
    /// </summary>
    public class MaterialBlock : Block
    {
        private StringData _MaterialName = new StringData();
        private StringData _DefineXML = new StringData();

        /// <summary>
        /// 获取或设置材质名字
        /// </summary>
        public string MaterialName
        {
            get
            {
                return _MaterialName.Content;
            }
            set
            {
                _MaterialName.Content = value;
            }
        }

        /// <summary>
        /// 获取或设置材质定义
        /// </summary>
        public string DefineXML
        {
            get
            {
                return _DefineXML.Content;
            }
            set
            {
                _DefineXML.Content = value;
            }
        }

        public override UInt32 GetBlockSize()
        {
            return (UInt32)(_MaterialName.GetSaveSize() + _DefineXML.GetSaveSize());
        }

        public override void New()
        {
            _MaterialName.New();
            _DefineXML.New();
        }

        public override void Load(BinaryReader Reader)
        {
            New();

            try
            {
                UInt32 tBlockLen = Reader.ReadUInt32();
                long tPos = Reader.BaseStream.Position;
                _MaterialName.Load(Reader);
                _DefineXML.Load(Reader);

                // 检查大小
                if (Reader.BaseStream.Position - tPos != tBlockLen)
                    throw new FormatException("Material block format error.");
            }
            catch
            {
                New();

                throw;
            }
        }

        public override void Save(BinaryWriter Writer)
        {
            base.Save(Writer);

            _MaterialName.Save(Writer);
            _DefineXML.Save(Writer);
        }

        public MaterialBlock()
            : base("MATERIAL")
        {
            New();
        }
    }

    /// <summary>
    /// 包围盒区块
    /// </summary>
    public class BoundingBoxBlock : Block
    {
        [DisplayName("X最小值")]
        [Description("获取或设置包围盒在X轴的最小值")]
        public float MinX { get; set; }
        [DisplayName("Y最小值")]
        [Description("获取或设置包围盒在Y轴的最小值")]
        public float MinY { get; set; }
        [DisplayName("Z最小值")]
        [Description("获取或设置包围盒在Z轴的最小值")]
        public float MinZ { get; set; }
        [DisplayName("X最大值")]
        [Description("获取或设置包围盒在X轴的最大值")]
        public float MaxX { get; set; }
        [DisplayName("Y最大值")]
        [Description("获取或设置包围盒在Y轴的最大值")]
        public float MaxY { get; set; }
        [DisplayName("Z最大值")]
        [Description("获取或设置包围盒在Z轴的最大值")]
        public float MaxZ { get; set; }

        public override UInt32 GetBlockSize()
        {
            return 6 * 4;
        }

        public override void New()
        {
            MinX = MinY = MinZ = 0;
            MaxX = MaxY = MaxZ = 0;
        }

        public override void Load(BinaryReader Reader)
        {
            New();

            try
            {
                UInt32 tBlockLen = Reader.ReadUInt32();
                long tPos = Reader.BaseStream.Position;
                MinX = Reader.ReadSingle();
                MinY = Reader.ReadSingle();
                MinZ = Reader.ReadSingle();
                MaxX = Reader.ReadSingle();
                MaxY = Reader.ReadSingle();
                MaxZ = Reader.ReadSingle();

                // 检查大小
                if (Reader.BaseStream.Position - tPos != tBlockLen)
                    throw new FormatException("BoundingBox block format error.");
            }
            catch
            {
                New();

                throw;
            }
        }

        public override void Save(BinaryWriter Writer)
        {
            base.Save(Writer);

            Writer.Write(MinX);
            Writer.Write(MinY);
            Writer.Write(MinZ);
            Writer.Write(MaxX);
            Writer.Write(MaxY);
            Writer.Write(MaxZ);
        }

        public BoundingBoxBlock Clone()
        {
            BoundingBoxBlock tRet = new BoundingBoxBlock();
            tRet.MinX = MinX;
            tRet.MinY = MinY;
            tRet.MinZ = MinZ;
            tRet.MaxX = MaxX;
            tRet.MaxY = MaxY;
            tRet.MaxZ = MaxZ;
            return tRet;
        }

        public BoundingBoxBlock()
            : base("BOUNDBOX")
        {
            New();
        }
    }

    /// <summary>
    /// 未知区块
    /// </summary>
    public class UnknownBlock : Block
    {
        private byte[] _BlockData = null;

        /// <summary>
        /// 获取块数据
        /// </summary>
        public byte[] BlockData
        {
            get
            {
                return _BlockData;
            }
        }

        public override UInt32 GetBlockSize()
        {
            return (UInt32)_BlockData.Length;
        }

        public override void New()
        {
            _BlockData = null;
        }

        public override void Load(BinaryReader Reader)
        {
            New();

            try
            {
                UInt32 tBlockLen = Reader.ReadUInt32();
                _BlockData = Reader.ReadBytes((int)tBlockLen);
            }
            catch
            {
                New();

                throw;
            }
        }

        public override void Save(BinaryWriter Writer)
        {
            base.Save(Writer);

            if (_BlockData != null)
                Writer.Write(_BlockData);
        }

        public UnknownBlock(string LabelName)
            : base(LabelName)
        {
            New();
        }
    }

    /// <summary>
    /// 网格模型文件
    /// </summary>
    public class MeshFile : Serializable
    {
        static public MeshFile FromFile(string Path)
        {
            MeshFile tRet = new MeshFile();
            tRet.Load(Path);
            return tRet;
        }

        private StringData _Author = new StringData();
        private StringData _Description = new StringData();
        private List<Block> _BlockList = new List<Block>();

        /// <summary>
        /// 获取或设置模型作者信息
        /// </summary>
        [DisplayName("作者")]
        [Description("获取或设置模型的作者")]
        public string Author
        {
            get
            {
                return _Author.Content;
            }
            set
            {
                _Author.Content = value;
            }
        }

        /// <summary>
        /// 获取或设置模型描述信息
        /// </summary>
        [DisplayName("描述")]
        [Description("获取或设置模型的描述")]
        public string Description
        {
            get
            {
                return _Description.Content;
            }
            set
            {
                _Description.Content = value;
            }
        }

        /// <summary>
        /// 获取块列表
        /// </summary>
        [Browsable(false)]
        public List<Block> BlockList
        {
            get
            {
                return _BlockList;
            }
        }

        public void New()
        {
            _Author.New();
            _Description.New();
            _BlockList.Clear();
        }

        public void Load(BinaryReader Reader)
        {
            New();

            try
            {
                // 检查文件头
                Label tHeader = new Label(4);
                tHeader.Load(Reader);
                if (tHeader.Content != "FMM")
                    throw new FormatException("Header check failed.");
                if (Reader.ReadUInt32() != 100)
                    throw new FormatException("Version not support.");
                UInt32 tBlockCount = Reader.ReadUInt32();
                _Author.Load(Reader);
                _Description.Load(Reader);

                // 读取所有块
                Label tBlockHeader = new Label(8);
                for (UInt32 i = 0; i < tBlockCount; ++i)
                {
                    Block tBlock = null;
                    tBlockHeader.Load(Reader);
                    switch (tBlockHeader.Content)
                    {
                        case "VERTEX":
                            tBlock = new VertexBlock();
                            break;
                        case "INDEX":
                            tBlock = new IndexBlock();
                            break;
                        case "SUBSET":
                            tBlock = new SubsetBlock();
                            break;
                        case "MATERIAL":
                            tBlock = new MaterialBlock();
                            break;
                        case "BOUNDBOX":
                            tBlock = new BoundingBoxBlock();
                            break;
                        default:
                            tBlock = new UnknownBlock(tBlockHeader.Content);
                            break;
                    }

                    tBlock.Load(Reader);
                    _BlockList.Add(tBlock);
                }
            }
            catch
            {
                New();

                throw;
            }
        }

        public void Load(string Path)
        {
            using (FileStream tFile = new FileStream(Path, FileMode.Open, FileAccess.Read))
            {
                BinaryReader tReader = new BinaryReader(tFile);
                Load(tReader);
                tFile.Close();
            }
        }

        public void Save(BinaryWriter Writer)
        {
            Label tHeader = new Label(4);
            tHeader.Content = "FMM";
            tHeader.Save(Writer);
            Writer.Write((UInt32)100);
            Writer.Write((UInt32)_BlockList.Count);
            _Author.Save(Writer);
            _Description.Save(Writer);
            for (int i = 0; i < _BlockList.Count; ++i)
            {
                _BlockList[i].Save(Writer);
            }
        }

        public void Save(string Path)
        {
            // 使用MemoryStream预先保存数据
            using (MemoryStream tStream = new MemoryStream())
            {
                BinaryWriter tWriter = new BinaryWriter(tStream);
                Save(tWriter);
                tWriter.Seek(0, SeekOrigin.Begin);

                using (FileStream tFile = new FileStream(Path, FileMode.Create, FileAccess.Write))
                {
                    byte[] tBuf = new byte[4096];
                    int tLen = 0;
                    while ((tLen = tStream.Read(tBuf, 0, 4096)) > 0)
                    {
                        tFile.Write(tBuf, 0, tLen);
                    }
                    tFile.Close();
                }
                tStream.Close();
            }
        }

        public MeshFile()
        {
            New();
        }
    }
}
