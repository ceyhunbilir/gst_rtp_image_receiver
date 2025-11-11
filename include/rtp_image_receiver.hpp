#ifndef GST_RTP_IMAGE_RECEIVER_INCLUDE_RTP_IMAGE_RECEIVER_HPP_
#define GST_RTP_IMAGE_RECEIVER_INCLUDE_RTP_IMAGE_RECEIVER_HPP_

#include <memory>
#include <string>

#include <ros/ros.h>
#include <camera_info_manager/camera_info_manager.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/CompressedImage.h>
#include <sensor_msgs/Image.h>
#include <std_msgs/Header.h>

#include "image_receiver.h"

namespace rtp_image_receiver {

// ROS1 node for receiving and publishing RTP image streams.
// This node receives RTP video streams via GStreamer and publishes them
// as ROS topics in both raw and compressed formats.
class RtpImageReceiverNode {
 public:
  // Constructor initializes the node with ROS handles.
  // @param nh: ROS node handle for public topics
  // @param private_nh: ROS node handle for private parameters
  explicit RtpImageReceiverNode(ros::NodeHandle& nh,
                                ros::NodeHandle& private_nh);

  // Destructor stops the image receiver.
  ~RtpImageReceiverNode();

  // Disable copy and assign.
  RtpImageReceiverNode(const RtpImageReceiverNode&) = delete;
  RtpImageReceiverNode& operator=(const RtpImageReceiverNode&) = delete;

 private:
  // Creates ROS header with timestamp information.
  // @param timestamp: RTP timestamp information
  // @return: ROS message header
  std_msgs::Header CreateHeader(
      const ImageReceiver::RTPTimestamp& timestamp);

  // Publishing helper methods
  void PublishRaw(const uint8_t* data, size_t size,
                  const std_msgs::Header& header);
  void PublishCompressed(const uint8_t* data, size_t size,
                         const std_msgs::Header& header);
  void PublishCameraInfo(const std_msgs::Header& header);

  // Callback methods for different modes
  void JpegCallback(const uint8_t* data, size_t size,
                    const ImageReceiver::RTPTimestamp& timestamp);
  void RawCallback(const uint8_t* data, size_t size,
                   const ImageReceiver::RTPTimestamp& timestamp);
  void CombinedCallback(const uint8_t* raw_data, size_t raw_size,
                        const uint8_t* jpeg_data, size_t jpeg_size,
                        const ImageReceiver::RTPTimestamp& timestamp);

  // Statistics publishing callback.
  void PublishStatistics(const ros::TimerEvent&);

  // ROS handles
  ros::NodeHandle nh_;
  ros::NodeHandle private_nh_;
  image_transport::ImageTransport it_;

  // Publishers
  std::unique_ptr<ImageReceiver> receiver_;
  ros::Publisher compressed_pub_;
  ros::Publisher camera_info_pub_;
  image_transport::Publisher raw_pub_;
  ros::Timer stats_timer_;
  std::shared_ptr<camera_info_manager::CameraInfoManager>
      camera_info_manager_;

  // Configuration
  ReceiverConfig config_;
  bool publish_raw_;
  bool publish_compressed_;
  std::string frame_id_;
  uint64_t frame_count_;
};

}  // namespace rtp_image_receiver

#endif  // GST_RTP_IMAGE_RECEIVER_INCLUDE_RTP_IMAGE_RECEIVER_HPP_
