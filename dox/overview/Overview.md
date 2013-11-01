Overview {#mainpage}
========

@section OCCT_OVW_SECTION_1 Welcome

Welcome to Open CASCADE Technology version 6.7.0, a minor release, 
which introduces a number of new features and improved traditional 
functionality along with some changes over the previous release 6.6.0.

This release makes Open CASCADE Technology even a more powerful and stable 
development platform for 3D modeling and numerical simulation applications.

Open CASCADE Technology 6.7.0 is a full-featured package that allows developing 
applications on Windows and Linux platforms.

@htmlonly<center>@endhtmlonly http://www.opencascade.org

@image html /overview/images/overview_occttransparent.png
@image latex /overview/images/overview_occttransparent.png

Copyright (c) 2001-2013 OPEN CASCADE S.A.S.

@image html /resources/occt_logo.png
@image latex /resources/occt_logo.png

@htmlonly</center>@endhtmlonly

@section OCCT_OVW_SECTION_2 Copyrights

Copyright(c) 2001-2013 by OPEN CASCADE S.A.S. All rights reserved.

 Trademark information
----------------------

You are hereby informed that all software is a property of its respective authors and is protected by 
international and domestic laws on intellectual property and trademarks. 
Should you need further information, please directly contact the authors.

CAS.CADE and Open CASCADE are registered trademarks of OPEN CASCADE S.A.S.

 Acknowledgement
------------------

The following parties are acknowledged for producing tools which are used within 
Open CASCADE Technology libraries or for release preparation.

You are hereby informed that all rights to the software listed below belong to its respective 
authors and such software may not be freely available and/or be free of charge for any kind 
of use or purpose. We strongly recommend that you carefully read the license of these products 
and, in case you need any further information, directly contact their authors.

**Qt** is a cross-platform application framework that is widely used for developing application software 
with graphical user interface (GUI). Qt is free and open source software distributed under 
the terms of the GNU Lesser General Public License. In OCCT Qt is used for programming samples. 
If you need further information on Qt, please, refer to Qt Homepage (qt.digia.com).

**Tcl** is a high-level programming language. Tk is a graphical user interface (GUI) toolkit, 
with buttons, menus, listboxes, scrollbars, and so on. Taken together Tcl and Tk provide a solution 
to develop cross-platform graphical user interfaces with a native look and feel. Tcl/Tk is under copyright by 
Scriptics Corp., Sun Microsystems, and other companies. However, Tcl/Tk is an open source, and 
the copyright allows you to use, modify, and redistribute Tcl/Tk for any purpose, without an 
explicit license agreement and without paying any license fees or royalties. 
To use Tcl/Tk, please refer to the Licensing Terms (http://www.tcl.tk/software/tcltk/license.html).

**Robert Boehne** has developed **GNU Autoconf**, **Automake** and **Libtool** scripts and makefiles 
for the Open CASCADE project http://sourceforge.net/projects/autoopencas/, 
which became an initial groundwork for the build scripts based on respective GNU tools 
(autoconf, automake and libtool) in Open CASCADE Technology version 4.0. 
These scripts are now maintained by the OPEN CASCADE company.

**GL2PS** is developed by Christophe Geuzaine and others. It is OpenGL to PostScript printing library. 
The library is licensed under GL2PS LICENSE http://www.geuz.org/gl2ps/COPYING.GL2PS Version 2, November 2003.

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

Open CASCADE Technology WOK module on Windows also makes use of LGPL-licensed C routines   * regexp and getopt, taken from GNU C library.

**Doxygen** (Copyright (c) 1997-2010 by Dimitri van Heesch) is open source documentation system for 
C++, C, Java, Objective-C, Python, IDL, PHP and C#. This product is used in Open CASCADE Technology 
for automatic creation of Technical Documentation from C++ header files. 
If you need further information on Doxygen, please refer to http://www.stack.nl/~dimitri/doxygen/index.html.

**Graphviz** is open source graph visualization software developed by John Ellson, Emden Gansner, Yifan Hu and Arif Bilgin. 
Graph visualization is representiation of structured information as diagrams of abstract graphs and networks. 
This product is used together with Doxygen in Open CASCADE Technology for automatic creation of Technical Documentation 
(generation of dependency graphs). Current versions of Graphviz are licensed on an open source 
basis only under The Eclipse Public License (EPL) (http://www.graphviz.org/License.php).

**Inno Setup** is a free script-driven installation system created in CodeGear Delphi by Jordan Russell. 
In OCCT Inno Setup is used to create Installation Wizard on Windows. 
It is licensed under Inno Setup License (http://www.jrsoftware.org/files/is/license.txt).

**FreeImage** is an Open Source library supporting popular graphics image formats, such as PNG, BMP, JPEG, TIFF 
and others used by multimedia applications. This library is developed by Hervé Drolon and Floris van den Berg. 
FreeImage is easy to use, fast, multithreading safe, compatible with all 32-bit or 64-bit versions of Windows, 
and cross-platform (works both with Linux and Mac OS X). FreeImage is licensed under the 
GNU General Public License, version 2.0 (GPLv2) and 
the FreeImage Public License (FIPL) (http://freeimage.sourceforge.net/freeimage-license.txt).

Adobe Systems, Inc. provides **Adobe Acrobat Professional**, which is a software to view, create, manipulate, 
print and manage files in Portable Document Format (PDF). 
This product is used in OCCT for the development and update of User's Guides.

The same developer provides **Robohelp HTML** that allows developing online Help for applications that are run on the Web and on Intranets. 
**Robohelp HTML X5.0.2** is used in OCCT for the development and update of OCCT Overview.

**Linux** is a registered trademark of Linus Torvalds.

**Windows** is a registered trademark of Microsoft Corporation in the United States and other countries.

**Mac** and the Mac logo are trademarks of Apple Inc., registered in the U.S. and other countries.


@section OCCT_OVW_SECTION_3 Introduction


This document is just an introduction to Open CASCADE Technology (OCCT) dealing with 
compatibility and installation issues and providing a general description of OCCT modules 
and other features. All modules and development tools are described in User's Guides, available in 
Adobe Portable Document Format (PDF). To read this format, you need Adobe Acrobat Reader, 
which is a freeware and can be downloaded from the Adobe site. 
All user guides can be accessed directly from this help.

Alongside with PDF User Guides, OCCT suggests its users full reference documentation on all 
implementation classes automatically generated by Doxygen software. 
This Doxygen generated documentation is supplied  in the form of a separate package, 
in a usual html file format.

Reference documentation is presented in **Modules --> Toolkits --> Packages --> Classes** 
logic structure with cross-references to all OCCT classes and complete in-browser search by all classes.

**Recommendation for generation of reference documentation**

Reference documentation can be generated by OCCT binary WOK package that 
is available for downloading from www.opencascade.org and dev.opencascade.org sites.

Prerequisites:

  * Doxygen version 1.7.4 or higher
  * Graphviz version 2.28.0 or higher

Run WOK (cd \<WOK_INSTALL_DIR\>/site folder):

* Using WOK TCL shell:
      > wok_tclsh.sh

* Using Emacs editor:
      > wok_emacs.sh

In the WOK prompt, step into your workbench:

      >wokcd <your workbench>

In your workbench, use **wgendoc** command with –h argument to get information about arguments of **wgendoc** command:

      >wgendoc -h

then run **wgendoc** command with required arguments

e.g., wgendoc –output=d:/occt/doc {–m=Draw Visualization} -chm

@section OCCT_OVW_SECTION_5 Requirements

@subsection OCCT_OVW_SECTION_5_1 Linux Intel
<table>
<tr> <th>Operating System  </th> <th> 64-bit:  Mandriva 2010, CentOS 5. 5, CentOS 6.3, Fedora 17, Fedora 18, Ubuntu-1304, Debian 6.0 *  </th> </tr>
<tr> <td> Minimum memory </td> <td> 512 Mb, 1 Gb recommended </td > </tr> 
<tr> <td> Free disk space (complete installation)  </td> <td> For full installation Open CASCADE Technology requires 600 Mb of disk space.  </td > </tr> 
<tr> <td>Minimum swap space  </td> <td> 500 Mb </td > </tr> 
<tr> <td> Video card   </td> <td> **GeForce** The following versions of GeForce drivers are recommended: 64-bit Version: 100.14.19 or later http://www.nvidia.com/object/linux_display_amd64_100.14.19.html 32-bit Version: 100.14.19 or later http://www.nvidia.com/object/linux_display_ia32_100.14.19.html  </td > </tr> 
<tr> <td> Graphic library     </td> <td> OpenGL 1.1+ (OpenGL 1.5+ is recommended) </td > </tr> 
<tr> <td>C++      </td> <td>GNU gcc 4.0.  - 4.7.3.  </td > </tr> 
<tr> <td> TCL (for testing tools)    </td> <td> Tcltk 8.5 or 8.6 http://www.tcl.tk/software/tcltk/8.6.html</td > </tr> 
<tr> <td> Qt (for demonstration tools)  </td> <td> Qt 4.6.2 http://qt.nokia.com/downloads </td > </tr> 
<tr> <td> Freetype (OCCT Text rendering) </td> <td> freetype-2.4.11 http://sourceforge.net/projects/freetype/files/ </td > </tr> 
<tr> <td> FreeImage (Support of common graphic formats) </td> <td>FreeImage 3.15.4 http://sourceforge.net/projects/freeimage/files/Source%20Distribution/     </td > </tr> 
<tr> <td> gl2ps (Export contents of OCCT viewer to vector graphic file)      </td> <td> gl2ps-1.3.8  http://geuz.org/gl2ps/ </td > </tr> 
<tr> <td>TBB (optional tool for parallelized version of BRepMesh component)  </td> <td> TBB 3.x or 4.x http://www.threadingbuildingblocks.org/    </td > </tr> 
<tr> <td> OpenCL (optional for providing ray tracing visualization core </td> <td>http://developer.amd.com/tools-and-sdks/heterogeneous-computing/amd-accelerated-parallel-processing-app-sdk/downloads/          </td>  </tr> 
</table>
\* Debian 60 64 bit is a permanently tested platform. 

@subsection OCCT_OVW_SECTION_5_2 Windows Intel

<table>
<tr> <th>Operating System  </th> <th> 32/64-bit: 8/ 7 SP1 / VISTA SP2 /XP SP3    </th> </tr>
<tr> <td> Minimum memory </td> <td> 512 Mb, 1 Gb recommended </td > </tr> 
<tr> <td> Free disk space (complete installation)  </td> <td> For full installation Open CASCADE Technology requires 650 Mb of disk space but during the process of installation you will need 1,2 Gb of free disk space.  </td > </tr> 
<tr> <td>Minimum swap space  </td> <td> 500 Mb </td > </tr> 
<tr> <td> Video card   </td> <td> **GeForce** Version 266.58 WHQL or later is recommended: http://www.nvidia.com/Download/index.aspx  
 </td > </tr> 
<tr> <td> Graphic library     </td> <td> OpenGL 1.1+ (OpenGL 1.5+ is recommended) </td > </tr> 
<tr> <td>C++      </td> <td>Microsoft Visual Studio .NET 2005 SP1 with all security updates
Microsoft Visual Studio .NET 2008 SP1*
Microsoft Visual Studio .NET 2010
Microsoft Visual Studio .NET 2012
Microsoft Visual Studio .NET 2013
  </td > </tr> 
<tr> <td> TCL (for testing tools)    </td> <td> TActiveTcl 8.5 or 8.6
http://www.activestate.com/activetcl/downloads  </td > </tr> 
<tr> <td> Qt (for demonstration tools)  </td> <td> Qt 4.6.2 http://qt.digia.com/downloads </td > </tr> 
<tr> <td> Freetype (OCCT Text rendering) </td> <td> freetype-2.4.11 http://sourceforge.net/projects/freetype/files/ </td > </tr> 
<tr> <td> FreeImage (Support of common graphic formats )</td> <td>FreeImage 3.15.4 http://sourceforge.net/projects/freeimage/files/Source%20Distribution/     </td > </tr> 
<tr> <td> gl2ps (Export contents of OCCT viewer to vector graphic file)      </td> <td> gl2ps-1.3.8  http://geuz.org/gl2ps/ </td > </tr> 
<tr> <td>TBB (optional tool for parallelized version of BRepMesh component)  </td> <td> TBB 3.x or 4.x http://www.threadingbuildingblocks.org/    </td > </tr> 
<tr> <td> OpenCL (optional for providing ray tracing visualization core </td> <td>http://developer.amd.com/tools-and-sdks/heterogeneous-computing/amd-accelerated-parallel-processing-app-sdk/downloads/          </td>  </tr> 
</table>

* The official release of OCCT for Windows contains libraries built with VC++ 2008. 


@subsection OCCT_OVW_SECTION_5_3 MAC OS X

<table>
<tr> <th>Operating System  </th> <th> Mac OS X 10.6.8 Snow Leopard / 10.7 Lion    </th> </tr>
<tr> <td> Minimum memory </td> <td> 512 Mb, 1 Gb recommended </td > </tr> 
<tr> <td> Free disk space (complete installation)  </td> <td> For full installation Open CASCADE Technology requires 600 Mb of disk space. </td > </tr> 
<tr> <td>Minimum swap space  </td> <td> 500 Mb </td > </tr> 
<tr> <td> Video card   </td> <td> **GeForce** Version 266.58 WHQL or later is recommended: http://www.nvidia.com/Download/index.aspx  
 </td > </tr> 
<tr> <td> Graphic library     </td> <td> OpenGL 1.1+ (OpenGL 1.5+ is recommended) </td > </tr> 
<tr> <td>C++      </td> <td>XCode 3.2 or newer (4.x is recommended)  </td > </tr> 
<tr> <td> Qt (for demonstration tools)  </td> <td> Qt 4.6.2 http://qt.digia.com/downloads </td > </tr> 
<tr> <td> Freetype (OCCT Text rendering) </td> <td> freetype-2.4.11 http://sourceforge.net/projects/freetype/files/ </td > </tr> 
<tr> <td> FreeImage (Support of common graphic formats )</td> <td>FreeImage 3.15.4 http://sourceforge.net/projects/freeimage/files/Source%20Distribution/     </td > </tr> 
<tr> <td> gl2ps (Export contents of OCCT viewer to vector graphic file)      </td> <td> gl2ps-1.3.8  http://geuz.org/gl2ps/ </td > </tr> 
<tr> <td>TBB (optional tool for parallelized version of BRepMesh component)  </td> <td> TBB 3.x or 4.x http://www.threadingbuildingblocks.org/    </td > </tr> 
<tr> <td> OpenCL (optional for providing ray tracing visualization core </td> <td> OpenCL 1.2.8 native </td>  </tr> 
</table>

@section OCCT_OVW_SECTION_4 Installation

Open CASCADE Technology can be installed with binaries precompiled by 
Visual C++ 2008 using Installation Procedure under Windows platform only

The source package and the building tools are available for self-dependent 
preparation binary files on Unix and Windows platforms.

@subsection OCCT_OVW_SECTION_4_1 Windows

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

### Third-party tools


The includes and binaries of third-party libraries necessary for building and launching 
OCCT are included into binary distribution (built with Visual C++ 2008). 
To recompile OCCT libraries with other Visual C++ versions, 
it is necessary to install headers and libraries of these third-party products.

The recommended way to do this is to download each of the third-party tools from its web site 
and build it using the relevant tools. For additional convenience of the users, 
OPEN CASCADE also provides the documents with recommendations on building 
third-party products from source files.



When the installation is complete, you will find the following directories 
(some might be absent in case of custom installation):

@image html /overview/images/overview_installation.png "The directory tree"
@image latex /overview/images/overview_installation.png "The directory tree"



  * **adm**   This folder contains administration files, which allow rebuilding OCCT;
  * **adm/cmake**  This folder contains files of CMake building procedure;
  * **adm/msvc**  This folder contains Visual Studio projects for Visual C++  2005, 2008 and 2010, which allow rebuilding OCCT under Windows platform in 32 and 64-bit mode;
  * **data**  This folder contains CAD files in different formats, which can be used to test the OCCT functionalities;
  * **doc**  This folder contains OCCT Overview documentation;
  * **drv**  This folder contains source files generated by WOK (private header files and instantiations of generic classes);
  * **inc**  This folder contains all OCCT header files;
  * **samples**  This folder contains sample applications.
  * **src**  This folder contains OCCT source files. They are organized in folders, one per development unit;
  * **tests**  This folder contains scripts for OCCT testing.
  * **win32/vc9**  This folder contains executable and library files built in optimize mode for Windows platform by Visual C++  2008;

3rd party products have been moved to the root of Open CASCADE installation.

@image html /overview/images/overview_3rdparty.png "The third-party products"
@image latex /overview/images/overview_3rdparty.png "The third-party products"


@subsection OCCT_OVW_SECTION_4_2 System Environment Variables

To run any Open CASCADE Technology application you need to set the environment variables.

### On Windows


You can define the environment variables with env.bat script located in the 
OpenCACADE<version_number>/ros folder. This script accepts two arguments to be used: 
the version of Visual Studio (vc8, vc9, or vc10) and the architecture (win32 or win64).

The additional environment settings necessary for compiling OCCT libraries and samples 
by Microsoft Visual Studio can be set using script custom.bat located in the same folder. 
You might need to edit this script to correct the paths to third-party libraries 
if they are installed on your system in a non-default location.

Script msvc.bat can be used with the same arguments for immediate launch of Visual Studio for (re)compiling OCCT.

### On Unix


  If OCCT was built by Code::Blocks, you can define the environment variables with env_cbp.sh or custom_cbp.sh script.

  If OCCT was built by Automake, you can define the environment variables with env_amk.sh or custom_amk.sh script.

The scripts are located in the OpenCACADE<version_number>/ros folder of the source package.

 
### Description of system variables:


  * **CASROOT** is used to define the root directory of Open CASCADE Technology;
  * **PATH** is required to define the path to OCCT binaries and 3rdparty folder;
  * **LD_LIBRARY_PATH** is required to define the path to OCCT libraries (on UNIX platforms only);
  * **MMGT_OPT** if set to 1, the memory manager performs optimizations as described below; if set to 2, 
  Intel (R) TBB optimized memory manager is used; if 0 (default), every memory block is allocated 
  in C memory heap directly (via malloc() and free() functions). 
  In the latter case, all other options except MMGT_CLEAR  and MMGT_REENTRANT are ignored;
  * **MMGT_CLEAR** if set to 1 (default), every allocated memory block is cleared by zeros; 
  if set to 0, memory block is returned as it is;
  * **MMGT_CELLSIZE** defines the maximal size of blocks allocated in large pools of memory. Default is 200;
  * **MMGT_NBPAGES** defines the size of memory chunks allocated for small blocks in pages 
  (operating-system dependent). Default is 10000;
  * **MMGT_THRESHOLD** defines the maximal size of blocks that are recycled internally 
  instead of being returned to the heap. Default is 40000;
  * **MMGT_MMAP** when set to 1 (default), large memory blocks are allocated using 
  memory mapping functions of the operating system; if set to 0, 
  they will be allocated in the C heap by malloc();
  * **MMGT_REENTRANT** when set to 1 (default), all calls to the 
  optimized memory manager will be secured against possible simultaneous access from different execution threads. 

This variable should be set in any multithreaded application that uses 
an optimized memory manager (MMGT_OPT=1) and has more than one thread 
potentially calling OCCT functions. If set to 0, OCCT memory management and 
exception handling routines will skip the code protecting from possible concurrency 
in multi-threaded environment. This can yield some performance gain in some applications, 
but can lead to unpredictable results if used in a multithreaded application;

**Special note:** for applications that use OCCT memory manager from more than one thread, 
on multiprocessor hardware, it is recommended to use options MMGT_OPT=2 and MMGT_REENTRANT=1.

  * **CSF_LANGUAGE** is required to define the default language of messages;
  * **CSF_EXCEPTION_PROMPT** – if defined and set to 1 then a diagnostic message is displayed in case of an exception;
  * **CSF_MDTVFontDirectory** accesses the fonts that can be used in OCCT;
  * **CSF_MDTVTexturesDirectory** defines the directory for available textures when using texture mapping;
  * **CSF_UnitsDefinition** and **CSF_UnitsLexicon** are required by programs considering units;
  * **CSF_SHMessage** is required in order to define the path to the messages file for *ShapeHealing*;
  * **CSF_XSMessage** is required in order to define the path to the messages file for **STEP** and **IGES** translators;
  * **CSF_StandardDefaults** and **CSF_PluginDefaults** are required in order to maintain CASCADE Persistence mechanism to make possible   any open/save operations with OCAF documents;
  * **CSF_StandardLiteDefaults** is required in order to maintain *OCCT Persistence mechanism* to make possible any open/save operations with Lite OCAF documents;
  * **CSF_XCAFDefaults**  any open/save operations for **XDE** documents;
  * **CSF_GraphicShr** is required to define the path to the *TKOpenGl* library;
  * **CSF_IGESDefaults** and **CSF_STEPDefaults** are required for **IGES** and **STEP** translators correspondingly in order to define the path to the resource files;
  * **CSF_XmlOcafResource** is required in order to set the path to **XSD** resources, which defines XML grammar.

As part of XML persistence support, these definitions can be used by end users 
in XML validators or editors, together with persistent XmlOcaf documents;

* **CSF_MIGRATION_TYPES** is required in order to read documents that contain old data types, such as *TDataStd_Shape*;
* **TCLLIBPATH**, **TCL_LIBRARY**, **TK_LIBRARY** and **TIX_LIBRARY** are required to allow work with **DRAW** and **WOK**.

@section OCCT_OVW_SECTION_6 Release Notes


Open CASCADE Technology latest version 
@htmlonly 
<a href="http://occtrel.nnov.opencascade.com/OpenCASCADE6.7.0/doc/release_notes.pdf">Release Notes</a>
@endhtmlonly  (PDF)


@section OCCT_OVW_SECTION_7 Getting Started


@subsection OCCT_OVW_SECTION_7_1 Draw Test Harness

Draw is a command interpreter based on TCL and a graphical system used for testing and demonstrating OCCT modeling libraries.

Draw can be used interactively to create, display and modify objects such as curves, surfaces and topological shapes.

@image html /overview/images/overview_draw.png
@image latex /overview/images/overview_draw.png

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

Currently DRAW Test Harness is a single executable called DRAWEXE.

Commands grouped in toolkits can be loaded at run-time thereby implementing dynamically loaded plug-ins. 
Thus you can work only with the commands that suit your needs adding 
the commands dynamically without leaving the Test Harness session.

Declaration of available plug-ins is done through special resource file(s). 
The pload command loads the plug-in in accordance with 
the specified resource file and activates the commands implemented in the plug-in.

The whole process of using the plug-in mechanism as well as the instructions for extending Test Harness is described in the 
@htmlonly 
<a href="http://occtrel.nnov.opencascade.com/OpenCASCADE6.6.0/doc/OCCT_Tests.pdf">User's Guide/</a>
@endhtmlonly

Draw Test Harness provides an environment for OCCT automated testing system. Please, consult its 
@htmlonly 
<a href="http://occtrel.nnov.opencascade.com/OpenCASCADE6.6.0/doc/OCCT_Tests.pdf">User's Guide /</a>
@endhtmlonly
for details.

Remarks:

* The DRAWEXE executable is delivered with the installation procedure on Windows platform only.
* To start it, launch DRAWEXE executable from Open CASCADE Technology//Draw Test Harness item of the Start\\Programs menu.

@subsection OCCT_OVW_SECTION_7_2 Experimenting with Draw Test Harness

 Running Draw
------------

**On Linux:**

1. If OCCT was built by Code::Blocks   * use $CASROOT/draw_cbp.sh file to launch DRAWEXE executable;
2. If OCCT was built by Automake    * use $CASROOT/draw_amk.sh file to launch DRAWEXE executable;

Draw[1]> prompt appears in the command window

Type pload ALL

**On Windows:**

Launch Draw executable from Open CASCADE Technology\\Test Harness\\Draw Test Harness 
item of the Start\\Programs menu or Use $CASROOT\\draw.bat file to launch DRAWEXE executable.

Draw[1]> prompt appears in the command window

Type pload ALL

**Creating your first geometric objects**

1. In the command window, type axo to create an axonometric view
2. Type box b -10 -10 -10 20 20 20 to create a cube b of size 20, 
   parallel to the X Y Z axis and centered on the origin. 
   The cube will be displayed in the axonometric view in wireframe mode
3. Type fit to fill the viewer with the cube
4. Type pcylinder c 2 30 to create a cylinder c of radius 2 and height 30.
   The cylinder will be displayed in addition to the cube

**Manipulating the view**

1. Type clear to erase the view
2. Type donly c to display the cylinder only
3. Type donly b to display the cube only
4. Type hlr hlr b to display the cube in the hidden line removal mode

**Running demonstration files**

1. Type cd ..//.. to return to the root directory
2. Type cd src//DrawResources to reach the DrawResources directory
3. Type source "Available Demo File" to run the demonstration provided with Open CASCADE
4. The following demonstration files are available:
  * DataExchangeDemo.tcl
  * ModelingDemo.tcl
  * OCAFDemo.tcl
  * VisualizationDemo.tcl

**Getting Help**

1. Type help to see all available commands
2. Type help command-name to find out the arguments for a given command

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

@image html /overview/images/overview_mvc.png
@image latex /overview/images/overview_mvc.png

**Remarks:**

  * MFC samples are available only on Windows platform;
  * To start a sample use Open CASCADE Technology\\Samples\\Mfc\\ item of the Start\\Programs menu;
  * Read carefully readme.txt to learn about launching and compilation options.

@subsubsection OCCT_OVW_SECTION_7_3_2 Qt

OCCT contains three samples based on Qt application framework

 Import Export
-------------

 Import Export programming sample contains 3D Viewer and Import // Export functionality.

@image html /overview/images/overview_qt.png
@image latex /overview/images/overview_qt.png

 Tutorial
---------

The Qt programming tutorial teaches how to use Open CASCADE Technology services to model a 3D object. 
The purpose of the tutorial is not to explain all OCCT classes but 
to help start thinking in terms of the Open CASCADE Technology.

This tutorial assumes that  the user has experience in using and setting up C++. 
From the viewpoint of programming, Open CASCADE Technology is designed 
to enhance user's C++ tools with high performance modeling classes, methods and functions. 
The combination of these resources allows creating substantial applications.

**See also:** @subpage overview__tutorial "3D Object Tutorial" 

 Voxel
------

This is a demonstration application showing OCCT voxel models. 
It also includes a set of non-regression tests and other commands 
for testing this functionality (accessible only through TEST pre-processor definition).

**See also:** 
 @htmlonly 
<a href="http://occtrel.nnov.opencascade.com/OpenCASCADE6.6.0/doc/voxels_wp.pdf">Voxels User's guide (PDF)</a>
@endhtmlonly

**Remarks:**

  * Qt samples are available on all supported platforms;
  * To start a sample on Windows use Open CASCADE Technology\\Samples\\Qt\\ item of the Start\\Programs menu.

@subsubsection OCCT_OVW_SECTION_7_3_3 C#

C# sample containing 3D Viewer and Import // Export functionality.

@image html /overview/images/overview_c__ie.png
@image latex /overview/images/overview_c__ie.png

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

**Remarks:**

  * C# sample is available only on Windows platform;
  * It is delivered in source code only and must be built with Microsoft Visual C++ 2005.
  
  
  
  
