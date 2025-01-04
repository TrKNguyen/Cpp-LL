#include <string> 
#include <vector> 
#include <unordered_map>
#include <iostream> 
#include <map> 

using DataSet = std::vector<long>; 
using DataSets = std::unordered_map<std::string, DataSet>; 

DataSets dataSets; 

DataSet intersections(const std::vector<std::string>& names, int left, int right, const DataSets& dataSets) {
    if (left > right) {
        return std::vector<long>();
    }
    if (left == right) {
        return dataSets.at(names.at(left));
    }
    int mid = (left + right) / 2; 
    DataSet leftHalf = intersections(names, left, mid, dataSets); 
    DataSet rightHalf = intersections(names, mid + 1, right, dataSets); 
    DataSet result; 
    int i = 0; // iterator for leftHalf 
    int j = 0; // iterator for rightHalf 
    while (i < leftHalf.size() && j < rightHalf.size()) {
        if (leftHalf[i] > rightHalf[j]) {
            result.emplace_back(rightHalf[j]);
            j++;
        } else {
            result.emplace_back(leftHalf[i]);
            i++;
        } 
    }
    while (i < leftHalf.size()) {
        result.emplace_back(leftHalf[i]);
        i++;
    }
    while (j < rightHalf.size()) {
        result.emplace_back(rightHalf[j]);
        j++;
    }
    return result;
}

int main() {    
    DataSet st1{1, 2, 3, 4, 5}; 
    DataSet 
}