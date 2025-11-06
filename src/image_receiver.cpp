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