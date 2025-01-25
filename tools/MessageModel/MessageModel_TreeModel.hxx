// Created on: 2021-04-27
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef MessageModel_TreeModel_H
#define MessageModel_TreeModel_H

#include <inspector/MessageModel_ItemBase.hxx>
#include <inspector/MessageModel_ItemRoot.hxx>
#include <Standard.hxx>
#include <TCollection_AsciiString.hxx>
#include <Message_Report.hxx>
#include <inspector/TreeModel_ModelBase.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QMap>
#include <QObject>
#include <Standard_WarningsRestore.hxx>

class MessageModel_TreeModel;

//! \class MessageModel_TreeModel
//! View model to visualize MessageReport/s content
class MessageModel_TreeModel : public TreeModel_ModelBase
{
public:
  //! Constructor
  Standard_EXPORT MessageModel_TreeModel(QObject* theParent);

  //! Destructor
  virtual ~MessageModel_TreeModel() Standard_OVERRIDE {};

  //! Creates model columns and root items.
  Standard_EXPORT virtual void InitColumns() Standard_OVERRIDE;

  //!< Returns columns of the model for the metric
  //!< \param theMetricType metric
  //!< \param[out] theMetricColumns  container of metric columns
  static Standard_EXPORT void GetMetricColumns(const Message_MetricType theMetricType,
                                               QList<int>&              theMetricColumns);

  //!< Returns metric type for the column
  //!< \param[in] theColumnId  index of the tree column
  //!< \param[out] theMetricType  metric type if found
  //!< \param[out] thePosition  index of the metric column, 0 - is metric, 1 - is delta
  //!< \return true if the column has metric parameters
  static Standard_EXPORT bool IsMetricColumn(const int           theColumnId,
                                             Message_MetricType& theMetricType,
                                             int&                thePosition);

  //! Returns true if parameter report was added into the model
  //! \param theReport a report instance
  //! \return boolean value
  Standard_EXPORT Standard_Boolean HasReport(const Handle(Message_Report)& theReport);

  //! Add shape, append it to the model root item
  //! \param theReport a report instance
  //! \param theReportDescription an additional report information
  Standard_EXPORT void AddReport(const Handle(Message_Report)&  theReport,
                                 const TCollection_AsciiString& theReportDescription = "");

  //! Set report, se it into the given row index
  //! \param theRowId a report child row
  //! \param theReport a report instance
  //! \param theReportDescription an additional report information
  Standard_EXPORT void SetReport(const int                      theRowId,
                                 const Handle(Message_Report)&  theReport,
                                 const TCollection_AsciiString& theReportDescription = "");

  //!< Returns processed reports
  Standard_EXPORT const NCollection_List<MessageModel_ReportInformation>& Reports() const;

  //! Sets the text value of the Root item, only "Name" column accepts the parameter value
  //! \theName visualized text of root item
  Standard_EXPORT void SetRootItemName(const TCollection_AsciiString& theName);

  //! Updates tree model
  Standard_EXPORT void UpdateTreeModel();

protected:
  //! Creates root item
  //! \param theColumnId index of a column
  virtual TreeModel_ItemBasePtr createRootItem(const int theColumnId) Standard_OVERRIDE;
};

#endif
