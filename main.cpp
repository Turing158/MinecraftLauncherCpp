#include <iostream>
#include "launchUtil.cpp"
#include <string>
#include <vector>
#include <cstdlib>
using namespace std;

string os = "windows";//系统类型 
string framework = "x64";//系统架构 
string selectDir = "";//.minecraft路径 
vector<string> versions;//获取到的Minecraft版本 
string selectVersion = "";//当前选择的Minecraft版本 
int memoryMax = 0;//最大内存 
string username = "";//Minecraft用户名称 
string uuid = "";//用户UUID 
int width = 854;//游戏窗口宽度 
int height = 480;//游戏窗口高度 
string javaPath = "java";//java.exe路径
int isIsolate = 0;//是否版本分离 
void init(){
	selectDir = "E:/Game/test/.minecraft";
	versions = findVersion(selectDir);
	selectVersion = "1.20Fabric";
	memoryMax = 6000;
	username = "Turing_ICE";
	uuid = generateUUID();
//	javaPath = "E:/Programmer/jre-8/bin/java.exe";
//	isIsolate = 1;
}
//#define _INC_WINDOWS
//#define NOMINMAX
#define WIN32_LEAN_AND_MEAN//解决了启动编译时报了一堆错的问题，但是也禁用了一些东西 
#include <windows.h>

//通过创建新线程运行java虚拟机 
int threat(string str){
	STARTUPINFOA si;  
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));  
    si.cb = sizeof(si);  
    ZeroMemory(&pi, sizeof(pi));  
  	char* cmdLineChars = new char[str.size() + 1];  
    strcpy(cmdLineChars, str.c_str());
    if (!CreateProcessA(  
        "E:\\Programmer\\jdk-21\\bin\\java.exe", // java.exe的路径  
        cmdLineChars, // 命令行  
        NULL, // 进程句柄不可继承  
        NULL, // 线程句柄不可继承  
        FALSE, // 设置句柄继承为 FALSE  
        0, // 没有创建标志  
        NULL, // 使用父进程的环境块  
        NULL, // 使用父进程的起始目录   
        &si, // 指向 STARTUPINFO 结构  
        &pi) // 指向 PROCESS_INFORMATION 结构  
        )   {  
        cout << "CreateProcess failed (" << GetLastError() << ").\n";  
        return -1;  
    }  
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);  
    CloseHandle(pi.hThread);  
  
    return 0;  
}


void launchMc(){
	string launchStr1 = 
	javaPath+" -Xmx"+to_string(memoryMax)+"m "+
	"-Dfile.encoding=GB18030 -Dstdout.encoding=GB18030 -Dsun.stdout.encoding=GB18030 -Dstderr.encoding=GB18030 -Dsun.stderr.encoding=GB18030 "+
	"-Djava.rmi.server.useCodebaseOnly=true -Dcom.sun.jndi.rmi.object.trustURLCodebase=false -Dcom.sun.jndi.cosnaming.object.trustURLCodebase=false ";
	string log4j2File = selectDir+"/versions/"+selectVersion+"/log4j2.xml";
	string clientPath = ".minecraft/versions/"+selectVersion+"/"+selectVersion+".jar";
	string launchStr2;//参数在下面，因为路径有空格的话会报错，所有在下面集中处理了 
	string launchStr3 = "-XX:+UnlockExperimentalVMOptions -XX:+UseG1GC -XX:G1NewSizePercent=20 -XX:G1ReservePercent=20 -XX:MaxGCPauseMillis=50 -XX:G1HeapRegionSize=32m -XX:-UseAdaptiveSizePolicy -XX:-OmitStackTraceInFastThrow -XX:-DontCompileHugeMethods ";
	string launchStr4 = "-Dfml.ignoreInvalidMinecraftCertificates=true -Dfml.ignorePatchDiscrepancies=true -XX:HeapDumpPath=MojangTricksIntelDriversForPerformance_javaw.exe_minecraft.exe.heapdump ";
	string libraryPath = selectDir+"/versions/"+selectVersion+"/"+findNativeFile(selectDir,selectVersion);
	string launchStr5;//参数在下面，因为路径有空格的话会报错，所有在下面集中处理了 
	string launchStr6 = "-Dminecraft.launcher.brand=CMDL -Dminecraft.launcher.version=1.0.0 ";
	string cpStr = "";
	string jsonContent = readFile(selectDir+"/versions/"+selectVersion+"/"+selectVersion+".json");
	vector<string> CpPaths = getLibPaths(jsonContent);
	if(isOptifine(jsonContent)){
		vector<string> optifineLibPaths = getOptifineLib(jsonContent);
		for(int i=0;i<optifineLibPaths.size();i++){
			string path = selectDir+"/libraries/"+optifineLibPaths[i];
			if(existFile(path)){
				cpStr+=path+";";
			}
		}
	}
	if(isForge(jsonContent)){
		vector<string> forgeLibPaths = getForgeLib(jsonContent,CpPaths);
		for(int i=0;i<forgeLibPaths.size();i++){
			string path = selectDir+"/libraries/"+forgeLibPaths[i];
			if(existFile(path)){
				cpStr+=path+";";
			}
		}
	}
	if(isFabric(jsonContent)){
		vector<string> fabricLibPaths = getFabricLib(jsonContent,CpPaths);
		for(int i=0;i<fabricLibPaths.size();i++){
			string path = selectDir+"/libraries/"+fabricLibPaths[i];
			if(existFile(path)){
				cpStr+=path+";";
			}
		}
	}
	for(int i=0;i<CpPaths.size();i++){
		string path = selectDir+"/libraries/"+CpPaths[i];
		if(existFile(path)){
			cpStr+=path+";";
		}
	}
	cpStr+=selectDir+"/versions/"+selectVersion+"/"+selectVersion+".jar";
	string mainClass = getMainClass(jsonContent)+" ";
	string assetIndex = getAssetIndex(jsonContent);
	string gameDir = "";
	if(isIsolate){
		gameDir += selectDir+"/versions/"+selectVersion;
	}
	else{
		gameDir += selectDir;
	}
	if(selectVersion.find(" ") == string::npos){
		launchStr2 = "-Dlog4j2.formatMsgNoLookups=true -Dlog4j.configurationFile="+log4j2File+" -Dminecraft.client.jar="+clientPath+" ";
		launchStr5 = "-Djava.library.path="+libraryPath+" ";
		cpStr = "-cp "+cpStr+" ";
	}
	else{
		launchStr2 = "-Dlog4j2.formatMsgNoLookups=true \"-Dlog4j.configurationFile="+log4j2File+"\" \"-Dminecraft.client.jar="+clientPath+"\" ";
		launchStr5 = "\"-Djava.library.path="+libraryPath+"\" ";
		cpStr = "-cp \""+cpStr+"\" ";
		gameDir = "\""+gameDir+"\"";
	}
	string version = selectVersion.find(" ") != string::npos ? "\""+selectVersion+"\"" : selectVersion;
	string mcInfoStr = "--username "+username+" --version "+version+" --gameDir "+gameDir+" --assetsDir "+selectDir+"/assets --assetIndex "+assetIndex+" --uuid "+uuid+" --accessToken "+random_str(32)+" --userType msa --versionType \"CMDL 1.0.0\" ";
	string tweakClass = getTweakClass(jsonContent);
	if(tweakClass.size()){
		mcInfoStr+="--tweakClass "+tweakClass+" ";
	}
	mcInfoStr+="--width "+to_string(width)+" --height "+to_string(height)+" ";
//	1.20Forge参数 
	string prePara = extraPrePara(jsonContent,libraryPath);
	string morePara = extraMorePara(jsonContent,selectDir,selectVersion);
	string fmlPara = extraParaNameFml(jsonContent);
	string launchStr = launchStr1+launchStr2+launchStr3+launchStr4+launchStr5+prePara+launchStr6+cpStr+morePara+mainClass+mcInfoStr+fmlPara;
	cout<<launchStr;
	threat(launchStr);
}

int main(){
	init();
	launchMc();
	return 0;
} 
