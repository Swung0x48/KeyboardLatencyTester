# ImPlot
set(IMPLOT_PUBLIC_HDRS
        ${IMPLOT_SOURCE_DIR}/implot.h
        ${IMPLOT_SOURCE_DIR}/implot_internal.h
        )

set(IMPLOT_SRCS
        ${IMPLOT_SOURCE_DIR}/implot.cpp
        ${IMPLOT_SOURCE_DIR}/implot_items.cpp
        ${IMPLOT_SOURCE_DIR}/implot_demo.cpp
        )

add_library(ImPlot INTERFACE)
target_sources(ImPlot INTERFACE ${IMPLOT_SRCS} ${IMPLOT_PUBLIC_HDRS})
target_include_directories(ImPlot INTERFACE
        $<BUILD_INTERFACE:${IMPLOT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${IMPLOT_SOURCE_DIR}/backends>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        )