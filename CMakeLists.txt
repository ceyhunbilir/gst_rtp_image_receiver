cmake_minimum_required(VERSION 3.10)
project(rtp_receiver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection
if(EXISTS "/etc/nv_tegra_release")
    set(JETSON_PLATFORM ON)
    add_definitions(-DJETSON_PLATFORM)
endif()

# Find ROS1 packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  sensor_msgs
  std_msgs
  cv_bridge
  image_transport
  camera_info_manager
)
find_package(OpenCV REQUIRED)

# Dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED
    gstreamer-1.0>=1.14
    gstreamer-app-1.0>=1.14
    gstreamer-rtp-1.0>=1.14
)

# Catkin package declaration
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES imagereceiver
  CATKIN_DEPENDS
    roscpp
    sensor_msgs
    std_msgs
    cv_bridge
    image_transport
    camera_info_manager
  DEPENDS OpenCV
)

# Source files
set(SOURCES
    src/image_receiver.cpp
)

# Library creation
add_library(imagereceiver SHARED ${SOURCES})

target_include_directories(imagereceiver
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${catkin_INCLUDE_DIRS}
    PRIVATE
        ${GST_INCLUDE_DIRS}
)

target_link_libraries(imagereceiver
    PRIVATE
        ${GST_LIBRARIES}
)

target_compile_options(imagereceiver PRIVATE ${GST_CFLAGS_OTHER})

# ROS1 Node executable
add_executable(rtp_image_receiver_node
    src/rtp_image_receiver_node.cpp
    src/rtp_image_receiver.cpp
)

target_include_directories(rtp_image_receiver_node
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${catkin_INCLUDE_DIRS}
    PRIVATE
        ${GST_INCLUDE_DIRS}
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(rtp_image_receiver_node
    imagereceiver
    ${catkin_LIBRARIES}
    ${GST_LIBRARIES}
    ${OpenCV_LIBS}
)

# # Original demo executable
# add_executable(image_receiver_demo examples/main.cpp)
# target_link_libraries(image_receiver_demo imagereceiver)

# Installation
install(TARGETS imagereceiver
    ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
)

install(DIRECTORY include/
    DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)

install(TARGETS rtp_image_receiver_node
    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

# Install launch files
install(DIRECTORY launch/
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
    PATTERN "*.py" EXCLUDE
)

# Install config files
install(DIRECTORY config/
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/config
)