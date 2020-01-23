#ifndef _CCBST_H_
#define _CCBST_H_

//#include "../src/idlib/precompiled.h"		//TEMP

//The cvars, for which parts to display

extern idCVar cc_dialogue;
extern idCVar cc_environment;
extern idCVar cc_languageSelector;

#include "CCHelpers.h"

//educated guess for the amount of text the height of 32 can hold
const int cc_SCROLLHALF = 99;
const int cc_SCROLLFULL = 148;
const int cc_SCROLLONEANDHALF = 197;
const int cc_SCROLLTWO = 246;
const int cc_DELAY = 2000;
const int cc_OVERIDE_DISTANCE = 10;
//idVec4 cc_DEFAULT_COLOR = idVec4(1.0f, 1.0f, 1.0f, 1.0f);

class ccBst {

	// DG: those structs are only used within the ccBst class anyway, so put them in here

	//added for the time code structure 11/27/04
	struct ccOutput{
		ccParamaters Data;
		int time;
		int length;
	};

	struct Node {
		Node *left;
		Node *right;
		ccParamaters data;

		~Node()
		{
			delete left;
			delete right;
		}
	};

public:	
	ccBst();
	~ccBst();
	void			InitOnce(); // DG: can't use cvar in constructor
	void			Init(idStr mapName, bool MFS); //MFS = Multi Folder Support added 11-27-04
	void			Display(idStr soundName, int length, idVec3 ent_vec, float max, idStr entName);
	void			Update();
	bool			IsEmpty();
	void			Reset();
	void			Remove(idStr soundName);
	bool			GetColorInit() {return CInit;};
	void			initLanguages();

	//distance has been coded but not implemented
	//not sure if I am using the right variables for distance
private:
	bool			DistanceBetween(idVec3 ent_vec, idVec3 player_vec, idVec3 old_vec, idStr NP, idStr OP, bool empty);
	bool			inDistance(idVec3 ent_vec, idVec3 player_vec, float maxDis, bool empty);
	void			Scroll(int scr, idStr prefix);
	void			Add(ccParamaters newData);
	void			AddHelper(Node *head, ccParamaters newData);
	ccParamaters	Find(idStr requisition);
	ccOutput		output[3];
	Node			*Tree; // TODO: maybe make this a sorted idList and binary search on it?
	int				toInt(char t);
	bool			CInit;
	bool			HandleRadar(idVec3 difVec, float vAngle, idStr Prior, idStr Color);
	float			AngleBetween(idVec3 difVec, float vAngle);
	void			InitColors(idFile *g);
	ccColorManager  ccColorInfo;


	//Jason added for language support: 12/18/04
	//The code checks each time Update() OR Display() is called whether language has changed.
	idStr           *languages;                 //List of languages. - TODO: make idList
	int             numLanguages;
	bool            languagesLoaded;
	bool			checkLanguage();			//sets the language if the CVar has changed - returns true if the language has changed
	idStr			getLanguageName(int index);	//returns the path to given language - language names are HARD-CODED in here
	int				language;					//stores what language is currently being used
	idStr			currentMap;					//So that we can call Init() again to load the new language
	//FOR REFERENCE:
	//language = 0 means English
	//language = 1 means Spanish
	//language = 2 means German

};
#endif
