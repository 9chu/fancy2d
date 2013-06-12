import re
import os

# .h文件搜索环境
global_h_path = []

# 已分析的h文件
global_h = []

# 要删除的目录中的所有.h文件
all_h = []

# 返回正则匹配结果
def OpenRE(pageText, MatchText):
    rexPat = re.compile(MatchText, re.IGNORECASE)
    findResult = rexPat.findall(pageText)
    return findResult

# 递归遍历目录
def RecurseFloder(dirname):
    global all_h
    dirname = os.path.abspath(dirname)
    files = os.listdir(dirname)
    for file in files :
        if(os.path.isdir(dirname + "\\" + file)) :
            RecurseFloder(dirname + "\\" + file)
        else :
            name = os.path.splitext(file)
            if(name[1] == ".h") :
                filestr = dirname + "\\" + file
                if(not filestr in all_h):
                    all_h.append(filestr)

# 分析文件的包含项
def AnalyzeFile(filefullpath):
    # 跳过已分析的文件
    if(filefullpath.lower() in global_h):
        return
    
    pathdata = os.path.split(filefullpath)
    parentdir = pathdata[0]
    filename = pathdata[1]
    
    parentdir = os.path.abspath(parentdir)
    if(not os.path.exists(parentdir + "\\" + filename)):
        global global_h_path

        searched = False
        for tmppath in global_h_path :
            if(os.path.exists(tmppath + "\\" + filename)):
                parentdir = tmppath
                searched = True
                break
        if(not searched) :
            return

    # 记录已分析的文件
    global_h.append((parentdir + "\\" + filename).lower())
    print(parentdir + "\\" + filename)

    # 打开文件并读取数据
    file = open(parentdir + "\\" + filename)
    filetext = ""
    try:
        filetext = file.read()
    finally:
        file.close()

    # 正则表达式
    retdata = OpenRE(filetext, r'#include <(.+?)>')
    retdata += OpenRE(filetext, r'#include "(.+?)"')
    for hfile in retdata:
        fullpath = parentdir + "\\" + hfile
        fullpath = os.path.abspath(fullpath)
        print(fullpath)
        AnalyzeFile(fullpath)

# 导入文件
def ImportHFile(filename):
    filename = os.path.abspath(filename)
    pathdata = os.path.split(filename)
    parentdir = pathdata[0]
    filename = pathdata[1]

    RecurseFloder(parentdir)
    
    print("被 %s 引用的头文件（含该文件在内）："%(parentdir+"\\"+filename))
    AnalyzeFile(parentdir+"\\"+filename)

# 重命名所有未使用的头文件
def Rename() :
    for file in all_h:
        if(not (file.lower()) in global_h):
            os.rename(file, file + ".unused")

# h文件搜索路径
def SetHFloder(dir):
    global global_h_path
    global_h_path.append(os.path.abspath(dir))
    
SetHFloder(r"E:\Projects\fancy2D\tools\FMM_3DSMAX_ExportPlugin\extern\maxsdk2010\include")

ImportHFile(r"E:\Projects\fancy2D\tools\FMM_3DSMAX_ExportPlugin\extern\maxsdk2010\include\max.h")
ImportHFile(r"E:\Projects\fancy2D\tools\FMM_3DSMAX_ExportPlugin\extern\maxsdk2010\include\istdplug.h")
ImportHFile(r"E:\Projects\fancy2D\tools\FMM_3DSMAX_ExportPlugin\extern\maxsdk2010\include\iparamb2.h")
ImportHFile(r"E:\Projects\fancy2D\tools\FMM_3DSMAX_ExportPlugin\extern\maxsdk2010\include\iparamm2.h")
ImportHFile(r"E:\Projects\fancy2D\tools\FMM_3DSMAX_ExportPlugin\extern\maxsdk2010\include\IGame\IGame.h")
ImportHFile(r"E:\Projects\fancy2D\tools\FMM_3DSMAX_ExportPlugin\extern\maxsdk2010\include\IGame\IGameError.h")

Rename()
