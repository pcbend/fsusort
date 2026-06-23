
#this will compile the current library.
add_library(GPIPELINE SHARED 
  GPipeline.cxx
)

target_include_directories(GPIPELINE
  PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${ROOT_INCLUDE_DIRS}
) 

#root_generate_dictionary(G__GPipeline 
#  GPipeline
#  MODULE GPIPELINE
#  LINKDEF Linkdef.h
#)

target_link_libraries(GPIPELINE 
  PUBLIC
    ROOT::Core
)


