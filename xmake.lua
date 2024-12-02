set_runtimes(is_mode("debug") and "MDd" or "MD")
set_languages("c++23")
set_policy("build.ccache", false)

add_rules("mode.debug", "mode.release", "mode.coverage")

target("1st", function()
    set_kind("binary")
    add_files("src/1st/**.cpp")
end)

target("2nd", function()
    set_kind("binary")
    add_files("src/2nd/**.cpp")
end)