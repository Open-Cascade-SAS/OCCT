// Created on: 2008-05-11
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


#include <Voxel_DS.hxx>
#include <Voxel_Reader.hxx>
#include <Voxel_Writer.hxx>

// Empty constructor
Voxel_DS::Voxel_DS()
:myData(0),
 myX(0.0),myY(0.0),myZ(0.0),
 myXLen(0.0),myYLen(0.0),myZLen(0.0),
 myNbX(0),myNbY(0),myNbZ(0),
 myNbXY(0),myDX(0.0),myDY(0.0),myDZ(0.0),
 myHalfDX(0.0),myHalfDY(0.0),myHalfDZ(0.0)
{

}

// Constructor with intialization.
Voxel_DS::Voxel_DS(const Standard_Real      x, const Standard_Real      y, const Standard_Real      z, 
		   const Standard_Real   xlen, const Standard_Real   ylen, const Standard_Real   zlen,
		   const Standard_Integer nbx, const Standard_Integer nby, const Standard_Integer nbz)
{
  Init(x, y, z, xlen, ylen, zlen, nbx, nby, nbz);
}

// Initialization.
void Voxel_DS::Init(const Standard_Real      x, const Standard_Real      y, const Standard_Real      z, 
		    const Standard_Real   xlen, const Standard_Real   ylen, const Standard_Real   zlen,
		    const Standard_Integer nbx, const Standard_Integer nby, const Standard_Integer nbz)
{
  myX    = x;
  myY    = y;
  myZ    = z;
  myXLen = xlen;
  myYLen = ylen;
  myZLen = zlen;
  myNbX  = nbx;
  myNbY  = nby;
  myNbZ  = nbz;
  myNbXY = myNbX * myNbY;
  myDX   = myXLen / (Standard_Real) myNbX;
  myDY   = myYLen / (Standard_Real) myNbY;
  myDZ   = myZLen / (Standard_Real) myNbZ;
  myHalfDX = myDX / 2.0;
  myHalfDY = myDY / 2.0;
  myHalfDZ = myDZ / 2.0;
}

// Get the initial information on voxels
Standard_Real Voxel_DS::GetX() const
{
  return myX;
}

Standard_Real Voxel_DS::GetY() const
{
  return myY;
}

Standard_Real Voxel_DS::GetZ() const
{
  return myZ;
}

Standard_Real Voxel_DS::GetXLen() const
{
  return myXLen;
}

Standard_Real Voxel_DS::GetYLen() const
{
  return myYLen;
}

Standard_Real Voxel_DS::GetZLen() const
{
  return myZLen;
}

Standard_Integer Voxel_DS::GetNbX() const
{
  return myNbX;
}

Standard_Integer Voxel_DS::GetNbY() const
{
  return myNbY;
}

Standard_Integer Voxel_DS::GetNbZ() const
{
  return myNbZ;
}

void Voxel_DS::GetCenter(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz,
			 Standard_Real&         xc, Standard_Real&         yc, Standard_Real&         zc) const
{
  GetOrigin(ix, iy, iz, xc, yc, zc);
  xc += myHalfDX;
  yc += myHalfDY;
  zc += myHalfDZ;
}

void Voxel_DS::GetOrigin(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz,
			 Standard_Real&         x0, Standard_Real&         y0, Standard_Real&         z0) const
{
  x0 = myX + ix * myDX;
  y0 = myY + iy * myDY;
  z0 = myZ + iz * myDZ;
}

// The method uses a chordial approach to find the index of voxel by co-ordinate.
static Standard_Integer findIndex(const Standard_Real xstart, const Standard_Real dx,
				  const Standard_Integer ix1, const Standard_Integer ix2,
				  const Standard_Real x)
{
  if (ix2 - ix1 < 2)
  { 
    if (x < xstart + ix2 * dx)
      return ix1;
    return ix2;
  }

  // Middle index
  const Standard_Integer ixm = (ix1 + ix2) / 2;

  // Check if it is in the first half:
  if (x >= xstart + ix1 * dx && x < xstart + ixm * dx)
  {
    return findIndex(xstart, dx, ix1, ixm, x);
  }

  return findIndex(xstart, dx, ixm, ix2, x);
}

Standard_Boolean Voxel_DS::GetVoxel(const Standard_Real x, const Standard_Real y, const Standard_Real z, 
				    Standard_Integer&  ix, Standard_Integer&  iy, Standard_Integer&  iz) const
{
  // X
  if (!GetVoxelX(x, ix))
    return Standard_False;
  
  // Y
  if (!GetVoxelY(y, iy))
    return Standard_False;
  
  // Z
  return GetVoxelZ(z, iz);
}

Standard_Boolean Voxel_DS::GetVoxelX(const Standard_Real x,
				     Standard_Integer&  ix) const
{
  // X
  if (x < myX || x > myX + myXLen)
    return Standard_False;
  ix = findIndex(myX, myXLen / (Standard_Real) myNbX, 0, myNbX - 1, x);
  return Standard_True;
}

Standard_Boolean Voxel_DS::GetVoxelY(const Standard_Real y,
				     Standard_Integer&  iy) const
{
  // Y
  if (y < myY || y > myY + myYLen)
    return Standard_False;
  iy = findIndex(myY, myYLen / (Standard_Real) myNbY, 0, myNbY - 1, y);
  return Standard_True;
}

Standard_Boolean Voxel_DS::GetVoxelZ(const Standard_Real z,
				     Standard_Integer&  iz) const
{
  // Z
  if (z < myZ || z > myZ + myZLen)
    return Standard_False;
  iz = findIndex(myZ, myZLen / (Standard_Real) myNbZ, 0, myNbZ - 1, z);
  return Standard_True;
}
