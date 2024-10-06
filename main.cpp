#include <iostream>
#include "libfiemap/image_manager.h"
#include "android-base/file.h"
#include "libfiemap/split_fiemap_writer.h"
#include <chrono>

std::string getInputString(const std::string &msg, const std::string &def) {
    std::cout << msg << (def.empty() ? "" : " (type `d` to default [" + def + "])") << std::flush;
    std::string out;
    std::cin >> out;
    if (out == "d") {
        std::cout << "defaulting to " << def << "\n";
        out = def;
    }
    return out;
}

int main() {
    android::fiemap::IImageManager::DeviceInfo deviceInfo;
    deviceInfo.is_recovery = false;

    std::string metadata_dir;
    std::string data_dir;
    std::string dsuInstallDirFile;

    metadata_dir = getInputString("Input metadata_dir:", "/metadata/gsi/dsu/dsu");
    dsuInstallDirFile = getInputString("Input install_dir file path:", "/metadata/gsi/dsu/dsu/install_dir");

    {
        std::string path;
        if (android::base::ReadFileToString(dsuInstallDirFile, &path)) {
            data_dir = path;
        }
    }

    std::unique_ptr<android::fiemap::ImageManager> imageManager =
            android::fiemap::ImageManager::Open(metadata_dir, data_dir, deviceInfo);
    auto images = imageManager->GetAllBackingImages();
    if (images.empty()) {
        std::cerr << "Backing image not found\n";
        return -1;
    }

    for (int i = 0; i < images.size(); ++i) {
        std::cout << "Find backing image " << images[i] << " index " << i << "\n";
    }

    int i = 0;
    while (true) {
        std::string str = getInputString("Which image do you want to update (index):", "");
        try {
            i = std::stoi(str);
            if (i < images.size())
                break;
            else {
                std::cerr << "Out of range [0," << images.size() - 1 << "]\n";
            }
        } catch (std::exception &e) {
            std::cerr << "Unable to parse " << str << " to Integer, retry\n";
        }
    }

    std::string targetImg = images[i];
    std::cout << "Selected " << targetImg << "\n";
    auto mapped = android::fiemap::MappedDevice::Open(imageManager.get(), std::chrono::seconds(5), targetImg);
    if (mapped.get() == nullptr) {
        std::cerr << "MappedDevice::Open " << targetImg << " failed\n";
        return -1;
    }

    std::cerr << "  Mapped path " << mapped->path() << "\n";
    std::cerr << "  Mapped fd " << mapped->fd() << "\n";

    std::string newImagePath;
    while (true) {
        std::string path = getInputString("New image path: ", "");
        if (access(path.c_str(), F_OK)) {
            std::cerr << "Access " << path << " failed: " << std::string(strerror(errno)) << ", retry\n";
        } else {
            newImagePath = path;
            break;
        }
    }

    int rfd = open(newImagePath.c_str(), O_RDONLY);
    if (rfd == -1) {
        std::cerr << "Open " << newImagePath << " failed: " << std::string(strerror(errno)) << "\n";
        return -1;
    }

    struct stat st{};
    if (fstat(rfd, &st) == -1) {
        std::cerr << "Fstat " << newImagePath << " failed: " << std::string(strerror(errno)) << "\n";
        close(rfd);
        return -1;
    }
    off_t total_size = st.st_size;
    off_t copied_size = 0;


    uint8_t buf[1024];

    auto start_time = std::chrono::steady_clock::now();
    auto last_print_time = start_time;

    while (true) {
        ssize_t size = read(rfd, buf, sizeof(buf));
        if (size < 0) {
            std::cerr << "Read " << newImagePath << " failed: " << std::string(strerror(errno)) << "\n";
            close(rfd);
            return -1;
        } else if (size == 0) {
            std::cout << "Writing " << newImagePath << " finished, " << copied_size << " bytes in total.\n";
            break;
        }

        write(mapped->fd(), buf, size);
        copied_size += size;

        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - last_print_time);

        if (elapsed_time.count() >= 1) {
            std::cout << "[Copied: " << copied_size << ", total: " << total_size << "]\n";
            last_print_time = current_time;
        }
    }

    if (close(rfd) == -1) {
        std::cerr << "Close " << newImagePath << " failed: " << std::string(strerror(errno)) << "\n";
        return -1;
    }
    return 0;
}