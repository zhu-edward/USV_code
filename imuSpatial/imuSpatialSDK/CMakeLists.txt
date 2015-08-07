# Set System Specific Libraries
#if (${WIN32})
#  SET(SYSTEM_LIBS
#    wsock32)
#else (${WIN32})
#  SET(SYSTEM_LIBS
#    m
#    pthread)
#endif (${WIN32})

cmake_minimum_required(VERSION 2.8)

project(imuSpatial)

message (STATUS ${CMAKE_INSTALL_PREFIX})
message (STATUS ${SYSTEM_LIBS})
message (STATUS ${SYSTEM_LIBS})
message (STATUS ${SYSTEM_LIBS})
message (STATUS ${SYSTEM_LIBS})
message (STATUS ${SYSTEM_LIBS})
message (STATUS ${SYSTEM_LIBS})

SET(SRC
    packet_example.c
    spatial_packets.c
    an_packet_protocol.c
    rs232/rs232.c
)


#find_package(Boost COMPONENTS system)
#find_package(CommonSense)

#include_directories(${Boost_INCLUDE_DIRS})
#include_directories(${COMMONSENSE_INCLUDE_DIRS})

#message (STATUS ${Boost_INCLUDE_DIRS})

ADD_EXECUTABLE(packet_example ${SRC})


#TARGET_LINK_LIBRARIES(imuSpatial 
#  MOOS
#  MOOSGen
#  MOOSUtility
#  contacts
#  geometry 
#  mbutil
#${Boost_LIBRARIES}
#  ${SYSTEM_LIBS}
#${COMMONSENSE_LIBRARIES}
#)

# Install Targets
INSTALL(TARGETS packet_example
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
