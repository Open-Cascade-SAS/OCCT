// Created on: 1993-04-15
// Created by: Jean-Louis Frenkel
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

#ifndef _Prs3d_Arrow_HeaderFile
#define _Prs3d_Arrow_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Root.hxx>
#include <Quantity_PlaneAngle.hxx>
#include <Quantity_Length.hxx>
class Prs3d_Presentation;
class gp_Pnt;
class gp_Dir;


//! provides class methods to draw an arrow at a given
//! location, along a given direction and using a given
//! angle.
class Prs3d_Arrow  : public Prs3d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Defines the representation of the arrow defined by
  //! the location point aLocation, the direction
  //! aDirection and the length aLength.
  //! The angle anAngle defines the angle of opening of the arrow head.
  //! The presentation object aPresentation stores the
  //! information defined in this framework.
  Standard_EXPORT static void Draw (const Handle(Prs3d_Presentation)& aPresentation, const gp_Pnt& aLocation, const gp_Dir& aDirection, const Quantity_PlaneAngle anAngle, const Quantity_Length aLength);
  
  //! Defines the representation of the arrow defined by
  //! the location point aLocation, the direction vector
  //! aDirection and the length aLength.
  //! The angle anAngle defines the angle of opening of
  //! the arrow head, and the drawer aDrawer specifies
  //! the display attributes which arrows will have.
  //! With this syntax, no presentation object is created.
  Standard_EXPORT static void Fill (const Handle(Prs3d_Presentation)& aPresentation, const gp_Pnt& aLocation, const gp_Dir& aDirection, const Quantity_PlaneAngle anAngle, const Quantity_Length aLength);




protected:





private:





};







#endif // _Prs3d_Arrow_HeaderFile
