// Created on: 2008-08-27
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


#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <Voxel_OctBoolDS.hxx>
#include <Voxel_TypeDef.hxx>

#include <stdlib.h>
static Standard_Byte gbits[8] = {1, 2, 4, 8, 16, 32, 64, 128};
static Standard_Byte gnbits[8] = {255-1, 255-2, 255-4, 255-8, 255-16, 255-32, 255-64, 255-128};
static iXYZ xyz;

// Empty constructor
Voxel_OctBoolDS::Voxel_OctBoolDS():Voxel_DS(),mySubVoxels(0)
{

}

// Constructor with intialization.
Voxel_OctBoolDS::Voxel_OctBoolDS(const Standard_Real      x, const Standard_Real      y, const Standard_Real      z, 
				 const Standard_Real   xlen, const Standard_Real   ylen, const Standard_Real   zlen,
				 const Standard_Integer nbx, const Standard_Integer nby, const Standard_Integer nbz)
:Voxel_DS(),mySubVoxels(0)
{
  Init(x, y, z, xlen, ylen, zlen, nbx, nby, nbz);
}

// Initialization.
void Voxel_OctBoolDS::Init(const Standard_Real      x, const Standard_Real      y, const Standard_Real      z, 
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
void Voxel_OctBoolDS::Destroy()
{
  if (myData)
  {
    SetZero();
    free((Standard_Byte**)myData);
    myData = 0;
    if (mySubVoxels)
    {
      delete (iXYZBool*)mySubVoxels;
      mySubVoxels = 0;
    }
  }
}

void Voxel_OctBoolDS::SetZero()
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

  if (mySubVoxels)
  {
    iXYZBool* map = (iXYZBool*) mySubVoxels;
    map->Clear();
  }
}

// Access to the boolean information attached to a particular voxel:
// Info: (ix >= 0 && ix < theNb_x), etc.
void Voxel_OctBoolDS::Set(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz, 
			  const Standard_Boolean data)
{
  // All 8 sub-voxels have the same value.
  // No need anymore to keep them in memory.
  if (IsSplit(ix, iy, iz))
  {
    UnSplit(ix, iy, iz);
  }

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

void Voxel_OctBoolDS::Set(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz, 
			  const Standard_Integer ioct, const Standard_Boolean data)
{
  // If the voxel is not split yet, do it now.
  if (!IsSplit(ix, iy, iz))
  {
    Split(ix, iy, iz);
  }

  // Voxel
  xyz.ix = ix;
  xyz.iy = iy;
  xyz.iz = iz;

  // Take the value
  Standard_Byte value = ((iXYZBool*)mySubVoxels)->Find(xyz);

  // Set data
  if (data != ((value & gbits[ioct]) ? Standard_True : Standard_False))
  {
    if (data)
      value |= gbits[ioct];
    else
      value &= gnbits[ioct];
    ((iXYZBool*)mySubVoxels)->ChangeFind(xyz) = value;
  }
}

Standard_Boolean Voxel_OctBoolDS::Get(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz) const
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

Standard_Boolean Voxel_OctBoolDS::Get(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz,
				      const Standard_Integer ioct) const
{
  // If the voxel is not split, return the value of the voxel.
  if (!IsSplit(ix, iy, iz))
  {
    return Get(ix, iy, iz);
  }

  // Voxel
  xyz.ix = ix;
  xyz.iy = iy;
  xyz.iz = iz;

  // Take the value
  const Standard_Byte value = ((iXYZBool*)mySubVoxels)->Find(xyz);

  // Return data
  return (value & gbits[ioct]) ? Standard_True : Standard_False;
}

Standard_Boolean Voxel_OctBoolDS::IsSplit(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz) const
{
  if (!mySubVoxels)
    return Standard_False;

  // Voxel
  xyz.ix = ix;
  xyz.iy = iy;
  xyz.iz = iz;

  return ((iXYZBool*)mySubVoxels)->IsBound(xyz);
}

void Voxel_OctBoolDS::Split(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz)
{
  // Voxel
  xyz.ix = ix;
  xyz.iy = iy;
  xyz.iz = iz;

  if (mySubVoxels)
  {
    if (!((iXYZBool*)mySubVoxels)->IsBound(xyz))
    {
      ((iXYZBool*)mySubVoxels)->Bind(xyz, 0);
    }
  }
  else
  {
    mySubVoxels = (Standard_Address) new iXYZBool;
    ((iXYZBool*)mySubVoxels)->Bind(xyz, 0);
  }
}

void Voxel_OctBoolDS::UnSplit(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz)
{
  if (!mySubVoxels)
    return;

  // Voxel
  xyz.ix = ix;
  xyz.iy = iy;
  xyz.iz = iz;

  if (((iXYZBool*)mySubVoxels)->IsBound(xyz))
  {
    ((iXYZBool*)mySubVoxels)->UnBind(xyz);
  }
}

void Voxel_OctBoolDS::OptimizeMemory()
{
  if (!mySubVoxels)
    return;

  Standard_Byte value;
  TColStd_ListOfInteger ixs, iys, izs, values;
  iXYZBool::Iterator itr(*((iXYZBool*)mySubVoxels));
  for (; itr.More(); itr.Next())
  {
    value = itr.Value();
    if (value == 0 || value == 255)
    {
      xyz = itr.Key();
      ixs.Append(xyz.ix);
      iys.Append(xyz.iy);
      izs.Append(xyz.iz);
      values.Append((Standard_Integer)value);
    }
  }

  TColStd_ListIteratorOfListOfInteger itrix(ixs), itriy(iys), itriz(izs), itrvalues(values);
  for (; itrix.More(); itrix.Next(), itriy.Next(), itriz.Next(), itrvalues.Next())
  {
      const Standard_Integer ix = itrix.Value();
      const Standard_Integer iy = itriy.Value();
      const Standard_Integer iz = itriz.Value();
      const Standard_Integer aValue = itrvalues.Value();

      Set(ix, iy, iz, (aValue ? Standard_True : Standard_False));
      UnSplit(ix, iy, iz);
  }

  // If the map is empty, release it.
  if (((iXYZBool*)mySubVoxels)->IsEmpty())
  {
    delete (iXYZBool*)mySubVoxels;
    mySubVoxels = 0;
  }
}
