// Created on: 2008-05-15
// Created by: Vladislav ROMASHKO
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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


#include <Voxel_FloatDS.hxx>

#include <stdlib.h>
// Empty constructor
Voxel_FloatDS::Voxel_FloatDS():Voxel_DS()
{

}

// Constructor with intialization.
Voxel_FloatDS::Voxel_FloatDS(const Standard_Real      x, const Standard_Real      y, const Standard_Real      z, 
			     const Standard_Real   xlen, const Standard_Real   ylen, const Standard_Real   zlen,
			     const Standard_Integer nbx, const Standard_Integer nby, const Standard_Integer nbz)
:Voxel_DS()
{
  Init(x, y, z, xlen, ylen, zlen, nbx, nby, nbz);
}

// Initialization.
void Voxel_FloatDS::Init(const Standard_Real      x, const Standard_Real      y, const Standard_Real      z, 
			 const Standard_Real   xlen, const Standard_Real   ylen, const Standard_Real   zlen,
			 const Standard_Integer nbx, const Standard_Integer nby, const Standard_Integer nbz)
{
  Destroy();

  Voxel_DS::Init(x, y, z, xlen, ylen, zlen, nbx, nby, nbz);

  if (!myNbX || !myNbY || !myNbZ)
    return;

  Standard_Integer nb_floats = myNbXY * myNbZ;
  Standard_Integer nb_slices = RealToInt(ceil(nb_floats / 32.0)); // 32 values in 1 slice
  myData = (Standard_Address) calloc(nb_slices, sizeof(Standard_ShortReal*));
}

// Destructor
void Voxel_FloatDS::Destroy()
{
  if (myData)
  {
    SetZero();
    free((Standard_ShortReal**)myData);
    myData = 0;
  }
}

void Voxel_FloatDS::SetZero()
{
  if (myData)
  {
    Standard_Integer nb_bytes = myNbXY * myNbZ;
    Standard_Integer ix = 0, nb_slices = RealToInt(ceil(nb_bytes / 32.0));
    for (; ix < nb_slices; ix++)
    {
      if (((Standard_ShortReal**)myData)[ix])
      {
	free(((Standard_ShortReal**)myData)[ix]);
	((Standard_ShortReal**)myData)[ix] = 0;
      }
    }
  }
}

// Access to the floating-point information attached to a particular voxel:
// Info: (ix >= 0 && ix < theNb_x), etc.
void Voxel_FloatDS::Set(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz, 
			const Standard_ShortReal data)
{
  Standard_Integer ifloat = ix + myNbX * iy + myNbXY * iz;
  Standard_Integer islice = ifloat >> 5;

  // Allocate the slice if it is not done yet.
  if (!((Standard_ShortReal**)myData)[islice])
  {
    ((Standard_ShortReal**)myData)[islice] = 
      (Standard_ShortReal*) calloc(32/*number of floating values in slice*/, sizeof(Standard_ShortReal));
  }

  // Index of start-byte of the value within the slice.
  Standard_Integer ivalue = ifloat - (islice << 5);

  // Value (float)
  ((Standard_ShortReal*)((Standard_ShortReal**)myData)[islice])[ivalue] = data;
}

Standard_ShortReal Voxel_FloatDS::Get(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz) const
{
  Standard_Integer ifloat = ix + myNbX * iy + myNbXY * iz;
  Standard_Integer islice = ifloat >> 5;

  // If the slice of data is not allocated, it means that its values are 0.
  if (!((Standard_ShortReal**)myData)[islice])
    return 0.0f;

  // Index of start-byte of the value within the slice.
  Standard_Integer ivalue = ifloat - (islice << 5);

  // Value (floating-point value)
  return ((Standard_ShortReal*)((Standard_ShortReal**)myData)[islice])[ivalue];
}
