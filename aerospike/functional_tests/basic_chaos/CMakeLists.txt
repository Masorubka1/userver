project(userver-aerospike-tests-basic-chaos CXX)

add_executable(${PROJECT_NAME} "aerospike_service.cpp")
target_link_libraries(${PROJECT_NAME} userver-core userver-aerospike)

userver_chaos_testsuite_add()
