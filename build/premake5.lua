-- premake5.lua

local wxWidgetsVersion = "3.1.4"
local wxWidgetsArchFolder = ""
local wxWidgetsFileFilter = ""
local sdlVersion = "2.0.14"
local sdlArchFolder = ""

function download_progress(total, current)
   local ratio = current / total;
   ratio = math.min(math.max(ratio, 0), 1);
   local percent = math.floor(ratio * 100);
   print("Download progress (" .. percent .. "%/100%)")
end

function check_raylib()
   if(os.isdir("../libraries/raylib") == false and os.isdir("../libraries/raylib-master") == false) then
       if(not os.isfile("../libraries/raylib-master.zip")) then
           print("Raylib not found, downloading from github")
           local result_str, response_code = http.download("https://github.com/raysan5/raylib/archive/refs/heads/master.zip", "../libraries/raylib-master.zip", {
               progress = download_progress,
               headers = { "From: Premake", "Referer: Premake" }
           })
       end
       print("Unzipping to " ..  "../libraries/")
       zip.extract("../libraries/raylib-master.zip", "../libraries/")
       os.remove("../libraries/raylib-master.zip")
   end
end

function EmulationPlatform()
   kind "SharedLib"

   includedirs {
      "../libraries/SDL2-"..sdlVersion.."/include",
   }

   defines {
      "DLL_BUILD",
      "HAVE_CONFIG_H"
   }

   filter "architecture:x86"
      sdlArchFolder = "x86"

   filter "architecture:x86_64"
      sdlArchFolder = "x64"
   
   filter { }

   libdirs {
      "../libraries/SDL2-"..sdlVersion.."/lib/"..sdlArchFolder
   }

   links { "SDL2", "SDL2main" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

   -- Reset the filter for other settings
   filter { }
end

workspace "MultiEmu3D"
   -- We set the location of the files Premake will generate
   location "%{_ACTION}"

   -- We indicate that all the projects are C++ only
   language "C++"
   
   -- We will compile for x86_64. You can change this to x86 for 32 bit builds.
   architecture "x86_64"

   -- Configurations are often used to store some compiler / linker settings together.
   -- The Debug configuration will be used by us while debugging.
   -- The optimized Release configuration will be used when shipping the app.
   configurations { "Debug", "Release" }

   defines {
       "_WINDOWS",
       "_UNICODE",
       "UNICODE",
       "_CRT_SECURE_NO_DEPRECATE",
       "_CRT_NONSTDC_NO_DEPRECATE"
    }
    
   -- Here we use some "tokens" (the things between %{ ... }). They will be replaced by Premake
	-- automatically when configuring the projects.
	-- * %{prj.name} will be replaced by "MultiEmu3D"
	--  * %{cfg.longname} will be replaced by "Debug" or "Release" depending on the configuration
   -- The path is relative to *this* folder
   targetdir "%{wks.location}/bin/%{cfg.buildcfg}"
	objdir "%{wks.location}/obj/%{cfg.buildcfg}"

check_raylib();
include ("raylib_premake5.lua")

--------
-- GB --
--------
project "GB"
   files {
      "../src/Common/**.h",
      "../src/Common/**.cpp",
      "../src/GB-GBC/**.h",
      "../src/GB-GBC/**.cpp",
      "../libraries/Gb_Snd_Emu-0.1.4/**.h",
      "../libraries/Gb_Snd_Emu-0.1.4/**.cpp"
   }

   removefiles {
      "../src/Common/SoundPortaudio.*",
      "../libraries/Gb_Snd_Emu-0.1.4/demo.cpp",
      "../libraries/Gb_Snd_Emu-0.1.4/demo_sdl.cpp"
   }
   
   includedirs {
      "../libraries/Gb_Snd_Emu-0.1.4"
   }

   EmulationPlatform()


---------
-- SMS --
---------
project "SMS"
   files {
      "../src/Common/**.h",
      "../src/Common/**.cpp",
      "../src/SMS-GG/**.h",
      "../src/SMS-GG/**.cpp",
      "../libraries/Sms_Snd_Emu-0.1.1/**.h",
      "../libraries/Sms_Snd_Emu-0.1.1/**.cpp"
   }

   removefiles {
      "../src/Common/SoundPortaudio.*",
      "../libraries/Sms_Snd_Emu-0.1.1/demo.cpp"
   }
   
   includedirs {
      "../libraries/Sms_Snd_Emu-0.1.1"
   }

   EmulationPlatform()

---------
-- NES --
---------
project "NES"
   files {
      "../src/Common/**.h",
      "../src/Common/**.cpp",
      "../src/NES/**.h",
      "../src/NES/**.cpp"
   }

   removefiles {
      "../src/Common/SoundPortaudio.*"
   }
   
   includedirs {
      
   }

   EmulationPlatform()

-------------------------------
-- Nativefiledialog-extended --
-------------------------------
project "nativefiledialog-extended"
   kind "StaticLib"

   includedirs {
      "../libraries/nativefiledialog-extended/src/include"
   }

   filter "system:windows"
      files {
         "../libraries/nativefiledialog-extended/src/nfd_win.cpp"
      }

      defines { "nfd_PLATFORM=PLATFORM_WIN32" }

      links { "ole32", "uuid", "shell32" }
   filter { }

--------------------------
-- MultiEmu3D wxWidgets --
--------------------------
project "MultiEmu3D_wx"

   dependson { "GB", "SMS", "NES" }
   
   kind "WindowedApp"
   -- Turn on DPI awareness (Default, None, High, HighPerMonitor)
   dpiawareness "High"

   files {
      "../src/Common/**.h",
      "../src/Common/**.cpp",
      "../src/GUI/**.h",
      "../src/GUI/**.cpp",
      "../src/GUI/Windows/MultiEmu3D.rc",
      "../libraries/glew-1.10.0/src/glew.c",
    }

   removefiles {
      "../src/Common/SoundPortaudio.*",
      "../src/Common/Debuggable.*"
    }
    
   includedirs {
      "../libraries/wxWidgets-"..wxWidgetsVersion.."/include/msvc",
      "../libraries/wxWidgets-"..wxWidgetsVersion.."/include",
      "../libraries/glew-1.10.0/include",
      "../libraries/SDL2-"..sdlVersion.."/include"
   }

   defines {
      "__WXMSW__",
      "WXUSINGDLL",
      "wxMSVC_VERSION_AUTO",
      "wxMSVC_VERSION_ABI_COMPAT",
      "GLEW_STATIC"
   }
   
   filter "architecture:x86"
      wxWidgetsArchFolder = "vc14x_dll"
      sdlArchFolder = "x86"

   filter "architecture:x86_64"
      wxWidgetsArchFolder = "vc14x_x64_dll"
      sdlArchFolder = "x64"
   
   filter { }

   libdirs {
      "../libraries/SDL2-"..sdlVersion.."/lib/"..sdlArchFolder,
      "../libraries/wxWidgets-"..wxWidgetsVersion.."/lib/"..wxWidgetsArchFolder
   }

   links { "opengl32", "glu32", "SDL2", "SDL2main", "GB", "SMS", "NES" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      wxWidgetsFileFilter = "*314ud_*.dll"

      -- copy dlls from the libraries directories to the target directory
      postbuildcommands {
         "{COPY} %{wks.location}\\..\\..\\libraries\\wxWidgets-"..wxWidgetsVersion.."\\lib\\"..wxWidgetsArchFolder.."\\"..wxWidgetsFileFilter.." %{wks.location}\\bin\\%{cfg.buildcfg}",
         "{COPY} %{wks.location}\\..\\..\\libraries\\SDL2-"..sdlVersion.."\\lib\\"..sdlArchFolder.."\\SDL2.dll %{wks.location}\\bin\\%{cfg.buildcfg}",
      }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      wxWidgetsFileFilter = "*314u_*.dll"

      -- copy dlls from the libraries directories to the target directory
      postbuildcommands {
         "{COPY} %{wks.location}\\..\\..\\libraries\\wxWidgets-"..wxWidgetsVersion.."\\lib\\"..wxWidgetsArchFolder.."\\"..wxWidgetsFileFilter.." %{wks.location}\\bin\\%{cfg.buildcfg}",
         "{COPY} %{wks.location}\\..\\..\\libraries\\SDL2-"..sdlVersion.."\\lib\\"..sdlArchFolder.."\\SDL2.dll %{wks.location}\\bin\\%{cfg.buildcfg}",
      }

   -- Reset the filter for other settings
   filter { }

-----------------------
-- MultiEmu3D raylib --
-----------------------
project "MultiEmu3D_raylib"

   dependson { "GB", "SMS", "NES", "raylib", "nativefiledialog-extended" }
   
   kind "ConsoleApp"
   -- Turn on DPI awareness (Default, None, High, HighPerMonitor)
   -- dpiawareness "High"

   cppdialect "C++17"

   files {
      "../src/Common/**.h",
      "../src/Common/**.cpp",
      "../src/GUI_raylib/**.h",
      "../src/GUI_raylib/**.cpp",
      "../src/GUI_raylib/**.rc",
      "../libraries/physfs/src/**.h",
      "../libraries/physfs/src/**.c",
   }

   removefiles {
      "../src/Common/SoundPortaudio.*",
      "../src/Common/SoundSDL.*",
      "../src/Common/Debuggable.*"
   }

   includedirs {
      "../libraries/raygui",
      "../libraries/physfs/src",
      "../libraries/raylib-physfs",
      "../libraries/nativefiledialog-extended/src/include"
   }

   include_raylib()
   
   filter { }

   links { "GB", "SMS", "NES", "nativefiledialog-extended" }
   link_raylib()

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

   -- Reset the filter for other settings
   filter { }