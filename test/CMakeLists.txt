add_executable(test_stack mem/test_stack.c)
target_link_libraries(
    test_stack PRIVATE
    Shemodogan::Shemodogan
)

add_executable(test_shd test_shd.c)
target_link_libraries(
    test_shd PRIVATE
    Shemodogan::Shemodogan
)

add_executable(test_logger handler/test_logger.c)
target_link_libraries(
    test_logger PRIVATE
    Shemodogan::Shemodogan
)