# bDLL
自动化dll劫持测试工具。Automated dll hijacking testing tool
因为在aDLL的基础上弄的，干脆就叫bDLL吧

调用aDLL工具，修改其payload（informer.dll）和部分代码，批量检测指定目录的exe是否可以dll劫持

> https://github.com/ideaslocas/aDLL

v0.1.0的里面，有个自己好久之前找的arch.exe 现在vt上报毒4个，不知道咋回事🤷‍♂️，
v0.2.0的找了个开源的加进去了，进去尽量在虚拟机运行

## usage:

```
python DllJacking_Python.py 目标文件夹
```

思路：

1. 目标文件夹所有签名过的exe文件复制到当前exe目录下
2. 调用aDLL检测目标，加载修改过的dll文件
3. dll会在当前目录下生成“dlljacking.txt”，并把可劫持的exe复制到goodexe目录下
4. 根据检测前后“dlljacking.txt”的大小，来判断是否有新的成果
5. 整理“dlljacking.txt”的内容输出为“result.txt”

编译后，按以下结构放置：
```
│-bDLL.py
│
└─bin
        aDLL32.exe
        aDLL64.exe
        arch.exe
        Hook32.dll
        Hook64.dll
        informer32.dll
        informer64.dll
```
## Log
### v0.2.0
替换arch模块

### v0.2.1
Add Deduplication result
添加结果去重功能
