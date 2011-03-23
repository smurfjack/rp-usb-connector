简介:
--------------------------------

RoboPeak USB Connector是由RoboPeak团队(www.RoboPeak.com)开发的供AVR/C51单片机使用的USB免驱动ISP编程器。

RoboPeak USB Connector基于AVR-Doper项目(http://www.obdev.at/products/vusb/avrdoper.html)，它是一款兼容STK500的ISP编程器和高压编程器(HVSP). 

USB的通讯能力是通过v-usb库(以纯软件模拟实现基于AVR单片机的低速USB通讯)。 在RoboPeak USB Connector，我们仅使用了一块AVR芯片。

RoboPeak USB Connector可以认为是AVR-Doper的简化版本，除此以外，我们使用Arduino-Lite固件库重写了RoboPeak USB Connector的固件，并且采用Arduino-Lite的编译系统。


支持的操作系统:
---------------------------------
理论上说，任何内建了HID usb驱动程序的操作系统均可工作。

因为PC的客户端程序(avrdude.exe)以及可以在以下操作系统上工作，因此RoboPeak USB Connector应当也支持这些操作系统:

<> Linux with libusb
<> Windows from the version WinXP
<> MacOS

我们已经验证了如下的Windows操作系统版本:
<> Windows XP
<> Windows Vista, 32位版 和 64位版
<> Windows 7, 32位版 和 64位版


如何编译固件?
---------------------------------

RoboPeak USB Connector需要使用Arduino-Lite来编译产生hex文件。请参考在firmware目录下的文件(请先读我!.txt)了解详细过程。

在固件编译完成后，请使用make burnall命令对一块新的RP USB Connector进行编程烧录。该动作将会把编译产生的hex文档以及BootloaderHID烧录至芯片并配置正确的熔丝位。

如何制作硬件?
---------------------------------

在hardware目录下包含了RoboPeak USB Connector的原理图。PCB的布局文档不久将会发布，您可以先参考目录referece_layout/下的文档。
另外, AVR-Doper应当可以直接使用我们的固件。


如何给新制作的RoboPeak USB Connector编程?
---------------------------------
你可以使用Arduino-Lite并用make burnall命令来完成对新制作的RoboPeak USB Connector的编程。该命令将会把固件hex文件以及BootLoaderHID的固件下载到目标AVR芯片。并且目标芯片的熔丝也会自动得到配置。

也可以人工对RoboPeak USB Connector进行编程，使用如下的熔丝设置：

// 仅用于Atmega88
LFUSE=CE
HFUSE=DD
EFUSE=00

您也需要去烧录BootloaderHID 固件，他的hex文件可以在Arduino-Lite项目中找到( <Arduino-Lite/src/bootloaders/hidbootloader/pre-compiled/mega88_12M.hex)

如何使用?
---------------------------------
您可以参考AVR-doper项目网站了解他的一般使用过程:
  http://www.obdev.at/products/vusb/avrdoper.html

我们推荐您结合Arduino-Lite来使用RoboPeak USB Connector，因为Arduino-Lite的编译系统内建了对RoboPeak USB Connector(AVR-Doper)的使用支持。

使用Arduino-Lite的时候，可以用"make burn"命令轻松对AVR芯片进行编程烧录。

你可以参考一段使用Arduino-Lite通过RoboPeak USB Connector烧录程序的视频短片:
  http://www.tudou.com/programs/view/Den9uh3HTHE/


自我编程?
---------------------------------
RoboPeak USB Connector支持自我编程能力。在固件中包含了一个免驱动的USB Bootloader，BootloadHID (http://www.obdev.at/products/vusb/bootloadhid.html).

在某些情况下，例如您需要更新RoboPeak USB Connector的固件，但却只有一套RoboPeak USB Connector设备，此时，自我编程能力就很有用处。

请按照如下步骤进行自我编程:
<> 连接位于P_ADC排针上的ADC0(PC1)和VCC引脚。
<> 将RoboPeak USB Connector连接至PC。
<> 此时，操作系统应当能提示有一个名为HIDBoot的设备接入了。
<> 进入RoboPeak USB Connector的固件工程目录, 开启用于编译的bash shell终端, 输入并执行 
   make usbupload


参考资源:
---------------------------------
本项目主页:             http://code.google.com/p/rp-usb-connector/
Arduino-Lite 项目主页:  http://code.google.com/p/arduino-lite/
RoboPeak 团队:          http://www.RoboPeak.com
AVR-Doper:              http://www.obdev.at/products/vusb/avrdoper.html
