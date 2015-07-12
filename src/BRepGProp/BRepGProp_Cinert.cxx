// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <BRepAdaptor_Curve.hxx>
#include <BRepGProp_Cinert.hxx>
#include <BRepGProp_EdgeTool.hxx>
#include <gp_Pnt.hxx>
#include <math.hxx>
#include <TColStd_Array1OfReal.hxx>

BRepGProp_Cinert::BRepGProp_Cinert(){}

void BRepGProp_Cinert::SetLocation(const gp_Pnt& CLocation)
{
  loc = CLocation;
}

void BRepGProp_Cinert::Perform (const BRepAdaptor_Curve& C)
{

  Standard_Real Ix, Iy, Iz, Ixx, Iyy, Izz, Ixy, Ixz, Iyz;
  dim = Ix = Iy = Iz = Ixx = Iyy = Izz = Ixy = Ixz = Iyz = 0.0;

  Standard_Real Lower    = BRepGProp_EdgeTool::FirstParameter  (C);
  Standard_Real Upper    = BRepGProp_EdgeTool::LastParameter   (C);
  Standard_Integer Order = Min(BRepGProp_EdgeTool::IntegrationOrder (C),
    math::GaussPointsMax());

  gp_Pnt P;    //value on the curve
  gp_Vec V1;   //first derivative on the curve
  Standard_Real ds;  //curvilign abscissae
  Standard_Real ur, um, u;
  Standard_Real x, y, z; 
  Standard_Real xloc, yloc, zloc;

  math_Vector GaussP (1, Order);
  math_Vector GaussW (1, Order);

  //Recuperation des points de Gauss dans le fichier GaussPoints.
  math::GaussPoints  (Order,GaussP);
  math::GaussWeights (Order,GaussW);

  // modified by NIZHNY-MKK  Thu Jun  9 12:13:21 2005.BEGIN
  Standard_Integer nbIntervals = BRepGProp_EdgeTool::NbIntervals(C, GeomAbs_CN);
  Standard_Boolean bHasIntervals = (nbIntervals > 1);
  TColStd_Array1OfReal TI(1, nbIntervals + 1);

  if(bHasIntervals) {
    BRepGProp_EdgeTool::Intervals(C, TI, GeomAbs_CN);
  }
  else {
    nbIntervals = 1;
  }
  Standard_Integer nIndex = 0;
  Standard_Real UU1 = Min(Lower, Upper);
  Standard_Real UU2 = Max(Lower, Upper);

  for(nIndex = 1; nIndex <= nbIntervals; nIndex++) {
    if(bHasIntervals) {
      Lower = Max(TI(nIndex), UU1);
      Upper = Min(TI(nIndex+1), UU2);
    }
    else {
      Lower = UU1;
      Upper = UU2;
    }

    Standard_Real dimLocal, IxLocal, IyLocal, IzLocal, IxxLocal, IyyLocal, IzzLocal, IxyLocal, IxzLocal, IyzLocal;
    dimLocal = IxLocal = IyLocal = IzLocal = IxxLocal = IyyLocal = IzzLocal = IxyLocal = IxzLocal = IyzLocal = 0.0;
    // modified by NIZHNY-MKK  Thu Jun  9 12:13:32 2005.END

    loc.Coord (xloc, yloc, zloc);

    Standard_Integer i;

    // Calcul des integrales aux points de gauss :
    um = 0.5 * (Upper + Lower);
    ur = 0.5 * (Upper - Lower);

    for (i = 1; i <= Order; i++) {
      u   = um + ur * GaussP (i);
      BRepGProp_EdgeTool::D1 (C,u, P, V1); 
      ds  = V1.Magnitude();
      P.Coord (x, y, z);
      x   -= xloc;
      y   -= yloc;
      z   -= zloc;
      ds  *= GaussW (i);
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
    // modified by NIZHNY-MKK  Thu Jun  9 12:13:47 2005.BEGIN
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
  // modified by NIZHNY-MKK  Thu Jun  9 12:13:55 2005.END

  inertia = gp_Mat (gp_XYZ (Ixx, -Ixy, -Ixz),
    gp_XYZ (-Ixy, Iyy, -Iyz),
    gp_XYZ (-Ixz, -Iyz, Izz));

  if (Abs(dim) < gp::Resolution())
    g = P;
  else
    g.SetCoord (Ix/dim, Iy/dim, Iz/dim);
}


BRepGProp_Cinert::BRepGProp_Cinert (const BRepAdaptor_Curve& C, 
                                    const gp_Pnt&   CLocation)
{
  SetLocation(CLocation);
  Perform(C);
}
