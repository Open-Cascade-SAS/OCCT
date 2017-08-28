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

#include <inspector/DFBrowser_SearchLine.hxx>
#include <inspector/DFBrowser_SearchLineModel.hxx>
#include <inspector/DFBrowser_Window.hxx>

#include <inspector/DFBrowserPane_Tools.hxx>
#include <inspector/DFBrowser_Module.hxx>

#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>

#include <QStringList>
#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QCompleter>
#include <QIcon>
#include <QPaintEvent>
#include <QPainter>
#include <QStyle>

//! class DFBrowser_LineEdit
//! Extension of Qt line edit to visualize help text until the line edit control has not been filled yet
class DFBrowser_LineEdit : public QLineEdit
{
public:

  //! Constructor
  DFBrowser_LineEdit (QWidget* theParent) : QLineEdit(theParent) {}

  //! Destructor
  virtual ~DFBrowser_LineEdit() {}

  //! Sets text that is shown in line edit it the text of this control is empty
  //! \param theText a string value
  void setPlaneHolder (const QString& theText) { myPlaceHolder = theText; }

  //! Draws the line edit context, put plane holder if text is empty
  //! \param theEvent a paint event
  virtual void paintEvent (QPaintEvent* theEvent) Standard_OVERRIDE
  {
    QLineEdit::paintEvent (theEvent);
    if (!text().isEmpty())
      return;

    QPainter aPainter (this);
    QFontMetrics aFontMetrics = fontMetrics();
    QRect aLineRect = rect();
    Qt::Alignment anAlignment = QStyle::visualAlignment (layoutDirection(), Qt::AlignLeft);

    QColor aColor = Qt::gray;
    QPen anOldpen = aPainter.pen();
    aPainter.setPen (aColor);
    aLineRect.adjust (4, 4, 0, 0);
    QString anElidedText = aFontMetrics.elidedText (myPlaceHolder, Qt::ElideRight, aLineRect.width());
    aPainter.drawText (aLineRect, anAlignment, anElidedText);
    aPainter.setPen (anOldpen);
  }

private:

  QString myPlaceHolder; //!< text of filling line edit content if the text is empty
};

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowser_SearchLine::DFBrowser_SearchLine (QWidget* theParent)
: QFrame (theParent)
{
  QHBoxLayout* aLayout = new QHBoxLayout (this);
  aLayout->setContentsMargins (0, 0, 0, 0);
  aLayout->setSpacing (0);

  myLineControl = new DFBrowser_LineEdit (this);
  ((DFBrowser_LineEdit*)myLineControl)->setPlaneHolder (QString (tr ("Scanning application ...")));
  mySearchButton = new QToolButton (this);
  mySearchButton->setIcon (QIcon (":/icons/search.png"));

  QCompleter* aCompleter = new QCompleter (this);
  aCompleter->setCaseSensitivity (Qt::CaseInsensitive);
  myLineControl->setCompleter (aCompleter);

  aLayout->addWidget (myLineControl);
  aLayout->addWidget (mySearchButton);

  connect (myLineControl, SIGNAL (textChanged (const QString&)), this, SLOT (onTextChanged (const QString&)));
  connect (myLineControl, SIGNAL (returnPressed()), this, SLOT (onReturnPressed()));
  connect (mySearchButton, SIGNAL (clicked()), this, SLOT (onSearchButtonClicked()));

  DFBrowser_Window::SetWhiteBackground (this);
}

// =======================================================================
// function : SetModule
// purpose :
// =======================================================================
void DFBrowser_SearchLine::SetModule (DFBrowser_Module* theModule)
{
  DFBrowser_SearchLineModel* aModel = new DFBrowser_SearchLineModel (myLineControl, theModule);
  myLineControl->completer()->setModel (aModel);
}

// =======================================================================
// function : GetModule
// purpose :
// =======================================================================
DFBrowser_Module* DFBrowser_SearchLine::GetModule()
{
  DFBrowser_SearchLineModel* aModel = dynamic_cast<DFBrowser_SearchLineModel*> (GetModel());
  return aModel->GetModule();
}

// =======================================================================
// function : SetValues
// purpose :
// =======================================================================
void DFBrowser_SearchLine::SetValues (const QMap<int, QMap<QString, DFBrowser_SearchItemInfo > >& theDocumentValues,
                                      const QMap<int, QStringList>& theDocumentInfoValues)
{
  DFBrowser_SearchLineModel* aModel = dynamic_cast<DFBrowser_SearchLineModel*> (GetModel());
  aModel->SetValues (theDocumentValues, theDocumentInfoValues);

  QString aFirstValue = !theDocumentInfoValues.empty() ? theDocumentInfoValues.begin().value().first() : "";
  DFBrowser_LineEdit* aLineEdit = dynamic_cast<DFBrowser_LineEdit*> (myLineControl);
  aLineEdit->setPlaneHolder (QString (tr ("Search : %1")).arg (aFirstValue));
}

// =======================================================================
// function : ClearValues
// purpose :
// =======================================================================
void DFBrowser_SearchLine::ClearValues()
{
  DFBrowser_SearchLineModel* aModel = dynamic_cast<DFBrowser_SearchLineModel*> (GetModel());
  aModel->ClearValues();

  DFBrowser_LineEdit* aLineEdit = dynamic_cast<DFBrowser_LineEdit*> (myLineControl);
  aLineEdit->setPlaneHolder(QString (tr ("Scanning application ...")));
}

// =======================================================================
// function : onTextChanged
// purpose :
// =======================================================================
void DFBrowser_SearchLine::onTextChanged (const QString& theText)
{
  mySearchButton->setIcon (theText.isEmpty() ? QIcon (":/icons/search.png")
                                             : QIcon (":/icons/search_cancel.png"));
  emit searchActivated();
}

// =======================================================================
// function : onSearchButtonClicked
// purpose :
// =======================================================================
void DFBrowser_SearchLine::onSearchButtonClicked()
{
  if (!Text().isEmpty())
    SetText (QString());
}
