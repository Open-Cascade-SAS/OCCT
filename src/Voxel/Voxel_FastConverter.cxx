// Created on: 2008-05-30
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
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <ElSLib.hxx>
#include <gce_MakePln.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <Poly_Triangulation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <Voxel_BoolDS.hxx>
#include <Voxel_ColorDS.hxx>
#include <Voxel_FastConverter.hxx>
#include <Voxel_ROctBoolDS.hxx>

// Printing the progress in stdout.
//#define CONV_DUMP
Voxel_FastConverter::Voxel_FastConverter(const TopoDS_Shape&    shape,
					 Voxel_ROctBoolDS&      voxels, 
					 const Standard_Real    deflection,
					 const Standard_Integer nbx,
					 const Standard_Integer nby,
					 const Standard_Integer nbz,
					 const Standard_Integer nbthreads,
					 const Standard_Boolean useExistingTriangulation)
:myShape(shape),myVoxels(&voxels),
 myDeflection(deflection),
 myIsBool(2),
 myNbX(nbx),myNbY(nby),myNbZ(nbz),
 myNbThreads(nbthreads),
 myNbTriangles(0),
 myUseExistingTriangulation(useExistingTriangulation)
{
  Init();
}

Voxel_FastConverter::Voxel_FastConverter(const TopoDS_Shape&    shape,
					 Voxel_BoolDS&          voxels, 
					 const Standard_Real    deflection,
					 const Standard_Integer nbx,
					 const Standard_Integer nby,
					 const Standard_Integer nbz,
					 const Standard_Integer nbthreads,
					 const Standard_Boolean useExistingTriangulation)
:myShape(shape),myVoxels(&voxels),
 myDeflection(deflection),
 myIsBool(1),
 myNbX(nbx),myNbY(nby),myNbZ(nbz),
 myNbThreads(nbthreads),
 myNbTriangles(0),
 myUseExistingTriangulation(useExistingTriangulation)
{
  Init();
}

Voxel_FastConverter::Voxel_FastConverter(const TopoDS_Shape&    shape,
					 Voxel_ColorDS&         voxels, 
					 const Standard_Real    deflection,
					 const Standard_Integer nbx,
					 const Standard_Integer nby,
					 const Standard_Integer nbz,
					 const Standard_Integer nbthreads,
					 const Standard_Boolean useExistingTriangulation)
:myShape(shape),myVoxels(&voxels),
 myDeflection(deflection),
 myIsBool(0),
 myNbX(nbx),myNbY(nby),myNbZ(nbz),
 myNbThreads(nbthreads),
 myNbTriangles(0),
 myUseExistingTriangulation(useExistingTriangulation)
{
  Init();
}

void Voxel_FastConverter::Init()
{
  if (myShape.IsNull())
    return;
  if (myNbThreads < 1)
    return;

  // Check number of splits.
  Voxel_DS* voxels = (Voxel_DS*) myVoxels;
  if (voxels->GetNbX() != myNbX || voxels->GetNbY() != myNbY || voxels->GetNbZ() != myNbZ)
  {
    // Compute boundary box of the shape
    Bnd_Box box;
    BRepBndLib::Add(myShape, box);

    // Define the voxel model by means of the boundary box of shape
    Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
    box.Get(xmin, ymin, zmin, xmax, ymax, zmax);

    // Initialize the voxels.
    if (myIsBool == 2)
      ((Voxel_ROctBoolDS*) voxels)->Init(xmin, ymin, zmin, xmax - xmin, ymax - ymin, zmax - zmin, myNbX, myNbY, myNbZ);
    else if (myIsBool == 1)
      ((Voxel_BoolDS*) voxels)->Init(xmin, ymin, zmin, xmax - xmin, ymax - ymin, zmax - zmin, myNbX, myNbY, myNbZ);
    else if (myIsBool == 0)
      ((Voxel_ColorDS*) voxels)->Init(xmin, ymin, zmin, xmax - xmin, ymax - ymin, zmax - zmin, myNbX, myNbY, myNbZ);
  }

  // Check presence of triangulation.
  TopLoc_Location L;
  Standard_Boolean triangulate = Standard_False;
  TopExp_Explorer expl(myShape, TopAbs_FACE);
  if(myUseExistingTriangulation == Standard_False)
  {
    for (; expl.More(); expl.Next())
    {
      const TopoDS_Face & F = TopoDS::Face(expl.Current());
      Handle(Poly_Triangulation) T = BRep_Tool::Triangulation(F, L);
      if (T.IsNull() || (T->Deflection() > myDeflection))
      {
        triangulate = Standard_True;
        break;
      }
    }
  }

  // Re-create the triangulation.
  if (triangulate)
  {
    BRepMesh_IncrementalMesh(myShape, myDeflection);
  }

  // Compute the number of triangles.
  myNbTriangles = 0;
  expl.Init(myShape, TopAbs_FACE);
  for (; expl.More(); expl.Next())
  {
    const TopoDS_Face & F = TopoDS::Face(expl.Current());
    Handle(Poly_Triangulation) T = BRep_Tool::Triangulation(F, L);
    if (T.IsNull() == Standard_False)
      myNbTriangles += T->NbTriangles();
  }
}

// Destructor
void Voxel_FastConverter::Destroy()
{

}

Standard_Boolean Voxel_FastConverter::Convert(Standard_Integer&      progress,
					      const Standard_Integer ithread)
{
  if (ithread == 1)
    progress = 0;
#ifdef CONV_DUMP
  if (ithread == 1)
    printf("Progress = %d   \r", progress);
#endif

  if (myNbX <= 0 || myNbY <= 0 || myNbZ <= 0)
    return Standard_False;

  if(myNbTriangles == 0)
    return Standard_False;

  // Half of diagonal of a voxel
  Voxel_DS* ds = (Voxel_DS*) myVoxels;
  Standard_Real dx = ds->GetXLen() / (Standard_Real) ds->GetNbX(),
                dy = ds->GetYLen() / (Standard_Real) ds->GetNbY(),
                dz = ds->GetZLen() / (Standard_Real) ds->GetNbZ();
  Standard_Real hdiagonal = sqrt(dx * dx + dy * dy + dz * dz);
  hdiagonal /= 2.0;

  // Compute the scope of triangles for current thread
  Standard_Integer start_thread_triangle = 1, end_thread_triangle = myNbTriangles, ithread_triangle = 0;
  if(myNbTriangles < myNbThreads)
  {
    if(ithread != 1)
      return Standard_False;
    //in case we're in thread one process all triangles
  }
  else
  {
    div_t division = div(myNbTriangles, myNbThreads);
    start_thread_triangle = (ithread - 1) * division.quot + 1;
    end_thread_triangle   = (ithread - 0) * division.quot;

    if(ithread == myNbThreads)
      end_thread_triangle += division.rem;
  }

  // Convert
  TopLoc_Location L;
  Standard_Integer iprogress = 0;
  Standard_Integer n1, n2, n3;
  Standard_Integer ixmin, iymin, izmin, ixmax, iymax, izmax;
  Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
  TopExp_Explorer expl(myShape, TopAbs_FACE);
  for (; expl.More(); expl.Next())
  {
    const TopoDS_Face & F = TopoDS::Face(expl.Current());
    Handle(Poly_Triangulation) T = BRep_Tool::Triangulation(F, L);
    if (T.IsNull())
      continue;

    gp_Trsf trsf;
    Standard_Boolean transform = !L.IsIdentity();
    if (transform)
      trsf = L.Transformation();

    const TColgp_Array1OfPnt& nodes = T->Nodes();
    const Poly_Array1OfTriangle& triangles = T->Triangles();
    Standard_Integer itriangle = triangles.Lower(), nb_triangles = triangles.Upper();
    for (; itriangle <= nb_triangles; itriangle++)
    {
      ithread_triangle++;
      if (ithread_triangle < start_thread_triangle )
        continue;
      if (ithread_triangle > end_thread_triangle)
      {
        if (ithread == 1)
          progress = 100;
#ifdef CONV_DUMP
        if (ithread == 1)
          printf("Progress = %d  \r", progress);
#endif
        return Standard_True;
      }

      const Poly_Triangle& t = triangles.Value(itriangle);
      t.Get(n1, n2, n3);
      gp_Pnt p1 = nodes.Value(n1);
      gp_Pnt p2 = nodes.Value(n2);
      gp_Pnt p3 = nodes.Value(n3);
      if (transform)
      {
        p1.Transform(trsf);
        p2.Transform(trsf);
        p3.Transform(trsf);
      }

      // Get boundary box of the triangle
      GetBndBox(p1, p2, p3, xmin, ymin, zmin, xmax, ymax, zmax);

      // Find the range of voxels inside the boudary box of the triangle.
      if (!ds->GetVoxel(xmin, ymin, zmin, ixmin, iymin, izmin))
	continue;
      if (!ds->GetVoxel(xmax, ymax, zmax, ixmax, iymax, izmax))
	continue;

      // Refuse voxels for whom distance from their center to plane of triangle is greater than half of diagonal.
      // Make a line from center of each voxel to the center of triangle and
      // compute intersection of the line with sides of triangle.
      // Refuse the voxel in case of intersection.
      gce_MakePln mkPlane(p1, p2, p3);
      if (!mkPlane.IsDone())
	continue;
      gp_Pln plane = mkPlane.Value();
      ComputeVoxelsNearTriangle(plane, p1, p2, p3, hdiagonal, ixmin, iymin, izmin, ixmax, iymax, izmax);

      // Progress
      if (ithread == 1)
      {
	iprogress++;
	progress = (Standard_Integer) ( (Standard_Real) iprogress / (Standard_Real) myNbTriangles * 100.0 );
      }
#ifdef CONV_DUMP
      if (ithread == 1 && prev_progress != progress)
      {
	printf("Progress = %d  \r", progress);
	prev_progress = progress;
      }
#endif

    } // iteration of triangles
  } // iteration of faces

  if (ithread == 1)
    progress = 100;
#ifdef CONV_DUMP
  if (ithread == 1)
    printf("Progress = %d  \r", progress);
#endif
  return Standard_True;
}

Standard_Boolean Voxel_FastConverter::ConvertUsingSAT(Standard_Integer&      progress,
                                                      const Standard_Integer ithread)
{
  if (ithread == 1)
    progress = 0;
#ifdef CONV_DUMP
  if (ithread == 1)
    printf("Progress = %d   \r", progress);
#endif

  if (myNbX <= 0 || myNbY <= 0 || myNbZ <= 0)
    return Standard_False;

  if(myNbTriangles == 0)
    return Standard_False;

  // Half of size of a voxel (also for Voxel_ROctBoolDS)
  Voxel_DS* ds = (Voxel_DS*) myVoxels;
  Standard_Real dx = ds->GetXLen() / (Standard_Real) ds->GetNbX(),
                dy = ds->GetYLen() / (Standard_Real) ds->GetNbY(),
                dz = ds->GetZLen() / (Standard_Real) ds->GetNbZ();
  gp_Pnt extents(dx/2.0, dy/2.0, dz/2.0);
  gp_Pnt extents2(dx/4.0, dy/4.0, dz/4.0);
  gp_Pnt extents4(dx/8.0, dy/8.0, dz/8.0);

  // Compute the scope of triangles for current thread
  Standard_Integer start_thread_triangle = 1, end_thread_triangle = myNbTriangles, ithread_triangle = 0;
  if(myNbTriangles < myNbThreads)
  {
    if(ithread != 1)
      return Standard_False;
    //in case we're in thread one process all triangles
  }
  else
  {
    div_t division = div(myNbTriangles, myNbThreads);
    start_thread_triangle = (ithread - 1) * division.quot + 1;
    end_thread_triangle   = (ithread - 0) * division.quot;

    if(ithread == myNbThreads)
      end_thread_triangle += division.rem;
  }

  // Convert
  TopLoc_Location L;
  Standard_Integer iprogress = 0;
  Standard_Integer n1, n2, n3;
  Standard_Integer ixmin, iymin, izmin, ixmax, iymax, izmax;
  Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
  TopExp_Explorer expl(myShape, TopAbs_FACE);
  for (; expl.More(); expl.Next())
  {
    const TopoDS_Face & F = TopoDS::Face(expl.Current());
    Handle(Poly_Triangulation) T = BRep_Tool::Triangulation(F, L);
    if (T.IsNull())
      continue;

    gp_Trsf trsf;
    Standard_Boolean transform = !L.IsIdentity();
    if (transform)
      trsf = L.Transformation();

    const TColgp_Array1OfPnt& nodes = T->Nodes();
    const Poly_Array1OfTriangle& triangles = T->Triangles();
    Standard_Integer itriangle = triangles.Lower(), nb_triangles = triangles.Upper();
    for (; itriangle <= nb_triangles; itriangle++)
    {
      ithread_triangle++;
      if (ithread_triangle < start_thread_triangle )
        continue;
      if (ithread_triangle > end_thread_triangle)
      {
        if (ithread == 1)
          progress = 100;
#ifdef CONV_DUMP
        if (ithread == 1)
          printf("Progress = %d  \r", progress);
#endif
        return Standard_True;
      }

      const Poly_Triangle& t = triangles.Value(itriangle);

      t.Get(n1, n2, n3);
      gp_Pnt p1 = nodes.Value(n1);
      gp_Pnt p2 = nodes.Value(n2);
      gp_Pnt p3 = nodes.Value(n3);
      if (transform)
      {
        p1.Transform(trsf);
        p2.Transform(trsf);
        p3.Transform(trsf);
      }

      // Get boundary box of the triangle
      GetBndBox(p1, p2, p3, xmin, ymin, zmin, xmax, ymax, zmax);

      // Find the range of voxels inside the boudary box of the triangle.
      if (!ds->GetVoxel(xmin, ymin, zmin, ixmin, iymin, izmin))
        continue;
      if (!ds->GetVoxel(xmax, ymax, zmax, ixmax, iymax, izmax))
        continue;

      // Perform triangle-box intersection to find the voxels resulting from the processed triangle.;
      // Using SAT theorem to quickly find the intersection.
      ComputeVoxelsNearTriangle(p1, p2, p3,
                                extents, extents2, extents4, 
                                ixmin, iymin, izmin, ixmax, iymax, izmax);

      // Progress
      if (ithread == 1)
      {
        iprogress++;
        progress = (Standard_Integer) ( (Standard_Real) iprogress / (Standard_Real) myNbTriangles * 100.0 );
      }
#ifdef CONV_DUMP
      if (ithread == 1 && prev_progress != progress)
      {
        printf("Progress = %d  \r", progress);
        prev_progress = progress;
      }
#endif

    } // iteration of triangles
  } // iteration of faces

  if (ithread == 1)
    progress = 100;
#ifdef CONV_DUMP
  if (ithread == 1)
    printf("Progress = %d  \r", progress);
#endif
  return Standard_True;
}

Standard_Boolean Voxel_FastConverter::FillInVolume(const Standard_Byte inner,
						   const Standard_Integer /*ithread*/)
{
  Voxel_DS* ds = (Voxel_DS*) myVoxels;
  Standard_Integer ix, iy, iz, nbx = ds->GetNbX(), nby = ds->GetNbY(), nbz = ds->GetNbZ();
  Standard_Boolean prev_surface, surface, volume;

  if (inner)
  {
    // Fill-in internal voxels by the value "inner"
    for (ix = 0; ix < nbx; ix++)
    {
      for (iy = 0; iy < nby; iy++)
      {
	// Check existence of volume.
	volume = Standard_False;
	surface = Standard_False;
	prev_surface = Standard_False;
	for (iz = 0; iz < nbz; iz++)
	{
	  surface = (myIsBool == 1) ? 
	    ((Voxel_BoolDS*)myVoxels)->Get(ix, iy, iz) == Standard_True : 
	      ((Voxel_ColorDS*)myVoxels)->Get(ix, iy, iz) > 0;
	  if (prev_surface && !surface)
	  {
	    volume = !volume;
	  }
	  prev_surface = surface;
	}
	if (volume)
	  continue;

	// Fill-in the volume.
	volume = Standard_False;
	surface = Standard_False;
	prev_surface = Standard_False;
	for (iz = 0; iz < nbz; iz++)
	{
	  surface = (myIsBool == 1) ? 
	    ((Voxel_BoolDS*)myVoxels)->Get(ix, iy, iz) == Standard_True : 
	      ((Voxel_ColorDS*)myVoxels)->Get(ix, iy, iz) > 0;
	  if (prev_surface && !surface)
	  {
	    volume = !volume;
	  }
	  if (volume && !surface)
	  {
	    (myIsBool == 1) ? ((Voxel_BoolDS*)myVoxels)->Set(ix, iy, iz, inner) : 
	      ((Voxel_ColorDS*)myVoxels)->Set(ix, iy, iz, inner);
	  }
	  prev_surface = surface;
	}
      }
    }
  }
  else
  {
    // Set value of interbal voxels to 0 ("inner" = 0)
    Standard_Boolean next_surface;
    for (ix = 0; ix < nbx; ix++)
    {
      for (iy = 0; iy < nby; iy++)
      {
	volume = Standard_False;
	surface = Standard_False;
	prev_surface = Standard_False;
	next_surface = Standard_False;
	for (iz = 0; iz < nbz; iz++)
	{
	  surface = (myIsBool == 1) ? 
	    ((Voxel_BoolDS*)myVoxels)->Get(ix, iy, iz) == Standard_True : 
	      ((Voxel_ColorDS*)myVoxels)->Get(ix, iy, iz) > 0;
	  if (prev_surface != surface)
	  {
	    volume = !volume;
	  }
	  if (volume && iz + 1 < nbz)
	  {
	    next_surface = (myIsBool == 1) ? 
	      ((Voxel_BoolDS*)myVoxels)->Get(ix, iy, iz + 1) == Standard_True : 
	      ((Voxel_ColorDS*)myVoxels)->Get(ix, iy, iz + 1) > 0;
	  }
	  if (volume && prev_surface == surface && next_surface)
	  {
	    (myIsBool == 1) ? ((Voxel_BoolDS*)myVoxels)->Set(ix, iy, iz, inner) : 
	      ((Voxel_ColorDS*)myVoxels)->Set(ix, iy, iz, inner);
	  }
	  prev_surface = surface;
	}
      }
    }
  }

  return Standard_True;
}

Standard_Boolean Voxel_FastConverter::FillInVolume(const Standard_Byte inner, const TopoDS_Shape & shape, const Standard_Integer /*ithread*/)
{
  Voxel_DS* ds = (Voxel_DS*) myVoxels;
  Standard_Integer ix, iy, iz, nbx = ds->GetNbX(), nby = ds->GetNbY(), nbz = ds->GetNbZ();
  Standard_Boolean prev_surface, surface, volume, isOnVerticalSurface;

  BRepClass3d_SolidClassifier solidClassifier(shape);
  Standard_Real xc, yc, zc;

  if (inner)
  {
    // Fill-in internal voxels by the value "inner"
    for (ix = 0; ix < nbx; ix++)
    {
      for (iy = 0; iy < nby; iy++)
      {
        // Check existence of volume.
        volume = Standard_False;
        surface = Standard_False;
        prev_surface = Standard_False;
        isOnVerticalSurface = Standard_False;
        for (iz = 0; iz < nbz; iz++)
        {
          surface = (myIsBool == 1) ? 
            ((Voxel_BoolDS*)myVoxels)->Get(ix, iy, iz) == Standard_True : 
              ((Voxel_ColorDS*)myVoxels)->Get(ix, iy, iz) > 0;
          if (prev_surface && !surface)
          {
            if(isOnVerticalSurface)
            {
              isOnVerticalSurface = Standard_False;
              ((Voxel_BoolDS*)myVoxels)->GetCenter(ix, iy, iz, xc, yc, zc);
              gp_Pnt P(xc, yc, zc);
              solidClassifier.Perform(P, Precision::Confusion());

              if(solidClassifier.State() == TopAbs_IN)
                volume = Standard_True;
              else
                volume = Standard_False;
            }
            else
              volume = !volume;
          }
          if(prev_surface && surface)
            isOnVerticalSurface = Standard_True;
          else
            isOnVerticalSurface = Standard_False;
          prev_surface = surface;
        }
        if (volume)
          continue;

        // Fill-in the volume.
        volume = Standard_False;
        surface = Standard_False;
        prev_surface = Standard_False;
        isOnVerticalSurface = Standard_False;
        for (iz = 0; iz < nbz; iz++)
        {
          surface = (myIsBool == 1) ? 
            ((Voxel_BoolDS*)myVoxels)->Get(ix, iy, iz) == Standard_True : 
              ((Voxel_ColorDS*)myVoxels)->Get(ix, iy, iz) > 0;
          if (prev_surface && !surface)
          {
            if(isOnVerticalSurface)
            {
              isOnVerticalSurface = Standard_False;
              ((Voxel_BoolDS*)myVoxels)->GetCenter(ix, iy, iz, xc, yc, zc);
              gp_Pnt P(xc, yc, zc);
              solidClassifier.Perform(P, Precision::Confusion());

              if(solidClassifier.State() == TopAbs_IN)
                volume = Standard_True;
              else
                volume = Standard_False;
            }
            else
              volume = !volume;
          }
          if (volume && !surface)
          {
            (myIsBool == 1) ? ((Voxel_BoolDS*)myVoxels)->Set(ix, iy, iz, inner) : 
              ((Voxel_ColorDS*)myVoxels)->Set(ix, iy, iz, inner);
          }
          if(prev_surface && surface)
            isOnVerticalSurface = Standard_True;
          else
            isOnVerticalSurface = Standard_False;
          prev_surface = surface;
        }
      }
    }
  }

  return Standard_True;
}

void Voxel_FastConverter::GetBndBox(const gp_Pnt&  p1,
				    const gp_Pnt&  p2,
				    const gp_Pnt&  p3,
				    Standard_Real& xmin,
				    Standard_Real& ymin,
				    Standard_Real& zmin,
				    Standard_Real& xmax,
				    Standard_Real& ymax,
				    Standard_Real& zmax) const
{
  // P1:
  xmin = p1.X();
  ymin = p1.Y();
  zmin = p1.Z();
  xmax = p1.X();
  ymax = p1.Y();
  zmax = p1.Z();
  // P2:
  if (xmin > p2.X())
    xmin = p2.X();
  if (ymin > p2.Y())
    ymin = p2.Y();
  if (zmin > p2.Z())
    zmin = p2.Z();
  if (xmax < p2.X())
    xmax = p2.X();
  if (ymax < p2.Y())
    ymax = p2.Y();
  if (zmax < p2.Z())
    zmax = p2.Z();
  // P3:
  if (xmin > p3.X())
    xmin = p3.X();
  if (ymin > p3.Y())
    ymin = p3.Y();
  if (zmin > p3.Z())
    zmin = p3.Z();
  if (xmax < p3.X())
    xmax = p3.X();
  if (ymax < p3.Y())
    ymax = p3.Y();
  if (zmax < p3.Z())
    zmax = p3.Z();
}

// This method is copied from Voxel_ShapeIntersector.cxx
static Standard_Boolean mayIntersect(const gp_Pnt2d& p11, const gp_Pnt2d& p12,
				     const gp_Pnt2d& p21, const gp_Pnt2d& p22)
{
    if (p11.X() > p21.X() && p11.X() > p22.X() && p12.X() > p21.X() && p12.X() > p22.X())
        return Standard_False;
    if (p11.X() < p21.X() && p11.X() < p22.X() && p12.X() < p21.X() && p12.X() < p22.X())
        return Standard_False;
    if (p11.Y() > p21.Y() && p11.Y() > p22.Y() && p12.Y() > p21.Y() && p12.Y() > p22.Y())
        return Standard_False;
    if (p11.Y() < p21.Y() && p11.Y() < p22.Y() && p12.Y() < p21.Y() && p12.Y() < p22.Y())
        return Standard_False;
    return Standard_True;
}

void Voxel_FastConverter::ComputeVoxelsNearTriangle(const gp_Pln&          plane,
						    const gp_Pnt&          p1,
						    const gp_Pnt&          p2,
						    const gp_Pnt&          p3,
						    const Standard_Real    hdiagonal,
						    const Standard_Integer ixmin,
						    const Standard_Integer iymin,
						    const Standard_Integer izmin,
						    const Standard_Integer ixmax,
						    const Standard_Integer iymax,
						    const Standard_Integer izmax) const
{
  gp_Pnt pc;
  Standard_Real xc, yc, zc, uc, vc, u1, v1, u2, v2, u3, v3;
  Standard_Integer ix, iy, iz;
  IntAna2d_AnaIntersection intersector2d;

  // Project points of triangle onto the plane
  ElSLib::Parameters(plane, p1, u1, v1);
  ElSLib::Parameters(plane, p2, u2, v2);
  ElSLib::Parameters(plane, p3, u3, v3);

  // Make lines of triangle
  gp_Pnt2d p2d1(u1, v1), p2d2(u2, v2), p2d3(u3, v3), p2dt((u1+u2+u3)/3.0,(v1+v2+v3)/3.0), p2dc;
  gp_Vec2d v2d12(p2d1, p2d2), v2d23(p2d2, p2d3), v2d31(p2d3, p2d1);
  gp_Lin2d L1(p2d1, v2d12), L2(p2d2, v2d23), L3(p2d3, v2d31), Lv;
  Standard_Real d1 = p2d1.Distance(p2d2) - Precision::Confusion(), 
                d2 = p2d2.Distance(p2d3) - Precision::Confusion(), 
                d3 = p2d3.Distance(p2d1) - Precision::Confusion(), dv;

  Voxel_DS* ds = (Voxel_DS*) myVoxels;
  for (ix = ixmin; ix <= ixmax; ix++)
  {
    for (iy = iymin; iy <= iymax; iy++)
    {
      for (iz = izmin; iz <= izmax; iz++)
      {
	ds->GetCenter(ix, iy, iz, xc, yc, zc);
	pc.SetCoord(xc, yc, zc);
	if (plane.Distance(pc) < hdiagonal)
	{
	  ElSLib::Parameters(plane, pc, uc, vc);
	  p2dc.SetCoord(uc, vc);

	  gp_Vec2d v2dct(p2dc, p2dt);
	  dv = v2dct.Magnitude() - Precision::Confusion();
	  Lv.SetLocation(p2dc);
	  Lv.SetDirection(v2dct);

	  // Side 1:
	  if (mayIntersect(p2d1, p2d2, p2dc, p2dt))
	  {
	    intersector2d.Perform(Lv, L1);
	    if (intersector2d.IsDone() && !intersector2d.ParallelElements() && intersector2d.NbPoints())
	    {
	      const IntAna2d_IntPoint& i2d = intersector2d.Point(1);
	      Standard_Real param1 = i2d.ParamOnFirst();
	      Standard_Real param2 = i2d.ParamOnSecond();
	      if (param1 > Precision::Confusion() && param1 < dv && 
		  param2 > Precision::Confusion() && param2 < d1)
	      {
		continue;
	      }    
	    }
	  }

	  // Side 2:
	  if (mayIntersect(p2d2, p2d3, p2dc, p2dt))
	  {
	    intersector2d.Perform(Lv, L2);
	    if (intersector2d.IsDone() && !intersector2d.ParallelElements() && intersector2d.NbPoints())
	    {
	      const IntAna2d_IntPoint& i2d = intersector2d.Point(1);
	      Standard_Real param1 = i2d.ParamOnFirst();
	      Standard_Real param2 = i2d.ParamOnSecond();
	      if (param1 > Precision::Confusion() && param1 < dv && 
		  param2 > Precision::Confusion() && param2 < d2)
	      {
		continue;
	      }    
	    }
	  }

	  // Side 3:
	  if (mayIntersect(p2d3, p2d1, p2dc, p2dt))
	  {
	    intersector2d.Perform(Lv, L3);
	    if (intersector2d.IsDone() && !intersector2d.ParallelElements() && intersector2d.NbPoints())
	    {
	      const IntAna2d_IntPoint& i2d = intersector2d.Point(1);
	      Standard_Real param1 = i2d.ParamOnFirst();
	      Standard_Real param2 = i2d.ParamOnSecond();
	      if (param1 > Precision::Confusion() && param1 < dv && 
		  param2 > Precision::Confusion() && param2 < d3)
	      {
		continue;
	      }    
	    }
	  }

	  // Set positive value to this voxel:
	  switch (myIsBool)
	  {
	    case 0:
	      ((Voxel_ColorDS*) myVoxels)->Set(ix, iy, iz, 15);
	      break;
	    case 1:
	      ((Voxel_BoolDS*) myVoxels)->Set(ix, iy, iz, Standard_True);
	      break;
	    case 2:
	    {
	      //((Voxel_ROctBoolDS*) myVoxels)->Set(ix, iy, iz, Standard_True);

	      // Check intersection between the triangle & sub-voxels of the voxel.
	      Standard_Real hdiagonal2 = hdiagonal / 2.0, hdiagonal4 = hdiagonal / 4.0;
	      for (Standard_Integer i = 0; i < 8; i++)
	      {
		((Voxel_ROctBoolDS*) myVoxels)->GetCenter(ix, iy, iz, i, xc, yc, zc);
		pc.SetCoord(xc, yc, zc);
		if (plane.Distance(pc) < hdiagonal2)
		{
		  ElSLib::Parameters(plane, pc, uc, vc);
		  p2dc.SetCoord(uc, vc);

		  gp_Vec2d aVec2dct1(p2dc, p2dt);
		  dv = aVec2dct1.Magnitude() - Precision::Confusion();
		  Lv.SetLocation(p2dc);
		  Lv.SetDirection(aVec2dct1);

		  // Side 1:
		  if (mayIntersect(p2d1, p2d2, p2dc, p2dt))
		  {
		    intersector2d.Perform(Lv, L1);
		    if (intersector2d.IsDone() && !intersector2d.ParallelElements() && intersector2d.NbPoints())
		    {
		      const IntAna2d_IntPoint& i2d = intersector2d.Point(1);
		      Standard_Real param1 = i2d.ParamOnFirst();
		      Standard_Real param2 = i2d.ParamOnSecond();
		      if (param1 > Precision::Confusion() && param1 < dv && 
			  param2 > Precision::Confusion() && param2 < d1)
		      {
			continue;
		      }    
		    }
		  }

		  // Side 2:
		  if (mayIntersect(p2d2, p2d3, p2dc, p2dt))
		  {
		    intersector2d.Perform(Lv, L2);
		    if (intersector2d.IsDone() && !intersector2d.ParallelElements() && intersector2d.NbPoints())
		    {
		      const IntAna2d_IntPoint& i2d = intersector2d.Point(1);
		      Standard_Real param1 = i2d.ParamOnFirst();
		      Standard_Real param2 = i2d.ParamOnSecond();
		      if (param1 > Precision::Confusion() && param1 < dv && 
			  param2 > Precision::Confusion() && param2 < d2)
		      {
			continue;
		      }    
		    }
		  }

		  // Side 3:
		  if (mayIntersect(p2d3, p2d1, p2dc, p2dt))
		  {
		    intersector2d.Perform(Lv, L3);
		    if (intersector2d.IsDone() && !intersector2d.ParallelElements() && intersector2d.NbPoints())
		    {
		      const IntAna2d_IntPoint& i2d = intersector2d.Point(1);
		      Standard_Real param1 = i2d.ParamOnFirst();
		      Standard_Real param2 = i2d.ParamOnSecond();
		      if (param1 > Precision::Confusion() && param1 < dv && 
			  param2 > Precision::Confusion() && param2 < d3)
		      {
			continue;
		      }    
		    }
		  }

		  //((Voxel_ROctBoolDS*) myVoxels)->Set(ix, iy, iz, i, Standard_True);

		  // Check intersection between the triangle & sub-voxels of the sub-voxel.
		  for (Standard_Integer j = 0; j < 8; j++)
		  {
		    ((Voxel_ROctBoolDS*) myVoxels)->GetCenter(ix, iy, iz, i, j, xc, yc, zc);
		    pc.SetCoord(xc, yc, zc);
		    if (plane.Distance(pc) < hdiagonal4)
		    {
		      ElSLib::Parameters(plane, pc, uc, vc);
		      p2dc.SetCoord(uc, vc);

		      gp_Vec2d aVec2dct2(p2dc, p2dt);
		      dv = aVec2dct2.Magnitude() - Precision::Confusion();
		      Lv.SetLocation(p2dc);
		      Lv.SetDirection(aVec2dct2);
		      
		      // Side 1:
		      if (mayIntersect(p2d1, p2d2, p2dc, p2dt))
		      {
			intersector2d.Perform(Lv, L1);
			if (intersector2d.IsDone() && !intersector2d.ParallelElements() && intersector2d.NbPoints())
			{
			  const IntAna2d_IntPoint& i2d = intersector2d.Point(1);
			  Standard_Real param1 = i2d.ParamOnFirst();
			  Standard_Real param2 = i2d.ParamOnSecond();
			  if (param1 > Precision::Confusion() && param1 < dv && 
			      param2 > Precision::Confusion() && param2 < d1)
			  {
			    continue;
			  }    
			}
		      }
		      
		      // Side 2:
		      if (mayIntersect(p2d2, p2d3, p2dc, p2dt))
		      {
			intersector2d.Perform(Lv, L2);
			if (intersector2d.IsDone() && !intersector2d.ParallelElements() && intersector2d.NbPoints())
			{
			  const IntAna2d_IntPoint& i2d = intersector2d.Point(1);
			  Standard_Real param1 = i2d.ParamOnFirst();
			  Standard_Real param2 = i2d.ParamOnSecond();
			  if (param1 > Precision::Confusion() && param1 < dv && 
			      param2 > Precision::Confusion() && param2 < d2)
			  {
			    continue;
			  }    
			}
		      }

		      // Side 3:
		      if (mayIntersect(p2d3, p2d1, p2dc, p2dt))
		      {
			intersector2d.Perform(Lv, L3);
			if (intersector2d.IsDone() && !intersector2d.ParallelElements() && intersector2d.NbPoints())
			{
			  const IntAna2d_IntPoint& i2d = intersector2d.Point(1);
			  Standard_Real param1 = i2d.ParamOnFirst();
			  Standard_Real param2 = i2d.ParamOnSecond();
			  if (param1 > Precision::Confusion() && param1 < dv && 
			      param2 > Precision::Confusion() && param2 < d3)
			  {
			    continue;
			  }    
			}
		      }

		      ((Voxel_ROctBoolDS*) myVoxels)->Set(ix, iy, iz, i, j, Standard_True);
		    }
		  } // End of "Check level 2".

		}
	      } // End of "Check level 1".
 
	      break;
	    }
	  }
	}
      }
    }
  }
}

//! This macro quickly finds the min & max values among 3 variables
#define FINDMINMAX(x0, x1, x2, min, max)	\
  min = max = x0;                               \
  if(x1<min) min=x1;                            \
  if(x1>max) max=x1;                            \
  if(x2<min) min=x2;                            \
  if(x2>max) max=x2;

static bool planeBoxOverlap(const gp_Vec & normal, const double d, const gp_Pnt & maxbox)
{
  gp_Vec vmin, vmax;
  if(normal.X() > 0.0)    { vmin.SetX(-maxbox.X()); vmax.SetX(maxbox.X()); }
  else                    { vmin.SetX(maxbox.X());  vmax.SetX(-maxbox.X()); }
  
  if(normal.Y() > 0.0)    { vmin.SetY(-maxbox.Y()); vmax.SetY(maxbox.Y()); }
  else                    { vmin.SetY(maxbox.Y());  vmax.SetY(-maxbox.Y()); }
  
  if(normal.Z() > 0.0)    { vmin.SetZ(-maxbox.Z()); vmax.SetZ(maxbox.Z()); }
  else                    { vmin.SetZ(maxbox.Z());  vmax.SetZ(-maxbox.Z()); }
  
  if((normal.Dot(vmin)) + d > 0.0) return false;
  if((normal.Dot(vmax)) + d>= 0.0) return true;
  
  return false;
}

#define AXISTEST_X01(a, b, fa, fb)                            \
    min = a*v0.Y() - b*v0.Z();                                    \
    max = a*v2.Y() - b*v2.Z();                                    \
    if(min>max) {const double tmp=max; max=min; min=tmp;    }    \
    rad = fa * extents.Y() + fb * extents.Z();                    \
    if(min>rad || max<-rad) return false;

#define AXISTEST_X2(a, b, fa, fb)                            \
    min = a*v0.Y() - b*v0.Z();                                    \
    max = a*v1.Y() - b*v1.Z();                                    \
    if(min>max) {const double tmp=max; max=min; min=tmp;    }    \
    rad = fa * extents.Y() + fb * extents.Z();                    \
    if(min>rad || max<-rad) return false;

#define AXISTEST_Y02(a, b, fa, fb)                            \
    min = b*v0.Z() - a*v0.X();                                    \
    max = b*v2.Z() - a*v2.X();                                    \
    if(min>max) {const double tmp=max; max=min; min=tmp;    }    \
    rad = fa * extents.X() + fb * extents.Z();                    \
    if(min>rad || max<-rad) return false;

#define AXISTEST_Y1(a, b, fa, fb)                            \
    min = b*v0.Z() - a*v0.X();                                    \
    max = b*v1.Z() - a*v1.X();                                    \
    if(min>max) {const double tmp=max; max=min; min=tmp;    }    \
    rad = fa * extents.X() + fb * extents.Z();                    \
    if(min>rad || max<-rad) return false;

#define AXISTEST_Z12(a, b, fa, fb)                            \
    min = a*v1.X() - b*v1.Y();                                    \
    max = a*v2.X() - b*v2.Y();                                    \
    if(min>max) {const double tmp=max; max=min; min=tmp;    }    \
    rad = fa * extents.X() + fb * extents.Y();                    \
    if(min>rad || max<-rad) return false;

#define AXISTEST_Z0(a, b, fa, fb)                            \
    min = a*v0.X() - b*v0.Y();                                    \
    max = a*v1.X() - b*v1.Y();                                    \
    if(min>max) {const double tmp=max; max=min; min=tmp;    }    \
    rad = fa * extents.X() + fb * extents.Y();                    \
    if(min>rad || max<-rad) return false;

// compute triangle edges
// - edges lazy evaluated to take advantage of early exits
// - fabs precomputed (half less work, possible since extents are always >0)
// - customized macros to take advantage of the null component
// - axis vector discarded, possibly saves useless movs
#define IMPLEMENT_CLASS3_TESTS                        \
  double rad;                                         \
  double min, max;                                    \
                                                      \
  const double fey0 = fabs(e0.Y());                   \
  const double fez0 = fabs(e0.Z());                   \
  AXISTEST_X01(e0.Z(), e0.Y(), fez0, fey0);           \
  const double fex0 = fabs(e0.X());                   \
  AXISTEST_Y02(e0.Z(), e0.X(), fez0, fex0);           \
  AXISTEST_Z12(e0.Y(), e0.X(), fey0, fex0);           \
                                                      \
  const double fey1 = fabs(e1.Y());                   \
  const double fez1 = fabs(e1.Z());                   \
  AXISTEST_X01(e1.Z(), e1.Y(), fez1, fey1);           \
  const double fex1 = fabs(e1.X());                   \
  AXISTEST_Y02(e1.Z(), e1.X(), fez1, fex1);           \
  AXISTEST_Z0(e1.Y(), e1.X(), fey1, fex1);            \
                                                      \
  const gp_Vec e2 = v2 - v0;                          \
  const double fey2 = fabs(e2.Y());                   \
  const double fez2 = fabs(e2.Z());                   \
  AXISTEST_X2(e2.Z(), e2.Y(), fez2, fey2);            \
  const double fex2 = fabs(e2.X());                   \
  AXISTEST_Y1(e2.Z(), e2.X(), fez2, fex2);            \
  AXISTEST_Z12(e2.Y(), e2.X(), fey2, fex2);

static bool TriBoxOverlap(const gp_Pnt & p1, const gp_Pnt & p2, const gp_Pnt & p3,
                          const gp_Pnt & center, const gp_Pnt & extents)
{
  // use separating axis theorem to test overlap between triangle and box 
  // need to test for overlap in these directions: 
  // 1) the {x,y,z}-directions (actually, since we use the AABB of the triangle 
  //    we do not even need to test these) 
  // 2) normal of the triangle 
  // 3) crossproduct(edge from tri, {x,y,z}-directin) 
  //    this gives 3x3=9 more tests 
  
  // move everything so that the boxcenter is in (0,0,0)
  gp_Vec v0(center, p1);
  gp_Vec v1(center, p2);
  gp_Vec v2(center, p3);
  
  // First, test overlap in the {x,y,z}-directions
  double aMin,aMax;
  // Find min, max of the triangle in x-direction, and test for overlap in X
  FINDMINMAX(v0.X(), v1.X(), v2.X(), aMin, aMax);
  if(aMin>extents.X() || aMax<-extents.X()) return false;
  
  // Same for Y
  FINDMINMAX(v0.Y(), v1.Y(), v2.Y(), aMin, aMax);
  if(aMin>extents.Y() || aMax<-extents.Y()) return false;
  
  // Same for Z
  FINDMINMAX(v0.Z(), v1.Z(), v2.Z(), aMin, aMax);
  if(aMin>extents.Z() || aMax<-extents.Z()) return false;
  
  // 2) Test if the box intersects the plane of the triangle
  // compute plane equation of triangle: normal*x+d=0
  // ### could be precomputed since we use the same leaf triangle several times
  const gp_Vec e0 = v1 - v0;
  const gp_Vec e1 = v2 - v1;
  const gp_Vec normal = e0.Crossed(e1);
  const double d = -normal.Dot(v0);
  if(!planeBoxOverlap(normal, d, extents)) return false;
  
  // 3) "Class III" tests
  //if(mFullPrimBoxTest)
  {
    IMPLEMENT_CLASS3_TESTS
  }
  
  return true;
}

void Voxel_FastConverter::ComputeVoxelsNearTriangle(const gp_Pnt& p1,
                                                    const gp_Pnt& p2,
                                                    const gp_Pnt& p3,
                                                    const gp_Pnt& extents,
                                                    const gp_Pnt& extents2,
                                                    const gp_Pnt& extents4,
                                                    const Standard_Integer ixmin,
                                                    const Standard_Integer iymin,
                                                    const Standard_Integer izmin,
                                                    const Standard_Integer ixmax,
                                                    const Standard_Integer iymax,
                                                    const Standard_Integer izmax) const
{
  gp_Pnt pc;
  Standard_Real xc, yc, zc;
  Standard_Integer ix, iy, iz;

  Voxel_DS* ds = (Voxel_DS*) myVoxels;
  for (ix = ixmin; ix <= ixmax; ix++)
  {
    for (iy = iymin; iy <= iymax; iy++)
    {
      for (iz = izmin; iz <= izmax; iz++)
      {
        ds->GetCenter(ix, iy, iz, xc, yc, zc);
        pc.SetCoord(xc, yc, zc);

        if(TriBoxOverlap(p1, p2, p3, pc, extents))
        {
          // Set positive value to this voxel:
          switch (myIsBool)
          {
            case 0:
              ((Voxel_ColorDS*) myVoxels)->Set(ix, iy, iz, 15);
              break;
            case 1:
              ((Voxel_BoolDS*) myVoxels)->Set(ix, iy, iz, Standard_True);
              break;
            case 2:
            {
              //((Voxel_ROctBoolDS*) myVoxels)->Set(ix, iy, iz, Standard_True);

              // Check intersection between the triangle & sub-voxels of the voxel.
              for (Standard_Integer i = 0; i < 8; i++)
              {
                ((Voxel_ROctBoolDS*) myVoxels)->GetCenter(ix, iy, iz, i, xc, yc, zc);
                pc.SetCoord(xc, yc, zc);
                if(TriBoxOverlap(p1, p2, p3, pc, extents2))
                {
                  //((Voxel_ROctBoolDS*) myVoxels)->Set(ix, iy, iz, i, Standard_True);

                  // Check intersection between the triangle & sub-voxels of the sub-voxel.
                  for (Standard_Integer j = 0; j < 8; j++)
                  {
                    ((Voxel_ROctBoolDS*) myVoxels)->GetCenter(ix, iy, iz, i, j, xc, yc, zc);
                    pc.SetCoord(xc, yc, zc);
                    if(TriBoxOverlap(p1, p2, p3, pc, extents4))
                    {
                      ((Voxel_ROctBoolDS*) myVoxels)->Set(ix, iy, iz, i, j, Standard_True);
                    }
                  } // End of "Check level 2".

                }
              } // End of "Check level 1".
              break;
            }
          }
        }
      }
    }
  }
}
