function newplatform(plf)
	local name = plf.name
	local description = plf.description

	-- Register new platform
	premake.platforms[name] = {
		cfgsuffix = "_"..name,
		iscrosscompiler = true
	}

	-- Allow use of new platform in --platfroms
	table.insert(premake.option.list["platform"].allowed, { name, description })
	table.insert(premake.fields.platforms.allowed, name)

	-- Add compiler support
	premake.gcc.platforms[name] = plf.gcc
end

function newgcctoolchain(toolchain)
	newplatform {
		name = toolchain.name,
		description = toolchain.description,
		gcc = {
			cc = toolchain.prefix .. "gcc",
			cxx = toolchain.prefix .. "g++",
			ar = toolchain.prefix .. "ar",
			cppflags = "-MMD " .. toolchain.cppflags
		}
	}
end

newplatform {
	name = "clang",
	description = "Clang",
	gcc = {
		cc = "clang",
		cxx = "clang++",
		ar = "ar",
		cppflags = "-MMD "
	}
}

newgcctoolchain {
	name = "mingw32",
	description = "Mingw32 to cross-compile windows binaries from *nix",
	prefix = "i686-w64-mingw32-",
	cppflags = ""
}

if _OPTIONS.platform then
	-- overwrite the native platform with the options::platform
	premake.gcc.platforms['Native'] = premake.gcc.platforms[_OPTIONS.platform]
end

function string.starts(String,Start)
	if ( _ACTION ) then
		return string.sub(String,1,string.len(Start))==Start
	end
	
	return false
end

function is_vs()
	return ( string.starts(_ACTION,"vs") )
end

solution "SOIL2"
	location("./make/" .. os.get() .. "/")
	targetdir("./bin")
	configurations { "debug", "release" }
	objdir("obj/" .. os.get() .. "/")
	
	project "soil2-static-lib"
		kind "StaticLib"

		if is_vs() then
			language "C++"
			buildoptions { "/TP" }
			defines { "_CRT_SECURE_NO_WARNINGS" }
		else
			language "C"
		end
		
		targetdir("lib/" .. os.get() .. "/")
		files { "src/SOIL2/*.c" }

		configuration "debug"
			defines { "DEBUG" }
			flags { "Symbols" }
			if not is_vs() then
				buildoptions{ "-Wall" }
			end
			targetname "soil2-debug"

		configuration "release"
			defines { "NDEBUG" }
			flags { "Optimize" }
			targetname "soil2"

	project "soil2-test"
		kind "WindowedApp"
		language "C++"
		links { "soil2-static-lib" }
		files { "src/test/*.cpp" }
		
		if os.is("windows") and not is_vs() then
			links { "mingw32" }
		end

		configuration "mingw32"
			links { "mingw32" }

		configuration "windows"
			links {"opengl32","SDL2main","SDL2"}

		configuration "linux"
			links {"GL","SDL2"}
		
		configuration "macosx"
			links {"OpenGL.framework","CoreFoundation.framework","SDL2.framework"}
		
		configuration "haiku"
			links {"GL","SDL2"}

		configuration "freebsd"
			links {"GL","SDL2"}
		
		configuration "debug"
			defines { "DEBUG" }
			flags { "Symbols" }
			if not is_vs() then
				buildoptions{ "-Wall" }
			end
			targetname "soil2-test-debug"

		configuration "release"
			defines { "NDEBUG" }
			flags { "Optimize" }
			targetname "soil2-test-release"
			
	project "soil2-perf-test"
		kind "ConsoleApp"
		language "C++"
		links { "soil2-static-lib" }
		files { "src/perf_test/*.cpp" }

		if os.is("windows") and not is_vs() then
			links { "mingw32" }
		end
		
		configuration "mingw32"
			links { "mingw32" }

		configuration "windows"
			links {"opengl32","SDL2main","SDL2"}

		configuration "linux"
			links {"GL","SDL2"}
		
		configuration "macosx"
			links {"OpenGL.framework","CoreFoundation.framework","SDL2.framework"}
		
		configuration "haiku"
			links {"GL","SDL2"}

		configuration "freebsd"
			links {"GL","SDL2"}
		
		configuration "debug"
			defines { "DEBUG" }
			flags { "Symbols" }
			if not is_vs() then
				buildoptions{ "-Wall" }
			end
			targetname "soil2-perf-test-debug"

		configuration "release"
			defines { "NDEBUG" }
			flags { "Optimize" }
			targetname "soil2-perf-test-release"
