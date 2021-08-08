
add_library(
        scope SHARED
        scope.c
        node.c
)

target_include_directories(
        scope PRIVATE
        ${PROJECT_SOURCE_DIR}
)
