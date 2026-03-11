add_executable(
    unit_sbblock
    unit_mem_sbblock.c
)
target_link_libraries(
    unit_sbblock PRIVATE
    Shemodogan::Shemodogan
)