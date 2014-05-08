// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _VOXEL_TYPEDEF_HXX_
#define _VOXEL_TYPEDEF_HXX_

// Basic data types

typedef struct
{
  Standard_Integer ix;
  Standard_Integer iy;
  Standard_Integer iz;
} iXYZ;


// Maps and Tables

inline Standard_Integer HashCode(const iXYZ& me, const Standard_Integer upper)
{
  return (Abs(me.ix + me.iy + me.iz) % upper) + 1;
}

inline Standard_Boolean IsEqual(const iXYZ& one, const iXYZ& two)
{ 
  return one.ix == two.ix && one.iy == two.iy && one.iz == two.iz;
}

#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<iXYZ, Standard_Integer> iXYZIndex;
typedef NCollection_DataMap<iXYZ, Standard_Byte> iXYZBool;

// Defines

#define VOXELS  "Voxels"
#define ASCII   "Ascii"
#define BINARY  "Binary"
#define BOOL    "Bool"
#define COLOR   "Color"
#define FLOAT   "Float"

#endif // _VOXEL_TYPEDEF_HXX_
