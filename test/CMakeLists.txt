add_executable(bench_logger_size benchmark/logger_size.c)
target_link_libraries(
    bench_logger_size PRIVATE
    Shemodogan::Shemodogan
)
add_executable(bench_logger_speed benchmark/logger_speed.c)
target_link_libraries(
    bench_logger_speed PRIVATE
    Shemodogan::Shemodogan
)

add_executable(unit_logger unit/logger.c)
target_link_libraries(
    unit_logger PRIVATE
    Shemodogan::Shemodogan
)
add_executable(unit_shd unit/shd.c)
target_link_libraries(
    unit_shd PRIVATE
    Shemodogan::Shemodogan
)
add_executable(unit_stack unit/stack.c)
target_link_libraries(
    unit_stack PRIVATE
    Shemodogan::Shemodogan
)