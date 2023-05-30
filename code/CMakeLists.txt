cmake_minimum_required(VERSION 3.15)
project(diploma_code LANGUAGES C CXX VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(FETCHCONTENT_QUIET FALSE)
set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE ALWAYS)
include(FetchContent)

FetchContent_Declare(fmt
	GIT_REPOSITORY https://github.com/fmtlib/fmt.git
	GIT_TAG master
	GIT_PROGRESS TRUE
)

FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG "v1.x"
    GIT_PROGRESS TRUE
)

FetchContent_Declare(helib
    GIT_REPOSITORY https://github.com/homenc/HElib.git
    GIT_TAG master
    GIT_PROGRESS TRUE
)


Fetchcontent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG "develop"
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(spdlog fmt helib nlohmann_json)

find_package(Boost COMPONENTS program_options REQUIRED)

# =============================================================
#                           Sources
# =============================================================

set(CLIENT_SRC
    main.cpp
    client.cpp
)

set(SERVER_SRC
    main.cpp
    db.cpp
    server.cpp
    tcp_session.cpp
)

set(UTILS_SRC
    main.cpp
)

foreach(APP IN ITEMS client server utils)
    set(TARGET_NAME ${PROJECT_NAME}_${APP})
    string(TOUPPER ${APP} UPPER_APP)
    list(TRANSFORM ${UPPER_APP}_SRC PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/${APP}/)

    message(STATUS "Creating target for ${APP}, with following sources ${${UPPER_APP}_SRC}")
    add_executable(${TARGET_NAME} ${${UPPER_APP}_SRC})
    set_target_properties(${TARGET_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    target_link_libraries(${TARGET_NAME} PRIVATE spdlog::spdlog)
    target_link_libraries(${TARGET_NAME} PRIVATE fmt::fmt)
    target_link_libraries(${TARGET_NAME} PRIVATE ${Boost_LIBRARIES})
    target_link_libraries(${TARGET_NAME} PRIVATE helib)
    target_link_libraries(${TARGET_NAME} PRIVATE nlohmann_json)
endforeach()

