#include <fstream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

void saveData(const json& data);
json loadData();
std::string processRequest(const std::string& request);