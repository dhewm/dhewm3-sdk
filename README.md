# A branch to checkout all mods of this repo into a worktree

The contained `mods.sh` script can be used to checkout mod branches into `mods/` (using `git worktree`),
for example:  
`./mods.sh checkout d3le`  
will checkout the branch for Doom3: Lost Mission (d3le) into `mods/d3le/`.  
`./mods.sh checkout all`  
will checkout all mod branches listed in modlist.txt into `mods/`.

It also allows updating those branches from the remote server with git pull, creating CMake
build directories for them (in `mods/modname/build/`), building them in those directories or
custom commands and removing it from mods/ again.  
Run `./mods.sh` without any arguments to get more information.

This branch is useful if you want to build libs ("Game DLLs") for all mods or want to apply patches
to all of them and similar tasks.

`modlist.txt` contains a list of mod branches that are automatically checked out when using `./mods.sh all`.  
All other `./mods.sh` actions (besides `checkout`) are run on all directories in `mods/`,
no matter if they're listed in `modlist.txt` or not.
