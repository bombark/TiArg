#include<iostream>
#include<string.h>
#include<string>
#include<tiobj.hpp>
#include<tisys.hpp>
#include<stdio.h>
#include<stdlib.h>
#include<vector>

using namespace std;

// ticomplete cursor comando argumentos

//void ajustArgs(vector<string>& a)

bool isSubstr(std::string word, std::string text){
	if ( word.size() == 0 )
		return true;

	if (word.size() <=	 text.size() ){
		for (int i=0; i<word.size(); i++){
			if ( word[i] != text[i] )
				return false;
		}
		return true;
	} else {
		return false;
	}
}

class Option{
  public:
	std::string name;
	std::string real;

	Option(std::string name, std::string real){
		this->name = name;
		this->real = real;
	}
};



class AutoComplete {
  public:
	TiObj param;
	TiObj syntax;
	int cur_pos;
	std::string cur;
	std::string last;


	AutoComplete(int argc, char** argv){
		this->getSyntax(argv[2]);

		this->cur_pos = atoi(argv[1]);
		if ( this->cur_pos+2 < argc )
			this->cur = argv[this->cur_pos+2];
		this->last = argv[this->cur_pos+2-1];


		string noun, prep;
		int state = 0;
		for (int i=3; i<argc; i++){
			string word = argv[i];
			if ( state == 0 ){
				if ( this->isPreposition(word) ){
					if ( noun != "" )
						param.set(prep,noun);
					state = 1;
					prep = word;
					noun = "";
				} else {
					param.set("_Akk", word);
				}
			} else {
				noun += word;
				state = 0;
				/*if ( word == "/" ){
					state = 1;
				} else if ( i+1 < argc ){
					if ( strcmp(argv[i+1],"/") == 0 )
						state = 1;
				}*/
			}
		}
		if ( noun != "" )
			param.set(prep,noun);


	}



	void getSyntax(std::string cmd){
		char buffer[1024];
		std::string text;
		FILE* fp = popen( Join("%s --syntax").at(cmd).ok().c_str(), "r" );
		while ( !feof(fp) ){
			fgets(buffer,1024,fp);
			text += buffer;
		}
		this->syntax.loadText(text);
		//this->syntax.loadFile("arg.ti");
	}


	bool isPreposition(std::string word){
		return this->syntax.has(word);
	}


	void showPreposition(){
		for (int i=0; i<this->syntax.length(); i++){
			if ( isSubstr(this->cur, syntax[i].name) )
				cout << syntax[i].name << " " << endl;
		}
	}

	void showNoun(TiObj& obj){
		if ( obj.is("Enum") ){
			showEnum(obj);
		} else {
			//this->showNoun(obj.atStr("type"));
		}
	}


	void showNoun(std::string type){
		if ( type=="Node" ){
			showFolder(this->cur);
		} else if ( type=="User" ){
			Filesystem fs;
			fs.listdir("/home");
			for (int i=0; i<fs.box.size(); i++){
				if ( isSubstr(this->cur, fs.box[i].atStr("name")) )
					cout << fs.box[i].atStr("name") << " " << endl;
			}
		} else {
			showFolder(this->cur, type);
		}
	}

  private:
	int getSimilarity(vector<Option>& options, int ini){
		int min = 0xFFFF;
		string base = options[0].name;
		for (int i=1; i<options.size(); i++){
			int size = base.size();
			if ( options[i].name.size() < size )
				size = options[i].name.size();

			int cont = 0;
			for (int j=ini; j<size; j++){
				if ( base[j] != options[i].name[j] )
					break;
				cont+=1;
			}
			if ( cont < min )
				min = cont;
		}
		return min;
	}


	void showEnum(TiObj& obj){
		vector<Option> options;
		for (int i=0; i<obj.size(); i++){
			if ( isSubstr(this->cur, obj.box[i].atStr("name")) ){
				options.push_back( Option(obj.box[i].atStr("name"),"") );
			}
		}
		if ( options.size() == 1 ){
			cout << options[0].name << " " << endl;
		} else {
			for (int i=0; i<options.size(); i++){
				cout << options[i].name << endl;
			}
		}
	}

	void showFolder(std::string url, std::string type=""){
		Filesystem fs;

		string path;
		string name;

		if ( url == "/" ){
			path = "/";
		} else {
			path = path_remove(url);
			name = path_last(url);
		}

		fs.listdir(path);

		vector<Option> options;
		if ( type == "" ){
			for (int i=0; i<fs.box.size(); i++){
				if ( isSubstr(name, fs.box[i].atStr("name")) ){
					if ( fs.box[i].is("Folder") )
						options.push_back( Option(fs.box[i].atStr("name"), fs.box[i].atStr("url")+"/") );
					else
						options.push_back( Option(fs.box[i].atStr("name"), fs.box[i].atStr("url")+" ") );
				}
			}
		} else {
			for (int i=0; i<fs.box.size(); i++){
				if ( isSubstr(name, fs.box[i].atStr("name")) ){
					if ( fs.box[i].is("Folder") )
						options.push_back( Option(fs.box[i].atStr("name"), fs.box[i].atStr("url")+"/") );
					else if ( fs.box[i].is(type) )
						options.push_back( Option(fs.box[i].atStr("name"), fs.box[i].atStr("url")+" ") );
				}
			}
		}
		if ( name == "." || name == ".." ){
			options.push_back( Option("..", path_add(path,"../")) );
		}


		if ( options.size() == 0 ){
			return;
		}



		if ( options.size() == 1 ){
			cout << options[0].real << endl;
		} else {

			int similarity = getSimilarity(options, name.size());

//cout << similarity << endl;

			if ( similarity == 0 ){
				cout << ".." << endl;	// to avoid replace the parameter by similarity at option
				for (int i=0; i<options.size(); i++){
					cout << options[i].name << endl;
				}
			} else {
				cout << path_add(path,options[0].name.substr(0,name.size()+similarity)) << endl;
			}
		}
	}
};





int main(int argc, char** argv){
	if ( argc < 2 )
		return 0;
	AutoComplete complete(argc,argv);

	if ( argc <= 4 ){
		complete.showPreposition();
	} else {
		if ( complete.isPreposition(complete.last) ){
			TiVar& var = complete.syntax.at(complete.last);
			if ( var.isObj() ){
				complete.showNoun( var.Obj() );
			} else {
				complete.showNoun( var.Str() );
			}
		} else {
			complete.showPreposition();
		}
	}


	return 0;
}
