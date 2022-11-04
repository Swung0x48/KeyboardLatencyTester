# ImGui
if (EXISTS ${IMGUI_SOURCE_DIR}/imconfig.h)
    file(RENAME
            ${IMGUI_SOURCE_DIR}/imconfig.h
            ${IMGUI_SOURCE_DIR}/imconfig.h.bak)
endif ()

file(COPY ${CMAKE_CURRENT_LIST_DIR}/../../imconfig.h DESTINATION ${IMGUI_SOURCE_DIR})

set(IMGUI_PUBLIC_HDRS
        ${IMGUI_SOURCE_DIR}/imgui.h
        ${IMGUI_SOURCE_DIR}/imgui_internal.h
        ${IMGUI_SOURCE_DIR}/imstb_rectpack.h
        ${IMGUI_SOURCE_DIR}/imstb_textedit.h
        ${IMGUI_SOURCE_DIR}/imstb_truetype.h
        )


set(IMGUI_SRCS
        ${IMGUI_SOURCE_DIR}/imgui.cpp
        ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
        ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
        ${IMGUI_SOURCE_DIR}/imgui_demo.cpp
        ${IMGUI_SOURCE_DIR}/imgui_tables.cpp)

if (WIN32)
    set(IMGUI_PUBLIC_HDRS ${IMGUI_PUBLIC_HDRS}
            ${IMGUI_SOURCE_DIR}/backends/imgui_impl_win32.h
            ${IMGUI_SOURCE_DIR}/backends/imgui_impl_dx9.h
            ${IMGUI_SOURCE_DIR}/backends/imgui_impl_dx11.h
            ${IMGUI_SOURCE_DIR}/backends/imgui_impl_dx12.h)

    set(IMGUI_SRCS ${IMGUI_SRCS}
            ${IMGUI_SOURCE_DIR}/backends/imgui_impl_win32.cpp
            ${IMGUI_SOURCE_DIR}/backends/imgui_impl_dx9.cpp
            ${IMGUI_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
            ${IMGUI_SOURCE_DIR}/backends/imgui_impl_dx12.cpp)
endif()

if (APPLE)
    set(IMGUI_PUBLIC_HDRS ${IMGUI_PUBLIC_HDRS}
            ${IMGUI_SOURCE_DIR}/backends/imgui_impl_metal.h)

    set(IMGUI_SRCS ${IMGUI_SRCS}
            ${IMGUI_SOURCE_DIR}/backends/imgui_impl_metal.mm)
endif()

set(IMGUI_PUBLIC_HDRS ${IMGUI_PUBLIC_HDRS}
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.h
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.h)

set(IMGUI_SRCS ${IMGUI_SRCS}
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)

add_library(ImGui INTERFACE)
target_sources(ImGui INTERFACE ${IMGUI_SRCS} ${IMGUI_PUBLIC_HDRS})
target_include_directories(ImGui INTERFACE
        $<BUILD_INTERFACE:${IMGUI_SOURCE_DIR}>
        $<BUILD_INTERFACE:${IMGUI_SOURCE_DIR}/backends>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        )