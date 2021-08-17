# ---------------------------------------------------------------------------
# QO Executables
# ---------------------------------------------------------------------------
find_package(PythonInterp REQUIRED)

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------


# ---------------------------------------------------------------------------
# Executables
# ---------------------------------------------------------------------------

add_executable(tester "tests/tester.cpp")
add_dependencies(tester catch_src)
target_link_libraries(tester qolib catch)
set_target_properties(tester PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
target_compile_definitions(tester PRIVATE CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR})

add_test(NAME Tests COMMAND tester WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

#add_executable(testgenerator "tests/testgenerator.cpp")
#target_link_libraries(testgenerator qolib catch)
#set_target_properties(testgenerator PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
#target_compile_definitions(testgenerator PRIVATE CMAKE_SOURCE_DIR="\\"${CMAKE_SOURCE_DIR}\\"")

add_custom_target(check
   COMMAND $<TARGET_FILE:tester>
   WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

add_dependencies(check tester)
   

# ---------------------------------------------------------------------------
# Linting
# ---------------------------------------------------------------------------

#add_tidy_target(lint_tools "${TESTS_SRC}")
#list(APPEND lint_targets lint_tools)

