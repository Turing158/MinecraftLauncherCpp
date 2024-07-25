#include <iostream>
#include "launchUtil.cpp"
#include <string>
#include <vector>
#include <cstdlib>
using namespace std;

string selectDir = "";//.minecraft路径 
vector<string> versions;//获取到的Minecraft版本 
string selectVersion = "";//当前选择的Minecraft版本 
int memoryMax = 0;//最大内存 
string username = "";//Minecraft用户名称 
string uuid = "";//用户UUID 
int width = 854;//游戏窗口宽度 
int height = 480;//游戏窗口高度 
void init(){
	selectDir = "E:/Game/test/.minecraft";
	versions = findVersion(selectDir);
	selectVersion = "1.12Opt";
	memoryMax = 2000;
	username = "Turing_ICE";
	uuid = generateUUID();
}


void launchMc(){
	string launchStr1 = 
	"java -Xmx"+to_string(memoryMax)+"m "+
	"-Dfile.encoding=GB18030 -Dstdout.encoding=GB18030 -Dstderr.encoding=GB18030 "+
	"-Djava.rmi.server.useCodebaseOnly=true -Dcom.sun.jndi.rmi.object.trustURLCodebase=false -Dcom.sun.jndi.cosnaming.object.trustURLCodebase=false ";
	string log4j2File = selectDir+"/versions/"+selectVersion+"/log4j2.xml";
	string launchStr2 = "-Dlog4j2.formatMsgNoLookups=true -Dlog4j.configurationFile="+log4j2File+" ";
	string launchStr3 = "-XX:+UnlockExperimentalVMOptions -XX:+UseG1GC -XX:G1NewSizePercent=20 -XX:G1ReservePercent=20 -XX:MaxGCPauseMillis=50 -XX:G1HeapRegionSize=32m -XX:-UseAdaptiveSizePolicy -XX:-OmitStackTraceInFastThrow -XX:-DontCompileHugeMethods ";
	string launchStr4 = "-Dfml.ignoreInvalidMinecraftCertificates=true -Dfml.ignorePatchDiscrepancies=true -XX:HeapDumpPath=MojangTricksIntelDriversForPerformance_javaw.exe_minecraft.exe.heapdump ";
	string libraryPath = selectDir+"/versions/"+selectVersion+"/"+"natives-windows-x86_64";
	string launchStr5 = "-Djava.library.path="+libraryPath+" ";
	string launchStr6 = "-Dminecraft.launcher.brand=CMDL -Dminecraft.launcher.version=1.0.0 ";
	string cpStr = "-cp ";
	string jsonContent = readFile(selectDir+"/versions/"+selectVersion+"/"+selectVersion+".json");
	vector<string> CpPaths = getLibPaths(jsonContent);
	if(isOptifine(jsonContent)){
		vector<string> optifineLibPaths = getOptifineLib(jsonContent);
		for(int i=0;i<optifineLibPaths.size();i++){
			cpStr+=selectDir+"/libraries/"+optifineLibPaths[i]+";";
		}
	}
	for(int i=0;i<CpPaths.size();i++){
		cpStr+=selectDir+"/libraries/"+CpPaths[i]+";";
	}
	cpStr+=selectDir+"/versions/"+selectVersion+"/"+selectVersion+".jar ";
	string mainClass = getMainClass(jsonContent)+" ";
	string assetIndex = getAssetIndex(jsonContent);
	string mcInfoStr = "--username "+username+" --version "+selectVersion+" --gameDir "+selectDir+" --assetsDir "+selectDir+"/assets --assetIndex "+assetIndex+" --uuid "+uuid+" --accessToken "+random_str(32)+" --userType msa --versionType \"CMDL 1.0.0\" ";
	string tweakClass = getTweakClass(jsonContent);
	if(tweakClass.size()){
		mcInfoStr+="--tweakClass "+tweakClass+" ";
	}
	mcInfoStr+="--width "+to_string(width)+" --height "+to_string(height);
	string launchStr = launchStr1+launchStr2+launchStr3+launchStr4+launchStr5+launchStr6+cpStr+mainClass+mcInfoStr;
	system(launchStr.c_str());
}

int main(){
	init();
	launchMc();
	return 0;
} 
