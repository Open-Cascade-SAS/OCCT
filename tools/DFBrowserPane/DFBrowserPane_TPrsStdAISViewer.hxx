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

#ifndef DFBrowserPane_TPrsStdAISViewer_H
#define DFBrowserPane_TPrsStdAISViewer_H

#include <inspector/DFBrowserPane_AttributePane.hxx>

#include <Standard.hxx>

//! \class DFBrowserPane_TPrsStdAISViewer
//! \brief The class to manipulate of TPrsStd_AISViewer attribute
class DFBrowserPane_TPrsStdAISViewer : public DFBrowserPane_AttributePane
{
public:
  //! Constructor
  DFBrowserPane_TPrsStdAISViewer() {}

  //! Destructor
  virtual ~DFBrowserPane_TPrsStdAISViewer() {}

  //! Returns values to fill the table view model
  //! \param theAttribute a current attribute
  //! \param theValues a container of values
  virtual void GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues) Standard_OVERRIDE;
};

#endif
