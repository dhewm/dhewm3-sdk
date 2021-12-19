cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(dhewm3sdk)

option(BASE		"Build the base (game/) game code" ON)
set(BASE_NAME	"hardcorps" CACHE STRING "Name of the mod built from game/ (will result in \${BASE_NAME}.dll)")
# FIXME: maybe try to get it to work with assertions enabled (i.e. in Debug builds or in Release builds without NDEBUG)
set(BASE_DEFS	"GAME_DLL;_WATER_PHYSICS;_PORTALSKY;_DENTONMOD;NDEBUG" CACHE STRING "Compiler definitions for the mod built from game/")

option(D3XP		"Build the d3xp/ game code" OFF)
set(D3XP_NAME	"d3xp" CACHE STRING "Name of the mod built from d3xp/ (will result in \${D3XP_NAME}.dll)")
set(D3XP_DEFS	"GAME_DLL;_D3XP;CTF" CACHE STRING "Compiler definitions for the mod built from d3xp/")

option(ONATIVE	"Optimize for the host CPU" OFF)

set(src_game_mod
	# add additional .cpp files of your mod in game/
	# (that you added to the ones already existant in the SDK/in dhewm3)
	# like "game/MyFile.cpp" (without quotes, one file per line)
	game/Liquid.cpp
	game/ai/AI_bot.cpp
	game/physics/Physics_Liquid.cpp
	game/tracer.cpp
	game/TrailGenerator.cpp
)

set(src_d3xp_mod
	# add additional .cpp files of your mod in d3xp/
	# (that you added to the ones already existant in the SDK/in dhewm3)
	# like "d3xp/MyFile.cpp" (without quotes, one file per line)
)


########################################################################
# You /probably/ don't need to change anything below here for your Mod #
########################################################################

# TODO
# osx: place game .dylib's in the bundle (next to the binary)
# osx: -weak_framework ?

# maybe add these as options:
# TARGET_MONO
# SETUP
# SDK				-D_D3SDK

# don't add these as options, but document them?
# IDNET_HOST		-DIDNET_HOST=\\"%s\\"' % IDNET_HOST
# DEBUG_MEMORY		-DID_DEBUG_MEMORY', '-DID_REDIRECT_NEWDELETE
# LIBC_MALLOC		-DUSE_LIBC_MALLOC=0
# ID_NOLANADDRESS	-DID_NOLANADDRESS

# fallback for cmake versions without add_compile_options
if(NOT COMMAND add_compile_options)
	function(add_compile_options)
		foreach(arg ${ARGN})
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${arg}" PARENT_SCOPE)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${arg}" PARENT_SCOPE)
		endforeach()
	endfunction()
endif()

if(NOT CMAKE_SYSTEM_PROCESSOR)
	message(FATAL_ERROR "No target CPU architecture set")
endif()

if(NOT CMAKE_SYSTEM_NAME)
	message(FATAL_ERROR "No target OS set")
endif()

# target cpu
set(cpu ${CMAKE_SYSTEM_PROCESSOR})
if(cpu STREQUAL "powerpc")
	set(cpu "ppc")
elseif(cpu MATCHES "i.86")
	set(cpu "x86")
elseif(cpu MATCHES "[aA][mM][dD]64" OR cpu MATCHES "[xX]64")
	set(cpu "x86_64")
endif()

# On Windows ${CMAKE_SYSTEM_PROCESSOR} is broken (always returns host CPU)
# which has only been reported >7 years ago (https://cmake.org/pipermail/cmake-developers/2014-September/011405.html)
# so obviously a fix is too much to ask. Here's the special case to make that wonderful platform work;
# except if it's Windows for ARM(64) I guess, no idea how to detect that properly (I don't own such hardware).
if(MSVC)
	if(cpu MATCHES ".*[aA][rR][mM].*")
		message(FATAL_ERROR "please fix this code to work for Windows on ARM and send a pull request")
	endif()
	if(CMAKE_CL_64)
		set(cpu "x86_64")
	else()
		set(cpu "x86")
	endif()
elseif(DEFINED ENV{MINGW_CHOST})
	# looks like it's broken in MinGW32 shells (or 32bit mingw-w64 shells) as well, this should help:
	message(STATUS "MINGW_CHOST = $ENV{MINGW_CHOST}")
	if($ENV{MINGW_CHOST} MATCHES "^i.86.*")
		set(cpu "x86")
	elseif($ENV{MINGW_CHOST} MATCHES "^x86_64.*")
		set(cpu "x86_64")
	else()
		message(FATAL_ERROR "please fix this code to work for MINGW_CHOST = $ENV{MINGW_CHOST} and send a pull request!")
	endif()
endif()

add_definitions(-DD3_ARCH="${cpu}" -DD3_SIZEOFPTR=${CMAKE_SIZEOF_VOID_P})

# target os
if(APPLE)
	set(os "macosx")
else()
	string(TOLOWER "${CMAKE_SYSTEM_NAME}" os)
endif()

add_definitions(-DD3_OSTYPE="${os}")

message(STATUS "Setting -DD3_ARCH=\"${cpu}\" -DD3_SIZEOFPTR=${CMAKE_SIZEOF_VOID_P} -DD3_OSTYPE=\"${os}\" ")

if(cpu MATCHES ".*64.*" AND NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
	# tough luck if some CPU architecture has "64" in its name but uses 32bit pointers
	message(SEND_ERROR "CMake thinks sizeof(void*) == 4, but the target CPU looks like a 64bit CPU!")
	message(FATAL_ERROR "If you're building in a 32bit chroot on a 64bit host, switch to it with 'linux32 chroot' or at least call cmake with linux32 (or your OSs equivalent)!")
endif()

# build type
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()


include(CheckCXXCompilerFlag)
include(TestBigEndian)

# compiler specific flags
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
	add_compile_options(-pipe)
	add_compile_options(-Wall)

	if(NOT CMAKE_CROSSCOMPILING AND ONATIVE)
		add_compile_options(-march=native)
	elseif(NOT APPLE AND cpu STREQUAL "x86")
		add_compile_options(-march=pentium3)
	endif()

	set(CMAKE_C_FLAGS_DEBUG "-g -D_DEBUG -O0")
	set(CMAKE_C_FLAGS_DEBUGALL "-g -ggdb -D_DEBUG")
	set(CMAKE_C_FLAGS_PROFILE "-g -ggdb -D_DEBUG -O1 -fno-omit-frame-pointer")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -fno-unsafe-math-optimizations -fno-math-errno -fno-trapping-math -fomit-frame-pointer")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -ggdb -fno-unsafe-math-optimizations -fno-math-errno -fno-trapping-math -fno-omit-frame-pointer")
	set(CMAKE_C_FLAGS_MINSIZEREL "-Os -fno-unsafe-math-optimizations -fno-math-errno -fno-trapping-math -fomit-frame-pointer")

	set(CMAKE_CXX_FLAGS_DEBUGALL ${CMAKE_C_FLAGS_DEBUGALL})
	set(CMAKE_CXX_FLAGS_PROFILE ${CMAKE_C_FLAGS_PROFILE})

	add_compile_options(-fno-strict-aliasing)

	if(NOT AROS)
		CHECK_CXX_COMPILER_FLAG("-fvisibility=hidden" cxx_has_fvisibility)
		if(NOT cxx_has_fvisibility)
			message(FATAL_ERROR "Compiler does not support -fvisibility")
		endif()
		add_compile_options(-fvisibility=hidden)
	endif()

	# TODO fix these warnings
	add_compile_options(-Wno-sign-compare)
	add_compile_options(-Wno-switch)
	add_compile_options(-Wno-strict-overflow)
	add_compile_options(-Wno-format-security)
	
	if(CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.0)
		# ‘void* memset(void*, int, size_t)’ clearing an object of type ‘struct refSound_t’ with no trivial copy-assignment; use assignment or value-initialization instead
		# ‘void* memcpy(void*, const void*, size_t)’ writing to an object of type ‘class idDrawVert’ with no trivial copy-assignment; use copy-assignment or copy-initialization instead
		# etc - I don't care, all the cases I've checked were safe and I'm not gonna add (void*) casts in 1000 places
		#       (that would make merging new mods painful)
		add_compile_options(-Wno-class-memaccess)
	endif()

	CHECK_CXX_COMPILER_FLAG("-Woverloaded-virtual" cxx_has_Woverload_virtual)
	if(cxx_has_Woverload_virtual)
		add_compile_options(-Woverloaded-virtual)
	endif()

	if(AROS)
		set(CMAKE_SHARED_LIBRARY_SUFFIX ".aros-${cpu}")
		add_definitions(-DIOAPI_NO_64)
	elseif(APPLE)
		add_definitions(-DMACOS_X=1)

		if(cpu STREQUAL "x86_64")
			add_compile_options(-arch x86_64 -mmacosx-version-min=10.6)
			set(ldflags "${ldflags} -arch x86_64 -mmacosx-version-min=10.6")
		elseif(cpu STREQUAL "arm64")
			add_compile_options(-arch arm64 -mmacosx-version-min=11.0)
			set(ldflags "${ldflags} -arch arm64 -mmacosx-version-min=11.0")
		elseif(cpu STREQUAL "x86")
			CHECK_CXX_COMPILER_FLAG("-arch i386" cxx_has_arch_i386)
			if(cxx_has_arch_i386)
				add_compile_options(-arch i386)
				set(ldflags "${ldflags} -arch i386")
			endif()

			add_compile_options(-mmacosx-version-min=10.4)
			set(ldflags "${ldflags} -mmacosx-version-min=10.4")
		elseif(cpu STREQUAL "ppc")
			CHECK_CXX_COMPILER_FLAG("-arch ppc" cxx_has_arch_ppc)
			if(cxx_has_arch_ppc)
				add_compile_options(-arch ppc)
				set(ldflags "${ldflags} -arch ppc")
			endif()

			add_compile_options(-mmacosx-version-min=10.4)
			set(ldflags "${ldflags} -mmacosx-version-min=10.4")
		else()
			message(FATAL_ERROR "Unsupported CPU architecture for OSX")
		endif()
	elseif(WIN32)
		set(ldflags "${ldflags} -static-libgcc -static-libstdc++")
	elseif(os STREQUAL "linux")
		# set(sys_libs ${sys_libs} dl) FIXME: -ldl needed anyway?
	endif()
elseif(MSVC)
	add_definitions(/MP) # parallel build (use all cores, or as many as configured in VS)

	add_compile_options(/W4)
	add_compile_options(/we4840) # treat as error when passing a class to a vararg-function (probably printf-like)
	add_compile_options(/wd4100) # unreferenced formal parameter
	add_compile_options(/wd4127) # conditional expression is constant
	add_compile_options(/wd4244) # possible loss of data
	add_compile_options(/wd4245) # signed/unsigned mismatch
	add_compile_options(/wd4267) # possible loss of data
	add_compile_options(/wd4714) # 'function' marked as __forceinline not inlined
	add_compile_options(/wd4996) # 'function': was declared deprecated
	add_compile_options(/wd4068) # unknown pragma
	add_compile_options(/wd4458) # declaration of 'bla' hides class member
	add_definitions(-D_ALLOW_KEYWORD_MACROS) # because of the "#define private public" and "#define protected public" in TypeInfo.cpp
	set(CMAKE_C_FLAGS_DEBUG "-D_DEBUG /Od /Zi /MDd")
	set(CMAKE_C_FLAGS_RELEASE "/Ox /Oy /MD")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "/Ox /Oy /Zi /MD")
	set(CMAKE_C_FLAGS_MINSIZEREL "/Ox /Oy /Os /MD")
else()
	message(FATAL_ERROR "Unsupported compiler")
endif()

set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO})
set(CMAKE_CXX_FLAGS_MINSIZEREL ${CMAKE_C_FLAGS_MINSIZEREL})

# mingw and msvc
if(WIN32)
	add_definitions(-DWINVER=0x0501)
	add_definitions(-D_WIN32_WINNT=0x0501)
endif()

set(bindir		"${CMAKE_INSTALL_FULL_BINDIR}")
set(libdir		"${CMAKE_INSTALL_FULL_LIBDIR}/dhewm3")
set(datadir		"${CMAKE_INSTALL_FULL_DATADIR}/dhewm3")

TEST_BIG_ENDIAN(is_big_endian)

configure_file(
	"${CMAKE_SOURCE_DIR}/config.h.in"
	"${CMAKE_BINARY_DIR}/config.h"
)

message(STATUS "Building ${CMAKE_BUILD_TYPE} for ${os}-${cpu}")

if(NOT APPLE AND NOT WIN32)
	message(STATUS "The install target will use the following directories:")
	message(STATUS "  Binary directory:  ${bindir}")
	message(STATUS "  Library directory: ${libdir}")
	message(STATUS "  Data directory:    ${datadir}")
endif()

# I'm a bit sloppy with headers and just glob them in..
# they're only handled in CMake at all so they turn up in Visual Studio solutions..

# globs all the headers from ${PATHPREFIX}/ and adds them to ${SRCLIST}
function(add_globbed_headers SRCLIST PATHPREFIX)
	file(GLOB_RECURSE tmp_hdrs RELATIVE "${CMAKE_SOURCE_DIR}" "${PATHPREFIX}/*.h")
	set(${SRCLIST} ${tmp_hdrs} ${${SRCLIST}} PARENT_SCOPE)
endfunction()

if(CMAKE_MAJOR_VERSION LESS 3 OR ( CMAKE_MAJOR_VERSION EQUAL 3 AND CMAKE_MINOR_VERSION LESS 8 ))
	# cmake < 3.8 doesn't support source_group(TREE ...) so replace it with a dummy
	# (it's only cosmetical anyway, to make source files show up properly in Visual Studio)
	function(source_group)
	endfunction()
	message(STATUS "Using CMake < 3.8, doesn't support source_group(TREE ...), replacing it with a dummy")
	message(STATUS "  (this is only relevants for IDEs, doesn't matter for just compiling dhewm3)")
#else()
#	message(STATUS "Using CMake >= 3.8, supports source_group(TREE ...)")
endif()


set(src_game
	game/AF.cpp
	game/AFEntity.cpp
	game/Actor.cpp
	game/Camera.cpp
	game/Entity.cpp
	game/BrittleFracture.cpp
	game/Fx.cpp
	game/GameEdit.cpp
	game/Game_local.cpp
	game/Game_network.cpp
	game/Item.cpp
	game/IK.cpp
	game/Light.cpp
	game/Misc.cpp
	game/Mover.cpp
	game/Moveable.cpp
	game/MultiplayerGame.cpp
	game/Player.cpp
	game/PlayerIcon.cpp
	game/PlayerView.cpp
	game/Projectile.cpp
	game/Pvs.cpp
	game/SecurityCamera.cpp
	game/SmokeParticles.cpp
	game/Sound.cpp
	game/Target.cpp
	game/Trigger.cpp
	game/Weapon.cpp
	game/WorldSpawn.cpp
	game/ai/AAS.cpp
	game/ai/AAS_debug.cpp
	game/ai/AAS_pathing.cpp
	game/ai/AAS_routing.cpp
	game/ai/AI.cpp
	game/ai/AI_events.cpp
	game/ai/AI_pathing.cpp
	game/ai/AI_Vagary.cpp
	game/gamesys/DebugGraph.cpp
	game/gamesys/Class.cpp
	game/gamesys/Event.cpp
	game/gamesys/SaveGame.cpp
	game/gamesys/SysCmds.cpp
	game/gamesys/SysCvar.cpp
	game/gamesys/TypeInfo.cpp
	game/anim/Anim.cpp
	game/anim/Anim_Blend.cpp
	game/anim/Anim_Import.cpp
	game/anim/Anim_Testmodel.cpp
	game/script/Script_Compiler.cpp
	game/script/Script_Interpreter.cpp
	game/script/Script_Program.cpp
	game/script/Script_Thread.cpp
	game/physics/Clip.cpp
	game/physics/Force.cpp
	game/physics/Force_Constant.cpp
	game/physics/Force_Drag.cpp
	game/physics/Force_Field.cpp
	game/physics/Force_Spring.cpp
	game/physics/Physics.cpp
	game/physics/Physics_AF.cpp
	game/physics/Physics_Actor.cpp
	game/physics/Physics_Base.cpp
	game/physics/Physics_Monster.cpp
	game/physics/Physics_Parametric.cpp
	game/physics/Physics_Player.cpp
	game/physics/Physics_RigidBody.cpp
	game/physics/Physics_Static.cpp
	game/physics/Physics_StaticMulti.cpp
	game/physics/Push.cpp

	${src_game_mod}
)

add_globbed_headers(src_game "game")

set(src_d3xp
	d3xp/AF.cpp
	d3xp/AFEntity.cpp
	d3xp/Actor.cpp
	d3xp/Camera.cpp
	d3xp/Entity.cpp
	d3xp/BrittleFracture.cpp
	d3xp/Fx.cpp
	d3xp/GameEdit.cpp
	d3xp/Game_local.cpp
	d3xp/Game_network.cpp
	d3xp/Item.cpp
	d3xp/IK.cpp
	d3xp/Light.cpp
	d3xp/Misc.cpp
	d3xp/Mover.cpp
	d3xp/Moveable.cpp
	d3xp/MultiplayerGame.cpp
	d3xp/Player.cpp
	d3xp/PlayerIcon.cpp
	d3xp/PlayerView.cpp
	d3xp/Projectile.cpp
	d3xp/Pvs.cpp
	d3xp/SecurityCamera.cpp
	d3xp/SmokeParticles.cpp
	d3xp/Sound.cpp
	d3xp/Target.cpp
	d3xp/Trigger.cpp
	d3xp/Weapon.cpp
	d3xp/WorldSpawn.cpp
	d3xp/ai/AAS.cpp
	d3xp/ai/AAS_debug.cpp
	d3xp/ai/AAS_pathing.cpp
	d3xp/ai/AAS_routing.cpp
	d3xp/ai/AI.cpp
	d3xp/ai/AI_events.cpp
	d3xp/ai/AI_pathing.cpp
	d3xp/ai/AI_Vagary.cpp
	d3xp/gamesys/DebugGraph.cpp
	d3xp/gamesys/Class.cpp
	d3xp/gamesys/Event.cpp
	d3xp/gamesys/SaveGame.cpp
	d3xp/gamesys/SysCmds.cpp
	d3xp/gamesys/SysCvar.cpp
	d3xp/gamesys/TypeInfo.cpp
	d3xp/anim/Anim.cpp
	d3xp/anim/Anim_Blend.cpp
	d3xp/anim/Anim_Import.cpp
	d3xp/anim/Anim_Testmodel.cpp
	d3xp/script/Script_Compiler.cpp
	d3xp/script/Script_Interpreter.cpp
	d3xp/script/Script_Program.cpp
	d3xp/script/Script_Thread.cpp
	d3xp/physics/Clip.cpp
	d3xp/physics/Force.cpp
	d3xp/physics/Force_Constant.cpp
	d3xp/physics/Force_Drag.cpp
	d3xp/physics/Force_Field.cpp
	d3xp/physics/Force_Spring.cpp
	d3xp/physics/Physics.cpp
	d3xp/physics/Physics_AF.cpp
	d3xp/physics/Physics_Actor.cpp
	d3xp/physics/Physics_Base.cpp
	d3xp/physics/Physics_Monster.cpp
	d3xp/physics/Physics_Parametric.cpp
	d3xp/physics/Physics_Player.cpp
	d3xp/physics/Physics_RigidBody.cpp
	d3xp/physics/Physics_Static.cpp
	d3xp/physics/Physics_StaticMulti.cpp
	d3xp/physics/Push.cpp
	d3xp/Grabber.cpp
	d3xp/physics/Force_Grab.cpp

	${src_d3xp_mod}
)

add_globbed_headers(src_d3xp "d3xp")

set(src_idlib
	idlib/bv/Bounds.cpp
	idlib/bv/Frustum.cpp
	idlib/bv/Sphere.cpp
	idlib/bv/Box.cpp
	idlib/geometry/DrawVert.cpp
	idlib/geometry/Winding2D.cpp
	idlib/geometry/Surface_SweptSpline.cpp
	idlib/geometry/Winding.cpp
	idlib/geometry/Surface.cpp
	idlib/geometry/Surface_Patch.cpp
	idlib/geometry/TraceModel.cpp
	idlib/geometry/JointTransform.cpp
	idlib/hashing/CRC32.cpp
	idlib/hashing/MD4.cpp
	idlib/hashing/MD5.cpp
	idlib/math/Angles.cpp
	idlib/math/Lcp.cpp
	idlib/math/Math.cpp
	idlib/math/Matrix.cpp
	idlib/math/Ode.cpp
	idlib/math/Plane.cpp
	idlib/math/Pluecker.cpp
	idlib/math/Polynomial.cpp
	idlib/math/Quat.cpp
	idlib/math/Rotation.cpp
	idlib/math/Simd.cpp
	idlib/math/Simd_Generic.cpp
	idlib/math/Simd_AltiVec.cpp
	idlib/math/Simd_MMX.cpp
	idlib/math/Simd_3DNow.cpp
	idlib/math/Simd_SSE.cpp
	idlib/math/Simd_SSE2.cpp
	idlib/math/Simd_SSE3.cpp
	idlib/math/Vector.cpp
	idlib/BitMsg.cpp
	idlib/LangDict.cpp
	idlib/Lexer.cpp
	idlib/Lib.cpp
	idlib/containers/HashIndex.cpp
	idlib/Dict.cpp
	idlib/Str.cpp
	idlib/Parser.cpp
	idlib/MapFile.cpp
	idlib/CmdArgs.cpp
	idlib/Token.cpp
	idlib/Base64.cpp
	idlib/Timer.cpp
	idlib/Heap.cpp
)

add_globbed_headers(src_idlib "idlib")
# just add all the remaining headers (that have no corresponding .cpp in the SDK)
# to idlib so they can be navigated there
add_globbed_headers(src_idlib "cm")
add_globbed_headers(src_idlib "framework")
add_globbed_headers(src_idlib "renderer")
add_globbed_headers(src_idlib "sound")
add_globbed_headers(src_idlib "sys")
add_globbed_headers(src_idlib "tools")
add_globbed_headers(src_idlib "ui")


include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})

add_library(idlib STATIC ${src_idlib})
if (AROS)
	add_library(dll STATIC ${src_arosdll})
        if(CMAKE_SYSTEM_PROCESSOR STREQUAL "i386")
		set(AROS_ARCH "x86")
        else()
		set(AROS_ARCH ${CMAKE_SYSTEM_PROCESSOR})
	endif()
else()
	if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID STREQUAL "Clang" AND NOT MINGW)
		set_target_properties(idlib PROPERTIES COMPILE_FLAGS "-fPIC")
	endif()
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX neo FILES ${src_idlib})

if(BASE)
	if (AROS)
		add_executable(base sys/aros/dll/dllglue.c ${src_game})
		set_target_properties(base PROPERTIES OUTPUT_NAME "${BASE_NAME}.aros-${AROS_ARCH}")
	else()
		add_library(base SHARED ${src_game})
		# so mods can create cdoom.dll instead of base.dll from the code in game/
		set_target_properties(base PROPERTIES OUTPUT_NAME "${BASE_NAME}")
	endif()

	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX neo FILES ${src_game})

	set_target_properties(base PROPERTIES PREFIX "")
	set_target_properties(base PROPERTIES COMPILE_DEFINITIONS "${BASE_DEFS}")
	target_include_directories(base PRIVATE "${CMAKE_SOURCE_DIR}/game")
	set_target_properties(base PROPERTIES LINK_FLAGS "${ldflags}")
	set_target_properties(base PROPERTIES INSTALL_NAME_DIR "@executable_path")
	if (AROS)
		target_link_libraries(base idlib dll)
	else()
		target_link_libraries(base idlib)
	endif()

	if(NOT APPLE AND NOT WIN32)
		install(TARGETS base
				RUNTIME DESTINATION "${bindir}"
				LIBRARY DESTINATION "${libdir}"
				ARCHIVE DESTINATION "${libdir}"
		)
	endif()
endif()

if(D3XP)
	if (AROS)
		add_executable(d3xp sys/aros/dll/dllglue.c ${src_d3xp})
		set_target_properties(d3xp PROPERTIES OUTPUT_NAME "${D3XP_NAME}.aros-${AROS_ARCH}")
	else()
		add_library(d3xp SHARED ${src_d3xp})
		# so mods can create whatever.dll instead of d3xp.dll from the code in d3xp/
		set_target_properties(d3xp PROPERTIES OUTPUT_NAME "${D3XP_NAME}")
	endif()

	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX neo FILES ${src_d3xp})

	set_target_properties(d3xp PROPERTIES PREFIX "")
	set_target_properties(d3xp PROPERTIES COMPILE_DEFINITIONS "${D3XP_DEFS}")
	target_include_directories(d3xp PRIVATE "${CMAKE_SOURCE_DIR}/d3xp")
	set_target_properties(d3xp PROPERTIES LINK_FLAGS "${ldflags}")
	set_target_properties(d3xp PROPERTIES INSTALL_NAME_DIR "@executable_path")
	if (AROS)
		target_link_libraries(d3xp idlib dll)
	else()
		target_link_libraries(d3xp idlib)
	endif()

	if(NOT APPLE AND NOT WIN32)
		install(TARGETS d3xp
				RUNTIME DESTINATION "${bindir}"
				LIBRARY DESTINATION "${libdir}"
				ARCHIVE DESTINATION "${libdir}"
		)
	endif()
endif()
