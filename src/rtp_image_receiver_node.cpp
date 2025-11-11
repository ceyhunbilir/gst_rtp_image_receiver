#include "rtp_image_receiver.hpp"
#include <ros/ros.h>

int main(int argc, char** argv) {
    ros::init(argc, argv, "rtp_image_receiver_node");
    ros::NodeHandle nh;
    ros::NodeHandle private_nh("~");

    rtp_image_receiver::RtpImageReceiverNode node(nh, private_nh);

    ros::spin();

    return 0;
}
