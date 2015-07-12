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

//szv#4 S4163

#include <gp_Dir.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <GProp_PGProps.hxx>
#include <GProp_PrincipalProps.hxx>
#include <ShapeAnalysis_Geom.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_OutOfRange.hxx>

//=======================================================================
//function : NearestPlane
//purpose  : 
//=======================================================================
Standard_Boolean ShapeAnalysis_Geom::NearestPlane(const TColgp_Array1OfPnt& Pnts,
						   gp_Pln& aPln, Standard_Real& Dmax)
{
  //szv#4:S4163:12Mar99 warning
  GProp_PGProps Pmat(Pnts);
  gp_Pnt g = Pmat.CentreOfMass();
  Standard_Real Xg,Yg,Zg;
  g.Coord(Xg,Yg,Zg);

  GProp_PrincipalProps Pp = Pmat.PrincipalProperties();
  gp_Vec V1 = Pp.FirstAxisOfInertia();
  Standard_Real Xv1,Yv1,Zv1;
  V1.Coord(Xv1,Yv1,Zv1); 
  gp_Vec V2 = Pp.SecondAxisOfInertia(); 
  Standard_Real Xv2,Yv2,Zv2;
  V2.Coord(Xv2,Yv2,Zv2);
  gp_Vec V3 = Pp.ThirdAxisOfInertia(); 
  Standard_Real Xv3,Yv3,Zv3;
  V3.Coord(Xv3,Yv3,Zv3);

  Standard_Real D,X,Y,Z;
  Standard_Real Dmx1 = RealFirst();
  Standard_Real Dmn1 = RealLast();
  Standard_Real Dmx2 = RealFirst();
  Standard_Real Dmn2 = RealLast();
  Standard_Real Dmx3 = RealFirst();
  Standard_Real Dmn3 = RealLast();

  Standard_Integer ilow = Pnts.Lower(), iup = Pnts.Upper();
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = ilow; i <= iup; i ++) {
    Pnts(i).Coord(X,Y,Z);
    D = (X-Xg)*Xv1 +(Y-Yg)*Yv1 + (Z-Zg)*Zv1;
    if (D > Dmx1) Dmx1 = D;
    if (D < Dmn1) Dmn1 = D;
    D = (X-Xg)*Xv2 +(Y-Yg)*Yv2 + (Z-Zg)*Zv2;
    if (D > Dmx2) Dmx2 = D;
    if (D < Dmn2) Dmn2 = D;
    D = (X-Xg)*Xv3 +(Y-Yg)*Yv3 + (Z-Zg)*Zv3;
    if (D > Dmx3) Dmx3 = D;
    if (D < Dmn3) Dmn3 = D;
  }

  //szv#4:S4163:12Mar99 optimized
  Standard_Real Dev1 = Dmx1-Dmn1, Dev2 = Dmx2-Dmn2, Dev3 = Dmx3-Dmn3;
  Standard_Integer It = (Dev1 < Dev2)? ((Dev1 < Dev3)? 1 : 3) : ((Dev2 < Dev3)? 2 : 3);

  switch (It) {
  case 1:
    {
    //szv#4:S4163:12Mar99 optimized
    if ((2.*Dev1 > Dev2) || (2.*Dev1 > Dev3)) It = 0;
    else aPln = gp_Pln(g,V1);
    break;
    }
  case 2:
    {
    //szv#4:S4163:12Mar99 optimized
    if ((2.*Dev2 > Dev1) || (2.*Dev2 > Dev3)) It = 0;
    else aPln = gp_Pln(g,V2);
    break;
    }
  case 3:
    {
    //szv#4:S4163:12Mar99 optimized
    if ((2.*Dev3 > Dev2) || (2.*Dev3 > Dev1)) It = 0;
    else aPln = gp_Pln(g,V3);
    break;
    }
  }

  Dmax = RealFirst();
  if ( It != 0 ) //szv#4:S4163:12Mar99 anti-exception
    for (i = ilow; i <= iup; i ++) {
      D = aPln.Distance (Pnts(i));
      if (Dmax < D) Dmax = D;
    }

  return (It != 0);
}

//=======================================================================
//function : PositionTrsf
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeAnalysis_Geom::PositionTrsf(const Handle(TColStd_HArray2OfReal)& coefs,gp_Trsf& trsf,
						   const Standard_Real unit,const Standard_Real prec)
{
  Standard_Boolean result = Standard_True;

  trsf = gp_Trsf(); //szv#4:S4163:12Mar99 moved

  if (coefs.IsNull()) return Standard_True; //szv#4:S4163:12Mar99 moved

  gp_GTrsf gtrsf;
  for (Standard_Integer i = 1; i <= 3; i ++)
    for (Standard_Integer j = 1; j <= 4; j ++)
      gtrsf.SetValue (i,j, coefs->Value(i,j));

  //try { //szv#4:S4163:12Mar99 waste try
    ////    trsf = gtrsf.Trsf();
    // ---  Prec et Unit ont ete lues suite aux StepFile_Read
    //      Valables pour tous les composants d un assemblage transmis
    //trsf = gp_Trsf();  // Identite forcee au depart //szv#4:S4163:12Mar99 not needed
    //  On prend le contenu de <gtrsf>. Attention a l adressage
    gp_XYZ v1 ( gtrsf.Value(1,1), gtrsf.Value(2,1), gtrsf.Value(3,1) );
    gp_XYZ v2 ( gtrsf.Value(1,2), gtrsf.Value(2,2), gtrsf.Value(3,2) );
    gp_XYZ v3 ( gtrsf.Value(1,3), gtrsf.Value(2,3), gtrsf.Value(3,3) );
    //  A-t-on affaire a une similitude ?
    Standard_Real m1 = v1.Modulus();
    Standard_Real m2 = v2.Modulus();
    Standard_Real m3 = v3.Modulus();

    //    D abord est-elle singuliere cette matrice ?
    if (m1 < prec || m2 < prec || m3 < prec) return Standard_False;
    Standard_Real mm = (m1+m2+m3)/3.;  // voici la Norme moyenne, cf Scale
    //szv#4:S4163:12Mar99 optimized
    Standard_Real pmm = prec*mm;
    if ( Abs(m1 - mm) > pmm || Abs(m2 - mm) > pmm || Abs(m3 - mm) > pmm )
      return Standard_False;
    //szv#4:S4163:12Mar99 warning
    v1.Divide(m1);
    v2.Divide(m2);
    v3.Divide(m3);
    //szv#4:S4163:12Mar99 optimized
    if ( Abs(v1.Dot(v2)) > prec || Abs(v2.Dot(v3)) > prec || Abs(v3.Dot(v1)) > prec )
      return Standard_False;

    //  Ici, Orthogonale et memes normes. En plus on l a Normee
    //  On isole le cas de l Identite (tellement facile et avantageux)
    if (v1.X() != 1 || v1.Y() != 0 || v1.Z() != 0 ||
	v2.X() != 0 || v2.Y() != 1 || v2.Z() != 0 ||
	v3.X() != 0 || v3.Y() != 0 || v3.Z() != 1 ) {
      //  Pas Identite : vraie construction depuis un Ax3
      gp_Dir d1(v1);
      gp_Dir d2(v2);
      gp_Dir d3(v3);
      gp_Ax3 axes (gp_Pnt(0,0,0),d3,d1);
      d3.Cross(d1);
      if (d3.Dot(d2) < 0) axes.YReverse();
      trsf.SetTransformation(axes);
    }

    //  Restent les autres caracteristiques :
    if ( Abs(mm - 1.) > prec ) trsf.SetScale(gp_Pnt(0,0,0), mm); //szv#4:S4163:12Mar99 optimized
    gp_Vec tp (gtrsf.TranslationPart());
    if (unit != 1.) tp.Multiply(unit);
    if (tp.X() != 0 || tp.Y() != 0 || tp.Z() != 0) trsf.SetTranslationPart(tp);
  /* }
  catch(Standard_Failure) {
    trsf = gp_Trsf();
    result = Standard_False;
  } */

  return result;
}
