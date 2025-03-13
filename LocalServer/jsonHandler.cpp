// Load and save data from/to the JSON file
#include <iostream>
#include <fstream>
#include <string>
#include "jsonHandler.h"
#include "misc.h"

std::string dataFile = "json-server.json";

void ensureFileExists() {
    std::ifstream file(dataFile);
    if (!file.good()) {
        std::ofstream newFile(dataFile, std::ios::out);
        if (newFile.is_open()) {
            newFile << "{}";
            newFile.close();
        }
        else {
            std::cerr << "Error: Unable to create json-server.json" << std::endl;
        }
    }
}

void saveData(const json& data) {
    ensureFileExists();

    std::ofstream file(dataFile, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open json-server.json for writing." << std::endl;
        return;
    }

    file << data.dump(4);
    file.flush();
    file.close();

    std::cout << "Data saved successfully!" << std::endl;
}

json loadData() {
    std::ifstream file(dataFile);

    if (!file.is_open()) {
        std::cerr << "Warning: Could not open JSON file. Creating a new one.\n";
        std::ofstream newFile(dataFile);
        if (newFile.is_open()) {
            newFile << "{}";
            newFile.close();
        }
        return json::object();
    }

    json data;
    try {
        file >> data;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading JSON: " << e.what() << std::endl;
        return json::object();
    }

    file.close();
    return data;
}


// Function to process client request
std::string processRequest(const std::string& request) {

    try {
        json received;
        try {
            received = json::parse(request);
        }
        catch (...) {
            return R"({"status":"error","message":"Invalid JSON"})";
        }

        json data = loadData();
        std::string action = received["action"];

        if (action == "REGISTER") {
            // Handle registration
            std::string username = received["user"];
            std::string password = received["pass"];
            std::string dateCreated = received["DateCreated"]; 
            if (data.contains(username)) {
                return R"({"status":"error","message":"User already exists"})";
            }

           
            std::string secretID = std::to_string(generateRandomNumber(10000000, 9999999999));

            data[username]["password"] = password;
            data[username]["secretID"] = secretID;
            data[username]["DateCreated"] = dateCreated;
            data[username]["hasAccount"] = true;

            data[username]["stats"] = {
                {"CollaborationsMade", 0},
                {"CollaborationsJoined", 0},
                {"CollaborationPoints", 0.0f},
                {"TotalLikes", 0.0f},
                {"CollaborationsWeek", 0},
                {"GroupsJoinedWeek", 0},
                {"LikesWeek", 0},
                {"ContributionPointsWeek", 0}
            };

            saveData(data);
            return R"({"status":"success","message":"Registration successful","secretID":")" + secretID + R"("})";
        }
        else if (action == "LOGIN") {
            std::string username = received["user"];
            std::string password = received["pass"];

            if (data.contains(username)) {
                if (data[username]["password"] == password) {
                    json userData = data[username]; 
                    return R"({"status":"success","message":"Login successful","userData":)" + userData.dump() + R"(})";
                }
                else {
                    return R"({"status":"error","message":"Incorrect password"})";
                }
            }
            else {
                return R"({"status":"error","message":"User not found"})";
            }
        }

        else if (action == "CHANGE_PASSWORD") {
            std::string secretID = received["secretID"];
            std::string newPassword = received["newPassword"];

            if (data.contains(secretID)) {
                data[secretID]["password"] = newPassword;

                saveData(data);

                return R"({"status":"success","message":"Password updated successfully"})";
            }
            else {
                return R"({"status":"error","message":"Invalid secretID"})";
            }
        }
        else if (action == "SYNC_DATA") {
            std::string secretID = received["secretID"];

            if (data.contains(secretID)) {
                std::string userDataJson = data[secretID].dump();

                return R"({"status":"success","userData":)" + userDataJson + R"("})";
            }
            else {
                return R"({"status":"error","message":"Invalid secretID"})";
            }
        }
        else if (action == "ERASE_ACCOUNT") {
            std::string username = received["user"].get<std::string>();
            std::string secretID = received["secretID"].get<std::string>();
            if (data.contains(username)) {
                if (data[username]["secretID"] == secretID) {
                    data.erase(username);
                    saveData(data);
                    return R"({"status":"success","message":"Account erased successfully"})";
                }
                else {
                    return R"({"status":"error","message":"Invalid secretID for the given user"})";
                }
            }
            else {
                return R"({"status":"error","message":"User not found"})";
            }
        }
        else if (action == "SYNC_WORKSPACE") {
            std::string name = received["name"];
            std::string content = received["content"];
            std::string secretID = received["secretID"];

            json data = loadData();
            data["workspaces"][name] = { {"content", content}, {"secretID", secretID} };

            saveData(data);

            return R"({"status":"success","message":"Workspace synced"})";
        }
        else if (action == "FETCH_WORKSPACES") {

            json data = loadData();
            if (data.contains("workspaces")) {
                json workspaces = data["workspaces"];
                return R"({"status":"success","workspaces":)" + workspaces.dump() + R"(})";
            }
            else {
                return R"({"status":"error","message":"No workspaces found"})";
            }
        }
        else if (action == "FETCH_WORKSPACE_CONTENT") {
            if (!received.contains("workspaceName") || received["workspaceName"].is_null()) {
                return R"({"status":"error","message":"Workspace name missing or null"})";
            }

            std::string workspaceName = received["workspaceName"];
            if (data["workspaces"].contains(workspaceName)) {
                std::string content = data["workspaces"][workspaceName]["content"];
                return R"({"status":"success","content":")" + content + R"("})";
            }
            else {
                return R"({"status":"error","message":"Workspace not found"})";
            }
        }
    }
    catch (const std::exception& e) {
        std::cout << "ERR : " << e.what();
    }

    return R"({"status":"error","message":"Unknown action"})";
}
