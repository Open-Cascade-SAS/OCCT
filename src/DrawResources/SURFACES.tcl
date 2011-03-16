proc dialanasurf {command sname args} {
    set com "dialbox $command name $sname origin {0 0 0} normal {0 0 1} xdir {1 0 0} "
    foreach l $args {append com " $l"}
    eval $com
}

addmenu Surfaces "Plane"      {dialanasurf plane p {}}
addmenu Surfaces "Cylinder"   {dialanasurf cylinder c {radius 1}}
addmenu Surfaces "Cone"       {dialanasurf cone c {angle 30 radius 0}}
addmenu Surfaces "Sphere"     {dialanasurf sphere s {radius 1}}
addmenu Surfaces "Torus"      {dialanasurf torus t {radii {1 0.8}}}
addmenu Surfaces "Revolution" {dialbox revsur name r basis . origin {0 0 0} axis {0 0 1}}
addmenu Surfaces "Extrusion"  {dialbox extsurf name e basis . direction {0 0 1}}
