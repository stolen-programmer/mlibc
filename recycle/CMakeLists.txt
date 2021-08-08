
add_library(
        recycle SHARED
        recycle.c
)

target_include_directories(
        recycle PRIVATE
        ${PROJECT_SOURCE_DIR}
)


