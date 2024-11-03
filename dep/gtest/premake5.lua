project "gtest"
    kind       "StaticLib"
    language   "C++"
    cppdialect "C++14"

    includedirs { "include", "." }

    files
    {
        "src/gtest-all.cc"
    }

    filter "system:linux"
        buildoptions { "-DGTEST_HAS_PTHREAD=1" }

    filter "system:windows"
        buildoptions { "-utf-8" }
