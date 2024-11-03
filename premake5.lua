workspace "RulesAnalyzer"
	configurations { "Release", "Debug" }
	architecture "x86_64"
	location ("build/")
	staticruntime "on"

	filter "configurations:Debug"
		symbols     "On"

	filter "configurations:Release"
		optimize    "Full"

	group "dependencies"
		include "dep/buddy"
		include "dep/linenoise"
		include "dep/fmtlib"
		include "dep/gtest"

	group ""
		project "rulan"
			kind "ConsoleApp"
			language "C++"
			cppdialect "C++14"
			includedirs { "dep/linenoise", "dep/buddy", "dep/fmtlib/include", "src" }

			files {
				"src/global.h",
				"src/main.cpp",

				"src/tools/*.h",
				"src/tools/*.hpp",
				"src/tools/*.cpp",
				"src/tools/*.c",
				"src/model/*.h",
				"src/model/*.cpp",
				"src/ostore/*.h",
				"src/ostore/*.cpp",
				"src/cli/*.h",
				"src/cli/*.cpp",
				"src/cli/fw/*.h",
				"src/cli/fw/*.cpp",
				"src/cli/nw/*.h",
				"src/cli/nw/*.cpp",
				"src/cli/os/*.h",
				"src/cli/os/*.cpp",
			}

			filter "system:linux"
				links { "buddy", "linenoise", "fmt" }
			filter "system:windows"
                buildoptions { "-utf-8" }
				links { "buddy", "Ws2_32", "fmt" }

		project "tests"
			kind "ConsoleApp"
			language "C++"
			cppdialect "C++14"
			includedirs { "dep/linenoise", "dep/buddy", "dep/fmtlib/include", "dep/gtest/include", "src" }

			files {
				"src/global.h",

				"src/tools/*.h",
				"src/tools/*.hpp",
				"src/tools/*.cpp",
				"src/tools/*.c",
				"src/model/*.h",
				"src/model/*.cpp",
				"src/ostore/*.h",
				"src/ostore/*.cpp",

				"tests/main.cpp",
				"tests/test_range.cpp",
				"tests/test_domains.cpp",
				"tests/test_address4.cpp",
				"tests/test_address6.cpp",
				"tests/test_network4.cpp",
				"tests/test_network6.cpp",
				"tests/test_analyzer4.cpp",
				"tests/test_analyzer6.cpp"
			}

			filter "system:linux"
				links { "buddy", "fmt", "pthread", "gtest" }
			filter "system:windows"
				buildoptions { "-utf-8" }
				links { "Ws2_32", "buddy", "fmt", "gtest" }
