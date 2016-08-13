PROJECT(gpPanel)
cmake_minimum_required(VERSION 2.8)

MACRO (ADD_SOURCES _target)
    IF (NOT ${_target}_source_dir)
        SET(${_target}_source_dir ${CMAKE_CURRENT_SOURCE_DIR})
    ENDIF (NOT ${_target}_source_dir)

    FILE (RELATIVE_PATH _relPath "${${_target}_source_dir}" "${CMAKE_CURRENT_SOURCE_DIR}")
    FOREACH (_src ${ARGN})
        IF (_relPath)
            list (APPEND ${_target}_sources "${_relPath}/${_src}")
        ELSE (_src ${ARGN})
            LIST (APPEND ${_target}_sources "${_src}")
        ENDIF (_relPath)
    ENDFOREACH (_src ${ARGN})
    #propigate up to parent
    IF (_relPath)
        SET (${_target}_sources ${${_target}_sources} PARENT_SCOPE)
    ENDIF (_relPath)
ENDMACRO (ADD_SOURCES)

IF (UNIX)
    INCLUDE(GNUInstallDirs)
ENDIF(UNIX)

SET (wxWidgets_USE_UNICODE TRUE)
FIND_PACKAGE(wxWidgets REQUIRED core base)
INCLUDE(${wxWidgets_USE_FILE})

MESSAGE(STATUS "${wxWidgets_ROOT_DIR}")

INCLUDE_DIRECTORIES( "include" )
#INCLUDE_DIRECTORIES(${wxWidgets_INCLUDE_DIRS} "include" )
#SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${wxWidgets_CXX_FLAGS})
#ADD_DEFINITIONS(${wxWidgets_DEFINITIONS})

ADD_SUBDIRECTORY(gpPanel)
ADD_SUBDIRECTORY(include)
