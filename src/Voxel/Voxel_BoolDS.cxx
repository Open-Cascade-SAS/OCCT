// File:	Voxel_BoolDS.cxx
// Created:	Mon Jun 21 14:35:37 2008
// Author:	Vladislav ROMASHKO
//		<vladislav.romashko@opencascade.com>

#include <Voxel_BoolDS.ixx>

#include <stdlib.h>

static Standard_Byte gbits[8] = {1, 2, 4, 8, 16, 32, 64, 128};
static Standard_Byte gnbits[8] = {255-1, 255-2, 255-4, 255-8, 255-16, 255-32, 255-64, 255-128};

// Empty constructor
Voxel_BoolDS::Voxel_BoolDS():Voxel_DS()
{

}

// Constructor with intialization.
Voxel_BoolDS::Voxel_BoolDS(const Standard_Real      x, const Standard_Real      y, const Standard_Real      z, 
			   const Standard_Real   xlen, const Standard_Real   ylen, const Standard_Real   zlen,
			   const Standard_Integer nbx, const Standard_Integer nby, const Standard_Integer nbz)
:Voxel_DS()
{
  Init(x, y, z, xlen, ylen, zlen, nbx, nby, nbz);
}

// Initialization.
void Voxel_BoolDS::Init(const Standard_Real      x, const Standard_Real      y, const Standard_Real      z, 
			const Standard_Real   xlen, const Standard_Real   ylen, const Standard_Real   zlen,
			const Standard_Integer nbx, const Standard_Integer nby, const Standard_Integer nbz)
{
  Destroy();

  Voxel_DS::Init(x, y, z, xlen, ylen, zlen, nbx, nby, nbz);

  if (!myNbX || !myNbY || !myNbZ)
    return;

  Standard_Integer nb_bytes = RealToInt(ceil(myNbXY * myNbZ / 8.0));
  Standard_Integer nb_slices = RealToInt(ceil(nb_bytes / 8.0));
  myData = (Standard_Address) calloc(nb_slices, sizeof(Standard_Byte*));
}

// Destructor
void Voxel_BoolDS::Destroy()
{
  if (myData)
  {
    SetZero();
    free((Standard_Byte**)myData);
    myData = 0;
  }
}

void Voxel_BoolDS::SetZero()
{
  if (myData)
  {
    Standard_Integer nb_bytes = RealToInt(ceil(myNbXY * myNbZ / 8.0));
    Standard_Integer ix = 0, nb_slices = RealToInt(ceil(nb_bytes / 8.0));
    for (; ix < nb_slices; ix++)
    {
      if (((Standard_Byte**)myData)[ix])
      {
	free(((Standard_Byte**)myData)[ix]);
	((Standard_Byte**)myData)[ix] = 0;
      }
    }
  }  
}

// Access to the boolean information attached to a particular voxel:
// Info: (ix >= 0 && ix < theNb_x), etc.
void Voxel_BoolDS::Set(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz, 
		       const Standard_Boolean data)
{
  Standard_Integer ibit = ix + myNbX * iy + myNbXY * iz;
  Standard_Integer islice = ibit >> 6;

  if (!data && !((Standard_Byte**)myData)[islice])
    return; // don't allocate a slice of data for setting a 0 value

  // Allocate the slice if it is not done yet.
  if (!((Standard_Byte**)myData)[islice])
  {
    ((Standard_Byte**)myData)[islice] = (Standard_Byte*) calloc(8/*number of bytes in slice*/, sizeof(Standard_Byte));
  }

  // Index within 8 bytes of the slice.
  Standard_Integer ibit_in_current_slice = ibit - (islice << 6);
  Standard_Integer ibyte = ibit_in_current_slice >> 3;

  // Value (byte)
  Standard_Byte value = ((Standard_Byte*)((Standard_Byte**)myData)[islice])[ibyte];

  // Position of data in the 8 bit-"value".
  Standard_Integer shift = ibit_in_current_slice - (ibyte << 3);

  // Set data
  if (data != ((value & gbits[shift]) ? Standard_True : Standard_False))
  {
    if (data)
      value |= gbits[shift];
    else
      value &= gnbits[shift];
    ((Standard_Byte*)((Standard_Byte**)myData)[islice])[ibyte] = value;
  }
}

Standard_Boolean Voxel_BoolDS::Get(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz) const
{
  Standard_Integer ibit = ix + myNbX * iy + myNbXY * iz;
  Standard_Integer islice = ibit >> 6;

  // If the slice of data is not allocated, it means that its values are 0.
  if (!((Standard_Byte**)myData)[islice])
    return Standard_False;

  // Index within 8 bytes of the slice.
  Standard_Integer ibit_in_current_slice = ibit - (islice << 6);
  Standard_Integer ibyte = ibit_in_current_slice >> 3;

  // Value (byte)
  Standard_Byte value = ((Standard_Byte*)((Standard_Byte**)myData)[islice])[ibyte];

  // Position of data in the 8 bit-"value".
  Standard_Integer shift = ibit_in_current_slice - (ibyte << 3);

  return ((value & gbits[shift]) ? Standard_True : Standard_False);
}
