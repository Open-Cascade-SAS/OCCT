// Created on: 1999-06-22
// Created by: Roman LYGIN
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

//    pdn 13.07.99 Derivatives are scaled in accordance with local/global parameter transition

#include <ShapeExtend_ComplexCurve.ixx>
#include <Precision.hxx>

//=======================================================================
//function : ShapeExtend_ComplexCurve
//purpose  : 
//=======================================================================

ShapeExtend_ComplexCurve::ShapeExtend_ComplexCurve()
{
  myClosed = Standard_False;
}

//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

 void ShapeExtend_ComplexCurve::Transform(const gp_Trsf& T) 
{
  for (Standard_Integer i = 1; i <= NbCurves(); i++)
    Curve(i)->Transform(T);
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

 void ShapeExtend_ComplexCurve::D0(const Standard_Real U,gp_Pnt& P) const
{
  Standard_Real UOut;
  Standard_Integer ind = LocateParameter (U, UOut);
  Curve(ind)->D0(UOut, P);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

 void ShapeExtend_ComplexCurve::D1(const Standard_Real U,gp_Pnt& P,gp_Vec& V1) const
{
  Standard_Real UOut;
  Standard_Integer ind = LocateParameter (U, UOut);
  Curve(ind)->D1(UOut, P, V1);
  TransformDN(V1,ind,1);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

 void ShapeExtend_ComplexCurve::D2(const Standard_Real U,gp_Pnt& P,gp_Vec& V1,gp_Vec& V2) const
{
  Standard_Real UOut;
  Standard_Integer ind = LocateParameter (U, UOut);
  Curve(ind)->D2(UOut, P, V1, V2);
  TransformDN(V1,ind,1);
  TransformDN(V2,ind,2);
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

 void ShapeExtend_ComplexCurve::D3(const Standard_Real U,gp_Pnt& P,gp_Vec& V1,gp_Vec& V2,gp_Vec& V3) const
{
  Standard_Real UOut;
  Standard_Integer ind = LocateParameter (U, UOut);
  Curve(ind)->D3(UOut, P, V1, V2, V3);
  TransformDN(V1,ind,1);
  TransformDN(V2,ind,2);
  TransformDN(V3,ind,3);
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

 gp_Vec ShapeExtend_ComplexCurve::DN(const Standard_Real U,const Standard_Integer N) const
{
  Standard_Real UOut;
  Standard_Integer ind = LocateParameter (U, UOut);
  gp_Vec res = Curve(ind)->DN(UOut, N);
  if(N)
    TransformDN(res,ind,N); 
  return res;
}

//=======================================================================
//function : CheckConnectivity
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeExtend_ComplexCurve::CheckConnectivity (const Standard_Real Preci)
{
  Standard_Integer NbC = NbCurves();
  Standard_Boolean ok = Standard_True;
  for (Standard_Integer i = 1; i < NbC; i++ ) {
    if (i == 1) myClosed = Value (FirstParameter()).IsEqual (Value (LastParameter()), Preci);
    ok &= Curve (i)->Value (Curve(i)->LastParameter()).IsEqual
      (Curve (i + 1)->Value (Curve(i + 1)->FirstParameter()), Preci);
  }
#ifdef DEB
  if (!ok) cout << "Warning: ShapeExtend_ComplexCurve: not connected in 3d" << endl;
#endif
  return ok;
}

//=======================================================================
//function : TransformDN
//purpose  : 
//=======================================================================

void ShapeExtend_ComplexCurve::TransformDN (gp_Vec& V,
					 const Standard_Integer ind,
					 const Standard_Integer N) const
{
  Standard_Real fact = GetScaleFactor(ind);
  for(Standard_Integer i = 1; i <= N; i++)
    V*= fact;
}
