// Created on: 2008-09-01
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


#include <Voxel_ROctBoolDS.hxx>
#include <Voxel_SplitData.hxx>

#include <stdlib.h>
static Standard_Byte gbits[8] = {1, 2, 4, 8, 16, 32, 64, 128};
static Standard_Byte gnbits[8] = {255-1, 255-2, 255-4, 255-8, 255-16, 255-32, 255-64, 255-128};

/* Data structure of the ROctBoolDS

SplitData: 1 byte (8 values)
       (a) SplitData: 8 bytes (64 values)
                  (b) SplitData: 64 bytes (512 values)
                             (c) SplitData: ...
                                        (d) SplitData: ...
*/

// Empty constructor
Voxel_ROctBoolDS::Voxel_ROctBoolDS():Voxel_DS()
{

}

// Constructor with intialization.
Voxel_ROctBoolDS::Voxel_ROctBoolDS(const Standard_Real      x, const Standard_Real      y, const Standard_Real      z, 
				   const Standard_Real   xlen, const Standard_Real   ylen, const Standard_Real   zlen,
				   const Standard_Integer nbx, const Standard_Integer nby, const Standard_Integer nbz)
:Voxel_DS()
{
  Init(x, y, z, xlen, ylen, zlen, nbx, nby, nbz);
}

// Initialization.
void Voxel_ROctBoolDS::Init(const Standard_Real      x, const Standard_Real      y, const Standard_Real      z, 
			    const Standard_Real   xlen, const Standard_Real   ylen, const Standard_Real   zlen,
			    const Standard_Integer nbx, const Standard_Integer nby, const Standard_Integer nbz)
{
  Destroy();

  Voxel_DS::Init(x, y, z, xlen, ylen, zlen, nbx, nby, nbz);

  if (!myNbX || !myNbY || !myNbZ)
    return;

  Standard_Integer nb_slices = RealToInt(ceil(myNbXY * myNbZ / 8.0));
  myData = (Standard_Address) calloc(nb_slices, sizeof(Voxel_SplitData*));
}

// Destructor
void Voxel_ROctBoolDS::Destroy()
{
  if (myData)
  {
    SetZero();
    free((Voxel_SplitData**)myData);
    myData = 0;
  }
}

// A recursive method of deletion of data.
static void SetZeroSplitData(Voxel_SplitData* data)
{
  // Values:
  free((Standard_Byte*) data->GetValues());
  data->GetValues() = 0;
  if (data->GetSplitData())
  {
    SetZeroSplitData((Voxel_SplitData*) data->GetSplitData());
  }
  delete data;
}

void Voxel_ROctBoolDS::SetZero()
{
  if (myData)
  {
    Standard_Integer ix = 0, nb_slices = RealToInt(ceil(myNbXY * myNbZ / 8.0));
    for (; ix < nb_slices; ix++)
    {
      if (((Voxel_SplitData**)myData)[ix])
      {
        SetZeroSplitData((Voxel_SplitData*)((Voxel_SplitData**)myData)[ix]);
        ((Voxel_SplitData**)myData)[ix] = 0;
      }
    }
  }
}

// Access to the boolean information attached to a particular voxel:
// Info: (ix >= 0 && ix < theNb_x), etc.
void Voxel_ROctBoolDS::Set(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz, 
			   const Standard_Boolean data)
{
  Standard_Integer ibit = ix + myNbX * iy + myNbXY * iz;
  Standard_Integer islice = ibit >> 3;

  if (!data && !((Voxel_SplitData**)myData)[islice])
    return; // don't allocate a slice of data for setting a 0 value

  // Allocate the slice if it is not done yet.
  if (!((Voxel_SplitData**)myData)[islice])
  {
    ((Voxel_SplitData**)myData)[islice] = (Voxel_SplitData*) new Voxel_SplitData;
    // Values:
    ((Voxel_SplitData**)myData)[islice]->GetValues() = 
      (Standard_Byte*) calloc(1/*one byte: 8 1-bit values*/, sizeof(Standard_Byte));
    // Sub-voxels:
    ((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData() = 0;
  }

  // Value
  Standard_Byte value = *((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetValues());

  // Position of data in the 8 bit-"value".
  Standard_Integer shift = ibit - (islice << 3);

  // Set data
  if (data != ((value & gbits[shift]) ? Standard_True : Standard_False))
  {
    if (data)
      value |= gbits[shift];
    else
      value &= gnbits[shift];
    *((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetValues()) = value;
  }

  // Set the same value to sub-voxels.
  if (((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())
  {
    // Get sub-value
    Standard_Byte subvalue = ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetValues())[shift];

    // Set sub-value
    if (subvalue != (data ? 255 : 0))
    {
      subvalue = data ? 255 : 0;
      ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetValues())[shift] = subvalue;
    }

    // Set the same value to sub-sub-voxels.
    if (((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())
    {
      // Start index of 64-bit value (index of byte of sub-sub-voxel).
      Standard_Integer ibyte2 = (shift << 3);
      for (Standard_Integer ioct2 = 0; ioct2 < 8; ioct2++)
      {
	// Get sub-sub-value
	Standard_Byte subsubvalue = ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())->GetValues())[ibyte2 + ioct2];

	// Set sub-sub-value
	if (subsubvalue != (data ? 255 : 0))
	{
	  subsubvalue = data ? 255 : 0;
	  ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())->GetValues())[ibyte2 + ioct2] = subsubvalue;
	}
      }
    }
  }
}

void Voxel_ROctBoolDS::Set(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz, 
			   const Standard_Integer ioct1, const Standard_Boolean data)
{
  Standard_Integer ibit = ix + myNbX * iy + myNbXY * iz;
  Standard_Integer islice = ibit >> 3;

  if (!data && !((Voxel_SplitData**)myData)[islice])
    return; // don't allocate a slice of data for setting a 0 value

  // Allocate the slice if it is not done yet.
  if (!((Voxel_SplitData**)myData)[islice])
  {
    ((Voxel_SplitData**)myData)[islice] = (Voxel_SplitData*) new Voxel_SplitData;
    // Values:
    ((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetValues() = 
      (Standard_Byte*) calloc(1/*one byte: 8 1-bit values*/, sizeof(Standard_Byte));
    // Sub-voxels:
    ((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData() = 0;
  }

  // Check sub-voxels of the first level
  if (!((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())
  {
    // Sub-voxels:
    ((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData() = (Voxel_SplitData*) new Voxel_SplitData;
    // Value of sub-voxels:
    ((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetValues() = 
      (Standard_Byte*) calloc(8/*eight bytes: 8 sub-voxels for each voxel*/, sizeof(Standard_Byte));

    // Set parent value
    Standard_Byte parent_value = *((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetValues());
    if (parent_value)
    {
      for (Standard_Integer shift = 0; shift < 8; shift++)
      {
	if ((parent_value & gbits[shift]) ? Standard_True : Standard_False)
	{
	  ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetValues())[shift] = 255;
	}
	else
	{
	  ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetValues())[shift] = 0;
	}
      }
    }

    // Sub-sub-voxels
    ((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData() = 0;
  }

  // Index of sub-voxel corresponding to ioct1: 8 voxels correspond to 64 sub-voxels.
  Standard_Integer ibyte = ibit - (islice << 3);

  // Value
  Standard_Byte value = ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetValues())[ibyte];

  // Set data
  if (data != ((value & gbits[ioct1]) ? Standard_True : Standard_False))
  {
    if (data)
      value |= gbits[ioct1];
    else
      value &= gnbits[ioct1];
    ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetValues())[ibyte] = value;
  }

  // Set the same value to sub-voxels.
  if (((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())
  {
    // Start index of 64-bit value (index of byte of sub-sub-voxel).
    Standard_Integer ibyte2 = (ibyte << 3) + ioct1;

    // Get sub-sub-value
    Standard_Byte subsubvalue = ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())->GetValues())[ibyte2];

    // Set sub-sub-value
    if (subsubvalue != (data ? 255 : 0))
    {
      subsubvalue = data ? 255 : 0;
      ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())->GetValues())[ibyte2] = subsubvalue;
    }
  }
}

void Voxel_ROctBoolDS::Set(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz, 
			   const Standard_Integer ioct1, const Standard_Integer ioct2, const Standard_Boolean data)
{
  Standard_Integer ibit = ix + myNbX * iy + myNbXY * iz;
  Standard_Integer islice = ibit >> 3;

  if (!data && !((Voxel_SplitData**)myData)[islice])
    return; // don't allocate a slice of data for setting a 0 value

  // Allocate the slice if it is not done yet.
  if (!((Voxel_SplitData**)myData)[islice])
  {
    ((Voxel_SplitData**)myData)[islice] = (Voxel_SplitData*) new Voxel_SplitData;
    // Values:
    ((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetValues() = 
      (Standard_Byte*) calloc(1/*one byte: 8 1-bit values*/, sizeof(Standard_Byte));
    // Sub-voxels:
    ((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData() = 0;
  }

  // Check sub-voxels of the first level
  if (!((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())
  {
    // Sub-voxels:
    ((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData() = (Voxel_SplitData*) new Voxel_SplitData;
    // Value of sub-voxels:
    ((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetValues() = 
      (Standard_Byte*) calloc(8/*eight bytes: 8 sub-voxels for each voxel*/, sizeof(Standard_Byte));

    // Set parent value
    Standard_Byte parent_value = *((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetValues());
    if (parent_value)
    {
      for (Standard_Integer shift = 0; shift < 8; shift++)
      {
	if ((parent_value & gbits[shift]) ? Standard_True : Standard_False)
	{
	  ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetValues())[shift] = 255;
	}
	else
	{
	  ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetValues())[shift] = 0;
	}
      }
    }

     // Sub-sub-voxels
    ((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData() = 0;
  }

  // Check sub-voxels of the second level
  if (!((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())
  {
    // Sub-voxels 2:
    ((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData() = 
      (Voxel_SplitData*) new Voxel_SplitData;
    // Value of sub-voxels 2:
    ((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())->GetValues() = 
      (Standard_Byte*) calloc(64/*sixty four bytes: 8 sub-voxels for each sub-voxel for each voxel*/, 
			      sizeof(Standard_Byte));

    // Set parent value
    for (Standard_Integer ibyte1 = 0; ibyte1 < 8; ibyte1++)
    {
      Standard_Byte parent_value = ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetValues())[ibyte1];
      if (parent_value)
      {
	Standard_Integer ibyte2 = (ibyte1 << 3);
	for (Standard_Integer shift = 0; shift < 8; shift++)
	{
	  if ((parent_value & gbits[shift]) ? Standard_True : Standard_False)
	  {
	    ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())->GetValues())[ibyte2 + shift] = 255;
	  }
	  else
	  {
	    ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())->GetValues())[ibyte2 + shift] = 0;
	  }
	}
      }
    }

    // Sub-sub-sub-voxels
    ((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())->GetSplitData() = 0;
  }

  // Index of sub-voxel corresponding to ioct1: 8 voxels correspond to 64 sub-voxels.
  Standard_Integer ibyte1 = ibit - (islice << 3);  // imdex of byte of  8-byte value (sub-voxel 1).
  Standard_Integer ibyte2 = (ibyte1 << 3) + ioct1; // index of byte of 64-byte value (sub-voxel 2)

  // Value
  Standard_Byte value = ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())->GetValues())[ibyte2];

  // Set data
  if (data != ((value & gbits[ioct2]) ? Standard_True : Standard_False))
  {
    if (data)
      value |= gbits[ioct2];
    else
      value &= gnbits[ioct2];
    ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())->GetValues())[ibyte2] = value;
  }
}

Standard_Boolean Voxel_ROctBoolDS::Get(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz) const
{
  Standard_Integer ibit = ix + myNbX * iy + myNbXY * iz;
  Standard_Integer islice = ibit >> 3;

  // If the slice of data is not allocated, it means that its values are 0.
  if (!((Voxel_SplitData**)myData)[islice])
    return Standard_False;

  // Value (byte)
  Standard_Byte value = *((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetValues());

  // Position of data in the 8 bit-"value".
  Standard_Integer shift = ibit - (islice << 3);
  return ((value & gbits[shift]) ? Standard_True : Standard_False);
}

Standard_Boolean Voxel_ROctBoolDS::Get(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz,
				      const Standard_Integer ioct1) const
{
  Standard_Integer ibit = ix + myNbX * iy + myNbXY * iz;
  Standard_Integer islice = ibit >> 3;

  // If the slice of data is not allocated, it means that its values are 0.
  if (!((Voxel_SplitData**)myData)[islice])
    return Standard_False;

  // If the voxel is not split, return the value of the voxel.
  if (!((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())
    return Get(ix, iy, iz);

  // Index of sub-voxel corresponding to ioct1: 8 voxels correspond to 64 sub-voxels.
  Standard_Integer ibyte = ibit - (islice << 3);

  // Value
  Standard_Byte value = ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetValues())[ibyte];

  return ((value & gbits[ioct1]) ? Standard_True : Standard_False);
}

Standard_Boolean Voxel_ROctBoolDS::Get(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz,
				       const Standard_Integer ioct1, const Standard_Integer ioct2) const
{
  Standard_Integer ibit = ix + myNbX * iy + myNbXY * iz;
  Standard_Integer islice = ibit >> 3;

  // If the slice of data is not allocated, it means that its values are 0.
  if (!((Voxel_SplitData**)myData)[islice])
    return Standard_False;

  // If the voxel is not split, return the value of the voxel.
  if (!((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())
    return Get(ix, iy, iz);

  // If the split voxel (sub-voxel 1) is not split, return the value of the sub-voxel 1.
  if (!((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())
    return Get(ix, iy, iz, ioct1);

  // Index of sub-voxel corresponding to ioct1: 8 voxels correspond to 64 sub-voxels.
  Standard_Integer ibyte1 = ibit - (islice << 3);  // index of byte of  8-byte value (sub-voxel 1).
  Standard_Integer ibyte2 = (ibyte1 << 3) + ioct1; // index of byte of 64-byte value (sub-voxel 2)

  // Value
  Standard_Byte value = ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())->GetValues())[ibyte2];

  return ((value & gbits[ioct2]) ? Standard_True : Standard_False);
}

Standard_Boolean Voxel_ROctBoolDS::IsSplit(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz) const
{
  Standard_Integer ibit = ix + myNbX * iy + myNbXY * iz;
  Standard_Integer islice = ibit >> 3;

  // If the voxel has no value, it is not split.
  if (!((Voxel_SplitData**)myData)[islice])
    return Standard_False;

  // Check existence of sub-voxels
  if (((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())
    return Standard_True;
  return Standard_False;
}

Standard_Integer Voxel_ROctBoolDS::Deepness(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz) const
{
  Standard_Integer ibit = ix + myNbX * iy + myNbXY * iz;
  Standard_Integer islice = ibit >> 3;

  // If the voxel has no value, it is not split.
  if (!((Voxel_SplitData**)myData)[islice])
    return 0;

  // Test deepness.
  if (((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())
  {
    if (((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())
    {
      return 2;
    }
    else
    {
      return 1;
    }
  }
  return 0;
}

void Voxel_ROctBoolDS::OptimizeMemory()
{
  // Iterate the array of voxels checking coincidence of values of sub-voxels.
  Standard_Integer islice = 0, nb_slices = RealToInt(ceil(myNbXY * myNbZ / 8.0));
  for (; islice < nb_slices; islice++)
  {
    if (!((Voxel_SplitData**)myData)[islice])
      continue;
    if (((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())
    {
      Standard_Boolean suppress = Standard_False;
      // Second level of sub-voxels
      if (((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())
      {
	suppress = Standard_False;
	Standard_Byte value1 = ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())->GetValues())[0];
	if (value1 == 0 || value1 == 255)
	{
	  suppress = Standard_True;
	  for (Standard_Integer ibyte2 = 1; ibyte2 < 64; ibyte2++)
	  {
	    Standard_Byte value2 = ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData())->GetValues())[ibyte2];
	    if (value2 != value1)
	    {
	      suppress = Standard_False;
	      break;
	    }
	  }
	}
	if (suppress)
	{
	  SetZeroSplitData((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData());
	  ((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetSplitData() = 0;
	  // Set value to upper level
	  for (Standard_Integer ibyte1 = 0; ibyte1 < 8; ibyte1++)
	  {
	    ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetValues())[ibyte1] = value1;
	  }
	}
	else
	{
	  // If we don't suppress sub-sub-voxels, we don't touch sub-voxels.
	  continue;
	}
      }
      // First level of sub-voxels
      suppress = Standard_False;
      Standard_Byte value1 = ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetValues())[0];
      if (value1 == 0 || value1 == 255)
      {
	suppress = Standard_True;
	for (Standard_Integer ibyte1 = 1; ibyte1 < 8; ibyte1++)
	{
	  Standard_Byte value2 = ((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData())->GetValues())[ibyte1];
	  if (value2 != value1)
	  {
	    suppress = Standard_False;
	    break;
	  }
	}
      }
      if (suppress)
      {
	SetZeroSplitData((Voxel_SplitData*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData());
	((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetSplitData() = 0;
	// Set value to upper level
	*((Standard_Byte*)((Voxel_SplitData*)((Voxel_SplitData**)myData)[islice])->GetValues()) = value1;
      }
    }
  }
}

void Voxel_ROctBoolDS::GetCenter(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz, 
				 const Standard_Integer i,
				 Standard_Real& xc, Standard_Real& yc, Standard_Real& zc) const
{
  xc = myX + ix * myDX;
  yc = myY + iy * myDY;
  zc = myZ + iz * myDZ;

  switch (i)
  {
    case 0:
    {
      xc += 0.5 * myHalfDX;
      yc += 0.5 * myHalfDY;
      zc += 0.5 * myHalfDZ;
      break;
    }
    case 1:
    {
      xc += 1.5 * myHalfDX;
      yc += 0.5 * myHalfDY;
      zc += 0.5 * myHalfDZ;
      break;
    }
    case 2:
    {
      xc += 0.5 * myHalfDX;
      yc += 1.5 * myHalfDY;
      zc += 0.5 * myHalfDZ;
      break;
    }
    case 3:
    {
      xc += 1.5 * myHalfDX;
      yc += 1.5 * myHalfDY;
      zc += 0.5 * myHalfDZ;
      break;
    }
    case 4:
    {
      xc += 0.5 * myHalfDX;
      yc += 0.5 * myHalfDY;
      zc += 1.5 * myHalfDZ;
      break;
    }
    case 5:
    {
      xc += 1.5 * myHalfDX;
      yc += 0.5 * myHalfDY;
      zc += 1.5 * myHalfDZ;
      break;
    }
    case 6:
    {
      xc += 0.5 * myHalfDX;
      yc += 1.5 * myHalfDY;
      zc += 1.5 * myHalfDZ;
      break;
    }
    case 7:
    {
      xc += 1.5 * myHalfDX;
      yc += 1.5 * myHalfDY;
      zc += 1.5 * myHalfDZ;
      break;
    }
  }
}

void Voxel_ROctBoolDS::GetCenter(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz, 
				 const Standard_Integer i, const Standard_Integer j,
				 Standard_Real& xc, Standard_Real& yc, Standard_Real& zc) const
{
  xc = myX + ix * myDX;
  yc = myY + iy * myDY;
  zc = myZ + iz * myDZ;

  switch (i)
  {
    case 0:
    {
      break;
    }
    case 1:
    {
      xc += myHalfDX;
      break;
    }
    case 2:
    {
      yc += myHalfDY;
      break;
    }
    case 3:
    {
      xc += myHalfDX;
      yc += myHalfDY;
      break;
    }
    case 4:
    {
      zc += myHalfDZ;
      break;
    }
    case 5:
    {
      xc += myHalfDX;
      zc += myHalfDZ;
      break;
    }
    case 6:
    {
      yc += myHalfDY;
      zc += myHalfDZ;
      break;
    }
    case 7:
    {
      xc += myHalfDX;
      yc += myHalfDY;
      zc += myHalfDZ;
      break;
    }
  }

  switch (j)
  {
    case 0:
    {
      xc += 0.25 * myHalfDX;
      yc += 0.25 * myHalfDY;
      zc += 0.25 * myHalfDZ;
      break;
    }
    case 1:
    {
      xc += 0.75 * myHalfDX;
      yc += 0.25 * myHalfDY;
      zc += 0.25 * myHalfDZ;
      break;
    }
    case 2:
    {
      xc += 0.25 * myHalfDX;
      yc += 0.75 * myHalfDY;
      zc += 0.25 * myHalfDZ;
      break;
    }
    case 3:
    {
      xc += 0.75 * myHalfDX;
      yc += 0.75 * myHalfDY;
      zc += 0.25 * myHalfDZ;
      break;
    }
    case 4:
    {
      xc += 0.25 * myHalfDX;
      yc += 0.25 * myHalfDY;
      zc += 0.75 * myHalfDZ;
      break;
    }
    case 5:
    {
      xc += 0.75 * myHalfDX;
      yc += 0.25 * myHalfDY;
      zc += 0.75 * myHalfDZ;
      break;
    }
    case 6:
    {
      xc += 0.25 * myHalfDX;
      yc += 0.75 * myHalfDY;
      zc += 0.75 * myHalfDZ;
      break;
    }
    case 7:
    {
      xc += 0.75 * myHalfDX;
      yc += 0.75 * myHalfDY;
      zc += 0.75 * myHalfDZ;
      break;
    }
  }
}
