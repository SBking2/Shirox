project "Engine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir (root_dir .. "/Build/%{prj.name}/" .. global_output_dir)
    objdir (root_dir .. "/Build-Int/%{prj.name}/" .. global_output_dir)

    files { "**.cpp", "**.h", "**.hpp" }

    includedirs {
        "vendor/glfw/include",
        "vendor/glm",
        "vendor/STB",
        "vendor/TinyObjLoader",
        "vendor/assimp/include",
        os.getenv("VK_SDK_PATH") .. "/Include",
        "src"
    }

    libdirs{
        "vendor/glfw/Build/%{cfg.buildcfg}",
        "vendor/assimp/CMakeBuild/lib/Debug",
        os.getenv("VK_SDK_PATH") .. "/Lib"
    }

    links{
        "vulkan-1",
        "glfw",
        "assimp-vc143-mtd"
    }

    pchheader "PCH.h"
    pchsource "src/PCH.cpp"

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
        symbols "Off"

    