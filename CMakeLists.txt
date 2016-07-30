cmake_policy(SET CMP0048 NEW)

project(PigaDevkit)

set(PIGA_DEVKIT_VERSION_MAJOR "0" CACHE STRING "Major-Version")
set(PIGA_DEVKIT_VERSION_MINOR "1" CACHE STRING "Minor-Version")
set(PIGA_DEVKIT_VERSION_PATCH "0" CACHE STRING "Patch-Version")
set(PIGA_DEVKIT_VERSION_TWEAK "0" CACHE STRING "Tweak-Version")
set(PIGA_DEVKIT_VERSION ${PIGA_DEVKIT_VERSION_MAJOR}.${PIGA_DEVKIT_VERSION_MINOR}.${PIGA_DEVKIT_VERSION_PATCH}.${PIGA_DEVKIT_VERSION_TWEAK})

message(STATUS "Version: " ${PIGA_DEVKIT_VERSION})

cmake_minimum_required(VERSION 3.0)

include(GenerateExportHeader)

set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(HDR ${CMAKE_CURRENT_SOURCE_DIR}/include/piga/devkit)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(HDRS
    ${HDR}/Server.hpp
    ${HDR}/Client.hpp
    ${HDR}/Request.hpp
    ${HDR}/InstallRequest.hpp
)
set(SRCS
    ${SRC}/main.cpp
    ${SRC}/Server.cpp
    ${SRC}/Client.cpp
    ${SRC}/InstallRequest.cpp
)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)

add_executable(pigadevkit ${SRCS} ${HDRS})

find_package(Threads)
target_link_libraries(pigadevkit ${CMAKE_THREADS_LIBS_INIT})

if(CMAKE_USE_PTHREADS_INIT)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-lpthread")
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-pthread")
endif()

target_include_directories(pigadevkit PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

configure_file(
    ${HDR}/Config.hpp.in
    ${HDR}/Config.hpp
)

find_package(Boost COMPONENTS system filesystem program_options serialization REQUIRED)
if(${Boost_FOUND})
    target_include_directories(pigadevkit PRIVATE ${Boost_INCLUDE_DIR})
    target_link_libraries(pigadevkit ${Boost_LIBRARIES})
    add_definitions("-DHAS_BOOST")
endif()

set_property(TARGET pigadevkit PROPERTY VESION ${PIGA_DEVKIT_VERSION})
set_property(TARGET pigadevkit PROPERTY SOVERSION ${PIGA_DEVKIT_VERSION_MAJOR})
set_property(TARGET pigadevkit PROPERTY
    INTERFACE_pigadevkit_MAJOR_VERSION ${PIGA_DEVKIT_VERSION_MAJOR})

install(TARGETS pigadevkit EXPORT PigaDevkitTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
    RUNTIME DESTINATION bin)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/piga/devkit
    DESTINATION include/piga)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/PigaDevkitConfigVersion.cmake"
  VERSION ${PIGA_DEVKIT_VERSION}
  COMPATIBILITY AnyNewerVersion
)
configure_file(${CMAKE_MODULE_PATH}/PigaDevkitConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/PigaDevkitConfig.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cmake)

install(EXPORT PigaDevkitTargets
    FILE
        PigaDevkitTargets.cmake
    NAMESPACE
        Upstream::
    DESTINATION
        ${ConfigPackageLocation}
)
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/PigaDevkitConfigVersion.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/PigaDevkitConfig.cmake"
    DESTINATION
        ${ConfigPackageLocation}
)
install(
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/pigadevkit.service
    DESTINATION
        /etc/systemd/system/
)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A utility to manage the pigaco instance on a machine.")
set(CPACK_PACKAGE_VENDOR "Pigaco")
set(CPACK_PACKAGE_VERSION_MAJOR ${PIGA_DEVKIT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PIGA_DEVKIT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PIGA_DEVKIT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION_TWEAK ${PIGA_DEVKIT_VERSION_TWEAK})
set(CPACK_PACKAGE_VERSION ${PIGA_DEVKIT_VERSION})
set(CPACK_DEBIAN_PACKAGE_HOMEPAHE "https://github.com/pigaco/devkit")
set(CPACK_DEBIAN_PACKAGE_DEPENDS
"libboost-filesystem1.55.0 (>= 1.55.0), libboost-serialization1.55.0 (>= 1.55.0), libboost-system1.55.0 (>= 1.55.0), libboost-program-options1.55.0 (>= 1.55.0), libstdc++6 (>= 4.9.2-10)")
set(CPACK_DEBIAN_PACKAGE_NAME "piga-devkit")
set(CPACK_PACKAGE_NAME "piga-devkit")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Max Heisinger <mail@maximaximal.com>")
include(CPack)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")