addmenu Curves "Line"    {
    dialbox line name l origin {0 0 0} direction {1 0 0}
}
addmenu Curves "Circle"  {
    dialbox circle name c center {0 0 0} normal {0 0 1} xdir {1 0 0} radius 1
}
addmenu Curves "Ellipse"  {
    dialbox ellipse name e center {0 0 0} normal {0 0 1} xdir {1 0 0} radii {1 0.5}
}
addmenu Curves "Hyperbola"  {
    dialbox hyperbola name h center {0 0 0} normal {0 0 1} xdir {1 0 0} radii {1 0.5}
}

addmenu Curves "Parabola"  {
    dialbox parabola name b center {0 0 0} normal {0 0 1} xdir {1 0 0} focus 1
}
