#include <iostream>
#include <string>
#include <vector>
#include "launchUtil.h"
using namespace std;

#include <dirent.h>
int existVersionJar(string filePath,string jarName){
	string folderPath = filePath+"/"+jarName;
    DIR* dir;
    if ((dir = opendir(folderPath.c_str())) != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
        	if(entry->d_name == jarName+".jar"){
        		return 1;
			}
        }
        closedir(dir);
    }
	return 0;
}
vector<string> findVersion(string dirPath){
	vector<string> version;
	string folderPath = dirPath+"/versions";
    DIR* dir;
    if ((dir = opendir(folderPath.c_str())) != nullptr) {
        struct dirent* entry;
        int index=0;
        while ((entry = readdir(dir)) != nullptr) {
        	if(index == 2){
        		if(existVersionJar(folderPath,entry->d_name) == 1){
        			version.push_back(entry->d_name);
				};
			}
        	else{
        		index++;
			}
        }
        closedir(dir);
    }
    return version;
}
#include <regex>  
#include <stdexcept>
vector<string> getLibPaths(string str){
	vector<string> libPaths;
	try {  
        regex pathRegex("(\"path\": \")([^\"]+)(\")");  
        smatch match;  
        auto pos = sregex_iterator(str.begin(), str.end(), pathRegex);  
        auto end = sregex_iterator();  
  
        while (pos != end) {  
            string pathValue = pos->str(2);
            if(pathValue.find("linux")==string::npos && pathValue.find("osx")==string::npos){
            	auto flag = find(libPaths.begin(), libPaths.end(), pathValue);
            	if(flag == libPaths.end()){
            		libPaths.push_back(pathValue);
				}
			}
            ++pos;
        }
    } catch (const regex_error& e) {  
        cerr << "Regex error: " << e.what() << endl;  
        throw;
    }
    return libPaths;
}
vector<string> getLibUrls(string str){
	vector<string> libUrls;
	try {
        regex pathRegex("(\"url\": \")([^\"]+)(\")");
        smatch match;  
        auto pos = sregex_iterator(str.begin(), str.end(), pathRegex);  
        auto end = sregex_iterator();  
  		++pos;//用于去除Mc的Json文件最顶部的url 
        while (pos != end) {  
            string pathValue = pos->str(2);
            libUrls.push_back(pathValue);
            ++pos;
        }
    } catch (const regex_error& e) {  
        cerr << "Regex error: " << e.what() << endl;  
        throw;
    }
    return libUrls;
}
string getAssetIndex(string json){
	string re;
	try {  
        regex pathRegex("(\"assets\": \")([^\"]+)(\",)");  
        smatch match;  
        auto pos = sregex_iterator(json.begin(), json.end(), pathRegex);  
        auto end = sregex_iterator();  
        while (pos != end) {  
            re = pos->str(2);
            ++pos;
        }
    } catch (const regex_error& e) {  
        cerr << "Regex error: " << e.what() << endl;  
        throw;
    }
    return re;
}
string getMainClass(string json){
	string re;
	try {  
        regex pathRegex("(\"mainClass\": \")([^\"]+)(\",)");  
        smatch match;  
        auto pos = sregex_iterator(json.begin(), json.end(), pathRegex);  
        auto end = sregex_iterator();  
        while (pos != end) {  
            re = pos->str(2);
            ++pos;
        }
    } catch (const regex_error& e) {  
        cerr << "Regex error: " << e.what() << endl;  
        throw;
    }
    return re;
}
string getTweakClass(string json){
	string re;
	try {  
        regex pathRegex("(--tweakClass )([^\"]+)(\",)");  
        smatch match;  
        auto pos = sregex_iterator(json.begin(), json.end(), pathRegex);  
        auto end = sregex_iterator();  
        while (pos != end) {  
            re = pos->str(2);
            ++pos;
        }
    } catch (const regex_error& e) {  
        cerr << "Regex error: " << e.what() << endl;  
        throw;
    }
    return re;
}
int isOptifine(string json){
	if(json.find("optifine")!=string::npos){
		return 1;
	}
    return 0;
}

//字符串分割，但是输出第二个字符串 
vector<string> splitStr(string str, string delimiter) {
    vector<string> tokens;
    size_t pos = 0;
    string token;
    while ((pos = str.find(delimiter)) != string::npos) {
        token = str.substr(0, pos);
        tokens.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    // 添加最后一个token（如果存在）
    if (!str.empty()) {
        tokens.push_back(str);
    }
    return tokens;
}


vector<string> getOptifineLib(string json){
	string splitJson = splitStr(json,"\"id\": \"optifine\",")[1];
	vector<string> libPaths;
	try {
        regex pathRegex("(\"name\": \")([^\"]+)(\")");  
        smatch match;  
        auto pos = sregex_iterator(splitJson.begin(), splitJson.end(), pathRegex);  
        auto end = sregex_iterator();  
        while (pos != end) {  
            string pathValue = pos->str(2);
            vector<string> pathSplit = splitStr(pathValue,":");
            libPaths.push_back(pathSplit[0]+"/"+pathSplit[1]+"/"+pathSplit[2]+"/"+pathSplit[1]+"-"+pathSplit[2]+".jar");
            ++pos;
        }
    } catch (const regex_error& e) {  
        cerr << "Regex error: " << e.what() << endl;  
        throw;
    }
    return libPaths;
}




#include <fstream>
string readFile(string filePath){
	ifstream file(filePath);
	string line;
	string result;
	if(file.is_open()){
		while (getline(file, line)) {
            result+=line;
        }
        file.close();
	}
	return result;
}


//UUID生成 
#include <sstream>
#include <random>
#include <chrono>
string generateUUID() {  
    auto now = chrono::system_clock::now();  
    auto now_c = chrono::system_clock::to_time_t(now);  
    random_device rd;  
    mt19937 gen(rd());  
    uniform_int_distribution<> dis(0, 15);  
    uniform_int_distribution<> hex_dis(0, 15);  
    stringstream ss;
    for (int i = 0; i < 4; ++i) {
        ss << hex << dis(gen);  
    }
    for (int i = 0; i < 4; ++i) {
        ss << hex << dis(gen);  
    }
    for (int i = 0; i < 4; ++i) {
        ss << hex << dis(gen);  
    }
    ss << hex << now_c;
    for (int i = 0; i < 4; ++i) {
        ss << hex << hex_dis(gen);  
    }
    for (int i = 0; i < 4; ++i) {
        ss << hex << hex_dis(gen);  
    }
    for (int i = 0; i < 4; ++i) {
        ss << hex << hex_dis(gen);  
    }  
    return ss.str();  
}
string random_str(int len){
    auto now = chrono::system_clock::now();  
    auto now_c = chrono::system_clock::to_time_t(now);  
    random_device rd;  
    mt19937 gen(rd());  
    uniform_int_distribution<> dis(0, 15);  
    uniform_int_distribution<> hex_dis(0, 15);  
    stringstream ss;
    for (int i = 0; i < len; ++i) {
        ss << hex << hex_dis(gen);  
    }
    return ss.str();
}
