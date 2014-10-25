#include "tiarg.hpp"
#include <string.h>


bool isPrep(string& out, string str){
	static string STD_PREP[]={"of","to","from","where"};
	for (int i=0; i<4; i++){
		if ( str == STD_PREP[i] ){
			out = STD_PREP[i];
			return true;
		}
	}
	return false;
}



void getArgs(TiObj& out, int argc, char** argv){
	string prep, aux;
	out.clear();
	string akk;
	int i,j, len, i_akk=0;
	for (i=1; i<argc; i++){
		if ( isPrep(prep, argv[i]) ){
			i_akk=0;
			aux = "";
			bool more = true;
			for (j=i+1; j<argc; j++){
				len = strlen(argv[j]);
				if ( len > 1 && argv[j][len-1] == ',' ){
					aux += argv[j];
					more = true;
				} else if ( strcmp(argv[j],",") == 0 ){
					aux += argv[j];
					more = true;
				} else if ( argv[j][0] == ',' ){
					aux += argv[j];
					more = false;
				} else if ( strcmp(argv[j],"or") == 0 || strcmp(argv[j],"and") == 0 ){
					aux += " ";
					aux += argv[j];
					aux += " ";
					more = true;
				} else {
					if ( more ){						
						aux += argv[j];
						more = false;
					} else						
						break;
				} 
			}
			i = j-1;
			out[prep] = aux;
		} else {

			if ( i_akk > 0 ){				
				akk += ",";
				akk += argv[i];
				i_akk += 1;
			} else {
				akk = argv[i];
				i_akk += 1;
			}
		}
	}
	if ( akk != "" )
		out["_Akk"] = akk;
}