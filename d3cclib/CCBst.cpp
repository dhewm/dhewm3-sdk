
//#define GAME_DLL
//#include "../src/idlib/precompiled.h"
//#pragma hdrstop

// DG: dhewm3 (and its SDK) don't have precompiled.h, so I added all needed includes here
#include "../sys/platform.h"
#include "../idlib/Lib.h"
#include "../idlib/Str.h"
#include "../framework/CVarSystem.h"
#include "../framework/FileSystem.h"

#ifdef _D3XP
#include "../d3xp/Game_local.h"
#include "../d3xp/Player.h"
#else
#include "../game/Game_local.h"
#include "../game/Player.h"
#endif

idCVar cc_dialogue( "cc_dialogue", "1", CVAR_BOOL|CVAR_SYSTEM|CVAR_NOCHEAT|CVAR_ARCHIVE, "Captions all dialogue" ); 
idCVar cc_environment( "cc_environment", "1", CVAR_BOOL|CVAR_SYSTEM|CVAR_NOCHEAT|CVAR_ARCHIVE, "Captions all sound effects" );
//idCVar cc_languageSelector("cc_language", "0", CVAR_INTEGER|CVAR_SYSTEM, "Chooses language" );
idCVar cc_languageSelector( "cc_language", "0", CVAR_INTEGER|CVAR_SYSTEM|CVAR_NOCHEAT|CVAR_ARCHIVE, "Chooses the language!" );


/*cc_Language* languages = new cc_Language[3];
languages[0].index = 0;
languages[0].language = "English";*/

//cc_LanguageStrings = {new idStr("English"), new idStr("Espanol"), new idStr("Deutsch")};

#include "CCBst.h"

// DG: this helper function is only used in this file anyway, so no need to add it to idStr
static int FindCharReverse(const idStr& str, const char c, int start)
{
	int x; 
	int len = strlen(str) - 1; // TODO: why not str.Length() ?
	if(len >= start) 
	{
		for( x = start; x >= 0; x--)
		{
			if( str[x] == c) 
				return x; 
		}
		return -1; 
	}
	else
		return -1; 
}

ccBst::ccBst()
{
	Tree = NULL;
	ccColorInfo.Reset();
	CInit = false;
	output[0].Data.Reset();
	output[1].Data.Reset();
	output[2].Data.Reset();

	output[0].Data.Clear();
	output[1].Data.Clear();
	output[2].Data.Clear();

	//Jason added 12/18/04
	// DG: can't use the cvar here, it might not be initialized yet
	//     (it's a global and so is gameLocal which contains ccBst ccDisplayInfo
	//      and if gameLocal is initialized first, like it's right now on my system,
	//      it calls this constructor even though the CVars above haven't been initialized yet)
	//     so this is now done in InitOnce(), called from GameLocal::Init()
	//language = cc_languageSelector.GetInteger();
	//gameLocal.Printf("CC INSTANTIATED!");

	// DG: the following were uninitialized; now my IDE is happy ;)
	language = 0;
	languages = NULL;
	numLanguages = 0;
	languagesLoaded = 0;
}
ccBst::~ccBst()
{
	delete Tree;
}

void ccBst::InitOnce()
{
	language = cc_languageSelector.GetInteger();
	initLanguages();
}

// FIXME: DG: apparently initLanguages() is never called? what's this good for?
void ccBst::initLanguages()
{
	char* buf = NULL;  // For initing languages, scope is only inside the constructor. Added Jason 04/02/05
	idStr currentLanguage = "";
	bool comment = false; // DG: make sure to init this..
	numLanguages = 0;
	languagesLoaded = false;

	idFile *pFile = idLib::fileSystem->OpenFileRead("caption/languages.dcc");

	if (pFile)
	{
		int length = pFile->Length();
		buf = new char[length+1]; // DG: +1 for terminating '\0' set in the next line
		buf[length] = '\0'; // allocate enough memory to read the whole file
		pFile->Read( buf, length ); // read the file
		bool read = false;

		for(int x = 0; x < length; x++)
		{
			if(buf[x] == '/' && buf[x + 1] == '/')
			{
				x++;
				if(comment)
				{
					comment = false;
				}
				else
					comment = true;
			}
			else if(comment)
			{
				if(buf[x] == '\n')
				{
					comment = false;
				}
			}
			else if(read && buf[x] != '"')
			{
				currentLanguage += buf[x];
			}
			else if(buf[x] == '"')
			{
				if (!read)
				{
					read = true;
				}
				else
				{
					read = false;
					numLanguages++;
					idStr *temp = new idStr[numLanguages];
					for (int i = 0; i < numLanguages - 1; i++)
					{
						temp[i] = languages[i];
					}
					temp[numLanguages - 1] = currentLanguage;
					languages = temp;
					//gameLocal.Printf(languages[numLanguages - 1] + "\n");
					currentLanguage = "";
				}
			}
		}
		languagesLoaded = true;

		delete[] buf;
	}
	else
	{
		gameLocal.Printf("\nError: languages.dcc missing!\n");
	}

	/*for(int x = 0; x < numLanguages; x++)
	{
		gameLocal.Printf(idStr(x) + " " + languages[x] + "\n");
	}*/
}

//added November 20, 2004
//resets the Root of the BST
void  ccBst::Reset()
{
	Tree = NULL; // TODO: DG: remove elements?
	output[0].Data.Reset();
	output[1].Data.Reset();
	output[2].Data.Reset();
	output[0].Data.Clear();
	output[1].Data.Clear();
	output[2].Data.Clear();
}

void	ccBst::Init(idStr mapName, bool MFS)
{
	//Jason added this  12/18/04 for language support
	if (!MFS)	//This works since only the outer file (which is the actual map) is called without MFS
		currentMap = mapName;

	if (!checkLanguage())
	{
		idStr languagePath = getLanguageName(language);

		bool flag = false;
		int period;
		int slash;
		int length;
		char *buf;
		if(!MFS)//added in for multifolder support
		{
			period = mapName.Find('.', 0, -1);
			//Find( const char c, int start = 0, int end = -1 )
			slash = FindCharReverse(mapName, '/', period);
			gameLocal.Printf("\n" + idStr(slash) + "\n");
			mapName = "caption/" + languagePath + "/" + mapName.Mid(slash + 1, period - slash) + "dcc";	//added "+languagePath" 12/18/04
			gameLocal.Printf("\nccBst::Init() inside MFS conditional...path for DCCs: " + mapName);
			//THIS MEANS THE DCC FILES MUST BE IN THE caption/english folder (or spanish or...)
		}
		else	//Added by Jason for languages - 12/18/04
		{
			/*period = mapName.Find('.', 0, -1);
			slash = FindCharReverse(mapName, '/', period);
			mapName = "caption/" + languagePath + "/" + mapName.Mid(slash +1, period - slash) + "dcc";	//added "+languagePath" 12/18/04*/
			gameLocal.Printf("\nccBst::Init() MFS is true, path for DCCs: " + mapName);
		}

		idFile *f = idLib::fileSystem->OpenFileRead(mapName);

		if ( f ) 
		{
			length = f->Length();
			buf = (char *) Mem_Alloc( length + 1 );
			buf[length] = '\0';//allocate enough memory to read the whole file
			f->Read( buf, length );//read the file
			bool quote = false;
			bool inside = false;
			// DG: made sure the next 3 variables are initialized (compiler was complaining)
			bool first = false; //used to keep track of first and second digits in timecode
			int code = 0; //used to keep track between minutes, seconds and milliseconds
			int tm = 0; //used to add up the time
			int lenscroll = 0;
			int y = 0;
			idStr temp = "";
			bool comment = false;
			if(mapName.FindText(mapName, "colors", false) >= 0)
			{
				//ADDED 1-30-05
				//a specail reader for the color file could not get it to load while in another function
				ccColor tempcolor;
				tempcolor.Reset();
				for(int x = 0; x < length;x++)
				{
					if(buf[x] == '/' && buf[x + 1] == '/')
					{
						x++;
						if(comment)
						{
							comment = false;
						}
						else
							comment = true;
					}
					else if(comment)
					{
						if(buf[x] == '\n')
						{
							comment = false;
						}
					}
					else if(buf[x] <= ' ' || buf[x] == '\n')
					{
						if(inside == false && temp != "")
						{
							if(temp == "None" || temp == "none" || temp == "NONE")
							{
								ccColorInfo.Default = true;
								return;
							}
							tempcolor.WordToString(temp);
							//gameLocal.Printf("Color: " + temp + "\n");
							temp = "";
						}
						else if(inside == true && temp!= "")
						{
							tempcolor.Name[tempcolor.size - 1] = temp;
							tempcolor.Resize(tempcolor.size+1);
							//gameLocal.Printf("Name: " + temp + "\n");
							temp = "";
						}

					}
					else if(buf[x] == '{')
					{
						inside = true;
					}
					else if(buf[x] == '}')
					{
						inside = false;
						//tempcolor.Check();
						ccColorInfo.Colors[ccColorInfo.size - 1] = tempcolor;
						ccColorInfo.Resize(ccColorInfo.size + 1);

						//gameLocal.Printf("Added \n\n");
						tempcolor.Reset();
					}
					else
					{
						temp+= buf[x];
					}
				}
				CInit = true;
			}

			else
			{//regular dcc file loader

				ccParamaters ntemp;

				ntemp.empty = false;
				ntemp.Reset();
				ntemp.Clear();
				for(int x = 0; x < length;x++)
				{
					if(buf[x] == '/' && buf[x + 1] == '/')
					{
						x++;
						if(comment)
						{
							comment = false;
						}
						else
							comment = true;
					}
					else if(comment)
					{
						if(buf[x] == '\n')
						{
							comment = false;
						}
					}
					else if(buf[x] == '\n')
					{
						if(ntemp.timecode && quote )
						{ //if it is in the time code, resize the list for one more and go to the next one

							if(lenscroll >= cc_SCROLLHALF && lenscroll < cc_SCROLLFULL)
							{
								ntemp.scroll[y] = 1;
								//added to make sure the display() knows it scrolls somewhere
								ntemp.needsscroll = true;
							}
							else if(lenscroll >= cc_SCROLLFULL && lenscroll < cc_SCROLLONEANDHALF)
							{
								ntemp.scroll[y] = 2;
								ntemp.needsscroll = true;
							}
							else if(lenscroll >= cc_SCROLLONEANDHALF && lenscroll < cc_SCROLLTWO)
							{
								ntemp.scroll[y] = 3;
								ntemp.needsscroll = true;
							}
							else if(lenscroll >= cc_SCROLLTWO)
							{
								ntemp.scroll[y] = 4;
								ntemp.needsscroll = true;
							}
							else
								ntemp.scroll[y] = 0;
							ntemp.caption[y] = temp;
							lenscroll = 0;
							temp = "";
							y++;
							ntemp.Resize(y + 1);
							//gameLocal.Printf("TCC: " + ntemp.caption[y-1] + "\n");

						}
					}
					else if(buf[x] <= ' ' && !quote)
					{

						if(ntemp.soundName == "" && !inside)
						{
							if(temp != "")
							{
								ntemp.soundName = temp;
								//gameLocal.Printf("S: " + ntemp.soundName + "\n");
								temp = "";
								ntemp.timecode = false;
							}
						}
						if(ntemp.soundName != "" && !inside && temp == "timecode")
						{
							ntemp.timecode = true;
							//gameLocal.Printf("Timecode == true \n");
							temp = "";
						}
					}
					else
					{
						if(buf[x] == '{')
						{
							inside = true;
						}
						else if(buf[x] == '}')
						{
							inside = false;
							//gameLocal.Printf(ntemp.priority + " \n");
							if(ntemp.priority != "load")//reset the required for the time code structure and add
							{
								quote = false;
								bool status = true;
								ntemp.empty = false;
								while(status && ntemp.timecode)
								{
									status = ntemp.Check();
								}
								//make sure it is not true

								Add(ntemp);
								y = 0;
								/*gameLocal.Printf(ntemp.soundName + "\n");
								gameLocal.Printf(ntemp.priority + "\n");
								gameLocal.Printf("TC : " + idStr(ntemp.timecode) + "\n");
								for(int x = 0; x < ntemp.size; x++)
								{
									gameLocal.Printf(idStr(ntemp.time[x]) + "\n");
									gameLocal.Printf(ntemp.caption[x] + "\n");		
								}
								gameLocal.Printf("\n");*/
								ntemp.Reset();
								ntemp.Clear();
							}
							else
							{
								ntemp.Reset();
								ntemp.Clear();
							}

						}
						else if(buf[x] == '"')
						{

							if(!quote)
							{
								quote = true;
							}
							else
							{
								quote = false;
								if(lenscroll > cc_SCROLLHALF && lenscroll < cc_SCROLLFULL)
								{
									ntemp.scroll[0] = 1;
									//added to make sure the display() knows it scrolls somewhere
									ntemp.needsscroll = true;
								}
								else if(lenscroll >= cc_SCROLLFULL && lenscroll < cc_SCROLLONEANDHALF)
								{
									ntemp.scroll[0] = 2;
									ntemp.needsscroll = true;
								}
								else if(lenscroll >= cc_SCROLLONEANDHALF && lenscroll < cc_SCROLLTWO)
								{
									ntemp.scroll[0] = 3;
									ntemp.needsscroll = true;
								}
								else if(lenscroll >= cc_SCROLLTWO)
								{
									ntemp.scroll[0] = 4;
									ntemp.needsscroll = true;
								}
								else
								{
									ntemp.scroll[0] = 0;
									ntemp.needsscroll = false;
								}

								lenscroll = 0;
								ntemp.caption[0] = temp;
								//gameLocal.Printf("C: " + ntemp.caption + "\n\n");
								temp = "";
							}
						}
						else if(!quote)
						{
							temp += buf[x];
							if((buf[x+1] == '\n' || buf[x+1] <= ' ') && inside)
							{
								ntemp.priority = temp;
								if(temp == "load")
								{
									temp = "";
									Init("caption/" + getLanguageName(language) + "/" +ntemp.soundName + ".dcc", true);
									//gameLocal.Printf("Loading File: /" + ntemp.soundName + ".dcc \n");
								}

								if(ntemp.timecode)
								{
									quote = true;
									x++;
									x++;//needed to make sure there is not an empty caption
								}
								//gameLocal.Printf("P: " + ntemp.priority + "\n");
								temp = "";
							}
						}

						else if(quote && buf[x] != '\n')
						{
							if(ntemp.timecode)
							{
								if(buf[x] == '[')
								{
									first = true;
									code = 0;
								}
								else if(code < 3)
								{
									if(first)
									{
										tm = toInt(buf[x]) * 10;
										first = false;

									}
									else
									{
										tm += toInt(buf[x]);
										first = true;

										if(code == 0)
										{
											ntemp.time[y] = tm * 60000;
											x++;
										}
										else if(code == 1)
										{
											ntemp.time[y] += tm * 1000;
											x++;
										}
										else if(code == 2)
										{
											ntemp.time[y] += tm;
											//gameLocal.Printf("TCT: " + idStr(ntemp.time[y]) + "\n");
											x++;
											x++;
										}
										code++;
									}
								}
								if(code == 3)
								{
									//am using this method to track the amount of characters in the string
									//as other methods have failed to work one way or another
									if(buf[x] == '*')
									{
										x++;
										lenscroll++;
										temp+= buf[x];
									}
									else
									{
										lenscroll++;
										temp += buf[x];
									}

								}
							}
							else
							{
								if(buf[x] == '*')
								{
									x++;
									lenscroll++;
									temp+= buf[x];
								}
								else
								{
									lenscroll++;
									temp += buf[x];
								}
							}

						}
					}
				}
			}
		}
		else
		{
			gameLocal.Printf("\nError: Could not Open file " + mapName + "\n");
		}
	}
}

bool	ccBst::IsEmpty()
{
	if(Tree == NULL)
		return true;
	else
		return false;
}
	
void	ccBst::Display(idStr soundName, int length, idVec3 ent_vec, float max, idStr entName)
{
	//Check for language change
	checkLanguage();

	idPlayer *player = gameLocal.GetLocalPlayer();
	
	if(player)
	{
		int period;
		period = soundName.Find('.', 0, -1);
		idStr soundName_Path = soundName.Mid(0, period);
		int slash = FindCharReverse(soundName, '/', period);
		soundName = soundName.Mid(slash+1, period - (slash + 1));
		//gameLocal.Printf(soundName_Path + "  " + soundName + "\n");
		
		if((output[0].Data.soundName == soundName || output[0].Data.soundName == soundName_Path) && !output[0].Data.empty)
		{//added another 5 seconds, should stop some hecticness of the gui
			//a true timecode should never enter this conditional

			//ADDED MINIMUM TIME OF 4 SECONDS TO THIS CONDITIONAL - 11/29/04 by JASON
			if (gameLocal.GetTime() - output[0].time > 4000)
			{
				output[0].Data.ent_vec = ent_vec;
				output[0].time = gameLocal.GetTime();

				//TO PUT "CONTINUES" IN CAPS AND INSIDE SQUARE BRACKETS USE FOLLOWING CODE:
				/*
				if(output[0].Data.caption[0].Find("continues", false, 0, -1) < 0)
				{
					idStr tempCaption = output[0].Data.caption[0];
					if (tempCaption.Last(')') > 0)
					{
						output[0].Data.caption[0].Insert(" CONTINUES", strlen(tempCaption) - 1);
					}
					else
						output[0].Data.caption[0] += " continues";				

					player->hud->SetStateString("Hdialog", output[0].Data.caption[0]);
				}*/
				
				//INSTEAD OF THE FOLLOWING IF:

				if(output[0].Data.caption[0].Find("continues", true, 0, -1) < 0)
				{
					output[0].Data.caption[0] += " continues";				
					player->hud->SetStateString("Hdialog", output[0].Data.caption[0]);
				}
			}
		}
		
		else if((output[1].Data.soundName == soundName || output[1].Data.soundName == soundName_Path) && !output[1].Data.empty )
		{
			//ADDED MINIMUM TIME OF 4 SECONDS TO THIS CONDITIONAL - 11/29/04 by JASON
			if (gameLocal.GetTime() - output[1].time > 4000)\
			{
				output[1].time = gameLocal.GetTime();
				output[1].Data.ent_vec = ent_vec;

				//TO PUT "CONTINUES" IN CAPS AND INSIDE SQUARE BRACKETS USE FOLLOWING CODE:
				/*
				if(output[1].Data.caption[0].Find("continues", false, 0, -1) < 0)
				{
					idStr tempCaption = output[1].Data.caption[0];
					if (tempCaption.Last(')') > 0)
					{
						output[1].Data.caption[0].Insert(" CONTINUES", strlen(tempCaption) - 1);
					}
					else
						output[1].Data.caption[0] += " continues";				

					player->hud->SetStateString("Hdialog", output[0].Data.caption[0]);
				}*/
				
				//INSTEAD OF THE FOLLOWING IF:

				if(output[1].Data.caption[0].Find("continues", true, 0, -1) < 0)
				{
					output[1].Data.caption[0] += " continues";
					player->hud->SetStateString("Mdialog", output[1].Data.caption[0]);
				}
			}
		}
		
		else if((output[2].Data.soundName == soundName || output[2].Data.soundName == soundName_Path) && !output[2].Data.empty)
		{

			//ADDED MINIMUM TIME OF 4 SECONDS TO THIS CONDITIONAL - 11/29/04 by JASON
			if (gameLocal.GetTime() - output[2].time > 4000)
			{
				output[2].time = gameLocal.GetTime();
				output[2].Data.ent_vec = ent_vec;

				//TO PUT "CONTINUES" IN CAPS AND INSIDE SQUARE BRACKETS USE FOLLOWING CODE:
				/*
				if(output[2].Data.caption[0].Find("continues", false, 0, -1) < 0)
				{
					idStr tempCaption = output[2].Data.caption[0];
					if (tempCaption.Last(')') > 0)
					{
						output[2].Data.caption[0].Insert(" CONTINUES", strlen(tempCaption) - 1);
					}
					else
						output[2].Data.caption[0] += " continues";				

					player->hud->SetStateString("Hdialog", output[0].Data.caption[0]);
				}*/
				
				//INSTEAD OF THE FOLLOWING IF:

				if(output[2].Data.caption[0].Find("continues", true, 0, -1) < 0)
				{
					output[2].Data.caption[0] += " continues";
					player->hud->SetStateString("Ldialog", output[2].Data.caption[0]);
				}
			}
		}
		else
		{
			ccParamaters display;
			display.Reset();
			display = Find(soundName);
			display.scrolled = false;
			display.current = 0;
			//not in use yet
			display.ent_vec = ent_vec;
			display.max = max;
		//	gameLocal.Printf("SN: " + soundName + "  " + "\n");
			if(display.caption[0] == "ERROR")
			{
				display.Reset();
				display.Clear();
				display = Find(soundName_Path);
				//gameLocal.Printf(display.soundName + "\n");
			}


			if(display.caption[0] == "ERROR")
			{
				// player = player;
				gameLocal.Printf("Error: Couldn't find sound, " + soundName + "\n");
			}

			else if ((display.priority[0] == '0' && cc_dialogue.GetBool()) || (display.priority[0] != '0' && cc_environment.GetBool())) 
			{
				if(inDistance(ent_vec, player->GetWorldCoordinates(player->GetPhysics()->GetOrigin()), max, false) || gameLocal.inCinematic || display.priority[0] == '0')
				{
					idVec3 temp  = player->GetWorldCoordinates(player->GetPhysics()->GetOrigin());
				//	if(display.priority[0] != 'A' && ent_vec != temp)
				//	{
					//gameLocal.Printf("Entity: " + entName + "\n");
				//	HandleRadar(ent_vec - temp, player->viewAngles.yaw);
					
				//	}

					display.ent_vec = ent_vec;
					display.max = max;
					//gameLocal.Printf("Caption: " + display.soundName + "\n");
					gameLocal.Printf("Caption: " + display.caption[0] + "\n");
					//gameLocal.Printf("Entity: " + entName + "\n");
					bool output2WasEmpty = output[2].Data.empty;	//ADDED 11/23/04 BY JASON
					//has a priority higher than the high box
					if((display.priority.Cmp(display.priority, output[0].Data.priority) <= 0 && !output[0].Data.needsscroll)
						|| (DistanceBetween(display.ent_vec, player->GetWorldCoordinates(player->GetPhysics()->GetOrigin()), output[0].Data.ent_vec, display.priority, output[0].Data.priority, output[0].Data.empty)
						&& (output[0].Data.priority[0] != '0' || (output[0].Data.timecode && output[0].Data.scroll[output[0].Data.current] == 0))))
						
					{
						player->hud->HandleNamedEvent("resetHigh");
						int pos = output[2].Data.caption[0].Find("continues", true, 0, -1);
						if(pos > 0)//make sure the continues is removed
						{
							output[2].Data.caption[0] = output[2].Data.caption[0].Mid(0, pos - 1);

						}
						output[2] = output[1];
						output[1] = output[0];		
						
						
						if(output[2].Data.empty)
						{
							if (!output2WasEmpty)	//ADDED 11/23/04 BY JASON
							{
								player->hud->SetStateString("Ldialog", "");
								player->hud->HandleNamedEvent("removedlow");
								player->hud->HandleNamedEvent("LowPHide");
								output[2].Data.radar = false;
							}
						}
						else
						{	//This means that even if there was a caption there before, it will still play the animation.  This is a design decision. - Jason
							//ADDED 1-30-05
							//Set the low color to the old color
							player->hud->HandleNamedEvent(output[2].Data.currentcolor + "Low");
							player->hud->SetStateString("Ldialog", output[2].Data.caption[output[2].Data.current]);
							player->hud->HandleNamedEvent("captionlow");
							HandleRadar(output[2].Data.ent_vec - temp, player->viewAngles.yaw, "Low", output[2].Data.currentcolor);
							player->hud->HandleNamedEvent("LowPVis");
							output[2].Data.radar = true;
						}
						
						if(output[1].Data.empty)
						{
							if(!output[2].Data.empty)	//ADDED 11/23/04 BY JASON
							{
								player->hud->SetStateString("Mdialog", "");
								player->hud->HandleNamedEvent("removedmid");
								player->hud->HandleNamedEvent("MidPHide");
								output[1].Data.radar = false;
							}
						}
						else	//This means that even if there was a caption there before, it will still play the animation.  This is a design decision. - Jason
						{
							//ADDED 1-30-05
							//Set the mid color to the old color
							player->hud->HandleNamedEvent(output[1].Data.currentcolor + "Mid");
							player->hud->SetStateString("Mdialog", output[1].Data.caption[output[1].Data.current]);
							player->hud->HandleNamedEvent("captionmid");
							HandleRadar(output[1].Data.ent_vec - temp, player->viewAngles.yaw, "Mid", output[1].Data.currentcolor);
							player->hud->HandleNamedEvent("MidPVis");
							output[1].Data.radar = true;
						}

						//ADDED 1-30-05
						//search and find the color, call the on Name event to handle the color change
						display.currentcolor = ccColorInfo.Search(entName);
						player->hud->HandleNamedEvent(display.currentcolor + "High");
						//
						player->hud->SetStateString("Hdialog", display.caption[0]);
						HandleRadar(display.ent_vec - temp, player->viewAngles.yaw, "High", display.currentcolor);
						player->hud->HandleNamedEvent("HighPVis");
						output[0].Data = display;
						output[0].Data.radar = true;
						output[0].time = gameLocal.GetTime();
						//gameLocal.Printf(output[0].Data.soundName + " " + idStr(output[0].time) + "\n");
						if(length >= 5000)
						{
							output[0].length = length;
						}
						else
							output[0].length = 5000;
						player->hud->HandleNamedEvent("captionhigh");

					}
					//has a priority higher than the mid box
					else if((display.priority.Cmp(display.priority, output[1].Data.priority) <= 0 && !output[1].Data.needsscroll)
						|| (DistanceBetween(display.ent_vec, player->GetWorldCoordinates(player->GetPhysics()->GetOrigin()), output[1].Data.ent_vec, display.priority, output[1].Data.priority, output[1].Data.empty)
						&& (output[1].Data.priority[0] != '0'|| (output[1].Data.timecode && output[1].Data.scroll[output[1].Data.current] == 0))))
						
					{
						int pos = output[2].Data.caption[0].Find("continues", true, 0, -1);
						if(pos > 0)//make sure the continues is removed
						{
							output[2].Data.caption[0] = output[2].Data.caption[0].Mid(0, pos - 1);

						}
						player->hud->HandleNamedEvent("resetMid");
						output[2] = output[1];
						player->hud->SetStateString("Ldialog", output[2].Data.caption[output[2].Data.current]);
						if(output[2].Data.empty)
						{
							if (!output2WasEmpty)	//ADDED 11/23/04 BY JASON
							{
								player->hud->SetStateString("Ldialog", "");
								player->hud->HandleNamedEvent("removedlow");
								player->hud->HandleNamedEvent("LowPHide");
								output[2].Data.radar = false;
							}
						}
						else
						{	//This means that even if there was a caption there before, it will still play the animation.  This is a design decision. - Jason
							player->hud->HandleNamedEvent(output[2].Data.currentcolor + "Low");
							player->hud->SetStateString("Ldialog", output[2].Data.caption[output[2].Data.current]);
							player->hud->HandleNamedEvent("captionlow");
							HandleRadar(output[2].Data.ent_vec - temp, player->viewAngles.yaw, "Low", output[2].Data.currentcolor);
							player->hud->HandleNamedEvent("LowPVis");
							output[2].Data.radar = true;
						}
						display.currentcolor = ccColorInfo.Search(entName);
						player->hud->HandleNamedEvent(display.currentcolor + "Mid");
						player->hud->SetStateString("Mdialog", display.caption[0]);
						output[1].Data = display;
						output[1].Data.radar = true;
						output[1].time = gameLocal.GetTime();
						HandleRadar(output[1].Data.ent_vec - temp, player->viewAngles.yaw, "Mid", output[1].Data.currentcolor);
						player->hud->HandleNamedEvent("MidPVis");
						if(length >= 5000)
						{
							output[1].length = length;
						}
						else
							output[1].length = 5000;
						player->hud->HandleNamedEvent("captionmid");
					}
					//it has a priority higher than the low box
					else if((display.priority.Cmp(display.priority, output[2].Data.priority) <= 0 && !output[2].Data.needsscroll)
						|| (DistanceBetween(display.ent_vec, player->GetWorldCoordinates(player->GetPhysics()->GetOrigin()), output[2].Data.ent_vec, display.priority, output[2].Data.priority, output[2].Data.empty)
						&& (output[2].Data.priority[0] != '0' || (output[2].Data.timecode && output[2].Data.scroll[output[2].Data.current] == 0))))
						
					{
						int pos = output[2].Data.caption[0].Find("continues", true, 0, -1);
						if(pos > 0)//make sure the continues is removed
						{
							output[2].Data.caption[0] = output[2].Data.caption[0].Mid(0, pos - 1);

						}
						player->hud->HandleNamedEvent("resetLow");
						display.currentcolor = ccColorInfo.Search(entName);
						player->hud->HandleNamedEvent(display.currentcolor + "Low");
						player->hud->SetStateString("Ldialog", display.caption[0]);
						output[2].Data = display;
						output[2].Data.radar = true;
						output[2].time = gameLocal.GetTime();
						HandleRadar(output[2].Data.ent_vec - temp, player->viewAngles.yaw, "Low", output[2].Data.currentcolor);
						player->hud->HandleNamedEvent("LowPVis");
						if(length >= 5000)
						{
							output[2].length = length;
						}
						else
							output[2].length = 5000;
						player->hud->HandleNamedEvent("captionlow");
					}

				}
			}
		}
	}
}


void	ccBst::Update()
{
	//Check for language change
	checkLanguage();

	idPlayer *player = gameLocal.GetLocalPlayer();
	idVec3 temppos  = player->GetWorldCoordinates(player->GetPhysics()->GetOrigin());

	//check for the end of the first windowdef box (high priority) and shift accordingly
	if( (!output[0].Data.empty && output[0].length <= gameLocal.GetTime() - output[0].time)
	   || (!inDistance(output[0].Data.ent_vec, player->GetWorldCoordinates(player->GetPhysics()->GetOrigin()), output[0].Data.max, output[0].Data.empty)
	        && !gameLocal.inCinematic && (output[0].Data.priority[0] != '0' || output[0].Data.timecode) ) )
	{
		int pos = output[0].Data.caption[0].Find("continues", true, 0, -1);
		if(pos > 0)//make sure the continues is removed
		{
			output[0].Data.caption[0] = output[0].Data.caption[0].Mid(0, pos - 1);

		}
		output[0] = output[1];
		output[1] = output[2];
		if(output[0].Data.scrolled)
		{
			output[0].Data.scrolled = false;
		}
		//ADDED 1-30-05
		//Make sure the old color follows the caption
		player->hud->HandleNamedEvent(output[0].Data.currentcolor + "High");
		player->hud->SetStateString("Hdialog", output[0].Data.caption[output[0].Data.current]);
		player->hud->HandleNamedEvent("resetHigh");
		player->hud->HandleNamedEvent("HighPVis");
		if(output[0].Data.empty)
		{
			player->hud->HandleNamedEvent("removedhigh");
			player->hud->HandleNamedEvent("HighPHide");
		}
		
		if(output[1].Data.scrolled)
		{
			output[1].Data.scrolled = false;
		}
		player->hud->HandleNamedEvent("resetMid");
		//ADDED 1-30-05
		//Make sure the old color follows the caption
		player->hud->HandleNamedEvent(output[1].Data.currentcolor + "Mid");
		player->hud->SetStateString("Mdialog", output[1].Data.caption[output[1].Data.current]);
		player->hud->HandleNamedEvent("MidPVis");
		if(output[1].Data.empty && !output[0].Data.empty)
		{//added the second check to make sure the previous wasn't empty
			player->hud->HandleNamedEvent("removedmid");
			player->hud->HandleNamedEvent("MidPHide");
		}
		
		
		if(!output[1].Data.empty)
		{//made sure previous wasn't empty
			player->hud->HandleNamedEvent("resetlow");
			player->hud->SetStateString("Ldialog", "");
			player->hud->HandleNamedEvent("removedlow");
			player->hud->HandleNamedEvent("LowPHide");
		}
		output[2].Data.Reset();
		output[2].Data.Clear();
	}

	//check the second, and shift accordingly
	if( (!output[1].Data.empty && output[1].length <= gameLocal.GetTime() - output[1].time)
	   || (!inDistance(output[1].Data.ent_vec, player->GetWorldCoordinates(player->GetPhysics()->GetOrigin()), output[1].Data.max , output[1].Data.empty)
	        && !gameLocal.inCinematic && (output[1].Data.priority[0] != '0' || output[1].Data.timecode) ) )
	{
		int pos = output[1].Data.caption[0].Find("continues", true, 0, -1);
		if(pos > 0)
		{
			output[1].Data.caption[0] = output[1].Data.caption[0].Mid(0, pos - 1);
		}

		output[1] = output[2];
		
		if(output[1].Data.scrolled)
		{
			output[1].Data.scrolled = false;
		}
		player->hud->HandleNamedEvent("resetMid");
		player->hud->HandleNamedEvent("resetlow");
		if(output[1].Data.empty)
		{
			player->hud->SetStateString("Mdialog", "");
			player->hud->HandleNamedEvent("removedmid");
			player->hud->HandleNamedEvent("MidPHide");
		}
		else	//SWITCHED THIS FROM if(output[1].Data.caption != "") TO else FOR EFFICIENCY - Jason on 11/23/04
		{
			//ADDED 1-30-05
			//Make sure the old color follows the caption
			player->hud->HandleNamedEvent(output[1].Data.currentcolor + "Mid");
			player->hud->SetStateString("Mdialog", output[1].Data.caption[output[1].Data.current]);
			player->hud->HandleNamedEvent("MidPVis");
			player->hud->SetStateString("Ldialog", "");
			player->hud->HandleNamedEvent("removedlow");
			player->hud->HandleNamedEvent("LowPHide");
		}
		output[2].Data.Reset();
		output[2].Data.Clear();
	}

	//check third, shift accordingly
	if( (!output[2].Data.empty && output[2].length <= gameLocal.GetTime() - output[2].time)
	    || (!inDistance(output[2].Data.ent_vec, player->GetWorldCoordinates(player->GetPhysics()->GetOrigin()), output[2].Data.max , output[2].Data.empty)
	         && !gameLocal.inCinematic && (output[2].Data.priority[0] != '0' || output[2].Data.timecode)) )
	{//should not need a check since the previous was not empty
		int pos = output[2].Data.caption[0].Find("continues", true, 0, -1);
		if(pos > 0)
		{
			output[2].Data.caption[0] = output[2].Data.caption[0].Mid(0, pos - 1);
		}
		player->hud->SetStateString("Ldialog", "");
		player->hud->HandleNamedEvent("resetLow");
		player->hud->HandleNamedEvent("removedlow");
		player->hud->HandleNamedEvent("LowPHide");
		output[2].Data.Reset();
		output[2].Data.Clear();
		
	}	

	//check for timecode switch, high
	if(output[0].Data.timecode)
	{//due to some problems displaying the last caption, I had to run a check to make sure it isn't the last element
		if(output[0].Data.current != output[0].Data.size -1)
		{
			
			if(gameLocal.GetTime() >= output[0].time + output[0].Data.time[output[0].Data.current + 1])
			{
				output[0].Data.current++;
				player->hud->HandleNamedEvent("resetHigh");
				output[0].Data.scrolled = false;
				gameLocal.Printf("Caption: " + output[0].Data.caption[output[0].Data.current] + "\n");
				player->hud->SetStateString("Hdialog",  output[0].Data.caption[output[0].Data.current]);
			}
		}
	}

	//check for timecode switch, mid
	if(output[1].Data.timecode)
	{
		if(output[1].Data.current != output[1].Data.size -1)
		{
			if(gameLocal.GetTime() >= output[1].time + output[1].Data.time[output[1].Data.current + 1])
			{
				output[1].Data.current++;
				player->hud->HandleNamedEvent("resetMid");
				output[1].Data.scrolled = false;
				gameLocal.Printf("Caption: " + output[1].Data.caption[output[1].Data.current] + "\n");
				player->hud->SetStateString("Mdialog",  output[1].Data.caption[output[1].Data.current]);
			}
		}
	}
	
	if(output[2].Data.timecode)
	{	//check for scrolling in current
		
		//check for timecode switch, low
		if(output[2].Data.current != output[2].Data.size -1)
		{
			if(gameLocal.GetTime() >= output[2].time + output[2].Data.time[output[2].Data.current + 1])
			{
				output[2].Data.current++;
				player->hud->HandleNamedEvent("resetLow");
				output[2].Data.scrolled = false;
				gameLocal.Printf("Caption: " + output[2].Data.caption[output[2].Data.current] + "\n");
				player->hud->SetStateString("Ldialog", output[2].Data.caption[output[2].Data.current]);
			}
		}

	}

//check for scrolling High
	if(output[0].Data.scroll[output[0].Data.current] > 0 && !output[0].Data.scrolled)
	{
		//if(output[0].time + output[0].Data.time[output[0].Data.current] + cc_DELAY >= gameLocal.GetTime())
		//{
			
			if(output[0].length <= output[0].Data.scroll[output[0].Data.current] * 3500 + 3000)
			{
				output[0].length = output[0].Data.scroll[output[0].Data.current] * 3500 + 3000;
			}
			Scroll(output[0].Data.scroll[output[0].Data.current], "High_");
			output[0].Data.scrolled = true;
		//}
	}

	
//check for scrolling Mid
	if(output[1].Data.scroll[output[1].Data.current] > 0 && !output[1].Data.scrolled)
	{
	//	if(output[1].time + output[1].Data.time[output[1].Data.current] + cc_DELAY >= gameLocal.GetTime())
	//	{
			if(output[1].length <= output[1].Data.scroll[output[1].Data.current] * 3500 + 3000)
			{
				output[1].length = output[1].Data.scroll[output[1].Data.current] * 3500 + 3000;
			}
			Scroll(output[1].Data.scroll[output[1].Data.current], "Mid_");
			output[1].Data.scrolled = true;
		//}
	}
//check for scrolling Low
	if(output[2].Data.scroll[output[2].Data.current] > 0 && !output[2].Data.scrolled)
	{
		//if(output[2].time + output[2].Data.time[output[2].Data.current] + cc_DELAY >= gameLocal.GetTime())
		//{ //if it is not longer than the scroll time w/ 1.5 seconds to spare, set the length that long
			if(output[2].length <= output[2].Data.scroll[output[2].Data.current] * 3500 + 3000)
			{
				output[2].length = output[2].Data.scroll[output[2].Data.current] * 3500 + 3000;
			}
			Scroll(output[2].Data.scroll[output[2].Data.current], "Low_");
			output[2].Data.scrolled = true;
		//}
	}

	//These will update the radar accordingly to the sounds first known position, and the players latest.
	if(output[0].Data.radar)
	{
		HandleRadar(output[0].Data.ent_vec - temppos, player->viewAngles.yaw, "High", output[0].Data.currentcolor);
	}

	if(output[1].Data.radar)
	{
		HandleRadar(output[1].Data.ent_vec - temppos, player->viewAngles.yaw, "Mid", output[1].Data.currentcolor);
	}

	if(output[2].Data.radar)
	{
		HandleRadar(output[2].Data.ent_vec - temppos, player->viewAngles.yaw, "Low", output[2].Data.currentcolor);
	}
}


ccParamaters	ccBst::Find(idStr requisition)
{
	ccParamaters temp;
	temp.Reset();
	temp.Clear();
	temp.caption[0] = "ERROR";
	if(!IsEmpty())
	{
		
		Node *p = Tree;
		while(p != NULL)
		{
			//gameLocal.Printf(p->data.soundName + "\n");
			int comp = requisition.Cmp(requisition, p->data.soundName);
			if(comp == 0 || requisition == p->data.soundName)
			{
				temp = p->data;
				//gameLocal.Printf(temp.soundName + "\n");
				p = NULL;
			}
			else if(comp > 0)
			{
				p = p->right;
			}
			else if(comp < 0)
			{
				p = p->left;
			}
		}
	}
	return temp;
}



void	ccBst::Add(ccParamaters newData)
{	
	if(Tree == NULL)
	{
		Node *temp = new Node;
		temp->data = newData;
		temp->left = NULL;
		temp->right = NULL;
		Tree = temp;
	}
	else
	{
		AddHelper(Tree, newData);
	}
}


void	ccBst::AddHelper(Node *head, ccParamaters newData)
{
	if(newData.soundName.Cmp(newData.soundName, head->data.soundName) < 0 && head->left == NULL)
	{
		Node *temp = new Node;
		temp->data = newData;
		temp->left = NULL;
		temp->right = NULL;
		head->left = temp;
		//used to make sure the caption file is formatted correctly 
		//gameLocal.Printf("Left  " + newData.soundName +"   " + head->data.soundName +   "\n");
	}
	else if(newData.soundName.Cmp(newData.soundName, head->data.soundName) > 0 && head->right == NULL)
	{
		Node *temp = new Node;
		temp->data = newData;
		temp->left = NULL;
		temp->right = NULL;
		head->right = temp;
		//used to make sure the caption file is formatted correctly 
		//gameLocal.Printf("Right  " + newData.soundName +"   " + head->data.soundName +   "\n");
	}
	else if(newData.soundName.Cmp(newData.soundName, head->data.soundName) > 0 )
	{
		AddHelper(head->right, newData);
	}
	else if(newData.soundName.Cmp(newData.soundName, head->data.soundName) < 0)
	{
		AddHelper(head->left, newData);
	}
}

int ccBst::toInt(char t)
{
	switch (t)
	{
		case '0' :
			return 0;
		case '1' :
			return 1;
		case '2' :
			return 2;
		case '3' :
			return 3;
		case '4' :
			return 4;
		case '5' :
			return 5;
		case '6' :
			return 6;
		case '7' :
			return 7;
		case '8' :
			return 8;
		case '9' :
			return 9;
		case 'A' :
			return 10;
	}
	return 0;
}
//added 11/29/04
void  ccBst::Scroll(int scr, idStr prefix)
{
	idPlayer *player = gameLocal.GetLocalPlayer();
	switch (scr)
	{
		case 1:
			//gameLocal.Printf("Scrolling 1 \n");
			player->hud->HandleNamedEvent(prefix + "scroll_half");
			break;
		case 2:
			//gameLocal.Printf("Scrolling 2 \n");
			player->hud->HandleNamedEvent(prefix + "scroll");
			break;
		case 3:
			//gameLocal.Printf("Scrolling 3 \n");
			player->hud->HandleNamedEvent(prefix + "scroll_one_and_half");
			break;
		case 4:
			//gameLocal.Printf("Scrolling 4 \n");
			player->hud->HandleNamedEvent(prefix + "scroll_full");
			break;
	}
}



bool ccBst::DistanceBetween(idVec3 ent_vec, idVec3 player_vec, idVec3 old_vec, idStr NP, idStr OP, bool empty)
{
	//idVec3 player_vec = physicsObj.current.origin;
	//return 10;
	int NPI = toInt(NP[0]);
	int OPI = toInt(OP[0]);
	int overide = cc_OVERIDE_DISTANCE * (NPI - OPI);
	float temp = sqrt((player_vec.x - ent_vec.x)*(player_vec.x - ent_vec.x) + (player_vec.y - ent_vec.y)*(player_vec.y - ent_vec.y) +(player_vec.z - ent_vec.z)*(player_vec.z - ent_vec.z))*DOOM_TO_METERS;
	float tmp = sqrt((player_vec.x - old_vec.x)*(player_vec.x - old_vec.x) + (player_vec.y - old_vec.y)*(player_vec.y - old_vec.y) +(player_vec.z - old_vec.z)*(player_vec.z - old_vec.z))*DOOM_TO_METERS;
	//gameLocal.Printf("TMP = " + idStr(tmp) + "\n");
	if(gameLocal.inCinematic)
	{
		return false;
	}
	else if (empty)
	{
		return true;
	}
	
	
	if(overide < 0)
	{
		if(temp < (overide*-1))
		{
			return true;
		}
		else 
		{
			return false;
		}

	}
	if(overide > 0)
	{
		if(tmp > overide)
		{
			return true;
		}
		else
		{
			return false;
		}

	}
	else
	{
		
		if(tmp > temp)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

bool	ccBst::inDistance(idVec3 ent_vec, idVec3 player_vec, float maxDis, bool empty)
{
	/*
	delta = end.ToVec2() - start.ToVec2();
		dist = delta.LengthFast();*/
	if(maxDis <=0)
		return true;
	if(empty)
	{
		return true;
	}
	float temp = sqrt((player_vec.x - ent_vec.x)*(player_vec.x - ent_vec.x) + (player_vec.y - ent_vec.y)*(player_vec.y - ent_vec.y) +(player_vec.z - ent_vec.z)*(player_vec.z - ent_vec.z))*DOOM_TO_METERS;
	//idVec2 tempy= ent_vec.ToVec2() - player_vec.ToVec2();
	//float tmp =  tempy.Length() * DOOM_TO_METERS; //used to verify the correctness of my equation
	//gameLocal.Printf("Max: " + idStr(maxDis) + "Dis: " + idStr(temp) + "\n");
	//gameLocal.Printf("tempy: " + idStr(tmp) + "\n");
	if(temp < maxDis)
		return true;
	else
		return false;


}

idStr	ccBst::getLanguageName(int index)
{
	
	if (languagesLoaded)
	{
		//languages[index].ToLower();
		return languages[index];
	}
	else
		return "english";    //Default to english if languages.dcc missing.
}

bool	ccBst::checkLanguage()
{
	if (language != cc_languageSelector.GetInteger())
	{
		//int oldLanguage = language;		//In case we need to roll back
		Reset();
		language = cc_languageSelector.GetInteger();
		gameLocal.Printf("ccBst::checkLanguage() - the language has changed!\n");
		Init(currentMap, false);
		return true;
	}
	return false;
}

//ADDED Feb 27 2005
bool ccBst::HandleRadar(idVec3 difVec, float vAngle, idStr Prior, idStr Color)
{
	float angle = AngleBetween(difVec, vAngle);

	int dis = sqrt(difVec.x*difVec.x + difVec.y*difVec.y)*DOOM_TO_METERS;
	
	if(dis > 30)
		dis = 30;

	int y = sin(angle*(idMath::PI / 180))*dis;
	float tempf = dis*dis - y*y;
	int x = sqrt(tempf);
	x -= 2;
	y += 2;
	if(x > 30)
		x = 30;
	if(x < -30)
		x = -30;
	if(y > 30)
		x = 30;
	if(y < -30)
		y = -30;
	x = 1.5*x;
	y = 1.5*y;
	if((angle >= 0 && angle <= 90) || angle == 360)
	{
		x = abs(x);
		y = abs(y);
	}
	if((angle > 90 && angle <= 180) || angle == -180)
	{
		x = abs(x) * -1;
		y = abs(y);
	}
	if((angle < 0 && angle >= -90) || (angle >= 270 && angle < 360))
	{
		x = abs(x) *1;
		y = abs(y) *-1;
	}
	if((angle < -90 && angle > -180) || (angle > 180 && angle <270))
	{
		x = abs(x) *-1;
		y = abs(y) *-1;
	}
	//gameLocal.Printf("X: " + idStr(x) + " Y: " + idStr(y) + "Anagle: " + angle + "\n\n");
	y = 45-y;
	x = 45+x;
	x -= 3;
	y += 3;


	idPlayer *player = gameLocal.GetLocalPlayer();
	player->hud->SetStateFloat(Prior+"X", x);
	player->hud->SetStateFloat(Prior+"Y", y);

	//method not working, will needs to be reworked the way it is done for captions
	player->hud->HandleNamedEvent(Color + Prior + "R");
	
	return true;
}



float	ccBst::AngleBetween(idVec3 difVec, float vAngle)
{
	int angle = 0;
	float temp = abs(difVec.y/difVec.x);
	angle = int(atan(temp)*(180/idMath::PI));
	if(difVec.x == 0 && difVec.y == 0)
		angle = 0;
	else if(difVec.x == 0)
	{
		if(difVec.y < 0)
			angle = -270;
		else if(difVec.y > 0)
			angle = 90;
	}
	else if(difVec.y == 0)
	{
		if(difVec.x > 0) 
			angle = 0;
		else if(difVec.x <0 )
			angle = 180;
	}
	
	int tempy = angle;
	angle = angle - vAngle +90;
	if (angle >= 360)
	{
		angle = angle - 360;
	}

	return angle;
}

