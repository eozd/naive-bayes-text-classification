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

std::ostream& ir::write_dataset(std::ostream& os,
                                const doc_term_index& term_index,
                                const doc_class_index& class_index) {
    for (const auto& pair : term_index) {
        const size_t id = pair.first;
        const auto& doc_terms_counts = pair.second;
        const auto& classes = class_index.at(id);

        os << id;
        for (const auto doc_class : classes) {
            os << ' ' << doc_class;
        }
        os << '\n';
        for (const auto& term_count_pair : doc_terms_counts) {
            const auto& term = term_count_pair.first;
            const size_t count = term_count_pair.second;

            os << term << " " << count << '\n';
        }

        os << '\n';
    }
    os << std::flush;

    return os;
}
