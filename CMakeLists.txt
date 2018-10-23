cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

################################################################################
#
# Configuration
#
################################################################################

project (ImCon)

#
# Paths
#

set (IMCON_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

#
# Flags
#

set (CMAKE_C_FLAGS_WARNINGS "-Wall -Wextra -Wno-unused-parameter -Woverflow -Wno-unused-result")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS_WARNINGS} -O3")
set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS}")
set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_WARNINGS} -O0 -g")

#
# Packages
#

find_package (MPI REQUIRED)
include_directories (${MPI_INCLUDE_PATH})

################################################################################
#
# Building
#
################################################################################

include_directories (${IMCON_SOURCE_DIR}/lib)

#
# Objects
#

# Types
add_library (ictypes ${IMCON_SOURCE_DIR}/lib/types/matrix.c
    ${IMCON_SOURCE_DIR}/lib/types/image.c)

# Utilities
add_library (icutil ${IMCON_SOURCE_DIR}/lib/util/log.c)

#
# Executables
#

# Test: Types
add_executable (test-types ${IMCON_SOURCE_DIR}/tests/types.c)
target_link_libraries (test-types m ictypes ${MPI_LIBRARIES})

# Test: Convolution
add_executable (test-convolution ${IMCON_SOURCE_DIR}/tests/convolution.c
    ${IMCON_SOURCE_DIR}/app/convolution.c)
target_link_libraries (test-convolution m ictypes ${MPI_LIBRARIES})

# Test: MPI
add_executable (test-mpi ${IMCON_SOURCE_DIR}/tests/mpi.c)
target_link_libraries (test-mpi ${MPI_LIBRARIES})

# Image convolution
add_executable (imcon ${IMCON_SOURCE_DIR}/app/main.c
    ${IMCON_SOURCE_DIR}/app/cmd.c
    ${IMCON_SOURCE_DIR}/app/comm.c
    ${IMCON_SOURCE_DIR}/app/convolution.c)
target_link_libraries (imcon m ictypes icutil ${MPI_LIBRARIES})
add_executable (imcon-serial ${IMCON_SOURCE_DIR}/app/serial_main.c
    ${IMCON_SOURCE_DIR}/app/cmd.c
    ${IMCON_SOURCE_DIR}/app/convolution.c)
target_link_libraries (imcon-serial m ictypes icutil)
