#include "image_receiver.h"

// Include implementation
#include "image_receiver_impl.cpp"

// ImageReceiver implementation
ImageReceiver::ImageReceiver(const ReceiverConfig& config) 
    : pImpl(std::make_unique<Impl>(config)) {}

ImageReceiver::~ImageReceiver() = default;

bool ImageReceiver::start() {
    return pImpl->start();
}

void ImageReceiver::stop() {
    pImpl->stop();
}

void ImageReceiver::setRawFrameCallback(FrameCallback callback) {
    pImpl->setRawFrameCallback(callback);
}

void ImageReceiver::setJpegFrameCallback(FrameCallback callback) {
    pImpl->setJpegFrameCallback(callback);
}

void ImageReceiver::setCombinedFrameCallback(CombinedFrameCallback callback) {
    pImpl->setCombinedFrameCallback(callback);
}

ImageReceiver::Statistics ImageReceiver::getStatistics() const {
    return pImpl->getStatistics();
}