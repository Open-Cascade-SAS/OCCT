// Created by: Julia GERASIMOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <BlendFunc.hxx>
#include <BlendFunc_ConstThroatWithPenetrationInv.hxx>
#include <math_Matrix.hxx>
#include <Precision.hxx>

//=======================================================================
//function : BlendFunc_ConstThroatInv
//purpose  : 
//=======================================================================

BlendFunc_ConstThroatWithPenetrationInv::
BlendFunc_ConstThroatWithPenetrationInv(const Handle(Adaptor3d_HSurface)& S1,
                                        const Handle(Adaptor3d_HSurface)& S2,
                                        const Handle(Adaptor3d_HCurve)&   C)
  : BlendFunc_ConstThroatInv(S1,S2,C)
{
}


//=======================================================================
//function : IsSolution
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstThroatWithPenetrationInv::IsSolution(const math_Vector& Sol,
                                                                     const Standard_Real Tol)
{
  math_Vector valsol(1,4);
  Value(Sol, valsol);

  if (Abs(valsol(1)) <= Tol &&
      Abs(valsol(2)) <= Tol &&
      Abs(valsol(3)) <= Tol*Tol &&
      Abs(valsol(4)) <= Tol)
    return Standard_True;

  return Standard_False;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstThroatWithPenetrationInv::Value(const math_Vector& X,
                                                                math_Vector& F)
{
  gp_Pnt2d p2d;
  gp_Vec2d v2d;
  csurf->D1(X(1),p2d,v2d);  
  param = X(2);
  curv->D2(param,ptgui,d1gui,d2gui);
  normtg = d1gui.Magnitude();
  nplan  = d1gui.Normalized();
  theD = - (nplan.XYZ().Dot(ptgui.XYZ()));

  math_Vector XX(1,4);

  if(first){
    XX(1) = p2d.X(); XX(2) = p2d.Y();
    XX(3) = X(3); XX(4) = X(4);
  }

  else{
    XX(1) = X(3); XX(2) = X(4);
    XX(3) = p2d.X(); XX(4) = p2d.Y();
  }
  
  surf1->D0( XX(1), XX(2), pts1 );
  surf2->D0( XX(3), XX(4), pts2 );
  
  F(1) = nplan.XYZ().Dot(pts1.XYZ()) + theD;
  F(2) = nplan.XYZ().Dot(pts2.XYZ()) + theD;

  const gp_Vec vref(ptgui, pts1);
  
  F(3) = vref.SquareMagnitude() - Throat*Throat;

  const gp_Vec vec12(pts1, pts2);

  F(4) = vref.Dot(vec12);
  
  return Standard_True;
}

//=======================================================================
//function : Derivatives
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstThroatWithPenetrationInv::Derivatives(const math_Vector& X,
                                                                      math_Matrix& D)
{
  //Standard_Integer i, j;
  gp_Pnt2d p2d;
  gp_Vec2d v2d; //, df1, df2;
  //gp_Pnt pts, ptgui;
  gp_Vec dnplan, temp, temp1, temp2, temp3; //, d1u, d1v, nplan;
  math_Vector XX(1,4); //x1(1,2), x2(1,2);
  //math_Matrix d1(1,2,1,2), d2(1,2,1,2);

  csurf->D1(X(1), p2d, v2d);  
  //corde1.SetParam(X(2));
  //corde2.SetParam(X(2));
  param = X(2);
  curv->D2(param,ptgui,d1gui,d2gui);
  normtg = d1gui.Magnitude();
  nplan  = d1gui.Normalized();
  theD = - (nplan.XYZ().Dot(ptgui.XYZ()));

  dnplan.SetLinearForm(1./normtg,d2gui,
                       -1./normtg*(nplan.Dot(d2gui)),nplan); 
  
  temp1.SetXYZ(pts1.XYZ() - ptgui.XYZ());
  temp2.SetXYZ(pts2.XYZ() - ptgui.XYZ());
  temp3.SetXYZ(pts2.XYZ() - pts1.XYZ());
  
  //x1(1) = p2d.X(); x1(2) = p2d.Y();
  //x2(1) = X(3); x2(2) = X(4);
  if (first)
  {
    XX(1) = p2d.X(); XX(2) = p2d.Y();
    XX(3) = X(3); XX(4) = X(4);
  }
  else
  {
    XX(1) = X(3); XX(2) = X(4);
    XX(3) = p2d.X(); XX(4) = p2d.Y();
  }

  surf1->D1(XX(1), XX(2), pts1, d1u1, d1v1);
  surf2->D1(XX(3), XX(4), pts2, d1u2, d1v2);
  
  if( first ){
    // p2d = pts est sur surf1
    //ptgui = corde1.PointOnGuide();
    //nplan = corde1.NPlan();
    temp.SetLinearForm(v2d.X(),d1u1, v2d.Y(),d1v1);

    D(1,1) = nplan.Dot(temp);
    D(2,1) = 0.;
    //D(3,1) = 2*gp_Vec(ptgui,pts1).Dot(temp);
    D(3,1) = 2*temp1.Dot(temp);
    //D(4,1) = temp.Dot(gp_Vec(pts1,pts2)) - temp.Dot(gp_Vec(ptgui,pts1));
    D(4,1) = temp.Dot(temp3) - temp.Dot(temp1);

    D(1,3) = 0.;
    D(1,4) = 0.;
    D(2,3) = nplan.Dot(d1u2);
    D(2,4) = nplan.Dot(d1v2);
    D(3,3) = 0.;
    D(3,4) = 0.;
    //D(4,3) = gp_Vec(ptgui,pts1).Dot(d1u2);
    D(4,3) = temp1.Dot(d1u2);
    //D(4,4) = gp_Vec(ptgui,pts1).Dot(d1v2);
    D(4,4) = temp1.Dot(d1v2);
    
    //surf1->D1(x1(1),x1(2),pts,d1u,d1v);
  }   
  else{
    //  p2d = pts est sur surf2
    //ptgui = corde2.PointOnGuide();
    //nplan = corde2.NPlan();
    temp.SetLinearForm(v2d.X(),d1u2, v2d.Y(),d1v2);

    D(1,1) = 0.;
    D(2,1) = nplan.Dot(temp);
    D(3,1) = 0.;
    //D(4,1) = gp_Vec(ptgui,pts1).Dot(temp);
    D(4,1) = temp1.Dot(temp);

    D(1,3) = nplan.Dot(d1u1);
    D(1,4) = nplan.Dot(d1v1);
    D(2,3) = 0.;
    D(2,4) = 0.;
    //D(3,3) = 2.*gp_Vec(ptgui,pts1).Dot(d1u1);
    D(3,3) = 2.*temp1.Dot(d1u1);
    //D(3,4) = 2.*gp_Vec(ptgui,pts1).Dot(d1v1);
    D(3,4) = 2.*temp1.Dot(d1v1);
    //D(4,3) = d1u1.Dot(gp_Vec(pts1,pts2)) - d1u1.Dot(gp_Vec(ptgui,pts1));
    D(4,3) = d1u1.Dot(temp3) - d1u1.Dot(temp1);
    D(4,4) = d1v1.Dot(temp3) - d1v1.Dot(temp1);
    
    //surf2->D1(x1(1),x1(2),pts,d1u,d1v);
  }

  D(1,2) = dnplan.Dot(temp1) - nplan.Dot(d1gui);
  D(2,2) = dnplan.Dot(temp2) - nplan.Dot(d1gui);
  //D(3,2) = -2.*gp_Vec(ptgui,pts1).Dot(d1gui);
  D(3,2) = -2.*d1gui.Dot(temp1);
  //D(4,2) = -(gp_Vec(pts1,pts2).Dot(d1gui));
  D(4,2) = -d1gui.Dot(temp3);

  return Standard_True;
} 
