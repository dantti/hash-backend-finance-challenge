file(GLOB_RECURSE hash_backend_finance_challenge_SRCS *.cpp *.h)

set(hash_backend_finance_challenge_SRCS
    ${hash_backend_finance_challenge_SRCS}
    ${TEMPLATES_SRC}
)

# Create the application
add_library(hash_backend_finance_challenge SHARED ${hash_backend_finance_challenge_SRCS})

# Link to Cutelyst
target_link_libraries(hash_backend_finance_challenge
    Cutelyst::Core
    ASqlQt5::Core
    Qt5::Core
    Qt5::Network
)

install(TARGETS hash_backend_finance_challenge
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
)

install(DIRECTORY ../root
        DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/hashf
)
