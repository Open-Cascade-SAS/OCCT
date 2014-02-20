// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_Vec3_HeaderFile
#define _Graphic3d_Vec3_HeaderFile

#include <NCollection_Vec3.hxx>
#include <Standard_TypeDef.hxx>

typedef NCollection_Vec3<Standard_ShortReal> Graphic3d_Vec3;
typedef NCollection_Vec3<Standard_Real>      Graphic3d_Vec3d;
typedef NCollection_Vec3<Standard_Integer>   Graphic3d_Vec3i;
typedef NCollection_Vec3<unsigned int>       Graphic3d_Vec3u;
typedef NCollection_Vec3<Standard_Byte>      Graphic3d_Vec3ub;
typedef NCollection_Vec3<Standard_Character> Graphic3d_Vec3b;

#endif // _Graphic3d_Vec3_HeaderFile
