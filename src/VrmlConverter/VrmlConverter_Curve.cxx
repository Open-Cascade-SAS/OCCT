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


#include <Adaptor3d_Curve.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TColgp_HArray1OfVec.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Vrml_Coordinate3.hxx>
#include <Vrml_IndexedLineSet.hxx>
#include <Vrml_Material.hxx>
#include <Vrml_Separator.hxx>
#include <VrmlConverter_Curve.hxx>
#include <VrmlConverter_Drawer.hxx>
#include <VrmlConverter_LineAspect.hxx>

//==================================================================
// function: FindLimits
// purpose:
//==================================================================
static void FindLimits(const Adaptor3d_Curve& aCurve,
		       const Standard_Real  aLimit,
		       Standard_Real&       First,
		       Standard_Real&       Last)
{
  First = aCurve.FirstParameter();
  Last  = aCurve.LastParameter();
  Standard_Boolean firstInf = Precision::IsNegativeInfinite(First);
  Standard_Boolean lastInf  = Precision::IsPositiveInfinite(Last);

  if (firstInf || lastInf) {
    gp_Pnt P1,P2;
    Standard_Real delta = 1;
    if (firstInf && lastInf) {
      do {
	delta *= 2;
	First = - delta;
	Last  =   delta;
	aCurve.D0(First,P1);
	aCurve.D0(Last,P2);
      } while (P1.Distance(P2) < aLimit);
    }
    else if (firstInf) {
      aCurve.D0(Last,P2);
      do {
	delta *= 2;
	First = Last - delta;
	aCurve.D0(First,P1);
      } while (P1.Distance(P2) < aLimit);
    }
    else if (lastInf) {
      aCurve.D0(First,P1);
      do {
	delta *= 2;
	Last = First + delta;
	aCurve.D0(Last,P2);
      } while (P1.Distance(P2) < aLimit);
    }
  }    
}


//==================================================================
// function: DrawCurve
// purpose:
//==================================================================
static void DrawCurve (const Adaptor3d_Curve&          aCurve,
		       const Standard_Integer        NbP,
                       const Standard_Real           U1,
                       const Standard_Real           U2,
		       const Handle(VrmlConverter_Drawer)& aDrawer, // for passsing of LineAspect
                       Standard_OStream&             anOStream) 
{
  Standard_Integer nbintervals = 1, i;
  Handle(TColgp_HArray1OfVec) HAV1;
  Handle(TColStd_HArray1OfInteger) HAI1;
  
  if (aCurve.GetType() == GeomAbs_BSplineCurve) {
    nbintervals = aCurve.NbKnots() - 1;
//     cout << "NbKnots "<<aCurve.NbKnots() << endl;
    nbintervals = Max(1, nbintervals/3);
  }


  switch (aCurve.GetType()) {
  case GeomAbs_Line:
    {
     gp_Vec V;
     HAV1 = new TColgp_HArray1OfVec(1, 2);
// array of coordinates of line 
     gp_Pnt p = aCurve.Value(U1);
     V.SetX(p.X()); V.SetY(p.Y()); V.SetZ(p.Z());
     HAV1->SetValue(1,V);

     p = aCurve.Value(U2);
     V.SetX(p.X()); V.SetY(p.Y()); V.SetZ(p.Z());
     HAV1->SetValue(2,V);

     HAI1 = new TColStd_HArray1OfInteger(1,3);
// array of indexes of line
     HAI1->SetValue(1,0);	
     HAI1->SetValue(2,1);
     HAI1->SetValue(3,-1);

    }
    break;
  default:
    {

      Standard_Real U;
      Standard_Integer N = Max(2, NbP*nbintervals);

//     cout << "nbintervals " << nbintervals << endl;
//     cout <<  "N " << N << endl;

      gp_Vec V;
      HAV1 = new TColgp_HArray1OfVec(1, N);
//      HAI1 = new TColStd_HArray1OfInteger(1,(N/2*3+N%2));
      HAI1 = new TColStd_HArray1OfInteger(1,N+1);
      Standard_Real DU = (U2-U1) / (N-1);
      gp_Pnt p;

      for (i = 1; i <= N;i++) { 
	U = U1 + (i-1)*DU;
 	p = aCurve.Value(U);

 	V.SetX(p.X()); V.SetY(p.Y()); V.SetZ(p.Z());
 	HAV1->SetValue(i,V);
      }      

//      Standard_Integer j=1,k;

//     for (i=HAI1->Lower(); i <= HAI1->Upper(); i++)
// 	{
// 	  k = i % 3;
// 	  if(k == 0)
// 	    {
// 	      HAI1->SetValue(i,-1);	
// 	      j++;
// 	    }
// 	  else
// 	    {
// 	      HAI1->SetValue(i,i-j);	
// 	    }
// 	}

     for (i=HAI1->Lower(); i < HAI1->Upper(); i++)
 	{
	  HAI1->SetValue(i,i-1);	
	}
 	HAI1->SetValue(HAI1->Upper(),-1);
    }
  }
  
//  cout  << " Array HAI1 - coordIndex " << endl;  
//  for ( i=HAI1->Lower(); i <= HAI1->Upper(); i++ )
//    {
//      cout << HAI1->Value(i) << endl;
//       } 

// creation of Vrml objects
  Handle(VrmlConverter_LineAspect) LA = new VrmlConverter_LineAspect;
  LA = aDrawer->LineAspect();

//     cout << "LA->HasMaterial() = " << LA->HasMaterial()  << endl;

// Separator 1 {
  Vrml_Separator SE1;
  SE1.Print(anOStream);
// Material

  if (LA->HasMaterial()){

    Handle(Vrml_Material) M;
    M = LA->Material();
    
    M->Print(anOStream);
  }
// Coordinate3
  Handle(Vrml_Coordinate3)  C3 = new Vrml_Coordinate3(HAV1);
  C3->Print(anOStream);
// IndexedLineSet
  Vrml_IndexedLineSet  ILS;
  ILS.SetCoordIndex(HAI1);
  ILS.Print(anOStream);
// Separator 1 }
  SE1.Print(anOStream);
}

//==================================================================
// function: Add 1
// purpose:
//==================================================================
void VrmlConverter_Curve::Add(const Adaptor3d_Curve&                aCurve, 
			      const Handle(VrmlConverter_Drawer)& aDrawer,
			      Standard_OStream&                   anOStream) 
{


  Standard_Integer NbPoints = aDrawer->Discretisation();
  Standard_Real V1, V2;
  Standard_Real aLimit = aDrawer->MaximalParameterValue();
  FindLimits(aCurve, aLimit, V1, V2);

//     cout << "V1 = "<< V1 << endl;
//     cout << "V2 = "<< V2 << endl;
//     cout << "NbPoints = "<< NbPoints << endl;
//     cout << "aLimit = "<< aLimit << endl;

  DrawCurve(aCurve,
 	    NbPoints,
            V1 , V2, aDrawer, anOStream);

}

//==================================================================
// function: Add 2
// purpose:
//==================================================================
void VrmlConverter_Curve::Add(const Adaptor3d_Curve&                aCurve, 
 			      const Standard_Real                 U1, 
 			      const Standard_Real                 U2, 
 			      const Handle(VrmlConverter_Drawer)& aDrawer,
 			      Standard_OStream&                   anOStream) 
{

  Standard_Integer NbPoints = aDrawer->Discretisation();
  Standard_Real V1 = U1;
  Standard_Real V2 = U2;  

  if (Precision::IsNegativeInfinite(V1)) V1 = -aDrawer->MaximalParameterValue();
  if (Precision::IsPositiveInfinite(V2)) V2 = aDrawer->MaximalParameterValue();

//     cout << "V1 = "<< V1 << endl;
//     cout << "V2 = "<< V2 << endl;
//     cout << "NbPoints = "<< NbPoints << endl;
 
  DrawCurve(aCurve,
	     NbPoints,
	     V1 , V2, aDrawer, anOStream);
  
}

//==================================================================
// function: Add 3
// purpose:
//==================================================================
void VrmlConverter_Curve::Add(const Adaptor3d_Curve&   aCurve, 
			      const Standard_Real    U1, 
			      const Standard_Real    U2, 
			      Standard_OStream&      anOStream, 
			      const Standard_Integer aNbPoints)
{
  Handle(VrmlConverter_Drawer) aDrawer = new VrmlConverter_Drawer;
  Handle(VrmlConverter_LineAspect) la = new VrmlConverter_LineAspect;
  aDrawer->SetLineAspect(la);


  Standard_Real V1 = U1;
  Standard_Real V2 = U2;  

  if (Precision::IsNegativeInfinite(V1)) V1 = -aDrawer->MaximalParameterValue();
  if (Precision::IsPositiveInfinite(V2)) V2 = aDrawer->MaximalParameterValue();

//     cout << "V1 = "<< V1 << endl;
//     cout << "V2 = "<< V2 << endl;
//     cout << "NbPoints = "<< aNbPoints << endl;

  DrawCurve(aCurve,
	    aNbPoints,
            V1 , V2, aDrawer, anOStream);

}

