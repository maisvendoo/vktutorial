# Настройка проекта

Все приведенные в данном руководстве примеры кода будут разрабатываться с учетом возможности их сборки в операционных системах Windows и Linux, однако, акцент пока будет сделан на ОС семейства Windows, так как работа в этой среде вызывает наибольшую "боль". Поэтому, для сборки нашего первого приложения на Vulkan, мы будем, первоначально использовать следующий перечень программного обеспечения

+ Компилятор языков C/C++ поддержкой стандарта не ниже C++20, так как некоторые возможности этого стандарта мы будем использовать в нашем учебном проекте. Рекомендуется компилятор  [MinGW](https://www.mingw-w64.org/) версии не ниже 13.1.0 для 64-битный систем. Неофициальный дистрибутив этого компилятора можно скачать например, [здесь](https://cloud.mail.ru/public/epKs/mT6pueitf).
+ Система сборки CMake, дистрибутив которой [загружается с официального сайта](https://cmake.org/).
+ Официальный [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/) актуальной версии.

## Установка компилятора MinGW и системы сборки CMake

Скачанный архив распаковываем в произвольное место файловой системы. Далее, определяем переменную окружения %MINGW_DIR% указывая путь к каталогу компилятора

![](https://habrastorage.org/webt/hm/id/3q/hmid3qg5fhmxho3nxvdpdhnb9v8.png)

Кроме того, добавляем путь к бинарникам компилятора в переменную %PATH%

![](https://habrastorage.org/webt/iq/jx/jp/iqjxjpr1bi30pnq3fsjrs2nx13e.png)

Для удобства запуска консоли компилятора в каталоге %MINGW_DIR%\bin\ создан ярлык MinGW 11.2.0 64-bit.ink, который можно запустить непосредственно или закрепить на рабочем столе или панели задач. При запуске этого ярлыка должна открыться консоль. Проверяем версию компилятора, введя команду

```E:\Apps\mingw-13.1.0\bin> gcc --version```

![](https://habrastorage.org/webt/z9/--/nt/z9--ntob5fvvot2bgnxddsawapa.png)

Дистрибутив CMake содержит стандартный инсталятор для ОС Windows, типа "далее-далее", поэтому никаких проблем с установкой не возникает. При установке CMake настоятельно рекомендуется установить опцию «Add CMake to system PATH for all users», для доступа к системе сборки из консоли

![](https://habrastorage.org/webt/mk/wt/96/mkwt96or8nyce5r0iumns71p6fi.png)


## Установка Vulkan SDK

Для написания приложения, использующего Vulkan API требуется установить текущую версию Vulkan SDK, доступный на [официальной странице загрузки LunarG](https://vulkan.lunarg.com/sdk/home#windows). Следует скачать и установить актуальную версию

![](https://habrastorage.org/webt/z2/ec/ol/z2ecol2k50htqsyy-_24gjxfho0.png)

После установки следует убедиться в том, что путь к Vulkan SDK присвоен системной переменной окружения %VK_SDK_PATH%. Если этого, по каким-то причинам, не произошло, следуют вручную добавить данную переменную, указав путь к установленному SDK, как показано на рисунке ниже

![](https://habrastorage.org/webt/v3/bf/bu/v3bfbuc04hlsky6iyazxoegjuus.png)

Следует также убедится в корректной установке следующих переменных окружения

+ %Vulkan_GLSLANG_VALIDATOR_EXECUTABLE% — путь к исполняемому файлу компилятора шейдеров glslangValidator.exe
+ %Vulkan_GLSLC_EXECUTABLE% — путь к исполняемому файлу компилятора шейдеров glslc.exe
+ %Vulkan_INCLUDE_DIR% — путь к заголовочным файлам Vulkan API
+ %Vulkan_LIBRARY% — путь к статической библиотеке vulkan-1.lib
+ %VULKAN_SDK% — путь к корневому каталогу Vulkan SDK

Значения этих переменных следует привести в соответствие с фактическим расположением SDK на вашем компьютере

## Структура проекта

Наш учебный проект будет иметь следующую структуру каталогов

```
vktutorial-
          |
          -bin
          |
          -build-win64-debug
          |
          -build-win64-release
          |
          -vktutorial-
                     |
                     - docs
                     |
                     - vkengine-
                     |         |
                     |         - include-
                     |		   |        |
                     |         |        - main.h
                     |         |
                     |         - src-
                     |         |    |
                     |         |    - main.cpp   
                     |         |
                     |         -CMakeLists.txt
                     |
                     -CMakeLists.txt
                     |
                     - .gitignore
                     |
                     - .gitattributes
```


+ Каталог bin/ предназначен для размещения собранного исполняемого файла
+ Каталоги build-win64-debug/ и build-win64-release/ - каталоги сборки CMake для отладочной и релизной версий
+ Каталог docs/ содержит файлы данного руководства в формате разметки Markdown
+ vkengine/ - содержит сиходный код основного проекта
+ каталоги include/ и src/ - заголовочные и исходные файлы основного проекта


Пишем код пока еще пустой функции main() нашей будущей учебной программы в файле **main.cpp**

```cpp
#include	<main.h>

int main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;
	
	return 0;
}
```
и заголовочный файл **main.h** который будет содержать включения всех необходимых заголовков.

```cpp
#ifndef		MAIN_H
#define		MAIN_H

#endif
```


## Настройка сборки проекта


Для сборки проекта напишем два сценария CMakeLists.txt. Одни для vkengine

```cmake
# Минимально необходимая версия CMake
cmake_minimum_required (VERSION 3.7)

# Устанавливаем переменную TARGET как имя исполняемого файла
set (TARGET vkengine)

# Задаем имя проекта
project (${TARGET})

# Включаем требование соответствие стандарту C++20
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Задаем путь, по которому будет расположен исполняемый файл
set (EXECUTABLE_OUTPUT_PATH "../../bin/")

# Указываем шаблон поиска заголовочных и исходных файлов
file (GLOB INCLUDES "./include/*.h")
file (GLOB SOURCES "./src/*.cpp")

# Ищем в системе установленный Vulkan SDK
find_package (Vulkan REQUIRED)

# Задаем сборку исполняемого файла
add_executable (${TARGET} ${SOURCES} ${INCLUDES})

# Указыаем каталог поиска заголовочных файлов
target_include_directories (${TARGET} PRIVATE ./include/)

# Компонуем исполняемый файл с библиотеками Vulkan
target_link_libraries (${TARGET} PRIVATE Vulkan::Vulkan)
```

другой для сборки vkengine и других подпроектов, находящихся с ним на одном уровне иерархии

```cmake
cmake_minimum_required (VERSION 3.7)

project (vktutorial)

add_subdirectory (vkengine)
```

Проверим сборку проекта, для чего запустим консоль компилятора, перейдем в каталог vktutorial/build-win64-release/ и выполним команду

```
cmake -G "MinGW Makefiles" ..\vktutorial
```

Если все выполнено верно, получаем выхлоп

```
-- The C compiler identification is GNU 13.1.0
-- The CXX compiler identification is GNU 13.1.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: E:/Apps/mingw-13.1.0/bin/gcc.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: E:/Apps/mingw-13.1.0/bin/c++.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Found Vulkan: C:/VulkanSDK/1.3.283.0/Lib/vulkan-1.lib (found version "1.3.283") found components: glslc glslangValidator
-- Configuring done (1.4s)
-- Generating done (0.0s)
-- Build files have been written to: D:/work/Projects/vktutorial/build-win64-release
```

CMake сообщает нам что он нашел библиотеку Vulkan и компиляторы шейдеров. После этого можно приступить непосредственно к сборке командой

```
mingw32-make -j8
```

ключем -j указав число потоков процессора. 

```
D:\work\Projects\vktutorial\build-win64-release>mingw32-make -j8
[ 50%] Building CXX object vkengine/CMakeFiles/vkengine.dir/src/main.cpp.obj
[100%] Linking CXX executable D:\work\Projects\vktutorial\bin\vkengine.exe
[100%] Built target vkengine
```

В каталоге vktutorial/bin должен образоваться файл vkengine.exe. Если это получилось, то можно считать, что всё готова к работе с Vulkan API



