#include <iostream>
#include "libfiemap/image_manager.h"

int main() {
    android::fiemap::IImageManager::DeviceInfo deviceInfo;
    deviceInfo.is_recovery = false;
    std::unique_ptr<android::fiemap::ImageManager> imageManager =
            android::fiemap::ImageManager::Open("dsu/dsu", deviceInfo);
    auto images = imageManager->GetAllBackingImages();
    for (const auto &item: images) {
        std::cout << "find backing image " << item << "\n";
    }
    return 0;
}