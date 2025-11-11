    }
    
    void TearDown() override {
        // Cleanup if needed
    }
    
    ReceiverConfig config_;
};

// Test receiver creation
TEST_F(ImageReceiverTest, CreateReceiver) {
    ImageReceiver receiver(config_);
    // Should not crash
    SUCCEED();
}

// Test start and stop
TEST_F(ImageReceiverTest, StartStop) {
    ImageReceiver receiver(config_);
    
    // Start should succeed
    EXPECT_TRUE(receiver.start());
    
    // Give it some time to initialize
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Stop should work without issues
    receiver.stop();
    SUCCEED();
}

// Test callback setting
TEST_F(ImageReceiverTest, SetCallback) {
    ImageReceiver receiver(config_);
    
    std::atomic<bool> callback_called(false);
    
    receiver.setFrameCallback([&callback_called](const uint8_t* data, 
                                                  size_t size, 
                                                  int64_t timestamp) {
        callback_called = true;
    });
    
    // Should not crash
    SUCCEED();
}

// Test statistics
TEST_F(ImageReceiverTest, GetStatistics) {
    ImageReceiver receiver(config_);
    
    receiver.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto stats = receiver.getStatistics();
    
    // Initial statistics should be zero or non-negative
    EXPECT_GE(stats.frames_processed, 0);
    EXPECT_GE(stats.frames_dropped, 0);
    EXPECT_GE(stats.average_fps, 0.0);
    EXPECT_GE(stats.average_latency_ms, 0.0);
    
    receiver.stop();
}

// Test multiple start/stop cycles
TEST_F(ImageReceiverTest, MultipleStartStop) {
    ImageReceiver receiver(config_);
    
    for (int i = 0; i < 3; ++i) {
        EXPECT_TRUE(receiver.start());
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        receiver.stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    SUCCEED();
}

// Test configuration variations
TEST_F(ImageReceiverTest, DifferentConfigs) {
    // Test with different port
    config_.udp_port = 5005;
    ImageReceiver receiver1(config_);
    EXPECT_TRUE(receiver1.start());
    receiver1.stop();
    
    // Test with different quality
    config_.jpeg_quality = 50;
    ImageReceiver receiver2(config_);
    EXPECT_TRUE(receiver2.start());
    receiver2.stop();
    
    // Test with zero-copy disabled
    config_.enable_zero_copy = false;
    ImageReceiver receiver3(config_);
    EXPECT_TRUE(receiver3.start());
    receiver3.stop();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}