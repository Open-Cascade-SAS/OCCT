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

#ifndef _Contap_Line_HeaderFile
#define _Contap_Line_HeaderFile

#include <Contap_IType.hxx>
#include <Contap_TheHSequenceOfPoint.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <IntSurf_TypeTrans.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Integer.hxx>

class IntSurf_LineOn2S;
class IntSurf_PntOn2S;
class gp_Lin;
class gp_Circ;
class Contap_Point;

class Contap_Line 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Contap_Line();
  
  Standard_EXPORT void SetLineOn2S (const Handle(IntSurf_LineOn2S)& L);
  
  Standard_EXPORT void Clear();
  
    const Handle(IntSurf_LineOn2S)& LineOn2S() const;
  
  Standard_EXPORT void ResetSeqOfVertex();
  
    void Add (const IntSurf_PntOn2S& P);
  
  Standard_EXPORT void SetValue (const gp_Lin& L);
  
  Standard_EXPORT void SetValue (const gp_Circ& C);
  
  Standard_EXPORT void SetValue (const Handle(Adaptor2d_Curve2d)& A);
  
  Standard_EXPORT void Add (const Contap_Point& P);
  
    Standard_Integer NbVertex() const;
  
    Contap_Point& Vertex (const Standard_Integer Index) const;
  
  //! Returns Contap_Lin for a line, Contap_Circle for
  //! a circle, and Contap_Walking for a Walking line,
  //! Contap_Restriction for a part of  boundarie.
    Contap_IType TypeContour() const;
  
    Standard_Integer NbPnts() const;
  
    const IntSurf_PntOn2S& Point (const Standard_Integer Index) const;
  
    gp_Lin Line() const;
  
    gp_Circ Circle() const;
  
  Standard_EXPORT const Handle(Adaptor2d_Curve2d)& Arc() const;
  
  //! Set The Tansition of the line.
  Standard_EXPORT void SetTransitionOnS (const IntSurf_TypeTrans T);
  
  //! returns IN if at the "left" of the line, the normale of the
  //! surface is oriented to the observator.
  Standard_EXPORT IntSurf_TypeTrans TransitionOnS() const;




protected:





private:



  IntSurf_TypeTrans Trans;
  Handle(IntSurf_LineOn2S) curv;
  Handle(Contap_TheHSequenceOfPoint) svtx;
  Handle(Adaptor2d_Curve2d) thearc;
  Contap_IType typL;
  gp_Pnt pt;
  gp_Dir dir1;
  gp_Dir dir2;
  Standard_Real rad;


};


#include <Contap_Line.lxx>





#endif // _Contap_Line_HeaderFile
