-- premake5.lua
workspace "MultiEmu3D"
   -- We set the location of the files Premake will generate
   location "%{_ACTION}"

   -- We indicate that all the projects are C++ only
   language "C++"
   
   -- We will compile for x86_64. You can change this to x86 for 32 bit builds.
   architecture "x86"

   -- Configurations are often used to store some compiler / linker settings together.
   -- The Debug configuration will be used by us while debugging.
   -- The optimized Release configuration will be used when shipping the app.
   configurations { "Debug", "Release" }

   defines {
       "__WXMSW__",
       "_WINDOWS",
       "_UNICODE",
       "UNICODE",
       "WXUSINGDLL",
       "wxMSVC_VERSION_AUTO",
       "_CRT_SECURE_NO_DEPRECATE",
       "_CRT_NONSTDC_NO_DEPRECATE",
       "GLEW_STATIC"
    }
    
    -- Here we use some "tokens" (the things between %{ ... }). They will be replaced by Premake
	-- automatically when configuring the projects.
	-- * %{prj.name} will be replaced by "MultiEmu3D"
	--  * %{cfg.longname} will be replaced by "Debug" or "Release" depending on the configuration
    -- The path is relative to *this* folder
    targetdir "%{wks.location}/bin/%{cfg.buildcfg}"
	objdir "%{wks.location}/obj/%{cfg.buildcfg}"

project "MultiEmu3D"
   kind "WindowedApp"

   files {
       "../src/**.h",
       "../src/**.cpp",
       "../src/GUI/Windows/MultiEmu3D.rc",
       "../libraries/Gb_Snd_Emu-0.1.4/**.h",
       "../libraries/Gb_Snd_Emu-0.1.4/**.cpp",
       "../libraries/Sms_Snd_Emu-0.1.1/**.h",
       "../libraries/Sms_Snd_Emu-0.1.1/**.cpp",
       "../libraries/glew-1.10.0/src/glew.c",
    }
   removefiles {
       "../src/Common/SoundPortaudio.*",
       "../libraries/Gb_Snd_Emu-0.1.4/demo.cpp",
       "../libraries/Gb_Snd_Emu-0.1.4/demo_sdl.cpp",
       "../libraries/Sms_Snd_Emu-0.1.1/demo.cpp",
    }
   includedirs {
        "../libraries/wxWidgets-3.0.5/include/msvc",
        "../libraries/wxWidgets-3.0.5/include",
        "../libraries/glew-1.10.0/include",
        "../libraries/SDL2-2.0.14/include",
        "../libraries/Sms_Snd_Emu-0.1.1",
        "../libraries/Gb_Snd_Emu-0.1.4"
   }
   libdirs {
        "../libraries/SDL2-2.0.14/lib/x86",
        "../libraries/wxWidgets-3.0.5/lib/vc142_dll"
   }
   links { "opengl32", "glu32", "SDL2", "SDL2main" }
    
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

    -- Reset the filter for other settings
    filter { }

    -- copy a file from the objects directory to the target directory
   postbuildcommands {
      "{COPY} %{wks.location}\\..\\..\\libraries\\wxWidgets-3.0.5\\lib\\vc142_dll\\*.dll %{wks.location}\\bin\\%{cfg.buildcfg}",
      "{COPY} %{wks.location}\\..\\..\\libraries\\SDL2-2.0.14\\lib\\x86\\SDL2.dll %{wks.location}\\bin\\%{cfg.buildcfg}",
   }