// Created on: 2020-01-25
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <inspector/ViewControl_PropertyView.hxx>
#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_TableModel.hxx>
#include <inspector/ViewControl_TableModelValues.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAbstractTableModel>
#include <QHeaderView>
#include <QStackedWidget>
#include <QScrollArea>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

//! Class that uses parameter size as recommended size for the widget.
//! If the control is placed in a dock widget of the main window, it will not be resized on free size by resizing the main window.
class ViewControl_PredefinedSizeWidget : public QWidget
{
public:
  //! Constructor
  ViewControl_PredefinedSizeWidget (QWidget* theParent, const QSize& theSize) : QWidget (theParent) { SetPredefinedSize (theSize); }

  //! Destructor
  virtual ~ViewControl_PredefinedSizeWidget() {}

  //! Sets default size of control, that is used by the first control show
  //! \param theDefaultWidth the width value
  //! \param theDefaultHeight the height value
  void SetPredefinedSize (const QSize& theSize) { myDefaultSize = theSize;}

  //! Returns predefined size if both values are positive, otherwise parent size hint
  virtual QSize sizeHint() const Standard_OVERRIDE { return myDefaultSize.isValid() ? myDefaultSize : QWidget::sizeHint(); }

private:
  QSize myDefaultSize; //!< default size, empty size if it should not be used
};

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
ViewControl_PropertyView::ViewControl_PropertyView (QWidget* theParent, const QSize& thePredefinedSize)
: QObject (theParent), myOwnSelectionChangeBlocked (false)
{
  myMainWidget = new ViewControl_PredefinedSizeWidget (theParent, QSize (1, 100));
  if (!thePredefinedSize.isEmpty())
    ((ViewControl_PredefinedSizeWidget*)myMainWidget)->SetPredefinedSize (thePredefinedSize);

  QVBoxLayout* aLayout = new QVBoxLayout (myMainWidget);
  aLayout->setContentsMargins (0, 0, 0, 0);

  QScrollArea* anArea = new QScrollArea (myMainWidget);

  myAttributesStack = new QStackedWidget (myMainWidget);
  anArea->setWidget (myAttributesStack);
  anArea->setWidgetResizable( true );
  aLayout->addWidget (anArea);

  myEmptyWidget = new QWidget (myAttributesStack);
  myAttributesStack->addWidget (myEmptyWidget);

  myTableWidget = new QWidget (myAttributesStack);
  myTableWidgetLayout = new QVBoxLayout (myTableWidget);
  myTableWidgetLayout->setContentsMargins (0, 0, 0, 0);
  myAttributesStack->addWidget (myTableWidget);

  myAttributesStack->setCurrentWidget (myEmptyWidget);

  // create table
  ViewControl_Table* aTable = new ViewControl_Table (myMainWidget);
  ViewControl_TableModel* aModel = new ViewControl_TableModel(aTable->TableView());
  aTable->SetModel (aModel);

  connect (aTable->TableView()->selectionModel(),
          SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
          this, SLOT(onTableSelectionChanged (const QItemSelection&, const QItemSelection&)));

  connect (aModel, SIGNAL (dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
           this, SIGNAL (propertyViewDataChanged()));

  myTableWidgetLayout->addWidget (aTable->GetControl());
  myTable = aTable;
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void ViewControl_PropertyView::Init (ViewControl_TableModelValues* theTableValues)
{
  ViewControl_Table* aTable = Table();
  if (theTableValues)
  {
    aTable->Init (theTableValues);
    ViewControl_Tools::SetDefaultHeaderSections (aTable->TableView(), Qt::Horizontal);
  }
  aTable->SetActive (theTableValues != 0);

  if (theTableValues)
    myAttributesStack->setCurrentWidget (myTableWidget);
  else
    myAttributesStack->setCurrentWidget (myEmptyWidget);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void ViewControl_PropertyView::Init (QWidget*)
{
}

// =======================================================================
// function : ClearActiveTablesSelection
// purpose :
// =======================================================================
void ViewControl_PropertyView::ClearActiveTablesSelection()
{
  bool aWasBlocked = myOwnSelectionChangeBlocked;
  myOwnSelectionChangeBlocked = true;

  QList<ViewControl_Table*> aTables;
  ViewControl_Table* aTable = Table();
  if (aTable->IsActive())
    aTable->TableView()->selectionModel()->clearSelection();

  myOwnSelectionChangeBlocked = aWasBlocked;
}

// =======================================================================
// function : Clear
// purpose :
// =======================================================================
void ViewControl_PropertyView::Clear()
{
  ViewControl_Table* aTable = Table();
  if (aTable->IsActive())
  {
    ViewControl_TableModel* aModel = dynamic_cast<ViewControl_TableModel*> (aTable->TableView()->model());
    aModel->SetModelValues (0);

    aTable->SetActive (true);
  }
  myAttributesStack->setCurrentWidget (myEmptyWidget);
}

// =======================================================================
// function : SaveState
// purpose :
// =======================================================================
void ViewControl_PropertyView::SaveState (ViewControl_PropertyView* thePropertyView,
                                          QMap<QString, QString>& theItems,
                                          const QString& thePrefix)
{
  ViewControl_Table* aTable = thePropertyView->Table();
  int aColumnWidth = aTable->TableView()->columnWidth (0);
  theItems[thePrefix + "column_width_0"] = QString::number (aColumnWidth);
}

// =======================================================================
// function : RestoreState
// purpose :
// =======================================================================
bool ViewControl_PropertyView::RestoreState (ViewControl_PropertyView* thePropertyView,
                                             const QString& theKey,
                                             const QString& theValue,
                                             const QString& thePrefix)
{
  if (theKey == thePrefix + "column_width_0")
  {
    bool isOk;
    int aWidth = theValue.toInt (&isOk);
    if (isOk)
      thePropertyView->Table()->TableView()->horizontalHeader()->setDefaultSectionSize (aWidth);
  }
  else
    return false;
  return true;
}

// =======================================================================
// function : onTableSelectionChanged
// purpose :
// =======================================================================
void ViewControl_PropertyView::onTableSelectionChanged (const QItemSelection&, const QItemSelection&)
{
  if (myOwnSelectionChangeBlocked)
    return;

  emit propertyViewSelectionChanged();
}
