add_executable(mem_stack mem/test_stack.c)
target_link_libraries(
    mem_stack PRIVATE
    Shemodogan::SHD
)