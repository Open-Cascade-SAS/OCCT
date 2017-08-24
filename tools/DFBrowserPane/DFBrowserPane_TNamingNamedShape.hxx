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

#ifndef DFBrowserPane_TNamingNamedShape_H
#define DFBrowserPane_TNamingNamedShape_H

#include <inspector/DFBrowserPane_AttributePane.hxx>
#include <inspector/DFBrowserPane_HelperExport.hxx>

#include <Standard.hxx>
#include <TDF_Attribute.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Shape.hxx>

#include <QMap>
#include <QModelIndex>

class DFBrowserPane_TableView;
class DFBrowserPane_AttributePaneModel;

//! \class DFBrowserPane_TNamingNamedShape
//! \brief The class to manipulate of TNaming_NamedShape attribute
class DFBrowserPane_TNamingNamedShape : public DFBrowserPane_AttributePane
{
public:

  //! Constructor
  Standard_EXPORT DFBrowserPane_TNamingNamedShape();

  //! Destructor
  Standard_EXPORT virtual ~DFBrowserPane_TNamingNamedShape() {}

  //! Creates table view and call create widget of array table helper
  //! \param theParent a parent widget
  //! \return a new widget
  Standard_EXPORT virtual QWidget* CreateWidget (QWidget* theParent) Standard_OVERRIDE;

  //! Initializes the content of the pane by the parameter attribute
  //! \param theAttribute an OCAF attribute
  Standard_EXPORT void Init (const Handle(TDF_Attribute)& theAttribute);

  //! Returns information for the given attribute
  //! \param theAttribute a current attribute
  //! \param theRole a role of information, used by tree model (e.g. DisplayRole, icon, background and so on)
  //! \param theColumnId a tree model column
  //! \return value, interpreted by tree model depending on the role
  Standard_EXPORT virtual QVariant GetAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                     int theRole, int theColumnId) Standard_OVERRIDE;

  //! Returns brief attribute information. In general case, it returns GetValues() result.
  //! \param theAttribute a current attribute
  //! \param theValues a result list of values
  Standard_EXPORT virtual void GetShortAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                      QList<QVariant>& theValues)  Standard_OVERRIDE;

  //! Returns selection kind for the model, it may be General selection or Additional selection for example
  //! \param theModel one of selection models provided by this pane
  //! \return selection kind
  Standard_EXPORT virtual int GetSelectionKind (QItemSelectionModel* theModel) Standard_OVERRIDE;

  //! Returns selection parameters, that may be useful for communicate between tools
  //! \param theModel one of selection models provided by this pane
  //! \theParameters a container of parameters, might be extended depending on the pane state(e.g. selection)
  Standard_EXPORT virtual void GetSelectionParameters (QItemSelectionModel* theModel,
                                       NCollection_List<Handle(Standard_Transient)>& theParameters) Standard_OVERRIDE;

  //! Returns container of Label references to the attribute
  //! \param theAttribute a current attribute
  //! \param theRefLabels a container of label references, to be selected in tree view
  //! \param theRefPresentation handle of presentation for the references, to be visualized
  Standard_EXPORT virtual void GetReferences (const Handle(TDF_Attribute)& theAttribute,
                                              NCollection_List<TDF_Label>& theRefLabels,
                                              Handle(Standard_Transient)& theRefPresentation) Standard_OVERRIDE;

  //! Returns presentation of the attribute to be visualized in the view
  //! \param theAttribute a current attribute
  //! \return handle of presentation if the attribute has, to be visualized
  Standard_EXPORT virtual Handle(Standard_Transient) GetPresentation
    (const Handle (TDF_Attribute)& theAttribute) Standard_OVERRIDE;

  //! Returns values to fill the table view model
  //! \param theAttribute a current attribute
  //! \param theValues a container of values
  Standard_EXPORT virtual void GetValues (const Handle(TDF_Attribute)& theAttribute,
                                          QList<QVariant>& theValues) Standard_OVERRIDE;

protected:

  //! Returns a compound of selected shapes in both, values and evolution tables
  //! \return shape or NULL
  TopoDS_Shape getSelectedShapes();

private:

  DFBrowserPane_TableView* myEvolutionTableView; //!< table view for evolution shapes
  DFBrowserPane_AttributePaneModel* myEvolutionPaneModel;//!< view model for evolution shapes

  DFBrowserPane_HelperExport myHelperExport; //!<! helper to perform export to BREP
};

#endif
