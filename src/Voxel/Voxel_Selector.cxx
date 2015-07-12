// Created on: 2008-07-30
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


#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <IntAna_Quadric.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <V3d_View.hxx>
#include <Voxel_BoolDS.hxx>
#include <Voxel_ColorDS.hxx>
#include <Voxel_ROctBoolDS.hxx>
#include <Voxel_Selector.hxx>

Voxel_Selector::Voxel_Selector():myVoxels(0)
{

}

Voxel_Selector::Voxel_Selector(const Handle(V3d_View)& view):myView(view),myVoxels(0)
{

}

void Voxel_Selector::Init(const Handle(V3d_View)& view)
{
  myView = view;
}

void Voxel_Selector::SetVoxels(const Voxel_BoolDS& voxels)
{
  myIsBool = 1;
  myVoxels = (void*) &voxels;
}

void Voxel_Selector::SetVoxels(const Voxel_ColorDS& voxels)
{
  myIsBool = 0;
  myVoxels = (void*) &voxels;
}

void Voxel_Selector::SetVoxels(const Voxel_ROctBoolDS& voxels)
{
  myIsBool = 2;
  myVoxels = (void*) &voxels;
}

// This function is copied from ViewerTest_RelationCommands.cxx
static Standard_Boolean ComputeIntersection(const gp_Lin& L,const gp_Pln& ThePl, gp_Pnt& TheInter)
{
  static IntAna_Quadric TheQuad;
  TheQuad.SetQuadric(ThePl);
  static IntAna_IntConicQuad QQ;
  QQ.Perform(L,TheQuad);
  if(QQ.IsDone()){
    if(QQ.NbPoints()>0){
      TheInter = QQ.Point(1);
      return Standard_True;
    }
  }
  return Standard_False;
}

static inline Standard_Integer GetIVoxel(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz, 
					 const Standard_Integer nbx, const Standard_Integer nbxy)
{
  return ix + iy * nbx + iz * nbxy;
}

static inline Standard_Boolean Get(const Standard_Address voxels, const Standard_Integer isBool,
				   const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz)
{
  switch (isBool)
  {
    case 0:
      return ((Voxel_ColorDS*) voxels)->Get(ix, iy, iz) > 0;
    case 1:
      return ((Voxel_BoolDS*) voxels)->Get(ix, iy, iz);
    case 2:
    {
      Standard_Integer deepness = ((Voxel_ROctBoolDS*) voxels)->Deepness(ix, iy, iz);
      switch (deepness)
      {
	case 0:
	  return ((Voxel_ROctBoolDS*) voxels)->Get(ix, iy, iz);
	case 1:
	{
	  for (Standard_Integer i = 0; i < 8; i++)
	  {
	    if (((Voxel_ROctBoolDS*) voxels)->Get(ix, iy, iz, i) == Standard_True)
	      return Standard_True;
	  }
	  break;
	}
	case 2:
	{
	  for (Standard_Integer i = 0; i < 8; i++)
	  {
	    for (Standard_Integer j = 0; j < 8; j++)
	    {
	      if (((Voxel_ROctBoolDS*) voxels)->Get(ix, iy, iz, i, j) == Standard_True)
		return Standard_True;
	    }
	  }
	  break;
	}
      }
    }
  }
  return Standard_False;
}

Standard_Boolean Voxel_Selector::Detect(const Standard_Integer winx, const Standard_Integer winy,
					Standard_Integer& ixdetect, Standard_Integer& iydetect, Standard_Integer& izdetect)
{
  ixdetect = -1; iydetect = -1; izdetect = -1;
  if (myView.IsNull() || !myVoxels)
    return Standard_False;

  Voxel_DS* ds = 0;
  switch (myIsBool)
  {
    case 0:
      ds = (Voxel_ColorDS*) myVoxels;
      break;
    case 1:
      ds = (Voxel_BoolDS*) myVoxels;
      break;
    case 2:
      ds = (Voxel_ROctBoolDS*) myVoxels;
      break;
  }
  Standard_Integer nbx = ds->GetNbX(), nby = ds->GetNbY(), nbz = ds->GetNbZ(), nbxy = nbx * nby;

  // Construct a line perpendicular to the screen
  Standard_Real eyex, eyey, eyez, nx, ny, nz;
  myView->Convert(winx, winy, eyex, eyey, eyez);
  myView->Proj(nx, ny, nz);
  gp_Pnt peye(eyex, eyey, eyez);
  gp_Lin line(peye, gp_Dir(nx, ny, nz));

  // Find the first voxel meeting the line at entrance to the cube of voxels.
  // Construct planes of the cube of voxels
  Standard_Real xstart = ds->GetX(), ystart = ds->GetY(), zstart = ds->GetZ();
  Standard_Real xlen = ds->GetXLen(), ylen = ds->GetYLen(), zlen = ds->GetZLen();
  Standard_Real xend = xstart + xlen, yend = ystart + ylen, zend = zstart + zlen;
  gp_Pln xplane_minus(gp_Pnt(xstart, ystart, zstart), -gp::DX());
  gp_Pln xplane_plus (gp_Pnt(xend,   ystart, zstart),  gp::DX());
  gp_Pln yplane_minus(gp_Pnt(xstart, ystart, zstart), -gp::DY());
  gp_Pln yplane_plus (gp_Pnt(xstart, yend,   zstart),  gp::DY());
  gp_Pln zplane_minus(gp_Pnt(xstart, ystart, zstart), -gp::DZ());
  gp_Pln zplane_plus (gp_Pnt(xstart, ystart, zend),    gp::DZ());
  // Intersect the planes with the line.
  gp_Pnt pintersection, p;
  Standard_Real depth = DBL_MAX, d;
  Standard_Integer iplane = -1; // not found
  if (ComputeIntersection(line, xplane_minus, p)) // -X
  {
    if (p.Y() >= ystart && p.Y() <= yend &&
	p.Z() >= zstart && p.Z() <= zend)
    {
      p.SetX(xstart);
      depth = peye.SquareDistance(p);
      iplane = 0;
      pintersection = p;
    }
  }
  if (ComputeIntersection(line, xplane_plus, p)) // +X
  {
    if (p.Y() >= ystart && p.Y() <= yend &&
	p.Z() >= zstart && p.Z() <= zend)
    {
      d = peye.SquareDistance(p);
      if (d < depth)
      {
	p.SetX(xend);
	depth = d;
	iplane = 1;
	pintersection = p;
      }
    }
  }
  if (ComputeIntersection(line, yplane_minus, p)) // -Y
  {
    if (p.X() >= xstart && p.X() <= xend &&
	p.Z() >= zstart && p.Z() <= zend)
    {
      d = peye.SquareDistance(p);
      if (d < depth)
      {
	p.SetY(ystart);
	depth = d;
	iplane = 2;
	pintersection = p;
      }
    }
  }
  if (ComputeIntersection(line, yplane_plus, p)) // +Y
  {
    if (p.X() >= xstart && p.X() <= xend &&
	p.Z() >= zstart && p.Z() <= zend)
    {
      d = peye.SquareDistance(p);
      if (d < depth)
      {
	p.SetY(yend);
	depth = d;
	iplane = 3;
	pintersection = p;
      }
    }
  }
  if (ComputeIntersection(line, zplane_minus, p)) // -Z
  {
    if (p.X() >= xstart && p.X() <= xend &&
	p.Y() >= ystart && p.Y() <= yend)
    {
      d = peye.SquareDistance(p);
      if (d < depth)
      {
	p.SetZ(zstart);
	depth = d;
	iplane = 4;
	pintersection = p;
      }
    }
  }
  if (ComputeIntersection(line, zplane_plus, p)) // +Z
  {
    if (p.X() >= xstart && p.X() <= xend &&
	p.Y() >= ystart && p.Y() <= yend)
    {
      d = peye.SquareDistance(p);
      if (d < depth)
      {
	p.SetZ(zend);
	depth = d;
	iplane = 5;
	pintersection = p;
      }
    }
  }
  // Find the voxel on the detected plane
  if (iplane == -1)
    return Standard_False;
  Standard_Integer ix, iy, iz;
  if (!ds->GetVoxel(pintersection.X(), pintersection.Y(), pintersection.Z(), ix, iy, iz))
    return Standard_False;
  ixdetect = ix; iydetect = iy; izdetect = iz;

  // Find a non-zero voxel at the line
  Standard_Real xmin = xlen / Standard_Real(nbx),
                ymin = ylen / Standard_Real(nby),
                zmin = zlen / Standard_Real(nbz),
                vmin = sqrt(xmin * xmin + ymin * ymin + zmin * zmin) / 2.0;
  Standard_Real xc, yc, zc, dist, distmin = DBL_MAX;
  TColStd_MapOfInteger passed;
  while (!Get(myVoxels, myIsBool, ixdetect, iydetect, izdetect))
  {
    // Memorize already checked voxels
    if (!passed.Add(GetIVoxel(ixdetect, iydetect, izdetect, nbx, nbxy)))
      return Standard_False;

    distmin = DBL_MAX;
    ix = ixdetect; iy = iydetect; iz = izdetect;

    //1: -X neighbour
    if (ix - 1 >= 0 && !passed.Contains(GetIVoxel(ix - 1, iy, iz, nbx, nbxy)))
    {
      ds->GetCenter(ix - 1, iy, iz, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix - 1; iydetect = iy; izdetect = iz;
	distmin = dist;
      }
    }
    //2: +X neighbour
    if (ix + 1 < nbx && !passed.Contains(GetIVoxel(ix + 1, iy, iz, nbx, nbxy)))
    {
      ds->GetCenter(ix + 1, iy, iz, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix + 1; iydetect = iy; izdetect = iz;
	distmin = dist;
      }
    }
    //3: -Y neighbour
    if (iy - 1 >= 0 && !passed.Contains(GetIVoxel(ix, iy - 1, iz, nbx, nbxy)))
    {
      ds->GetCenter(ix, iy - 1, iz, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix; iydetect = iy - 1; izdetect = iz;
	distmin = dist;
      }
    }
    //4: +Y neighbour
    if (iy + 1 < nby && !passed.Contains(GetIVoxel(ix, iy + 1, iz, nbx, nbxy)))
    {
      ds->GetCenter(ix, iy + 1, iz, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix; iydetect = iy + 1; izdetect = iz;
	distmin = dist;
      }
    }
    //5: -Z neighbour
    if (iz - 1 >= 0 && !passed.Contains(GetIVoxel(ix, iy, iz - 1, nbx, nbxy)))
    {
      ds->GetCenter(ix, iy, iz - 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix; iydetect = iy; izdetect = iz - 1;
	distmin = dist;
      }
    }
    //6: +Z neighbour
    if (iz + 1 < nbz && !passed.Contains(GetIVoxel(ix, iy, iz + 1, nbx, nbxy)))
    {
      ds->GetCenter(ix, iy, iz + 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix; iydetect = iy; izdetect = iz + 1;
	distmin = dist;
      }
    }

    // Diagonal voxels
    //7: -X-Y neighbour
    if (ix - 1 >= 0 && iy - 1 >= 0 && !passed.Contains(GetIVoxel(ix - 1, iy - 1, iz, nbx, nbxy)))
    {
      ds->GetCenter(ix - 1, iy - 1, iz, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix - 1; iydetect = iy - 1; izdetect = iz;
	distmin = dist;
      }
    }
    //8: -X-Z neighbour
    if (ix - 1 >= 0 && iz - 1 >= 0 && !passed.Contains(GetIVoxel(ix - 1, iy, iz - 1, nbx, nbxy)))
    {
      ds->GetCenter(ix - 1, iy, iz - 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix - 1; iydetect = iy; izdetect = iz - 1;
	distmin = dist;
      }
    }
    //9: -Y-Z neighbour
    if (iy - 1 >= 0 && iz - 1 >= 0 && !passed.Contains(GetIVoxel(ix, iy - 1, iz - 1, nbx, nbxy)))
    {
      ds->GetCenter(ix, iy - 1, iz - 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix; iydetect = iy - 1; izdetect = iz - 1;
	distmin = dist;
      }
    }

    //10: +X-Y neighbour
    if (ix + 1 < nbx && iy - 1 >= 0 && !passed.Contains(GetIVoxel(ix + 1, iy - 1, iz, nbx, nbxy)))
    {
      ds->GetCenter(ix + 1, iy - 1, iz, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix + 1; iydetect = iy - 1; izdetect = iz;
	distmin = dist;
      }
    }
    //11: +X-Z neighbour
    if (ix + 1 < nbx && iz - 1 >= 0 && !passed.Contains(GetIVoxel(ix + 1, iy, iz - 1, nbx, nbxy)))
    {
      ds->GetCenter(ix + 1, iy, iz - 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix + 1; iydetect = iy; izdetect = iz - 1;
	distmin = dist;
      }
    }
    //12: +Y-Z neighbour
    if (iy + 1 < nby && iz - 1 >= 0 && !passed.Contains(GetIVoxel(ix, iy + 1, iz - 1, nbx, nbxy)))
    {
      ds->GetCenter(ix, iy + 1, iz - 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix; iydetect = iy + 1; izdetect = iz - 1;
	distmin = dist;
      }
    }

    //13: -X+Y neighbour
    if (ix - 1 >= 0 && iy + 1 < nby && !passed.Contains(GetIVoxel(ix - 1, iy + 1, iz, nbx, nbxy)))
    {
      ds->GetCenter(ix - 1, iy + 1, iz, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix - 1; iydetect = iy + 1; izdetect = iz;
	distmin = dist;
      }
    }
    //14: -X+Z neighbour
    if (ix - 1 >= 0 && iz + 1 < nbz && !passed.Contains(GetIVoxel(ix - 1, iy, iz + 1, nbx, nbxy)))
    {
      ds->GetCenter(ix - 1, iy, iz + 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix - 1; iydetect = iy; izdetect = iz + 1;
	distmin = dist;
      }
    }
    //15: -Y+Z neighbour
    if (iy - 1 >= 0 && iz + 1 < nbz && !passed.Contains(GetIVoxel(ix, iy - 1, iz + 1, nbx, nbxy)))
    {
      ds->GetCenter(ix, iy - 1, iz + 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix; iydetect = iy - 1; izdetect = iz + 1;
	distmin = dist;
      }
    }

    //16: +X+Y neighbour
    if (ix + 1 < nbx && iy + 1 < nby && !passed.Contains(GetIVoxel(ix + 1, iy + 1, iz, nbx, nbxy)))
    {
      ds->GetCenter(ix + 1, iy + 1, iz, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix + 1; iydetect = iy + 1; izdetect = iz;
	distmin = dist;
      }
    }
    //17: +X+Z neighbour
    if (ix + 1 < nbx && iz + 1 < nbz && !passed.Contains(GetIVoxel(ix + 1, iy, iz + 1, nbx, nbxy)))
    {
      ds->GetCenter(ix + 1, iy, iz + 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix + 1; iydetect = iy; izdetect = iz + 1;
	distmin = dist;
      }
    }
    //18: +Y+Z neighbour
    if (iy + 1 < nby && iz + 1 < nbz && !passed.Contains(GetIVoxel(ix, iy + 1, iz + 1, nbx, nbxy)))
    {
      ds->GetCenter(ix, iy + 1, iz + 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix; iydetect = iy + 1; izdetect = iz + 1;
	distmin = dist;
      }
    }

    // Farest neighbours
    //19: -X-Y-Z neighbour
    if (ix - 1 >= 0 && iy - 1 >= 0 && iz - 1 >= 0 && !passed.Contains(GetIVoxel(ix - 1, iy - 1, iz - 1, nbx, nbxy)))
    {
      ds->GetCenter(ix - 1, iy - 1, iz - 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix - 1; iydetect = iy - 1; izdetect = iz - 1;
	distmin = dist;
      }
    }
    //20: +X-Y-Z neighbour
    if (ix + 1 < nbx && iy - 1 >= 0 && iz - 1 >= 0 && !passed.Contains(GetIVoxel(ix + 1, iy - 1, iz - 1, nbx, nbxy)))
    {
      ds->GetCenter(ix + 1, iy - 1, iz - 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix + 1; iydetect = iy - 1; izdetect = iz - 1;
 	distmin = dist;
     }
    }
    //21: -X+Y-Z neighbour
    if (ix - 1 >= 0 && iy + 1 < nby && iz - 1 >= 0 && !passed.Contains(GetIVoxel(ix - 1, iy + 1, iz - 1, nbx, nbxy)))
    {
      ds->GetCenter(ix - 1, iy + 1, iz - 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix - 1; iydetect = iy + 1; izdetect = iz - 1;
	distmin = dist;
      }
    }
    //22: -X-Y+Z neighbour
    if (ix - 1 >= 0 && iy - 1 >= 0 && iz + 1 < nbz && !passed.Contains(GetIVoxel(ix - 1, iy - 1, iz + 1, nbx, nbxy)))
    {
      ds->GetCenter(ix - 1, iy - 1, iz + 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix - 1; iydetect = iy - 1; izdetect = iz + 1;
	distmin = dist;
      }
    }
    //23: +X+Y-Z neighbour
    if (ix + 1 < nbx && iy + 1 < nby && iz - 1 >= 0 && !passed.Contains(GetIVoxel(ix + 1, iy + 1, iz - 1, nbx, nbxy)))
    {
      ds->GetCenter(ix + 1, iy + 1, iz - 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix + 1; iydetect = iy + 1; izdetect = iz - 1;
	distmin = dist;
      }
    }
    //24: +X-Y+Z neighbour
    if (ix + 1 < nbx && iy - 1 >= 0 && iz + 1 < nbz && !passed.Contains(GetIVoxel(ix + 1, iy - 1, iz + 1, nbx, nbxy)))
    {
      ds->GetCenter(ix + 1, iy - 1, iz + 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix + 1; iydetect = iy - 1; izdetect = iz + 1;
	distmin = dist;
      }
    }
    //25: -X+Y+Z neighbour
    if (ix - 1 >= 0 && iy + 1 < nby && iz + 1 < nbz && !passed.Contains(GetIVoxel(ix - 1, iy + 1, iz + 1, nbx, nbxy)))
    {
      ds->GetCenter(ix - 1, iy + 1, iz + 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix - 1; iydetect = iy + 1; izdetect = iz + 1;
	distmin = dist;
      }
    }
    //26: +X+Y+Z neighbour
    if (ix + 1 < nbx && iy + 1 < nby && iz + 1 < nbz && !passed.Contains(GetIVoxel(ix + 1, iy + 1, iz + 1, nbx, nbxy)))
    {
      ds->GetCenter(ix + 1, iy + 1, iz + 1, xc, yc, zc);
      dist = line.Distance(gp_Pnt(xc, yc, zc));
      if (dist < vmin && dist < distmin)
      {
	ixdetect = ix + 1; iydetect = iy + 1; izdetect = iz + 1;
	distmin = dist;
      }
    }

  } // End of while (zero-voxel...

  if (!Get(myVoxels, myIsBool, ixdetect, iydetect, izdetect))
    return Standard_False;
  return Standard_True;
}
