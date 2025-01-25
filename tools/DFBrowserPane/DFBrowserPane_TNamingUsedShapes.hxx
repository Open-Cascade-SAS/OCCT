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

#ifndef DFBrowserPane_TNamingUsedShapes_H
#define DFBrowserPane_TNamingUsedShapes_H

#include <inspector/DFBrowserPane_AttributePane.hxx>

#include <Standard.hxx>
#include <TCollection_AsciiString.hxx>

class QWidget;

//! \class DFBrowserPane_TNamingUsedShapes
//! \brief The class to manipulate of TNaming_UsedShapes attribute
class DFBrowserPane_TNamingUsedShapes : public DFBrowserPane_AttributePane
{
public:
  //! Constructor
  Standard_EXPORT DFBrowserPane_TNamingUsedShapes();

  //! Destructor
  virtual ~DFBrowserPane_TNamingUsedShapes() {}

  //! Returns values to fill the table view model
  //! \param theAttribute a current attribute
  //! \param theValues a container of values
  Standard_EXPORT virtual void GetValues(const Handle(TDF_Attribute)& theAttribute,
                                         QList<QVariant>&             theValues) Standard_OVERRIDE;

  //! Returns brief attribute information. In general case, it returns GetValues() result.
  //! \param theAttribute a current attribute
  //! \param theValues a result list of values
  Standard_EXPORT virtual void GetShortAttributeInfo(const Handle(TDF_Attribute)& theAttribute,
                                                     QList<QVariant>& theValues) Standard_OVERRIDE;

  //! Returns container of Attribute references to the attribute
  //! \param theAttribute a current attribute
  //! \param theRefAttributes a container of attribute references, to be selected in tree view
  //! \param theRefPresentation handle of presentation for the references, to be visualized
  Standard_EXPORT virtual void GetAttributeReferences(
    const Handle(TDF_Attribute)&             theAttribute,
    NCollection_List<Handle(TDF_Attribute)>& theRefAttributes,
    Handle(Standard_Transient)&              theRefPresentation) Standard_OVERRIDE;

protected:
  //! Defines widths of table columns
  //! \return container of widths
  Standard_EXPORT virtual QMap<int, int> getTableColumnWidths() const Standard_OVERRIDE;
};

#endif
