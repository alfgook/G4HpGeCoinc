#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(ED)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Locate the CRY library
find_library(LCRY libCRY.a
             PATHS ${CRY_PATH}/lib
             DOC "cry library"
             NO_DEFAULT_PATH
            )
if(NOT LCRY)
  message(WARNING "cry library not found... skipping this in the build!")
else()
  add_compile_definitions(CRY_DATA_DIR="${CRY_PATH}/data")
  add_compile_definitions(BUILD_WITH_CRY)
  include_directories(${CRY_PATH}/src)
endif()
#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

if(NOT LCRY)
  list(REMOVE_ITEM sources ${PROJECT_SOURCE_DIR}/src/PrimaryGeneratorActionCRY.cc)
  list(REMOVE_ITEM sources ${PROJECT_SOURCE_DIR}/src/PrimaryGeneratorMessenger.cc)
  list(REMOVE_ITEM sources ${PROJECT_SOURCE_DIR}/src/RNGWrapper.cc)
  list(REMOVE_ITEM headers ${PROJECT_SOURCE_DIR}/include/PrimaryGeneratorActionCRY.hh)
  list(REMOVE_ITEM headers ${PROJECT_SOURCE_DIR}/include/PrimaryGeneratorMessenger.hh)
  list(REMOVE_ITEM headers ${PROJECT_SOURCE_DIR}/include/RNGWrapper.hh)
endif()
#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(G4HPGeCoinc G4HPGeCoinc.cc ${sources} ${headers})
target_link_libraries(G4HPGeCoinc ${Geant4_LIBRARIES})

#----------------------------------------------------------------------------
# link the CRY library
if(LCRY)
  target_link_libraries(G4HPGeCoinc ${LCRY})
endif()

#----------------------------------------------------------------------------
# Set the c++ std
set_target_properties(G4HPGeCoinc PROPERTIES CXX_STANDARD 17)
#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build ED. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(EXAMPLE_SCRIPTS
  macros/init.mac
  macros/init_vis.mac
  macros/icons.mac
  macros/run.png
  macros/vis.mac
  macros/testRun.mac
  macros/default.cry.setup
  macros/detectorSetup.mac
  macros/Cs134.mac
  )

foreach(_script ${EXAMPLE_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
#install(TARGETS G4HPGeCoinc DESTINATION bin)


