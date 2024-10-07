#!/bin/bash

cd "$(dirname "$0")" || exit 1

echo "Cloning repositories..."

#URL="https://mirrors.bfsu.edu.cn/git/AOSP"
URL="https://android.googlesource.com"

git clone "$URL"/platform/system/core.git/ platform-system-core
git clone "$URL"/platform/system/extras.git/ platform-system-extras
git clone "$URL"/platform/system/gsid.git/ platform-system-gsid
git clone "$URL"/platform/system/libbase.git/ platform-system-libbase
git clone "$URL"/platform/system/logging.git/ platform-system-logging

echo "Checking out specific commits..."

git -C platform-system-core checkout e563c3f48a2478ea909f2fe08790e53bc7787b70
git -C platform-system-extras checkout e963aa41bb0292a0fb96ee0c4b29df692b940cad
git -C platform-system-gsid checkout 4d095e34734edbccf50dcbaa820884a1860bcaa6
git -C platform-system-libbase checkout 38c889f97dd421e8d83cb15c9c50f2b557efcfcc
git -C platform-system-logging checkout b34720941c1a18249615ebed5a25d05ff12bfad1

echo "Applying patches..."

for patch_dir in patch/*/; do
    project=$(basename "$patch_dir")
    repo_dir="platform-$project"

    if [ -d "$repo_dir" ]; then
        echo "Found patches for $repo_dir, applying..."
        for patch_file in "$patch_dir"*.patch; do
            if [ -f "$patch_file" ]; then
                echo "Applying patch $patch_file to $repo_dir"
                git -C "$repo_dir" apply "$(realpath "$patch_file")"
            fi
        done
    else
        echo "Repository not found for $project"
    fi
done