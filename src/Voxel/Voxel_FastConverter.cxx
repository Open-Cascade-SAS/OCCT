// File:	Voxel_FastConverter.cxx
// Created:	Mon May 30 17:55:14 2008
// Author:	Vladislav ROMASHKO
//		<vladislav.romashko@opencascade.com>

#include <Voxel_FastConverter.ixx>

#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepMesh.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>

#include <gp_Lin2d.hxx>
#include <gce_MakePln.hxx>

#include <ElSLib.hxx>
#include <Poly_Triangulation.hxx>
#include <IntAna2d_AnaIntersection.hxx>

// Printing the progress in stdout.
//#define CONV_DUMP

Voxel_FastConverter::Voxel_FastConverter(const TopoDS_Shape&    shape,
					 Voxel_ROctBoolDS&      voxels, 
					 const Standard_Real    deflection,
					 const Standard_Integer nbx,
					 const Standard_Integer nby,
					 const Standard_Integer nbz,
					 const Standard_Integer nbthreads)
:myShape(shape),myVoxels(&voxels),
 myDeflection(deflection),
 myNbX(nbx),myNbY(nby),myNbZ(nbz),
 myNbThreads(nbthreads),myIsBool(2),
 myNbTriangles(0)
{
  Init();
}

Voxel_FastConverter::Voxel_FastConverter(const TopoDS_Shape&    shape,
					 Voxel_BoolDS&          voxels, 
					 const Standard_Real    deflection,
					 const Standard_Integer nbx,
					 const Standard_Integer nby,
					 const Standard_Integer nbz,
					 const Standard_Integer nbthreads)
:myShape(shape),myVoxels(&voxels),
 myDeflection(deflection),
 myNbX(nbx),myNbY(nby),myNbZ(nbz),
 myNbThreads(nbthreads),myIsBool(1),
 myNbTriangles(0)
{
  Init();
}

Voxel_FastConverter::Voxel_FastConverter(const TopoDS_Shape&    shape,
					 Voxel_ColorDS&         voxels, 
					 const Standard_Real    deflection,
					 const Standard_Integer nbx,
					 const Standard_Integer nby,
					 const Standard_Integer nbz,
					 const Standard_Integer nbthreads)
:myShape(shape),myVoxels(&voxels),
 myDeflection(deflection),
 myNbX(nbx),myNbY(nby),myNbZ(nbz),
 myNbThreads(nbthreads),myIsBool(0),
 myNbTriangles(0)
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
  for (; expl.More(); expl.Next())
  {
    TopoDS_Face F = TopoDS::Face(expl.Current());
    Handle(Poly_Triangulation) T = BRep_Tool::Triangulation(F, L);
    if (T.IsNull() || fabs(T->Deflection() - myDeflection) > Precision::Confusion())
    {
      triangulate = Standard_True;
      break;
    }
  }

  // Re-create the triangulation.
  if (triangulate)
  {
    BRepMesh::Mesh(myShape, myDeflection);
  }

  // Compute the number of triangles.
  myNbTriangles = 0;
  expl.Init(myShape, TopAbs_FACE);
  for (; expl.More(); expl.Next())
  {
    TopoDS_Face F = TopoDS::Face(expl.Current());
    Handle(Poly_Triangulation) T = BRep_Tool::Triangulation(F, L);
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

  // Half of diagonal of a voxel
  Voxel_DS* ds = (Voxel_DS*) myVoxels;
  Standard_Real dx = ds->GetXLen() / (Standard_Real) ds->GetNbX(),
                dy = ds->GetYLen() / (Standard_Real) ds->GetNbY(),
                dz = ds->GetZLen() / (Standard_Real) ds->GetNbZ();
  Standard_Real hdiagonal = sqrt(dx * dx + dy * dy + dz * dz);
  hdiagonal /= 2.0;

  // Compute the scope of triangles for current thread
  Standard_Integer start_thread_triangle = 1, end_thread_triangle = myNbTriangles, ithread_triangle = 0;
  start_thread_triangle = (ithread - 1) * (myNbTriangles / myNbThreads) + 1;
  end_thread_triangle   = (ithread - 0) * (myNbTriangles / myNbThreads);

  // Convert
  TopLoc_Location L;
  Standard_Integer iprogress = 0, prev_progress = 0;
  Standard_Integer n1, n2, n3;
  Standard_Integer ixmin, iymin, izmin, ixmax, iymax, izmax;
  Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
  TopExp_Explorer expl(myShape, TopAbs_FACE);
  for (; expl.More(); expl.Next())
  {
    TopoDS_Face F = TopoDS::Face(expl.Current());
    Handle(Poly_Triangulation) T = BRep_Tool::Triangulation(F, L);

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
      if (ithread_triangle < start_thread_triangle || ithread_triangle > end_thread_triangle)
	continue;

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

Standard_Boolean Voxel_FastConverter::FillInVolume(const Standard_Byte inner,
						   const Standard_Integer ithread)
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
