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

#ifndef DFBrowserPane_Tools_H
#define DFBrowserPane_Tools_H

#include <inspector/DFBrowserPane_OcctEnumType.hxx>

#include <Standard.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDF_Label.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QApplication>
#include <QString>
#include <QStyle>
#include <QString>
#include <QVariant>

#define PROPERTY_TABLE_ROW_COUNT 200

//! \class DFBrowserPane_Tools
//! \brief The tool that gives auxiliary methods for TDocStd elements manipulation
class DFBrowserPane_Tools
{
public:

  //! Returns default value for table view model: width[0] = 200, width[others] = 120
  //! \param theColumnId a column index
  //! \returns value
  Standard_EXPORT static int DefaultPanelColumnWidth (const int theColumnId);

  //! Returns header margin, defined in style settings of application
  //! \return integer value
  Standard_EXPORT static int HeaderSectionMargin() { return qApp->style()->pixelMetric (QStyle::PM_HeaderMargin); }

  //! Returns a string presentation of the label
  //! \param theLabel a label object
  //! \return the string value
  Standard_EXPORT static TCollection_AsciiString GetEntry (const TDF_Label& theLabel);

  //! Convert pointer to string value
  //! \param thePointer a pointer
  //! \param isShortInfo if true, all '0' symbols in the beginning of the pointer are skipped
  //! \return the string value
  Standard_EXPORT static TCollection_AsciiString GetPointerInfo (const Handle(Standard_Transient)& thePointer,
                                                                 const bool isShortInfo = true);

  //! Returns string value corresponded to the shape type if it is not null.
  //! \param theShape a checked shape
  //! \return string value or empty string value
  Standard_EXPORT static QVariant ShapeTypeInfo (const TopoDS_Shape& theShape);

  //! Returns "true" or "false" text for the given boolean state
  //! \param theValue a boolean value
  //! \return string value
  Standard_EXPORT static QString BoolToStr (const bool theValue) { return theValue ? "true" : "false"; }

  //! Converts a Qt string to OCCT extended string
  //! \param theValue a converted string
  //! \return the extended string value
  Standard_EXPORT static QString ToString(const TCollection_ExtendedString& theValue)
  {
    return QString::fromUtf16((uint16_t*)theValue.ToExtString());
  }

  //! Returns light highlight color
  //! \returns Qt color
  static Standard_EXPORT QColor LightHighlightColor();

  //! Converts enumeration value into string text
  //! \param theType an enumeration kind
  //! \param theEnumId an enumeration value
  //! \return string presentation
  Standard_EXPORT static TCollection_AsciiString ToName (const DFBrowserPane_OcctEnumType& theType,
                                                         const Standard_Integer& theEnumId);
};

#endif
