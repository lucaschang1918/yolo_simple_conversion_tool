# CMake 最低版本要求，Qt6 建议至少 3.16，更高版本（如 3.18+）能提供更好的支持
cmake_minimum_required(VERSION 3.18)

# -------------------------------- 基础配置 --------------------------------
# 设置应用程序名称，后续所有目标、安装等都用这个变量，方便统一修改
set(APP_NAME Project)
# 动态获取项目名称（使用源码目录名）
get_filename_component(PROJECT_NAME ${CMAKE_SOURCE_DIR} NAME)
string(TOUPPER ${APP_NAME} PROJECT_NAME_CAPITALIZED)  # 转换为大写（可选）

# -------------------------------- 版本管理 --------------------------------
# 使用 project() 内置版本管理（主版本将被缓存）
project(${APP_NAME}
        VERSION 1.0.0      # 默认版本号
        LANGUAGES CXX      # 指定语言
)

# 允许通过命令行覆盖版本参数
set(PROJECT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE STRING "主版本号")
set(PROJECT_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE STRING "次版本号")
set(PROJECT_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE STRING "补丁版本号")
set(VERSION_SUFFIX "" CACHE STRING "版本后缀 (如: beta, rc1)")

# 组合完整版本号
if (VERSION_SUFFIX)
    set(PROJECT_FULL_VERSION "${PROJECT_VERSION}-${VERSION_SUFFIX}")
else ()
    set(PROJECT_FULL_VERSION "${PROJECT_VERSION}")
endif ()

# -------------------------------- 构建信息 --------------------------------
# 配置构建信息输出级别
set(CMAKE_MESSAGE_LOG_LEVEL NOTICE)       # 抑制冗余信息
set(CMAKE_SUPPRESS_REGENERATION ON)       # 禁止自动重新生成
set(CMAKE_VERBOSE_MAKEFILE OFF)           # 关闭详细构建日志

# -------------------------------- Git 信息 --------------------------------
find_package(Git QUIET)
set(GIT_HASH "unknown")
set(GIT_BRANCH "unknown")

if (GIT_FOUND)
    # 获取 Git 短哈希
    execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_HASH_TMP
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE GIT_RESULT
    )
    # 获取 Git 分支名
    execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_BRANCH_TMP
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE GIT_RESULT
    )

    # 处理执行结果
    if (GIT_RESULT EQUAL 0)
        set(GIT_HASH ${GIT_HASH_TMP})
        set(GIT_BRANCH ${GIT_BRANCH_TMP})
    endif ()
endif ()

# 获取构建时间戳
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")

# -------------------------------- Qt 配置 --------------------------------
# 启用自动处理 Qt 资源 (MOC, UIC, RCC)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 查找必需 Qt6 组件
# Qt6 的 find_package 更加现代化。
# 当你查找一个组件（如 Widgets）时，它会自动包含其依赖项（如 Core, Gui）。
find_package(Qt6 REQUIRED COMPONENTS
        Widgets
        Sql
)

# -------------------------------- 编译选项 --------------------------------
# C++ 标准配置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 目标专属编译选项
add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE
        # MSVC 选项
        $<$<CXX_COMPILER_ID:MSVC>:
        /W4          # 警告等级4
        /WX          # 视警告为错误
        /permissive- # 标准一致性模式
        >

        # GCC/Clang 选项
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
        -Wall
        -Wextra
        -Wpedantic
        >
)

# -------------------------------- 源文件收集 --------------------------------
set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# 使用 CONFIGURE_DEPENDS 自动跟踪新文件
# 注意：虽然方便，但现代 CMake 更推荐显式列出所有源文件，以获得最可靠的构建行为。
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
        "${PROJECT_ROOT_DIR}/src/*.cpp"
        "${PROJECT_ROOT_DIR}/src/*.h"
        "${PROJECT_ROOT_DIR}/src/*.hpp"
        "${PROJECT_ROOT_DIR}/src/*.ui"
        "${PROJECT_ROOT_DIR}/src/*.qrc"    # 建议 qrc 文件放在 src 目录
)

# 在 IDE 中按目录结构组织文件
source_group(TREE ${PROJECT_ROOT_DIR} FILES ${SRC_FILES})
qt_add_resources(PROJECT_RESOURCES
        resource/app_icon.qrc
)
#target_sources(Project PRIVATE naiLong.o)
# -------------------------------- 可执行目标 --------------------------------
# 生成可执行文件，所有源文件都打包进 ${APP_NAME}
add_executable(${APP_NAME} ${SRC_FILES}
        src/widget/yolocontrol.cpp
        src/widget/yolocontrol.hpp
        src/widget/yolocontrol.ui

        naoLong.o
        ${PROJECT_RESOURCES}
#        Project.rcc.cpp
)

# 链接库和编译器选项
# Qt6 使用命名空间的方式链接库，例如 Qt6::Widgets。
# 这会自动处理其所有依赖项（如 Qt6::Core 和 Qt6::Gui），无需手动列出。
target_link_libraries(${APP_NAME} PRIVATE
        Qt6::Widgets
        Qt6::Sql
        project_warnings
)

# 包含目录配置
target_include_directories(${APP_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}       # 自动生成的 moc 文件
)

# -------------------------------- 平台特定配置 --------------------------------
if (WIN32)
    # 定义 Windows 下的宏，减少头文件体积，指定 Windows 10 API
    target_compile_definitions(${APP_NAME} PRIVATE
            WIN32_LEAN_AND_MEAN
            _WIN32_WINNT=0x0A00           # Windows 10
    )
    # Release 模式下为 GUI 程序（无命令行窗口），Debug 下为控制台程序（方便调试）
    set_target_properties(${APP_NAME} PROPERTIES
            WIN32_EXECUTABLE $<CONFIG:Release>
            # 设置可执行文件输出目录为工程根目录下 bin 文件夹，Linux/Windows 都适用
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    )

    # ------------------ 新增：自动调用 windeployqt6 ------------------
    # 定义 Qt bin 目录，注意这里需要使用 CMake 变量来获取，更通用
    # 查找 windeployqt6.exe 的路径
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt6.exe
            PATHS
            "${Qt6_DIR}/../bin"
            # 如果上面找不到，可以添加其他可能的路径
            # 例如: "D:/qt/6.9.1/mingw_64/bin"
            # 但优先使用 Qt6_DIR，因为它是自动检测的
    )

    if (WINDEPLOYQT_EXECUTABLE)
        message(STATUS "Found windeployqt6 at: ${WINDEPLOYQT_EXECUTABLE}")

        # 定义一个 custom command，在 ${APP_NAME} 可执行文件生成后执行
        add_custom_command(
                TARGET ${APP_NAME}       # 指定目标，此命令在构建此目标后执行
                POST_BUILD               # 在构建后执行
                COMMAND
                ${WINDEPLOYQT_EXECUTABLE}  # windeployqt6.exe 的完整路径
                --qmldir ${CMAKE_CURRENT_SOURCE_DIR} # 可选：如果项目使用 QML，添加此参数
                $<TARGET_FILE:${APP_NAME}>           # 待部署的可执行文件路径
                COMMENT "Automatically deploying Qt dependencies with windeployqt6"
        )
    else ()
        message(WARNING "windeployqt6.exe was not found. Please add its path to find_program() or run it manually.")
    endif ()


elseif (APPLE)
    target_compile_definitions(${APP_NAME} PRIVATE MACOS)
    set_target_properties(${APP_NAME} PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_FULL_VERSION}
            MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    )
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15") # macOS 部署目标版本，Qt6 要求更高版本

else ()
    target_compile_definitions(${APP_NAME} PRIVATE LINUX)
endif ()

# -------------------------------- 构建信息注入 --------------------------------
target_compile_definitions(${APP_NAME} PRIVATE
        BUILD_GIT_HASH="${GIT_HASH}"
        BUILD_GIT_BRANCH="${GIT_BRANCH}"
        BUILD_TIMESTAMP="${BUILD_TIMESTAMP}"
        PROJECT_FULL_VERSION="${PROJECT_FULL_VERSION}"
)

# -------------------------------- 安装规则 --------------------------------
include(GNUInstallDirs)

install(TARGETS ${APP_NAME}
        BUNDLE DESTINATION .                      # macOS .app
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # Windows/Linux 可执行文件
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # 共享库
)

# 安装附加资源（假设存在 assets 目录）
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    install(DIRECTORY assets/
            DESTINATION ${CMAKE_INSTALL_DATADIR}/${APP_NAME}
    )
endif ()

# macOS 专用 Bundle 修复
if (APPLE)
    # 对于 Qt6，更推荐使用 macdeployqt 工具进行部署，
    # 但 BundleUtilities 仍然是一个有效的备选方案。
    install(CODE "include(BundleUtilities)")
    install(CODE "fixup_bundle(\\\"\${CMAKE_INSTALL_PREFIX}/${APP_NAME}.app\\\" \\\"\\\" \\\"\\\")")
endif ()

# -------------------------------- 自动创建项目目录结构 --------------------------------
# 定义目录结构
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(SUBDIRS
        "${SRC_DIR}/resource"
        "${SRC_DIR}/utils"
        "${SRC_DIR}/widget"
)

# 创建目录
foreach (dir ${SUBDIRS})
    if (NOT EXISTS ${dir})
        file(MAKE_DIRECTORY ${dir})
    endif ()
endforeach ()