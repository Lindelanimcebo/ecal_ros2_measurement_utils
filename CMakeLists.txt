cmake_minimum_required(VERSION 3.10)

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)
set(XIMEA_LIBRARY m3api)
set(MSGS_INCLUDE_PATH usr/include)

project(measurement_read)


find_package(eCAL REQUIRED)
find_package(OpenCV REQUIRED)

set(PROJECT_NAME measurement_read)

# NOTE: build with ->  cmake --build . --target measurement_read

set(measurement_read_src
    src/ximea_measurement_read.cpp
)

ecal_add_sample(${PROJECT_NAME} ${measurement_read_src})
# add_executable(${XI_TARGET} ${ximea_read_src})

# NOTE: build with ->  cmake --build . --target xi_target
set(XI_TARGET xi_target)
ecal_add_sample(${XI_TARGET} src/deserialize_ximea.cpp)


set(INCLUDE_DIRS 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${OpenCV_INCLUDE_DIRS} 
    ${MSGS_INCLUDE_PATH}
)

set(LIBS
    eCAL::hdf5
    ${OpenCV_LIBS}
    ${XIMEA_LIBRARY}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE MEASUREMENT_PATH="${SAMPLE_PERSON_MEASUREMENT_PATH}")
target_compile_definitions(${XI_TARGET} PRIVATE MEASUREMENT_PATH="${SAMPLE_PERSON_MEASUREMENT_PATH}")

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})
target_include_directories(${XI_TARGET} PUBLIC ${INCLUDE_DIRS}) 

target_link_libraries(${PROJECT_NAME} ${LIBS})
target_link_libraries(${XI_TARGET} ${LIBS})
target_link_directories(${XI_TARGET} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)
target_compile_features(${XI_TARGET} PRIVATE cxx_std_14)

ecal_install_sample(${PROJECT_NAME})
ecal_install_sample(${XI_TARGET})

if(MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY LINK_FLAGS "/ignore:4098,4099")
endif(MSVC)

if(MSVC)
    set_property(TARGET ${XI_TARGET} PROPERTY LINK_FLAGS "/ignore:4098,4099")
endif(MSVC)