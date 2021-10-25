// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Niraj RANGWALA )
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

#ifndef _IGESDraw_ViewsVisible_HeaderFile
#define _IGESDraw_ViewsVisible_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESDraw_HArray1OfViewKindEntity.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class IGESData_IGESEntity;


class IGESDraw_ViewsVisible;
DEFINE_STANDARD_HANDLE(IGESDraw_ViewsVisible, IGESData_ViewKindEntity)

//! Defines IGESViewsVisible, Type <402>, Form <3>
//! in package IGESDraw
//!
//! If an entity is to be displayed in more than one views,
//! this class instance is used, which contains the Visible
//! views and the associated entity Displays.
class IGESDraw_ViewsVisible : public IGESData_ViewKindEntity
{

public:

  
  Standard_EXPORT IGESDraw_ViewsVisible();
  
  //! This method is used to set the fields of the class
  //! ViewsVisible
  //! - allViewEntities  : All View kind entities
  //! - allDisplayEntity : All entities whose display is specified
  Standard_EXPORT void Init (const Handle(IGESDraw_HArray1OfViewKindEntity)& allViewEntities, const Handle(IGESData_HArray1OfIGESEntity)& allDisplayEntity);
  
  //! Changes only the list of Displayed Entities (Null allowed)
  Standard_EXPORT void InitImplied (const Handle(IGESData_HArray1OfIGESEntity)& allDisplayEntity);
  
  //! Returns False (for a complex view)
  Standard_EXPORT Standard_Boolean IsSingle() const Standard_OVERRIDE;
  
  //! returns the Number of views visible
  Standard_EXPORT Standard_Integer NbViews() const Standard_OVERRIDE;
  
  //! returns the number of entities displayed in the Views or zero if
  //! no Entities specified in these Views
  Standard_EXPORT Standard_Integer NbDisplayedEntities() const;
  
  //! returns the Index'th ViewKindEntity Entity
  //! raises exception if Index  <= 0 or Index > NbViewsVisible()
  Standard_EXPORT Handle(IGESData_ViewKindEntity) ViewItem (const Standard_Integer Index) const Standard_OVERRIDE;
  
  //! returns the Index'th entity whose display is being specified by
  //! this associativity instance
  //! raises exception if Index  <= 0 or Index > NbEntityDisplayed()
  Standard_EXPORT Handle(IGESData_IGESEntity) DisplayedEntity (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESDraw_ViewsVisible,IGESData_ViewKindEntity)

protected:




private:


  Handle(IGESDraw_HArray1OfViewKindEntity) theViewEntities;
  Handle(IGESData_HArray1OfIGESEntity) theDisplayEntity;


};







#endif // _IGESDraw_ViewsVisible_HeaderFile
