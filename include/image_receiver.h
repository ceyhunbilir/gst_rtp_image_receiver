#ifndef IMAGE_RECEIVER_H
#define IMAGE_RECEIVER_H

#include <memory>
#include <functional>
#include <cstdint>
#include <cstddef>

enum class SinkMode {
    RAW_ONLY,
    JPEG_ONLY,
    RAW_AND_JPEG
};

enum class HardwareType {
    VAAPI_INTEL,
    VAAPI_AMD,
    NVIDIA_DESKTOP,
    NVIDIA_JETSON,
    SOFTWARE_FALLBACK
};

struct ReceiverConfig {
    int udp_port = 5008;
    int jpeg_quality = 90;
    int buffer_size = 8388608;  // 8MB
    int max_buffers = 3;
    int width = 1920;           // Video width
    int height = 1280;          // Video height
    SinkMode mode = SinkMode::RAW_AND_JPEG;
};

class ImageReceiver {
public:
    struct RTPTimestamp {
        uint32_t rtp_timestamp;     // RTP header timestamp (32-bit)
        uint32_t ssrc;               // Synchronization source identifier
        uint32_t csrc;               // Contributing source identifier (contains nanoseconds)
        int64_t pts_ms;              // Presentation timestamp in milliseconds
        uint64_t seconds;            // Reconstructed seconds (48-bit in original)
        uint32_t nanoseconds;        // Nanoseconds portion
        uint16_t fractions;          // Fractions of nanoseconds
    };
    
    // use for RAW_ONLY and JPEG_ONLY
    using FrameCallback = std::function<void(const uint8_t* data, size_t size, const RTPTimestamp& rtp_ts)>;
    
    // use for CombinedFrameCallback
    using CombinedFrameCallback = std::function<void(
        const uint8_t* raw_data, size_t raw_size, 
        const uint8_t* jpeg_data, size_t jpeg_size, 
        const RTPTimestamp& rtp_ts)>;
    
    ImageReceiver(const ReceiverConfig& config);
    ~ImageReceiver();
    
    // Disable copy
    ImageReceiver(const ImageReceiver&) = delete;
    ImageReceiver& operator=(const ImageReceiver&) = delete;
    
    // Start/stop processing
    bool start();
    void stop();
    
    // Set callback for processed frames
    void setRawFrameCallback(FrameCallback callback);
    void setJpegFrameCallback(FrameCallback callback);
    void setCombinedFrameCallback(CombinedFrameCallback callback);
    
    // Statistics
    struct Statistics {
        uint64_t frames_processed_raw = 0;
        uint64_t frames_processed_jpeg = 0;
        uint64_t frames_dropped_raw = 0;
        uint64_t frames_dropped_jpeg = 0;
        double average_fps_raw = 0;
    };
    Statistics getStatistics() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif // IMAGE_RECEIVER_H