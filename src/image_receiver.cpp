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

void ImageReceiver::setFrameCallback(FrameCallback callback) {
    pImpl->setFrameCallback(callback);
}

ImageReceiver::Statistics ImageReceiver::getStatistics() const {
    return pImpl->getStatistics();
}