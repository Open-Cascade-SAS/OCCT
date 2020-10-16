Introduction {#mainpage}
========

@tableofcontents

@htmlonly<center>@endhtmlonly 
@figure{/resources/occt_logo.png}
@htmlonly</center>@endhtmlonly

Welcome to Open CASCADE Technology (OCCT), a software development platform 
providing services for 3D surface and solid modeling, CAD data exchange, and 
visualization. Most of OCCT functionality is available in the form of C++ 
libraries. OCCT can be best applied in development of software dealing with 3D 
modeling (CAD), manufacturing / measuring (CAM) or numerical simulation (CAE).

@htmlonly<center>@endhtmlonly
https://www.opencascade.com
@figure{/resources/occ_logo.png}
@htmlonly</center>@endhtmlonly

@section intro_overview Overview

Open CASCADE Technology (OCCT) is an object-oriented C++ class library designed for rapid production of sophisticated domain-specific CAD/CAM/CAE applications. 

A typical application developed using OCCT deals with two or three-dimensional (2D or 3D) geometric modeling
in general-purpose or specialized Computer Aided Design (CAD) systems, manufacturing
or analysis applications, simulation applications, or even illustration tools. 

OCCT library is designed to be truly modular and extensible, providing C++ classes for:
  * Basic data structures (geometric modeling, visualization, interactive selection and application specific services); 
  * Modeling algorithms; 
  * Working with mesh (faceted) data; 
  * Data interoperability with neutral formats (IGES, STEP); 

The C++ classes and other types are grouped into packages. Packages are organized into toolkits (libraries), to which you can link your application. Finally, toolkits are grouped into seven modules.

This modular structure is illustrated in the diagram below.

@figure{/introduction/images/technical_overview_schema.png}

* @ref intro_overview_fclasses  "Foundation Classes" module underlies all other OCCT classes; 
* @ref intro_overview_moddata   "Modeling Data" module supplies data structures to represent 2D and 3D geometric primitives and their compositions into CAD models; 
* @ref intro_overview_modalgo   "Modeling Algorithms" module contains a vast range of geometrical and topological algorithms;
  * @ref intro_overview_mesh    "Mesh" toolkit from "Modeling Algorithms" module implements tessellated representations of objects;
* @ref intro_overview_visu      "Visualization" module provides complex mechanisms for graphical data representation;
* @ref intro_overview_de        "Data Exchange" module inter-operates with popular data formats and relies on @ref intro_overview_heal "Shape Healing" to improve compatibility between CAD software of different vendors;
* @ref intro_overview_ocaf      "Application Framework" module offers ready-to-use solutions for handling application-specific data (user attributes) and commonly used functionality (save/restore, undo/redo, copy/paste, tracking CAD modifications, etc). 

In addition, @ref intro_overview_draw "Open CASCADE Test Harness", also called Draw, provides an entry point to the library and can be used as a testing tool for its modules.

@subsection intro_overview_fclasses Foundation Classes

**Foundation Classes** module contains data structures and services used by higher-level Open CASCADE Technology classes:

  * Primitive types, such as Boolean, Character, Integer or Real;
  * String classes that handle Unicode strings;
  * Collection classes that handle statically or dynamically sized aggregates of data, such as arrays, lists, queues, sets and hash tables (data maps).
  * Classes providing commonly used numerical algorithms and basic linear algebra calculations (addition, multiplication, transposition of vectors and matrices, solving linear systems etc).
  * Fundamental types like color, date and time information;
  * Primitive geometry types providing implementation of basic geometric and algebraic entities that define and manipulate elementary data structures. 
  * Exception classes that describe situations, when the normal execution of program is abandoned;

This module also provides a variety of general-purpose services, such as:
  * Safe handling of dynamically created objects, ensuring automatic deletion of unreferenced objects (smart pointers);
  * Standard and specialized memory allocators;
  * Extended run-time type information (RTTI) mechanism maintaining a full type hierarchy and providing means to iterate over it;
  * Encapsulation of C++ streams;
  * Basic interpreter of expressions facilitating the creation of customized scripting tools, generic definition of expressions, etc.;
  * Tools for dealing with configuration resource files and customizable message files facilitating multi-language support in applications;
  * Progress indication and user break interfaces, giving a possibility even for low-level algorithms to communicate with the user in a universal and convenient way;
  * and many others...

See the details in @ref occt_user_guides__foundation_classes "Foundation Classes User's Guide"

@subsection intro_overview_moddata Modeling Data

**Modeling Data** supplies data structures to implement boundary representation (BRep) of objects in 3D.
In BRep the shape is represented as an aggregation of geometry within topology.
The geometry is understood as a mathematical description of a shape, e.g. as curves and surfaces (simple or canonical, Bezier, NURBS, etc).
The topology is a data structure binding geometrical objects together.

Geometry types and utilities provide geometric data structures and services for:
 * Description of points, vectors, curves and surfaces:
   * their positioning in 3D space using axis or coordinate systems, and
   * their geometric transformation, by applying translations, rotations, symmetries, scaling transformations and combinations thereof.
 * Creation of parametric curves and surfaces by interpolation and approximation;
 * Algorithms of direct construction; 
 * Conversion of curves and surfaces to NURBS form;
 * Computation of point coordinates on 2D and 3D curves; 
 * Calculation of extrema between geometric objects. 
  
Topology defines relationships between simple geometric entities.
A shape, which is a basic topological entity, can be divided into components (sub-shapes):
 * Vertex -- a zero-dimensional shape corresponding to a point;
 * Edge -- a shape corresponding to a curve and bounded by a vertex at each extremity;
 * Wire -- a sequence of edges connected by their vertices;
 * Face -- a part of a plane (in 2D) or a surface (in 3D) bounded by wires;
 * Shell -- a collection of faces connected by edges of their wire boundaries;
 * Solid -- a finite closed part of 3D space bounded by shells;
 * Composite solid -- a collection of solids connected by faces of their shell boundaries;
 * Compound -- a collection of shapes of arbitrary type.

Complex shapes can be defined as assemblies (compounds) of simpler entities.

See the details in @ref occt_user_guides__modeling_data "Modeling Data User's Guide"

3D geometric models can be stored in OCCT native BREP format.
See @ref specification__brep_format "BREP Format Description White Paper" for details on the format.

@subsection intro_overview_modalgo Modeling Algorithms

**Modeling Algorithms** module groups a wide range of topological and geometric algorithms used in geometric modeling.
Basically, there are two groups of algorithms in Open CASCADE Technology:
 * High-level modeling routines used in the real design;
 * Low-level mathematical support functions used as a groundwork for the modeling API.

Low-level *geometric tools* provide the algorithms, which:
 * Calculate the intersection of two curves, surfaces, or a curve and a surface;
 * Project points onto 2D and 3D curves, points onto surfaces and 3D curves onto surfaces;
 * Construct lines and circles from constraints;
 * Construct free-form curves and surfaces from constraints (interpolation, approximation, skinning, gap filling, etc).
  
Low-level *topological tools* provide the algorithms, which:
 * Tessellate shapes;
 * Check correct definition of shapes;
 * Determine the local and global properties of shapes (derivatives, mass-inertia properties, etc);
 * Perform affine transformations;
 * Find planes in which edges are located;
 * Convert shapes to NURBS geometry;
 * Sew connected topologies (shells and wires) from separate topological elements (faces and edges).

Top-level API provides the following functionality: 
 * Construction of Primitives:
   * Boxes;
   * Prisms;
   * Cylinders;
   * Cones;
   * Spheres;
   * Toruses.
 * Kinematic Modeling:
   * Prisms -- linear sweeps;
   * Revolutions -- rotational sweeps;
   * Pipes -- general-form sweeps;
   * Lofting.

@figure{/introduction/images/0001.png "Shapes containing pipes with variable radius produced by sweeping"}

 * Boolean Operations, which allow creating new shapes from the combinations of source shapes. For two shapes *S1* and *S2*:
   * *Common* contains all points that are in *S1* and *S2*;
   * *Fuse* contains all points that are in *S1* or *S2*;
   * *Cut* contains all points in that are in *S1* and not in *S2*.

See @ref specification__boolean_operations "Boolean Operations" User's Guide for detailed documentation.

 * Algorithms for local modifications such as:
   * Hollowing;
   * Shelling;
   * Creation of tapered shapes using draft angles;
   * Algorithms to make fillets and chamfers on shape edges, including those with variable radius (chord).

 * Algorithms for creation of mechanical features, i.e. depressions, protrusions, ribs and grooves or slots along planar or revolution surfaces.

@figure{/introduction/images/0004.png}
 
See the details in @ref occt_user_guides__modeling_algos "Modeling Algorithms User's Guide".

@subsection intro_overview_mesh Mesh

**Mesh** toolkit provides the functionality to work with tessellated representations of objects in form of triangular facets. This toolkit contains:
- data structures to store surface mesh data associated to shapes and basic algorithms to handle them;
- data structures and algorithms to build triangular surface mesh from *BRep* objects (shapes);
- tools for displaying meshes with associated pre- and post-processor data (scalars or vectors).

Open CASCADE SAS also offers Advanced Mesh Products:
- <a href="https://www.opencascade.com/content/mesh-framework">Open CASCADE Mesh Framework (OMF)</a>
- <a href="https://www.opencascade.com/content/express-mesh">Express Mesh</a>

@figure{/introduction/images/0003.png}

@subsection intro_overview_visu Visualization

**Visualization** module provides ready-to-use algorithms to create graphic presentations from various objects: shapes, meshes, etc.

In Open CASCADE Technology visualization is based on the separation of CAD data and its graphical presentation.
The module also supports a fast and powerful interactive selection mechanism. 

Visualization module relies on the following key toolkits:
- *TKV3d* toolkit defines a high-level API called (Application Interactive Services* (AIS) for working with interactive objects.
- *TKService* toolkit defines a low-level API for managing and creating presentations from primitive arrays.
  This toolkit defines an abstraction layer for defining an arbitrary graphic driver responsible for actual rendering.
- *TKOpenGl* toolkit implements the graphic driver using OpenGL and OpenGL ES libraries.

While low-level API operates with primitive arrays (triangles, lines, points), the higher level includes services for building presentations for B-Rep shapes (shaded and wireframe).
A comprehensive list of standard interactive objects includes topological shape, mesh presentation, various dimensions, manipulators and others.
It provides a solid basis for rapid application development, while flexible and extensible API allows development of highly customized application-specific presentations.

Here are a few examples of OCCT Visualization features:
* Camera-driven view projection and orientation.
  Perspective, orthographic and stereographic projections are supported.
* Support of Common (diffuse/ambient/specular) and PBR metallic-roughness material models.
* Possibility to flexibly adjust appearance of dimensions in a 3D view.
  The 3D text object represents a given text string as a true 3D object in the model space.
* Definition of clipping planes through the plane equation coefficients.
  Ability to define visual attributes for cross-section at the level or individual clipping planes.
  In the image below different parts of the rocket are clipped with different planes and hatched.
@figure{/introduction/images/0008.png, "Display of shape cross-section and dimensions"}

* Support of built-in and application-specific GLSL shaders.
@figure{/introduction/images/0013.png, "Fragment shader implementing custom clipping surface"}

* Optimization of rendering performance through the algorithms of:
  * View frustum culling, which skips the presentation outside camera at the rendering stage;
  * Back face culling, which reduces the rendered number of triangles and eliminates artifacts at shape boundaries.
* Real-time ray tracing technique using recursive Whitted's algorithm and Bounded Volume Hierarchy effective optimization structure.
@figure{introduction/images/0002.png, "Real time visualization by ray tracing method"}
@figure{introduction/images/0012.png, "Simulation of a glass cover"}

For more details, see @ref occt_user_guides__visualization "Visualization User's Guide".

The visualization of OCCT topological shapes by means of VTK library provided by VIS component is described in a separate @ref occt_user_guides__vis "VTK Integration Services" User's Guide.

@subsection intro_overview_de Data Exchange

**Data Exchange** allows developing OCCT-based applications that can interact with other CAD systems by writing and reading CAD models to and from external data.

@figure{/introduction/images/0014.png,"Shape imported from STEP"}

**Data Exchange** is organized in a modular way as a set of interfaces that comply with various CAD formats: IGES, STEP, STL, VRML, etc.
The interfaces allow software based on OCCT to exchange data with various CAD/PDM software packages, maintaining a good level of interoperability.
This module handles various problems of interoperability between CAD systems, caused by differences in model validity criteria and requirements to internal representation.

* **Standardized Data Exchange** interfaces allow querying and examining the input file, converting its contents to a CAD model and running validity checks on a fully translated shape.
  The following formats are currently supported:
  * @ref occt_user_guides__step "STEP" (AP203: Mechanical Design, this covers General 3D CAD; AP214: Automotive Design; AP242).
  * @ref occt_iges_1 "IGES" (up to 5.3).
  * **glTF** 2.0 reader and writer.
  * **OBJ** mesh file reader.
  * **VRML** converter translates Open CASCADE shapes to VRML 1.0 files (Virtual Reality Modeling Language).
  * **STL** converter translates Open CASCADE shapes to STL files.
    STL (STtereoLithography) format is widely used for rapid prototyping (3D printing).
* @ref occt_user_guides__xde "Extended data exchange" (XDE) allows translating  additional attributes attached to geometric data (colors, layers, names, materials etc).
* <a href="https://www.opencascade.com/content/advanced-data-exchange-components">Advanced Data Exchange Components</a>
  are available in addition to standard Data Exchange interfaces to support interoperability and data adaptation (also using @ref intro_overview_heal "Shape Healing") with CAD software using the following proprietary formats:
	* <a href="https://www.opencascade.com/content/acis-sat-import-export">ACIS SAT</a>
	* <a href="https://www.opencascade.com/content/parasolid-import">Parasolid</a>
	* <a href="https://www.opencascade.com/content/dxf-import-export">DXF</a>
	* <a href="https://www.opencascade.com/content/ifc-import">IFC</a>
	* <a href="https://www.opencascade.com/content/jt-import-export">JT</a>

These components are based on the same architecture as interfaces with STEP and IGES.

@subsection intro_overview_heal Shape Healing

**Shape Healing** library provides algorithms to correct and adapt the geometry and topology of shapes imported to OCCT from other CAD systems. 

Shape Healing algorithms include, but are not limited to, the following operations:
 * Analyze shape characteristics and, in particular, identify the shapes that do not comply with OCCT geometry and topology validity rules by analyzing geometrical objects and topology:
   - check edge and wire consistency;
   - check edge order in a wire;
   - check the orientation of face boundaries;
   - analyze shape tolerances;
   - identify closed and open wires in a boundary.
 * Fix incorrect or incomplete shapes:
   - provide consistency between a 3D curve and its corresponding parametric curve;
   - repair defective wires;
   - fit the shapes to a user-defined tolerance value;
   - fill gaps between patches and edges.
 * Upgrade and change shape characteristics:
   - reduce curve and surface degree;
   - split shapes to obtain C1 continuity;
   - convert any types of curves or surfaces to Bezier or B-Spline curves or surfaces and back;
   - split closed surfaces and revolution surfaces.

Each sub-domain of Shape Healing has its own scope of functionality:

| Sub-domain | Description | Impact on the shape |
| :--- | :---- | :---- |
| Analysis | Explores shape properties, computes shape features, detects violation of OCCT requirements. | The shape itself is not modified. |
| Fixing  | Fixes the shape to meet the OCCT requirements. | The shape may change its original form: modification, removal or creation of sub-shapes, etc.) |
| Upgrade | Improves the shape to fit some particular algorithms. | The shape is replaced with a new one, but geometrically they are the same. |
| Customization | Modifies the shape representation to fit specific needs. | The shape is not modified, only the mathematical form of its internal representation is changed. |
| Processing | Mechanism of shape modification via a user-editable resource file. | |

For more details, refer to @ref occt_user_guides__shape_healing "Shape Healing User's guide".

@subsection intro_overview_ocaf Application Framework

**Open CASCADE Application Framework** (OCAF) handles Application Data basing on the Application/Document paradigm.
It uses an associativity engine to simplify the development of a CAD application thanks to the following ready-to-use features and services:

* Data attributes managing the application data, which can be organized according to the development needs; 
* Data storage and persistence (open/save); 
* Possibility to modify and recompute attributes in documents.
  With OCAF it is easy to represent the history of modification and parametric dependencies within your model;
* Possibility to manage multiple documents;  
* Predefined attributes common to CAD/CAM/CAE applications (e.g. to store dimensions);
* Undo-Redo and Copy-Paste functions.

Since OCAF handles the application structure, the only development task is the creation of application-specific data and GUIs. 

OCAF differs from any other CAD framework in the organization of application data, as there the data structures are based on reference keys rather than on shapes.
In a model, such attributes as shape data, color and material are attached to an invariant structure, which is deeper than the shapes.
A shape object becomes the value of *Shape* attribute, in the same way as an integer number is the value of *Integer* attribute and a string is the value of *Name* attribute.

OCAF organizes and embeds these attributes in a document. OCAF documents, in their turn, are managed by an OCAF application.

For more details, see @ref occt_user_guides__ocaf "OCAF User's Guide". 

@subsection intro_overview_draw Draw Test Harness

**Test Harness** or **Draw** is a convenient testing tool for OCCT libraries.
It can be used to test and prototype various algorithms before building an entire application.
It includes:
- A command interpreter based on the TCL language;
- A number of 2D and 3D viewers;
- A set of predefined commands.

The viewers support operations such as zoom, pan, rotation and full-screen views.

The basic commands provide general-purpose services such as:
- Getting help;
- Evaluating a script from a file;
- Capturing commands in a file;
- Managing views;
- Displaying objects.

In addition, **Test Harness** provides commands to create and manipulate curves and surfaces (geometry) and shapes, access visualization services, work with OCAF documents, perform data exchange, etc.

You can add custom commands to test or demonstrate any new functionalities, which you develop.

For more details, see @ref occt_user_guides__test_harness "Draw Test Harness Manual".

@section intro_req Requirements

Open CASCADE Technology is designed to be highly portable and is known to 
work on wide range of platforms. 
Current version is officially certified on Windows (IA-32 and x86-64), 
Linux (x86-64), OS X / macOS (x86-64), Android (armv7 and x86), and 
iOS (armv7, arm64) platforms. 

The tables below describe the recommended software configurations
for which OCCT is certified to work.

@subsection intro_req_cpp C++ Compiler / IDE

| OS        | Compiler |
| --------- | ----------- |
| Windows   | Microsoft Visual Studio: 2008 SP1, 2010 SP1, 2012 Update 4, 2013 Update 5, 2015 Update 3, 2017 <sup>1</sup>, 2019 <br>, LLVM (ClangCL), GCC 4.3+ (Mingw-w64)|
| Linux     | GNU gcc 4.3+ <br> LLVM CLang 3.6+ |
| OS X / macOS | XCode 6 or newer |
| Android   | NDK r10, GNU gcc 4.8 or newer |
| Web       | Emscripten SDK 1.39 or newer (CLang) |

1) VC++ 141 64-bit is used for regular testing and for building binary package of official release of OCCT on Windows.

@subsection intro_req_libs Third-party libraries and tools

The following third-party libraries and tools are not included in OCCT sources but are either required or can be optionally used for the indicated components of OCCT.
They are not needed if relevant component is not needed.

Note that pre-built packages of many of the listed libraries are available at
https://www.opencascade.com/content/3rd-party-components

| Component | Where to find | Used for | Required or optional |
| --------- | ------------- | -------- | -------------------- |
| CMake 2.8+ | https://cmake.org/ | Build from sources | Optional |
| Intel TBB 4.x or later | https://www.threadingbuildingblocks.org/ | All | Optional (advanced parallelization of algorithms) |
| OpenGL 3.3+, OpenGL ES 2.0+ | System | Visualization | Required |
| OpenVR 1.10+ | https://github.com/ValveSoftware/openvr | Visualization | Optional (VR support) |
| Direct3D 9 | Windows | Visualization | Optional (integration with GUI using Direct3D) |
| FreeType 2.4.11-2.7.1 | https://sourceforge.net/projects/freetype/files/ | Visualization | Required |
| FreeImage 3.17.0+ | https://sourceforge.net/projects/freeimage/files | Visualization | Optional (support of common 2D graphic formats) |
| FFmpeg 3.1+ | https://www.ffmpeg.org/download.html | Visualization | Optional (video recording) |
| VTK 6.1+ | https://www.vtk.org/download/ | Visualization | Optional (VTK integration) |
| Flex 2.6.4+ and Bison 3.7.1+ | https://sourceforge.net/projects/winflexbison/ | Data Exchange | Optional (update of STEP and ExprIntrp parsers) |
| RapidJSON 1.1+ | https://rapidjson.org/ | Data Exchange | Optional (reading glTF files) |
| Tcl/Tk 8.6.3+ <br> or ActiveTcl 8.6 | https://www.tcl.tk/software/tcltk/download.html <br> https://www.activestate.com/activetcl/downloads | DRAW Test Harness | Required |
| Qt Desktop: Qt 4.8.6+ <br> Android: Qt 5.3.2+ | https://www.qt.io/download/ | Samples and demos | Optional (Qt samples) |
| Doxygen 1.8.5+ | https://www.doxygen.nl/download.html | Documentation | Required |
| Graphviz 2.38+ | https://graphviz.org/ | Documentation | Optional (dependency graphs) |

@subsection intro_req_hw Hardware

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

@section intro_install Download and Installation

OCCT can be downloaded from https://www.opencascade.com/content/latest-release

In most cases you would want to rebuild OCCT from sources on your platform (OS, compiler) before
using it in your project, to ensure binary compatibility and appropriate configuration of the library.
See @ref build_upgrade for instructions on building OCCT from sources on supported platforms.

The following subsections describe how OCCT can be installed from ready-to-use packages on different platforms.

@subsection intro_install_windows Windows

On Windows Open CASCADE Technology with binaries precompiled by Visual C++ 2017 
can be installed using installation procedure available on official download page.

**Recommendation:**

If you have a previous version of OCCT installed on your station, 
and you do not plan to use it along with the new version, you might want to uninstall 
the previous version (using Control Panel, Add/Remove Programs) before 
the installation of this new version, to avoid possible problems 
(conflict of system variables, paths, etc).

Full OCCT installation with reference documentation requires 1.8 Gb on disk.

When the installation is complete, you will find the directories for 3rd party products 
(some might be absent in case of custom installation) and the main **OCCT** directory:

@figure{/introduction/images/overview_3rdparty.png}

The contents of the OCCT-7.4.0 directory (called further "OCCT root", or $CASROOT) are as follows:

@figure{/introduction/images/overview_installation.png, "The directory tree"}

  * **adm**   This folder contains administration files, which allow rebuilding OCCT;
  * **adm/cmake**  This folder contains files of CMake building procedure;
  * **adm/msvc**  This folder contains Visual Studio projects for Visual C++ 2010, 2012, 2013, 2015, 2017 and 2019 which allow rebuilding OCCT under Windows platform in 32 and 64-bit mode;
  * **adm/scripts** This folder contains auxiliary scripts for semi-automated building and packaging of OCCT for different platforms;
  * **data**  This folder contains CAD files in different formats, which can be used to test the OCCT functionality;
  * **doc**  This folder contains OCCT documentation in HTML and PDF format;
  * **dox**  This folder contains sources of OCCT documentation in plain text (MarkDown) format;
  * **inc**  This folder contains copies of all OCCT header files;
  * **samples**  This folder contains sample applications.
  * **src**  This folder contains OCCT source files. They are organized in folders, one per development unit;
  * **tests**  This folder contains scripts for OCCT testing.
  * **tools**  This folder contains sources of Inspector tool.
  * **win64/vc10**  This folder contains executable and library files built in optimize mode for Windows platform by Visual C++  2010;

@subsection intro_install_linux Linux

OCCT is available as package "opencascade" in official repositories of many Linux distributions.

See https://repology.org/project/opencascade/versions for overview of available repositories.

@subsection intro_install_mac macOS

On macOS, OCCT is available in Homebrew (https://formulae.brew.sh/formula/opencascade)
and MacPorts (https://ports.macports.org/port/opencascade/summary) repositories.

@section intro_env Environment Variables

To run any Open CASCADE Technology application you need to set the environment variables.

### On Windows

You can define the environment variables with env.bat script located in the 
$CASROOT folder. This script accepts two arguments to be used: 
the version of Visual Studio (vc10 -- vc142) and the architecture (win32 or win64).

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
  * **LD_LIBRARY_PATH** is required to define the path to OCCT libraries (on UNIX platforms only; **DYLD_LIBRARY_PATH** variable in case of macOS);
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

@section intro_license License

Open CASCADE Technology and all materials, including this documentation, is 
Copyright (c) 1999-2020 by OPEN CASCADE S.A.S. All rights reserved.

Open CASCADE Technology is free software; you can redistribute it and / or modify it under the terms of the 
@ref license_lgpl_21 "GNU Lesser General Public License (LGPL) version 2.1", with additional @ref occt_lgpl_exception "exception".

Note that LGPL imposes some obligations on the application linked with Open CASCADE Technology.
If you wish to use OCCT in a proprietary application, please pay a special attention to address the requirements of LGPL section 6.
At minimum the following should be considered:
1. Add the notice visible to the users of your application clearly stating that Open CASCADE Technology is used in this application, and that they have rights in this regard according to LGPL. 
   Such notice can be added in About dialog box (this is mandatory if this box contains copyright statements) or a similar place and/or in the documentation. 
   The text of LGPL license should be accessible to the user.
2. Make the copy of OCCT sources used by the application available to its users, and if necessary provide instructions on how to build it in a way compatible with the application.
3. Ensure that the user actually can exercise the right to run your application with a modified version of OCCT. 
   If the application is distributed in a form that does not allow the user to modify OCCT part (e.g. the application is linked to OCCT statically or is distributed via AppStore on iOS, GooglePlay on Android, Windows Store, etc.), 
   the application should be provided separately in a modifiable form, with all materials needed for the user to be able to run the application with a modified version of OCCT.

If you want to use Open CASCADE Technology without being bound by LGPL requirements, 
please <a href="https://www.opencascade.com/contact">contact Open CASCADE company</a> for a commercial license.

Note that Open CASCADE Technology is provided on an "AS IS" basis, WITHOUT 
WARRANTY OF ANY KIND. The entire risk related to any use of the OCCT code and 
materials is on you. See the @ref occt_public_license "license" text for formal 
disclaimer.

@section intro_acknowledge Acknowledgments

The following parties are acknowledged for producing tools which are used within 
Open CASCADE Technology libraries or for release preparation.

You are hereby informed that all rights to the software listed below belong to its respective 
authors and such software may not be freely available and/or be free of charge for any kind 
of use or purpose. We strongly recommend that you carefully read the license of these products 
and, in case you need any further information, directly contact their authors.

**Qt** is a cross-platform application framework that is widely used for developing application software 
with graphical user interface (GUI). Qt is free and open source software distributed under 
the terms of the GNU Lesser General Public License. In OCCT Qt is used for programming samples. 
If you need further information on Qt, refer to Qt Homepage (https://www.qt.io/)

**Tcl** is a high-level programming language. Tk is a graphical user interface (GUI) toolkit, 
with buttons, menus, listboxes, scrollbars, and so on. Taken together Tcl and Tk provide a solution 
to develop cross-platform graphical user interfaces with a native look and feel. Tcl/Tk is under copyright by 
Scriptics Corp., Sun Microsystems, and other companies. However, Tcl/Tk is an open source, and 
the copyright allows you to use, modify, and redistribute Tcl/Tk for any purpose, without an 
explicit license agreement and without paying any license fees or royalties. 
To use Tcl/Tk, refer to the Licensing Terms (https://www.tcl.tk/software/tcltk/license.html).

**FreeType 2** is developed by Antoine Leca, David Turner, Werner Lemberg and others. 
It is a software font engine that is designed to be small, efficient, highly customizable and 
portable while capable of producing high-quality output (glyph images). This product 
can be used in graphic libraries, display servers, font conversion tools, 
text image generation tools, and many other products.
FreeType 2 is released under two open-source licenses: BSD-like FreeType License and the GPL (https://www.freetype.org/license.html).

**Intel(R) Threading Building Blocks (TBB)** offers a rich and complete approach to expressing parallelism in a C++ program. 
It is a library that helps you to take advantage of multi-core processor performance without having to be a threading expert. 
Threading Building Blocks is not just a threads-replacement library. It represents a higher-level, task-based parallelism that 
abstracts platform details and threading mechanisms for scalability and performance. 
TBB version 2017 is available under Apache 2.0 license, while older versions 
until 4.4 are available under GPLv2 license with the runtime exception (https://www.threadingbuildingblocks.org).

**OpenGL** is an industry standard API for 3D graphics used by OCCT for 
implementation of 3D viewer. OpenGL specification is developed by the
Khronos group, https://www.khronos.org/opengl/. OCCT code includes header 
file *glext.h* obtained from Khronos web site.

**OpenVR** is an API and runtime that allows access to VR hardware from multiple vendors
without requiring that applications have specific knowledge of the hardware they are targeting.
OpenVR is optionally used by OCCT for VR support.
OpenVR is released under BSD-like license (https://github.com/ValveSoftware/openvr/blob/master/LICENSE).

**VTK** -- The **Visualization Toolkit (VTK)** is an open-source, freely available software system for 3D computer graphics, image processing and visualization.
OCCT VIS component provides adaptation functionality for visualization of OCCT topological shapes by means of VTK library.
If you need further information on VTK, refer to VTK Homepage https://www.vtk.org/.

**Doxygen** developed by Dimitri van Heesch is open source documentation system for 
C++, C, Java, Objective-C, Python, IDL, PHP and C#. This product is used in Open CASCADE Technology 
for automatic creation of Technical Documentation from C++ header files. 
If you need further information on Doxygen, refer to https://www.stack.nl/~dimitri/doxygen/index.html.

**Graphviz** is open source graph visualization software developed by John Ellson, Emden Gansner, Yifan Hu and Arif Bilgin. 
Graph visualization is representiation of structured information as diagrams of abstract graphs and networks. 
This product is used together with Doxygen in Open CASCADE Technology for automatic creation of Technical Documentation 
(generation of dependency graphs). Current versions of Graphviz are licensed on an open source 
basis under The Eclipse Public License (EPL) (https://www.graphviz.org/license/).

**Inno Setup** is a free script-driven installation system created in CodeGear Delphi by Jordan Russell. 
In OCCT Inno Setup is used to create Installation Wizard on Windows. 
It is licensed under Inno Setup License (http://www.jrsoftware.org/files/is/license.txt).

**FreeImage** is an Open Source library supporting popular graphics image formats, such as PNG, BMP, JPEG, TIFF, 
and others used by multimedia applications. This library is developed by Hervé Drolon and Floris van den Berg. 
FreeImage is easy to use, fast, multithreading safe, compatible with all 32-bit or 64-bit versions of Windows, 
and cross-platform (works both with Linux and Mac OS X). FreeImage is optionally used by OCCT to work
with images, on conditions of the FreeImage Public License (FIPL) (https://freeimage.sourceforge.net/freeimage-license.txt).

**FFmpeg** is an Open Source framework supporting various image, video and audio codecs.
FFmpeg is optionally used by OCCT for video recording, on LGPL conditions (https://www.ffmpeg.org/legal.html).

**David M. Gay's floating point routines** (dtoa.c) are used for fast reading of floating point values from text strings.
These routines are available under MIT-like license (see https://www.netlib.org/fp/).

**Flex** is a generator of lexical analyzers (scanners), available under BSD license (https://github.com/westes/flex).

GNU **Bison** is a parser generator used (together with **Flex**) for implementation of reader of STEP file format and parser of expressions.
It is available under GNU GPL v3 license (https://www.gnu.org/software/bison/).

**Delabella** is an open-source, cross-platform implementation of the Newton Apple Wrapper algorithm producing 2D Delaunay triangulation. 
Delabella is used by BRepMesh as one of alternative 2D triangulation algorithms. 
Delabella is licensed under the MIT license (https://github.com/msokalski/delabella).

**CMake** is an open-source, cross-platform family of tools designed to build, test and package software.
CMake is used to control the software compilation process using simple platform and compiler independent configuration files, and generate native makefiles and workspaces that can be used in the compiler environment of your choice. 
OCCT uses CMake as a build system. CMake is available under BSD 3-Clause license. 
See more at https://cmake.org/

**Cotire** (compile time reducer) is a CMake module that speeds up the build process of CMake based build systems 
by fully automating techniques as precompiled header usage and single compilation unit builds for C and C++.
Cotire is included in OCCT repository and used optionally by OCCT CMake scripts to accelerate builds by use of precompiled headers.
Cotire is licensed under the MIT license (https://github.com/sakra/cotire/blob/master/license).

**MikTEX** is up-to-date implementation of TeX/LaTeX and related programs for Windows. It is used 
for generation of User and Developer Guides in PDF format. See https://miktex.org for information
on this tool.

**RapidJSON** is an Open Source JSON parser and generator for C++.
RapidJSON is optionally used by OCCT for reading glTF files (https://rapidjson.org/).

**DejaVu** fonts are a font family based on the Vera Fonts under a permissive license (MIT-like, https://dejavu-fonts.github.io/License.html).
DejaVu Sans (basic Latin sub-set) is used by OCCT as fallback font when no system font is available.

Adobe Systems, Inc. provides **Adobe Reader**, which can be used to view files in Portable Document Format (PDF). 

**CAS.CADE** and **Open CASCADE** are registered trademarks of OPEN CASCADE S.A.S.

**Linux** is a registered trademark of Linus Torvalds.

**Windows** is a registered trademark of Microsoft Corporation in the United States and other countries.

**Mac**, **OS X**, **macOS**, and the Mac logo are trademarks of Apple Inc., registered in the U.S. and other countries.

**Android** is a trademark of Google LLC.
