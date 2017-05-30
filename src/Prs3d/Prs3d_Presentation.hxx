// Created on: 1992-08-26
// Created by: Jean Louis FRENKEL
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

#ifndef _Prs3d_Presentation_HeaderFile
#define _Prs3d_Presentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_Structure.hxx>
#include <Standard_Boolean.hxx>

class Prs3d_Root;
class Graphic3d_StructureManager;
class Graphic3d_Structure;
class Graphic3d_DataStructureManager;
class Geom_Transformation;
class Graphic3d_Group;

class Prs3d_Presentation;
DEFINE_STANDARD_HANDLE(Prs3d_Presentation, Graphic3d_Structure)

//! Defines a presentation object which can be displayed,
//! highlighted or erased.
//! The presentation object stores the results of the
//! presentation algorithms as defined in the StdPrs
//! classes and the Prs3d classes inheriting Prs3d_Root.
//! This presentation object is used to give display
//! attributes defined at this level to
//! ApplicationInteractiveServices classes at the level above.
//! A presentation object is attached to a given Viewer.
class Prs3d_Presentation : public Graphic3d_Structure
{

public:

  //! Constructs a presentation object
  //! if <Init> is false, no color initialization is done.
  Standard_EXPORT Prs3d_Presentation(const Handle(Graphic3d_StructureManager)& theStructManager, const Standard_Boolean theToInit = Standard_True);
  
  //! Constructs a presentation object.
  Standard_EXPORT Prs3d_Presentation(const Handle(Graphic3d_StructureManager)& theStructManager, const Handle(Prs3d_Presentation)& thePrs);
  
  Standard_EXPORT virtual Handle(Graphic3d_Structure) Compute (const Handle(Graphic3d_DataStructureManager)& aProjector) Standard_OVERRIDE;
  
  //! Returns the new Structure defined for the new visualization
  Standard_EXPORT virtual Handle(Graphic3d_Structure) Compute (const Handle(Graphic3d_DataStructureManager)& theProjector,
                                                               const Handle(Geom_Transformation)& theTrsf) Standard_OVERRIDE;

  //! Returns the new Structure defined for the new visualization
  Standard_EXPORT virtual void Compute (const Handle(Graphic3d_DataStructureManager)& aProjector, Handle(Graphic3d_Structure)& aStructure) Standard_OVERRIDE;
  
  //! Returns the new Structure defined for the new visualization
  Standard_EXPORT virtual void Compute (const Handle(Graphic3d_DataStructureManager)& theProjector,
                                        const Handle(Geom_Transformation)& theTrsf,
                                        Handle(Graphic3d_Structure)& theStructure) Standard_OVERRIDE;

  Standard_EXPORT void Connect (const Handle(Prs3d_Presentation)& aPresentation);
  
  Standard_EXPORT void Remove (const Handle(Prs3d_Presentation)& aPresentation);
  
  Standard_EXPORT void RemoveAll();

friend class Prs3d_Root;

  DEFINE_STANDARD_RTTIEXT(Prs3d_Presentation,Graphic3d_Structure)

private:
  
  Standard_EXPORT Handle(Graphic3d_Group) CurrentGroup() const;

};

#endif // _Prs3d_Presentation_HeaderFile
