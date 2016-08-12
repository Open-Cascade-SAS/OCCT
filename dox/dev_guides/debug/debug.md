Debugging tools and hints {#occt_dev_guides__debug}
=========================

@tableofcontents

@section occt_debug_intro Introduction

This manual describes facilities included in OCCT to support debugging, and provides some hints for more efficient debug.

@section occt_debug_macro Compiler macro to enable extended debug messages

Many OCCT algorithms can produce extended debug messages, usually printed to cout.
These include messages on internal errors and special cases encountered, timing etc.
In OCCT versions prior to 6.8.0 most of these messages were activated by compiler macro *DEB*, enabled by default in debug builds.
Since version 6.8.0 this is disabled by default but can be enabled by defining compiler macro *OCCT_DEBUG*.

To enable this macro on Windows when building with Visual Studio projects, edit file custom.bat and add the line:

    set CSF_DEFINES=OCCT_DEBUG

Some algorithms use specific macros for yet more verbose messages, usually started with OCCT_DEBUG_.
These messages can be enabled in the same way, by defining corresponding macro.

Note that some header files are modified when *OCCT_DEBUG* is enabled, hence binaries built with it enabled are not compatible with client code built without this option; this is not intended for production use.

@section occt_debug_exceptions Calling JIT debugger on exception

On Windows platform when using Visual Studio compiler there is a possibility to start the debugger automatically if an exception is caught in a program running OCCT. For this, set environment variable *CSF_DEBUG* to any value. Note that this feature works only if you enable OCCT exception handler in your application by calling *OSD::SetSignal()*.

@section occt_debug_bop Self-diagnostics in Boolean operations algorithm

In real-world applications modeling operations are often performed in a long sequence, while the user sees only the final result of the whole sequence. If the final result is wrong, the first debug step is to identify the offending operation to be debugged further. Boolean operation algorithm in OCCT provides a self-diagnostic feature which can help to do that step.

This feature can be activated by defining environment variable *CSF_DEBUG_BOP*, which should specify an existing writeable directory.

The diagnostic code checks validity of the input arguments and the result of each Boolean operation. When an invalid situation is detected, the report consisting of argument shapes and a DRAW script to reproduce the problematic operation is saved to the directory pointed by *CSF_DEBUG_BOP*.

Note that this feature does not applicable for UWP build.

@section occt_debug_call Functions for calling from debugger

Modern interactive debuggers provide the possibility to execute application code at a program break point. This feature can be used to analyse the temporary objects available only in the context of the debugged code. OCCT provides several global functions that can be used in this way.

Note that all these functions accept pointer to variable as <i>void*</i> to allow calling the function even when debugger does not recognize type equivalence or can not perform necessary type cast automatically. It is responsibility of the developer to provide the correct pointer. In general these functions are not guaranteed to work, thus use them with caution and at your own risk.

@subsection occt_debug_call_draw Interacting with DRAW

Open CASCADE Test Harness or @ref occt_user_guides__test_harness "DRAW" provides an extensive set of tools for inspection and analysis of OCCT shapes and geometric objects and is mostly used as environment for prototyping and debugging OCCT-based algorithms.

In some cases the objects to be inspected are available in DRAW as results of DRAW commands. In other cases, however, it is necessary to inspect intermediate objects created by the debugged algorithm. To support this, DRAW provides a set of commands allowing the developer to store intermediate objects directly from the debugger stopped at some point during the program execution (usually at a breakpoint).

~~~~~
const char* Draw_Eval (const char *theCommandStr)
~~~~~

Evaluates a DRAW command or script.
A command is passed as a string parameter.

~~~~~
const char* DBRep_Set (const char* theNameStr, void* theShapePtr)
~~~~~

Sets the specified shape as a value of DRAW interpreter variable with the given name.
- *theNameStr* -- the DRAW interpreter variable name to set.
- *theShapePtr* -- a pointer to *TopoDS_Shape* variable.

~~~~~
const char* DBRep_SetComp (const char* theNameStr, void* theListPtr)
~~~~~

Makes a compound from the specified list of shapes and sets it as a value of DRAW interpreter variable with the given name.
- *theNameStr* -- the DRAW interpreter variable name to set.
- *theListPtr* -- a pointer to *TopTools_ListOfShape* variable.

~~~~~
const char* DrawTrSurf_Set (const char* theNameStr, void* theHandlePtr)
const char* DrawTrSurf_SetPnt (const char* theNameStr, void* thePntPtr)
const char* DrawTrSurf_SetPnt2d (const char* theNameStr, void* thePnt2dPtr)
~~~~~

Sets the specified geometric object as a value of DRAW interpreter variable with the given name.
- *theNameStr* -- the DRAW interpreter variable name to set.
- *theHandlePtr* -- a pointer to the geometric variable (Handle to *Geom_Geometry* or *Geom2d_Curve* or descendant) to be set.
- *thePntPtr* -- a pointer to the variable of type *gp_Pnt* to be set.
- *thePnt2dPtr* -- a pointer to the variable of type *gp_Pnt2d* to be set.

All these functions are defined in *TKDraw* toolkit and return a string indicating the result of execution.

@subsection occt_debug_call_brep Saving and dumping shapes and geometric objects

The following functions are provided by *TKBRep* toolkit and can be used from debugger prompt:

~~~~~
const char* BRepTools_Write (const char* theFileNameStr, void* theShapePtr)
~~~~~

Saves the specified shape to a file with the given name.
- *theFileNameStr* -- the name of the file where the shape is saved.
- *theShapePtr* -- a pointer to *TopoDS_Shape* variable.

~~~~~
const char* BRepTools_Dump (void* theShapePtr)
const char* BRepTools_DumpLoc (void* theShapePtr)
~~~~~

Dumps shape or its location to cout.
- *theShapePtr* -- a pointer to *TopoDS_Shape* variable.

The following function is provided by *TKMesh* toolkit:

~~~~~
const char* BRepMesh_Dump (void* theMeshHandlePtr, const char* theFileNameStr)
~~~~~

Stores mesh produced in parametric space to BREP file.
- *theMeshHandlePtr* -- a pointer to *Handle(BRepMesh_DataStructureOfDelaun)* variable.
- *theFileNameStr* -- the name of the file where the mesh is stored.

The following functions are provided by *TKTopTest* toolkit:

~~~~~
const char* MeshTest_DrawLinks(const char* theNameStr, void* theFaceAttr)
const char* MeshTest_DrawTriangles(const char* theNameStr, void* theFaceAttr)
~~~~~

Sets the edges or triangles from mesh data structure of type *Handle(BRepMesh_FaceAttribute)* as DRAW interpreter variables, assigning a unique name in the form "<theNameStr>_<index>" to each object.
- *theNameStr* -- the prefix to use in names of objects.
- *theFaceAttr* -- a pointer to *Handle(BRepMesh_FaceAttribute)* variable.

The following additional function is provided by *TKGeomBase* toolkit:

~~~~~
const char* GeomTools_Dump (void* theHandlePtr)
~~~~~

Dump geometric object to cout.
- *theHandlePtr* -- a pointer to the geometric variable (<i>Handle</i> to *Geom_Geometry* or *Geom2d_Curve* or descendant) to be set.

@section occt_debug_vstudio Using Visual Studio debugger 

@subsection occt_debug_vstudio_command Command window 

Visual Studio debugger provides the Command Window (can be activated from menu <b>View / Other Windows / Command Window</b>), which can be used to evaluate variables and expressions interactively in a debug session (see http://msdn.microsoft.com/en-us/library/c785s0kz.aspx). Note that the Immediate Window can also be used but it has some limitations, e.g. does not support aliases.

When the execution is interrupted by a breakpoint, you can use this window to call the above described functions in context of the currently debugged function. Note that in most cases you will need to specify explicitly context of the function by indicating the name of the DLL where it is defined.

For example, assume that you are debugging a function, where local variable *TopoDS_Edge* *anEdge1* is of interest.
The following set of commands in the Command window will save this edge to file *edge1.brep*, then put it to DRAW variable *e1* and show it maximized in the axonometric DRAW view:

~~~~~
>? ({,,TKBRep.dll}BRepTools_Write)("d:/edge1.brep",(void*)&anEdge1)
0x04a2f234 "d:/edge1.brep"
>? ({,,TKDraw.dll}DBRep_Set)("e1",(void*)&anEdge1)
0x0369eba8 "e1"
>? ({,,TKDraw.dll}Draw_Eval)("donly e1; axo; fit")
0x029a48f0 ""
~~~~~

For convenience it is possible to define aliases to commands in this window, for instance (here ">" is prompt provided by the command window; in the Immediate window this symbol should be entered manually):

~~~~~
>alias deval      ? ({,,TKDraw}Draw_Eval)
>alias dsetshape  ? ({,,TKDraw}DBRep_Set)
>alias dsetcomp   ? ({,,TKDraw}DBRep_SetComp)
>alias dsetgeom   ? ({,,TKDraw}DrawTrSurf_Set)
>alias dsetpnt    ? ({,,TKDraw}DrawTrSurf_SetPnt)
>alias dsetpnt2d  ? ({,,TKDraw}DrawTrSurf_SetPnt2d)
>alias saveshape  ? ({,,TKBRep}BRepTools_Write)
>alias dumpshape  ? ({,,TKBRep}BRepTools_Dump)
>alias dumploc    ? ({,,TKBRep}BRepTools_DumpLoc)
>alias dumpmesh   ? ({,,TKMesh}BRepMesh_Dump)
>alias dumpgeom   ? ({,,TKGeomBase}GeomTools_Dump)
~~~~~ 

Note that aliases are stored in the Visual Studio user's preferences and it is sufficient to define them once on a workstation. With these aliases, the above example can be reproduced easier (note the space symbol after alias name!):

~~~~~
>saveshape ("d:/edge1.brep",(void*)&anEdge1)
0x04a2f234 "d:/edge1.brep"
>dsetshape ("e1",(void*)&anEdge1)
0x0369eba8 "e1"
>deval ("donly e1; axo; fit")
0x029a48f0 ""
~~~~~

Note that there is no guarantee that the call will succeed and will not affect the program execution, thus use this feature at your own risk. In particular, the commands interacting with window system (such as *axo*, *vinit*, etc.) are known to cause application crash when the program is built in 64-bit mode. To avoid this, it is recommended to prepare all necessary view windows in advance, and arrange these windows to avoid overlapping with the Visual Studio window, to ensure that they are visible during debug. 

@subsection occt_debug_vstudio_watch Customized display of variables content

Visual Studio provides a way to customize display of variables of different types in debugger windows (Watch, Autos, Locals, etc.).

In Visual Studio 2005-2010 the rules for this display are defined in file *autoexp.dat* located in  subfolder *Common7\\Packages\\Debugger* of the Visual Studio installation folder (hint: the path to that folder is given in the corresponding environment variable, e.g. *VS100COMNTOOLS* for vc10). This file contains two sections: *AutoExpand* and *Visualizer*. The following rules can be added to these sections to provide more convenient display of some OCCT data types. 

### \[AutoExpand\] section 

~~~~~
; Open CASCADE classes
Standard_Transient=<,t> count=<count,d>
Handle_Standard_Transient=<entity,x> count=<entity->count,d> <,t>
TCollection_AsciiString=<mylength,d> <mystring,s>
TCollection_HAsciiString=<myString.mylength,d> <myString.mystring,s>
TCollection_ExtendedString=<mylength,d> <mystring,su>
TCollection_HExtendedString=<myString.mylength,d> <myString.mystring,su>
TCollection_BaseSequence=size=<Size,d> curr=<CurrentIndex,d>
TCollection_BasicMap=size=<mySize,d>
NCollection_BaseSequence=size=<mySize,d> curr=<myCurrentIndex,d>
NCollection_BaseList=length=<myLength,d>
NCollection_BaseMap=size=<mySize,d> buckets=<myNbBuckets>
NCollection_BaseVector=length=<myLength,d>
TDF_Label=<myLabelNode,x> tag=<myLabelNode->myTag>
TDF_LabelNode=tag=<myTag,d>
TDocStd_Document=format=<myStorageFormat.mystring,su> count=<count,d> <,t>
TopoDS_Shape=<myTShape.entity,x> <myOrient>
gp_XYZ=<x,g>, <y,g>, <z,g>
gp_Pnt=<coord.x,g>, <coord.y,g>, <coord.z,g>
gp_Vec=<coord.x,g>, <coord.y,g>, <coord.z,g>
gp_Dir=<coord.x,g>, <coord.y,g>, <coord.z,g>
gp_XY=<x,g>, <y,g>
gp_Pnt2d=<coord.x,g>, <coord.y,g>
gp_Dir2d=<coord.x,g>, <coord.y,g>
gp_Vec2d=<coord.x,g>, <coord.y,g>
gp_Mat2d={<matrix[0][0],g>,<matrix[0][1],g>}, {<matrix[1][0],g>,<matrix[1][1],g>}
gp_Ax1=loc={<loc.coord.x,g>, <loc.coord.y,g>, <loc.coord.z,g>} vdir={<vdir.coord.x,g>, <vdir.coord.y,g>, <vdir.coord.z,g>}
~~~~~ 

### \[Visualizer\] section

~~~~~
; Open CASCADE classes

NCollection_Handle<*> {
  preview ( *((($T0::Ptr*)$e.entity)->myPtr) )
  children ( (($T0::Ptr*)$e.entity)->myPtr )
}

NCollection_List<*> {
  preview ( #( "NCollection_List [", $e.myLength, "]" )	)
  children ( #list( head: $c.myFirst, next: myNext ) : #(*($T1*)(&$e+1)) )
}

NCollection_Array1<*> {
  preview ( #( "NCollection_Array1 [", $e.myLowerBound, "..", $e.myUpperBound, "]" ) )
  children ( #array( expr: $c.myData[$i], size: 1+$c.myUpperBound ) )
}

math_Vector {
  preview ( #( "math_Vector [", $e.LowerIndex, "..", $e.UpperIndex, "]" ) )
  children ( #array ( expr: ((double*)($c.Array.Addr))[$i], size: 1+$c.UpperIndex ) )
}

TColStd_Array1OfReal {
  preview ( #( "Array1OfReal [", $e.myLowerBound, "..", $e.myUpperBound, "]" ) )
  children ( #array ( expr: ((double*)($c.myStart))[$i], size: 1+$c.myUpperBound ) )
}

Handle_TColStd_HArray1OfReal {
  preview ( #( "HArray1OfReal [",
               ((TColStd_HArray1OfReal*)$e.entity)->myArray.myLowerBound, "..", 
               ((TColStd_HArray1OfReal*)$e.entity)->myArray.myUpperBound, "] ",
               [$e.entity,x], " count=", $e.entity->count ) )
  children ( #array ( expr: ((double*)(((TColStd_HArray1OfReal*)$e.entity)->myArray.myStart))[$i],
                      size: 1+((TColStd_HArray1OfReal*)$e.entity)->myArray.myUpperBound ) )
}

TColStd_Array1OfInteger {
  preview ( #( "Array1OfInteger [", $e.myLowerBound, "..", $e.myUpperBound, "]" ) )
  children ( #array ( expr: ((int*)($c.myStart))[$i], size: 1+$c.myUpperBound ) )
}

Handle_TColStd_HArray1OfInteger {
  preview ( #( "HArray1OfInteger [",
               ((TColStd_HArray1OfInteger*)$e.entity)->myArray.myLowerBound, "..", 
               ((TColStd_HArray1OfInteger*)$e.entity)->myArray.myUpperBound, "] ",
               [$e.entity,x], " count=", $e.entity->count ) )
  children ( #array ( expr: ((int*)(((TColStd_HArray1OfInteger*)$e.entity)->myArray.myStart))[$i],
                      size: 1+((TColStd_HArray1OfInteger*)$e.entity)->myArray.myUpperBound ) )
}

Handle_TCollection_HExtendedString {
  preview ( #( "HExtendedString ", [$e.entity,x], " count=", $e.entity->count, 
               " ", ((TCollection_HExtendedString*)$e.entity)->myString ) )
  children ( #([actual members]: [$e,!] ) )
}

Handle_TCollection_HAsciiString {
  preview ( #( "HAsciiString ", [$e.entity,x], " count=", $e.entity->count, 
               " ", ((TCollection_HAsciiString*)$e.entity)->myString ) )
  children ( #([actual members]: [$e,!], 
             #array( expr: ((TCollection_HAsciiString*)$e.entity)->myString.mystring[$i], 
                     size: ((TCollection_HAsciiString*)$e.entity)->myString.mylength) ) )
}
~~~~~

In Visual Studio 2012 and later, visualizers can be put in a separate file in subdirectory *Visualizers*. See file *occt.natvis* for example.

@section occt_debug_perf Performance measurement tools

It is recommended to use specialized performance analysis tools to profile OCCT and application code.
However, when such tools are not available or cannot be used for some reason, tools provided by OSD package can be used: low-level C functions and macros defined in *OSD_PerfMeter.h* and *OSD_PerfMeter* class.

This tool maintains an array of 100 global performance counters that can be started and stopped independently. Adding a performance counter to a function of interest allows to get statistics on the number of calls and the total execution time of the function.
* In C++ code, this can be achieved by creating local variable *OSD_PerfMeter* in each block of code to be measured.
* In C or Fortran code, use functions *perf_start_meter* and *perf_stop_meter* to start and stop the counter.

Note that this instrumentation is intended to be removed when the profiling is completed.

Macros provided in *OSD_PerfMeter.h* can be used to keep instrumentation code permanently but enable it only when macro *PERF_ENABLE_METERS* is defined.
Each counter has its name shown when the collected statistics are printed.

In DRAW, use command *dperf* to print all performance statistics.

Note that performance counters are not thread-safe.
