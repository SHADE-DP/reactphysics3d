project "reactphysics3d"
	kind "StaticLib"
	language "C++"
	staticruntime "off"
	targetdir (outputdir)
	objdir (interdir)

	files
	{
		"include/**.h",
		"include/**.hpp",
		"src/**.cpp",
	}

	includedirs
	{
	  "include"
	}

    excludes
    {
    }

	links
	{
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
