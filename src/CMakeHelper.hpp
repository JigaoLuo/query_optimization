#ifndef H_CMakeHelper
#define H_CMakeHelper

#define Q(x) #x
#define QUOTE(x) Q(x)
#define CMAKE_SOURCE_DIR_CSTR QUOTE(CMAKE_SOURCE_DIR)
#define UNIDB_DEFAULT_PATH CMAKE_SOURCE_DIR_CSTR "/data/uni"
#define TPCH_DEFAULT_PATH CMAKE_SOURCE_DIR_CSTR "/data/tpch"

#endif
