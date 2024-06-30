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
set (SPV_SHADERS_PATH "../../shaders/")

# Указываем шаблон поиска заголовочных и исходных файлов
file (GLOB INCLUDES "./include/*.h")
file (GLOB SOURCES "./src/*.cpp")
file (GLOB VERT_SHADERS "./shaders/*.vert")
file (GLOB FRAG_SHADERS "./shaders/*.frag")

# Ищем в системе установленные зависимости
find_package (Vulkan REQUIRED)
find_package (SDL2 REQUIRED)

# Задаем сборку исполняемого файла
add_executable (${TARGET} ${SOURCES} ${INCLUDES})

# Указыаем каталог поиска заголовочных файлов
target_include_directories (${TARGET} PRIVATE ./include/)

# Компонуем исполняемый файл с библиотеками зависимостей
target_link_libraries (${TARGET} PRIVATE Vulkan::Vulkan)
target_link_libraries (${TARGET} PRIVATE SDL2::SDL2)

# Функция для компиляции шейдеров
function (add_shaders TARGET_NAME)

    set (SHADER_SOURCE_FILES ${ARGN})

    list(LENGTH SHADER_SOURCE_FILES FILE_COUNT)
    if (FILE_COUNT EQUAL 0)
        message(FATAL_ERROR "Cannot create a shaders without any source files")
    endif()

    set (SHADER_COMMAND)
    set (SHADER_PRODUCT)

    foreach(SHADER_SOURCE IN LISTS SHADER_SOURCE_FILES)
        cmake_path (ABSOLUTE_PATH SHADER_SOURCE NORMALIZE)
        cmake_path (GET SHADER_SOURCE FILENAME SHADER_NAME)

        list (APPEND SHADER_COMMAND COMMAND)
        list (APPEND SHADER_COMMAND Vulkan::glslc)
        list (APPEND SHADER_COMMAND "${SHADER_SOURCE}")
        list (APPEND SHADER_COMMAND "-o")
        list (APPEND SHADER_COMMAND "${SPV_SHADERS_PATH}/${SHADER_NAME}.spv")

        list (APPEND SHADER_PRODUCT "${SPV_SHADERS_PATH}/${SHADER_NAME}.spv")

    endforeach()

    add_custom_target(${TARGET_NAME} ALL
        ${SHADER_COMMAND}
        COMMENT "Compiling shaders [${TARGET_NAME}]"
        SOURCES ${SHADER_SOURCE_FILES}
        BYPRODUCTS ${SHADER_PRODUCT}
    )

endfunction()

# Собственно компиляция шейдеров
add_shaders(shaders ${VERT_SHADERS} ${FRAG_SHADERS})
