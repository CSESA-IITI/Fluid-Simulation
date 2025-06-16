
require "vendor/export-compile-commands"

-- Define the workspace

workspace "FluidSimulation"
    architecture "x64" -- x86_64 architecture
    configurations { "Debug", "Release" }
    startproject "FluidSim"

    -- Output directory format
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    -- Define the main project
    project "FluidSim"
        kind "ConsoleApp" -- Console application
        language "C++"
        cppdialect "C++17" -- C++17 standard

        -- Output directories
        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

        -- Source files
        files {
            "src/**.cpp",
            "src/**.hpp", -- All .cpp files in src/
            "include/**.hpp", -- All .h files in include/ for IDE visibility
			"vendor/**.h",
            "vendor/**.cpp"
        }

        -- Include directories
        includedirs {
            "include", -- Project headers (core/, renderer/)
			"vendor",
        }

        -- Link system-installed libraries (e.g., GLFW, OpenGL, GLEW)
        filter "system:linux"
            links {
				"vulkan",
                "glfw", -- GLFW
            }
            defines { "PLATFORM_LINUX" }

        filter "system:windows"
            links {
                "vulkan-1", -- vulkan-1.lib
                "glfw3", -- glfw3.lib
                "Triton"
            }
            defines { "PLATFORM_WINDOWS" }
            -- Vulkan SDK include and library paths (using VULKAN_SDK environment variable)
            includedirs {
                "C:/VulkanSDK/1.4.304.0/Include",
                "C:/glfw/glfw-3.4.bin.WIN64/include",
                "./Triton/include"
            }
            libdirs {
                "C:/VulkanSDK/1.4.304.0/Lib",
                "C:/glfw/glfw-3.4.bin.WIN64/lib-vc2022",
                "./Triton/bin/" .. outputdir .. "/Triton"
            }

        -- Configuration-specific settings
        filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On" -- Debugging symbols
            optimize "Off"

        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "On"

		
