# Инициализация Vulkan

## Предварительная подготовка

В нашем учебном примере мы будем писать, на C++, что-то вроде примитивного графического движка, а поэтому нам необходима неготорая стратовая архитектура нашего проекта. Создадим главный класс VulkanEngine, инкапсулирующий все механизмы взаимодействия нашего приложения с Vulkan API.

**vk_engine.h**
```cpp
#ifndef     VK_ENGINE_H
#define     VK_ENGINE_H

#include	<vk_types.h>

/*!
 * \class
 * \brief Простейший графический движок на Vulkan API
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class VulkanEngine
{
public:

    VulkanEngine() = default;

    ~VulkanEngine() = default;

    /// Инициализация движка
    void init();

    /// Очистка структур данных движка
    void cleanup();

    /// Запуск движка
    void run();

private:


};

#endif

```
с соответствующей реализацией в файле vk_engine.cpp, пока что пустой

**vk_engine.cpp**
```cpp
#include    <vk_engine.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::init()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::cleanup()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::run()
{

}
```

Опеределим файл, включающий все необходимые нам заголовки для движка, в частности заголовочные файлы Vulkan, а так же сдесь будем описывать все необходимые типы данных, которые будем использовать в проекте

**vk_types.h**
```cpp
#ifndef     VK_TYPES_H
#define     VK_TYPES_H

#include    <vulkan/vulkan.h>

#endif
```

Теперь мы можем приступить к инициализации Vulkan API

## Создание экземпляра Vulkan

Первым отличием Vulkan от OpenGL является то, что Vulkan не имеет глобального состояния, подобного конечному автомату OpenGL. Состояние вашего приложения, использующего Vulkan, логически отделено от других подобных приложений, и для описания этого состояния необходимо сосздать как минимум один объект, называемый *экземпляром* Vulkan. Поэтому, первым шагом инициализации Vulkan в нашем приложении будет создание экземпляра, представляемого в Vulkan объектом типа `VkInstance`. Определим дескриптор экземпляра в приватной части класса VulkanEngine

```cpp
class VulkanEngine
{
public:

    VulkanEngine() = default;

    ~VulkanEngine() = default;

    /// Инициализация движка
    void init();

    /// Очистка структур данных движка
    void cleanup();

    /// Запуск движка
    void run();

private:

    /// Экземпляр Vulkan
    VkInstance  instance;
};
```

Дескриптор представляет собою число размером 64 бита (причем вне зависимости от разрядности платформы, на которой работает приложение) и через него мы ссылемся на конкретный экземпляр Vulkan.

Для создания экземпляра тербуется выполнить вызов следующей функции

```cpp
VkResult vkCreateInstance(
    const VkInstanceCreateInfo*                 pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkInstance*                                 pInstance);
```

где

+ `pCreateInfo` - указатель на структуру, содержащую параметры, описывающие экземпляр;
+ `pAllocator` - указатель на аллокатор памяти центрального процессора (CPU), который Vulkan будет использовать для распределения памяти вашего приложения;
+ `pInstance` - указатель на переменную-дескриптор экземпляра


Что касается аллокаторов, это отдельная тема для разговора. На первых порах нам будет достаточно внутреннего аллокатора Vulkan, поэтому мы укажем для него нулевой указатель `nullptr`. 

Дескриптор экземпляра определен в классе движка переменной `instance`. Что касается структуры, то её необходимо корректно заполнить. Надо сказать это одна из самых муторных вещей при вызове функцию Vulkan - заполнение разнообразных структур. Тем неменее, все информационные структуры, передаваемые функциям API, придерживаются единого стиля оформления. Мы будем разбирать процесс инициализации достаточно подробно, но, чтобы упорядочить наш код и сделать его читабельным, введем дополнительный модуль, содержащий функции инициализации структур Vulkan. Создадим заголовочный файл

**vk_initializers.h**
```cpp
#ifndef     VK_INITIALIZERS_H
#define     VK_INITIALIZERS_H

#include    <vk_types.h>

namespace vkinit {

    VkInstanceCreateInfo instance_create_info(const std::string &app_name);
}

#endif
```

Эта функция будет принимать в качестве параметра строку с именем приложения, а возвращять структуру-описатель экземпляра. Формат этой структуры следующий

```cpp
typedef struct VkInstanceCreateInfo {
    VkStructureType             sType;
    const void*                 pNext;
    VkInstanceCreateFlags       flags;
    const VkApplicationInfo*    pApplicationInfo;
    uint32_t                    enabledLayerCount;
    const char* const*          ppEnabledLayerNames;
    uint32_t                    enabledExtensionCount;
    const char* const*          ppEnabledExtensionNames;
} VkInstanceCreateInfo;
```

+ `sType` - тип структуры. В данном случае передается константа-перечислитель `VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO`, что достаточно многословно, но удавлетворяте общему шаблону именования перечислителей типов структур в Vulkan, в этом мы неоднократно убедимся;
+ pNext - указатель на следующую структуру, служащий для передачи в функцию связанного списка структур. Это позволяет расширять число передаваемых параметров, не меняя структуры. В нашем случае это не требуется, поэтому данное поле должно получить `nulptr`;
+ `flags` - зарезервировано для дальнейшего использования и должно быть равно нулю;
+ `pApplicationInfo` - указатель на структуру `VkApplicationInfo`, через которую передается информация о приложении. О ней мы поговорим чуть ниже;
+ `enabledLayerCount` - число включенных смлоев валидации - пока делаем равным нулю, о слоях валидации поговорим позже;
+ `ppEnabledLayerNames` - указатель на список имен слоев валидации, пока делаем равным `nullptr`;
+ `enabledExtensionCount` - число включенных расширений Vulkan. Делаем равным нулю, потому что пока не планируем использовать какие-либо расширения;
+ `ppEnabledExtensionNames` - список имен используемых расширений. Поскольку мы работаем без расшинений - `nullptr`.


Очевидно, что нам предварительно нужно сформировать струтуру типа VkApplicationInfo. Разберем её подробнее

```cpp
typedef struct VkApplicationInfo {
    VkStructureType    sType;
    const void*        pNext;
    const char*        pApplicationName;
    uint32_t           applicationVersion;
    const char*        pEngineName;
    uint32_t           engineVersion;
    uint32_t           apiVersion;
} VkApplicationInfo;
```

+ sType - задает тип структуры, в данном случае `VK_STRUCTURE_TYPE_APPLICATION_INFO`;
+ pNext - указатель на следующую струтуру-расширитель, в нашем случае `nullptr`. В дальнейшем, значения этого поля и предыдущего я буду уточнять только в случае если они выбиваются из представленного шаблона.
+ `pApplicationName` - указатель на строку с именем приложения. Может принимать любое нужное вам значение, пускай в данном случае это будет "Vulkan Engine";
+ applicationVersion - номер версии вашего приложения - произвольно, по вашему вкусу;
+ `pEngineName` - указатедь на сроку с именем движка. Так же может как отсутствовать, так и принимать нужное вам значение;
+ `engineVersion` - номер версии движка;
+ `apiVersion` - версия Vulkan API, которую хочет использовать ваше приложение. Она должна соответствовать минимальной версии Vulkan API, которая требуется вашему приложению для работы и не обязательно должна быть равна версии Vulkan SDK, которая установлена в данный момент на вашем компьютере. 

Теперь приступим к реализации создания экземпляра. Для начала реализуем функцию заполнения необходимых структур

**vk_initializeers.cpp**
```cpp
#include    <vk_initializers.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
VkInstanceCreateInfo vkinit::instance_create_info(const std::string &app_name)
{
    // Заполняем структуру с информацией о приложении
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = app_name.c_str();
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = app_name.c_str();
    appInfo.engineVersion = 1;    
    // Используем макрос VK_MAKE_VERSION, для формирования значения
    // соотвествующего версии 1.3.0 Vulkan API
    appInfo.apiVersion = VK_MAKE_VERSION(1, 3, 0);

    // Заполняем структуру с параметрами экземпляра
    VkInstanceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.pApplicationInfo = &appInfo;
    info.enabledLayerCount = 0;
    info.ppEnabledLayerNames = nullptr;
    info.enabledExtensionCount = 0;
    info.ppEnabledExtensionNames = nullptr;

    return info;
}
```

Далее, реализуем вспомогательный макрос VK_CHECK(), формирующий удобочитаемые сообщения об ошибках в консоли нашего приложения. Им мы будем оборачивать все важные вызовы Vulkan

**vk_types.h**
```cpp
#ifndef     VK_TYPES_H
#define     VK_TYPES_H

#include    <iostream>
#include    <string>
#include    <format>

#include    <vulkan/vulkan.h>
#include    <vulkan/vk_enum_string_helper.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
#define VK_CHECK(x) \
    do \
    { \
            VkResult err = x; \
            if (err != VK_SUCCESS) \
            { \
                std::cout << std::format("Detected Vulkan error: {}", \
                                         string_VkResult(err)) << std::endl; \
            } \
    } while (0) \
 \

#endif
```

Для инициализации Vulkan создадим в нашем класса VulkanEngine приватную функцию `init_vulkan()`

```cpp
class VulkanEngine
{
public:

    VulkanEngine() = default;

    ~VulkanEngine() = default;

    /// Инициализация движка
    void init();

    /// Очистка структур данных движка
    void cleanup();

    /// Запуск движка
    void run();

private:

    /// Экземпляр Vulkan
    VkInstance  instance;

    /// Инициализация Vulkan
    void init_vulkan();
};

#endif
```

и, наконец, приступим к реализации

**vk_engine.cpp**
```cpp
#include    <vk_engine.h>
#include    <vk_initializers.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::init()
{
    init_vulkan();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::cleanup()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::run()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::init_vulkan()
{
    // Формируем структуру для создания экземпляра
    VkInstanceCreateInfo instCreateInfo =
            vkinit::instance_create_info("Vulkan Engine");

    // Создаем экземпляр
    VK_CHECK(vkCreateInstance(&instCreateInfo, nullptr, &instance));
}
```

Полагаю, что приведенный код не нуждается в особом описании. Сначала мы формируем структуру `VkInstanceCreateInfo` вызывая функцию `vkinit::instance_create_info()`, а затем передаем указатель на эту структуру, нулевой указатель для аллокатора и указатель на дескриптор экземпляра в вызов `vkCreateInstance()`, оборацивая её вызов макросом VK_CHECK(), формирующий код, который в случае неудачи укажет нам на ошибку в консольном выхлопе. 

После компиляции и запуска нашего приложения vkengine.exe оно запустившись, завершится, ничего не выводя на экран. Это означает, что экземпляр Vulkan успешно создается нашим приложением.

## Настройка слоев валидации (validation layers) Vulkan 

Vulkan API расчитан на обеспечение максимальной производительности приложения, которое его использует. По этой причине, в отличие от OpenGL, Vulkan явно не обрабатывает подавляющее большинство даже элементарных ошибок, таких как неверное значение перечислителей или передача нулевого указателя. Это может привести к неопределенному поведению и сбоям в работе приложения. Учитывая, что в Vulkan API настраивается очень много параметров, появление подобных ошибок происходит почти наверняка в процессе разработки.

Для решения задачи диагностики приложения в процессе его выполнения, Vulkan предоставляет механизм так называемых *слоев валидации*. Их использование позволяет расширить или изменить поведение вызовов Vulkan, что позволяет выполнять следующие операции:

+ проверку значений параметров на соответствие спецификации для обнаружения ошибок;
+ отслеживать утечки ресурсов;
+ проверять безопасность многопоточной работы;
+ логирование вызовов и их параметров;
+ отслеживание вызовов для профилирования.

Слои валидации можно полностью выключать в релизных сборках, для обеспечения наилучшей производительности.

Vulkan не имеет встроенных слоев валидации, однако, LunarG Vulkan SDK, который используем мы, содержит набор слоев для отслеживания наиболее частых ошибок. Введем в наш проект возможность включения слоев валидации в отладочную сборку. 

Для этого в модуле `vk_initializers.cpp` введем следующие определения

```cpp
// Определяем признак использования слоев валидации, взависимости от типа
// сборки приложения (выключаем при релизной сборке)
#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

// Список слоев валидации, которые предполагается использовать
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};
```

Включим один слой под названием `"VK_LAYER_KHRONOS_validation"`, содержащий все необходимые нам на данном этапе работы проверки.

Введем так же следующую функцию

**vk_initializers.cpp**
```cpp
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool vkinit::check_validation_layers_support(const std::vector<const char *> &validLayers)
{
    uint32_t layerCount = 0;

    // Получаем число доступных слоев валидации
    VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));

    // Теперь, получаем список доступных слоев валидации
    std::vector<VkLayerProperties> availableLayers(layerCount);

    VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

    // Формируем список корректных слоев
    for (const char* layerName : validLayers)
    {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}
```

Данная функция проверяет наличие слоев валидации, перечисленных в константе `validationLayers`. Для получения списка доступных слоев валидации используется вызов `vkEnumerateInstanceLayerProperties()`, имеющий следующую спецификацию

```cpp
VkResult vkEnumerateInstanceLayerProperties(
    uint32_t*                                   pPropertyCount,
    VkLayerProperties*                          pProperties);
```

+ `pPropertyCount` - указатель на переменную, куда будет возвращено число доступных слоев валидации;
+ `pProperties` - указатель на массив структур `VkLayerProperties`, содержащих информацию о каждом слое валидации

```cpp
typedef struct VkLayerProperties {
    char        layerName[VK_MAX_EXTENSION_NAME_SIZE];
    uint32_t    specVersion;
    uint32_t    implementationVersion;
    char        description[VK_MAX_DESCRIPTION_SIZE];
} VkLayerProperties;
```

Здесь

+ `layerName` - строка, содержащая имя слоя;
+ `specVersion` - версия Vulkan, в которой был введен данный слой;
+ `implementationVersion` - версия слоя;
+ `description` - строка-описание, содержащая дополнительные сведения, которые могут быть использованы приложениями для идентификации данного слоя.

Мы вызываем функцию `vkEnumerateInstanceLayerProperties()` дважды: в первый раз передаем ей нулевой указатель на список свойств слоев, получая число доступных слоев. Далее мы выделяем память под список свойств слоев, и вызываем функцию еще раз, получая список доступных слоев. Затем мы сравниеваем полученный список с со списком доступных слоев, чтобы убедится, что заданные нами слои корректны.

Теперь мы можем изменить функцию `vkinit::instance_create_info()` следующим образом, в части инициализации структуры VkInstanceCreateInfo

```cpp
// Заполняем структуру с параметрами экземпляра
    VkInstanceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.pApplicationInfo = &appInfo;
    info.enabledLayerCount = 0;
    info.ppEnabledLayerNames = nullptr;

    // Задаем слои валидации, если нужно
    if (enableValidationLayers)
    {
        if (check_validation_layers_support(validationLayers))
        {
            info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            info.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            throw std::runtime_error("ERROR: Validation layers requested, but not available!");
        }

    }
```
В случае, если хотя бы одно из заданных нами имен слоев некорректно, мы генерируем исключение с выводом соответствующего сообщения об ошибке.

## Получение дескриптора физического устройства

