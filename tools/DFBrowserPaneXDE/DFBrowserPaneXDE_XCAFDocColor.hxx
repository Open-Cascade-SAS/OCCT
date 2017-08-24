// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef DFBrowserPaneXDE_XCAFDocColor_H
#define DFBrowserPaneXDE_XCAFDocColor_H

#include <inspector/DFBrowserPane_AttributePane.hxx>

#include <Standard.hxx>
#include <TDF_Attribute.hxx>

//! \class DFBrowserPaneXDE_XCAFDocColor
//! \brief The class to manipulate of XCAFDoc_Color attribute
class DFBrowserPaneXDE_XCAFDocColor : public DFBrowserPane_AttributePane
{
public:
  //! Constructor
  Standard_EXPORT DFBrowserPaneXDE_XCAFDocColor();

  //! Destructor
  virtual ~DFBrowserPaneXDE_XCAFDocColor() {}

  //! Returns values to fill the table view model
  //! \param theAttribute a current attribute
  //! \param theValues a container of values
  Standard_EXPORT virtual void GetValues (const Handle(TDF_Attribute)& theAttribute,
                                          QList<QVariant>& theValues) Standard_OVERRIDE;

  //! Returns brief attribute information. In general case, it returns GetValues() result.
  //! \param theAttribute a current attribute
  //! \param theValues a result list of values
  Standard_EXPORT virtual void GetShortAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                      QList<QVariant>& theValues) Standard_OVERRIDE;

  //! Returns information for the given attribute
  //! \param theAttribute a current attribute
  //! \param theRole a role of information, used by tree model (e.g. DisplayRole, icon, background and so on)
  //! \param theColumnId a tree model column
  //! \return value, interpreted by tree model depending on the role
  Standard_EXPORT virtual QVariant GetAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                     int theRole, int theColumnId) Standard_OVERRIDE;
};

#endif
