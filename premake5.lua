workspace "Shirox"
    configurations { "Debug", "Release" }
    architecture "x64"
    startproject "Engine"

    global_output_dir = "%{cfg.buildcfg}/%{cfg.architecture}"
    root_dir = path.getabsolute(".")
    
    include "Engine"
