Tutorials and Samples {#samples}
=====================

Tutorial: Modelling a Bottle
----------------------------
The Qt programming tutorial teaches how to use Open CASCADE Technology services to model a 3D object. 
The purpose of the tutorial is not to explain all OCCT classes but 
to help start thinking in terms of the Open CASCADE Technology.

This tutorial assumes that  the user has experience in using and setting up C++. 
From the viewpoint of programming, Open CASCADE Technology is designed 
to enhance user's C++ tools with high performance modeling classes, methods and functions. 
The combination of these resources allows creating substantial applications.

Read more about @subpage occt__tutorial

MFC
---------

Visual C++ programming samples containing 10 Visual C++ projects 
illustrating how to use a particular module or functionality.

The list of MFC samples:

  * Geometry
  * Modeling
  * Viewer2d
  * Viewer3d
  * ImportExport
  * Ocaf
  * Triangulation
  * HLR
  * Animation
  * Convert

@figure{/samples/images/samples_mvc.png}

**Remarks:**

  * MFC samples are available only on Windows platform;
  * To start a sample use Open CASCADE Technology\\Samples\\Mfc\\ item of the Start\\Programs menu;
  * Read carefully readme.txt to learn about launching and compilation options.

See @subpage samples_mfc_standard "Readme" for details.

Qt
---

 OCCT includes several samples based on Qt application framework.
These samples are available on all supported desktop platforms.

To start a sample on Windows use Open CASCADE Technology\\Samples\\Qt\\ item of the Start\\Programs menu.

 Import Export
-------------

 Import Export programming sample contains 3D Viewer and Import / Export functionality.

@figure{/samples/images/samples_qt.png}

 Tutorial
---------

The Qt programming tutorial teaches how to use Open CASCADE Technology services to model a 3D object. 
The purpose of the tutorial is not to explain all OCCT classes but 
to help start thinking in terms of the Open CASCADE Technology.

This tutorial assumes that  the user has experience in using and setting up C++. 
From the viewpoint of programming, Open CASCADE Technology is designed 
to enhance user's C++ tools with high performance modeling classes, methods and functions. 
The combination of these resources allows creating substantial applications.

**See also:** @ref occt__tutorial "OCCT Tutorial"

Overview
---------
 
The Qt application providing samples for basic usage of C++ API of various OCCT functionality.

The samples are organized in several categories according to relevant module of OCCT:
 
 * Geometry
 * Topology,
 * Triangulation
 * DataExchange
 * OCAF
 * Viewer 2d
 * Viewer 3d
 
Each sample presents geometry view, C++ code fragment and sample output window.
 
@figure{/samples/images/sample_overview_qt.png}
 
See \subpage samples_qt_overview "Readme" for details.

C#
---

C# sample demonstrates integration of OCCT 3D Viewer and Import / Export functionality into .NET applications (using Windows Forms and WPF front ends).

@figure{/samples/images/samples_c__ie.png}

Import:

  * BRep
  * Iges
  * Step

Export: 

  * Brep
  * Iges
  * Step
  * Stl
  * Vrml

See @subpage samples_csharp_occt "C# sample Readme" for details.

There is also another C# example with the same functionality, which demonstrates the integration of Direct3D Viewer into .NET applications using WPF front end.

See @subpage samples_csharp_direct3d "Direct3D C# sample Readme" for details.

Android
---------

There are two samples are representing usage OCCT framework on Android mobile platform. They represent an OCCT-based 3D-viewer with CAD import support in formats BREP, STEP and IGES: jniviewer (java) and AndroidQt (qt+qml)

jniviewer
@figure{/samples/images/samples_java_android_occt.jpg}
Java -- See @subpage samples_java_android_occt "Android Java sample Readme" for details.

AndroidQt
@figure{/samples/images/samples_qml_android_occt.jpg}
Qt -- See \subpage samples_qml_android_occt "Android Qt sample Readme" for details.

iOS
---

There is a sample demonstrating usage of OCCT on iOS with Apple UIKit framework.

@figure{/samples/images/sample_ios_uikit.png}

See @subpage occt_samples_ios_uikit "iOS sample Readme" for details.

Web
---------

WebGL Viewer sample demonstrating usage of OCCT 3D Viewer in Web browser with Emscripten SDK can be found in `samples/webgl`.

@figure{/samples/images/sample_webgl.png}

See @subpage occt_samples_webgl "WebGL sample Readme" for details.

OCAF Usage Sample
------------------

The provided set of samples dedicates to get initial knowledge about typical actions with OCAF services.  It may be 
useful for newcomers.

Read more about @subpage samples__ocaf

OCAF Function Mechanism Usage
-----------------------------

This simple example dedicates to the usage of "Function Mechanism" of OCCT Application Framework. It represents a "nail" 
composed by a cone and two cylinders of different radius and height.
 
Read more about @subpage samples__ocaf_func

Draw Demo Scripts
------------------

A set of demo scripts demonsrates using OCCT functionality from DRAW. These scripts can be also considered as a 
tutorials on tcl usage within Draw.

Read more about @subpage samples__draw_scripts
