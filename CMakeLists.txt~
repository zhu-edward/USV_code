set(PROJECTNAME USV)

project(${PROJECTNAME})

if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
endif()

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

find_package(MOOS 10.0)

include_directories(${MOOS_INCLUDE_DIRS})

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

#add_subdirectory(EX1000)
#add_subdirectory(EX1010)
