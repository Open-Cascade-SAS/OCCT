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
