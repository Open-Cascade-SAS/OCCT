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

#include <inspector/ViewControl_ColorSelector.hxx>
#include <inspector/ViewControl_TableItemDelegate.hxx>
#include <inspector/ViewControl_TableModel.hxx>
#include <inspector/ViewControl_TableModelValues.hxx>
#include <inspector/ViewControl_Tools.hxx>
#include <inspector/TreeModel_Tools.hxx>

#include <Standard_Dump.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAbstractTableModel>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QGridLayout>
#include <QItemSelectionModel>
#include <QPainter>
#include <QPushButton>
#include <QTableView>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

//! Kinds of delegate cell in OCCT Color model to present a custom presentation (rect bounded by a colored frame)
enum ViewControl_ColorDelegateKind
{
  ViewControl_ColorDelegateKind_None, //!< usual item
  ViewControl_ColorDelegateKind_Activated, //!< active item
  ViewControl_ColorDelegateKind_Highlighted, //!< highlighted item
  ViewControl_ColorDelegateKind_Selected //!< selected item
};

//! Model for a table of parameters: Current Color, Red, Green, Blue, Alpha, OCCT color name
class ViewControl_ParametersModel : public ViewControl_TableModelValues
{
public:
  ViewControl_ParametersModel (ViewControl_ColorSelector* theSelector)
    : ViewControl_TableModelValues(), mySelector (theSelector) {}
  virtual ~ViewControl_ParametersModel() {}

  //! Inits model by the parameter color
  //! \param theColor model active color
  void SetColor (const Quantity_ColorRGBA& theColor, ViewControl_TableModel* theModel)
  { myColor = theColor; theModel->EmitLayoutChanged(); }

  //! Returns current selected color
  //! \return color value
  Quantity_ColorRGBA GetColor() const { return myColor; }

  //! Returns item information(short) for display role.
  //! \param theIndex a model index
  //! \param theRole a view role
  //! \return value intepreted depending on the given role
  Standard_EXPORT virtual QVariant Data (const int theRow, const int theColumn,
                                         int theRole = Qt::DisplayRole) const Standard_OVERRIDE
  {
    //(void)theRow; (void)theColumn; (void) theRole;
    if (theRole == Qt::BackgroundRole && theColumn == 1 && theRow == 0)
      return ViewControl_ColorSelector::ColorToQColor (myColor);

    if (theRole == Qt::ForegroundRole && theColumn == 1 && theRow >= 2 && theRow <= 5)
      return ViewControl_TableModelValues::EditCellColor();

    if (theRole != Qt::DisplayRole)
    return QVariant();

    bool isFirstColumn = theColumn == 0;
    switch (theRow)
    {
      case 0: return isFirstColumn ? QVariant ("Color") : QVariant ();
      case 1:
      {
        if (isFirstColumn)
          return QVariant ("Name");
        Quantity_NameOfColor aColorName;
        if (ViewControl_ColorSelector::IsExactColorName(myColor, aColorName))
          return Quantity_Color::StringName(aColorName);
      }
      break;
      case 2: return isFirstColumn ? QVariant ("Red") : ViewControl_Tools::ToVariant (myColor.GetRGB().Red());
      case 3: return isFirstColumn ? QVariant ("Green") : ViewControl_Tools::ToVariant (myColor.GetRGB().Green());
      case 4: return isFirstColumn ? QVariant ("Blue") : ViewControl_Tools::ToVariant (myColor.GetRGB().Blue());
      case 5: return isFirstColumn ? QVariant ("Alpha") : ViewControl_Tools::ToVariant (myColor.Alpha());
      case 6: return isFirstColumn ? QVariant ("Near Name") 
                                   : Quantity_Color::StringName(myColor.GetRGB().Name());
    }
    return QVariant();
  }

  //! Sets content of the model index for the given role, it is applyed to internal container of values
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param theRole a view role
  //! \return true if the value is changed
  virtual bool SetData (const int theRow, const int theColumn, const QVariant& theValue, int) Standard_OVERRIDE
  {
    if (theColumn != 1 || theRow < 2 || theRow > 5)
      return false;

    switch (theRow)
    {
      case 2:
      case 3:
      case 4:
      {
        myColor.ChangeRGB().SetValues (theRow == 2 ? ViewControl_Tools::ToShortRealValue (theValue) : myColor.GetRGB().Red(),
                                       theRow == 3 ? ViewControl_Tools::ToShortRealValue (theValue) : myColor.GetRGB().Green(),
                                       theRow == 4 ? ViewControl_Tools::ToShortRealValue (theValue) : myColor.GetRGB().Blue(),
                                       Quantity_TOC_RGB);
      }
      break;
      case 5: myColor.SetAlpha (ViewControl_Tools::ToShortRealValue (theValue)); break;
    }
    mySelector->ParameterColorChanged();
    return true;
  }

  //! Returns number of tree level line items = colums in table view
  virtual int ColumnCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 2; }

  //! Returns onlly one row in table view
  virtual int RowCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 7; }

  //! Returns editable flag for color RGB and alpha rows
  //! \return flags
  virtual Qt::ItemFlags Flags (const QModelIndex& theIndex) const Standard_OVERRIDE
  {
    Qt::ItemFlags aFlags = ViewControl_TableModelValues::Flags (theIndex);

    if (theIndex.column() == 1 && theIndex.row() >= 2 && theIndex.row() <= 5)
      aFlags = aFlags | Qt::ItemIsEditable;

    return aFlags;
  }

  //! Returns type of edit control for the model index. By default, it is an empty control
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \return edit type
  virtual ViewControl_EditType GetEditType (const int theRow, const int theColumn) const
  {
    if (theColumn == 1 && theRow >= 2 && theRow <= 5)
      return ViewControl_EditType_Double;

    return ViewControl_EditType_None;
  }

private:
  Quantity_ColorRGBA myColor;
  ViewControl_ColorSelector* mySelector;
};

//! Table of parameters: Red, Green, Blue, Alpha, OCCT color name
class ViewControl_OCCTColorModel : public QAbstractTableModel
{
public:
  ViewControl_OCCTColorModel (QObject* theParent)
    : QAbstractTableModel (theParent), myCurrentIndexKind (ViewControl_ColorDelegateKind_None) {}
  virtual ~ViewControl_OCCTColorModel() {}

  //! Sets current color, that should have custom presented
  //! \param theColor current color
  //! \param theKind presentation kind
  void SetColor (const Quantity_NameOfColor& theColor, const ViewControl_ColorDelegateKind theKind)
  {
    int aColorPosition = (int)theColor;
    int aRow = (int) (aColorPosition / columnCount());
    int aColumn = aColorPosition - aRow * columnCount();
    myCurrentIndex = index (aRow, aColumn);
    myCurrentIndexKind = theKind;

    emit layoutChanged();
  }

  //! Returns OCCT name of color if index position does not exceed Quantity_NameOfColor elements
  //! \param theIndex model index
  //! \param theNameOfColor [out] OCCT color name
  //! \return true if the color is found
  bool GetOCCTColor (const QModelIndex& theIndex, Quantity_NameOfColor& theNameOfColor) const
  {
    int aNameOfColorId = theIndex.row() * columnCount() + theIndex.column();
    if (aNameOfColorId > Quantity_NOC_WHITE)
      return false;
    theNameOfColor = (Quantity_NameOfColor)aNameOfColorId;
    return true;
  }

  //! Returns index that has custom presentation
  //! \return model index
  QModelIndex GetCurrentIndex() const { return myCurrentIndex; }

  //! Returns index color kind that has custom presentation
  //! \return kind
  ViewControl_ColorDelegateKind GetCurrentIndexKind() const { return myCurrentIndexKind; }

  //! Returns item information(short) for display role.
  //! \param theIndex a model index
  //! \param theRole a view role
  //! \return value intepreted depending on the given role
  Standard_EXPORT virtual QVariant data (const QModelIndex& theIndex,
                                         int theRole = Qt::DisplayRole) const Standard_OVERRIDE
  {
    if (theRole != Qt::ToolTipRole) // background is processed in table item delegate
      return QVariant();

    Quantity_NameOfColor aNameOfColor;
    if (!GetOCCTColor (theIndex, aNameOfColor))
      return QVariant();

    if (theRole == Qt::ToolTipRole)
      return QString("%1").arg (ViewControl_ColorSelector::ColorToString (Quantity_Color (aNameOfColor)));
    return QVariant();
  }

  //! Returns number of tree level line items = colums in table view
  virtual int columnCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 26; }

  //! Returns onlly one row in table view
  virtual int rowCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 20; }

  //! Returns color for the delegate kind
  //! \param theKind kind
  //! \return color
  static QColor GetKindColor (const ViewControl_ColorDelegateKind theKind)
  {
    switch (theKind)
    {
      case ViewControl_ColorDelegateKind_Activated:
      case ViewControl_ColorDelegateKind_Highlighted: return Qt::blue;
      case ViewControl_ColorDelegateKind_Selected: return Qt::black;
      default: break;
    }
    return QColor();
  }

private:
  QModelIndex myCurrentIndex; //!< index to be presented through item delegate
  ViewControl_ColorDelegateKind myCurrentIndexKind; //!< kind of custom item
};

//! \class DFBrowser_HighlightDelegate
//! \brief An item delegate to paint in highlight color the cell when the mouse cursor is over it
class ViewControl_OCCTColorDelegate : public QItemDelegate
{
public:

  //! Constructor
  ViewControl_OCCTColorDelegate (QObject* theParent = 0) : QItemDelegate (theParent) {}

  //! Destructor
  virtual ~ViewControl_OCCTColorDelegate() Standard_OVERRIDE {}

  //! Redefine of the parent virtual method to color the cell rectangle in highlight style
  //! \param thePainter a painter
  //! \param theOption a paint options
  //! \param theIndex a view index
  virtual void paint (QPainter* thePainter, const QStyleOptionViewItem& theOption,
                      const QModelIndex& theIndex) const Standard_OVERRIDE
  {
    QRect aBaseRect = theOption.rect;
    int aNameOfColorId = theIndex.row() * theIndex.model()->columnCount(theIndex) + theIndex.column();
    Quantity_NameOfColor aNameOfColor = Quantity_NOC_WHITE;
    if (aNameOfColorId < (int)Quantity_NOC_WHITE)
      aNameOfColor = (Quantity_NameOfColor)aNameOfColorId;

    Quantity_Color anOCCTColor (aNameOfColor);
    QColor aQColor = ViewControl_ColorSelector::ColorToQColor (Quantity_ColorRGBA (anOCCTColor));
    thePainter->fillRect (aBaseRect, aQColor);

    QColor aColor;
    // highlight cell
    if (theOption.state & QStyle::State_MouseOver)
      aColor = ViewControl_OCCTColorModel::GetKindColor (ViewControl_ColorDelegateKind_Highlighted);
    else
    {
      const ViewControl_OCCTColorModel* aTableModel = dynamic_cast<const ViewControl_OCCTColorModel*> (theIndex.model());
      QModelIndex anIndex = aTableModel->GetCurrentIndex();
      if (anIndex.isValid() && anIndex.row() == theIndex.row() && anIndex.column() == theIndex.column())
        aColor = ViewControl_OCCTColorModel::GetKindColor (aTableModel->GetCurrentIndexKind());
    }
    
    if (aColor.isValid())
    {
      int aRectSize = 2;
      thePainter->fillRect (QRect (aBaseRect.topLeft(), QPoint (aBaseRect.bottomLeft().x() + aRectSize, aBaseRect.bottomLeft().y())),
                            aColor);
      thePainter->fillRect (QRect (QPoint (aBaseRect.topRight().x() - aRectSize, aBaseRect.topRight().y()), aBaseRect.bottomRight()),
                            aColor);
      thePainter->fillRect (QRect (QPoint (aBaseRect.topLeft().x() + aRectSize, aBaseRect.topLeft().y()),
                                   QPoint (aBaseRect.topRight().x() - aRectSize, aBaseRect.topRight().y() + aRectSize)),
                            aColor);
      thePainter->fillRect (QRect (QPoint (aBaseRect.bottomLeft().x() + aRectSize, aBaseRect.bottomLeft().y() - aRectSize),
                                   QPoint (aBaseRect.bottomRight().x() - aRectSize, aBaseRect.bottomRight().y())),
                            aColor);
    }
  }
};

//! Color picker class
class ViewControl_ColorPicker : public QWidget
{
public:
  ViewControl_ColorPicker (QWidget* theParent) : QWidget (theParent) {}
  virtual ~ViewControl_ColorPicker() {}
};

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
ViewControl_ColorSelector::ViewControl_ColorSelector (QWidget* theParent)
: QDialog (theParent)
{
  QGridLayout* aLayout = new QGridLayout (this);
  aLayout->setContentsMargins (0, 0, 0, 0);

  myParameters = new QTableView (this);
  ViewControl_TableModel* aTableModel = new ViewControl_TableModel (myParameters);
  aTableModel->SetModelValues (new ViewControl_ParametersModel (this));
  myParameters->setModel(aTableModel);

  ViewControl_TableItemDelegate* anItemDelegate = new ViewControl_TableItemDelegate();
  anItemDelegate->SetModelValues (aTableModel->ModelValues());
  myParameters->setItemDelegate(anItemDelegate);

  myParameters->verticalHeader()->setDefaultSectionSize (myParameters->verticalHeader()->minimumSectionSize());
  myParameters->verticalHeader()->setVisible (false);
  myParameters->horizontalHeader()->setVisible (false);
  myParameters->setMinimumHeight (myParameters->verticalHeader()->minimumSectionSize() * aTableModel->rowCount() +
                                  TreeModel_Tools::HeaderSectionMargin());

  QItemSelectionModel* aSelectionModel = new QItemSelectionModel (myParameters->model());
  myParameters->setSelectionMode (QAbstractItemView::SingleSelection);
  myParameters->setSelectionModel (aSelectionModel);

  aLayout->addWidget (myParameters, 0, 0);

  myColorPicker = new ViewControl_ColorPicker (this);
  aLayout->addWidget (myColorPicker, 0, 1);

  myOCCTColors = new QTableView (this);
  myOCCTColors->setFixedSize (525, 405);
  myOCCTColors->verticalHeader()->setDefaultSectionSize (20);
  myOCCTColors->verticalHeader()->setVisible (false);
  myOCCTColors->horizontalHeader()->setDefaultSectionSize (20);
  myOCCTColors->horizontalHeader()->setVisible (false);
  myOCCTColors->setModel(new ViewControl_OCCTColorModel(myOCCTColors));

  myOCCTColors->viewport()->setAttribute (Qt::WA_Hover);
  myOCCTColors->setItemDelegate (new ViewControl_OCCTColorDelegate (myOCCTColors));

  aSelectionModel = new QItemSelectionModel (myOCCTColors->model());
  myOCCTColors->setSelectionMode (QAbstractItemView::SingleSelection);
  myOCCTColors->setSelectionModel (aSelectionModel);
  connect (aSelectionModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
          this, SLOT (onOCCTColorsTableSelectionChanged (const QItemSelection&, const QItemSelection&)));
  aLayout->addWidget (myOCCTColors, 1, 0, 1, 2);

  QWidget* aBtnWidget = new QWidget (this);
  aLayout->addWidget (aBtnWidget, 2, 0, 1, 2);

  QHBoxLayout* aBtnLayout = new QHBoxLayout (aBtnWidget);
  myOkButton = new QPushButton ("Ok", aBtnWidget);
  myCancelButton = new QPushButton ("Cancel", aBtnWidget);
  connect (myOkButton, SIGNAL (clicked()), this, SLOT (accept()));
  connect (myCancelButton, SIGNAL (clicked()), this, SLOT (reject()));
  aBtnLayout->addStretch ();
  aBtnLayout->addWidget (myOkButton);
  aBtnLayout->addWidget (myCancelButton);
}

// =======================================================================
// function : SetStreamValue
// purpose :
// =======================================================================
void ViewControl_ColorSelector::SetStreamValue (const QString& theValue)
{
  Quantity_ColorRGBA aColor = StringToColorRGBA (theValue);
  // parameters model
  ViewControl_TableModel* aTableModel = dynamic_cast<ViewControl_TableModel*> (myParameters->model());
  ViewControl_ParametersModel* aParametersModel = dynamic_cast<ViewControl_ParametersModel*> (aTableModel->ModelValues());
  aParametersModel->SetColor (aColor, aTableModel);

  // OCCT color model
  Quantity_NameOfColor aColorName;
  bool isExactColorName = ViewControl_ColorSelector::IsExactColorName(aColor, aColorName);
  ViewControl_OCCTColorModel* anOCCTColorModel = dynamic_cast<ViewControl_OCCTColorModel*>(myOCCTColors->model());
  anOCCTColorModel->SetColor (aColorName, isExactColorName ? ViewControl_ColorDelegateKind_Selected
                                                           : ViewControl_ColorDelegateKind_Activated);
}

// =======================================================================
// function : GetStreamValue
// purpose :
// =======================================================================
QString ViewControl_ColorSelector::GetStreamValue() const
{
  ViewControl_TableModel* aTableModel = dynamic_cast<ViewControl_TableModel*> (myParameters->model());
  ViewControl_ParametersModel* aParametersModel = dynamic_cast<ViewControl_ParametersModel*> (aTableModel->ModelValues());

  Quantity_ColorRGBA aColor = aParametersModel->GetColor();

  Standard_SStream aStream;
  aColor.DumpJson (aStream);

  return Standard_Dump::Text (aStream).ToCString();
}

// =======================================================================
// function : ParameterColorChanged
// purpose :
// =======================================================================
void ViewControl_ColorSelector::ParameterColorChanged()
{
  ViewControl_TableModel* aTableModel = dynamic_cast<ViewControl_TableModel*> (myParameters->model());
  ViewControl_ParametersModel* aParametersModel = dynamic_cast<ViewControl_ParametersModel*> (aTableModel->ModelValues());
  Quantity_ColorRGBA aColor = aParametersModel->GetColor();

  // OCCT color model
  Quantity_NameOfColor aColorName;
  bool isExactColorName = ViewControl_ColorSelector::IsExactColorName(aColor, aColorName);
  ViewControl_OCCTColorModel* anOCCTColorModel = dynamic_cast<ViewControl_OCCTColorModel*>(myOCCTColors->model());
  anOCCTColorModel->SetColor (aColorName, isExactColorName ? ViewControl_ColorDelegateKind_Selected
                                                           : ViewControl_ColorDelegateKind_Activated);
}

// =======================================================================
// function : ColorToString
// purpose :
// =======================================================================
QString ViewControl_ColorSelector::ColorToString (const Quantity_Color& theColor)
{
  Standard_Real aRed, aGreen, aBlue;
  theColor.Values (aRed, aGreen, aBlue, Quantity_TOC_RGB);
  return QString::number (aRed) + ViewControl_ColorSelector::ColorSeparator() +
         QString::number (aGreen) + ViewControl_ColorSelector::ColorSeparator() +
         QString::number (aBlue);
}

// =======================================================================
// function : ColorToQColor
// purpose :
// =======================================================================
QColor ViewControl_ColorSelector::ColorToQColor (const Quantity_ColorRGBA& theColor)
{
  int aDelta = 255;

  Standard_Real aRed, aGreen, aBlue;
  theColor.GetRGB().Values (aRed, aGreen, aBlue, Quantity_TOC_RGB);

  return QColor((int)(aRed * aDelta), (int)(aGreen * aDelta), (int)(aBlue * aDelta));
}

// =======================================================================
// function : StringToColorRGBA
// purpose :
// =======================================================================
Quantity_ColorRGBA ViewControl_ColorSelector::StringToColorRGBA (const QString& theColor)
{
  Quantity_ColorRGBA aColorRGBA;
  Standard_SStream aStream;
  aStream << theColor.toStdString();
  int aStreamPos = 1;
  aColorRGBA.InitFromJson (aStream, aStreamPos);
  return aColorRGBA;
}

// =======================================================================
// function : IsExactColorName
// purpose :
// =======================================================================
Standard_Boolean ViewControl_ColorSelector::IsExactColorName (const Quantity_ColorRGBA& theColor,
                                                              Quantity_NameOfColor& theColorName)
{
  theColorName = theColor.GetRGB().Name();
  return Quantity_Color (theColorName).IsEqual (theColor.GetRGB());
}

// =======================================================================
// function : onOCCTColorsTableSelectionChanged
// purpose :
// =======================================================================
void ViewControl_ColorSelector::onOCCTColorsTableSelectionChanged (const QItemSelection& theSelected, const QItemSelection&)
{
  QModelIndexList aSelectedIndices = theSelected.indexes();
  if (aSelectedIndices.size() != 1)
    return;

  ViewControl_OCCTColorModel* anOCCTColorModel = dynamic_cast<ViewControl_OCCTColorModel*>(myOCCTColors->model());
  Quantity_NameOfColor aNameOfColor;
  if (!anOCCTColorModel->GetOCCTColor (aSelectedIndices.first(), aNameOfColor))
    return;

  anOCCTColorModel->SetColor (aNameOfColor, ViewControl_ColorDelegateKind_Selected);

  // parameters model
  ViewControl_TableModel* aTableModel = dynamic_cast<ViewControl_TableModel*> (myParameters->model());
  ViewControl_ParametersModel* aParametersModel = dynamic_cast<ViewControl_ParametersModel*> (aTableModel->ModelValues());
  Quantity_Color anOCCTColor (aNameOfColor);
  aParametersModel->SetColor (Quantity_ColorRGBA (anOCCTColor), aTableModel);
}
