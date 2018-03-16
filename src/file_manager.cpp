#include <dirent.h>

#include "file_manager.hpp"
#include "util.hpp"
#include <algorithm>
#include <cassert>
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

void ir::write_dict_file(const pos_inv_index& inverted_index) {
    size_t index = 0;
    std::ofstream ofs(ir::DICT_PATH, std::ios_base::trunc);

    // write each term and its corresponding ID on a separate line
    for (const auto& pair : inverted_index) {
        const auto& term = pair.first;
        ofs << term << ' ' << index << '\n';
        ++index;
    }
    ofs << std::flush;
}

void ir::write_index_file(const pos_inv_index& inverted_index) {
    size_t index = 0;
    std::ofstream ofs(INDEX_PATH, std::ios_base::trunc);

    for (const auto& term_pair : inverted_index) {
        const auto& doc_vec = term_pair.second;

        ofs << index << '\n' << POS_LIST_BEG_TAG << '\n';

        // write position list of each doc that term appears in
        for (const auto& doc_pair : doc_vec) {
            size_t doc_id = doc_pair.first;
            const auto& pos_vec = doc_pair.second;
            ofs << '\t' << doc_id << " :";
            for (size_t pos : pos_vec) {
                ofs << ' ' << pos;
            }
            ofs << '\n';
        }
        ofs << POS_LIST_END_TAG << '\n';
        ++index;
    }
    ofs << std::flush;
}

ir::term_id_map ir::read_dict_file() {
    term_id_map result;
    std::string term;
    size_t id;

    std::ifstream ifs(DICT_PATH);
    if (!ifs) {
        throw std::runtime_error(DICT_PATH + " does not exist in the current folder!");
    }

    // read each line and construct the dictionary
    while (ifs >> term >> id) {
        result[term] = id;
    }

    return result;
}

ir::id_pos_map ir::read_index_file() {
    id_pos_map result;
    size_t term_id, doc_id, pos;
    std::string tag;
    std::string pos_line;

    std::ifstream ifs(INDEX_PATH);
    if (!ifs) {
        throw std::runtime_error(INDEX_PATH + " does not exist in the current folder!");
    }

    // start reading an entry by first reading the term ID
    while (ifs >> term_id) {
        // get rid of newline
        ifs.ignore(1);

        // read begin tag
        std::getline(ifs, tag);
        assert(tag == POS_LIST_BEG_TAG);

        // while end tag is not reached, continue reading
        std::getline(ifs, pos_line);
        while (pos_line != POS_LIST_END_TAG) {
            // document id on current line
            auto first_space = pos_line.find(' ');
            doc_id = std::stoul(pos_line.substr(1, first_space - 1));

            // beginning index of the position list
            size_t num_beg = first_space + 3;
            // get the rest of the line
            std::string pos_str = pos_line.substr(num_beg);
            // split tokens by space
            std::vector<std::string> pos_str_vec = split(pos_str, " ");

            // transform string positions to int positions
            std::vector<size_t> pos_vec(pos_str_vec.size());
            std::transform(
                pos_str_vec.begin(), pos_str_vec.end(), pos_vec.begin(),
                [](const std::string& pos_str) { return std::stoul(pos_str); });

            // Add <doc_id, pos_list> pair to document list of current term
            result[term_id].emplace_back(doc_id, pos_vec);

            std::getline(ifs, pos_line);
        }
    }

    return result;
}
