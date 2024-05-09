//registers //vexec //vmem   //vwriteback 
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include<fstream>
#include<iomanip>
#include <map>
#include<algorithm>
#include <bitset>
#include<math.h>
#include<limits.h>
#include<sstream>
#include<random>
class SIMD
{
    std::unordered_map<std::string, std::vector<int>> vregisters{
    {"v0", {0,0,0,0}}, {"v1", {0,0,0,0}}, {"v2", {0,0,0,0}}, {"v3", {0,0,0,0}},
    {"v4", {0,0,0,0}}, {"v5", {0,0,0,0}}, {"v6", {0,0,0,0}}, {"v7", {0,0,0,0}}
};
std::vector<int> vexecute();

};