# -*- coding: utf-8 -*-
import os
import re
import sys
import time
import pefile
import shutil
import subprocess

result  = r"result.txt"
temp    = r"dlljacking.txt"
adllx86 = r"bin\adll32.exe"
adllx64 = r"bin\adll64.exe"
archexe = r"bin\arch.exe "

# 拷贝所有exe到指定目录
def copyexe(dirpath):
    listexe(dirpath)
    print("[+] Copy cer_exe from "+dirpath+" to exe\\")
    for filepath in filepaths:
        if is_cer(filepath):
            shutil.copyfile(filepath,"exe\\"+os.path.basename(filepath)) 

# 列出指定目录所有exe文件
def listexe(dirpath):
    print("[+] List exe in "+dirpath)
    filepaths.clear()
    for root,dirs,files in os.walk(dirpath):
      for file in files:
          if os.path.splitext(file)[1] == '.exe':
             filepaths.append(os.path.join(root,file))

# 判断PE是否有签名
def is_cer(PEfile_Path):
    pe = pefile.PE(PEfile_Path)
    IMAGE_DIRECTORY_ENTRY_SECURITY = str(pe.OPTIONAL_HEADER.DATA_DIRECTORY[4])
    pattern1 = r'VirtualAddress:\s+0x0\b'
    pattern2 = r'Size:\s+0x0\b'
    if (re.search(pattern1, IMAGE_DIRECTORY_ENTRY_SECURITY) is None and re.search(pattern2, IMAGE_DIRECTORY_ENTRY_SECURITY) is None):
        return True

# 利用aDll工具进行检测
# https://github.com/ideaslocas/aDLL
def adll(arch, exepath):
    if arch == 86:
        command = adllx86 + " -e "+exepath+" -a "
    elif arch == 64:
        command = adllx64 + " -e "+exepath+" -a "

    # print(command)
    os.system(command)

# 判断文件架构
# 返回 Arch=x86 | Arch=x64
def arch(Exepath):
    rc,out = subprocess.getstatusoutput(archexe+Exepath)
    return(out)

# 整理结果
def add_arch(txtpath):
    Deduplication(txtpath)
    print("\r\nFormating result")
    with open(result, 'w+') as File_Result:
        with open(txtpath,'r',encoding='utf-8') as File_Temp:
            for line in File_Temp:
                # print(line)
                if "exe: " in line:
                    goodexepath = line.strip("\t\r\n").replace("exe: ","")
                    line = line.strip("\t\r\n")+" ("+arch(goodexepath)+")"
                    # print(line)
                File_Result.write(line)
        File_Temp.close()
    File_Result.close()
    os.remove(txtpath)

# 去重复数据
def Deduplication(txtpath):
    print("\r\n[+] Good job.Deduplication result")
    Temp_Result = []
    Str_temp = open(txtpath,'r',encoding='utf-8').read()
    Str_temp = Str_temp.split("\n\n\n\n")
    for line in Str_temp:
        if line not in Temp_Result:
            Temp_Result.append(line)
    os.remove(txtpath)

    with open(txtpath, 'w+') as File_Temp2:
        for line in Temp_Result:
            File_Temp2.write(line+"\n\n")
    File_Temp2.close()

if __name__ == '__main__':
    # os.chdir("bin\\")
    filepaths = []
    try:
        dirpath = sys.argv[1]
    except(Exception):
        print("""Usage:
            file.py DirPath""")
        sys.exit(0)

    # 建立相应文件夹
    if not os.path.exists("exe"):
        print("[+] Make dir \"exe\"")
        os.makedirs("exe")
    if not os.path.exists("goodexe"):
        print("[+] Make dir \"goodexe\"")
        os.makedirs("goodexe")

    # 复制目标exe到缓存目录，获得缓存目录的exe文件绝对路径
    copyexe(dirpath)
    listexe("exe\\")
    # print(filepaths)

    # 获取结果文件的大小
    fsize = os.path.getsize(temp) if  os.path.exists(temp) else 0
    # print(fsize)

    # 判断exe的架构执行检测
    for filepath in filepaths:
        out = arch(filepath)
        if out == "Arch=x86":
            # print("x86")
            adll(86, filepath)
        elif out == "Arch=x64":
            # print("x64")
            adll(64, filepath)
        else:
            print("[!]"+out+" "+filepath)

    # 显示结果，大小变大说明有写入
    fsize2 = os.path.getsize(temp) if  os.path.exists(temp) else 0
    if fsize2-fsize :
        add_arch(temp)
        os.system("type "+result)
    else:
        print("\r\n[!] not good")
    time.sleep(5)
    shutil.rmtree(r"exe")
