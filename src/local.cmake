# ---------------------------------------------------------------------------
# QO Src
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# Files
# ---------------------------------------------------------------------------

list(APPEND INCLUDE_H src/Attribute.hpp src/Database.hpp src/Register.hpp src/SQLParser.hpp src/Table.hpp)
list(APPEND INCLUDE_H src/operator/Chi.hpp src/operator/CrossProduct.hpp src/operator/HashJoin.hpp
                   src/operator/Indexscan.hpp src/operator/Operator.hpp src/operator/Printer.hpp
                   src/operator/Projection.hpp src/operator/Selection.hpp src/operator/Tablescan.hpp)

list(APPEND INCLUDE_H src/QueryGraph.hpp)           
list(APPEND INCLUDE_H src/SQLLexer.hpp) #append your own includes to the list

list(APPEND SRC_CC src/Attribute.cpp src/Database.cpp src/Register.cpp src/Table.cpp)
list(APPEND SRC_CC src/operator/Chi.cpp src/operator/CrossProduct.cpp src/operator/HashJoin.cpp
                   src/operator/Indexscan.cpp src/operator/Operator.cpp src/operator/Printer.cpp
                   src/operator/Projection.cpp src/operator/Selection.cpp src/operator/Tablescan.cpp)
list(APPEND SRC_CC src/SQLParser.cpp
                   src/SQLLexer.cpp) #append your own sources to the list
                   
if(EXISTS "${CMAKE_SOURCE_DIR}/src/muster/local.cmake")
   include("${CMAKE_SOURCE_DIR}/src/muster/local.cmake")
endif()

# ---------------------------------------------------------------------------
# Library
# ---------------------------------------------------------------------------

add_library(qolib STATIC ${SRC_CC} ${INCLUDE_H})
target_link_libraries(qolib)
target_compile_definitions(qolib PRIVATE CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR})

add_executable(admin "src/Admin.cpp")
target_link_libraries(admin qolib)
set_target_properties(admin PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
target_compile_definitions(admin PRIVATE CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR})

# ---------------------------------------------------------------------------
# Linting
# ---------------------------------------------------------------------------

#add_tidy_target(lint_src "${SRC_CC}")
#list(APPEND lint_targets lint_src)
