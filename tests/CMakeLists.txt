project(tests)

find_package(Qt5Test 5.6.0 REQUIRED)

add_executable(hf_exec hf.cpp)
add_test(NAME hf COMMAND hf_exec)
target_link_libraries(hf_exec
    Qt5::Test
    Cutelyst::Core
    Cutelyst::WSGI
    Qt5::Network
    hash_backend_finance_challenge
)

