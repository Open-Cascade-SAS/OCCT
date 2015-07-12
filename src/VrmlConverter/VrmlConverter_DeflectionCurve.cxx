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
#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
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
#include <VrmlConverter_DeflectionCurve.hxx>
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
// function: PrintPoints
// purpose:
//==================================================================
static void PrintPoints (Handle(TColgp_HArray1OfVec)& aHAV1,
                         Handle(TColStd_HArray1OfInteger)& aHAI1,
                         const Handle(VrmlConverter_Drawer)& aDrawer,
                         Standard_OStream&             anOStream)
{
// creation of Vrml objects
    Handle(VrmlConverter_LineAspect) LA = new VrmlConverter_LineAspect;
    LA = aDrawer->LineAspect();

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
    Handle(Vrml_Coordinate3)  C3 = new Vrml_Coordinate3(aHAV1);
    C3->Print(anOStream);
// IndexedLineSet
    Vrml_IndexedLineSet  ILS;
    ILS.SetCoordIndex(aHAI1);
    ILS.Print(anOStream);
// Separator 1 }
    SE1.Print(anOStream);
}

//==================================================================
// function: DrawCurve
// purpose:
//==================================================================
static void DrawCurve (Adaptor3d_Curve&          aCurve,
		       const Standard_Real           TheDeflection,
                       const Standard_Real           U1,
                       const Standard_Real           U2,
		       const Handle(VrmlConverter_Drawer)& aDrawer, // for passsing of LineAspect
                       Standard_OStream&             anOStream) 
{
  Standard_Integer i;
  Standard_Boolean key = Standard_False;
  Handle(TColgp_HArray1OfVec) HAV1;
  Handle(TColStd_HArray1OfInteger) HAI1;

  switch (aCurve.GetType()) {
  case GeomAbs_Line:
    {
     gp_Vec V;
     key = Standard_True;
     HAV1 = new TColgp_HArray1OfVec(1, 2);
     HAI1 = new TColStd_HArray1OfInteger(1,3);

// array of coordinates of line 
     gp_Pnt p = aCurve.Value(U1);
     V.SetX(p.X()); V.SetY(p.Y()); V.SetZ(p.Z());
     HAV1->SetValue(1,V);

     p = aCurve.Value(U2);
     V.SetX(p.X()); V.SetY(p.Y()); V.SetZ(p.Z());
     HAV1->SetValue(2,V);

// array of indexes of line
     HAI1->SetValue(1,0);	
     HAI1->SetValue(2,1);
     HAI1->SetValue(3,-1);

    }
    break;
  case GeomAbs_Circle:
    {
      Standard_Real Radius = aCurve.Circle().Radius();
     if (!Precision::IsInfinite(Radius)) {
       Standard_Real DU = Sqrt(8.0 * TheDeflection / Radius);
       Standard_Integer N = Standard_Integer(Abs( U2 - U1) / DU);

       if ( N > 0) {

	 gp_Vec V;
         key = Standard_True;
	 HAV1 = new TColgp_HArray1OfVec(1, N+1);
	 HAI1 = new TColStd_HArray1OfInteger(1,N+2);

	 DU = (U2-U1) / N;
	 Standard_Real U;
	 gp_Pnt p;

	 for (Standard_Integer Index = 1; Index <= N+1; Index++) {
	   U = U1 + (Index - 1) * DU;
	   p = aCurve.Value(U);

	   V.SetX(p.X()); V.SetY(p.Y()); V.SetZ(p.Z());
	   HAV1->SetValue(Index,V);
	   HAI1->SetValue(Index,Index-1);	

	 }
/*
  	 if( HAV1->Value(1).IsEqual( HAV1->Value(N+1),Precision::Confusion(), Precision::Angular() ) )
 	   {
 	     HAI1->SetValue(N+1, 0);
 	   }
*/	 
 	 HAI1->SetValue(HAI1->Upper(),-1);
       }
     }
    }
    break;
    
  default:
    {
       GCPnts_QuasiUniformDeflection Algo(aCurve,TheDeflection,U1,U2);
       if(Algo.IsDone()){

	 Standard_Integer NumberOfPoints = Algo.NbPoints();
	 if (NumberOfPoints > 0) {
	  
  	  gp_Vec V;
	  key = Standard_True;
 	  HAV1 = new TColgp_HArray1OfVec(1, NumberOfPoints);
  	  HAI1 = new TColStd_HArray1OfInteger(1,NumberOfPoints+1);
 	  gp_Pnt p;
	  
  	  for (i=1;i<=NumberOfPoints;i++) { 
 	    p = Algo.Value(i);
 	    V.SetX(p.X()); V.SetY(p.Y()); V.SetZ(p.Z());
  	    HAV1->SetValue(i,V);
 	  }

 	  for (i=HAI1->Lower(); i < HAI1->Upper(); i++)
	    {
	      HAI1->SetValue(i,i-1);	
	    }
	  HAI1->SetValue(HAI1->Upper(),-1);
	  
	}
      }
      //else
      //cannot draw with respect to a maximal chordial deviation
    }
  }

//cout  << " Array HAI1 - coordIndex " << endl;  
//     
//for ( i=HAI1->Lower(); i <= HAI1->Upper(); i++ )
//  {
//    cout << HAI1->Value(i) << endl;
//  } 


  if (key) {
    PrintPoints(HAV1, HAI1, aDrawer, anOStream);
  }
}

//==================================================================
// function: GetDeflection
// purpose:
//==================================================================
static Standard_Real GetDeflection(const Adaptor3d_Curve&        aCurve,
				   const Standard_Real         U1, 
				   const Standard_Real         U2, 
				   const Handle(VrmlConverter_Drawer)& aDrawer) {

  Standard_Real theRequestedDeflection;
  if(aDrawer->TypeOfDeflection() == Aspect_TOD_RELATIVE)   // TOD_RELATIVE, TOD_ABSOLUTE
    {
      Bnd_Box box;
      BndLib_Add3dCurve::Add(aCurve, U1, U2, Precision::Confusion(), box);

      Standard_Real  Xmin, Xmax, Ymin, Ymax, Zmin, Zmax, diagonal;
      box.Get( Xmin, Ymin, Zmin, Xmax, Ymax, Zmax );
      if (!(box.IsOpenXmin() || box.IsOpenXmax() ||
	    box.IsOpenYmin() || box.IsOpenYmax() ||
	    box.IsOpenZmin() || box.IsOpenZmax()))
	{
	  diagonal = Sqrt ((Xmax - Xmin)*( Xmax - Xmin) + ( Ymax - Ymin)*( Ymax - Ymin) + ( Zmax - Zmin)*( Zmax - Zmin));
	  diagonal = Max(diagonal, Precision::Confusion());
	  theRequestedDeflection = aDrawer->DeviationCoefficient() * diagonal;      
	}  
      else
	{
	  diagonal =1000000.;
	  theRequestedDeflection = aDrawer->DeviationCoefficient() * diagonal;  
	}
//      cout << "diagonal = " << diagonal << endl;
//      cout << "theRequestedDeflection = " << theRequestedDeflection << endl;
    }
  else 
    {
      theRequestedDeflection = aDrawer->MaximalChordialDeviation(); 
    }
  return theRequestedDeflection;
}
//==================================================================
// function: Add
// purpose: 1
//==================================================================
void VrmlConverter_DeflectionCurve::Add(Standard_OStream&                   anOStream, 
					 Adaptor3d_Curve&                aCurve, 
					const Handle(VrmlConverter_Drawer)& aDrawer)
{

  Standard_Real V1, V2;
  Standard_Real aLimit = aDrawer->MaximalParameterValue();
  FindLimits(aCurve, aLimit, V1, V2);
 
  Standard_Real theRequestedDeflection = GetDeflection(aCurve, V1, V2, aDrawer);

  DrawCurve(aCurve,
	    theRequestedDeflection,
            V1 , V2, aDrawer, anOStream);
}

//==================================================================
// function: Add 
// purpose: 2
//==================================================================
void VrmlConverter_DeflectionCurve::Add(Standard_OStream&                   anOStream,
					Adaptor3d_Curve&                aCurve, 
					const Standard_Real                 U1, 
					const Standard_Real                 U2, 
					const Handle(VrmlConverter_Drawer)& aDrawer)
{
  Standard_Real V1 = U1;
  Standard_Real V2 = U2;  

  if (Precision::IsNegativeInfinite(V1)) V1 = -aDrawer->MaximalParameterValue();
  if (Precision::IsPositiveInfinite(V2)) V2 = aDrawer->MaximalParameterValue();

  Standard_Real theRequestedDeflection = GetDeflection(aCurve, V1, V2, aDrawer);
  DrawCurve(aCurve,
	    theRequestedDeflection,
            V1 , V2, aDrawer, anOStream);

}
//==================================================================
// function: Add
// purpose: 3
//==================================================================

void VrmlConverter_DeflectionCurve::Add(Standard_OStream&    anOStream, 
					Adaptor3d_Curve& aCurve, 
					const Standard_Real  aDeflection, 
					const Standard_Real  aLimit)
{
  Standard_Real V1, V2;
  FindLimits(aCurve, aLimit, V1, V2);

  Handle(VrmlConverter_Drawer) aDrawer = new VrmlConverter_Drawer;
  Handle(VrmlConverter_LineAspect) la = new  VrmlConverter_LineAspect;
  aDrawer->SetLineAspect(la);

  DrawCurve(aCurve,
	    aDeflection,
            V1 , V2, aDrawer, anOStream);
}
//==================================================================
// function: Add
// purpose: 4
//==================================================================

void VrmlConverter_DeflectionCurve::Add(Standard_OStream&                   anOStream, 
					Adaptor3d_Curve&                aCurve, 
					const Standard_Real                 aDeflection, 
					const Handle(VrmlConverter_Drawer)& aDrawer)
{
  Standard_Real aLimit = aDrawer->MaximalParameterValue();
  Standard_Real V1, V2;
  FindLimits(aCurve, aLimit, V1, V2);

  DrawCurve(aCurve,
	    aDeflection,
            V1 , V2, aDrawer, anOStream);
}
//==================================================================
// function: Add
// purpose: 5
//==================================================================

void VrmlConverter_DeflectionCurve::Add(Standard_OStream&    anOStream, 
					Adaptor3d_Curve& aCurve, 
					const Standard_Real  U1, 
					const Standard_Real  U2, 
					const Standard_Real  aDeflection)
{
  Handle(VrmlConverter_Drawer) aDrawer = new VrmlConverter_Drawer;
  Handle(VrmlConverter_LineAspect) la = new VrmlConverter_LineAspect;
  aDrawer->SetLineAspect(la);

  DrawCurve(aCurve,
	    aDeflection,
            U1 , U2, aDrawer, anOStream);
}

//==================================================================
// function: Add
// purpose: 6
//==================================================================

void VrmlConverter_DeflectionCurve::Add(Standard_OStream& anOStream,
                                        const Adaptor3d_Curve& aCurve,
                                        const Handle(TColStd_HArray1OfReal)& aParams,
                                        const Standard_Integer aNbNodes,
                                        const Handle(VrmlConverter_Drawer)& aDrawer)
{
  Handle(TColgp_HArray1OfVec) aHAV1 = new TColgp_HArray1OfVec(1, aNbNodes);
  Handle(TColStd_HArray1OfInteger) aHAI1 = new TColStd_HArray1OfInteger(1, aNbNodes + 1);

  Standard_Integer i;
  gp_Pnt aPoint;
  gp_Vec aVec;
  for (i = 1; i<=aNbNodes; i++)
  {
    Standard_Real aParam = aParams->Value(aParams->Lower() + i - 1);
    aPoint = aCurve.Value(aParam);
    aVec.SetX(aPoint.X());
    aVec.SetY(aPoint.Y());
    aVec.SetZ(aPoint.Z());
    aHAV1->SetValue(i, aVec);
  }

  for (i = aHAI1->Lower(); i < aHAI1->Upper(); i++)
  {
    aHAI1->SetValue(i,i-1);
  }
  aHAI1->SetValue(aHAI1->Upper(),-1);

  PrintPoints(aHAV1, aHAI1, aDrawer, anOStream);
}
