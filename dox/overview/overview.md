Overview {#mainpage}
========

@tableofcontents

@htmlonly<center>@endhtmlonly 
@figure{/resources/occt_logo.png}
@htmlonly</center>@endhtmlonly

@section OCCT_OVW_SECTION_1 Welcome

Welcome to Open CASCADE Technology (OCCT), a software development platform 
providing services for 3D surface and solid modeling, CAD data exchange, and 
visualization. Most of OCCT functionality is available in the form of C++ 
libraries. OCCT can be best applied in development of software dealing with 3D 
modeling (CAD), manufacturing / measuring (CAM) or numerical simulation (CAE).

@section OCCT_OVW_SECTION_2 Copyrights

Open CASCADE Technology and all materials, including this documentation, is 
Copyright (c) 1999-2016 by OPEN CASCADE S.A.S. All rights reserved.

@htmlonly<center>@endhtmlonly
https://www.opencascade.com
@figure{/resources/occ_logo.png}
@htmlonly</center>@endhtmlonly

 License
--------

Open CASCADE Technology is free software; you can redistribute it and / or 
modify it under the terms of the 
@ref license_lgpl_21 "GNU Lesser General Public License (LGPL) version 2.1", 
with additional @ref occt_lgpl_exception "exception".

Alternatively, Open CASCADE Technology may be used under the terms of Open 
CASCADE commercial license or contractual agreement.

Note that Open CASCADE Technology is provided on an "AS IS" basis, WITHOUT 
WARRANTY OF ANY KIND. The entire risk related to any use of the OCCT code and 
materials is on you. See the @ref occt_public_license "license" text for formal 
disclaimer.

 Trademark information
----------------------

You are hereby informed that all software is a property of its respective authors and is protected by 
international and domestic laws on intellectual property and trademarks. 
Should you need further information, please directly contact the authors.

**CAS.CADE** and **Open CASCADE** are registered trademarks of 
OPEN CASCADE S.A.S.

**Linux** is a registered trademark of Linus Torvalds.

**Windows** is a registered trademark of Microsoft Corporation in the United States and other countries.

**Mac** and the Mac logo are trademarks of Apple Inc., registered in the U.S. and other countries.

 Acknowledgments
-----------------

The following parties are acknowledged for producing tools which are used within 
Open CASCADE Technology libraries or for release preparation.

You are hereby informed that all rights to the software listed below belong to its respective 
authors and such software may not be freely available and/or be free of charge for any kind 
of use or purpose. We strongly recommend that you carefully read the license of these products 
and, in case you need any further information, directly contact their authors.

**Qt** is a cross-platform application framework that is widely used for developing application software 
with graphical user interface (GUI). Qt is free and open source software distributed under 
the terms of the GNU Lesser General Public License. In OCCT Qt is used for programming samples. 
If you need further information on Qt, please, refer to Qt Homepage (https://www.qt.io/)

**Tcl** is a high-level programming language. Tk is a graphical user interface (GUI) toolkit, 
with buttons, menus, listboxes, scrollbars, and so on. Taken together Tcl and Tk provide a solution 
to develop cross-platform graphical user interfaces with a native look and feel. Tcl/Tk is under copyright by 
Scriptics Corp., Sun Microsystems, and other companies. However, Tcl/Tk is an open source, and 
the copyright allows you to use, modify, and redistribute Tcl/Tk for any purpose, without an 
explicit license agreement and without paying any license fees or royalties. 
To use Tcl/Tk, please refer to the Licensing Terms (https://www.tcl.tk/software/tcltk/license.html).

**GL2PS** is developed by Christophe Geuzaine and others. It is optionally used by OCCT to
export content of OpenGL scene to vector graphics formats (PS, PDF, EMF, SVG). 
The library is licensed under GL2PS LICENSE https://www.geuz.org/gl2ps/COPYING.GL2PS Version 2, November 2003.

**FreeType 2** is developed by Antoine Leca, David Turner, Werner Lemberg and others. 
It is a software font engine that is designed to be small, efficient, highly customizable and 
portable while capable of producing high-quality output (glyph images). This product 
can be used in graphic libraries, display servers, font conversion tools, 
text image generation tools, and many other products.

FreeType 2 is released under two open-source licenses: BSD-like FreeType License and the GPL.

**Intel(R) Threading Building Blocks (TBB)** offers a rich and complete approach to expressing parallelism in a C++ program. 
It is a library that helps you to take advantage of multi-core processor performance without having to be a threading expert. 
Threading Building Blocks is not just a threads-replacement library. It represents a higher-level, task-based parallelism that 
abstracts platform details and threading mechanisms for scalability and performance. 
TBB is available under GPLv2 license with the runtime exception.

**OpenGL** is an industry standard API for 3D graphics used by OCCT for 
implementation of 3D viewer. OpenGL specification is developed by the
Khronos group, https://www.khronos.org/opengl/. OCCT code includes header 
file *glext.h* obtained from Khronos web site.

**VTK** -- The **Visualization Toolkit (VTK)** is an open-source, freely available software system for 3D computer graphics, image processing and visualization. OCCT VIS component provides adaptation functionality for visualization of OCCT topological shapes by means of VTK library. If you need further information on VTK, please, refer to VTK Homepage https://www.vtk.org/.

**Doxygen** developed by Dimitri van Heesch is open source documentation system for 
C++, C, Java, Objective-C, Python, IDL, PHP and C#. This product is used in Open CASCADE Technology 
for automatic creation of Technical Documentation from C++ header files. 
If you need further information on Doxygen, please refer to https://www.stack.nl/~dimitri/doxygen/index.html.

**Graphviz** is open source graph visualization software developed by John Ellson, Emden Gansner, Yifan Hu and Arif Bilgin. 
Graph visualization is representiation of structured information as diagrams of abstract graphs and networks. 
This product is used together with Doxygen in Open CASCADE Technology for automatic creation of Technical Documentation 
(generation of dependency graphs). Current versions of Graphviz are licensed on an open source 
basis under The Eclipse Public License (EPL) (https://www.graphviz.org/License.php).

**Inno Setup** is a free script-driven installation system created in CodeGear Delphi by Jordan Russell. 
In OCCT Inno Setup is used to create Installation Wizard on Windows. 
It is licensed under Inno Setup License (https://www.jrsoftware.org/files/is/license.txt).

**FreeImage** is an Open Source library supporting popular graphics image formats, such as PNG, BMP, JPEG, TIFF, 
and others used by multimedia applications. This library is developed by Hervé Drolon and Floris van den Berg. 
FreeImage is easy to use, fast, multithreading safe, compatible with all 32-bit or 64-bit versions of Windows, 
and cross-platform (works both with Linux and Mac OS X). FreeImage is optionally used by OCCT to work
with images, on conditions of the FreeImage Public License (FIPL) (https://freeimage.sourceforge.net/freeimage-license.txt).

**CMake** is an open-source, cross-platform family of tools designed to build, test and package software. CMake is used to control the software compilation process using simple platform and compiler independent configuration files, and generate native makefiles and workspaces that can be used in the compiler environment of your choice. 
OCCT uses CMake as a build system. CMake is available under BSD 3-Clause license. See more at https://cmake.org/

**Cotire** (compile time reducer) is a CMake module that speeds up the build process of CMake based build systems 
by fully automating techniques as precompiled header usage and single compilation unit builds for C and C++.
Cotire is included in OCCT repository and used optionally by OCCT CMake scripts to accelerate builds by use of precompiled headers.
Cotire is licensed under the MIT license (https://github.com/sakra/cotire/blob/master/license).

**FFmpeg** is an Open Source framework supporting various image, video and audio codecs.
FFmpeg is optionally used by OCCT for video recording, on LGPL conditions (https://www.ffmpeg.org/legal.html).

**MikTEX** is up-to-date implementation of TeX/LaTeX and related programs for Windows. It is used 
for generation of User and Developer Guides in PDF format. See https://miktex.org for information
on this tool.

Adobe Systems, Inc. provides **Adobe Reader**, which can be used to view files in Portable Document Format (PDF). 

@section OCCT_OVW_SECTION_3 Documentation

OCCT documentation is provided in several forms:

- This overview provides general description of OCCT structure, functionality, modules, and features. 
  It is available in HTML format (generated by Doxygen) and includes User and Developer Guides.
  The sources of this documentation are contained in **dox** subdirectory of OCCT sources 
  (plain text format is used, with mixed MarkDown / Doxygen syntax mark-up).

- User and Developer Guides describing in details OCCT modules and development tools are also available in 
  Adobe Portable Document Format (PDF). To read this format, you need Adobe Acrobat Reader, 
  which is a freeware and can be downloaded from the Adobe site. 

- Full reference documentation covering all OCCT classes generated automatically by Doxygen 
  software is provided in HTML format. 
  Reference documentation is presented in **Modules --> Toolkits --> Packages --> Classes** 
  logic structure with cross-references to all OCCT classes and complete in-browser search by all classes.

See @ref occt_dev_guides__documentation "OCCT Documentation Guide" for details on OCCT documentation system.

**Generation of HTML documentation**

To generate HTML documentation from sources contained in *dox* subdirectory, 
you need to have Tcl and Doxygen 1.8.5 (or above) installed on your system.

Use script **gendoc** (batch file on Windows, shell script on Linux / Mac OSX) to generate documentation.

To generate Overview documentation:

    cmd> gendoc -overview

To generate Reference manual:

    cmd> gendoc -refman

Run this command without arguments to get help on supported options.

@section OCCT_OVW_SECTION_5 Requirements

Open CASCADE Technology is designed to be highly portable and is known to 
work on wide range of platforms. 
Current version is officially certified on Windows (IA-32 and x86-64), 
Linux (x86-64), OS X / macOS (x86-64), Android (armv7 and x86), and 
iOS (armv7, arm64) platforms. 

The tables below describe the recommended software configurations
for which OCCT is certified to work.

@subsection overview_req_cpp C++ Compiler / IDE

| OS        | Compiler |
| --------- | ----------- |
| Windows   | Microsoft Visual Studio: 2010 SP1<sup>1</sup>, 2012 Update 4, 2013 Update 5, 2015 <br> Intel C++ Composer XE 2013 SP1 <br> GCC 4.3+ (Mingw-w64)|
| Linux     | GNU gcc 4.3+ <br> LLVM CLang 3.6+ |
| OS X      | XCode 6 or newer |
| Android   | NDK r10, GNU gcc 4.8 or newer |

1) VC++ 10 64-bit is used for regular testing and for building 
  binary package of official release of OCCT on Windows.

@subsection overview_req_libs Third-party libraries

| Component | Requirement |
| --------- | ----------- |
| Graphic library | OpenGL 3.3+, OpenGL ES 2.0+ <br> Direct3D 9 |
| Qt (for samples and demos) | Desktop: Qt 4.8.6+ http://www.qt.io/download/ <br> Android: Qt 5.3.2+ http://www.qt.io/download/ |
| TCL (for testing tools)    | Tcl/Tk 8.6.3+ http://www.tcl.tk/software/tcltk/download.html <br> or ActiveTcl 8.6 http://www.activestate.com/activetcl/downloads (for Windows)| 
| Freetype (for text rendering) | FreeType 2.4.11-2.5.5 http://sourceforge.net/projects/freetype/files/ |
| FreeImage (optional, for support of common 2D graphic formats) | FreeImage 3.17.0+ http://sourceforge.net/projects/freeimage/files |
| FFmpeg (optional, for video recording) | FFmpeg 3.1+ https://www.ffmpeg.org |
| gl2ps (optional, for export contents of OCCT viewer to vector formats) | gl2ps-1.3.8+  http://geuz.org/gl2ps/ |
| Intel TBB (optional, for multithreaded algorithms) | TBB 4.x or 5.x http://www.threadingbuildingblocks.org/ |
| VTK (for VTK Integration Services | VTK 6.1+ http://www.vtk.org/VTK/resources/software.html |
| Doxygen (optional for building documentation) | Doxygen 1.8.5+ http://www.stack.nl/~dimitri/doxygen/download.html |

@subsection overview_req_hw Hardware

| Component | Requirement |
| --------- | ----------- |
| Minimum memory    | 512 MB, 1 GB recommended |
| Free disk space (complete installation) | 600 MB approx. |

On desktop, 3D viewer for optimal performance requires graphics processing unit (GPU) supporting OpenGL 3.3 or above. 
Ray tracing requires OpenGL 4.0+ or OpenGL 3.3+ with *GL_ARB_texture_buffer_object_rgb32* extension.
Textures within ray tracing will be available only when *GL_ARB_bindless_texture extension* is provided by driver.

On mobile platforms, OpenGL ES 2.0+ is required for 3D viewer (OpenGL ES 3.1+ is recommended).
The ray tracing is not yet available on mobile platforms.
Some old hardware might be unable to execute complex GLSL programs (e.g. with high number of light sources, clipping planes).

OCCT 3D Viewer, in general, supports wide range of graphics hardware - from very old to new.
Therefore, if you observe some unexpected visual issues - first check for OpenGL driver update (or firmware update in case of mobile platforms);
but beware that driver update might also come with new bugs.
Don't forget to report these bugs to vendors.

@section OCCT_OVW_SECTION_4 Installation

In most cases you need to rebuild OCCT on your platform (OS, compiler) before
using it in your project, to ensure binary compatibility.
See @ref occt_dev_guides__building for instructions on
building OCCT from sources on supported platforms.

@subsection OCCT_OVW_SECTION_4_1 Using Windows installer

On Windows Open CASCADE Technology can be installed with binaries precompiled by 
Visual C++ 2010 with installation procedure.

**Recommendation:**

If you have a previous version of OCCT installed on your station, 
and you do not plan to use it along with the new version, you might want to uninstall 
the previous version (using Control Panel, Add/Remove Programs) before 
the installation of this new version, to avoid possible problems 
(conflict of system variables, paths, etc).

**Attention:** For full installation OCCT requires approximately 650 Mb of disk space, 
but during the installation process you will need 1,2 Gb of free disk space.

OCCT installation with reference documentation requires 1,4 Gb on disk.

  * Download the OCCT installer from OPEN CASCADE web site using the link. you have been provided
  * Launch the installer and follow the instructions.

The includes and binaries of third-party libraries necessary for building and launching 
OCCT are included into binary distribution (built with Visual C++ 2010). 
When the installation is complete, you will find the directories for 3rd party products 
(some might be absent in case of custom installation) and the main **OCCT** directory:

@figure{/overview/images/overview_3rdparty.png}

The contents of the OCCT-7.0.0 directory (called further "OCCT root", or $CASROOT) are as follows:

@figure{/overview/images/overview_installation.png, "The directory tree"}

  * **adm**   This folder contains administration files, which allow rebuilding OCCT;
  * **adm/cmake**  This folder contains files of CMake building procedure;
  * **adm/msvc**  This folder contains Visual Studio projects for Visual C++  2005, 2008, 2010, 2012 and 2013 which allow rebuilding OCCT under Windows platform in 32 and 64-bit mode;
  * **data**  This folder contains CAD files in different formats, which can be used to test the OCCT functionality;
  * **doc**  This folder contains OCCT documentation in HTML and PDF format;
  * **dox**  This folder contains sources of OCCT documentation in plain text (MarkDown) format;
  * **inc**  This folder contains copies of all OCCT header files;
  * **samples**  This folder contains sample applications.
  * **src**  This folder contains OCCT source files. They are organized in folders, one per development unit;
  * **tests**  This folder contains scripts for OCCT testing.
  * **win64/vc10**  This folder contains executable and library files built in optimize mode for Windows platform by Visual C++  2010;

@section OCCT_OVW_SECTION_4_2 Environment Variables

To run any Open CASCADE Technology application you need to set the environment variables.

### On Windows

You can define the environment variables with env.bat script located in the 
$CASROOT folder. This script accepts two arguments to be used: 
the version of Visual Studio (vc8 -- vc12) and the architecture (win32 or win64).

The additional environment settings necessary for compiling OCCT libraries and samples 
by Microsoft Visual Studio can be set using script custom.bat located in the same folder. 
You might need to edit this script to correct the paths to third-party libraries 
if they are installed on your system in a non-default location.

Script msvc.bat can be used with the same arguments for immediate launch of Visual Studio for (re)compiling OCCT.

### On Unix


  If OCCT was built by Code::Blocks, you can define the environment variables with env_cbp.sh or custom_cbp.sh script.

  If OCCT was built by Automake, you can define the environment variables with env_amk.sh or custom_amk.sh script.

The scripts are located in the OCCT root folder.

 
### Description of system variables:

  * **CASROOT** is used to define the root directory of Open CASCADE Technology;
  * **PATH** is required to define the path to OCCT binaries and 3rdparty folder;
  * **LD_LIBRARY_PATH** is required to define the path to OCCT libraries (on UNIX platforms only);
  * **MMGT_OPT** (optional) if set to 1, the memory manager performs optimizations as described below; if set to 2, 
    Intel (R) TBB optimized memory manager is used; if 0 (default), every memory block is allocated 
    in C memory heap directly (via malloc() and free() functions). 
    In the latter case, all other options starting with *MMGT*, except MMGT_CLEAR, are ignored;
  * **MMGT_CLEAR** (optional) if set to 1 (default), every allocated memory block is cleared by zeros; 
    if set to 0, memory block is returned as it is;
  * **MMGT_CELLSIZE** (optional) defines the maximal size of blocks allocated in large pools of memory. Default is 200;
  * **MMGT_NBPAGES** (optional) defines the size of memory chunks allocated for small blocks in pages 
    (operating-system dependent). Default is 10000;
  * **MMGT_THRESHOLD** (optional) defines the maximal size of blocks that are recycled internally 
    instead of being returned to the heap. Default is 40000;
  * **MMGT_MMAP** (optional) when set to 1 (default), large memory blocks are allocated using 
    memory mapping functions of the operating system; if set to 0, 
    they will be allocated in the C heap by malloc();
  * **CSF_LANGUAGE** (optional) defines default language of messages;
  * **CSF_DEBUG** (optional, Windows only): if defined then a diagnostic message is displayed in case of an exception;
  * **CSF_DEBUG_BOP** (optional): if defined then it should specify directory where diagnostic data on problems occured in Boolean operations will be saved;
  * **CSF_MDTVTexturesDirectory** defines the directory for available textures when using texture mapping;
  * **CSF_ShadersDirectory** (optional) defines the directory for GLSL programs for Ray Tracing renderer (embedded resources are used when variable is undefined);
  * **CSF_SHMessage** (optional) defines the path to the messages file for *ShapeHealing*;
  * **CSF_XSMessage** (optional) defines the path to the messages file for **STEP** and **IGES** translators;
  * **CSF_StandardDefaults**, **CSF_StandardLiteDefaults*, **CSF_XCAFDefaults**, and **CSF_PluginDefaults** define paths to directory where configuration files for OCAF persistence are located (required for open/save operations with OCAF documents);
  * **CSF_IGESDefaults** and **CSF_STEPDefaults** (optional) define paths to directory where resource files of **IGES** and **STEP** translators are located;
  * **CSF_XmlOcafResource** is required in order to set the path to **XSD** resources, which defines XML grammar.
  * **CSF_MIGRATION_TYPES** is required in order to read documents that contain old data types, such as *TDataStd_Shape*;

@section OCCT_OVW_SECTION_7 Getting Started

@subsection OCCT_OVW_SECTION_7_1 Draw Test Harness

Draw is a command interpreter based on TCL and a graphical system used for testing and demonstrating OCCT modeling libraries.

Draw can be used interactively to create, display and modify objects such as curves, surfaces and topological shapes.

@figure{/overview/images/overview_draw.png}

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
Please, consult its @ref occt_dev_guides__tests "Automated Testing System" for details.

Remarks:

* The DRAWEXE executable is delivered with the installation procedure on Windows platform only.
* To start it, launch DRAWEXE executable from Open CASCADE Technology/Draw Test Harness item of the Start\\Programs menu.

@subsection OCCT_OVW_SECTION_7_2 Experimenting with Draw Test Harness

 Running Draw
------------

**On Linux:**

1. If OCCT was built by Code::Blocks  use <i>$CASROOT/draw_cbp.sh</i> file to launch *DRAWEXE* executable;
2. If OCCT was built by Automake    use <i>$CASROOT/draw_amk.sh</i> file to launch *DRAWEXE* executable;

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

@subsection OCCT_OVW_SECTION_7_3 Programming Samples

@subsubsection OCCT_OVW_SECTION_7_3_1 MFC 

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

@figure{/overview/images/overview_mvc.png}

**Remarks:**

  * MFC samples are available only on Windows platform;
  * To start a sample use Open CASCADE Technology\\Samples\\Mfc\\ item of the Start\\Programs menu;
  * Read carefully readme.txt to learn about launching and compilation options.

See \subpage samples_mfc_standard "Readme" for details.

@subsubsection OCCT_OVW_SECTION_7_3_2 Qt

OCCT contains three samples based on Qt application framework

 Import Export
-------------

 Import Export programming sample contains 3D Viewer and Import / Export functionality.

@figure{/overview/images/overview_qt.png}

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

**Remarks:**

  * Qt samples are available on all supported platforms;
  * To start a sample on Windows use Open CASCADE Technology\\Samples\\Qt\\ item of the Start\\Programs menu.

@subsubsection OCCT_OVW_SECTION_7_3_3 C#

C# sample demonstrates integration of OCCT 3D Viewer and Import / Export functionality into .NET applications (using Windows Forms and WPF front ends).

@figure{/overview/images/overview_c__ie.png}

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

See \subpage samples_csharp_occt "Readme" for details.

There is also another C# example with the same functionality, which demonstrates the integration of Direct3D Viewer into .NET applications using WPF front end.

See \subpage samples_csharp_direct3d "Readme" for details.

@subsubsection OCCT_OVW_SECTION_7_3_4 Android

There are two samples are representing usage OCCT framework on Android mobile platform. They represent an OCCT-based 3D-viewer with CAD import support in formats BREP, STEP and IGES: jniviewer (java) and AndroidQt (qt+qml)

jniviewer
@figure{/overview/images/samples_java_android_occt.jpg}
Java -- See \subpage samples_java_android_occt "Readme" for details.

AndroidQt
@figure{/overview/images/samples_qml_android_occt.jpg}
Qt -- See \subpage samples_qml_android_occt "Readme" for details.
