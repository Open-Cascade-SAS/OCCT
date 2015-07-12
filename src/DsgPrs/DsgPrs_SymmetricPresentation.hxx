// Created on: 1997-01-22
// Created by: Prestataire Michael ALEONARD
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

#ifndef _DsgPrs_SymmetricPresentation_HeaderFile
#define _DsgPrs_SymmetricPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Drawer.hxx>
class Prs3d_Presentation;
class gp_Pnt;
class gp_Dir;
class gp_Lin;
class gp_Circ;


//! A framework to define display of symmetry between shapes.
class DsgPrs_SymmetricPresentation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds the points OffsetPoint, AttachmentPoint1,
  //! AttachmentPoint2, the direction aDirection1 and the
  //! axis anAxis to the presentation object aPresentation.
  //! The display attributes of the symmetry are defined by
  //! the attribute manager aDrawer.
  //! This syntax is used for display of symmetries between two segments.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const gp_Pnt& AttachmentPoint1, const gp_Pnt& AttachmentPoint2, const gp_Dir& aDirection1, const gp_Lin& aAxis, const gp_Pnt& OffsetPoint);
  
  //! Adds the points OffsetPoint, AttachmentPoint1,
  //! AttachmentPoint2, the direction aDirection1 the circle
  //! aCircle1 and the axis anAxis to the presentation
  //! object aPresentation.
  //! The display attributes of the symmetry are defined by
  //! the attribute manager aDrawer.
  //! This syntax is used for display of symmetries between two arcs.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const gp_Pnt& AttachmentPoint1, const gp_Pnt& AttachmentPoint2, const gp_Circ& aCircle1, const gp_Lin& aAxis, const gp_Pnt& OffsetPoint);
  
  //! Adds the points OffsetPoint, AttachmentPoint1,
  //! AttachmentPoint2 and the axis anAxis to the
  //! presentation object aPresentation.
  //! The display attributes of the symmetry are defined by
  //! the attribute manager aDrawer.
  //! This syntax is used for display of symmetries between two vertices.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const gp_Pnt& AttachmentPoint1, const gp_Pnt& AttachmentPoint2, const gp_Lin& aAxis, const gp_Pnt& OffsetPoint);




protected:





private:





};







#endif // _DsgPrs_SymmetricPresentation_HeaderFile
