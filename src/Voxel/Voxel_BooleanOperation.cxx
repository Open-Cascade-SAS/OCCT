// Created on: 2008-05-21
// Created by: Vladislav ROMASHKO
// Copyright (c) 2008-2012 OPEN CASCADE SAS
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


#include <Voxel_BooleanOperation.ixx>
#include <Precision.hxx>

Voxel_BooleanOperation::Voxel_BooleanOperation()
{

}

Standard_Boolean Voxel_BooleanOperation::Fuse(      Voxel_BoolDS& theVoxels1,
					      const Voxel_BoolDS& theVoxels2) const
{
  // Check the voxels
  if (!Check(theVoxels1, theVoxels2))
    return Standard_False;

  // Take the values of the second cube and put them to the first one.
  Standard_Integer ix, iy, iz;
  for (iz = 0; iz < theVoxels2.GetNbZ(); iz++)
  {
    for (iy = 0; iy < theVoxels2.GetNbY(); iy++)
    {
      for (ix = 0; ix < theVoxels2.GetNbX(); ix++)
      {
	Standard_Boolean value2 = theVoxels2.Get(ix, iy, iz);
	if (value2)
	  theVoxels1.Set(ix, iy, iz, value2);
      }
    }
  }

  return Standard_True;
}

Standard_Boolean Voxel_BooleanOperation::Fuse(      Voxel_ColorDS& theVoxels1,
					      const Voxel_ColorDS& theVoxels2) const
{
  // Check the voxels
  if (!Check(theVoxels1, theVoxels2))
    return Standard_False;

  // Take the values of the second cube and put them to the first one.
  Standard_Integer ix, iy, iz;
  for (iz = 0; iz < theVoxels2.GetNbZ(); iz++)
  {
    for (iy = 0; iy < theVoxels2.GetNbY(); iy++)
    {
      for (ix = 0; ix < theVoxels2.GetNbX(); ix++)
      {
	Standard_Byte value2 = theVoxels2.Get(ix, iy, iz);
	if (value2)
	{
	  Standard_Byte value1 = theVoxels1.Get(ix, iy, iz);
	  Standard_Byte value = value1 + value2;
	  if (value > 15)
	    value = 15;
	  theVoxels1.Set(ix, iy, iz, value);
	}
      }
    }
  }

  return Standard_True;
}

Standard_Boolean Voxel_BooleanOperation::Fuse(      Voxel_FloatDS& theVoxels1,
					      const Voxel_FloatDS& theVoxels2) const
{
  // Check the voxels
  if (!Check(theVoxels1, theVoxels2))
    return Standard_False;

  // Take the values of the second cube and put them to the first one.
  Standard_Integer ix, iy, iz;
  for (iz = 0; iz < theVoxels2.GetNbZ(); iz++)
  {
    for (iy = 0; iy < theVoxels2.GetNbY(); iy++)
    {
      for (ix = 0; ix < theVoxels2.GetNbX(); ix++)
      {
	Standard_ShortReal value2 = theVoxels2.Get(ix, iy, iz);
	if (value2)
	{
	  Standard_ShortReal value1 = theVoxels1.Get(ix, iy, iz);
	  theVoxels1.Set(ix, iy, iz, value1 + value2);
	}
      }
    }
  }

  return Standard_True;
}

Standard_Boolean Voxel_BooleanOperation::Cut(      Voxel_BoolDS& theVoxels1,
					     const Voxel_BoolDS& theVoxels2) const
{
  // Check the voxels
  if (!Check(theVoxels1, theVoxels2))
    return Standard_False;

  // Subtract the values.
  Standard_Integer ix, iy, iz;
  for (iz = 0; iz < theVoxels2.GetNbZ(); iz++)
  {
    for (iy = 0; iy < theVoxels2.GetNbY(); iy++)
    {
      for (ix = 0; ix < theVoxels2.GetNbX(); ix++)
      {
	Standard_Boolean value1 = theVoxels1.Get(ix, iy, iz);
	if (value1)
	{
	  Standard_Boolean value2 = theVoxels2.Get(ix, iy, iz);
	  if (value2)
	    theVoxels1.Set(ix, iy, iz, Standard_False);
	}
      }
    }
  }

  return Standard_True;
}

Standard_Boolean Voxel_BooleanOperation::Cut(      Voxel_ColorDS& theVoxels1,
					     const Voxel_ColorDS& theVoxels2) const
{
  // Check the voxels
  if (!Check(theVoxels1, theVoxels2))
    return Standard_False;

  // Subtract the values.
  Standard_Integer ix, iy, iz;
  for (iz = 0; iz < theVoxels2.GetNbZ(); iz++)
  {
    for (iy = 0; iy < theVoxels2.GetNbY(); iy++)
    {
      for (ix = 0; ix < theVoxels2.GetNbX(); ix++)
      {
	Standard_Byte value2 = theVoxels2.Get(ix, iy, iz);
	if (value2)
	{
	  Standard_Byte value1 = theVoxels1.Get(ix, iy, iz);
	  if (value1)
	  {
	    Standard_Integer value = value1 - value2;
	    if (value < 0)
	      value = 0;
	    theVoxels1.Set(ix, iy, iz, value);
	  }
	}
      }
    }
  }

  return Standard_True;
}

Standard_Boolean Voxel_BooleanOperation::Cut(      Voxel_FloatDS& theVoxels1,
					     const Voxel_FloatDS& theVoxels2) const
{
  // Check the voxels
  if (!Check(theVoxels1, theVoxels2))
    return Standard_False;

  // Subtract the values.
  Standard_Integer ix, iy, iz;
  for (iz = 0; iz < theVoxels2.GetNbZ(); iz++)
  {
    for (iy = 0; iy < theVoxels2.GetNbY(); iy++)
    {
      for (ix = 0; ix < theVoxels2.GetNbX(); ix++)
      {
	Standard_ShortReal value2 = theVoxels2.Get(ix, iy, iz);
	if (value2)
	{
	  Standard_ShortReal value1 = theVoxels1.Get(ix, iy, iz);
	  theVoxels1.Set(ix, iy, iz, value1 - value2);
	}
      }
    }
  }

  return Standard_True;
}

Standard_Boolean Voxel_BooleanOperation::Check(const Voxel_DS& theVoxels1,
					       const Voxel_DS& theVoxels2) const
{
  // Check the voxels
  // Number of splits along X, Y and Z axes.
  if (!theVoxels1.GetNbX() && theVoxels1.GetNbX() != theVoxels2.GetNbX())
    return Standard_False;
  if (!theVoxels1.GetNbY() && theVoxels1.GetNbY() != theVoxels2.GetNbY())
    return Standard_False;
  if (!theVoxels1.GetNbZ() && theVoxels1.GetNbZ() != theVoxels2.GetNbZ())
    return Standard_False;
  // Start point
  if (fabs(theVoxels1.GetX() - theVoxels2.GetX()) > Precision::Confusion() ||
      fabs(theVoxels1.GetY() - theVoxels2.GetY()) > Precision::Confusion() ||
      fabs(theVoxels1.GetZ() - theVoxels2.GetZ()) > Precision::Confusion())
  {
    return Standard_False;
  }
  // Length along X, Y and Z axes.
  if (fabs(theVoxels1.GetXLen() - theVoxels2.GetXLen()) > Precision::Confusion() ||
      fabs(theVoxels1.GetYLen() - theVoxels2.GetYLen()) > Precision::Confusion() ||
      fabs(theVoxels1.GetZLen() - theVoxels2.GetZLen()) > Precision::Confusion())
  {
    return Standard_False;
  }
  return Standard_True;
}
