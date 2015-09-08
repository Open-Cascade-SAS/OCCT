// Created on: 2008-07-16
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


#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <Precision.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <Voxel_BoolDS.hxx>
#include <Voxel_CollisionDetection.hxx>
#include <Voxel_FastConverter.hxx>

Voxel_CollisionDetection::Voxel_CollisionDetection()
:myDeflection(0.1),
 myNbX(100),
 myNbY(100),
 myNbZ(100),
 myUsageOfVolume(Standard_False),
 myKeepCollisions(Standard_False),
 myXLen(-1.0),
 myYLen(-1.0),
 myZLen(-1.0),
 myVoxels(0),
 myHasCollisions(Standard_False)
{

}

Voxel_CollisionDetection::Voxel_CollisionDetection(const Standard_Real deflection,
						   const Standard_Integer nbx,
						   const Standard_Integer nby,
						   const Standard_Integer nbz)
:myDeflection(deflection),
 myNbX(nbx),
 myNbY(nby),
 myNbZ(nbz),
 myUsageOfVolume(Standard_False),
 myKeepCollisions(Standard_False),
 myXLen(-1.0),
 myYLen(-1.0),
 myZLen(-1.0),
 myVoxels(0),
 myHasCollisions(Standard_False)
{

}

// Destructor
void Voxel_CollisionDetection::Destroy()
{
  Clear();
}

Standard_Integer Voxel_CollisionDetection::AddShape(const TopoDS_Shape& shape)
{
  Clear();
  myShapes.Append(shape);
  return myShapes.Extent();
}

Standard_Boolean Voxel_CollisionDetection::ReplaceShape(const Standard_Integer ishape,
							const TopoDS_Shape& shape)
{
  if (ishape == 1)
  {
    myShapes.RemoveFirst();
    myShapes.Prepend(shape);
    return Standard_True;
  }

  Standard_Integer i = 1;
  Standard_Boolean is_replaced = Standard_False;
  TopTools_ListIteratorOfListOfShape itr(myShapes);
  for (; itr.More(); itr.Next(), i++)
  {
    if (i == ishape)
    {
      myShapes.Remove(itr);
      myShapes.InsertBefore(shape, itr);
      is_replaced = Standard_True;
      break;
    }
  }
  return is_replaced;
}

void Voxel_CollisionDetection::SetDeflection(const Standard_Real deflection)
{
  myDeflection = deflection;
}

void Voxel_CollisionDetection::SetNbVoxels(const Standard_Integer nbx,
					   const Standard_Integer nby,
					   const Standard_Integer nbz)
{
  myNbX = nbx;
  myNbY = nby;
  myNbZ = nbz;
}

void Voxel_CollisionDetection::SetBoundaryBox(const Bnd_Box& box)
{
  if (box.IsVoid())
    return;

  Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
  box.Get(xmin, ymin, zmin, xmax, ymax, zmax);
  
  myX = xmin;
  myY = ymin;
  myZ = zmin;

  myXLen = xmax - xmin;
  myYLen = ymax - ymin;
  myZLen = zmax - zmin;
}

void Voxel_CollisionDetection::SetUsageOfVolume(const Standard_Boolean usage)
{
  myUsageOfVolume = usage;
}

void Voxel_CollisionDetection::KeepCollisions(const Standard_Boolean keep)
{
  myKeepCollisions = keep;
}

Standard_Boolean Voxel_CollisionDetection::Voxelize(const Standard_Integer ishape)
{
  // Check the arguments
  if (myNbX <= 0 || myNbY <= 0 || myNbZ <= 0)
    return Standard_False;

  // Calculate the boundary box of the shapes to define the size of voxels.
  // This code is called only if the user didn't define the boundary box himself.
  if (myXLen < 0.0)
  {
    Bnd_Box B, b;
    TopTools_ListIteratorOfListOfShape itrs(myShapes);
    for (; itrs.More(); itrs.Next())
    {
      TopoDS_Shape S = itrs.Value();
      BRepBndLib::Add(S, b);
      B.Add(b);
    }
    SetBoundaryBox(B);
  }

  // Voxelize the shapes
  Standard_Integer progress, ithread = 1, i = 1;
  TopTools_ListIteratorOfListOfShape itrs(myShapes);
  for (; itrs.More(); itrs.Next(), i++)
  {
    if (ishape != -1 && i != ishape)
      continue;

    if (!myVoxels)
      myVoxels = (Standard_Address) new Voxel_BoolDS[myShapes.Extent()];
    Voxel_BoolDS& voxels = ((Voxel_BoolDS*)myVoxels)[i - 1];
    if (!CheckVoxels(voxels))
    {
      voxels.Init(myX, myY, myZ, myXLen, myYLen, myZLen, myNbX, myNbY, myNbZ);
    }
    else
    {
      voxels.SetZero();
    }

    TopoDS_Shape S = itrs.Value();
    Voxel_FastConverter voxelizer(S, voxels, myDeflection, myNbX, myNbY, myNbZ, 1 /*number of threads */);
    if (!voxelizer.Convert(progress, ithread))
      return Standard_False;
    if (myUsageOfVolume && !voxelizer.FillInVolume(1, ithread))
      return Standard_False;
  }

  return Standard_True;
}

Standard_Boolean Voxel_CollisionDetection::Compute()
{
  myHasCollisions = Standard_False;

  // Check voxels of shapes
  if (!myVoxels)
    return Standard_False;
  Standard_Integer ishape = 0, nb_shapes = myShapes.Extent();
  for (; ishape < nb_shapes; ishape++)
  {
    Voxel_BoolDS& voxels = ((Voxel_BoolDS*)myVoxels)[ishape];
    if (!CheckVoxels(voxels))
    {
      return Standard_False;
    }
  }

  // Check the resulting voxels
  Standard_Boolean created = Standard_False;
  if (!CheckVoxels(myCollisions))
  {
    // Create 0-voxels for the result, if it is needed.
    created = Standard_True;
    myCollisions.Init(myX, myY, myZ, myXLen, myYLen, myZLen, myNbX, myNbY, myNbZ);
  }

  // Nullify the voxels of the result (it corresponds to the state of no collisions).
  if (!myKeepCollisions && !created)
  {
    myCollisions.SetZero();
  }
  
  // Check collisions
  if (nb_shapes)
  {
    Standard_Integer ix, iy, iz;
    Voxel_BoolDS& voxels = ((Voxel_BoolDS*)myVoxels)[0]; // 1st shape
    for (ix = 0; ix < myNbX; ix++)
    {
      for (iy = 0; iy < myNbY; iy++)
      {
	for (iz = 0; iz < myNbZ; iz++)
	{
	  if (voxels.Get(ix, iy, iz))
	  {
	    for (ishape = 1; ishape < nb_shapes; ishape++) // start with second shape
	    {
	      Voxel_BoolDS& anOtherVoxels = ((Voxel_BoolDS*)myVoxels)[ishape];
	      if (anOtherVoxels.Get(ix, iy, iz))
	      {
		myCollisions.Set(ix, iy, iz, Standard_True);
		if (!myHasCollisions)
		{
		  myHasCollisions = Standard_True;
		}
		break;
	      }
	    }
	  }
	}
      }
    }
  }


  return Standard_True;
}

Standard_Boolean Voxel_CollisionDetection::HasCollisions() const
{
  return myHasCollisions;
}

const Voxel_BoolDS& Voxel_CollisionDetection::GetCollisions() const
{
  return myCollisions;
}

void Voxel_CollisionDetection::Clear()
{
  if (myVoxels)
  {
    delete[] ((Voxel_BoolDS*)myVoxels);
    myVoxels = 0;
  }
}

Standard_Boolean Voxel_CollisionDetection::CheckVoxels(const Voxel_BoolDS& voxels) const
{
  if (fabs(voxels.GetX() - myX) > Precision::Confusion() ||
      fabs(voxels.GetY() - myY) > Precision::Confusion() ||
      fabs(voxels.GetZ() - myZ) > Precision::Confusion() ||
      fabs(voxels.GetXLen() - myXLen) > Precision::Confusion() ||
      fabs(voxels.GetYLen() - myYLen) > Precision::Confusion() ||
      fabs(voxels.GetZLen() - myZLen) > Precision::Confusion() ||
      voxels.GetNbX() != myNbX ||
      voxels.GetNbY() != myNbY ||
      voxels.GetNbZ() != myNbZ)
  {
    return Standard_False;
  }
  return Standard_True;
}
