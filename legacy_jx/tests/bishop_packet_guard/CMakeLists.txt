cmake_minimum_required(VERSION 3.16)
project(JX_Bishop_Packet_Guard LANGUAGES CXX)
enable_testing()
add_executable(jx_bishop_guard_test test_bishop_packet_guard.cpp)
target_include_directories(jx_bishop_guard_test PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/../../gateway/Sources/MultiServer/Bishop")
target_compile_features(jx_bishop_guard_test PRIVATE cxx_std_17)
if(MSVC)
  target_compile_options(jx_bishop_guard_test PRIVATE /W4 /WX)
else()
  target_compile_options(jx_bishop_guard_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
add_test(NAME bishop_packet_guard_negative_cases COMMAND jx_bishop_guard_test)

# Verify the actual Heaven CSocketAddress peer-address grammar and explicit
# fail-closed GS peer allowlist policy without opening any network connection.
add_executable(jx_bishop_gs_peer_guard_test test_bishop_gs_peer_guard.cpp)
target_include_directories(jx_bishop_gs_peer_guard_test PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/../../gateway/Sources/MultiServer/Bishop")
target_compile_features(jx_bishop_gs_peer_guard_test PRIVATE cxx_std_17)
if(MSVC)
  target_compile_options(jx_bishop_gs_peer_guard_test PRIVATE /W4 /WX)
else()
  target_compile_options(jx_bishop_gs_peer_guard_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
add_test(NAME bishop_game_server_peer_trust COMMAND jx_bishop_gs_peer_guard_test)

# A connection may claim its initial login opcode exactly once;
# simulate concurrent duplicate login packets with 64 racing threads.
add_executable(jx_bishop_login_once_test test_bishop_login_once.cpp)
target_include_directories(jx_bishop_login_once_test PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/../../gateway/Sources/MultiServer/Bishop")
target_compile_features(jx_bishop_login_once_test PRIVATE cxx_std_17)
if(MSVC)
  target_compile_options(jx_bishop_login_once_test PRIVATE /W4 /WX)
else()
  target_compile_options(jx_bishop_login_once_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
add_test(NAME bishop_one_initial_login_per_connection COMMAND jx_bishop_login_once_test)

# Atomic claim/release ownership across local Bishop connections. These tests
# are not a substitute for AccountServer's authoritative session locks.
add_executable(jx_bishop_account_claim_test test_bishop_account_claim.cpp)
target_include_directories(jx_bishop_account_claim_test PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/../../gateway/Sources/MultiServer/Bishop")
target_compile_features(jx_bishop_account_claim_test PRIVATE cxx_std_17)
if(MSVC)
  target_compile_options(jx_bishop_account_claim_test PRIVATE /W4 /WX)
else()
  target_compile_options(jx_bishop_account_claim_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
add_test(NAME bishop_local_account_claim_ownership COMMAND jx_bishop_account_claim_test)
