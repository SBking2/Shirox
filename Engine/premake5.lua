project "Engine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir (root_dir .. "/Build/%{prj.name}/" .. global_output_dir)
    objdir (root_dir .. "/Build-Int/%{prj.name}/" .. global_output_dir)

    files { "**.cpp", "**.h", "**.hpp" }

    includedirs {
        root_dir .. "/vendor/GLFW/include",
        root_dir .. "/vendor/GLM",
        root_dir .. "/vendor/STB",
        root_dir .. "/vendor/TinyObjLoader",
        root_dir .. "/vendor/Assimp/include",
        "F:/vulkan/Include",
        "src"
    }

    libdirs{
        root_dir .. "/vendor/GLFW/bin/Debug-windows-x86_64/GLFW",
        root_dir .. "/vendor/Assimp",
        "F:/vulkan/Lib"
    }

    links{
        "vulkan-1",
        "GLFW",
        "assimp-vc143-mtd"
    }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
        symbols "Off"

    