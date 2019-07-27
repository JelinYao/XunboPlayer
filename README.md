# XunboPlayer
一个开放源码的Windows视频播放器  

界面使用Duilib库：  
视频播放使用迅雷播放器解码库：http://aplayer.open.xunlei.com/  

编译：  
                项目使用VS2008开发，目前电脑上安装的是VS2017所以转成了17的工程文件，可以自行将其改成08、10、12、13、15项目。  
        VS2017编译会报错，链接不到atlsd.lib，项目设置中忽略掉这个库就可以了。  
        打开sln工程文件，编译即可。  

运行：  
        依赖迅雷的播放器组件SDK，在bin目录下已经压缩成了SDK.7z，拷贝到Release下面 “解压到当前目录”。  
        然后就可以播放视频了，支持云播放（可以传入一个适配地址的url）^_^  

扩展：  
    很多基本功能没有，UI都是从射手影音里面拿出来的，实在太麻烦了。  
    循环播放、快进、快退、置顶、截图……这些基本功能都是很好扩展的，自由发挥哈  

建议：      
    2017年的项目，很久没有看这些代码了，能力有限，欢迎指正探讨。  

运行截图：  

![Image](https://raw.githubusercontent.com/JelinYao/XunboPlayer/master/img/1.png)

![Image](https://raw.githubusercontent.com/JelinYao/XunboPlayer/master/img/2.png)

![Image](https://raw.githubusercontent.com/JelinYao/XunboPlayer/master/img/3.png)

![Image](https://raw.githubusercontent.com/JelinYao/XunboPlayer/master/img/4.png)

![Image](https://raw.githubusercontent.com/JelinYao/XunboPlayer/master/img/5.png)

![Image](https://raw.githubusercontent.com/JelinYao/XunboPlayer/master/img/6.png)

![Image](https://raw.githubusercontent.com/JelinYao/XunboPlayer/master/img/7.png)

![Image](https://raw.githubusercontent.com/JelinYao/XunboPlayer/master/img/8.png)

![Image](https://raw.githubusercontent.com/JelinYao/XunboPlayer/master/img/9.png)

![Image](https://raw.githubusercontent.com/JelinYao/XunboPlayer/master/img/10.png)
