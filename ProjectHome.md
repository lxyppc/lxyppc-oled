# OLed点阵显示 #
用分辨率为128X64的OLed屏幕做了个小东西，用锂电池供电，可以通过USB电缆充电
可以通过USB用项目中的HidLoader来下载新的程序在里面。
里面有一个加速度传芯片MMA7260，一个锂电池管理芯片BQ24073。
画了个时钟出来，还没想好要做什么东西。

演示视频地址
http://v.youku.com/v_show/id_XMTU2NTYwNTQ0.html

# A project for the oled dot matrix display #
The screen is a oled with 128X64 pixels. Powered by Li-Ion battary, re-chargable through the USB cable.
The firmware can update through USB HID device by HidLoader within the project.
Just implement a clock in it right row.


预览/Preview:
装在外壳里的样子

http://lxyppc-oled.googlecode.com/svn/trunk/%20lxyppc-oled/preview/1.JPG

拆开之后

http://lxyppc-oled.googlecode.com/svn/trunk/%20lxyppc-oled/preview/2.JPG

显示效果

http://lxyppc-oled.googlecode.com/svn/trunk/%20lxyppc-oled/preview/4.JPG

多余的PCB

![http://lxyppc-oled.googlecode.com/svn/trunk/%20lxyppc-oled/preview/3.jpg](http://lxyppc-oled.googlecode.com/svn/trunk/%20lxyppc-oled/preview/3.jpg)

原图下载
http://lxyppc-oled.googlecode.com/files/OLedPic.zip

# 新增功能 New Feature #
可以在红牛开发板上调试UI，UI的数据通过USB摄像头发送上来
When debug on the ReadBull develop kit, display on Oled is send to PC as a USB camera

http://lxyppc-oled.googlecode.com/svn/trunk/%20lxyppc-oled/preview/UIDebug.JPG