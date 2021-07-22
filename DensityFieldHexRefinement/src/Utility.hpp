#ifndef DFHR_UTILITY_HPP
#define DFHR_UTILITY_HPP

#include <eigen3/Eigen/Eigen>
#include <vector>
#include <unordered_map>

#define REFINE_EPSILON  1e-8

template <class T>
inline bool isInVec(T &e, std::vector<T> &v){
    return (find(v.begin(), v.end(), e) == v.end()) ? false : true;
}

template <class T, class S>
inline bool isInMap(T &e, std::unordered_map<T, S> &v){
    return (v.find(e) == v.end()) ? false : true;
}

#endif