workspace "BBSScenario"

configurations {"Debug", "Release"}
platforms {"Win32"}
filter "configurations:Debug"
	defines {"DEBUG"} 
	symbols "on" 
	optimize "off"
	runtime "debug"
filter "configurations:Release"
	defines {"NDEBUG"} 
	optimize "on"
	runtime "release"
	flags {"NoRuntimeChecks", "LinkTimeOptimization"}
filter({})
staticruntime "off"
targetdir "%{prj.location}/bin/%{cfg.buildcfg}/%{cfg.platform}"
objdir "%{prj.location}/obj/%{cfg.buildcfg}/%{cfg.platform}"

project "WorldFlag"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"  -- Needed for proper <filesystem> support.
    system "Windows" 
    architecture "x32"
    flags {"NoPCH"}
    files { "WorldFlag/**.h", "WorldFlag/**.c", "WorldFlag/**.hpp", "WorldFlag/**.cpp" }