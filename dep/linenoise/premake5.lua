project "linenoise"
    language "C"
    kind     "StaticLib"

    filter "system:linux"
        files
        {
            "linenoise/linenoise.h",
            "linenoise/linenoise.c",
        }
