/*
===========================================================================

DOOM3[CC] Source Code
Copyright (C) 2004-2005 the DOOM3[CC] team
Copyright (C) 2004-2005 Reid Kimball, Jason Sadler, Kevin Maulding and Matt Sefton
Copyright (C) 2020 Reid Kimball
Copyright (C) 2020-2021 Daniel Gibson

This file is part of the DOOM3[CC] GPL Source Code ("DOOM3[CC]
Source Code").

DOOM3[CC] Source Code is free software: you can redistribute it and/or
modify it under the terms of the GNU General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

DOOM3[CC] Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with DOOM3[CC] Source Code.  If not, see <http://www.gnu.org/licenses/>.

===========================================================================
*/

#ifndef _CCHELPERS_H_
#define _CCHELPERS_H_




//I added most of the stuff other than the languages in here to make the CCBst.h a little easier to read
/*
	idVec4(1.0f, 0.0f, 0.0f, 1.0f), // S_COLOR_RED
	idVec4(0.0f, 1.0f, 0.0f, 1.0f), // S_COLOR_GREEN
	idVec4(1.0f, 1.0f, 0.0f, 1.0f), // S_COLOR_YELLOW
	idVec4(0.0f, 0.0f, 1.0f, 1.0f), // S_COLOR_BLUE
	idVec4(0.0f, 1.0f, 1.0f, 1.0f), // S_COLOR_CYAN
	idVec4(1.0f, 0.0f, 1.0f, 1.0f), // S_COLOR_MAGENTA
	idVec4(1.0f, 1.0f, 1.0f, 1.0f), // S_COLOR_WHITE
	idVec4(0.5f, 0.5f, 0.5f, 1.0f), // S_COLOR_GRAY
	idVec4(0.0f, 0.0f, 0.0f, 1.0f), // S_COLOR_BLACK
	#define S_COLOR_DEFAULT				"^0"
	#define S_COLOR_RED					"^1"
	#define S_COLOR_GREEN				"^2"
	#define S_COLOR_YELLOW				"^3"
	#define S_COLOR_BLUE				"^4"
	#define S_COLOR_CYAN				"^5"
	#define S_COLOR_MAGENTA				"^6"
	#define S_COLOR_WHITE				"^7"
	#define S_COLOR_GRAY				"^8"
	#define S_COLOR_BLACK				"^9"
	RED
	GREEN
	YELLOW
	BLUE
	CYAN
	MAGENTA
	WHITE
	GRAY
	BLACK
	
	ROUGE
	VERT
	JAUNE
	BLEU
	CYAN
	MAGENTA
	BLANC
	GRIS
	NOIR 

	ROJO
	VERDE
	AMARILLO
	AZUL
	CI�N
	MORADO
	BLANCO
	GRIS
	NEGRO




*/

//ADDED 1-30-05
//basic color structure
struct ccColor {
	idStr Color;
	idStr  *Name;
	int size;

	void    Reset()
	{
		size = 1;
		Name = new idStr[size];
		Name[size - 1] = "";
	};

	//ex: will convert red, RED or Red to Red 
	void  WordToString(idStr colorword) {
		colorword.ToLower();
		if(colorword == "white" || colorword == "default" || colorword == "blanc" || colorword == "blanco")
		{
			Color = "White";
		}
		
		else if(colorword == "green" || colorword == "vert" || colorword == "verde" )
		{
			Color = "Green"; // S_COLOR_GREEN
		}
		else if(colorword == "red"  || colorword == "rouge" || colorword == "rojo")
		{
			Color = "Red"; // S_COLOR_RED
		}
		else if(colorword == "yellow" || colorword == "juane" || colorword == "amarillo")
		{
			Color = "Yellow"; // S_COLOR_YELLOW
		}
		else if(colorword == "blue" || colorword == "bleu" || colorword == "azul")
		{
			Color = "Blue"; // S_COLOR_BLUE
		}
		else if(colorword == "cyan" || colorword == "cian")
		{
			Color = "Cyan"; // S_COLOR_CYAN
		}
		else if(colorword == "magenta" || colorword == "morado")
		{
			Color = "Magenta"; // S_COLOR_MAGENTA
		}
		else if(colorword == "gray" || colorword == "gris")
		{
			Color = "Gray"; // S_COLOR_GRAY
		}

	}
	void	Resize(int newsize)
	{//only works if size is less than newsize
		//could be easily changed with a conditional
		idStr *temp = new idStr[newsize];
		for(int x = 0; x < newsize - 1; x++)
		{
			temp[x] = Name[x];

		}
		Name = temp;
		size = newsize;
		Name[size - 1] = "";
	};
	void Check()
	{
		if(Name[size - 1] == "")
		{
			delete Name[size - 1];
			size--;
		}
	};
	void Clear()
	{

		WordToString("DEFAULT");
		for(int x = 0; x < size; x++)
		{
			Name[x] = "";
		}
	};
};
//ADDED 1-30-05
//overall structure for handling the colors.
struct ccColorManager {
	ccColor *Colors;
	int size;
	bool Default;

	idStr Search(idStr EntityName)
	{
		for(int x = 0; x < size - 1; x++)
		{	
			for (int y = 0; y <Colors[x].size - 1; y++)
			{
				if(EntityName.Find(Colors[x].Name[y], false, 0, -1) >= 0)
				{
						return Colors[x].Color;
				}

			}
		}
		return "White";

	}

	void    Reset()
	{
		size = 1;
		Colors = new ccColor[size];
	}

	void	Resize(int newsize)
	{//only works if size is less than newsize
		//could be easily changed with a conditional
		ccColor *temp = new ccColor[newsize];
		for(int x = 0; x < newsize - 1; x++)
		{
			temp[x] = Colors[x];

		}
		Colors = temp;
		size = newsize;
	};

};




struct ccParamaters {
	idStr *caption;
	idStr currentcolor;
	int *time; //in milleseconds
	//true = scroll, false = no scroll
	int *scroll; //if the caption[current] needs to scroll
	//0 = no scroll
	//1 = 1/2 scroll
	//2 = 1 scroll
	//3 = 1 1/2 scroll
	//4 = 2 scroll
	int size;
	int current;
	idStr soundName;
	bool needsscroll;// same as ccTimeCode
	idStr priority;
	bool timecode;
	bool empty;// should hopefully solve some problems
	bool scrolled; //makes sure scrolling isn't constantly called
	bool radar; //will be used to check and see if the radar needs to be updated
	//true = already scrolled, false = needs scrolling
	//not in use
	idVec3 ent_vec;
	float max;

	void    Reset()
	{
		size = 1;
		caption = new idStr[size];
		time = new int[size];
		scroll = new int[size];
		currentcolor = "White";
		radar = false;

	}

	void	Resize(int newsize)
	{//only works if size is less than newsize
		//could be easily changed with a conditional
		// FIXME: DG: who deletes those arrays?
		idStr *temp = new idStr[newsize];
		int *tempint = new int[newsize];
		int *tempbool = new int[newsize];
		for(int x = 0; x < newsize - 1; x++)
		{
			tempbool[x] = scroll[x];
			temp[x] = caption[x];
			tempint[x] = time[x];
		}
		scroll = tempbool;
		time = tempint;
		caption = temp;
		
		size = newsize;
	};
	bool Check()
	{
		bool c = false;
		if(caption[size - 1] == "")
		{
			c = true;
			// delete caption[size - 1]; DG: trying to delete an element of an array doesn't really make sense (and crashes)
			size--;
		}
		current = 0;
		return c;
	};
	void Clear()
	{
		soundName = "";
		priority = "B";
		timecode = false;
		needsscroll = false;
		current = 0;
		empty = true;
		scrolled = false;
		for(int x = 0; x < size; x++)
		{
			caption[x] = "";
			time[x] = 0;
			scroll[x] = false;

		}
	};
	
};

#endif
