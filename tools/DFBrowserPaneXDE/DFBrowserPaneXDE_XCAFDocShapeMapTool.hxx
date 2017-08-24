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

#ifndef DFBrowserPaneXDE_XCAFDocShapeMapTool_H
#define DFBrowserPaneXDE_XCAFDocShapeMapTool_H

#include <inspector/DFBrowserPane_AttributePane.hxx>
#include <inspector/DFBrowserPane_HelperExport.hxx>
#include <Standard.hxx>
#include <TDF_Attribute.hxx>

class DFBrowserPane_TableView;
class QWidget;

//! \class DFBrowserPaneXDE_
//! \brief The class to manipulate of XCAFDoc_ShapeMapTool attribute
class DFBrowserPaneXDE_XCAFDocShapeMapTool : public DFBrowserPane_AttributePane
{
public:
  //! Constructor
  Standard_EXPORT DFBrowserPaneXDE_XCAFDocShapeMapTool();

  //! Destructor
  virtual ~DFBrowserPaneXDE_XCAFDocShapeMapTool() {}

  //! Creates table view and call create widget of array table helper
  //! \param theParent a parent widget
  //! \return a new widget
  Standard_EXPORT virtual QWidget* CreateWidget (QWidget* theParent) Standard_OVERRIDE;

  //! Initializes the content of the pane by the parameter attribute
  //! \param theAttribute an OCAF attribute
  Standard_EXPORT virtual void Init (const Handle(TDF_Attribute)& theAttribute) Standard_OVERRIDE;

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

  //! Returns presentation of the attribute to be visualized in the view
  //! \param theAttribute a current attribute
  //! \return handle of presentation if the attribute has, to be visualized
  Standard_EXPORT virtual Handle(Standard_Transient) GetPresentation
    (const Handle(TDF_Attribute)& theAttribute) Standard_OVERRIDE;

private:

  DFBrowserPane_HelperExport myHelperExport; //!< processing of Export button click
};

#endif
