#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<vector>
#include<tiobj.hpp>
#include<tisys.hpp>

using namespace std;


bool isPreposition(string str, TiObj& sintax){
	for (int i=0; i<sintax.box.size(); i++){
		TiObj& obj  = sintax.box[i];
		if ( str == obj["prep"].str ){
			return true;
		}
	}
	return false;
}

void listPrepositions(string cur, TiObj& sintax){
	for (int i=0; i<sintax.box.size(); i++){
		TiObj&  obj  = sintax.box[i];
		string& prep = obj["prep"].str;
		if ( !obj.has("used") && prep.find(cur) == 0 ){
			cout << prep << " \n";
		}
	}
}



void explodeUrl(string url, string& out_path, string& out_name){
	out_path = "";
	out_name = "";
	int last = -1;
	for (int i=0; i<url.size(); i++){
		if ( url[i] == '/' )
			last = i;
	}
	for (int i=0; i<=last; i++)
		out_path += url[i];
	for (int i=last+1; i<url.size(); i++)
		out_name += url[i];
}

bool explodeTiUrl(string tiurl, string& out_classe, string& out_name){
	out_classe = "";
	out_name  = "";
	int last  = -1;
	for (int i=0; i<tiurl.size(); i++){
		if ( tiurl[i] == ':' )
			last = i;
	}
	if ( last == -1 ){
		out_classe = tiurl;
		return false;
	} else {
		for (int i=0; i<last; i++)
			out_classe += tiurl[i];
		for (int i=last+1; i<tiurl.size(); i++)
			out_name += tiurl[i];
		return true;
	}
}





void listFiles(string url){
	TiObj folder;
	string cur, path;
	explodeUrl(url, path, cur);
	if ( path == "" )
		TiSys::info(folder, ".");
	else
		TiSys::info(folder, path);
		
	vector<string> res;
	for (int i=0; i<folder.box.size(); i++){
		TiObj& obj   = folder.box[i];
		string& name = obj["name"].str;  
		if ( name.find(cur) == 0 ){
			if ( obj.is("Folder") )
				res.push_back(name+"/");
			else
				res.push_back(name+" ");
		}
	}

	if ( res.size() == 1 ){
		cout << path << res[0] << endl;
	} else {
		for (int i=0; i<res.size(); i++)
			cout << res[i] << endl;
	}
}

void listUsers(string cur){
	TiObj users;
	TiSys::listUsers(users);
	for (int i=0; i<users.box.size(); i++){
		TiObj& obj   = users.box[i];
		string& name = obj["name"].str;  
		if ( name.find(cur) == 0 ){
			cout << name << " \n";
		}
	}
}


void listObjects(string tiurl){
	string classe, cur; 
	if ( explodeTiUrl(tiurl, classe, cur) ){
		if ( classe == "@User" ){
			listUsers(cur);
		}
	} else {
		string aux;
		aux = "@Folder:";
		if ( aux.find(classe) == 0 ){
			cout << aux << "\n";
		}
		aux = "@File:";
		if ( aux.find(classe) == 0 ){
			cout << aux << "\n";
		}
		aux = "@Image:";
		if ( aux.find(classe) == 0 ){
			cout << aux << "\n";
		}
		aux = "@Video:";
		if ( aux.find(classe) == 0 ){
			cout << aux << "\n";
		}
		aux = "@User:";
		if ( aux.find(classe) == 0 ){
			cout << aux << "\n";
		}
		aux = "@Project:";
		if ( aux.find(classe) == 0 ){
			cout << aux << "\n";
		}
	}
}

#define PARAM_SIZE 2
class Parser{
public:
	vector<string> words;
	int cursor;
	
	Parser(int argc, char** argv){
		int pos = atoi(argv[1])+PARAM_SIZE;
		this->cursor = pos;
		bool isadd=false;
		for (int i=PARAM_SIZE; i<argc; i++){
			if ( strcmp(argv[i],":") == 0 ){
				int last = words.size() - 1;
				words[last] += ":";
				isadd = true;
				if ( i <= pos )
					this->cursor -= 1;
			} else if ( isadd ){
				int last = words.size() - 1;
				words[last] += argv[i];
				isadd = false;
				if ( i <= pos )
					this->cursor -= 1;
			} else 
				words.push_back(argv[i]);
		}
		this->cursor -= PARAM_SIZE;
	}
	
	string getCurrent(){
		return this->words[this->cursor];
	}
	
	string getPrev(){
		if ( this->cursor > 0 )
			return this->words[this->cursor-1];
		return "";
	}
	
	void write(){
		cout << "c:" << words[this->cursor] << endl;
		for (int i=0; i<words.size(); i++)
			cout << words[i] << endl;
	}
};



// ticomplete position:Int cursor comando argumentos
int main(int argc, char** argv){
	
	if ( argc < PARAM_SIZE )
		return 0;
	Parser parser(argc, argv);
	string cur  = parser.getCurrent();
	string prev = parser.getPrev(); 
	
	
	
	TiObj sintax("akk={type='User'}{prep='from';type='File';needed=y}{prep='to';type='File';needed=y}");
	
	// Set, if the preposition was already used
	for (int i=0; i<parser.words.size(); i++){
		for (int j=0; j<sintax.box.size(); j++){
			TiObj& obj = sintax.box[j];
			if ( parser.words[i] == obj["prep"].str ){
				obj["used"] = "y";
			}
		}
	}
	
	
	
	
	bool inside=false;
	if ( cur[0] == '@' ){
		inside = true;
	} else if ( isPreposition(prev, sintax) ){
		inside = true;
	}
		
	if ( inside ){
		
		if ( cur[0] == '@' ){
			//if ( cur[ cur.size()-1 ] == ':' ){
				listObjects(cur);
			/*} else {
				
			}*/
		}
		if ( cur == "" )
			cout << "@\n";
	} else {
	
		if ( argc > 3 ){
			listPrepositions(cur, sintax);
			if ( sintax.has("akk") ){
				listFiles(cur);
				if ( cur == "" )
					cout << "@\n";
			}
		}

	}
	

	



	/* DEBUG
	cout << argc << endl;
	cout << "c:" << cur << endl;
	for (int i=2; i<argc; i++){
		cout << argv[i] << endl;
	}*/
	return 0;
}
