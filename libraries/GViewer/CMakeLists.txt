
#this will compile the current library.
add_library(GVIEWER SHARED 
  GWaveViewer.cxx 
  
)

target_include_directories(GVIEWER
  PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${ROOT_INCLUDE_DIRS}
) 

root_generate_dictionary(G__GViewer 
  GWaveViewer.h 
  MODULE GVIEWER
  LINKDEF Linkdef.h
)

target_link_libraries(GVIEWER 
  PUBLIC
    GCORE
    GFORMAT
    ROOT::Core
    ROOT::Rint
    ROOT::RIO
    ROOT::Graf
    ROOT::Gpad
)



#link the compiled libraries above to the final executable.

