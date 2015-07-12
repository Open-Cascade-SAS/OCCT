// Created on: 1995-02-07
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _GeomInt_LineConstructor_HeaderFile
#define _GeomInt_LineConstructor_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
class Adaptor3d_TopolTool;
class GeomAdaptor_HSurface;
class StdFail_NotDone;
class Standard_OutOfRange;
class IntPatch_Line;


//! Splits given Line.
class GeomInt_LineConstructor 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Empty constructor
    GeomInt_LineConstructor();
  

  //! Initializes me by two surfaces and corresponding
  //! tools which represent boundaries of surfaces
    void Load (const Handle(Adaptor3d_TopolTool)& D1, const Handle(Adaptor3d_TopolTool)& D2, const Handle(GeomAdaptor_HSurface)& S1, const Handle(GeomAdaptor_HSurface)& S2);
  

  //! Splits line
  Standard_EXPORT void Perform (const Handle(IntPatch_Line)& L);
  

  //! Returns True if splitting was successful
    Standard_Boolean IsDone() const;
  

  //! Returns number of splits
    Standard_Integer NbParts() const;
  

  //! Return first and last parameters
  //! for given index of split
    void Part (const Standard_Integer I, Standard_Real& WFirst, Standard_Real& WLast) const;




protected:

  
  Standard_EXPORT void TreatCircle (const Handle(IntPatch_Line)& aLine, const Standard_Real aTol);




private:



  Standard_Boolean done;
  TColStd_SequenceOfReal seqp;
  Handle(Adaptor3d_TopolTool) myDom1;
  Handle(Adaptor3d_TopolTool) myDom2;
  Handle(GeomAdaptor_HSurface) myHS1;
  Handle(GeomAdaptor_HSurface) myHS2;


};


#include <GeomInt_LineConstructor.lxx>





#endif // _GeomInt_LineConstructor_HeaderFile
