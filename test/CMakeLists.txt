cmake_minimum_required(VERSION 3.5)
project(tests)
enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Boost COMPONENTS REQUIRED
        filesystem
        system
        thread
        program_options
        regex
        )
if (NOT Boost_FOUND)
    MESSAGE(FATAL_ERROR "Could not find boost library")
endif ()

find_package(GraphicsMagick REQUIRED)
if (NOT MAGICK_FOUND)
    MESSAGE(FATAL_ERROR "Could not find GraphicsMagick library")
endif ()

find_package(TBB REQUIRED)
if (NOT TBB_FOUND)
    MESSAGE(FATAL_ERROR "Could not find TBB library")
endif ()

find_package(Tesseract)
if (NOT Tesseract_FOUND)
    MESSAGE(FATAL_ERROR "Could not find Tesseract library")
endif ()

find_package(Poppler COMPONENTS cpp)
if (NOT POPPLER_FOUND)
    MESSAGE(FATAL_ERROR "Could not find Poppler library")
endif ()

include_directories(${MAGICK_INCLUDE_DIR})
include_directories(${TBB_INCLUDE_DIRS})
include_directories(${Tesseract_INCLUDE_DIR})
include_directories(${POPPLER_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src)

link_directories(${MAGICK++_LIBRARY_DIR})

set(SOURCE_FILES
        test_Commandline.cxx
        ../src/TesseractWrapper.cxx
        ../src/PixWrapper.cxx
        ../src/Timer.cxx
        ../src/Poppler.cxx
        ../src/PDF.cxx
        ../src/PDFPage.cxx
        ../src/Commandline.cxx
        ../src/Options.cxx
        test_Options.cxx
        main.cxx
        test_PDF.cxx
        )
add_executable(potoo_test ${SOURCE_FILES})

target_link_libraries(potoo_test ${MAGICK++_LIBRARIES} ${Boost_LIBRARIES} ${TBB_LIBRARIES} ${Tesseract_LIBS} ${POPPLER_LIBRARIES})

# declares a test with our executable
add_test(NAME test1 COMMAND potoo_test)