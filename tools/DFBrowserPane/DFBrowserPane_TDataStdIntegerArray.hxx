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

#ifndef DFBrowserPane_TDataStdIntegerArray_H
#define DFBrowserPane_TDataStdIntegerArray_H

#include <inspector/DFBrowserPane_AttributePane.hxx>
#include <inspector/DFBrowserPane_HelperArray.hxx>

#include <Standard.hxx>

//! \class DFBrowserPane_TDataStdIntegerArray
//! \brief The class to manipulate of TDataStd_IntegerArray attribute
class DFBrowserPane_TDataStdIntegerArray : public DFBrowserPane_AttributePane
{
public:
  //! Constructor
  Standard_EXPORT DFBrowserPane_TDataStdIntegerArray() : DFBrowserPane_AttributePane(), myArrayTableHelper(getPaneModel()) {}

  //! Destructor
  Standard_EXPORT virtual ~DFBrowserPane_TDataStdIntegerArray() {}

  //! Creates table view and call create widget of array table helper
  //! \param theParent a parent widget
  //! \return a new widget
  Standard_EXPORT virtual QWidget* CreateWidget (QWidget* theParent) Standard_OVERRIDE;

  //! Calls Init of array table helper
  //! \param theAttribute a current attribute
  Standard_EXPORT virtual void Init (const Handle(TDF_Attribute)& theAttribute) Standard_OVERRIDE;

  //! Returns short attribute information using array table helper
  //! \param theAttribute a current attribute
  //! \param theValues container of output values
  Standard_EXPORT virtual void GetShortAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                      QList<QVariant>& theValues) Standard_OVERRIDE;

  //! Returns values to fill the table view model
  //! \param theAttribute a current attribute
  //! \param theValues a container of values
  Standard_EXPORT virtual void GetValues (const Handle(TDF_Attribute)& theAttribute,
                                          QList<QVariant>& theValues) Standard_OVERRIDE;

private:

  DFBrowserPane_HelperArray myArrayTableHelper; //!< common interface to fill array pane
};

#endif
