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
	Filesystem fs;
	TiObj folder;
	string cur, path;
	explodeUrl(url, path, cur);
	if ( path == "" )
		fs.listdir(folder, ".");
	else
		fs.listdir(folder, path);
		
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
	/*TiObj users;
	TiSys::listUsers(users);
	for (int i=0; i<users.box.size(); i++){
		TiObj& obj   = users.box[i];
		string& name = obj["name"].str;  
		if ( name.find(cur) == 0 ){
			cout << name << " \n";
		}
	}*/
}


void listObjects(string tiurl){
	string classe, cur; 
	if ( explodeTiUrl(tiurl, classe, cur) ){
		/*if ( classe == "@User" ){
			listUsers(cur);
		}*/
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













bool isPrep(vector<string>& preps, string word){
	for (int i=0; i<preps.size(); i++){
		if ( preps[i] == word )
			return true;
	}
	return false;
}













class Parser{
  public:
	vector<string> words;
	int cursor;
	TiObj form;
	vector<string> preps;

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


	void analise(TiObj& syntax){
		// Build vector of prepositions
		this->preps.clear();
		for (int i=0; i<syntax.length(); i++){
			if ( syntax[i].name == "Akk" )
				continue;
			this->preps.push_back ( syntax[i].name );
		}

		// Analising
		for (int i=1; i<this->words.size(); i++){
			string& word = words[i];
			if ( word[0] == '-' ){
				TiObj* option = new TiObj();
				option->classe = "Option";
				option->set("value",word);
				this->form.box += option;
			} else if ( isPrep(this->preps, word) ){
				if ( i+1 < this->words.size() ){
					this->form.set(word, this->words[i+1]);
					i += 1;
				} else 
					this->form.set(word, "");
			} else {
				if ( !this->form.has("Akk") ){
					this->form.set("Akk", this->words[i]);
				}
			}
		}


	}



	static const int PARAM_SIZE = 2;
};





class OptionPkg {
	vector<string> labels;
	vector<string> reals;
	vector<bool>   deleted;
	int matches, last_macht;

  public:
	string pattern;

	OptionPkg(){
		matches = 0;
		last_macht = 0;
	}

	void add(std::string label){
		if ( label == "" )
			return;
		labels.push_back(label);
		reals.push_back("");
		deleted.push_back(false);
		matches += 1;
	}

	void add(std::string label, std::string real){
		labels.push_back(label);
		reals.push_back(real);
		deleted.push_back(false);
		matches += 1;
	}


	void eliminate(){
		if ( pattern == "" )
			return;
		for ( int i=0; i<this->labels.size(); i++ ){
			if ( this->deleted[i] == true )
				continue;
			string option = this->labels[i];
			size_t pos = option.find( pattern );
			if ( pos == string::npos ){
				this->deleted[i] = true;
				matches -= 1;
			} else {
				last_macht = i;
			}


			/*if ( this->isInside(pattern, option) ){
				last_macht = i;
			} else{
				this->deleted[i] = true;
				matches -= 1;
			}*/
		}
	}

	void write(){
		if ( matches == 1 ){
			if ( this->reals[last_macht] != "" )
				cout << this->reals[last_macht] << endl;
			else
				cout << this->labels[last_macht] << endl;
		} else {
			for (int i=0; i<this->labels.size(); i++){
				if ( this->deleted[i] == true )
					continue;
				cout << this->labels[i] << endl;
			}
		}
	}

	bool canExpand(){
		
	}

  private:
	bool isInside(string s_short, string s_long){
		if ( s_long.size() < s_short.size() )
			return false;
		for (int i=0; i<s_short.size(); i++){
			if ( s_short[i] != s_long[i] )
				return false;
		}
		return true;
	}
};








void runInt(OptionPkg& out){
	for (int i=0; i<10; i++){
		string aux;
		aux = (char)i+0x30;
		out.add ( aux );
	}
}


void runUrl(OptionPkg& out, string cur){
	Filesystem fs;

	string path;
	if ( cur[ cur.size()-1 ] != '/' ){
		path = path_remove(cur);
		out.pattern = path_last( cur );
	} else {
		path = cur;
		out.pattern = "";
	}



	string name;
	fs.listdir( path );
	for (int i=0; i<fs.box.size(); i++){
		name = fs.box[i].atStr("name");
		if ( fs.box[i].is("Folder") )
			out.add( name+"/", path_add(path,name)+"/" );
		else
			out.add( name, path_add(path,name)+" " );
	}
	out.add( "../", path_add(path,"../") );

}

void runEnum(OptionPkg& out, TiObj& prep){
	for (int i=0; i<prep.box.size(); i++){
		out.add( prep.box[i].atStr("name")+" " );
	}
}


void runBase(OptionPkg& out, Parser& parser, TiObj& syntax){
	for (int i=0; i<syntax.length(); i++){
		if ( syntax[i].name == "Akk" )
			continue;
		out.add ( syntax[i].name+" " );
	}

	if ( syntax.has("Akk") ){
		TiObj& akk = syntax.atObj("Akk");
		if ( akk.atStr("type") == "Url" )
			runUrl(out, parser.getCurrent());
	}
}


void runPrep(OptionPkg& out, Parser& parser, TiObj& prep){
	if ( prep.atStr("type") == "Url" ){
		runUrl(out, parser.getCurrent());
	} else if ( prep.atStr("type") == "Int" ){
		//runInt(out);
	} else if ( prep.atStr("type") == "Enum" ){
		//runEnum(out,prep);
	}
}





void fillList(OptionPkg& out, Parser& parser, TiObj& syntax){

	vector<string> preppkg;
	for (int i=0; i<syntax.length(); i++){
		if ( syntax[i].name == "Akk" )
			continue;
		preppkg.push_back ( syntax[i].name );
	} 


	string prev;
	prev = parser.getPrev();
	enum State {BASE,PREP,URL,STR,INT};
	State state = BASE;

	if ( isPrep(preppkg, prev) )
		state = PREP;


	if ( state == BASE ){
		runBase(out, parser, syntax);
	} else if ( state == PREP ){
		runPrep(out, parser, syntax.atObj(prev) );
	}
}









// ticomplete position:Int cursor comando argumentos
int main(int argc, char** argv){
	TiObj syntax;
	if ( argc < Parser::PARAM_SIZE )
		return 0;
	Parser parser(argc, argv);


	OptionPkg optionpkg;
	optionpkg.pattern = parser.getCurrent();
	syntax.loadText("Akk={type='Url';url='from'}from={type='Url'}to={type='Int'}as={type='Enum';{name='rabbit'}{name='cow'}}where={type='Url'}");

	fillList(optionpkg, parser, syntax);
	optionpkg.eliminate();
	optionpkg.write();



	/*parser.analise(syntax);
	if ( parser.form.has("from") ){
		TiObj groups;
		Filesystem fs;
		fs.listdir( parser.form.atStr("from") );
		fs.groupby(groups, "class");
		for (int i=0; i<groups.length(); i++){
			cout << groups[i].name << endl;
		}
	}*/



	




	return 0; 
}

	
	/*
	


	//TiObj sintax("akk={type='User'}{prep='from';type='File';needed=y}{prep='to';type='File';needed=y}");
	
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
				
			}*  /
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
