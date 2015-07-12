// Created on: 1996-04-01
// Created by: Flore Lantheaume
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

#ifndef _DsgPrs_FixPresentation_HeaderFile
#define _DsgPrs_FixPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Drawer.hxx>
#include <Standard_Real.hxx>
class Prs3d_Presentation;
class gp_Pnt;
class gp_Dir;


//! class which draws the presentation of Fixed objects
class DsgPrs_FixPresentation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! draws the presentation of fixed objects by
  //! drawing the 'fix' symbol at position <aPntEnd>.
  //! A binding segment is drawn between <aPntAttach>
  //! ( which belongs the the fix object) and <aPntEnd>.
  //! aSymbSize is the size of the 'fix'symbol
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const gp_Pnt& aPntAttach, const gp_Pnt& aPntEnd, const gp_Dir& aNormPln, const Standard_Real aSymbSize);




protected:





private:





};







#endif // _DsgPrs_FixPresentation_HeaderFile
