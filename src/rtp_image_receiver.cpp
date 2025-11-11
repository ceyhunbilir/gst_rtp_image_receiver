#include "rtp_image_receiver.hpp"
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>

namespace rtp_image_receiver {

RtpImageReceiverNode::RtpImageReceiverNode(ros::NodeHandle& nh, ros::NodeHandle& private_nh)
    : nh_(nh), private_nh_(private_nh), it_(nh) {
    // Get parameters with defaults
    int udp_port = private_nh_.param("udp_port", 5008);
    int jpeg_quality = private_nh_.param("jpeg_quality", 90);
    int buffer_size = private_nh_.param("buffer_size", 8388608);
    int max_buffers = private_nh_.param("max_buffers", 3);
    int width = private_nh_.param("width", 1920);
    int height = private_nh_.param("height", 1280);
    publish_raw_ = private_nh_.param("publish_raw", false);
    publish_compressed_ = private_nh_.param("publish_compressed", true);
    frame_id_ = private_nh_.param("frame_id", std::string("camera"));
    std::string camera_info_url = private_nh_.param("camera_info_url", std::string(""));

    // create config
    config_.udp_port = udp_port;
    config_.jpeg_quality = jpeg_quality;
    config_.buffer_size = buffer_size;
    config_.max_buffers = max_buffers;
    config_.width = width;
    config_.height = height;

    // sink mode selection
    if (publish_raw_ && publish_compressed_){
        config_.mode = SinkMode::RAW_AND_JPEG;
    }
    else if(publish_raw_){
        config_.mode = SinkMode::RAW_ONLY;
    }
    else if(publish_compressed_){
        config_.mode = SinkMode::JPEG_ONLY;
    }
    else{
        ROS_ERROR("Both publish_raw and publish_compressed are false. No data will be received. Defaulting to RAW_ONLY.");
        config_.mode = SinkMode::RAW_ONLY;
    }

    // create gstreamer receiver
    receiver_ = std::make_unique<ImageReceiver>(config_);

    // create publishers
    if (publish_raw_) {
        raw_pub_ = it_.advertise("image_raw", 1);
    }
    if (publish_compressed_) {
        compressed_pub_ = nh_.advertise<sensor_msgs::CompressedImage>(
            "image_raw/compressed", 1);
    }
    camera_info_pub_ = nh_.advertise<sensor_msgs::CameraInfo>(
        "camera_info", 1);

    // Initialize camera info manager
    camera_info_manager_ = std::make_shared<camera_info_manager::CameraInfoManager>(nh_, frame_id_);
    if (!camera_info_url.empty()) {
        camera_info_manager_->loadCameraInfo(camera_info_url);
        ROS_INFO("Loaded camera info from: %s", camera_info_url.c_str());
    } else {
        ROS_WARN("No camera_info_url provided, using default camera info");
    }

    // create statistics timer
    stats_timer_ = nh_.createTimer(
        ros::Duration(5.0),
        &RtpImageReceiverNode::publishStatistics, this);

    // setup receiver node callback
    if (config_.mode == SinkMode::RAW_AND_JPEG) {
        ROS_INFO("Set RAW_AND_JPEG Callback");
        receiver_->setCombinedFrameCallback(
            std::bind(&RtpImageReceiverNode::combinedCallback, this,
                     std::placeholders::_1, std::placeholders::_2,
                     std::placeholders::_3, std::placeholders::_4,
                     std::placeholders::_5));
    } else if (config_.mode == SinkMode::RAW_ONLY) {
        ROS_INFO("Set RAW Callback");
        receiver_->setRawFrameCallback(
            std::bind(&RtpImageReceiverNode::rawCallback, this,
                     std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    } else { // JPEG_ONLY
        ROS_INFO("Set JPEG Callback");
        receiver_->setJpegFrameCallback(
            std::bind(&RtpImageReceiverNode::jpegCallback, this,
                     std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    // launch gstreamer callback
    if (!receiver_->start()) {
        ROS_ERROR("Failed to start image receiver");
        ros::shutdown();
        return;
    }

    // display Node information
    ROS_INFO("RTP Image Receiver Node started on UDP port %d", udp_port);
    ROS_INFO("Resolution: %dx%d", width, height);
    ROS_INFO("Publishing: raw=%s, compressed=%s",
                publish_raw_ ? "true" : "false",
                publish_compressed_ ? "true" : "false");
}

RtpImageReceiverNode::~RtpImageReceiverNode() {
    if (receiver_) {
        receiver_->stop();
    }
}

std_msgs::Header RtpImageReceiverNode::createHeader(const ImageReceiver::RTPTimestamp& timestamp) {
    std_msgs::Header header;
    header.frame_id = frame_id_;
    // Use the extracted RTP timestamp if available
    if (timestamp.seconds > 0) {
        header.stamp.sec = timestamp.seconds;
        header.stamp.nsec = timestamp.nanoseconds;
    } else {
        // Otherwise, use the current ROS time
        header.stamp = ros::Time::now();
    }
    return header;
}

void RtpImageReceiverNode::publishRaw(const uint8_t* data, size_t size, const std_msgs::Header& header) {
    if (publish_raw_ && raw_pub_.getNumSubscribers() > 0) {
        cv::Mat bgr_image(config_.height, config_.width, CV_8UC3, (void*)data);

        sensor_msgs::ImagePtr image_msg =
            cv_bridge::CvImage(header, "bgr8", bgr_image).toImageMsg();
        ROS_INFO("Publish Image topic          : %09u.%09u", header.stamp.sec, header.stamp.nsec);

        raw_pub_.publish(image_msg);
    }
}

void RtpImageReceiverNode::publishCompressed(const uint8_t* data, size_t size, const std_msgs::Header& header) {
    if (publish_compressed_ && compressed_pub_.getNumSubscribers() > 0) {
        sensor_msgs::CompressedImage compressed_msg;
        compressed_msg.header = header;
        compressed_msg.format = "jpeg";
        compressed_msg.data.assign(data, data + size);

        compressed_pub_.publish(compressed_msg);
        ROS_INFO("Publish CompressedImage topic: %09u.%09u", header.stamp.sec, header.stamp.nsec);
    }
}

void RtpImageReceiverNode::publishCameraInfo(const std_msgs::Header& header) {
    if (camera_info_pub_.getNumSubscribers() > 0) {
        sensor_msgs::CameraInfo camera_info_msg = camera_info_manager_->getCameraInfo();
        camera_info_msg.header = header;
        camera_info_pub_.publish(camera_info_msg);
    }
}

void RtpImageReceiverNode::jpegCallback(const uint8_t* data, size_t size, const ImageReceiver::RTPTimestamp& timestamp) {
    std_msgs::Header header = createHeader(timestamp);

    publishCompressed(data, size, header);
    publishCameraInfo(header);

    frame_count_++;
}

void RtpImageReceiverNode::rawCallback(const uint8_t* data, size_t size, const ImageReceiver::RTPTimestamp& timestamp) {
    std_msgs::Header header = createHeader(timestamp);

    publishRaw(data, size, header);
    publishCameraInfo(header);

    frame_count_++;
}

void RtpImageReceiverNode::combinedCallback(const uint8_t* raw_data, size_t raw_size,
                      const uint8_t* jpeg_data, size_t jpeg_size,
                      const ImageReceiver::RTPTimestamp& timestamp) {
    std_msgs::Header header = createHeader(timestamp);

    // (Publish both data streams, which are synchronized by PTS within ImageReceiver::Impl)
    publishRaw(raw_data, raw_size, header);
    publishCompressed(jpeg_data, jpeg_size, header);
    publishCameraInfo(header);

    frame_count_++;
}

void RtpImageReceiverNode::publishStatistics(const ros::TimerEvent&) {
    auto stats = receiver_->getStatistics();
    ROS_INFO("Stats : Frames(Raw)=%lu, Dropped(Raw)=%lu, Frames(Jpeg)=%lu, Dropped(Jpeg)=%lu, FPS=%.2f",
        stats.frames_processed_raw, stats.frames_dropped_raw,
        stats.frames_processed_jpeg, stats.frames_dropped_jpeg,
        stats.average_fps_raw);
}

}  // namespace rtp_image_receiver
