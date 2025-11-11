// Copyright 2025 Tier IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef RTP_IMAGE_RECEIVER_HPP
#define RTP_IMAGE_RECEIVER_HPP

#include <ros/ros.h>
#include <sensor_msgs/CompressedImage.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CameraInfo.h>
#include <std_msgs/Header.h>
#include <image_transport/image_transport.h>
#include <camera_info_manager/camera_info_manager.h>
#include "image_receiver.h"
#include <memory>
#include <string>

namespace rtp_image_receiver {

class RtpImageReceiverNode {
public:
    explicit RtpImageReceiverNode(ros::NodeHandle& nh, ros::NodeHandle& private_nh);
    ~RtpImageReceiverNode();

private:
    std_msgs::Header createHeader(const ImageReceiver::RTPTimestamp& timestamp);

    // Publishing helper methods
    void publishRaw(const uint8_t* data, size_t size, const std_msgs::Header& header);
    void publishCompressed(const uint8_t* data, size_t size, const std_msgs::Header& header);
    void publishCameraInfo(const std_msgs::Header& header);

    // Callback methods for different modes
    void jpegCallback(const uint8_t* data, size_t size, const ImageReceiver::RTPTimestamp& timestamp);
    void rawCallback(const uint8_t* data, size_t size, const ImageReceiver::RTPTimestamp& timestamp);
    void combinedCallback(const uint8_t* raw_data, size_t raw_size,
                         const uint8_t* jpeg_data, size_t jpeg_size,
                         const ImageReceiver::RTPTimestamp& timestamp);

    // Statistics publishing
    void publishStatistics(const ros::TimerEvent&);

    // Member variables
    ros::NodeHandle nh_;
    ros::NodeHandle private_nh_;
    image_transport::ImageTransport it_;

    std::unique_ptr<ImageReceiver> receiver_;
    ros::Publisher compressed_pub_;
    ros::Publisher camera_info_pub_;
    image_transport::Publisher raw_pub_;
    ros::Timer stats_timer_;
    std::shared_ptr<camera_info_manager::CameraInfoManager> camera_info_manager_;

    ReceiverConfig config_;
    bool publish_raw_;
    bool publish_compressed_;
    std::string frame_id_;
    uint64_t frame_count_ = 0;
};

}  // namespace rtp_image_receiver

#endif  // RTP_IMAGE_RECEIVER_HPP
