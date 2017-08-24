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

#ifndef DFBrowserPane_TDataStdIntPackedMap_H
#define DFBrowserPane_TDataStdIntPackedMap_H

#include <inspector/DFBrowserPane_AttributePane.hxx>

#include <Standard.hxx>

//! \class DFBrowserPane_TDataStdIntPackedMap
//! \brief The class to manipulate of TDataStd_IntPackedMap attribute
class DFBrowserPane_TDataStdIntPackedMap : public DFBrowserPane_AttributePane
{
public:

  //! Constructor
  DFBrowserPane_TDataStdIntPackedMap() {}

  //! Destructor
  virtual ~DFBrowserPane_TDataStdIntPackedMap() {}

  //! Returns values to fill the table view model
  //! \param theAttribute a current attribute
  //! \param theValues a container of values
  Standard_EXPORT virtual void GetValues (const Handle(TDF_Attribute)& theAttribute,
                                          QList<QVariant>& theValues) Standard_OVERRIDE;
};

#endif
