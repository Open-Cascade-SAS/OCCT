set exclude_list [list \
    "bugs caf bug31918_1" \
    "bugs fclasses bug6143" \
    "bugs fclasses bug25574" \
    "bugs fclasses bug29064" \
    "chamfer dist_angle_sequence A5" \
    "collections n arrayMove" \
    "opengl background bug27836" \
    "opengl drivers d3dhost" \
    "opengl background srgb" \
    "opengl text C4" \
    "opengles2 text C4" \
    "opengles3 text C4" \
    "boolean gdml_private B5" \
    "chamfer dist_angle A3" \
    "chamfer dist_angle E5" \
    "chamfer dist_angle_complex A1" \
    "chamfer dist_angle_complex A4" \
    "chamfer dist_angle_complex A5" \
    "chamfer dist_angle_sequence A1" \
    "chamfer dist_angle_sequence A4"
]

set exclude_str [join $exclude_list ,]
testgrid -exclude {*}$exclude_str -outdir results/windows-clang-x64