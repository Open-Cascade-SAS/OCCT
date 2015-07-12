// Created on: 2008-05-08
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


#include <Voxel_ColorDS.hxx>

#include <stdlib.h>
static Standard_Byte gbits[8] = {1, 2, 4, 8, 16, 32, 64, 128};
static Standard_Byte gnbits[8] = {255-1, 255-2, 255-4, 255-8, 255-16, 255-32, 255-64, 255-128};

// Empty constructor
Voxel_ColorDS::Voxel_ColorDS():Voxel_DS()
{

}

// Constructor with intialization.
Voxel_ColorDS::Voxel_ColorDS(const Standard_Real      x, const Standard_Real      y, const Standard_Real      z, 
			     const Standard_Real   xlen, const Standard_Real   ylen, const Standard_Real   zlen,
			     const Standard_Integer nbx, const Standard_Integer nby, const Standard_Integer nbz)
:Voxel_DS()
{
  Init(x, y, z, xlen, ylen, zlen, nbx, nby, nbz);
}

// Initialization.
void Voxel_ColorDS::Init(const Standard_Real      x, const Standard_Real      y, const Standard_Real      z, 
			 const Standard_Real   xlen, const Standard_Real   ylen, const Standard_Real   zlen,
			 const Standard_Integer nbx, const Standard_Integer nby, const Standard_Integer nbz)
{
  Destroy();

  Voxel_DS::Init(x, y, z, xlen, ylen, zlen, nbx, nby, nbz);

  if (!myNbX || !myNbY || !myNbZ)
    return;

  Standard_Integer nb_bytes = RealToInt(ceil(myNbXY * myNbZ / 2.0));
  Standard_Integer nb_slices = RealToInt(ceil(nb_bytes / 32.0));
  myData = (Standard_Address) calloc(nb_slices, sizeof(Standard_Byte*));
}

// Destructor
void Voxel_ColorDS::Destroy()
{
  if (myData)
  {
    SetZero();
    free((Standard_Byte**)myData);
    myData = 0;
  }
}

void Voxel_ColorDS::SetZero()
{
  if (myData)
  {
    Standard_Integer nb_bytes = RealToInt(ceil(myNbXY * myNbZ / 2.0));
    Standard_Integer ix = 0, nb_slices = RealToInt(ceil(nb_bytes / 32.0));
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
void Voxel_ColorDS::Set(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz, 
			const Standard_Byte data)
{
  Standard_Integer ibit = 4 * (ix + myNbX * iy + myNbXY * iz);
  Standard_Integer islice = ibit >> 8;

  if (!data && !((Standard_Byte**)myData)[islice])
    return; // don't allocate a slice of data for setting a 0 value

  // Allocate the slice if it is not done yet.
  if (!((Standard_Byte**)myData)[islice])
  {
    ((Standard_Byte**)myData)[islice] = (Standard_Byte*) calloc(32/*number of bytes in slice*/, sizeof(Standard_Byte));
  }

  // Index within 32 bytes of the slice.
  Standard_Integer ibit_in_current_slice = ibit - (islice << 8);
  Standard_Integer ibyte = ibit_in_current_slice >> 3;

  // Value (byte)
  Standard_Byte value = ((Standard_Byte*)((Standard_Byte**)myData)[islice])[ibyte];

  // Position of data in the 8 bit-"value":
  // It is either 0 (first part of the byte) or 2 (second part of the byte)
  Standard_Integer shift = ibit_in_current_slice - (ibyte << 3);

  // Set data
  Standard_Integer i = 0, j = 0, nb = 4; // first part of byte
  if (shift == 4) 
  {
    // second part of byte
    i = 4; 
    nb = 8;
  }

  for (; i < nb; i++, j++)
  {
    if (data & gbits[j]) // if j-th bit is 1
    {
      value |= gbits[i]; // set 1 to the i-th bit
    }
    else 
    {
      value &= gnbits[i]; // set 0 to the i-th bit
    }
  }

  ((Standard_Byte*)((Standard_Byte**)myData)[islice])[ibyte] = value;
}

Standard_Byte Voxel_ColorDS::Get(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz) const
{
  Standard_Byte ret = 0;
  Standard_Integer ibit = 4 * (ix + myNbX * iy + myNbXY * iz);
  Standard_Integer islice = ibit >> 8;

  // If the slice of data is not allocated, it means that its values are 0.
  if (!((Standard_Byte**)myData)[islice])
    return ret;

  // Index within 8 bytes of the slice.
  Standard_Integer ibit_in_current_slice = ibit - (islice << 8);
  Standard_Integer ibyte = ibit_in_current_slice >> 3;

  // Value (byte)
  Standard_Byte value = ((Standard_Byte*)((Standard_Byte**)myData)[islice])[ibyte];

  // Position of data in the 8 bit-"value".
  Standard_Integer shift = ibit_in_current_slice - (ibyte << 3);

  // Return a byte with 4 first bits filled-in by the value.
  Standard_Integer i = 0, j = 0, nb = 4; // first part of byte
  if (shift == 4)
  {
    // second part of byte
    i = 4; 
    nb = 8;
  }

  for (; i < nb; i++, j++)
  {
    if (value & gbits[i])    // if i-th bit is 1
    {
      ret |= gbits[j]; // set 1 to the j-th bit
    }
  }

  return ret;
}
