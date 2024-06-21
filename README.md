# Sikkmod

**Short description from the original Sikkmod 1.2 README:**

Sikkmod is a general enhancement mod for id Software's 2004 FPS: Doom 3. Its main
purposes are to update visuals of Doom 3 to a more modern level and to allow the user
to customize various aspects of the game to suit their preferred styles all while 
maintaining the core mechanics and aesthetics originally put in by id Software.

The mod also includes a number of fixes to and better organization of stock files in order
to create a clean and more stable base for future mods.

----

This is a dhewm3 port of Sikkmod v1.2 for the base game and v1.1 for Resurrection of Evil (d3xp;
apparently there was no Sikkmod v1.2 for Resurrection of Evil).

You can get the game data at https://www.moddb.com/mods/sikkmod/downloads/sikkmod-v12
and https://www.moddb.com/mods/sikkmod/downloads/sikkmod-v11-roe

## License

For a long time it was unclear whether the Sikkmod C++ source could be released under GPL, because
Sikkpin disappeared from the scene many years ago and I wasn't able to contact him to ask for permission.

Luckily it turned out that *eezstreet* **did** ask him for permission back in 2011, soon after the
Doom3 source code was released under GPL, see https://github.com/dhewm/dhewm3/issues/120#issuecomment-1555662168

I'll quote the mail exchange here:

*Eezstreet wrote:*
> **From:** *(eezstreet's mail address)*  
> **To:** *(sikkpin's mail address)*  
> **Subject:** using sikkmod src with doom3.gpl  
> **Date:** Fri, 25 Nov 2011 10:34:32 -0500  
>
> I was wondering if it was okay to use sikkmod as a base in my doom3.gpl project? The visuals are
> extremely pleasing and other features (picking up physics objects for example) would be handy.

*Sikkpin's reply:*
> **To:** *(eezstreet's mail address)*
> 
> Yeah, of course. The only stipulation is that anything that is being used from the mod that is my 
> work can not be sold or used in any commercial product. Also, keep in mind that there are some 
> things in there that are not my own so you would need to get permission from the respective authors
> (check the readme). And one last thing, there's a lot in there that does not fall under the gpl 
> and can not be included legally. Basically, the only stuff I can legally give your permission to 
> use is the code and the shadrs, everything else (scripts, defs, sounds, textures, etc.) that aren't
> completely custom can't be used.
>
> Feel free to contact me again if you have any questions.
>
>
> Cheers,
>
> Sikk

So the C++ source code (found here) and the shaders may be used under GPL, but the game data can't.

Furthermore, Sikkpin would apparently prefer if his work wasn't used in commercial products (even
though the GPL allows that), so please be nice and respect his wish :-)
