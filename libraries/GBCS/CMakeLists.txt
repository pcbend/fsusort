
#this will compile the current library.
add_library(GBCS SHARED 
  GBCS.cxx
  GPIN.cxx
  GDSSD.cxx
  #GSSSD.cxx
)

target_include_directories(GBCS
  PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/include
  ${ROOT_INCLUDE_DIRS}
) 

root_generate_dictionary(G__GBCS 
  GBCS.h
  GPIN.h
  GDSSD.h
  #GSSSD.h
  MODULE GBCS
  LINKDEF Linkdef.h
)

target_link_libraries(GBCS 
  PUBLIC
    GFORMAT
    GHISTOGRAMER
    ROOT::Core
    ROOT::RIO
)



#link the compiled libraries above to the final executable.
