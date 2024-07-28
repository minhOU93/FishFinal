#New Module CMake Template written by Scott Nykl. This requires the engine is already built, compiled, and INSTALL'd to user land (.../repos/aburn/usr/).

#This simply includes the module-wide Google Test cmake script that all modules use
#to create a Test Project so Google Test can be used within a module.
#This CMake script is located in "..../usr/include/cmake/aftrModule_GTest_Proj.cmake"

include_guard()
MESSAGE( STATUS "[ ${CMAKE_CURRENT_LIST_FILE}:${CMAKE_CURRENT_LIST_LINE} ] "
                 "ENTERING Module's Google Test CMakeLists.txt..."          )

IF( AFTR_USE_GTEST )
   MESSAGE( STATUS "GTEST Enabled - Including aftr_module_load_GTest.cmake" )
   include( "${AFTR_PATH_TO_CMAKE_SCRIPTS}/aftr_module_load_GTest.cmake" )
ELSE()
   MESSAGE( STATUS "----------------------------------------------------------------------------------")
   MESSAGE( STATUS "GTEST Disabled - CMake Option AFTR_USE_GTEST was *not* enabled, not using GTest...")
   MESSAGE( STATUS "----------------------------------------------------------------------------------")
endif()

MESSAGE( STATUS "[ ${CMAKE_CURRENT_LIST_FILE}:${CMAKE_CURRENT_LIST_LINE} ] "
                 "LEAVING Module's Google Test CMakeLists.txt..."          )