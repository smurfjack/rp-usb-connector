![http://www.robopeak.net/blog/wp-content/uploads/2011/03/release_ver.jpg](http://www.robopeak.net/blog/wp-content/uploads/2011/03/release_ver.jpg)

[中文版本在本页下方：]

RoboPeak USB Connector is a driverless USB ISP programmer for AVR/C51 MCU. It is developed by RoboPeak team).

RoboPeak USB Connector is based on the work of AVR-Doper project (http://www.obdev.at/products/vusb/avrdoper.html), which is a STK500 compatible In-System Programmer (ISP) and High Voltage Serial Programmer (HVSP).

The USB connectivity is achieved using the v-usb lib (a software-only implementation of a low-speed USB device for Atmel's AVR microcontrollers). We only use a simple AVR MCU in the circuit.

The RoboPeak USB Connector can be considered as a simplified version of AVR-Doper, besides, the firmware has been written using Arduino-Lite firmware libarary and the building system.

### Supported Operating Systems: ###


In theory, any OS with build-in HID usb drivers is supported.

As the PC-side client (avrdude.exe) can work on the following systems, RoboPeak USB Connector should work for these systems as well:

  * Linux with libusb
  * Windows from the version WinXP
  * MacOS

We had verified the following Windows Versions:
  * Windows XP
  * Windows Vista, both 32bit and 64 bit versions
  * Windows 7, both 32bit and 64 bit versions


---

RoboPeak USB Connector是由RoboPeak团队开发的供AVR/C51单片机使用的USB免驱动ISP编程器。

RoboPeak USB Connector基于AVR-Doper项目(http://www.obdev.at/products/vusb/avrdoper.html )，它是一款兼容STK500的ISP编程器和高压编程器(HVSP).

USB的通讯能力是通过v-usb库(以纯软件模拟实现基于AVR单片机的低速USB通讯)。 在RoboPeak USB Connector，我们仅使用了一块AVR芯片。

RoboPeak USB Connector可以认为是AVR-Doper的简化版本，除此以外，我们使用Arduino-Lite固件库重写了RoboPeak USB Connector的固件，并且采用Arduino-Lite的编译系统。


### 支持的操作系统: ###

理论上说，任何内建了HID usb驱动程序的操作系统均可工作。

因为PC的客户端程序(avrdude.exe)以及可以在以下操作系统上工作，因此RoboPeak USB Connector应当也支持这些操作系统:

  * Linux with libusb
  * Windows from the version WinXP
  * MacOS

我们已经验证了如下的Windows操作系统版本:
  * Windows XP
  * Windows Vista, 32位版 和 64位版
  * Windows 7, 32位版 和 64位版