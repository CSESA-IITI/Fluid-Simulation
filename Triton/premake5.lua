require "vendor/export-compile-commands"

-- Define the workspace
workspace "Triton"
    architecture "x64" -- x86_64 architecture
    configurations { "Debug", "Release" }
    startproject "Triton"

    -- Output directory format
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    -- Define the main project
    project "Triton"
        kind "StaticLib" -- Changed from ConsoleApp to StaticLib for library output
        language "C++"
        cppdialect "C++17" -- C++17 standard

        -- Output directories
        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

        -- Source files
        files {
            "src/**.cpp",-- All .cpp files in src/
            "src/**.hpp", 
            "include/**.hpp", -- All .h files in include/ for IDE visibility
            "vendor/**.h"
        }

        -- Include directories (for use by projects that link against this library)
        includedirs {
            "include", -- Project headers (core/, renderer/)
            "vendor"
        }

        -- Link system-installed libraries (e.g., GLFW, Vulkan)
        filter "system:linux"
            links {
                "vulkan",
                "glfw" -- GLFW
            }
            defines { "PLATFORM_LINUX" }

        filter "system:windows"
            links {
                "vulkan-1", -- vulkan-1.lib
                "glfw3" -- glfw3.lib
            }
            defines { "PLATFORM_WINDOWS" }
            -- Vulkan SDK include and library paths (using VULKAN_SDK environment variable)
            includedirs {
                "C:/VulkanSDK/1.4.304.0/Include",
                "C:/glfw/glfw-3.4.bin.WIN64/include"
            }
            libdirs {
                "C:/VulkanSDK/1.4.304.0/Lib",
                "C:/glfw/glfw-3.4.bin.WIN64/lib-vc2022"
            }

        -- Configuration-specific settings
        filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On" -- Debugging symbols
            optimize "Off"

        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "On"
		
