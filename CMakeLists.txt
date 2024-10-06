cmake_minimum_required(VERSION 3.30)
project(dsu_updater)

set(CMAKE_CXX_STANDARD 23)

link_libraries("-static")

add_compile_options(-ftrivial-auto-var-init=zero)

########
find_library(UUID_STATIC_LIB NAMES libuuid.a REQUIRED)
find_library(Z_STATIC_LIB NAMES libz.a REQUIRED)
find_library(SSL_STATIC_LIB NAMES libcrypto.a REQUIRED)

if (UUID_STATIC_LIB)
    message("find libuuid.a at ${UUID_STATIC_LIB}")
else ()
    fatal("libuuid.a not found")
endif ()

if (Z_STATIC_LIB)
    message("find libz.a at ${Z_STATIC_LIB}")
else ()
    fatal("libz.a not found")
endif ()

if (SSL_STATIC_LIB)
    message("find libcrypto.a at ${SSL_STATIC_LIB}")
else ()
    fatal("libcrypto.a not found")
endif ()

########
add_library(base STATIC
        platform-system-libbase/file.cpp
        platform-system-libbase/strings.cpp
        platform-system-libbase/logging.cpp
        platform-system-libbase/properties.cpp
        platform-system-libbase/stringprintf.cpp
        platform-system-logging/liblog/properties.cpp
        platform-system-logging/liblog/logger_write.cpp
        platform-system-libbase/parsebool.cpp
        platform-system-libbase/threads.cpp
        platform-system-libbase/mapped_file.cpp
        platform-system-libbase/posix_strerror_r.cpp
)

target_include_directories(base PUBLIC
        platform-system-libbase/include
        platform-system-logging/liblog/include
)

#######
add_library(ext4_utils STATIC
        platform-system-extras/ext4_utils/ext4_utils.cpp
        platform-system-extras/ext4_utils/ext4_sb.cpp
)

target_include_directories(ext4_utils
        PUBLIC platform-system-extras/ext4_utils/include
)

#######
add_library(gsid STATIC
        platform-system-gsid/libgsi.cpp
)

target_include_directories(gsid PUBLIC
        platform-system-gsid/include
)

target_link_libraries(gsid PUBLIC base)

#######
add_library(sparse STATIC
        platform-system-core/libsparse/sparse.cpp
        platform-system-core/libsparse/backed_block.cpp
        platform-system-core/libsparse/output_file.cpp
        platform-system-core/libsparse/sparse_read.cpp
        platform-system-core/libsparse/sparse_crc32.cpp
        platform-system-core/libsparse/sparse_err.cpp
)

target_include_directories(sparse PUBLIC
        platform-system-core/libsparse/include
)

target_link_libraries(sparse PUBLIC base)

########
add_library(fs_mgr STATIC
        platform-system-core/fs_mgr/libfiemap/fiemap_status.cpp
        platform-system-core/fs_mgr/libfiemap/fiemap_writer.cpp
        platform-system-core/fs_mgr/libfiemap/image_manager.cpp
        platform-system-core/fs_mgr/libfiemap/utility.cpp
        platform-system-core/fs_mgr/libfiemap/metadata.cpp
        platform-system-core/fs_mgr/libfiemap/fiemap_writer.cpp
        platform-system-core/fs_mgr/libfiemap/split_fiemap_writer.cpp
        platform-system-core/fs_mgr/libdm/dm.cpp
        platform-system-core/fs_mgr/liblp/images.cpp
        platform-system-core/fs_mgr/liblp/partition_opener.cpp
        platform-system-core/fs_mgr/liblp/utility.cpp
        platform-system-core/fs_mgr/liblp/reader.cpp
        platform-system-core/fs_mgr/liblp/builder.cpp
        platform-system-core/fs_mgr/liblp/writer.cpp
        platform-system-core/fs_mgr/fs_mgr_dm_linear.cpp
        platform-system-core/fs_mgr/libdm/dm_target.cpp
        platform-system-core/fs_mgr/libdm/loop_control.cpp
        platform-system-core/fs_mgr/liblp/property_fetcher.cpp
        platform-system-core/fs_mgr/libdm/utility.cpp
        platform-system-core/fs_mgr/libdm/dm_table.cpp
        platform-system-core/fs_mgr/libfstab/slotselect.cpp
        platform-system-core/fs_mgr/libfstab/boot_config.cpp
        platform-system-core/fs_mgr/libfstab/fstab.cpp
)

target_include_directories(fs_mgr PUBLIC
        platform-system-core/fs_mgr/libfiemap/include
        platform-system-core/fs_mgr/libfstab/include
        platform-system-core/fs_mgr/libdm/include
        platform-system-core/fs_mgr/liblp/include
        platform-system-core/fs_mgr/include
        platform-system-core/fs_mgr/
)

target_link_libraries(fs_mgr PUBLIC base gsid ext4_utils sparse ${UUID_STATIC_LIB} ${Z_STATIC_LIB} ${SSL_STATIC_LIB})

########
add_executable(dsu_updater main.cpp)

target_link_libraries(dsu_updater fs_mgr)