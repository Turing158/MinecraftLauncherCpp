#include <iostream>
#include "launchUtil.cpp"
#include <string>
#include <vector>

using namespace std;

void testLibPath1(){
	string json = readFile(R"(E:\Game\test\.minecraft\versions\1.12\1.12.json)");
	vector<string> paths = getLibPaths(json);
	for(int i=0;i<paths.size();i++){
		cout<<paths[i]<<endl;
	}
	cout<<paths.size()<<endl;
}
void testOptifineLib(){
	string json = readFile(R"(E:\Game\test\.minecraft\versions\1.12Opt\1.12Opt.json)");
	cout<<isOptifine(json);
	vector<string> optifineLib =  getOptifineLib(json);
	for(int i=0;i<optifineLib.size();i++){
		cout<<optifineLib[i]<<endl;
	}
}

int main(){
//	testLibPath1();
	testOptifineLib();
	return 0;
}
