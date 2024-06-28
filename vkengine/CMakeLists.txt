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
