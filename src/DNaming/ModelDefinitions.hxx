// Created on: 2009-04-30
// Created by: Sergey ZARITCHNY <sergey.zaritchny@opencascade.com>
// Copyright (c) 2009-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef ModelDefinitions_HeaderFile
#define ModelDefinitions_HeaderFile

#define GEOMOBJECT_GUID Standard_GUID("6c6915ab-775f-4475-859e-befd74d26a23")
#define ATTCH_GUID Standard_GUID("12e94554-6dbc-11d4-b9c8-0060b0ee281b")
#define XTTCH_GUID Standard_GUID("12e94555-6dbc-11d4-b9c8-0060b0ee281b")
#define PTXYZ_GUID Standard_GUID("12e94556-6dbc-11d4-b9c8-0060b0ee281b")
#define PTALINE_GUID Standard_GUID("12e94557-6dbc-11d4-b9c8-0060b0ee281b")
#define PRRLINE_GUID Standard_GUID("12e94558-6dbc-11d4-b9c8-0060b0ee281b")
#define PMIRR_GUID Standard_GUID("12e94559-6dbc-11d4-b9c8-0060b0ee281b")

#define BOX_GUID Standard_GUID("12e94543-6dbc-11d4-b9c8-0060b0ee281b")
#define SPH_GUID Standard_GUID("12e94544-6dbc-11d4-b9c8-0060b0ee281b")
#define CYL_GUID Standard_GUID("12e94545-6dbc-11d4-b9c8-0060b0ee281b")
#define CONE_GUID Standard_GUID("12e94546-6dbc-11d4-b9c8-0060b0ee281b")
#define TORUS_GUID Standard_GUID("12e94547-6dbc-11d4-b9c8-0060b0ee281b")

#define CUT_GUID Standard_GUID("12e94548-6dbc-11d4-b9c8-0060b0ee281b")
#define FUSE_GUID Standard_GUID("12e94549-6dbc-11d4-b9c8-0060b0ee281b")
#define COMMON_GUID Standard_GUID("12e9454a-6dbc-11d4-b9c8-0060b0ee281b")
#define SECTION_GUID Standard_GUID("12e9454b-6dbc-11d4-b9c8-0060b0ee281b")

#define PRISM_GUID Standard_GUID("12e94550-6dbc-11d4-b9c8-0060b0ee281b")
#define FULREVOL_GUID Standard_GUID("12e94551-6dbc-11d4-b9c8-0060b0ee281b")
#define SECREVOL_GUID Standard_GUID("12e94552-6dbc-11d4-b9c8-0060b0ee281b")
#define FILLT_GUID Standard_GUID("12e94553-6dbc-11d4-b9c8-0060b0ee281b")
#define CHAMF_GUID Standard_GUID("12e9455a-6dbc-11d4-b9c8-0060b0ee281b")
#define OFFSET_GUID Standard_GUID("12e9455b-6dbc-11d4-b9c8-0060b0ee281b")

#define PNTXYZ_GUID Standard_GUID("12e9455c-6dbc-11d4-b9c8-0060b0ee281b")
#define PNTRLT_GUID Standard_GUID("12e9455d-6dbc-11d4-b9c8-0060b0ee281b")
#define LINE3D_GUID Standard_GUID("12e9455e-6dbc-11d4-b9c8-0060b0ee281b")
#define WIRE_GUID Standard_GUID("12e9455f-6dbc-11d4-b9c8-0060b0ee281b")

// Function structure definitions
#define FUNCTION_ARGUMENTS_LABEL 1
#define FUNCTION_RESULT_LABEL 2
#define POSITION(Function, theA)                                                                   \
  Function->Label().FindChild(FUNCTION_ARGUMENTS_LABEL).FindChild(theA)
#define RESPOSITION(Function) Function->Label().FindChild(FUNCTION_RESULT_LABEL)
#define BOX_DX 1
#define BOX_DY 2
#define BOX_DZ 3
#define CYL_RADIUS 1
#define CYL_HEIGHT 2
#define CYL_AXIS 3
#define ATTACH_ARG 1
#define BOOL_TOOL 1
#define SECT_OBJECT 1
#define SECT_TOOL 2
#define FILLET_RADIUS 1
#define FILLET_SURFTYPE 2
#define FILLET_PATH 3
#define PTRANSF_DX 1
#define PTRANSF_DY 2
#define PTRANSF_DZ 3
#define PTRANSF_OFF 1
#define PTRANSF_ANG 1
#define PTRANSF_LINE 2
#define PTRANSF_PLANE 1
#define PRISM_BASIS 1
#define PRISM_HEIGHT 2
#define PRISM_DIR 3
#define REVOL_BASIS 1
#define REVOL_AXIS 2
#define REVOL_ANGLE 3
#define REVOL_REV 4
#define SPHERE_CENTER 1
#define SPHERE_RADIUS 2
#define PNT_DX 1
#define PNT_DY 2
#define PNT_DZ 3
#define PNTRLT_REF 4
#define LINE3D_TYPE 1
#define LINE3D_PNTNB 2
#define DONE 0
#define NOTDONE 9999
#define ALGO_FAILED 11
#define RESULT_NOT_VALID 12
#define WRONG_AXIS 13
#define WRONG_ARGUMENT 14
#define UNSUPPORTED_FUNCTION 15
#define NULL_RESULT 16
#define WRONG_CONTEXT 14
#define NAMING_FAILED 15

#endif
