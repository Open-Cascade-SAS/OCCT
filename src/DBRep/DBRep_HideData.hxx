// Created on: 1995-09-21
// Created by: Remi LEQUETTE
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

#ifndef _DBRep_HideData_HeaderFile
#define _DBRep_HideData_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Trsf.hxx>
#include <HLRBRep_ListOfBPoint.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Boolean.hxx>
class Draw_Display;
class Draw_Color;


//! This  class stores all the information  concerning
//! hidden lines on a view.
//!
//! * View number
//! * Matrix of projection
//! * Type of projection, focal
//! * Arrays of 3d points
//!
//! A drawable shape contains a  list of such  objects
//! to store  the  hidden lines  for  each view.   The
//! IsSame method is  used  to check if   hidden lines
//! must be recompiled.
class DBRep_HideData 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT DBRep_HideData();
  
  //! ViewId is the view number
  //! TProj the projection
  //! Focal <= 0 means parallel projection
  //! Alg : the hidden lines
  Standard_EXPORT void Set (const Standard_Integer ViewId, const gp_Trsf& TProj, const Standard_Real Focal, const TopoDS_Shape& S, const Standard_Real ang);

  Standard_Integer ViewId() const { return myView; }

  Standard_Real Angle() const { return myAngle; }

  //! Returns True if the projection is the same
  Standard_EXPORT Standard_Boolean IsSame (const gp_Trsf& TProj, const Standard_Real Focla) const;
  
  Standard_EXPORT void DrawOn (Draw_Display& D, const Standard_Boolean withRg1, const Standard_Boolean withRgN, const Standard_Boolean withHid, const Draw_Color& VisCol, const Draw_Color& HidCol);
  
  //! Returns the subshape touched by the last pick.
  Standard_EXPORT const TopoDS_Shape& LastPick() const;

private:

  Standard_Integer myView;
  gp_Trsf myTrsf;
  Standard_Real myFocal;
  HLRBRep_ListOfBPoint myBiPntVis;
  HLRBRep_ListOfBPoint myBiPntHid;
  TopoDS_Shape myPickShap;
  Standard_Real myAngle;

};

#endif // _DBRep_HideData_HeaderFile
