
#this will compile the current library.
add_library(GPHYSICS SHARED 
  Clarion.cxx
  Trinity.cxx
  LaBr.cxx
  physicsLoop.cxx
  
)

target_include_directories(GPHYSICS
  PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/include
  ${ROOT_INCLUDE_DIRS}
) 

root_generate_dictionary(G__GPhysics 
  GDetector.h
  Clarion.h 
  Trinity.h 
  LaBr.h 
  MODULE GPHYSICS
  LINKDEF Linkdef.h
)

target_link_libraries(GPHYSICS 
  PUBLIC
    GFORMAT
    GHISTOGRAMER
    ROOT::Core
    ROOT::RIO
)



#link the compiled libraries above to the final executable.
