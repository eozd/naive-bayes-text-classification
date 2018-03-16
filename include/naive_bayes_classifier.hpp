#pragma once

#include <vector>

template <typename ClassType>
class naive_bayes_classifier {
public:
    naive_bayes_classifier& fit();
    std::vector<ClassType> predict();
};


