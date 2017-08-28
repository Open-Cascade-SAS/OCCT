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

#ifndef DFBrowserPane_HelperArray_H
#define DFBrowserPane_HelperArray_H

#include <Standard.hxx>
#include <TDF_Attribute.hxx>

class DFBrowserPane_AttributePaneModel;
class DFBrowserPane_TableView;

class QWidget;

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QList>
#include <QVariant>

//! \class DFBrowserPane_HelperArray
//! \brief Class that is used for list and array attributes. Two tables, the first for bounds, the second for values.
//! Bound table contains two values: Lower and Upper values of the container.
//! So, the first and the second values in GetValue() are these bounds, other values are used to fill usual table view.
class DFBrowserPane_HelperArray
{
public:

  //! Constructor
  Standard_EXPORT DFBrowserPane_HelperArray(DFBrowserPane_AttributePaneModel* theValuesModel);

  //! Destructor
  virtual ~DFBrowserPane_HelperArray() {}

  //! Creates bounds table view and grid layout, where the bounds view and the values view are shown
  //! \param theParent a parent widget
  //! \param theValuesView a view of values(table view filled by myValuesModel)
  Standard_EXPORT void CreateWidget(QWidget* theParent, DFBrowserPane_TableView* theValuesView);

  //! Fills bounds model by firsth and second values, fills values model by left values
  //! \param theValues values to fill views
  Standard_EXPORT void Init(const QList<QVariant>& theValues);

  //! Returns only values of values view model
  //! \param theAttribute a current attribute
  //! \param theValues a container of result values
  Standard_EXPORT virtual void GetShortAttributeInfo(const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues);

private:

  DFBrowserPane_AttributePaneModel* myValuesModel; //!< model of table view
  DFBrowserPane_AttributePaneModel* myBoundsModel; //!< model of bounds view
  DFBrowserPane_TableView* myArrayBounds; //!< bounds view
  DFBrowserPane_TableView* myValuesView; //!< values view
};
#endif
