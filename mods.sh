#!/bin/sh

if [ $# -lt 2 ]; then
	echo "Usage: $0 <action> <modname> [action args]"
	echo "Supported actions: add, checkout, pull, cmake, build, remove, command"
	echo "Special case: using 'all' as modname runs the action on all checked out mods"
	echo "              For the 'checkout' action, 'all' runs on all mods from modlist.txt"
	echo "The 'cmake', 'build' and 'command' actions allow specifying additional arguments (action args) that are passed to cmake"
	echo "Actions:"
	echo "* 'checkout' (or 'co') checks out the <modname> branch with at mods/modname using git worktree."
	echo "   It expects the branch to exist in the 'origin' remote"
	echo "  '$0 checkout all' does this for all branches listed in modlist.txt"
	echo "* 'add' creates a new local branch for a mod with given modname and checks it out into mods/modname"
	echo "   (if the branch already exists it will be used, this just calls 'git worktree add modname'"
	echo "    without the safeguards of the checkout action that are supposed to prevent typos)"
	echo "* 'pull' runs 'git pull' on <modname> in mods/ to update it from the server (or on all mods when using 'all')"
	echo "* 'cmake' creates a CMake build directory in the <modname>'s directory (or all directories)"
	echo "   You can specify additional options for CMake, e.g. '$0 cmake fitz -G Ninja -DFORCE_COLORED_OUTPUT=ON -DCMAKE_BUILD_TYPE=Debug'"
	echo "   or: $0" 'cmake cdoom -A Win32 -G "Visual Studio 17 2022"'
	echo "* 'build' runs cmake --build for <modnames> build directory (or all mods build directories)"
	echo "   Again you can specify additional arguments that are passed on to CMake (action args)"
	echo "   example for Windows+MSVC: '$0 build cdoom --config RelWithDebInfo'"
	echo "   example for make-based builds: '$0 build cdoom -j16'"
	echo "* 'remove' removes the git worktree for <modname> (or all mods) from mods/"
	echo "   Fails if there are uncommitted changes, unless you use '--force' as action arg"
	echo "   Commits are preserved in its branch, you can restore it with 'checkout'"
	echo "   NOTE that untracked files (like the build/ dir) will be removed!"
	echo "* 'command' lets you specify a custom command (first action arg) which is then run with"
	echo "   the other arguments you specified and the modname as last argument"
	echo "   Example: $0 command all echo I have this mod:"
	echo "   Will print 'I have this mod: bloodmod' 'I have this mod: cdoom' etc"
	exit 1
fi

ACTION="$1"

add_mod() {
	if [ -d "$MOD" ]; then
		echo "Not adding $MOD - already checked out"
	else
		echo "Creating branch '$MOD' and checking it out to directory of that name"
		git worktree add "$MOD"
		echo ""
		echo "You may want to add '$MOD' to modlist.txt"
	fi
}

checkout_mod() {
	if [ -d "$MOD" ]; then
		echo "Skipping $MOD - already checked out"
	else
		if ! git ls-remote --exit-code -b origin "$MOD" > /dev/null; then
			echo "ERROR: There's no branch with name '$MOD' in remote origin!"
		else
			# TODO: this guesses the remote instead of using origin - is there a nice way
			#  to check out the branch locally from origin in the worktree
			#  (without checking it out manually here in the parent?)
			git worktree add --guess-remote "$MOD"
		fi
	fi
}

pull_mod() {
	echo "Updating $MOD (with git pull)"
	cd "$MOD"
	git pull
	cd ..
}

cmake_mod() {
	echo "Setting up CMake build dir for $MOD"
	cmake -S "$MOD/" -B "$MOD/build" $ACTIONARGS
}

build_mod() {
	echo "Building $MOD"
	cmake --build "$MOD/build" $ACTIONARGS
	echo ""
}

remove_mod() {
	echo "Removing $MOD from mods/, its branch (incl. commits) will be preserved"
	git worktree remove $ACTIONARGS "$MOD"
}

command_on_mod() {
	# put $ACTIONARGS into $1, $2 etc
	set $ACTIONARGS
	if [ $# -eq 0 ]; then
		echo "You must provide a custom command to run for the 'command' action!"
		exit 1
	fi
	USERCMD="$1"
	shift
	"$USERCMD" "$@" "$MOD"
}

handle_mod() {
	case "$ACTION" in
		add )
			add_mod
			;;
		checkout | co )
			checkout_mod
			;;
		pull )
			pull_mod
			;;
		cmake )
			cmake_mod
			;;
		build )
			build_mod
			;;
		remove )
			remove_mod
			;;
		command )
			command_on_mod
			;;
		* )
			echo "ERROR: Unknown action $ACTION"
			exit 1
	esac
}

MOD="$2"
shift; shift # now $1 is the first argument after the mod name (I hope :-p)
# FIXME: this doesn't work with arguments that contain a space :-/
#        might need full bash instead of posix shell after all (for arrays)
ACTIONARGS="$@"

if [ ! -d mods ]; then
	if mkdir mods ; then
		echo "Created mods/ directory"
	else
		echo "ERROR: Couldnn't create mods/ directory"
		exit 1
	fi
fi
# all the commands must be run within the mods directory
cd mods

if [ "$MOD" = "all" ]; then
	# if special mod "all" was specified, run the action for all checked out mods (or the ones in modlist.txt for checkout)
	if [ "$ACTION" = "add" ]; then
		# well, except for add, it doesn't make sense without an explicit branch name
		echo 'The "add" action requires specifying one mod name for the newly created branch, not "all"!'
		exit 1
	fi

	if [ "$ACTION" = "checkout" -o "$ACTION" = "co" ]; then
		echo "You want to checkout several mods, fetching origin..."
		git fetch origin

		echo "Checking out all mods in modlist.txt"
		# Note: On Windows Git checks out modlist.txt with CRLF line-endings
		#  which confuses Git Bash's cat... tr -d "\\r" removes the CR part
		#  (not necessary on other platforms, but shouldn't hurt, I think)
		for MOD in $(cat ../modlist.txt | tr -d "\\r"); do
			handle_mod
		done
	else
		for i in */ ; do
			MOD=$(basename "$i")
			handle_mod
		done
	fi
else
	if [ "$ACTION" = "checkout" -o "$ACTION" = "co" ]; then
		echo "You want to checkout a mod, fetching origin..."
		git fetch origin
	fi

	if [ ! -d "$MOD" -a ! "$ACTION" = "co" -a ! "$ACTION" = "checkout" -a ! "$ACTION" = "add" ]; then
		# it's an error if the directory for the mod doesn't exist, except if we're checking it out or adding it
		echo "Mod $MOD isn't checked out!"
		exit 1
	fi
	handle_mod
fi
