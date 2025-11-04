cmake_minimum_required(VERSION 3.10)

# Find GTest
find_package(GTest)

if(GTest_FOUND)
    # Test executable
    add_executable(test_image_receiver test_image_receiver.cpp)
    
    target_link_libraries(test_image_receiver
        imagereceiver
        GTest::GTest
        GTest::Main
        ${GST_LIBRARIES}
    )
    
    target_include_directories(test_image_receiver PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${GST_INCLUDE_DIRS}
    )
    
    # Add test
    add_test(NAME ImageReceiverTest COMMAND test_image_receiver)
else()
    message(WARNING "GTest not found. Tests will not be built.")
endif()