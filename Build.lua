-- premake5.lua
workspace "Monty08Wrench"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "WrenchRuntime"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Core"
	include "Assembler-Core/Build-Core.lua"
	include "Parser-Core/Build-Core.lua"
	include "Preprocessor-Core/Build-Core.lua"
group ""

include "Wrench-Runtime/Build-App.lua"