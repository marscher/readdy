SET(SOURCES_DIR "${READDY_GLOBAL_DIR}/readdy2/main/common")

# boost
IF(${BUILD_SHARED_LIBS})
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
ELSE(${BUILD_SHARED_LIBS})
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME ON)
ENDIF(${BUILD_SHARED_LIBS})
FIND_PACKAGE(Boost 1.57 COMPONENTS system filesystem log thread REQUIRED)

# include dirs
SET(COMMON_INCLUDE_DIRS "${READDY_GLOBAL_INCLUDE_DIR};${Boost_INCLUDE_DIRS}" CACHE INTERNAL "Common include dirs" FORCE)

# sources
LIST(APPEND READDY_COMMON_SOURCES "${SOURCES_DIR}/Utils.cpp")

# all sources
LIST(APPEND READDY_ALL_SOURCES ${READDY_COMMON_SOURCES})