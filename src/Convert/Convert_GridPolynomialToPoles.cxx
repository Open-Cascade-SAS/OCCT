// Created on: 1996-07-08
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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

// Modified:	Fri Oct  3 14:58:05 1997
//    by:	Joelle CHAUVET
//              Condition d'extraction corrigee
//              + positionnement par EvalPoly2Var

#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <Convert_GridPolynomialToPoles.hxx>
#include <PLib.hxx>
#include <Standard_DomainError.hxx>
#include <StdFail_NotDone.hxx>

Convert_GridPolynomialToPoles::
Convert_GridPolynomialToPoles(
			      const Standard_Integer MaxUDegree, 
			      const Standard_Integer MaxVDegree, 
			      const Handle(TColStd_HArray1OfInteger)& NumCoeffPerSurface,
			      const Handle(TColStd_HArray1OfReal)& Coefficients, 
			      const Handle(TColStd_HArray1OfReal)& PolynomialUIntervals, 
			      const Handle(TColStd_HArray1OfReal)& PolynomialVIntervals) :
			      myDone(Standard_False)			      
{
  // Les Controles
  if ((NumCoeffPerSurface->Lower()!=1 ) || 
      (NumCoeffPerSurface->Upper()!= 2) )
    {
      throw Standard_DomainError("Convert : Wrong Coefficients");
    } 
  if ((Coefficients->Lower()!=1 ) || 
      (Coefficients->Upper()!= 3*(MaxUDegree+1)*(MaxVDegree+1)))
    {
      throw Standard_DomainError("Convert : Wrong Coefficients");
    }  

  // Les Degres
  myUDegree = NumCoeffPerSurface->Value(1)-1;
  myVDegree = NumCoeffPerSurface->Value(2)-1;

  if (myUDegree > MaxUDegree) 
     throw Standard_DomainError("Convert : Incoherence beetween NumCoeffPerSurface and MaxUDegree");
  if (myVDegree > MaxVDegree) 
     throw Standard_DomainError("Convert : Incoherence beetween NumCoeffPerSurface and MaxVDegree");

  Handle(TColStd_HArray2OfInteger) NumCoeff = 
    new (TColStd_HArray2OfInteger)(1, 1, 1, 2);
  NumCoeff->SetValue(1, 1, NumCoeffPerSurface->Value(1));
  NumCoeff->SetValue(1, 2, NumCoeffPerSurface->Value(2));
  
  Perform (0, 0, 
	   MaxUDegree, MaxVDegree,  
	   NumCoeff,
	   Coefficients,
	   PolynomialUIntervals,
	   PolynomialVIntervals,
           PolynomialUIntervals,
           PolynomialVIntervals);
}

Convert_GridPolynomialToPoles::
Convert_GridPolynomialToPoles(
			      const Standard_Integer NbUSurfaces,
			      const Standard_Integer NbVSurfaces, 
			      const Standard_Integer UContinuity, 
			      const Standard_Integer VContinuity,
			      const Standard_Integer MaxUDegree,
			      const Standard_Integer MaxVDegree,
			      const Handle(TColStd_HArray2OfInteger)& NumCoeffPerSurface,
			      const Handle(TColStd_HArray1OfReal)& Coefficients,
			      const Handle(TColStd_HArray1OfReal)& PolynomialUIntervals,
			      const Handle(TColStd_HArray1OfReal)& PolynomialVIntervals,
			      const Handle(TColStd_HArray1OfReal)& TrueUIntervals,
			      const Handle(TColStd_HArray1OfReal)& TrueVIntervals) :
			      myDone(Standard_False)
{
  Standard_Integer ii;
  Standard_Integer RealUDegree = Max(MaxUDegree, 2*UContinuity + 1);
  Standard_Integer RealVDegree = Max(MaxVDegree, 2*VContinuity + 1);
  myUDegree = 0;
  myVDegree = 0;
  
  // Les controles
  if((NumCoeffPerSurface->LowerRow()!=1) || 
     (NumCoeffPerSurface->UpperRow()!=NbUSurfaces*NbVSurfaces) ||
     (NumCoeffPerSurface->LowerCol()!=1) || 
     (NumCoeffPerSurface->UpperCol()!=2) )
    {
      throw Standard_DomainError("Convert : Wrong NumCoeffPerSurface");
    }

  if ((Coefficients->Lower()!=1 ) || 
      (Coefficients->Upper()!= 3*NbUSurfaces*NbVSurfaces*
       (RealUDegree + 1) * (RealVDegree + 1)) )
    {
      throw Standard_DomainError("Convert : Wrong Coefficients");
    }
     
  // Calcul des degree
  for (ii=1; ii<=NbUSurfaces*NbVSurfaces; ii++) {
    if (NumCoeffPerSurface->Value(ii,1) > myUDegree+1) 
      myUDegree = NumCoeffPerSurface->Value(ii,1)-1;
    if (NumCoeffPerSurface->Value(ii,2) > myVDegree+1) 
      myVDegree = NumCoeffPerSurface->Value(ii,2)-1;
  }

  if (myUDegree > RealUDegree) 
     throw Standard_DomainError("Convert : Incoherence beetween NumCoeffPerSurface and MaxUDegree");
  if (myVDegree > RealVDegree) 
     throw Standard_DomainError("Convert : Incoherence beetween NumCoeffPerSurface and MaxVDegree");

  Perform (UContinuity, VContinuity, 
	   RealUDegree, RealVDegree,
	   NumCoeffPerSurface,
	   Coefficients,
	   PolynomialUIntervals,
	   PolynomialVIntervals,
           TrueUIntervals,
           TrueVIntervals);
}

void Convert_GridPolynomialToPoles::Perform(const Standard_Integer UContinuity,
					    const Standard_Integer VContinuity,
					    const Standard_Integer MaxUDegree, 
					    const Standard_Integer MaxVDegree,
					    const Handle(TColStd_HArray2OfInteger)& NumCoeffPerSurface,
					    const Handle(TColStd_HArray1OfReal)& Coefficients,
					    const Handle(TColStd_HArray1OfReal)& PolynomialUIntervals,
					    const Handle(TColStd_HArray1OfReal)& PolynomialVIntervals,
					    const Handle(TColStd_HArray1OfReal)& TrueUIntervals,
					    const Handle(TColStd_HArray1OfReal)& TrueVIntervals)
{
 // (1) Construction des Tables monodimensionnelles ----------------------------
    Handle(TColStd_HArray1OfReal) UParameters, VParameters;
    myUKnots = new (TColStd_HArray1OfReal) (1,  TrueUIntervals->Length());
    myUKnots->ChangeArray1() =  TrueUIntervals->Array1();
    myVKnots = new (TColStd_HArray1OfReal) (1,  TrueVIntervals->Length());
    myVKnots->ChangeArray1() = TrueVIntervals->Array1();

    BuildArray( myUDegree,
		myUKnots,
		UContinuity,
		myUFlatKnots,
		myUMults, 
		UParameters);

    BuildArray( myVDegree,
		myVKnots,
		VContinuity,
		myVFlatKnots,
		myVMults, 
		VParameters);

 // (2) Digitalisation -------------------------------------------------------

   Standard_Integer ii, jj, Uindex=0, Vindex=0;
   Standard_Integer Patch_Indice=0;
   Standard_Real NValue, UValue, VValue;
   Standard_Integer dimension = 3*( myVDegree+1);
   Standard_Integer SizPatch = 3 * (MaxUDegree+1) * (MaxVDegree+1);
   myPoles = new (TColgp_HArray2OfPnt) (1, UParameters->Length(), 
				        1, VParameters->Length());

   TColStd_Array1OfReal Patch(1, (myUDegree+1)*dimension);
   TColStd_Array1OfReal Point(1, 3); 
   Standard_Real * Coeffs =  (Standard_Real *) &Patch.ChangeValue(1);
   Standard_Real * Digit  =  (Standard_Real *) &Point.ChangeValue(1);

   for (ii=1, Uindex=1; ii<=UParameters->Length(); ii++) {

     while (UParameters->Value(ii) > TrueUIntervals->Value(Uindex+1) 
            &&	Uindex < myUKnots->Length()-1) { Uindex++; }

     NValue = (UParameters->Value(ii) - TrueUIntervals->Value(Uindex) )
            / (TrueUIntervals->Value(Uindex+1) - TrueUIntervals->Value(Uindex));
     UValue = (1-NValue) *  PolynomialUIntervals->Value(1) 
            + NValue * PolynomialUIntervals->Value(2) ;
    
    for (jj=1, Vindex=1; jj<=VParameters->Length(); jj++) {
      
      while (VParameters->Value(jj) > TrueVIntervals->Value(Vindex+1) 
	     &&	Vindex < myVKnots->Length()-1) { Vindex++; }

      NValue = (VParameters->Value(jj) - TrueVIntervals->Value(Vindex) )
             / (TrueVIntervals->Value(Vindex+1) - TrueVIntervals->Value(Vindex));
      VValue = (1-NValue) *  PolynomialVIntervals->Value(1) 
             + NValue * PolynomialVIntervals->Value(2) ;

      // (2.1) Extraction du bon Patch
      if (Patch_Indice != Uindex + (myUKnots->Length()-1)*(Vindex-1)) {
	Standard_Integer k1, k2, pos, ll=1;
	Patch_Indice = Uindex + (myUKnots->Length()-1)*(Vindex-1);
	for (k1 = 1; k1 <= NumCoeffPerSurface->Value(Patch_Indice, 1); k1++) {
	  pos = SizPatch*(Patch_Indice-1)+3*(MaxVDegree+1)*(k1-1)+1;
	  for (k2 = 1; 
	       k2 <= NumCoeffPerSurface->Value(Patch_Indice, 2); 
	       k2++, pos+=3 ) {
	    Patch(ll)   =  Coefficients->Value(pos);
	    Patch(ll+1) =  Coefficients->Value(pos+1);
	    Patch(ll+2) =  Coefficients->Value(pos+2);
	    ll += 3;
	  }
	}
      }

      // (2.2) Positionnement en UValue,VValue
      PLib::EvalPoly2Var(UValue,VValue,0,0,
			 NumCoeffPerSurface->Value(Patch_Indice,1)-1,
			 NumCoeffPerSurface->Value(Patch_Indice,2)-1,  
			 3,
			 Coeffs[0],
			 Digit[0]);

      myPoles->SetValue(ii, jj, gp_Pnt (Digit[0], Digit[1], Digit[2]));
    }
   }


 // (3)Interpolation --------------------------------------------------------------

  Standard_Integer InversionProblem;
  BSplSLib::Interpolate(myUDegree, myVDegree, 
			myUFlatKnots->Array1(),
			myVFlatKnots->Array1(),
			UParameters->Array1(), 
			VParameters->Array1(),
			myPoles->ChangeArray2(),
			InversionProblem);
  myDone = (InversionProblem == 0);  
}


void  Convert_GridPolynomialToPoles::BuildArray(const Standard_Integer Degree,
						const Handle(TColStd_HArray1OfReal)& Knots,
						const Standard_Integer Continuity,
						Handle(TColStd_HArray1OfReal)& FlatKnots,
						Handle(TColStd_HArray1OfInteger)& Mults, 
						Handle(TColStd_HArray1OfReal)& Parameters) const
{
 Standard_Integer NumCurves =  Knots->Length()-1;

 // Calcul des Multiplicites
 Standard_Integer ii;
 Standard_Integer multiplicities = Degree - Continuity;
 Mults = new (TColStd_HArray1OfInteger)(1, Knots->Length());

 for (ii = 2 ; ii < Knots->Length() ; ii++) {
   Mults ->SetValue(ii,multiplicities) ;
 }
 Mults ->SetValue(1, Degree + 1) ;
 Mults ->SetValue(NumCurves + 1, Degree + 1) ;

 // Calcul des Noeuds Plats
 Standard_Integer num_flat_knots = multiplicities * (NumCurves - 1) 
                                 +  2 * Degree + 2;
 FlatKnots =
 new TColStd_HArray1OfReal(1,num_flat_knots) ;

 BSplCLib::KnotSequence (Knots->Array1(), 
			 Mults->Array1(),
			 Degree,
			 Standard_False,
			 FlatKnots->ChangeArray1());

 // Calcul du nombre de Poles
 Standard_Integer num_poles = num_flat_knots - Degree - 1;

 // Cacul des parametres d'interpolation
 Parameters = new (TColStd_HArray1OfReal) (1,num_poles);
 BSplCLib::BuildSchoenbergPoints(Degree,
				 FlatKnots->Array1(),
				 Parameters->ChangeArray1());  
}

Standard_Integer Convert_GridPolynomialToPoles::NbUPoles() const 
{
 StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
 return  myPoles->ColLength(); 
}

Standard_Integer Convert_GridPolynomialToPoles::NbVPoles() const 
{
 StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
 return  myPoles->RowLength(); 
}


const Handle(TColgp_HArray2OfPnt)& 
Convert_GridPolynomialToPoles::Poles() const 
{
 StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
 return myPoles; 
}

Standard_Integer Convert_GridPolynomialToPoles::UDegree() const 
{
 StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
 return  myUDegree;
}

Standard_Integer Convert_GridPolynomialToPoles::VDegree() const 
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return  myVDegree;
}

Standard_Integer Convert_GridPolynomialToPoles::NbUKnots() const 
{
 StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
 return myUKnots->Length();
}

Standard_Integer Convert_GridPolynomialToPoles::NbVKnots() const 
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myVKnots->Length();
}

const Handle(TColStd_HArray1OfReal)&  
Convert_GridPolynomialToPoles::UKnots() const 
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myUKnots;
}

const Handle(TColStd_HArray1OfReal)& 
Convert_GridPolynomialToPoles::VKnots() const 
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myVKnots;
}

const Handle(TColStd_HArray1OfInteger)& 
Convert_GridPolynomialToPoles::UMultiplicities() const 
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myUMults;
}

const Handle(TColStd_HArray1OfInteger)& 
Convert_GridPolynomialToPoles::VMultiplicities() const 
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myVMults;
}

Standard_Boolean Convert_GridPolynomialToPoles::IsDone() const 
{
  return myDone;
}
