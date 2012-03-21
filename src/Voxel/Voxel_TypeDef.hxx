// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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

#include <NCollection_DefineBaseCollection.hxx>
DEFINE_BASECOLLECTION(iXYZbase, iXYZ)

#include <NCollection_DefineDataMap.hxx>
DEFINE_DATAMAP(iXYZIndex, iXYZbase, iXYZ, Standard_Integer)
DEFINE_DATAMAP(iXYZBool,  iXYZbase, iXYZ, Standard_Byte)

// Defines

#define VOXELS  "Voxels"
#define ASCII   "Ascii"
#define BINARY  "Binary"
#define BOOL    "Bool"
#define COLOR   "Color"
#define FLOAT   "Float"

#endif // _VOXEL_TYPEDEF_HXX_
