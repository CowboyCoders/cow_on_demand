project(COW_ON_DEMAND)
cmake_minimum_required(VERSION 2.6)

ENABLE_TESTING()

set(CMAKE_BUILD_TYPE Debug)

include_directories(
    ${CURL_INCLUDE_DIR} 
    ${LIBTORRENT_INCLUDE_DIR}
	${COW_ON_DEMAND_SOURCE_DIR}/include
)

find_package(Boost COMPONENTS regex system thread log REQUIRED)

set(SOURCE
  ${COW_ON_DEMAND_SOURCE_DIR}/src/chunk_reader.cpp
  ${COW_ON_DEMAND_SOURCE_DIR}/src/connection.cpp
  ${COW_ON_DEMAND_SOURCE_DIR}/src/on_demand_server.cpp
  ${COW_ON_DEMAND_SOURCE_DIR}/src/req_parser.cpp
  ${COW_ON_DEMAND_SOURCE_DIR}/src/server.cpp
)

set(HEADERS
  ${COW_ON_DEMAND_SOURCE_DIR}/include/chunk_reader.hpp
  ${COW_ON_DEMAND_SOURCE_DIR}/include/connection.hpp
  ${COW_ON_DEMAND_SOURCE_DIR}/include/req_parser.hpp
  ${COW_ON_DEMAND_SOURCE_DIR}/include/server.hpp
)

set(REQ_PARSER_TEST_SOURCES
  ${COW_ON_DEMAND_SOURCE_DIR}/src/req_parser.cpp
  ${COW_ON_DEMAND_SOURCE_DIR}/test/req_parser_test.cpp
)

include_directories(
    ${Boost_INCLUDE_DIRS}
)

link_directories(
    ${Boost_LIBRARY_DIRS}
)

set(DEPS
    ${Boost_REGEX_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_LOG_LIBRARY}
)

# build the server
add_executable(cow_on_demand ${SOURCE} ${HEADERS})
target_link_libraries(cow_on_demand ${DEPS})

# build parser test
add_executable(req_parser_test ${REQ_PARSER_TEST_SOURCES} ${HEADERS})
target_link_libraries(req_parser_test ${DEPS})

# add tests
ADD_TEST(ParserRequestTest req_parser_test)
