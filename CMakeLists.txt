project(ON_DEMAND_SERVER)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Debug)
set(UTILS_SOURCE_DIR ../utils)
    
include(../FindCurl.cmake)
include(../FindLibTorrent.cmake)
    
include_directories(
    ${CURL_INCLUDE_DIR} 
    ${LIBTORRENT_INCLUDE_DIR}
    ${PACKETIZER_SOURCE_DIR}/include
    ${UTILS_SOURCE_DIR}/include
	 ${ON_DEMAND_SERVER_SOURCE_DIR}/include
)

find_package(Boost COMPONENTS regex system thread log REQUIRED)

set(SOURCE
  ${ON_DEMAND_SERVER_SOURCE_DIR}/src/chunk_reader.cpp
  ${ON_DEMAND_SERVER_SOURCE_DIR}/src/connection.cpp
  ${ON_DEMAND_SERVER_SOURCE_DIR}/src/on_demand_server.cpp
  ${ON_DEMAND_SERVER_SOURCE_DIR}/src/req_parser.cpp
  ${ON_DEMAND_SERVER_SOURCE_DIR}/src/server.cpp
)

set(HEADERS
  ${ON_DEMAND_SERVER_SOURCE_DIR}/include/chunk_reader.hpp
  ${ON_DEMAND_SERVER_SOURCE_DIR}/include/connection.hpp
  ${ON_DEMAND_SERVER_SOURCE_DIR}/include/req_parser.hpp
  ${ON_DEMAND_SERVER_SOURCE_DIR}/include/server.hpp
)

include_directories(
    ${Boost_INCLUDE_DIRS}
)

link_directories(
    ${Boost_LIBRARY_DIRS}
)
                 
add_executable(on_demand_server ${SOURCE} ${HEADERS})
    
target_link_libraries(on_demand_server 
    ${Boost_REGEX_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_LOG_LIBRARY}
)
