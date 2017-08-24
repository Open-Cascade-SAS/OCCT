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

#ifndef DFBrowserPane_TDocStdOwner_H
#define DFBrowserPane_TDocStdOwner_H

#include <inspector/DFBrowserPane_AttributePane.hxx>

#include <Standard.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>
#include <TDF_DeltaList.hxx>

//! \class DFBrowserPane_TDocStdOwner
//! \brief The class to manipulate of TDocStd_Owner attribute
class DFBrowserPane_TDocStdOwner : public DFBrowserPane_AttributePane
{
public:

  //! Constructor
  Standard_EXPORT DFBrowserPane_TDocStdOwner();

  //! Destructor
  virtual ~DFBrowserPane_TDocStdOwner() {}

  //! Returns brief attribute information. In general case, it returns GetValues() result.
  //! \param theAttribute a current attribute
  //! \param theValues a result list of values
  Standard_EXPORT virtual void GetShortAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                      QList<QVariant>& theValues) Standard_OVERRIDE;

  //! Returns values to fill the table view model
  //! \param theAttribute a current attribute
  //! \param theValues a container of values
  Standard_EXPORT virtual void GetValues (const Handle(TDF_Attribute)& theAttribute,
                                          QList<QVariant>& theValues) Standard_OVERRIDE;

private:

  //! Generate string container by the parameter list
  //! \param a list of TDF delta
  //! \param string result
  static QString convertToString (const TDF_DeltaList& theList);

  //! Generate string container by the parameter list
  //! \param a list of extensions
  //! \param string result
  static QString convertToString (const TColStd_SequenceOfExtendedString& theExtensions);

  //! Generate string container by the ext string value
  //! \param an ext string [short*]
  //! \param string result
  static QString convertToString (const Standard_ExtString& theValue) { (void)theValue; return ""; }
};

#endif
