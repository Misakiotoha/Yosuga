cmake_minimum_required(VERSION 3.26)
project(Yosuga VERSION 0.9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# 添加Ela子项目
add_subdirectory(ElaWidgetTools)

# 设置是否为debug模式 默认为Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 设置渲染方式，选择为OpenGL，不然默认是Cocos2d，编译包报错的，CMake也会给你警告的
set(FRAMEWORK_SOURCE OpenGL)

# 查找Live2D的所有src以及inc文件
file(GLOB LappLive "LAppLive2D/Src/*.cpp"
        "LAppLive2D/Inc/*.*"
)

find_package(Qt5 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Network
        OpenGL
        SerialPort
        Sql
        Svg
        WebChannel
        WebSockets
        Multimedia  # 音频模块
        REQUIRED)

# 如果失败，提示用户
if(NOT Qt5_FOUND)
    message(FATAL_ERROR "未找到 Qt5!可能的解决方案: \n"
            "1. 安装 Qt 5\n"
            "2. 将 Qt5_DIR 设置为你的 Qt 安装路径(例如：-DQt5_DIR=C:/Qt/5.12.12/mingw73_64)\n"
            "3. 将 Qt 的 bin 目录添加到 PATH 中"
    )
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "当前为 Debug 模式")
    # 添加Debug库文件搜索路径
    link_directories(
            "${CMAKE_SOURCE_DIR}/Lib/Debug/"
    )
else()
    message(STATUS "当前为 Release 或其他模式")
    # 添加Release库文件搜索路径
    link_directories(
            "${CMAKE_SOURCE_DIR}/Lib/Release/"
    )
    set(CON WIN32)  # Release关闭控制台输出
endif()

add_executable(${PROJECT_NAME} ${CON}
        Yosuga.rc  # 添加图标资源文件
        main.cpp
        GLCore.cpp
        GLCore.h
        "${LappLive}"
        "${ALL_INC}"
        USER_SOURCE/Menu/Inc/menu.h
        USER_SOURCE/Menu/Src/menu.cpp
        USER_SOURCE/AudioHandle/Inc/AudioOutput.h
        USER_SOURCE/AudioHandle/Src/AudioOutput.cpp
        USER_SOURCE/AudioHandle/Inc/AudioInput.h
        USER_SOURCE/AudioHandle/Src/AudioInput.cpp
        USER_SOURCE/NetWorkHandle/Inc/networkmanager.h
        USER_SOURCE/NetWorkHandle/Src/networkmanager.cpp
        USER_SOURCE/Setting/Inc/Setting.h
        USER_SOURCE/Setting/Src/Setting.cpp
        USER_SOURCE/NetWorkHandle/Inc/websocketmanager.h
        USER_SOURCE/NetWorkHandle/Src/websocketmanager.cpp
        USER_SOURCE/NetWorkHandle/Inc/socketmanager.h
        USER_SOURCE/NetWorkHandle/Src/socketmanager.cpp
        TextRenderer.cpp
        TextRenderer.h
        USER_SOURCE/Setting/Inc/BasePage.h
        USER_SOURCE/Setting/Src/BasePage.cpp
        USER_SOURCE/Setting/Inc/HomePage.h
        USER_SOURCE/Setting/Src/HomePage.cpp
        USER_SOURCE/Setting/Inc/NetworkPage.h
        USER_SOURCE/Setting/Src/NetworkPage.cpp
        USER_SOURCE/Setting/Inc/UISetting.h
        USER_SOURCE/Setting/Src/UISetting.cpp
        USER_SOURCE/Setting/Inc/AudioPage.h
        USER_SOURCE/Setting/Src/AudioPage.cpp
        USER_SOURCE/Setting/Inc/RenderPage.h
        USER_SOURCE/Setting/Src/RenderPage.cpp
        AppContext.h
)


# 根据编译选项来自动编译出Debug或是Release版
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # 链接Debug的库
    target_link_libraries(${PROJECT_NAME}
            ElaWidgetTools  # 直接链接子项目的目标
            Framework
            libglew32d.dll
            glfw3
            Live2DCubismCore_d
            opengl32
            glu32
            Qt5::Core
            Qt5::Gui
            Qt5::Widgets
            Qt5::Network
            Qt5::OpenGL
            Qt5::SerialPort
            Qt5::Sql
            Qt5::Svg
            Qt5::WebChannel
            Qt5::WebSockets
            Qt5::Multimedia
    )

else()
    # 链接Release的库
    target_link_libraries(${PROJECT_NAME}
            ElaWidgetTools  # 直接链接子项目的目标
            Framework
            libglew32.dll
            glfw3
            Live2DCubismCore
            opengl32
            glu32
            Qt5::Core
            Qt5::Gui
            Qt5::Widgets
            Qt5::Network
            Qt5::OpenGL
            Qt5::SerialPort
            Qt5::Sql
            Qt5::Svg
            Qt5::WebChannel
            Qt5::WebSockets
            Qt5::Multimedia
    )
endif()

# 添加头文件搜索路径
include_directories(
        ./
        LAppLive2D/Inc
        Thirdparty/Framework/src
        Thirdparty/Core/include
        Thirdparty/glew/include
        Thirdparty/glew/include/GL
        Thirdparty/glfw/include
        Thirdparty/glfw/include/GLFW
        Thirdparty/stb
        USER_SOURCE/Menu/Inc
        USER_SOURCE/AudioHandle/Inc
        USER_SOURCE/NetWorkHandle/Inc
        USER_SOURCE/Setting/Inc
)

# 自动复制Ela的DLL
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:ElaWidgetTools>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# 创建音频文件夹
file(GLOB WAV_FILE "${CMAKE_BINARY_DIR}/WavFiles")
if (NOT WAV_FILE)
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/WavFiles")
endif ()


file(GLOB RECORD_FILE "${CMAKE_BINARY_DIR}/Record")
if (NOT RECORD_FILE)
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Record")
endif ()

# 复制Resources目录到生成目录cmake-build-debug
file(GLOB RESOURCE_W "${CMAKE_BINARY_DIR}/Resources")
#message(STATUS RW: ${RESOURCE_W})
if (NOT RESOURCE_W) # 如果没有这个Resources目录就复制过去
    file(COPY
            "${CMAKE_SOURCE_DIR}/Resources"
            DESTINATION
            "${CMAKE_BINARY_DIR}/"
    )
endif ()

# 复制字体文件目录到生成目录cmake-build-xxx
file(GLOB FONT_W "${CMAKE_BINARY_DIR}/Font")
#message(STATUS FW: ${FONT_W})
if (NOT FONT_W) # 如果没有这个Font目录就复制过去
    file(COPY
            "${CMAKE_SOURCE_DIR}/Font"
            DESTINATION
            "${CMAKE_BINARY_DIR}/"
    )
endif ()

# 复制其他资源文件目录到生成目录cmake-build-debug
file(GLOB RES_W "${CMAKE_BINARY_DIR}/Resource")
#message(STATUS FW: ${RES_W})
if (NOT RES_W) # 如果没有这个Font目录就复制过去
    file(COPY
            "${CMAKE_SOURCE_DIR}/Resource"
            DESTINATION
            "${CMAKE_BINARY_DIR}/"
    )
endif ()

# 复制库文件中的dll文件到生成目录cmake-build-debug
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    file(GLOB GLEW_DLL_W "${CMAKE_BINARY_DIR}/glew32d.dll")
    file(GLOB LIVE2D_DLL_W "${CMAKE_BINARY_DIR}/Live2DCubismCore.dll")
    if (NOT GLEW_DLL_W)
        file(COPY
                "${CMAKE_SOURCE_DIR}/Lib/Debug/glew32d.dll"
                DESTINATION
                "${CMAKE_BINARY_DIR}/"
        )
    endif ()
    if (NOT LIVE2D_DLL_W)
        file(COPY
                "${CMAKE_SOURCE_DIR}/Lib/Debug/Live2DCubismCore.dll"
                DESTINATION
                "${CMAKE_BINARY_DIR}/"
        )
    endif ()
else()
    file(GLOB GLEW_DLL_W "${CMAKE_BINARY_DIR}/glew32.dll")
    file(GLOB LIVE2D_DLL_W "${CMAKE_BINARY_DIR}/Live2DCubismCore.dll")
    if (NOT GLEW_DLL_W)
        file(COPY
                "${CMAKE_SOURCE_DIR}/Lib/Release/glew32.dll"
                DESTINATION
                "${CMAKE_BINARY_DIR}/"
        )
    endif ()
    if (NOT LIVE2D_DLL_W)
        file(COPY
                "${CMAKE_SOURCE_DIR}/Lib/Release/Live2DCubismCore.dll"
                DESTINATION
                "${CMAKE_BINARY_DIR}/"
        )
    endif ()
endif()

# 部署 MinGW 运行时库
if (MINGW)
    get_filename_component(MINGW_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy
            "${MINGW_BIN_DIR}/libstdc++-6.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy
            "${MINGW_BIN_DIR}/libwinpthread-1.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
endif()

# 部署 Qt5 相关dll库
if (WIN32)
    # 查找 windeployqt 工具
    get_target_property(QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QMAKE_EXECUTABLE}/../bin" ABSOLUTE)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")

    # 执行自动部署
    if (WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --$<IF:$<CONFIG:Debug>,debug,release>  # 根据构建类型自动选择
                --verbose 1
                --no-compiler-runtime
                --no-system-d3d-compiler
                --no-webkit2
                --no-angle
                --no-opengl-sw
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
                COMMENT "Deploying Qt runtime with windeployqt..."
        )
    else ()
        message(WARNING "windeployqt not found, Qt DLLs will not be deployed automatically.")
    endif ()
endif ()
