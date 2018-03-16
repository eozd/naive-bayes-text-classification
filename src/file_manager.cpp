#include <dirent.h>

#include "file_manager.hpp"
#include <algorithm>
#include <fstream>

std::vector<std::string> ir::get_data_file_list() {
    DIR* dirp = opendir(DATASET_DIR.c_str());

    struct dirent* dp;
    std::string filename;
    std::vector<std::string> file_list;
    const std::string ext = ".sgm";

    while ((dp = readdir(dirp)) != nullptr) {
        filename = dp->d_name;

        // if filename ends with the extension
        if (filename.find(ext) == filename.size() - ext.size()) {
            file_list.push_back(DATASET_DIR + '/' + filename);
        }
    }

    closedir(dirp);

    std::sort(file_list.begin(), file_list.end());
    return file_list;
}

