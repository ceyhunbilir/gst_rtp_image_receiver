# ROS1 RTP Image Receiver Node via Gstreamer

ROS1 Noetic node for processing real-time video streams from UDP/RTP sources. This node is designed to handle YCbCr 4:2:2 format video, convert it to BGR for display, and then compress it into JPEG format. It leverages hardware acceleration (VA-API, NVIDIA, Jetson) for optimal performance.

## 🚀 Features

Hardware-accelerated video processing for high-speed performance.
Publishes both raw and compressed image topics.
All settings are configurable via ROS1 parameters.
Real-time performance metrics and debugging information.

## ⚡️ Topics

### Published Topics

- `~/image_raw/compressed` (`sensor_msgs/CompressedImage`): The processed and JPEG-compressed images.
- `~/image_raw` (`sensor_msgs/Image`): The raw BGR images. This topic is optional and can be enabled via parameters.
- `~/camera_info` (`sensor_msgs/CameraInfo`): Camera calibration information.

## ⚙️ Parameters

| Parameter | Type | Default | Description |
|---|---|---|---|
| `udp_port`            | `int`   | `5008`      | UDP port for the RTP stream. |
| `jpeg_quality`        | `int`   | `90`        | JPEG compression quality (`0`-`100`). |
| `buffer_size`         | `int`   | `8388608`   | Buffer size for the UDP socket in bytes. |
| `max_buffers`         | `int`   | `3`         | Maximum number of buffers to use. |
| `publish_raw`         | `bool`  | `false`     | If `true`, publishes the `~/image_raw` topic. |
| `publish_compressed`  | `bool`  | `true`      | If `true`, publishes the `~/image_compressed` topic. |
| `frame_id`            | `string`| `"camera"`  | Frame ID for published images. |

## 🛠️ Build Instructions

### Prerequisites

You'll need to install the necessary ROS1 Noetic and GStreamer dependencies.

```
# Install ROS1 Noetic dependencies
sudo apt install ros-noetic-cv-bridge ros-noetic-image-transport ros-noetic-compressed-image-transport ros-noetic-camera-info-manager

# Install GStreamer dependencies
sudo apt-get install \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-vaapi \
    gstreamer1.0-tools

# Install OpenCV
sudo apt install libopencv-dev
```

### Build

Navigate to your catkin workspace and build the package.

```
# Source ROS1 Noetic
source /opt/ros/noetic/setup.bash

# Build the package using catkin build
cd ~/catkin_ws
catkin build gst_rtp_image_receiver

# Or using catkin_make
# catkin_make --pkg gst_rtp_image_receiver

# Source the workspace to use the built packages
source devel/setup.bash
```


## 🚀 Usage

### Run the node

You can run the node directly from the command line, with or without parameters.

```
# Basic usage
rosrun gst_rtp_image_receiver rtp_image_receiver_node

# With custom parameters using rosrun
rosrun gst_rtp_image_receiver rtp_image_receiver_node _udp_port:=5008 _jpeg_quality:=95 _publish_raw:=true
```

### Using launch files

The node can also be launched using ROS1 launch files for easier configuration.

- Basic launch file usage

    ```
    roslaunch gst_rtp_image_receiver rtp_image_receiver.launch.xml
    ```

- With launch file arguments

    ```
    roslaunch gst_rtp_image_receiver rtp_image_receiver.launch.xml \
        udp_port:=5008 \
        publish_raw:=true \
        namespace:=my_camera
    ```

- Multi-camera setup

    ```
    roslaunch gst_rtp_image_receiver multi_camera.launch.xml
    ```

## 🔬 Testing

### Send a test RTP stream

Use GStreamer to generate a test pattern and stream it via RTP.

```
gst-launch-1.0 videotestsrc ! \
    video/x-raw,width=2880,height=1860,format=UYVY,framerate=30/1 ! \
    rtpvrawpay ! udpsink host=127.0.0.1 port=5008
```

### View the output

Use `rqt_image_view` to subscribe to the published topics and display the images.

- View compressed images

    ```
    rosrun rqt_image_view rqt_image_view
    ```

### Monitor topics

You can use standard ROS1 CLI tools to monitor the published topics.

```
# List all active topics
rostopic list

# Echo compressed image info (useful for checking header and metadata)
rostopic echo /rtp_receiver/rtp_image_receiver/image_raw/compressed --noarr

# Check the publishing rate
rostopic hz /rtp_receiver/rtp_image_receiver/image_raw/compressed
```

## 🏎️ Performance Tuning

### CPU Governor (for x86 systems)

Set the CPU governor to performance for maximum throughput.

```
echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
```

### Jetson Optimization (for NVIDIA Jetson platforms)

Use the built-in Jetson tools to maximize performance.

```
sudo jetson_clocks
sudo nvpmodel -m 0
```

## 🐛 Debugging

### Enable GStreamer debug output

Set the GST_DEBUG environment variable to get verbose GStreamer output.

```
export GST_DEBUG=3
rosrun gst_rtp_image_receiver rtp_image_receiver_node
```

### Check node info

Get detailed information about the running node, including subscribed and published topics.

```
rosnode info /rtp_receiver/rtp_image_receiver
```