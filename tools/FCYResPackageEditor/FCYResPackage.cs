using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Xml;
using zlib;

namespace FancyLib
{
    /*
     *  FCY资源结构 
     *   文件头
     *    char[4] = "frp\0" // 文件标识
     *    uint    = 1       // 文件版本
     *    uint              // 资源索引位移（相对文件）
     *   
     *   主数据区
     *    byte[]          // 数据流1
     *    byte[]          // 数据流2
     *    ...
     *   
     *   资源索引区
     *    uint            // 索引数据解压后长度
     *    byte[]          // 使用deflate压缩的XML数据（无xml头、UTF16编码）
     */
    /*
     *  XML数据结构
     *   <Res>
     *     <Floder Name="文件夹节点名">
     *       <Node Name="节点名"
     *             OrgSize="原始资源大小(10进制无符号整数)"
     *             IsCompressed="(yes/no强制小写)"
     *             CompressedSize="压缩后大小(10进制无符号整数)"
     *             Offset="相对于主数据区的位移(10进制无符号整数)">
     *             附加数据
     *       </Node>
     *     </Floder>
     *   </Res>
     */

    /// <summary>
    /// 辅助类
    /// </summary>
    abstract class FCYResPackageHelper
    {
        /// <summary>
        /// 将流A数据读入流B
        /// </summary>
        /// <param name="A">流A</param>
        /// <param name="B">流B</param>
        /// <param name="Length">数据长度</param>
        public static void FillStream(Stream A, Stream B, int Length)
        {
            byte[] tBuffer = new byte[8192]; // 8kb缓冲区
            int tLen = 0;
            int tTotal = 0;

            while ((tLen = A.Read(tBuffer, 0, (Length - tTotal) > 8192 ? 8192 : Length - tTotal)) > 0)
            {
                tTotal += tLen;
                B.Write(tBuffer, 0, tLen);
            }

            if (tTotal != Length)
                throw new EndOfStreamException();
        }

        /// <summary>
        /// 压缩Deflate数据至
        /// </summary>
        /// <param name="A">要被压缩的流</param>
        /// <param name="B">要输出的流</param>
        /// <param name="Length">要压缩的数据长度</param>
        public static void DeflateEncodeTo(Stream A, Stream B, int Length)
        {
            ZOutputStream tOutput = new ZOutputStream(B, 9);

            byte[] tBuffer = new byte[8192]; // 8kb缓冲区
            int tLen = 0;
            int tTotal = 0;
            while ((tLen = A.Read(tBuffer, 0, (Length - tTotal) > 8192 ? 8192 : Length - tTotal)) > 0)
            {
                tTotal += tLen;
                tOutput.Write(tBuffer, 0, tLen);
            }
            tOutput.finish();

            if (tTotal != Length)
                throw new EndOfStreamException();
        }

        /// <summary>
        /// 解压Deflate数据至
        /// </summary>
        /// <param name="A">被解压的流</param>
        /// <param name="B">要输出的流</param>
        /// <param name="Length">被解压的数据长度</param>
        /// <returns>解压的数据长度</returns>
        public static int DeflateDecodeTo(Stream A, Stream B, int Length = 0)
        {
            ZInputStream tDecode = new ZInputStream(A);

            byte[] tBuffer = new byte[8192]; // 8kb缓冲区
            int tLen = 0;
            int tTotal = 0;

            while ((tLen = tDecode.read(tBuffer, 0, (Length - tTotal > 8192 ? 8192 : Length - tTotal))) > 0)
            {
                tTotal += tLen;
                B.Write(tBuffer, 0, tLen);
            }

            if (Length != 0 && tTotal != Length)
                throw new IndexOutOfRangeException();

            return tTotal;
        }

        /// <summary>
        /// 解压Deflate数据
        /// </summary>
        /// <param name="A">被压缩的流</param>
        /// <param name="Length">原始数据长度（可忽略）</param>
        /// <returns>解压的流</returns>
        public static MemoryStream DeflateDecode(Stream A, int Length = 0)
        {
            MemoryStream tOut = new MemoryStream();

            DeflateDecodeTo(A, tOut, Length);

            tOut.Seek(0, SeekOrigin.Begin);

            return tOut;
        }
    };

    /// <summary>
    /// 无效节点
    /// </summary>
    public class FCYExcpInvalidNode : Exception
    {
        public FCYExcpInvalidNode() { }
        public FCYExcpInvalidNode(string Desc)
            : base(Desc) { }
    }
    /// <summary>
    /// 节点错误
    /// </summary>
    public class FCYExcpNodeErr : Exception
    {
        public FCYExcpNodeErr() { }
        public FCYExcpNodeErr(string Desc)
            : base(Desc) { }
    }
    /// <summary>
    /// 无效名称
    /// </summary>
    public class FCYExcpNameErr : Exception
    {
        public FCYExcpNameErr() { }
        public FCYExcpNameErr(string Desc)
            : base(Desc) { }
    }
    /// <summary>
    /// 数据错误
    /// </summary>
    public class FCYExcpDataErr : Exception
    {
        public FCYExcpDataErr() { }
        public FCYExcpDataErr(string Desc)
            : base(Desc) { }
    }
    /// <summary>
    /// 文件错误
    /// </summary>
    public class FCYExcpOpenErr : Exception
    {
        public FCYExcpOpenErr() { }
        public FCYExcpOpenErr(string Desc)
            : base(Desc) { }
    }
    public class FCYExcpSaveErr : Exception
    {
        public FCYExcpSaveErr() { }
        public FCYExcpSaveErr(string Desc)
            : base(Desc) { }
    }

    /// <summary>
    /// 节点基类
    /// </summary>
    public abstract class FCYResPackageNode : IComparable
    {
        private FCYResPackageFloderNode m_Parent; // 父节点
        private string m_Name; // 节点名

        /// <summary>
        /// 检查名称合法性
        /// </summary>
        /// <param name="Str">名称</param>
        /// <returns>是否合法</returns>
        public static bool CheckName(string Str)
        {
            if (Str == "")
                return false;

            foreach (char x in Str)
            {
                switch (x)
                {
                    case '\\':
                    case '/':
                    case ':':
                    case '*':
                    case '?':
                    case '"':
                    case '<':
                    case '>':
                    case '|':
                        return false;
                }
            }

            return true;
        }

        /// <summary>
        /// 节点名称
        /// </summary>
        public string Name
        {
            get
            {
                return m_Name;
            }
            set
            {
                if (m_Parent.Contains(value))
                    throw new FCYExcpNameErr("Node Already Exsited.");
                else if (CheckName(value) == false)
                    throw new FCYExcpNameErr("Node Name Invalid.");

                m_Name = value;
            }
        }

        /// <summary>
        /// 返回是否是文件夹节点
        /// </summary>
        /// <returns></returns>
        public abstract bool IsFloder();

        /// <summary>
        /// 写到流
        /// </summary>
        /// <param name="pOut">输出流</param>
        /// <param name="ParentXml">父Xml节点</param>
        internal abstract void WriteToStream(Stream pOut, XmlElement ParentXml);

        public int CompareTo(object obj)
        {
            return -((FCYResPackageNode)obj).Name.CompareTo(Name);
        }

        /// <summary>
        /// 创建一个节点
        /// </summary>
        /// <param name="Parent">父节点</param>
        /// <param name="Name">节点名称</param>
        protected FCYResPackageNode(FCYResPackageFloderNode Parent, string Name)
        {
            m_Parent = Parent;
            m_Name = Name;
        }
    }

    /// <summary>
    /// 数据节点
    /// </summary>
    public class FCYResPackageDataNode : FCYResPackageNode
    {
        // 保存文件类
        private FCYResPackage m_Package = null;

        // 数据属性
        private string m_Content = "";         // 附加数据
        private uint m_DataRealSize = 0;       // 真实数据大小
        private uint m_DataCompressedSize = 0; // 压缩后数据大小
        private uint m_MainDataOffset = 0;     // 数据偏移
        private bool m_IsCompressed = false;   // 是否压缩

        // 未被压缩的本地数据流，null表示来自数据包
        private Stream m_Stream = null;

        /// <summary>
        /// 额外数据
        /// </summary>
        public string AdditionData
        {
            get
            {
                return m_Content;
            }
            set
            {
                m_Content = value;
            }
        }

        /// <summary>
        /// 数据真实大小
        /// </summary>
        public uint DataRealSize
        {
            get
            {
                return m_DataRealSize;
            }
        }

        /// <summary>
        /// 数据压缩后大小
        /// </summary>
        public uint DataCompressedSize
        {
            get
            {
                return m_DataCompressedSize;
            }
        }

        /// <summary>
        /// 数据相对主缓冲区位移
        /// </summary>
        public uint DataOffset
        {
            get
            {
                return m_MainDataOffset;
            }
        }

        /// <summary>
        /// 是否压缩
        /// </summary>
        public bool IsCompressed
        {
            get
            {
                return m_IsCompressed;
            }
            set
            {
                if (m_Stream == null) // 数据包内流特殊对待
                {
                    // 将压缩的流转为未压缩的流
                    if (m_IsCompressed == true && value == false)
                    {
                        // 转到内存
                        m_Stream = ExtractStreamToMem();

                        m_IsCompressed = false;
                        m_MainDataOffset = 0;
                        m_DataCompressedSize = 0;
                    }
                    // 将未压缩的流转为压缩的流
                    else if (m_IsCompressed == false && value == true)
                    {
                        // 转到内存
                        m_Stream = ExtractStreamToMem();

                        m_IsCompressed = true;
                        m_MainDataOffset = 0;
                        m_DataCompressedSize = 0;
                    }
                }
                else // 外部流不予理会
                    m_IsCompressed = value;
            }
        }

        /// <summary>
        /// 解压原始数据流
        /// </summary>
        /// <param name="pOut">要输出的目的流</param>
        public void ExtractStreamTo(Stream pOut)
        {
            if (m_Stream == null)
            {
                // 来自数据包内的流
                Stream tBaseStream = m_Package.BaseStream;

                // 寻址到数据开始处
                tBaseStream.Seek(FCYResPackage.HeaderSize + DataOffset, SeekOrigin.Begin);

                // 是否压缩
                if (IsCompressed)
                {
                    // 已压缩，解压到目的流
                    FCYResPackageHelper.DeflateDecodeTo(tBaseStream, pOut, (int)DataRealSize);
                }
                else
                {
                    // 直接输出到目标流
                    FCYResPackageHelper.FillStream(tBaseStream, pOut, (int)DataRealSize);
                }
            }
            else // 来自本地的数据流
            {
                // 直接输出到目标流
                FCYResPackageHelper.FillStream(m_Stream, pOut, (int)DataRealSize);
            }
        }

        /// <summary>
        /// 解压原始数据流到内存
        /// </summary>
        /// <returns>原始数据流</returns>
        public MemoryStream ExtractStreamToMem()
        {
            MemoryStream pOut = new MemoryStream();
            ExtractStreamTo(pOut);
            return pOut;
        }

        /// <summary>
        /// 置换流
        /// </summary>
        /// <param name="NewStream">要置换的流</param>
        public void ReplaceStream(Stream NewStream)
        {
            m_DataCompressedSize = 0;
            m_DataRealSize = 0;
            m_IsCompressed = false;
            m_MainDataOffset = 0;
            if (m_Stream != null)
            {
                // 关闭原始流
                m_Stream.Close();
            }
            if (NewStream != null)
            {
                // 设置流
                m_Stream = NewStream;
                m_DataRealSize = (uint)m_Stream.Length;
            }
        }

        // === 接口实现 ===
        public override bool IsFloder()
        {
            return false;
        }

        internal override void WriteToStream(Stream pOut, XmlElement ParentXml)
        {
            // 记下当前位置
            long tPos = pOut.Position;

            // 是否是外部流
            if (m_Stream == null)
            {
                // 外部流
                Stream tBaseStream = m_Package.BaseStream;

                // 寻址
                tBaseStream.Seek(FCYResPackage.HeaderSize + m_MainDataOffset, SeekOrigin.Begin);

                // 直接复制
                FCYResPackageHelper.FillStream(tBaseStream, pOut, (int)(IsCompressed ? DataCompressedSize : DataRealSize));
            }
            else
            {
                // 内部流
                if (IsCompressed)
                {
                    long tPosStart = pOut.Position;

                    // 压缩流
                    m_Stream.Seek(0, SeekOrigin.Begin);
                    FCYResPackageHelper.DeflateEncodeTo(m_Stream, pOut, (int)DataRealSize);

                    long tPosEnd = pOut.Position;

                    // 关闭内部流
                    m_Stream.Close();
                    m_Stream = null;

                    // 更新数据
                    m_DataCompressedSize = (uint)(tPosEnd - tPosStart);
                }
                else
                {
                    // 直接复制
                    m_Stream.Seek(0, SeekOrigin.Begin);
                    FCYResPackageHelper.FillStream(m_Stream, pOut, (int)DataRealSize);
                }
            }

            // 更新位置
            m_MainDataOffset = (uint)tPos - FCYResPackage.HeaderSize;

            // 准备Xml数据
            XmlElement tElement = ParentXml.OwnerDocument.CreateElement("Node");
            tElement.SetAttribute("Name", Name);
            tElement.SetAttribute("OrgSize", DataRealSize.ToString());
            tElement.SetAttribute("IsCompressed", IsCompressed ? "yes" : "no");
            tElement.SetAttribute("CompressedSize", DataCompressedSize.ToString());
            tElement.SetAttribute("Offset", DataOffset.ToString());
            tElement.InnerText = AdditionData;

            // 写出Xml数据
            ParentXml.AppendChild(tElement);
        }

        /// <summary>
        /// 创建一个节点
        /// </summary>
        /// <param name="ParentFile">父文件</param>
        /// <param name="Parent">父节点</param>
        /// <param name="Name">名称</param>
        public FCYResPackageDataNode(FCYResPackage ParentFile, FCYResPackageFloderNode Parent, string Name)
            : base(Parent, Name)
        {
            m_Package = ParentFile;
        }

        /// <summary>
        /// 从文件创建一个节点
        /// </summary>
        /// <param name="ParentFile">父文件</param>
        /// <param name="Parent">父节点</param>
        /// <param name="Name">名称</param>
        /// <param name="FileName">文件名</param>
        public FCYResPackageDataNode(FCYResPackage ParentFile, FCYResPackageFloderNode Parent, string Name, string FileName)
            : base(Parent, Name)
        {
            m_Package = ParentFile;

            FileStream tFile = new FileStream(FileName, FileMode.Open, FileAccess.Read, FileShare.Read);
            ReplaceStream(tFile);
        }

        /// <summary>
        /// 从XML数据创建一个节点
        /// </summary>
        /// <param name="ParentFile">父文件</param>
        /// <param name="Parent">父节点</param>
        /// <param name="XmlNode">Xml数据</param>
        public FCYResPackageDataNode(FCYResPackage ParentFile, FCYResPackageFloderNode Parent, XmlElement XmlNode)
            : base(Parent, XmlNode.GetAttribute("Name"))
        {
            m_Package = ParentFile;

            // 检查Xml是否有效
            if (XmlNode.Name != "Node")
                throw new FCYExcpInvalidNode("Not a Node.");

            // 设置额外数据
            AdditionData = XmlNode.InnerText;

            // 原始大小
            if (XmlNode.HasAttribute("OrgSize"))
                m_DataRealSize = Convert.ToUInt32(XmlNode.GetAttribute("OrgSize"));
            else
                throw new FCYExcpNodeErr("Attribute Err.");

            // 压缩后大小
            if (XmlNode.HasAttribute("CompressedSize"))
                m_DataCompressedSize = Convert.ToUInt32(XmlNode.GetAttribute("CompressedSize"));
            else
                throw new FCYExcpNodeErr("Attribute Err.");

            // 位移量
            if (XmlNode.HasAttribute("Offset"))
                m_MainDataOffset = Convert.ToUInt32(XmlNode.GetAttribute("Offset"));
            else
                throw new FCYExcpNodeErr("Attribute Err.");

            // 是否压缩
            if (XmlNode.HasAttribute("IsCompressed"))
                m_IsCompressed = XmlNode.GetAttribute("IsCompressed") == "yes" ? true : false;
            else
                throw new FCYExcpNodeErr("Attribute Err.");
        }

        ~FCYResPackageDataNode()
        {
            if (m_Stream != null)
            {
                m_Stream.Close();
                m_Stream = null;
            }
        }
    }

    /// <summary>
    /// 档案夹节点
    /// </summary>
    public class FCYResPackageFloderNode : FCYResPackageNode
    {
        // 节点表
        private List<FCYResPackageNode> m_NodeList = new List<FCYResPackageNode>();

        /// <summary>
        /// 节点表
        /// </summary>
        public IList<FCYResPackageNode> NodeList
        {
            get
            {
                return m_NodeList;
            }
        }

        /// <summary>
        /// 是否包含节点
        /// </summary>
        /// <param name="Node">节点名</param>
        /// <returns>是否存在</returns>
        public bool Contains(string Node)
        {
            foreach (FCYResPackageNode x in NodeList)
            {
                if (x.Name == Node)
                    return true;
            }
            return false;
        }

        /// <summary>
        /// 追加节点
        /// </summary>
        /// <param name="Node">节点</param>
        public void Add(FCYResPackageNode Node)
        {
            if (Contains(Node.Name))
                throw new FCYExcpNameErr("Name exsited.");
            if (FCYResPackageDataNode.CheckName(Node.Name) == false)
                throw new FCYExcpNameErr("Invalid Name.");
            else
                NodeList.Add(Node);
        }

        public void Sort()
        {
            m_NodeList.Sort();
        }

        // === 接口实现 ===
        public override bool IsFloder()
        {
            return true;
        }

        internal override void WriteToStream(Stream pOut, XmlElement ParentXml)
        {
            XmlElement tElement = ParentXml.OwnerDocument.CreateElement("Floder");
            tElement.SetAttribute("Name", Name);

            m_NodeList.Sort();
            for (int i = 0; i < NodeList.Count; i++)
            {
                m_NodeList[i].WriteToStream(pOut, tElement);
            }

            ParentXml.AppendChild(tElement);
        }

        /// <summary>
        /// 创建文件夹节点
        /// </summary>
        /// <param name="Parent">父节点</param>
        /// <param name="Name">节点名</param>
        public FCYResPackageFloderNode(FCYResPackageFloderNode Parent, string Name)
            : base(Parent, Name)
        { }

        /// <summary>
        /// 从Xml创建文件夹节点
        /// </summary>
        /// <param name="Package">绑定的资源包</param>
        /// <param name="Parent">父节点</param>
        /// <param name="XmlNode">Xml节点</param>
        public FCYResPackageFloderNode(FCYResPackage Package, FCYResPackageFloderNode Parent, XmlElement XmlNode)
            : base(Parent, XmlNode.HasAttribute("Name") ? XmlNode.GetAttribute("Name") : "")
        {
            m_NodeList.Clear();

            foreach (XmlElement x in XmlNode)
            {
                if (x.Name == "Floder")
                {
                    FCYResPackageFloderNode tFloder = new FCYResPackageFloderNode(Package, this, x);
                    Add(tFloder);
                }
                else
                {
                    FCYResPackageDataNode tData = new FCYResPackageDataNode(Package, this, x);
                    Add(tData);
                }
            }
        }
    }

    /// <summary>
    /// FCY资源包文件
    /// </summary>
    public class FCYResPackage
    {
        internal const uint HeaderSize = 4 * 3;

        // 数据包头部
        private uint m_Flag = 0x00707266;        // 文件标识"frp\0"
        private uint m_Version = 1;              // 版本
        private uint m_IndexOffset = HeaderSize; // 索引区偏移

        // 根节点
        private FCYResPackageFloderNode m_Root = null;

        // 数据包绑定的流
        private FileStream m_BaseStream = null;
        private string m_FilePath = "";

        /// <summary>
        /// 返回文件路径
        /// </summary>
        public string FilePath
        {
            get
            {
                return m_FilePath;
            }
        }

        /// <summary>
        /// 返回数据包绑定的流
        /// </summary>
        public Stream BaseStream
        {
            get
            {
                return m_BaseStream;
            }
        }

        /// <summary>
        /// 返回根节点
        /// </summary>
        public FCYResPackageFloderNode RootNode
        {
            get
            {
                return m_Root;
            }
        }

        private void readDataHeader(Stream pStream)
        {
            pStream.Seek(0, SeekOrigin.Begin);

            BinaryReader tReader = new BinaryReader(pStream);
            m_Flag = tReader.ReadUInt32();
            m_Version = tReader.ReadUInt32();
            m_IndexOffset = tReader.ReadUInt32();
        }

        private void writeDataHeader(Stream pStream)
        {
            pStream.Seek(0, SeekOrigin.Begin);

            BinaryWriter tWriter = new BinaryWriter(pStream);
            tWriter.Write(m_Flag);
            tWriter.Write(m_Version);
            tWriter.Write(m_IndexOffset);

            // 检查文件
            if (m_Flag != 0x00707266 || m_Version != 1)
                throw new NotSupportedException();
        }

        /// <summary>
        /// 保存包至
        /// </summary>
        /// <param name="Path">文件路径</param>
        public void Save(string FilePath)
        {
            string tOrgFile = null;

            if (FilePath == m_FilePath)
            {
                tOrgFile = FilePath;
                FilePath += ".temp";
            }

            FileStream tNewStream = null;

            // 最高权限打开流
            try
            {
                tNewStream = new FileStream(FilePath, FileMode.Create, FileAccess.ReadWrite, FileShare.Read);
            }
            catch (IOException e)
            {
                throw new FCYExcpSaveErr(e.Message);
            }

            m_FilePath = FilePath;

            // 写出头
            writeDataHeader(tNewStream);

            // 创建Xml数据
            XmlDocument tXmlDoc = new XmlDocument();
            XmlElement tRootElement = tXmlDoc.CreateElement("Res");

            // 收集所有数据
            RootNode.Sort();
            for (int i = 0; i < RootNode.NodeList.Count; i++)
            {
                RootNode.NodeList[i].WriteToStream(tNewStream, tRootElement);
            }

            // 写入Xml
            tXmlDoc.AppendChild(tRootElement);

            // 写入内存流
            MemoryStream tXmlMemStream = new MemoryStream();
            XmlWriterSettings tXmlSetting = new XmlWriterSettings();
            tXmlSetting.Encoding = Encoding.Unicode;// 设置格式
            tXmlSetting.OmitXmlDeclaration = true;  // 关闭声明头
            XmlWriter tXmlWriter = XmlWriter.Create(tXmlMemStream, tXmlSetting);

            tXmlDoc.Save(tXmlWriter);

            // 记录当前位置
            m_IndexOffset = (uint)tNewStream.Position;

            // 写出Xml原始长度
            BinaryWriter tWriter = new BinaryWriter(tNewStream);
            tWriter.Write((uint)tXmlMemStream.Length);

            // 压缩数据并保存到流
            tXmlMemStream.Seek(0, SeekOrigin.Begin);
            FCYResPackageHelper.DeflateEncodeTo(tXmlMemStream, tNewStream, (int)tXmlMemStream.Length);

            // 重写文件头
            writeDataHeader(tNewStream);

            // 关闭Xml内存
            tXmlWriter.Close();

            // 关闭原始流
            if (BaseStream != null)
                BaseStream.Close();

            if (m_BaseStream != null)
            {
                m_BaseStream.Close();
                m_BaseStream = null;
            }
            m_BaseStream = tNewStream;

            GC.Collect();

            if (tOrgFile != null)
            {
                File.Delete(tOrgFile);
                Save(tOrgFile);
                File.Delete(FilePath);
            }
        }

        public FCYResPackage()
        {
            m_Root = new FCYResPackageFloderNode(null, "");
        }

        /// <summary>
        /// 从文件创建
        /// </summary>
        /// <param name="Path">文件路径</param>
        public FCYResPackage(string Path)
        {
            m_BaseStream = null;
            try
            {
                m_BaseStream = new FileStream(Path, FileMode.Open, FileAccess.ReadWrite, FileShare.Read);
            }
            catch (IOException e)
            {
                throw new FCYExcpOpenErr(e.Message);
            }

            m_FilePath = Path;

            BinaryReader tReader = new BinaryReader(m_BaseStream);

            // 读取文件头
            readDataHeader(BaseStream);

            // 至索引区
            m_BaseStream.Seek(m_IndexOffset, SeekOrigin.Begin);

            // 读取Xml数据大小
            uint tXmlDataSize = tReader.ReadUInt32();

            // 读取Xml数据并解压
            MemoryStream tXmlMem = FCYResPackageHelper.DeflateDecode(m_BaseStream, (int)tXmlDataSize);

            // 创建Xml文档
            XmlDocument tXmlDoc = new XmlDocument();
            tXmlDoc.Load(tXmlMem);

            // 读取数据
            m_Root = new FCYResPackageFloderNode(this, null, (XmlElement)tXmlDoc.SelectSingleNode("Res"));

            // 关闭Xml数据
            tXmlMem.Close();
        }

        ~FCYResPackage()
        {
            if (m_BaseStream != null)
            {
                m_BaseStream.Close();
                m_BaseStream = null;
            }
        }
    }
}
