Draw Demo Scripts {#samples__draw_scripts}
================

All demo scripts are provided with OCCT sources and locate in <i>CASROOT/samples/tcl</i>. To play around them please 
follow the steps below:

1. Start DRAWEXE 
2. Type *cd ../..* to return to the root directory
3. Type *cd samples/tcl* to reach the *DrawResources* directory
4. Type *source \<demo_file\>* to run the demonstration file provided with Open CASCADE. The following demonstration 
files are available:
  * <b>DataExchangeDemo.tcl</b>: demonstrates sample sequence of operations with writing and reading IGES file
  * <b>ModelingDemo.tcl</b>: demonstrates creation of simple shape and displaying it in HLR mode
  * <b>VisualizationDemo.tcl</b>: demonstrates use of 3d viewer
  * <b>cad.tcl</b>: creates solid shape looking like abbreviation "CAD"
  * <b>bottle.tcl</b>: creates bottle as in OCCT Tutorial
  * <b>drill.tcl</b>: creates twist drill bit shape
  * <b>cutter.tcl</b>: creates milling cutter shape
  * <b>xde.tcl</b>: demonstrates creation of simple assembly in XDE
  * <b>materials.tcl</b>: demonstrates visual properties of materials supported by 3d viewer
  * <b>raytrace.tcl</b>: demonstrates use of ray tracing display in 3d viewer
  * <b>dimensions.tcl</b>: demonstrates use of dimensions, clipping, and capping in 3d viewer
  * ...

Draw is a command interpreter based on TCL and a graphical system used for testing and demonstrating OCCT modeling libraries.

Draw can be used interactively to create, display and modify objects such as curves, surfaces and topological shapes.

@figure{/introduction/images/overview_draw.png}

Scripts can be written to customize Draw and perform tests. 
New types of objects and new commands can be added using C++ programming language.

Draw contains:

  * A command interpreter based on TCL command language.
  * A 2D an 3D graphic viewer with support of operations such as zoom, pan, rotation and full-screen views.
  * An optional set of geometric commands to create and modify curves and surfaces and to use OCCT geometry algorithms.
  * A set of topological commands to create and modify BRep shapes and to use OCCT topology algorithms.
  * A set of graphic commands for view and display operations including Mesh Visualization Service.
  * A set of Application framework commands for handling of files and attributes.
  * A set of Data Exchange commands for translation of files from various formats (IGES,STEP) into OCCT shapes.
  * A set of Shape Healing commands: check of overlapping edges, approximation of a shape to BSpline, etc.  

You can add new custom test harness commands to Draw in order to test 
or demonstrate a new functionality, which you are developing.

Currently DRAW Test Harness is a single executable called *DRAWEXE*.

Commands grouped in toolkits can be loaded at run-time thereby implementing dynamically loaded plug-ins. 
Thus you can work only with the commands that suit your needs adding 
the commands dynamically without leaving the Test Harness session.

Declaration of available plug-ins is done through special resource file(s). 
The *pload* command loads the plug-in in accordance with 
the specified resource file and activates the commands implemented in the plug-in.

The whole process of using the plug-in mechanism as well as the instructions for extending Test Harness is described in the @ref occt_user_guides__test_harness.

Draw Test Harness provides an environment for OCCT automated testing system. 
Check its @ref occt_contribution__tests "Automated Testing System" for details.

Remarks:

* The DRAWEXE executable is delivered with the installation procedure on Windows platform only.
* To start it, launch DRAWEXE executable from Open CASCADE Technology/Draw Test Harness item of the Start\\Programs menu.

Experimenting with Draw Test Harness
------------------------------------

 Running Draw
------------

**On Linux:**

* If OCCT was built by Code::Blocks  use <i>$CASROOT/draw.sh</i> file to launch *DRAWEXE* executable.

Draw[1]> prompt appears in the command window

Type *pload ALL*

**On Windows:**

Launch Draw executable from Open CASCADE Technology\\Test Harness\\Draw Test Harness 
item of the Start\\Programs menu or Use <i>$CASROOT\\draw.bat</i> file to launch *DRAWEXE* executable.

Draw[1]> prompt appears in the command window

Type pload ALL

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

1. Type *cd ../..* to return to the root directory
2. Type *cd samples/tcl* to reach the *DrawResources* directory
3. Type *source \<demo_file\>* to run the demonstration file provided with Open CASCADE. The following demonstration files are available:
  * DataExchangeDemo.tcl: demonstrates sample sequence of operations with writing and reading IGES file
  * ModelingDemo.tcl: demonstrates creation of simple shape and displaying it in HLR mode
  * VisualizationDemo.tcl: demonstrates use of 3d viewer
  * cad.tcl: creates solid shape looking like abbreviation "CAD"
  * bottle.tcl: creates bottle as in OCCT Tutorial
  * drill.tcl: creates twist drill bit shape
  * cutter.tcl: creates milling cutter shape
  * xde.tcl: demonstrates creation of simple assembly in XDE
  * materials.tcl: demonstrates visual properties of materials supported by 3d viewer
  * raytrace.tcl: demonstrates use of ray tracing display in 3d viewer
  * dimensions.tcl: demonstrates use of dimensions, clipping, and capping in 3d viewer

**Getting Help**

1. Type *help* to see all available commands
2. Type *help \<command_name\>* to find out the arguments for a given command