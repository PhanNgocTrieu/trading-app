
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEADER_FILES
    bank.h
    login.h
    service.h
    logger.hpp
)

set (SOURCE_FILES
    service.cpp
    login.cpp
    bank.cpp
)

add_library(service SHARED
    ${HEADER_FILES}
    ${SOURCE_FILES}
)
