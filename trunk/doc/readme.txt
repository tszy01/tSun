Tangsoft Sun Foundation Class Library

======================================================================================================

开发IDE: Microsoft Visual Studio 2015
开发操作系统: Microsoft Windows 10

依赖外部库：
tSun -- Tangsoft Sun Foundation Class Library
FreeImage -- 图片资源库 3.15.1
FreeType -- 文字渲染库 2.1.9
lua -- lua script
tinyxml -- xml library
zlib -- zlib library
3DMax 2013 SDK -- 3DMAX开发包
LibDDSTextureLoader -- DX示例的代码，读取DDS

======================================================================================================

SunBase

Singleton<T>:单例模式,需要子类继承使用
TSCommonTypeDef.h -- 定义一些基本
TSConfigDef.h -- 全局配置定义
TSCommonFunc.h -- 全局函数

TSMemDef.h -- 定义内存的申请释放
MemAllocator -- 内存管理类
Map<T>:一个<Key,Value>对的链表
List<T>:一个双向链表
Pair<T>:Map用键值对
注意:今后在设计到以上内存数据结构的时候,尽量使用这些结构,或者STL的结构,不要自己再另外做处理了

DateTime:日期时间操作
AutoPtr<T>:智能指针
String:字符串类
WString:宽字节字符串
SharePtr<T>：引用计数智能指针
Mutex：互斥信号
Lock：互斥锁

======================================================================================================

SunUtils

ConfigFile:简单的值对文件操作类
MemFile:内存文件映射
Log:日志系统
TxtFileReader:用于读取TXT文本文件的数据
TxtFileWritter:用于写入TXT文本文件的数据
UTF8TxtFileReader:用于读取UTF8-TXT文本文件的数据
UTF8TxtFileWritter:用于写入UTF8-TXT文本文件的数据

BinaryFileProcessor -- 二进制文件读写处理
Console -- 控制台系统
ConsoleInput -- 处理控制台输入
ConsoleOutput -- 处理控制台输出
LangDict -- 多语言的统一化支持
Timer -- 时钟信息获取

LuaBase -- 用于处理lua脚本
Zip -- 用于处理压缩
XMLBase -- 用于处理XML读写

======================================================================================================

SunMath

TLFPU.h:全局处理函数
TLMathUtils.h:全局函数
Vector4<T>:4d向量
Vector3<T>:3d向量
Vector2<T>:2d向量
Matrix3x3<T>:3X3矩阵
Matrix4x4<T>:4X4矩阵
Ray<T>:射线类
Radian<T>:弧度
Degree<T>:角度
Line<T>:线段类
Triangle<T>:三角形
Plane<T>:平面
PlaneBoundedVolume<T>:平面围成的体积
Polygon<T>:多边形
AABBbox<T>:AABB包围盒
Sphere<T>:包围球
Quaternion<T>:四元数类
DualQuaternion<T>:双四元数
RotationalSpline<T>:旋转样条曲线
SimpleSpline<T>:样条曲线
ConvexBody<T>:凸面体
ColorValue<T>:颜色
注:
1、所有TMath涉及到的坐标系,均以左手坐标系为准(X向右,Y向上,Z向里)
2、所有向量为横向量,3D向量点W=1,射线W=0
4、角度均以逆时针为正,默认范围0-360度,沿坐标轴正方向为0度
5、所有对外接口的角度必须使用角度,在内部转成弧度
6、没有向量*矩阵的直接重载,需要调用转换函数才能转换向量
7、矩阵对向量的转换在TMatrix4x4<T>中
8、其它图元的转换均放在自己的类里实现

======================================================================================================

关于压缩
Dependence:zlib,1.2.3
Zip:压缩和解压缩相关操作
Zip压缩后文件格式:
OrigSize<int> 压缩之前的文件总大小,用于制造缓冲区
-->FileNum<int> 文件总数
{
-->NameSize<int> 文件名长度
-->Name<char[]> 文件名(不包括原始路径)
-->ContentSize<int> 内容大小(字节)
-->Content<byte[]> 文件内容
}
注:-->表示是压缩之后的数据,<>表示空间大小

lua脚本部分
Dependence:lua,5.1
LuaBase:Lua脚本解析基础类

XML部分
请直接使用tinyxml处理xml文档

======================================================================================================

目录说明

文件索引：
resource -- 资源目录
doc -- 开发项目相关文档
src -- 代码
bin -- 生成目录
lib -- 依赖库生成目录
Intermediate -- 中间文件目录

config：各种配置文件
log：日志文件
lang_dict：字典文件

======================================================================================================

Sample_Window
ConsoleWindow -- 控制台窗体
LuaInitApp -- 初始化lua脚本
GlobleClass -- 全局对象管理
ConfigDef.h -- 编译宏定义等
Main.cpp -- 入口
XMLResTest -- xml读写测试
MemTest -- 用于内存管理模块的测试

运行参数
-initfile [filename]指定初始化脚本文件，默认为运行目录中的initApp.lua
-test 执行测试分支