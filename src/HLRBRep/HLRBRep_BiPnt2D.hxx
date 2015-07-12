// Created on: 1992-08-21
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _HLRBRep_BiPnt2D_HeaderFile
#define _HLRBRep_BiPnt2D_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt2d.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class TopoDS_Shape;
class gp_Pnt2d;


//! Contains the colors of a shape.
class HLRBRep_BiPnt2D 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT HLRBRep_BiPnt2D();
  
  Standard_EXPORT HLRBRep_BiPnt2D(const Standard_Real x1, const Standard_Real y1, const Standard_Real x2, const Standard_Real y2, const TopoDS_Shape& S, const Standard_Boolean reg1, const Standard_Boolean regn, const Standard_Boolean outl, const Standard_Boolean intl);
  
    const gp_Pnt2d& P1() const;
  
    const gp_Pnt2d& P2() const;
  
    const TopoDS_Shape& Shape() const;
  
    void Shape (const TopoDS_Shape& S);
  
    Standard_Boolean Rg1Line() const;
  
    void Rg1Line (const Standard_Boolean B);
  
    Standard_Boolean RgNLine() const;
  
    void RgNLine (const Standard_Boolean B);
  
    Standard_Boolean OutLine() const;
  
    void OutLine (const Standard_Boolean B);
  
    Standard_Boolean IntLine() const;
  
    void IntLine (const Standard_Boolean B);




protected:





private:



  gp_Pnt2d myP1;
  gp_Pnt2d myP2;
  TopoDS_Shape myShape;
  Standard_Boolean myFlags;


};


#include <HLRBRep_BiPnt2D.lxx>





#endif // _HLRBRep_BiPnt2D_HeaderFile
