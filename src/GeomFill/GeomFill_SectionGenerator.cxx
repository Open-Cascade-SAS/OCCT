// Created on: 1994-02-18
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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


#include <Geom_BSplineCurve.hxx>
#include <GeomFill_SectionGenerator.hxx>

//=======================================================================
//function : GeomFill_SectionGenerator
//purpose  : 
//=======================================================================
GeomFill_SectionGenerator::GeomFill_SectionGenerator()
{
  if ( mySequence.Length() > 1 ) {
    Handle(TColStd_HArray1OfReal) HPar
      = new (TColStd_HArray1OfReal) (1,mySequence.Length());
    for (Standard_Integer i = 1; i<=mySequence.Length(); i++) {
      HPar->ChangeValue(i) = i-1;
    }
    SetParam(HPar);
  }
}


//=======================================================================
//function : GeomFill_SectionGenerator
//purpose  : 
//=======================================================================

void GeomFill_SectionGenerator::SetParam(const Handle(TColStd_HArray1OfReal) & Params)
{
  Standard_Integer ii, L =  Params->Upper()-Params->Lower()+1;
  myParams = Params;
  for (ii=1; ii<=L; ii++) {
    myParams->SetValue(ii,Params->Value(Params->Lower()+ii-1));
  }
}


//=======================================================================
//function : GetShape
//purpose  : 
//=======================================================================

void GeomFill_SectionGenerator::GetShape(Standard_Integer& NbPoles,
					 Standard_Integer& NbKnots,
					 Standard_Integer& Degree,
					 Standard_Integer& NbPoles2d) const 
{
  Handle(Geom_BSplineCurve) C = 
    Handle(Geom_BSplineCurve)::DownCast(mySequence(1));
  NbPoles = C->NbPoles();
  NbKnots = C->NbKnots();
  Degree  = C->Degree();
  NbPoles2d = 0;
}


//=======================================================================
//function : Knots
//purpose  : 
//=======================================================================

void GeomFill_SectionGenerator::Knots(TColStd_Array1OfReal& TKnots) const 
{
  (Handle(Geom_BSplineCurve)::DownCast(mySequence(1)))->Knots(TKnots);
}


//=======================================================================
//function : Mults
//purpose  : 
//=======================================================================

void GeomFill_SectionGenerator::Mults(TColStd_Array1OfInteger& TMults) const 
{
  (Handle(Geom_BSplineCurve)::DownCast(mySequence(1)))->Multiplicities(TMults);
}


//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

Standard_Boolean GeomFill_SectionGenerator::Section
  (const Standard_Integer      P,
         TColgp_Array1OfPnt&   Poles,
         TColgp_Array1OfVec&   , //DPoles,
         TColgp_Array1OfPnt2d& Poles2d,
         TColgp_Array1OfVec2d& , //DPoles2d,
         TColStd_Array1OfReal& Weigths,
         TColStd_Array1OfReal&   //DWeigths
   ) const 
{
  Section(P , Poles, Poles2d, Weigths);
  return Standard_False;
}


//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

void GeomFill_SectionGenerator::Section
  (const Standard_Integer      P, 
         TColgp_Array1OfPnt&   Poles,
         TColgp_Array1OfPnt2d& , //Poles2d,
         TColStd_Array1OfReal& Weigths ) const 
{
  Handle(Geom_BSplineCurve) C = 
    Handle(Geom_BSplineCurve)::DownCast(mySequence(P));
  
  C->Poles(Poles);
  C->Weights(Weigths);
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real GeomFill_SectionGenerator::Parameter
  (const Standard_Integer P) const
{
  return myParams->Value(P);
}
