Draw: Demo Scripts {#samples__draw_scripts}
================

All demo scripts are shipped with OCCT sources under the <i>resources/samples/tcl</i> directory. To run them:

1. Source the build environment script `env.sh` (Linux/macOS) or `env.bat` (Windows) generated next to *DRAWEXE* in the build / install `bin/` directory.
2. Launch *DRAWEXE*.
3. From the DRAW prompt, source any demo script by absolute or `$CASROOT`-relative path, for example:

       source $env(CASROOT)/resources/samples/tcl/bottle.tcl

A few representative demos:

  * <b>DataExchangeDemo.tcl</b>: writes and reads an IGES file
  * <b>ModelingDemo.tcl</b>: creates a simple shape and displays it in HLR mode
  * <b>VisualizationDemo.tcl</b>: exercises the 3D viewer
  * <b>bottle.tcl</b>: builds the bottle from the OCCT Tutorial
  * <b>cad.tcl</b>, <b>drill.tcl</b>, <b>cutter.tcl</b>: solid modeling examples
  * <b>xde.tcl</b>: creates a simple assembly in XDE
  * <b>materials.tcl</b>, <b>raytrace.tcl</b>, <b>vis_pbr_spheres.tcl</b>, <b>pathtrace_*.tcl</b>: visualization, materials, ray-tracing and PBR demos
  * <b>dimensions.tcl</b>: dimensions, clipping and capping in the 3D viewer

For the full up-to-date list, see the contents of the `resources/samples/tcl/` directory.

Draw is a command interpreter based on TCL and a graphical system used for testing and demonstrating OCCT modeling libraries.
It can be used interactively to create, display, and modify objects such as curves, surfaces, and topological shapes.
Draw also provides a scriptable environment for OCCT tests and demonstrations.

Draw includes:

  * a TCL command interpreter;
  * 2D and 3D viewers with zoom, pan, rotation, and fit operations;
  * geometric and topological commands for OCCT modeling algorithms;
  * graphic commands for view and display operations;
  * OCAF, data exchange, and shape healing commands loaded by plug-ins.

Commands grouped in toolkits can be loaded at run time with *pload*.
The plug-in mechanism and instructions for extending Draw are described in @ref occt_user_guides__test_harness.
Draw Test Harness is also used by the OCCT automated testing system; see @ref occt_contribution__tests "Automated Test System" for details.

Experimenting with Draw Test Harness
------------------------------------

Running Draw
------------

**On Linux:**

* Launch *DRAWEXE* executable. The Draw[1]> prompt appears in the command window.

Type *pload ALL*

**On Windows:**

Launch Draw executable from Open CASCADE Technology\\DRAW Test Harness 
item of the Start\\Programs menu. The Draw[1]> prompt appears in the command window.

Type *pload ALL*

**Creating your first geometric objects**

1. In the command window, type *axo* to create an axonometric view
2. Type *box b -10 -10 -10 20 20 20* to create a cube *b* of size 20, parallel to the X Y Z axis and centered on the origin. The cube will be displayed in the axonometric view in wireframe mode.
3. Type *fit* to fill the viewer with the cube
4. Type *pcylinder c 2 30* to create a cylinder *c* of radius 2 and height 30. The cylinder will be displayed in addition to the cube

**Manipulating the view**

1. Type *clear* to erase the view
2. Type *donly c* to display the cylinder only
3. Type *donly b* to display the cube only
4. Type *hlr hlr b* to display the cube in the hidden line removal mode

**Running demonstration files**

Use `source $env(CASROOT)/resources/samples/tcl/<demo_file>` from the DRAW prompt, replacing `<demo_file>` with the name of the demonstration script.

**Getting Help**

1. Type *help* to see all available commands
2. Type *help \<command_name\>* to find out the arguments for a given command
