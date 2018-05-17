// Copyright (c) 2018 OPEN CASCADE SAS
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


#include <BRepGProp_MeshCinert.hxx>
#include <gp_Pnt.hxx>
#include <math.hxx>
#include <TopoDS_Edge.hxx>
#include <Poly_Polygon3D.hxx>
#include <BRep_Tool.hxx>

//=======================================================================
//function : BRepGProp_MeshCinert
//purpose  : 
//=======================================================================

BRepGProp_MeshCinert::BRepGProp_MeshCinert()
{
}

//=======================================================================
//function : SetLocation
//purpose  : 
//=======================================================================
void BRepGProp_MeshCinert::SetLocation(const gp_Pnt& CLocation)
{
  loc = CLocation;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepGProp_MeshCinert::Perform(const TColgp_Array1OfPnt& theNodes)
{

  Standard_Real Ix, Iy, Iz, Ixx, Iyy, Izz, Ixy, Ixz, Iyz;
  dim = Ix = Iy = Iz = Ixx = Iyy = Izz = Ixy = Ixz = Iyz = 0.0;

  Standard_Integer Order = 2; 

  Standard_Real ds;  
  Standard_Real ur, um, u;
  Standard_Real x, y, z; 
  Standard_Real xloc, yloc, zloc;
  Standard_Real Upper;
  gp_XYZ P, D;

  math_Vector GaussP (1, Order);
  math_Vector GaussW (1, Order);

  math::GaussPoints  (Order,GaussP);
  math::GaussWeights (Order,GaussW);

  Standard_Integer nIndex = 0;

  for(nIndex = 1; nIndex < theNodes.Length(); nIndex++) 
  {
    const gp_XYZ& aP1 = theNodes(nIndex).XYZ();
    const gp_XYZ& aP2 = theNodes(nIndex + 1).XYZ();
    Standard_Real dimLocal, IxLocal, IyLocal, IzLocal, IxxLocal, IyyLocal, IzzLocal, IxyLocal, IxzLocal, IyzLocal;
    dimLocal = IxLocal = IyLocal = IzLocal = IxxLocal = IyyLocal = IzzLocal = IxyLocal = IxzLocal = IyzLocal = 0.0;

    loc.Coord (xloc, yloc, zloc);

    Standard_Integer i;

    Upper = (aP2 - aP1).Modulus();
    if (Upper < gp::Resolution())
    {
      continue;
    }
    um = 0.5 * Upper;
    ur =um;
    D = (aP2 - aP1) / Upper;

    for (i = 1; i <= Order; i++) {
      u = um + ur * GaussP (i);
      P = aP1 + u * D;
      P.Coord (x, y, z);
      x   -= xloc;
      y   -= yloc;
      z   -= zloc;
      ds = GaussW (i);
      dimLocal += ds; 
      IxLocal  += x * ds;  
      IyLocal  += y * ds;
      IzLocal  += z * ds;
      IxyLocal += x * y * ds;
      IyzLocal += y * z * ds;
      IxzLocal += x * z * ds;
      x   *= x;      
      y   *= y;      
      z   *= z;      
      IxxLocal += (y + z) * ds;
      IyyLocal += (x + z) * ds;
      IzzLocal += (x + y) * ds;
    }

    dimLocal *= ur;
    IxLocal  *= ur;
    IyLocal  *= ur;
    IzLocal  *= ur;
    IxxLocal *= ur;
    IyyLocal *= ur;
    IzzLocal *= ur;
    IxyLocal *= ur;
    IxzLocal *= ur;
    IyzLocal *= ur;

    dim += dimLocal;
    Ix += IxLocal;
    Iy += IyLocal;
    Iz += IzLocal;
    Ixx += IxxLocal;
    Iyy += IyyLocal;
    Izz += IzzLocal;
    Ixy += IxyLocal;
    Ixz += IxzLocal;
    Iyz += IyzLocal;
  }

  inertia = gp_Mat (gp_XYZ (Ixx, -Ixy, -Ixz),
    gp_XYZ (-Ixy, Iyy, -Iyz),
    gp_XYZ (-Ixz, -Iyz, Izz));

  if (Abs(dim) < gp::Resolution())
    g = P;
  else
    g.SetCoord (Ix/dim, Iy/dim, Iz/dim);
}

//=======================================================================
//function : PreparePolygon
//purpose  : 
//=======================================================================
void BRepGProp_MeshCinert::PreparePolygon(const TopoDS_Edge& theE, 
                                          Handle(TColgp_HArray1OfPnt)& thePolyg)
{
  TopLoc_Location aLoc;
  const Handle(Poly_Polygon3D)& aPolyg = BRep_Tool::Polygon3D(theE, aLoc);
  if (!aPolyg.IsNull())
  {
    const TColgp_Array1OfPnt& aNodes = aPolyg->Nodes();
    thePolyg = new TColgp_HArray1OfPnt(1, aNodes.Length());
    Standard_Integer i;
    if (aLoc.IsIdentity())
    {
      for (i = 1; i <= aNodes.Length(); ++i)
      {
        thePolyg->SetValue(i, aNodes(i));
      }
    }
    else
    {
      const gp_Trsf& aTr = aLoc.Transformation();
      for (i = 1; i <= aNodes.Length(); ++i)
      {
        thePolyg->SetValue(i, aNodes.Value(i).Transformed(aTr));
      }
    }
    return;
  }

  //Try to get PolygonOnTriangulation
  Handle(Poly_Triangulation) aTri;
  Handle(Poly_PolygonOnTriangulation) aPOnTri;
  BRep_Tool::PolygonOnTriangulation(theE, aPOnTri, aTri, aLoc);
  if (!aPOnTri.IsNull())
  {
    Standard_Integer aNbNodes = aPOnTri->NbNodes();
    thePolyg = new TColgp_HArray1OfPnt(1, aNbNodes);
    const TColStd_Array1OfInteger& aNodeInds = aPOnTri->Nodes();
    const TColgp_Array1OfPnt& aNodes = aTri->Nodes();
    Standard_Integer i;
    if (aLoc.IsIdentity())
    {
      for (i = 1; i <= aNbNodes; ++i)
      {
        thePolyg->SetValue(i, aNodes(aNodeInds(i)));
      }
    }
    else
    {
      const gp_Trsf& aTr = aLoc.Transformation();
      for (i = 1; i <= aNbNodes; ++i)
      {
        thePolyg->SetValue(i, aNodes.Value(aNodeInds(i)).Transformed(aTr));
      }
    }
    return;
  }
  //
  //Try to get Polygon2D on Surface
  Handle(Poly_Polygon2D) aPolyg2D;
  Handle(Geom_Surface) aS;
  BRep_Tool::PolygonOnSurface(theE, aPolyg2D, aS, aLoc);
  if (!aPolyg2D.IsNull())
  {
    Standard_Integer aNbNodes = aPolyg2D->NbNodes();
    thePolyg = new TColgp_HArray1OfPnt(1, aNbNodes);
    const TColgp_Array1OfPnt2d& aNodes2D = aPolyg2D->Nodes();
    Standard_Integer i;
    if (aLoc.IsIdentity())
    {
      for (i = 1; i <= aNbNodes; ++i)
      {
        const gp_Pnt2d& aP2d = aNodes2D(i);
        gp_Pnt aP = aS->Value(aP2d.X(), aP2d.Y());
        thePolyg->SetValue(i, aP);
      }
    }
    else
    {
      const gp_Trsf& aTr = aLoc.Transformation();
      for (i = 1; i <= aNbNodes; ++i)
      {
        const gp_Pnt2d& aP2d = aNodes2D(i);
        gp_Pnt aP = aS->Value(aP2d.X(), aP2d.Y());
        aP.Transform(aTr);
        thePolyg->SetValue(i, aP);
      }
    }
    return;
  }

}
