# A branch to checkout all mods of this repo into a worktree

The contained `cmod.sh` script can be used to checkout mod branches into `mods/` (using `git worktree`),
for example:  
`./cmod.sh checkout d3le`  
will checkout the branch for Doom3: Lost Mission (d3le) into `mods/d3le/`.  
`./cmod.sh checkout all`  
will checkout all mod branches listed in acmodlist.txt into `mods/`.

It also allows updating those branches from the remote server with git pull, creating CMake
build directories for them (in `mods/modname/build/`), building them in those directories or
custom commands and removing it from mods/ again.  
Run `./cmod.sh` without any arguments to get more information.

This branch is useful if you want to build libs ("Game DLLs") for all mods or want to apply patches
to all of them and similar tasks.

`acmodlist.txt` contains a list of mod branches that are automatically checked out when using `./cmod.sh all`.  
All other `./cmod.sh` actions (besides `checkout`) are run on all directories in `mods/`,
no matter if they're listed in `acmodlist.txt` or not.

## NABAAQ (Never Asked But Answered Anyway Questions)

**Why would I want this, it's confusing**

Don't use it then, this is primarily to make maintainers lives easier

**WTF does "cmod.sh" stand for**

I don't know, maybe "control mod" or something?  
At first I called it "mods.sh" but it turned out that this sucks for tab-completion,
so I prefixed it with another character...

**And acmodlist.txt?!**

Auto-Checkout-modlist, obviously.

Or maybe all-checkout-modlist? Who knows.
