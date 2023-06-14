project(userver-aerospike CXX)

# Set the path to the Aerospike C client library
set(AEROSPIKE_LIB_DIR "/usr/lib")

# Find the Aerospike C client library
find_library(AEROSPIKE_LIB aerospike PATHS ${AEROSPIKE_LIB_DIR})

# Check if Aerospike C client library is found
if(NOT AEROSPIKE_LIB)
  message(FATAL_ERROR "Aerospike C client library not found")
endif()

file(GLOB_RECURSE SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

file(GLOB_RECURSE UTESTLIB_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/testing/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testing/src/*.hpp
)
list(REMOVE_ITEM SOURCES ${UTESTLIB_SOURCES})

file(GLOB_RECURSE UNIT_TEST_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp
)
list(REMOVE_ITEM SOURCES ${UNIT_TEST_SOURCES})

file(GLOB_RECURSE AEROSPIKE_TEST_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*_aerospiketest.cpp
)
list(REMOVE_ITEM SOURCES ${AEROSPIKE_TEST_SOURCES})

add_library(${PROJECT_NAME} STATIC ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_compile_options (${PROJECT_NAME} PRIVATE -Wno-old-style-cast)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
    userver-core
  PRIVATE
    userver-uboost-coro
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${AEROSPIKE_LIB})

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/utils
  /usr/include/aerospike  # Include Aerospike headers
  /usr/include/citrusleaf
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_include_directories (${PROJECT_NAME} PRIVATE
    $<TARGET_PROPERTY:userver-core,INCLUDE_DIRECTORIES>
)