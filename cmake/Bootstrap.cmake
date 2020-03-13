########################################################################################################################
## Bootstrap code for CMake projects
## (C) 2020 Russell Johnson
########################################################################################################################

# Define a function for printing out the header to the console
function(BOOTSTRAP_PRINT_HEADER msg)
    MESSAGE(STATUS "==================================================================================================")
    MESSAGE(STATUS "++++ ${msg} ++++")
    MESSAGE(STATUS "==================================================================================================")
endfunction(BOOTSTRAP_PRINT_HEADER)

function(BOOSTRAP_PRINT_BAR)
    MESSAGE(STATUS "==================================================================================================")
endfunction(BOOSTRAP_PRINT_BAR)


# Prevent insource building. This causes so many god damned problems, best to prevent it
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree built. Please avoid in tree source building.")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

if(WIN32 OR "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64|Visual Studio 15 2017)")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

cmake_policy(SET CMP0072 NEW)
if(${CMAKE_VERSION} VERSION_GREATER "3.12.0")
    cmake_policy(SET CMP0074 NEW)
endif()

set(CMAKE_INSTALL_RPATH "$ORIGIN")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Make the bin folder if it doesn't exist
set(BOOTSTRAP_BINARY_DIR ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${BOOTSTRAP_BINARY_DIR})

# Force all DLL and EXE's to be placed in /bin
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BOOTSTRAP_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BOOTSTRAP_BINARY_DIR})

#-------------------------------------------------------------------------------
# Set default install location to dist folder in build dir
# we do not want to install to /usr by default
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/dist" CACHE PATH
            "Install path prefix, prepended onto install directories." FORCE
            )
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

#------------------------------------------------------------------------------
# Custom Install target, used in run target in source/CMakeLists.txt
if (CMAKE_GENERATOR MATCHES "Makefiles")
    # Make it multithreaded
    add_custom_target( Install_ COMMAND "${CMAKE_COMMAND}" --build . --target
            install -- -j WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
else()
    add_custom_target( Install_ COMMAND "${CMAKE_COMMAND}" --build . --target
            install WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()


# Print out project info
#BOOTSTRAP_PRINT_HEADER("${PROJECT_NAME}")
#message(STATUS "Generator: ${CMAKE_GENERATOR}")