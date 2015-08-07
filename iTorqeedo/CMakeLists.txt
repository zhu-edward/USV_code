PROJECT(iTorqeedo)

#what files are needed?
SET(SRCS
  iTorqeedo.cpp
  iTorqeedo.h
  crc.h
  crc.c
  main.cpp
  )

SET(SRCS_NEW
  iTorqeedo_new.cpp
  iTorqeedo_new.h
  crc.h
  crc.c
  main_new.cpp
)

include_directories(
${MOOS_INCLUDE_DIRS} 
${MOOS_DEPEND_INCLUDE_DIRS}
${IVP_INC_DIR_ABS})

# Add executable called MOOSDB
#ADD_EXECUTABLE( iTorqeedo ${SRCS} )
ADD_EXECUTABLE( iTorqeedo_new ${SRCS_NEW} )

#TARGET_LINK_LIBRARIES( iTorqeedo ${MOOSLIBS} ${PLATFORM_LIBS} )
TARGET_LINK_LIBRARIES( iTorqeedo_new ${MOOSLIBS} ${PLATFORM_LIBS} ${MOOS_LIBRARIES}
${MOOS_DEPEND_LIBRARIES}
mbutil)

