cmake_minimum_required(VERSION 3.13)

#set(CMAKE_VERBOSE_MAKEFILE ON)

#project information
project(fsusort
        VERSION 0.1.2 
        DESCRIPTION "feed me"
        HOMEPAGE_URL "https://github.com/pcbend/fsusort"
        LANGUAGES CXX)


#set(CMAKE_BUILD_TYPE Debug)

#find the root, set the various cmake variables.
find_package(ROOT REQUIRED
  COMPONENTS
  Core
  RIO
  Tree
  Hist
  Thread
) 

set(CMAKE_CXX_STANDARD ${ROOT_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#setting output paths in the build directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_INSTALL_LIBDIR           ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(APPLE) 
  message("Building for Darwin")
  add_compile_definitions(__DARWIN__=TRUE)
elseif(LINUX) 
  message("Building for Linux")
  add_compile_definitions(__LINUX__=TRUE)
endif()


#This tells cmake to continue into these directories.
add_subdirectory(libraries)


#################
#################

add_executable(eventBuildingTest 
  src/eventBuildingTest.cxx
)

target_include_directories(eventBuildingTest  
  PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${ROOT_INCLUDE_DIRS}
)

target_link_libraries(eventBuildingTest
  PRIVATE
    GCORE
    GFORMAT
    GPHYSICS
    ${ROOT_LIBRARIES}
)

#################
#################

add_executable(physicsTest 
  src/physicsTest.cxx
)

target_include_directories(physicsTest  
  PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${ROOT_INCLUDE_DIRS}
)

target_link_libraries(physicsTest
  PRIVATE
    GCORE
    GFORMAT
    GPHYSICS
    GHISTOGRAMER
    ${ROOT_LIBRARIES}
)

#################
#################

add_executable(speedTest 
  src/speedTest.cxx
)

target_include_directories(speedTest  
  PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${ROOT_INCLUDE_DIRS}
)

target_link_libraries(speedTest
  PRIVATE
    GCORE
    GFORMAT
    GPHYSICS
    GHISTOGRAMER
    ${ROOT_LIBRARIES}
)

#################
#################

string(ASCII 27 Esc)
set(RED     "${Esc}[31m")
set(GREEN   "${Esc}[32m")
set(YELLOW  "${Esc}[33m")
set(BLUE    "${Esc}[34m")
set(MAGENTA "${Esc}[35m")
set(CYAN    "${Esc}[36m")
set(RESET   "${Esc}[0m")

#add_custom_command(
#  TARGET fsusort
#  POST_BUILD
#  COMMAND ${CMAKE_COMMAND} -E echo "-----------------------------------"
#  COMMAND ${CMAKE_COMMAND} -E echo "${CYAN} fsusort build done, ${YELLOW} woo hoo! ${RESET}"
#  COMMAND ${CMAKE_COMMAND} -E echo "-----------------------------------"
#)


#################
#################

function(add_fsu_executable target source)
  add_executable(${target} ${source} ) 

  target_include_directories(${target}
    PRIVATE
      ${CMAKE_SOURCE_DIR}/include
      ${ROOT_INCLUDE_DIRS}
  ) 
      
  
  target_link_libraries(${target}
    PRIVATE
      GCORE
      GFORMAT
      GPHYSICS
      #GTYPES
      GHISTOGRAMER
      GVIEWER
      #UNPACKER
      ${ROOT_LIBRARIES}
  )        

add_custom_command(
  TARGET ${target}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "-----------------------------------"
  COMMAND ${CMAKE_COMMAND} -E echo "${CYAN} ${target} build done, ${YELLOW} woo hoo! ${RESET}"
  COMMAND ${CMAKE_COMMAND} -E echo "-----------------------------------"
)

endfunction()

add_fsu_executable(fsusort           src/fsusort.cxx)
add_fsu_executable(simpleHists       src/simpleHists.cxx)
#add_fsu_executable(speedTest         src/speedTest.cxx)
#add_fsu_executable(physicsTest       src/physicsTest.cxx)
#add_fsu_executable(eventBuildingTest src/eventBuildingTest.cxx)
add_fsu_executable(waveformViewer    src/waveformViewer.cxx)
