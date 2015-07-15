set(PROJECTNAME USV)

project(${PROJECTNAME})

if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
endif()

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Find the MOOS package and include the header files
find_package(MOOS 10.0)
if(MOOS_FOUND)
   message(STATUS "Found the MOOS")
   include_directories(${MOOS_INCLUDE_DIRS})
else()
   message(FATAL_ERROR "Did not find the MOOS, stopping compilation...")
endif(MOOS_FOUND)

# Include the ivp include and library directories
set(IVP_INC_DIR ../../include/ivp CACHE STRING "The ivp include directory")
set(IVP_LIB_DIR ../../lib CACHE STRING "The ivp library directory")
get_filename_component(IVP_INC_DIR_ABS ${IVP_INC_DIR} ABSOLUTE)
get_filename_component(IVP_LIB_DIR_ABS ${IVP_LIB_DIR} ABSOLUTE)
include_directories(${IVP_INC_DIR_ABS})
link_directories(${IVP_LIB_DIR_ABS})

# Find the MOOSGeodesy package and include its header files and libraries
find_package(MOOSGeodesy)
if(MOOSGeodesy_FOUND)
   message(STATUS "Found MOOSGeodesy library")
   include_directories(${MOOSGeodesy_INCLUDE_DIRS})
   link_directories(${MOOSGeodesy_LIBRARY_PATH})
else()
   message(STATUS "Did not find MOOSGeodesy library")
endif(MOOSGeodesy_FOUND)

# add_subdirectory(test1)
# add_subdirectory(test2)

#add_subdirectory(vehicle)
#add_subdirectory(controller)
#add_subdirectory(desiredTrajectory)
add_subdirectory(pTimer)
add_subdirectory(pStateGen)
add_subdirectory(pTrajectorySim)
add_subdirectory(pVehicle)
add_subdirectory(pController)
add_subdirectory(pThrustSim)

#add_subdirectory(EX1000)
#add_subdirectory(EX1010)
