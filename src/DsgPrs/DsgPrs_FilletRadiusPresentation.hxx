// Created on: 1997-12-08
// Created by: Serguei ZARITCHNY
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _DsgPrs_FilletRadiusPresentation_HeaderFile
#define _DsgPrs_FilletRadiusPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Drawer.hxx>
#include <Standard_Real.hxx>
#include <DsgPrs_ArrowSide.hxx>
#include <Standard_Boolean.hxx>
class Prs3d_Presentation;
class TCollection_ExtendedString;
class gp_Pnt;
class gp_Dir;
class Geom_TrimmedCurve;


//! A framework for displaying radii of fillets.
class DsgPrs_FilletRadiusPresentation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds a display of the radius of a fillet to the
  //! presentation aPresentation. The display ttributes
  //! defined by the attribute manager aDrawer. the value
  //! specifies the length of the radius.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const Standard_Real thevalue, const TCollection_ExtendedString& aText, const gp_Pnt& aPosition, const gp_Dir& aNormalDir, const gp_Pnt& aBasePnt, const gp_Pnt& aFirstPoint, const gp_Pnt& aSecondPoint, const gp_Pnt& aCenter, const DsgPrs_ArrowSide ArrowPrs, const Standard_Boolean drawRevers, gp_Pnt& DrawPosition, gp_Pnt& EndOfArrow, Handle(Geom_TrimmedCurve)& TrimCurve, Standard_Boolean& HasCircle);




protected:





private:





};







#endif // _DsgPrs_FilletRadiusPresentation_HeaderFile
