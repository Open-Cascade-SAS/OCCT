set exclude_list [list \
    "bugs caf bug31075" \
    "bugs caf bug31546" \
    "bugs fclasses bug6143" \
    "bugs moddata_2 bug712_2" \
    "bugs moddata_1 bug20627" \
    "bugs modalg_6 bug27884" \
    "lowalgos intss bug565" \
    "lowalgos intss bug567_1" \
    "lowalgos intss bug23972" \
    "lowalgos intss bug29910_2" \
    "opengl background bug27836" \
    "opengl text C4" \
    "opengles3 background bug27836" \
    "opengles3 general msaa" \
    "opengles3 geom interior1" \
    "opengles3 geom interior2" \
    "opengles3 raytrace msaa" \
    "opengles3 text C4" \
    "opengles3 textures alpha_mask" \
    "boolean bopfuse_simple ZP6" \
    "boolean gdml_private B5" \
    "bugs modalg_1 bug19071" \
    "bugs modalg_5 bug25199"
]

set exclude_str [join $exclude_list ,]
testgrid -exclude {*}$exclude_str -outdir results/linux-gcc-x64