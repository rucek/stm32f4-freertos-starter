# STM32F429 + FreeRTOS 8.2 starter

Based on Eclipse "New STM32F4xx C/C++ project" wizard + FreeRTOS 8.2 sources.

The original application, based on [this great tutorial](https://istarc.wordpress.com/2014/07/10/stm32f4-deploy-freertos-in-under-10-seconds/),  should blink the green LED (*PG13*) with a period of 1.5s and toggle the red one (*PG14*) when the blue button (*USER*) is pressed. This happens in three separate FreeRTOS tasks: first one for the green LED, second one for detecting button presses and putting them in a queue and the third one for consuming the queued events and toggling the red LED.

## Prerequisites

### For OS X (tested on Yosemite):
- [gcc-arm-none-eabi](https://launchpad.net/gcc-arm-embedded/4.9/4.9-2014-q4-major/+download/gcc-arm-none-eabi-4_9-2014q4-20141203-mac.tar.bz2)

- [openocd](http://sourceforge.net/projects/gnuarmeclipse/files/OpenOCD/gnuarmeclipse-openocd-osx-0.8.0-2-201501311629.pkg/download)

- [Eclipse IDE for C/C++ Developers](http://www.eclipse.org/downloads/download.php?file=/technology/epp/downloads/release/luna/SR1/eclipse-cpp-luna-SR1-macosx-cocoa-x86_64.tar.gz) with:
  - all plugins from the http://gnuarmeclipse.sourceforge.net/updates update site
  - the *C/C++ GCC Cross Compiler Support* and *C/C++ GDB Hardware Debugging* plugins from http://download.eclipse.org/tools/cdt/releases/8.5

## Setup

1. Clone this repository :)

2. Create a project in Eclipse:
  - go to *File* / *New* / *C Project*
  - choose *Executable* / *New STM32F4xx C/C++ project*
  - use *stm32f4-freertos-starter* for the *Project name*
  - uncheck *Use default location* and choose the location where you cloned this repository, then click *Next*
  - in the *Chip family* dropdown, choose *STM32F429xx*
  - in the *Content* dropdown, choose *Empty (add your own content)*
  - in the *Trace output* dropdown, choose *Semihosting DEBUG channel*, then click *Next* until the *Cross GNU ARM toolchain* screen
  - choose the *GNU Tools for ARM Embedded Processors (arm-none-eabi-gcc)* in the *Toolchain name* dropdown
  - in the *Toolchain path*, give the path to the `bin` folder of your *gcc-arm-none-eabi* installation
  - configure build settings - go to project properties / *C/C++ Build* / *Settings*.

    Under *Target processor*:
      - check the *Thumb interwork (-mthumb-interwork)* checkbox
      - in *Endianness* select *Little endian (-mlittle-endian)*
      - in *Float ABI* select *FP instructions (hard)*
      - in *FPU type* select *fpv4-sp-d16*

    Under *Cross ARM C Compiler* / *Includes* add the following to the *Include paths*:
      - `../include/FreeRTOS/Source/include`
      - `../include/FreeRTOS/Source/portable/GCC/ARM_CM4F`

4. Build the project to check if everything works.

3. Create an OpenOCD debug configuration as described in this  [tutorial](http://gnuarmeclipse.livius.net/blog/openocd-debugging/).

  **NOTE:** on the OpenOCD Debugger tab, the value of *Other options* (which is called *Config options* in Eclipse Luna) should be `-f board/stm32f429discovery.cfg` instead of what the tutorial suggests.

  Use the created configuration to deploy the application to the board and debug it.

## Credits
- [@grajo](https://twitter.com/grajo)
- https://istarc.wordpress.com/2014/07/10/stm32f4-deploy-freertos-in-under-10-seconds/
- http://grafixmafia.net/updated-using-the-stm32f4-discovery-board-with-mac-osx-10-9-mavericks/
- http://gnuarmeclipse.livius.net/blog/openocd-debugging/
