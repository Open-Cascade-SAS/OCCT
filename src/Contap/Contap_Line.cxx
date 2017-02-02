// Created on: 1993-02-05
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Contap_Line.hxx>
#include <Contap_Point.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>

Contap_Line::Contap_Line () {
  svtx = new Contap_TheHSequenceOfPoint ();
  Trans = IntSurf_Undecided;
}

void Contap_Line::ResetSeqOfVertex() {
  svtx = new Contap_TheHSequenceOfPoint ();
}

void Contap_Line::Add(const Contap_Point& P) {
  Standard_Integer n = svtx->Length();
  if(n==0) { 
    svtx->Append(P);
  }
  else { 
    Standard_Real prm = P.ParameterOnLine();
    if(prm > svtx->Value(n).ParameterOnLine()) { 
      svtx->Append(P);      
    }
    else { 
      for(Standard_Integer i=n-1;i>0;i--) { 
        if(prm> svtx->Value(i).ParameterOnLine()) { 
          svtx->InsertBefore(i+1,P);
          return;
        }
      }
      svtx->Prepend(P);
    }
  }
}

void Contap_Line::Clear () {
  if(!curv.IsNull()) 
    curv->Clear();
  svtx = new Contap_TheHSequenceOfPoint ();
  typL = Contap_Walking;
}

void Contap_Line::SetValue(const gp_Lin& L)
{
  pt   = L.Location();
  dir1 = L.Direction();
  typL = Contap_Lin;
}

void Contap_Line::SetValue(const gp_Circ& C)
{
  pt   = C.Location();
  dir1 = C.Position().Direction();
  dir2 = C.Position().XDirection();
  rad  = C.Radius();
  typL = Contap_Circle;
}

void Contap_Line::SetValue(const Handle(Adaptor2d_HCurve2d)& A)
{
  thearc = A;
  typL = Contap_Restriction;
}

void Contap_Line::SetLineOn2S(const Handle(IntSurf_LineOn2S)& L) { 
  curv = L;
  typL = Contap_Walking;
}

void Contap_Line::SetTransitionOnS(const IntSurf_TypeTrans T) { 
  Trans = T;
}

IntSurf_TypeTrans Contap_Line::TransitionOnS() const { 
  return(Trans);
}

const Handle(Adaptor2d_HCurve2d)& Contap_Line::Arc () const
{
  if (typL != Contap_Restriction) {throw Standard_DomainError();}
  return thearc;
}
