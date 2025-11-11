#include <nodelet/nodelet.h>
#include <pluginlib/class_list_macros.h>
#include <ros/ros.h>

#include "rtp_image_receiver.hpp"

namespace rtp_image_receiver {

// Nodelet wrapper for RtpImageReceiverNode.
// This allows the RTP image receiver to run as a nodelet for efficient
// zero-copy message passing when used with other nodelets.
class RtpReceiverNodelet : public nodelet::Nodelet {
 public:
  RtpReceiverNodelet() {}
  ~RtpReceiverNodelet() {}

 private:
  // Nodelet initialization method.
  // Called when the nodelet is loaded. Creates the RtpImageReceiverNode
  // instance with the nodelet's node handles.
  virtual void onInit() {
    ros::NodeHandle& nh = getNodeHandle();
    ros::NodeHandle& private_nh = getPrivateNodeHandle();

    NODELET_INFO("Initializing RTP Image Receiver Nodelet");

    try {
      node_.reset(new RtpImageReceiverNode(nh, private_nh));
    } catch (const std::exception& e) {
      NODELET_ERROR("Failed to initialize RTP Image Receiver Nodelet: %s",
                    e.what());
      throw;
    }
  }

  // Instance of the actual node implementation
  std::unique_ptr<RtpImageReceiverNode> node_;
};

}  // namespace rtp_image_receiver

// Register the nodelet with pluginlib
PLUGINLIB_EXPORT_CLASS(rtp_image_receiver::RtpReceiverNodelet,
                       nodelet::Nodelet)
