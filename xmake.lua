add_rules("mode.debug", "mode.release")

local target_name = "cao"
local mode = "debug"

task("deploy")
    on_run(function ()
        local temp_path = "/data/local/tmp"
        local app_path = "/data/app/~~K8dkU8uLo_8wNikXdfRq1Q==/com.netease.yhtj-6GO9pN-rbwp8R3A0M_KgjQ==/lib/arm64/lib" .. target_name .. ".so"

        os.exec("adb push build/android/arm64-v8a/" .. mode .. "/lib" .. target_name .. ".so " .. temp_path .. "/lib" .. target_name .. ".so")
        os.exec('adb shell "su -c \\"chmod 777 ' .. temp_path .. '/lib' .. target_name .. '.so\\""')
        os.exec('adb shell "su -c \\"cp ' .. temp_path .. '/lib' .. target_name .. '.so ' .. app_path .. '\\""')
    end)

    local projectName = "dobby"

target(projectName)
    set_kind("static")
    set_languages("cxx20")
    set_exceptions("cxx")

    add_includedirs("dobby", { public = true }) 
    add_headerfiles("*.h")

    after_build(function (target)
        local arch = target:arch()
        if arch == "arm64-v8a" then
            os.cp("dobby/arm64-v8a/libdobby.a", path.join(target:targetdir(), "libdobby.a"))
        elseif arch == "armeabi-v7a" then
            os.cp("dobby/armeabi-v7a/libdobby.a", path.join(target:targetdir(), "libdobby.a"))
        elseif arch == "x86" then
            os.cp("dobby/x86/libdobby.a", path.join(target:targetdir(), "libdobby.a"))
        end
    end)

    add_links("dobby")

target(target_name)
    set_kind("shared")
    set_languages("c++20")

    -- add_deps("dobby")
    add_includedirs("dobby", { public = true }) 
    add_headerfiles("*.h")
    add_linkdirs("dobby/arm64-v8a")
    add_links("dobby")
    add_defines("_LIBCPP_ENABLE_EXPERIMENTAL")

    add_files("src/main.cpp","src/elf_util.cpp")

    after_build(function (target)
        import("core.project.task")
        task.run("deploy")
    end)

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

