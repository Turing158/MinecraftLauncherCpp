#include <iostream>
#include <string>
#include <vector>
#include "launchUtil.h"
using namespace std;

//#include <filesystem>
//int existFile(string pathStr){
//	filesystem::path path = pathStr;
//	cout<<filesystem::exists(pathStr)<<endl;
//    if (filesystem::exists(path)) {
//        return 1; 
//    }
//    return 0; 
//}
#include <dirent.h>
//判断versions里的文件夹是否为minecraft的核心jar储存文件夹 
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
//查找Minecraft版本 
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
				}
			}
        	else{
        		index++;
			}
        }
        closedir(dir);
    }
    return version;
}
//查找版本的动态链接库native文件夹 
string findNativeFile(string dirPath,string version){
	string folderPath = dirPath+"/versions/"+version+"/";
    DIR* dir;
    if ((dir = opendir(folderPath.c_str())) != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
        	string str = entry->d_name;
        	if(str.find("natives")!=string::npos){
        		return entry->d_name;
			}
        }
        closedir(dir);
    }
    return "NOT_FOUND";
}
#include <regex>  
#include <stdexcept>
//获取Minecraft的基础lib文件的路径 
vector<string> getLibPaths(string str){
	vector<string> libPaths;
	try {  
        regex pathRegex("(\"path\": \")([^\"]+)(\")");  
        smatch match;  
        auto pos = sregex_iterator(str.begin(), str.end(), pathRegex);  
        auto end = sregex_iterator();  
  
        while (pos != end) {  
            string pathValue = pos->str(2);
            if(pathValue.find("linux")==string::npos && pathValue.find("osx")==string::npos && pathValue.find("macos")==string::npos){//windows系统 
//            	if(framework == "x64" && pathValue.find("-x86")==string::npos){
//            		
//				}
				auto flag = find(libPaths.begin(), libPaths.end(), pathValue);
	            if(flag == libPaths.end()){
	            	libPaths.push_back(pathValue);
				}
			}
//			if(pathValue.find("windows")==string::npos && pathValue.find("osx")==string::npos && pathValue.find("macos")==string::npos){//linux系统 
//				auto flag = find(libPaths.begin(), libPaths.end(), pathValue);
//            	if(flag == libPaths.end()){
//            		libPaths.push_back(pathValue);
//				}
//			}
//			if(pathValue.find("windows")==string::npos && pathValue.find("linux")==string::npos){
//				auto flag = find(libPaths.begin(), libPaths.end(), pathValue);
//            	if(flag == libPaths.end()){
//            		libPaths.push_back(pathValue);
//				}
//			} 
            ++pos;
        }
    } catch (const regex_error& e) {  
        cerr << "Regex error: " << e.what() << endl;  
        throw;
    }
    return libPaths;
}


//获取Minecraft的基础lib文件的下载链接 
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
//获取 assetsIndex参数 
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
//获取 mainClass参数 
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
//获取 tweakClass参数 
string getTweakClass(string json){
	string re;
	try {  
        regex pathRegex("(--tweakClass )([^\" ]+)( |\",)");  
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
    if(!re.length()){
    	try {  
	        regex pathRegex("(\"--tweakClass\",      \")([^\"]+)(\"    ],)");  
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
	}
    return re;
}
//是否为optifine 
int isOptifine(string json){
	if(json.find("optifine")!=string::npos){
		return 1;
	}
    return 0;
}
//是否为forge 
int isForge(string json){
	if(json.find("forge")!=string::npos){
		return 1;
	}
    return 0;
}
//是否为fabric
int isFabric(string json){
	if(json.find("fabric")!=string::npos){
		return 1;
	}
    return 0;
}
//字符串分割
vector<string> splitStr(string str, string delimiter) {
    vector<std::string> tokens;  
    string::size_type pos, lastPos = 0;  
    string token;  
    while ((pos = str.find(delimiter, lastPos)) != std::string::npos) {  
        token = str.substr(lastPos, pos - lastPos);  
        if (!token.empty()) {  
            tokens.push_back(token);  
        }  
        lastPos = pos + delimiter.length();  
    }  
    if (lastPos < str.length()) {  
        tokens.push_back(str.substr(lastPos));  
    }  
    return tokens;  
}
//字符串替换 
string replaceStr(string original,string oldStr,string newStr){
	if (oldStr.empty()) {
        return "";
    }
    size_t pos = 0;
    while ((pos = original.find(oldStr, pos)) != string::npos) {
        original.replace(pos, oldStr.length(), newStr);
        pos += newStr.length();
    }
    return original;
}

//获取optifine的lib 
vector<string> getOptifineLib(string json){
	vector<string> splitJsonList = splitStr(json,"\"id\": \"optifine\",");
	string splitJson;
	if(splitJsonList.size() > 1){
		splitJson = splitJsonList[1];
	}
	else{
		splitJson = json;
	}
	vector<string> libPaths;
	try {
        regex pathRegex("(\"name\": \")([^\"]+)(\")");  
        smatch match;  
        auto pos = sregex_iterator(splitJson.begin(), splitJson.end(), pathRegex);  
        auto end = sregex_iterator();  
        while (pos != end) {  
            string pathValue = pos->str(2);
            vector<string> pathSplit = splitStr(pathValue,":");
            string pathPrefix = replaceStr(pathSplit[0],".","/");
	        if(pathValue != "linux" && pathValue != "osx" && pathValue != "windows"){
	        	string path = pathPrefix+"/"+pathSplit[1]+"/"+pathSplit[2]+"/"+pathSplit[1]+"-"+pathSplit[2]+".jar";
	        	libPaths.push_back(path);
			}
            ++pos;
        }
    } catch (const regex_error& e) {  
        cerr << "Regex error: " << e.what() << endl;  
        throw;
    }
    return libPaths;
}
//获取forge的lib 
vector<string> getForgeLib(string json,vector<string> cpPath){
	vector<string> splitJsonList = splitStr(json,"\"id\": \"forge\",");
	string splitJson;
	if(splitJsonList.size() > 1){
		splitJson = splitJsonList[1];
	}
	else{
		splitJson = json;
	}
	regex pathRegex("(\"name\": \")([^\"]+)(\")");  
	auto pos = sregex_iterator(splitJson.begin(), splitJson.end(), pathRegex);
	auto end = sregex_iterator();
	vector<string> libPaths;
	while (pos != end) {
        string pathValue = pos->str(2);
        vector<string> pathSplit = splitStr(pathValue,":");
        string pathPrefix = replaceStr(pathSplit[0],".","/");
        if(pathValue != "linux" && pathValue != "osx" && pathValue != "windows"){
        	string path = pathPrefix+"/"+pathSplit[1]+"/"+pathSplit[2]+"/"+pathSplit[1]+"-"+pathSplit[2]+".jar";
        	auto flag = find(cpPath.begin(), cpPath.end(), path);
	           if(flag == cpPath.end()){
	           	libPaths.push_back(path);
			}
		}
        ++pos;
    }
    return libPaths;
}
//获取fabric的lib 
vector<string> getFabricLib(string json,vector<string> cpPath){
	vector<string> splitJsonList = splitStr(json,"\"id\": \"fabric\",");
	string splitJson;
	if(splitJsonList.size() > 1){
		splitJson = splitJsonList[1];
	}
	else{
		splitJson = json;
	}
	regex pathRegex("(\"name\": \")([^\"]+)(\")");  
	auto pos = sregex_iterator(splitJson.begin(), splitJson.end(), pathRegex);
	auto end = sregex_iterator();
	vector<string> libPaths;
	while (pos != end) {
        string pathValue = pos->str(2);
        vector<string> pathSplit = splitStr(pathValue,":");
        string pathPrefix = replaceStr(pathSplit[0],".","/");
        if(pathValue != "linux" && pathValue != "osx" && pathValue != "windows"){
        	string path = pathPrefix+"/"+pathSplit[1]+"/"+pathSplit[2]+"/"+pathSplit[1]+"-"+pathSplit[2]+".jar";
        	auto flag = find(cpPath.begin(), cpPath.end(), path);
	           if(flag == cpPath.end()){
	           	libPaths.push_back(path);
			}
		}
        ++pos;
    }
    return libPaths;
}
//获取高版本forge的参数lib	-p参数
string getForge_pPara(string json,string libDir){
	string _pPara = "";
	vector<string> split_pPara;
	try {
        regex pathRegex("(\"-p\",      \")([^\"]+)(\",)");  
        smatch match;  
        auto pos = sregex_iterator(json.begin(), json.end(), pathRegex);  
        auto end = sregex_iterator();  
        while (pos != end) {
            string pathValue = pos->str(2);
            split_pPara = splitStr(pathValue,"${classpath_separator}");
            for(int i=0;i<split_pPara.size();i++){
            	_pPara += libDir+"/libraries"+replaceStr(split_pPara[i],"${library_directory}","");
            	if(i != split_pPara.size()-1){
            		_pPara+=";";
				}
			}
            ++pos;
        }
    } catch (const regex_error& e) {  
        cerr << "Regex error: " << e.what() << endl;  
        throw;
    }
	return _pPara;
}
string extraPrePara(string json,string libraryPath){
	string natives1;
	if(json.find("-Djna.tmpdir")!=string::npos){
		natives1 = "-Djna.tmpdir="+libraryPath+" ";
	}
	string natives2;
	if(json.find("-Djna.tmpdir")!=string::npos){
		natives2 = "-Dorg.lwjgl.system.SharedLibraryExtractPath="+libraryPath+" ";
	}
	string natives3;
	if(json.find("-Djna.tmpdir")!=string::npos){
		natives3 = "-Dio.netty.native.workdir="+libraryPath+" ";
	}
	return natives1+natives2+natives3;
}
string extraMorePara(string json,string selectDir,string selectVersion){
	string IPv6Addresses;
	if(json.find("-Djna.tmpdir")!=string::npos){
		IPv6Addresses = "-Djava.net.preferIPv6Addresses=system ";
	}
	string ignoreList;
	if(json.find("-DignoreList")!=string::npos){
		ignoreList = "-DignoreList=bootstraplauncher,securejarhandler,asm-commons,asm-util,asm-analysis,asm-tree,asm,JarJarFileSystems,client-extra,fmlcore,javafmllanguage,lowcodelanguage,mclanguage,forge-,"+selectVersion+".jar,"+selectVersion+".jar ";
	}
	string dmergeModules;
	if(json.find("-DmergeModules")!=string::npos){
		try {
	        regex pathRegex("(-DmergeModules=|-DmergeModules\\u003d)([^\"]+)(\")");  
	        smatch match;  
	        auto pos = sregex_iterator(json.begin(), json.end(), pathRegex);  
	        auto end = sregex_iterator();  
	        while (pos != end) {
	            string pathValue = pos->str(2);
	            dmergeModules = "-DmergeModules="+pathValue+" ";
	            ++pos;
	        }
	    } catch (const regex_error& e) {  
	        cerr << "Regex error: " << e.what() << endl;  
	        throw;
	    }
	}
	string libraryDirectory;
	if(json.find("-DlibraryDirectory")!=string::npos){
		libraryDirectory = "-DlibraryDirectory="+selectDir+"/libraries ";
	}
	string _pPara;
	if(json.find("\"-p\",")!=string::npos){
		_pPara = "-p "+getForge_pPara(json,selectDir)+" ";
	}
	string permitPacket = "";
	if(json.find("--add-modules")!=string::npos){
		permitPacket += "--add-modules ALL-MODULE-PATH ";
	}
	if(json.find("--add-opens")!=string::npos){
		permitPacket += "--add-opens java.base/java.util.jar=cpw.mods.securejarhandler --add-opens java.base/java.lang.invoke=cpw.mods.securejarhandler ";
	}
	if(json.find("--add-exports")!=string::npos){
		permitPacket += "--add-exports java.base/sun.security.util=cpw.mods.securejarhandler --add-exports jdk.naming.dns/com.sun.jndi.dns=java.naming ";
	}
	return IPv6Addresses+ignoreList+dmergeModules+libraryDirectory+_pPara+permitPacket;
}

string extraParaNameFml(string json){
	string fmlPara = "";
	if(json.find("--launchTarget")!=string::npos){
	 	try {
	        regex pathRegex("(\"--launchTarget\",      \")([^\"]+)(\")");  
	        smatch match;  
	        auto pos = sregex_iterator(json.begin(), json.end(), pathRegex);  
	        auto end = sregex_iterator();  
	        while (pos != end) {
	            string pathValue = pos->str(2);
	            fmlPara += "--launchTarget "+pathValue+" ";
	            ++pos;
	        }
	    } catch (const regex_error& e) {  
	        cerr << "Regex error: " << e.what() << endl;  
	        throw;
	    }
	}
	if(json.find("--fml.forgeVersion")!=string::npos){
	 	try {
	        regex pathRegex("(\"--fml.forgeVersion\",      \")([^\"]+)(\")");  
	        smatch match;  
	        auto pos = sregex_iterator(json.begin(), json.end(), pathRegex);  
	        auto end = sregex_iterator();  
	        while (pos != end) {
	            string pathValue = pos->str(2);
	            fmlPara += "--fml.forgeVersion "+pathValue+" ";
	            ++pos;
	        }
	    } catch (const regex_error& e) {  
	        cerr << "Regex error: " << e.what() << endl;  
	        throw;
	    }
	}
	if(json.find("--fml.mcVersion")!=string::npos){
	 	try {
	        regex pathRegex("(\"--fml.mcVersion\",      \")([^\"]+)(\")");  
	        smatch match;  
	        auto pos = sregex_iterator(json.begin(), json.end(), pathRegex);  
	        auto end = sregex_iterator();  
	        while (pos != end) {
	            string pathValue = pos->str(2);
	            fmlPara += "--fml.mcVersion "+pathValue+" ";
	            ++pos;
	        }
	    } catch (const regex_error& e) {  
	        cerr << "Regex error: " << e.what() << endl;  
	        throw;
	    }
	}
	if(json.find("--fml.forgeGroup")!=string::npos){
	 	try {
	        regex pathRegex("(\"--fml.forgeGroup\",      \")([^\"]+)(\")");  
	        smatch match;  
	        auto pos = sregex_iterator(json.begin(), json.end(), pathRegex);  
	        auto end = sregex_iterator();  
	        while (pos != end) {
	            string pathValue = pos->str(2);
	            fmlPara += "--fml.forgeGroup "+pathValue+" ";
	            ++pos;
	        }
	    } catch (const regex_error& e) {  
	        cerr << "Regex error: " << e.what() << endl;  
	        throw;
	    }
	}
	if(json.find("--fml.mcpVersion")!=string::npos){
	 	try {
	        regex pathRegex("(\"--fml.mcpVersion\",      \")([^\"]+)(\")");  
	        smatch match;  
	        auto pos = sregex_iterator(json.begin(), json.end(), pathRegex);  
	        auto end = sregex_iterator();  
	        while (pos != end) {
	            string pathValue = pos->str(2);
	            fmlPara += "--fml.mcpVersion "+pathValue+" ";
	            ++pos;
	        }
	    } catch (const regex_error& e) {  
	        cerr << "Regex error: " << e.what() << endl;  
	        throw;
	    }
	}
	return fmlPara;
}
#include <fstream>
//读取文件，变成一行 
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

int existFile(string pathStr){
	ifstream file(pathStr);
	return file.good();
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
//随机数字字母 
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
