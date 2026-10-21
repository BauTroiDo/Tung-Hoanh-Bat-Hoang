cmake_minimum_required(VERSION 3.20)
project(JX_LegacyLinuxABI LANGUAGES CXX)
include(CTest)
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(FATAL_ERROR "This ABI regression validates the Linux native compatibility header only")
endif()
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
  message(FATAL_ERROR "Linux x86_64 required for this ABI regression")
endif()

add_executable(jx_linux_abi_test linux_abi_test.cpp)
target_compile_features(jx_linux_abi_test PRIVATE cxx_std_17)
target_include_directories(jx_linux_abi_test PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/../../gameserver/Sources/Engine/Src"
  "${CMAKE_CURRENT_SOURCE_DIR}/../../gameserver/Sources/Core/Src")
target_compile_options(jx_linux_abi_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
add_test(NAME jx_linux_abi_contract COMMAND jx_linux_abi_test)
set_tests_properties(jx_linux_abi_contract PROPERTIES TIMEOUT 15)

# Compile two ACTUAL original Engine C++ source files, not replacement mocks.
add_library(jx_engine_node_list STATIC
  "${CMAKE_CURRENT_SOURCE_DIR}/../../gameserver/Sources/Engine/Src/KNode.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/../../gameserver/Sources/Engine/Src/KList.cpp")
target_include_directories(jx_engine_node_list PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/../../gameserver/Sources/Engine/Src")
target_compile_features(jx_engine_node_list PUBLIC cxx_std_17)
target_compile_options(jx_engine_node_list PRIVATE -Wall -Wextra -Wpedantic)

add_executable(jx_engine_node_list_smoke engine_node_list_smoke.cpp)
target_link_libraries(jx_engine_node_list_smoke PRIVATE jx_engine_node_list)
target_compile_features(jx_engine_node_list_smoke PRIVATE cxx_std_17)
target_compile_options(jx_engine_node_list_smoke PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME jx_engine_node_list_smoke COMMAND jx_engine_node_list_smoke)
set_tests_properties(jx_engine_node_list_smoke PROPERTIES TIMEOUT 15)

# Compile the actual legacy KTimer.cpp on Linux, not a mock clock.
add_library(jx_engine_timer STATIC
  "${CMAKE_CURRENT_SOURCE_DIR}/../../gameserver/Sources/Engine/Src/KTimer.cpp")
target_include_directories(jx_engine_timer PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/../../gameserver/Sources/Engine/Src")
target_compile_features(jx_engine_timer PUBLIC cxx_std_17)
target_compile_options(jx_engine_timer PRIVATE -Wall -Wextra -Wpedantic -Werror)
add_executable(jx_engine_timer_smoke engine_timer_smoke.cpp)
target_link_libraries(jx_engine_timer_smoke PRIVATE jx_engine_timer)
target_compile_features(jx_engine_timer_smoke PRIVATE cxx_std_17)
target_compile_options(jx_engine_timer_smoke PRIVATE -Wall -Wextra -Wpedantic -Werror)
add_test(NAME jx_engine_timer_monotonic COMMAND jx_engine_timer_smoke)
set_tests_properties(jx_engine_timer_monotonic PROPERTIES TIMEOUT 15)
