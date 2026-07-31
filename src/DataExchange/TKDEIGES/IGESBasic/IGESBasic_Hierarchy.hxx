// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Arun MENON )
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

#ifndef _IGESBasic_Hierarchy_HeaderFile
#define _IGESBasic_Hierarchy_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>

//! defines Hierarchy, Type <406> Form <10>
//! in package IGESBasic
//! Provides ability to control the hierarchy of each
//! directory entry attribute.
class IGESBasic_Hierarchy : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESBasic_Hierarchy();

  //! This method is used to set the fields of the class
  //! Hierarchy
  //! - nbPropVal     : Number of Property values = 6
  //! - aLineFont     : indicates the line font
  //! - aView         : indicates the view
  //! - aEntityLevel  : indicates the entity level
  //! - aBlankStatus  : indicates the blank status
  //! - aLineWt       : indicates the line weight
  //! - aColorNum     : indicates the color num
  //! aLineFont, aView, aEntityLevel, aBlankStatus, aLineWt and
  //! aColorNum can take 0 or 1.
  //! 0 : The directory entry attribute will apply to entities
  //! physically subordinate to this entity.
  //! 1 : The directory entry attribute of this entity will not
  //! apply to physically subordinate entities.
  Standard_EXPORT void Init(const int nbPropVal,
                            const int aLineFont,
                            const int aView,
                            const int anEntityLevel,
                            const int aBlankStatus,
                            const int aLineWt,
                            const int aColorNum);

  //! returns the number of property values, which should be 6
  Standard_EXPORT int NbPropertyValues() const;

  //! returns the line font
  Standard_EXPORT int NewLineFont() const;

  //! returns the view
  Standard_EXPORT int NewView() const;

  //! returns the entity level
  Standard_EXPORT int NewEntityLevel() const;

  //! returns the blank status
  Standard_EXPORT int NewBlankStatus() const;

  //! returns the line weight
  Standard_EXPORT int NewLineWeight() const;

  //! returns the color number
  Standard_EXPORT int NewColorNum() const;

  DEFINE_STANDARD_RTTIEXT(IGESBasic_Hierarchy, IGESData_IGESEntity)

private:
  int theNbPropertyValues;
  int theLineFont;
  int theView;
  int theEntityLevel;
  int theBlankStatus;
  int theLineWeight;
  int theColorNum;
};

#endif // _IGESBasic_Hierarchy_HeaderFile
