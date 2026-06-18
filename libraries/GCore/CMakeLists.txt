
#this will compile the current library.
add_library(GCORE SHARED 
  Gint.cxx 
  Gtypes.cxx
  globals.cxx
  
)

target_include_directories(GCORE
  PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${ROOT_INCLUDE_DIRS}
) 

root_generate_dictionary(G__GCore 
  Gint.h 
  Gtypes.h 
  MODULE GCORE
  LINKDEF Linkdef.h
)

target_link_libraries(GCORE 
  PUBLIC
    ROOT::Core
    ROOT::Rint
    ROOT::RIO
)



#link the compiled libraries above to the final executable.

