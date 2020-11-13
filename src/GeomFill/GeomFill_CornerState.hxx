// Created on: 1995-12-08
// Created by: Laurent BOURESCHE
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

#ifndef _GeomFill_CornerState_HeaderFile
#define _GeomFill_CornerState_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>


//! Class  (should    be  a  structure)   storing  the
//! information         about     continuity, normals
//! parallelism,  coons conditions and bounds tangents
//! angle on the corner of contour to be filled.
class GeomFill_CornerState 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomFill_CornerState();
  
  Standard_EXPORT Standard_Real Gap() const;
  
  Standard_EXPORT void Gap (const Standard_Real G);
  
  Standard_EXPORT Standard_Real TgtAng() const;
  
  Standard_EXPORT void TgtAng (const Standard_Real Ang);
  
  Standard_EXPORT Standard_Boolean HasConstraint() const;
  
  Standard_EXPORT void Constraint();
  
  Standard_EXPORT Standard_Real NorAng() const;
  
  Standard_EXPORT void NorAng (const Standard_Real Ang);
  
  Standard_EXPORT Standard_Boolean IsToKill (Standard_Real& Scal) const;
  
  Standard_EXPORT void DoKill (const Standard_Real Scal);




protected:





private:



  Standard_Real gap;
  Standard_Real tgtang;
  Standard_Boolean isconstrained;
  Standard_Real norang;
  Standard_Real scal;
  Standard_Boolean coonscnd;


};







#endif // _GeomFill_CornerState_HeaderFile
