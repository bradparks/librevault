# - find Cryptodiff
# CRYPTODIFF_INCLUDE_DIR - Where to find Cryptodiff header files (directory)
# CRYPTODIFF_LIBRARIES - Cryptodiff libraries
# CRYPTODIFF_LIBRARY_RELEASE - Where the release library is
# CRYPTODIFF_LIBRARY_DEBUG - Where the debug library is
# CRYPTODIFF_FOUND - Set to TRUE if we found everything (library, includes and executable)

# Copyright (c) 2014-2015 Alexander Shishenko, <GamePad64@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# Generated by CModuler, a CMake Module Generator - http://gitorious.org/cmoduler

IF( CRYPTODIFF_INCLUDE_DIR AND CRYPTODIFF_LIBRARY_RELEASE AND CRYPTODIFF_LIBRARY_DEBUG )
    SET(CRYPTODIFF_FIND_QUIETLY TRUE)
ENDIF( CRYPTODIFF_INCLUDE_DIR AND CRYPTODIFF_LIBRARY_RELEASE AND CRYPTODIFF_LIBRARY_DEBUG )

FIND_PATH( CRYPTODIFF_INCLUDE_DIR cryptodiff.h  )

FIND_LIBRARY(CRYPTODIFF_LIBRARY_RELEASE NAMES cryptodiff )

FIND_LIBRARY(CRYPTODIFF_LIBRARY_DEBUG NAMES cryptodiff  HINTS /usr/lib/debug/usr/lib/ )

IF( CRYPTODIFF_LIBRARY_RELEASE OR CRYPTODIFF_LIBRARY_DEBUG AND CRYPTODIFF_INCLUDE_DIR )
	SET( CRYPTODIFF_FOUND TRUE )
ENDIF( CRYPTODIFF_LIBRARY_RELEASE OR CRYPTODIFF_LIBRARY_DEBUG AND CRYPTODIFF_INCLUDE_DIR )

IF( CRYPTODIFF_LIBRARY_DEBUG AND CRYPTODIFF_LIBRARY_RELEASE )
	# if the generator supports configuration types then set
	# optimized and debug libraries, or if the CMAKE_BUILD_TYPE has a value
	IF( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
		SET( CRYPTODIFF_LIBRARIES optimized ${CRYPTODIFF_LIBRARY_RELEASE} debug ${CRYPTODIFF_LIBRARY_DEBUG} )
	ELSE( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
    # if there are no configuration types and CMAKE_BUILD_TYPE has no value
    # then just use the release libraries
		SET( CRYPTODIFF_LIBRARIES ${CRYPTODIFF_LIBRARY_RELEASE} )
	ENDIF( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
ELSEIF( CRYPTODIFF_LIBRARY_RELEASE )
	SET( CRYPTODIFF_LIBRARIES ${CRYPTODIFF_LIBRARY_RELEASE} )
ELSE( CRYPTODIFF_LIBRARY_DEBUG AND CRYPTODIFF_LIBRARY_RELEASE )
	SET( CRYPTODIFF_LIBRARIES ${CRYPTODIFF_LIBRARY_DEBUG} )
ENDIF( CRYPTODIFF_LIBRARY_DEBUG AND CRYPTODIFF_LIBRARY_RELEASE )

IF( CRYPTODIFF_FOUND )
	IF( NOT CRYPTODIFF_FIND_QUIETLY )
		MESSAGE( STATUS "Found Cryptodiff header file in ${CRYPTODIFF_INCLUDE_DIR}")
		MESSAGE( STATUS "Found Cryptodiff libraries: ${CRYPTODIFF_LIBRARIES}")
	ENDIF( NOT CRYPTODIFF_FIND_QUIETLY )
ELSE(CRYPTODIFF_FOUND)
	IF( CRYPTODIFF_FIND_REQUIRED)
		MESSAGE( FATAL_ERROR "Could not find Cryptodiff" )
	ELSE( CRYPTODIFF_FIND_REQUIRED)
		MESSAGE( STATUS "Optional package Cryptodiff was not found" )
	ENDIF( CRYPTODIFF_FIND_REQUIRED)
ENDIF(CRYPTODIFF_FOUND)