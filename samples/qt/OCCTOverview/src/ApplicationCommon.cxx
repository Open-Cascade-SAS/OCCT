// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#include "ApplicationCommon.h"

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFont>
#include <QFrame>
#include <QGroupBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPair>
#include <QSplitter>
#include <QStatusBar>
#include <QtGlobal>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

#include <OpenGl_GraphicDriver.hxx>
#include <OSD_Environment.hxx>

#include <stdlib.h>
#include <memory>

ApplicationCommonWindow::ApplicationCommonWindow (ApplicationType theCategory)
: QMainWindow (nullptr),
  myStdToolBar (nullptr),
  myViewBar (nullptr),
  myCasCadeBar (nullptr),
  myFilePopup (nullptr),
  myCategoryPopup (nullptr)
{
  myAppType = theCategory;
  mySampleMapper   = new QSignalMapper(this);
  myExchangeMapper = new QSignalMapper(this);
  myOcafMapper     = new QSignalMapper(this);
  myViewer3dMapper = new QSignalMapper(this);
  myViewer2dMapper = new QSignalMapper(this);

  myCategoryMapper = new QSignalMapper(this);

  connect(mySampleMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
    this, &ApplicationCommonWindow::onProcessSample);
  connect(myExchangeMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
    this, &ApplicationCommonWindow::onProcessExchange);
  connect(myOcafMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
    this, &ApplicationCommonWindow::onProcessOcaf);
  connect(myViewer3dMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
    this, &ApplicationCommonWindow::onProcessViewer3d);
  connect(myViewer2dMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
    this, &ApplicationCommonWindow::onProcessViewer2d);

  connect(myCategoryMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
    this, &ApplicationCommonWindow::onChangeCategory);

  setFocusPolicy(Qt::StrongFocus);

  QFont aCodeViewFont;
  aCodeViewFont.setFamily("Courier");
  aCodeViewFont.setFixedPitch(true);
  aCodeViewFont.setPointSize(10);

  QGroupBox* aCodeFrame = new QGroupBox(tr("Sample code"));
  QVBoxLayout* aCodeLayout = new QVBoxLayout(aCodeFrame);
  aCodeLayout->setContentsMargins(3, 3, 3, 3);
  myCodeView = new QTextEdit(aCodeFrame);
  aCodeLayout->addWidget(myCodeView);
  myCodeView->setDocumentTitle("Code");
  myCodeView->setLineWrapMode(QTextEdit::NoWrap);
  myCodeView->setReadOnly(true);
  myCodeView->setFont(aCodeViewFont);
  myCodeViewHighlighter = new OcctHighlighter(myCodeView->document());

  QGroupBox* aResultFrame = new QGroupBox(tr("Output"));
  QVBoxLayout* aResultLayout = new QVBoxLayout(aResultFrame);
  aResultLayout->setContentsMargins(3, 3, 3, 3);
  myResultView = new QTextEdit(aResultFrame);
  aResultLayout->addWidget(myResultView);
  myResultView->setDocumentTitle("Output");
  myResultView->setReadOnly(true);
  myResultView->setFont(aCodeViewFont);

  QSplitter* aCodeResultSplitter = new QSplitter(Qt::Vertical);
  aCodeResultSplitter->addWidget(aCodeFrame);
  aCodeResultSplitter->addWidget(aResultFrame);

  myDocument3d = createNewDocument();
  myDocument2d = createNewDocument();

  QFrame* aViewFrame = new QFrame;
  aViewFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  aViewFrame->setLineWidth(3);
  QVBoxLayout* aViewLayout = new QVBoxLayout(aViewFrame);
  aViewLayout->setContentsMargins(0, 0, 0, 0);
  myGeomWidget = new GeomWidget(myDocument3d, myDocument2d, this);
  aViewLayout->addWidget(myGeomWidget);
  //myGeomWidget->setContentsMargins(0, 0, 0, 0);
  QSplitter* aGeomTextSplitter = new QSplitter(Qt::Horizontal);

  aGeomTextSplitter->addWidget(aViewFrame);
  aGeomTextSplitter->addWidget(aCodeResultSplitter);
  aGeomTextSplitter->setStretchFactor(0, 1);
  aGeomTextSplitter->setStretchFactor(1, 1);
  QList<int> aSizeList{ 640, 640 };
  aGeomTextSplitter->setSizes(aSizeList);
  setCentralWidget(aGeomTextSplitter);

  Q_INIT_RESOURCE(Samples);

  TCollection_AsciiString aSampleSourcePach = getSampleSourceDir();
  myGeometrySamples      = new GeometrySamples(aSampleSourcePach,
                                               myDocument3d->getContext());
  myTopologySamples      = new TopologySamples(aSampleSourcePach,
                                               myDocument3d->getContext());
  myTriangulationSamples = new TriangulationSamples(aSampleSourcePach,
                                                    myDocument3d->getContext());
  myDataExchangeSamples  = new DataExchangeSamples(aSampleSourcePach,
                                                   myGeomWidget->Get3dView(),
                                                   myDocument3d->getContext());
  myOcafSamples          = new OcafSamples(aSampleSourcePach,
                                           myDocument3d->getViewer(),
                                           myDocument3d->getContext());
  myViewer3dSamples      = new Viewer3dSamples(aSampleSourcePach,
                                               myGeomWidget->Get3dView(),
                                               myDocument3d->getContext());
  myViewer2dSamples      = new Viewer2dSamples(aSampleSourcePach,
                                               myGeomWidget->Get2dView(),
                                               myDocument2d->getViewer(),
                                               myDocument2d->getContext());

  MenuFormJson(":/menus/Geometry.json",      mySampleMapper,   myGeometryMenus);
  MenuFormJson(":/menus/Topology.json",      mySampleMapper,   myTopologyMenus);
  MenuFormJson(":/menus/Triangulation.json", mySampleMapper,   myTriangulationMenus);
  MenuFormJson(":/menus/DataExchange.json",  myExchangeMapper, myDataExchangeMenus);
  MenuFormJson(":/menus/Ocaf.json",          myOcafMapper,     myOcafMenus);
  MenuFormJson(":/menus/Viewer3d.json",      myViewer3dMapper, myViewer3dMenus);
  MenuFormJson(":/menus/Viewer2d.json",      myViewer2dMapper, myViewer2dMenus);

  onChangeCategory(ALL_CATEGORIES[myAppType]);

  resize(1280, 560);
}

void ApplicationCommonWindow::RebuildMenu()
{
  menuBar()->clear();

  myStdActions[FileQuit] = CreateAction(&ApplicationCommonWindow::onCloseAllWindows, "Quit", "CTRL+Q");
  myStdActions[HelpAbout] = CreateAction(&ApplicationCommonWindow::onAbout, "About", "F1", ":/icons/help.png");

  // populate a menu with all actions
  myFilePopup = new QMenu(this);
  myFilePopup = menuBar()->addMenu(tr("&File"));
  myFilePopup->addAction(myStdActions[FileQuit]);

  myCategoryPopup = new QMenu(this);
  myCategoryPopup = menuBar()->addMenu(tr("&Category"));

  for (ApplicationType aCategory: ALL_CATEGORIES.keys())
  {
    QString aCategoryName = ALL_CATEGORIES.value(aCategory);
    QAction* anAction = myCategoryPopup->addAction(aCategoryName);
    anAction->setText(aCategoryName);
    myCategoryMapper->setMapping(anAction, aCategoryName);
    connect(anAction, &QAction::triggered, myCategoryMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    myCategoryPopup->addAction(anAction);
    myCategoryActions.insert(aCategory, anAction);
  }

  for (QMenu* aSampleMenu : GetCurrentMenus())
  {
    menuBar()->addMenu(aSampleMenu);
  }

  // add a help menu
  QMenu* aHelp = new QMenu(this);
  menuBar()->addSeparator();
  aHelp = menuBar()->addMenu(tr("&Help"));
  aHelp->addAction(myStdActions[HelpAbout]);
}

Handle(BaseSample) ApplicationCommonWindow::GetCurrentSamples()
{
  switch (myAppType)
  {
    case Geometry:      return myGeometrySamples;
    case Topology:      return myTopologySamples;
    case Triangulation: return myTriangulationSamples;
    case DataExchange:  return myDataExchangeSamples;
    case Ocaf:          return myOcafSamples;
    case Viewer2d:      return myViewer2dSamples;
    case Viewer3d:      return myViewer3dSamples;
    case Unknown:
      break;
  }
  throw QString("Unknown Application type");
}

const QList<QMenu*>& ApplicationCommonWindow::GetCurrentMenus()
{
  switch (myAppType)
  {
    case Geometry:      return myGeometryMenus;
    case Topology:      return myTopologyMenus;
    case Triangulation: return myTriangulationMenus;
    case DataExchange:  return myDataExchangeMenus;
    case Ocaf:          return myOcafMenus;
    case Viewer2d:      return myViewer2dMenus;
    case Viewer3d:      return myViewer3dMenus;
    case Unknown:
      break;
  }
  throw QString("Unknown Application type");
}

DocumentCommon* ApplicationCommonWindow::createNewDocument()
{
  return new DocumentCommon(this);
}

void ApplicationCommonWindow::onChangeCategory(const QString& theCategory)
{
  myAppType = ALL_CATEGORIES.key(theCategory);
  setWindowTitle(ALL_CATEGORIES[myAppType]);

  myOcafSamples->ClearExtra();
  myViewer3dSamples->ClearExtra();
  myViewer2dSamples->ClearExtra();

  GetCurrentSamples()->Clear();
  myDocument3d->Clear();
  myDocument2d->Clear();

  myCodeView->setPlainText("");
  myResultView->setPlainText("");
  GetCurrentSamples()->AppendCube();
  myDocument3d->SetObjects(GetCurrentSamples()->Get3dObjects());
  myGeomWidget->FitAll();

  RebuildMenu();

  switch (myAppType)
  {
    case DataExchange:
    {
      myDataExchangeSamples->AppendBottle();
      myDocument3d->SetObjects(GetCurrentSamples()->Get3dObjects());
      myGeomWidget->Show3d();
      break;
    }
    case Ocaf:
    {
      onProcessOcaf("CreateOcafDocument");
      myGeomWidget->Show3d();
      break;
    }
    case Viewer2d:
    {
      myGeomWidget->Show2d();
      break;
    }
    case Viewer3d:
    {
      myViewer3dSamples->AppendBottle();
      myDocument3d->SetObjects(GetCurrentSamples()->Get3dObjects());
      myGeomWidget->Show3d();
      break;
    }
    case Geometry:
    case Topology:
    case Triangulation:
    case Unknown:
    {
      break;
    }
  }
}

void ApplicationCommonWindow::onAbout()
{
  QMessageBox::information(this, tr("OCCT Overview"),
    tr("Qt based application to study OpenCASCADE Technology"),
    tr("Ok"), QString::null, QString::null, 0, 0);
}

TCollection_AsciiString ApplicationCommonWindow::getSampleSourceDir()
{
  TCollection_AsciiString aSampleSourceDir = OSD_Environment("CSF_OCCTOverviewSampleCodePath").Value();
  if (aSampleSourceDir.IsEmpty())
  {
    TCollection_AsciiString aCasRoot = OSD_Environment("CASROOT").Value();
    if (!aCasRoot.IsEmpty())
    {
      aSampleSourceDir = aCasRoot + "/samples/OCCTOverview/code";
    }
  }
  return aSampleSourceDir;
}

template <typename PointerToMemberFunction>
QAction* ApplicationCommonWindow::CreateAction (PointerToMemberFunction theHandlerMethod,
                                                const QString& theActionName,
                                                const QString& theShortcut,
                                                const QString& theIconName)
{
  QAction* aAction(NULL);
  if (theIconName.isEmpty())
  {
    aAction = new QAction(theActionName, this);
  }
  else
  {
    QPixmap aIcon = QPixmap(theIconName);
    aAction = new QAction(aIcon, theActionName, this);
  }
  aAction->setToolTip(theActionName);
  aAction->setStatusTip(theActionName);
  aAction->setShortcut(theShortcut);
  connect(aAction, &QAction::triggered, this, theHandlerMethod);
  return aAction;
}

template <typename PointerToMemberFunction>
QAction* ApplicationCommonWindow::CreateSample (PointerToMemberFunction theHandlerMethod,
                                                const char* theActionName)
{
  QAction* aAction = new QAction(QObject::tr(theActionName), this);
  connect(aAction, &QAction::triggered, this, theHandlerMethod);
  return aAction;
}

void ApplicationCommonWindow::resizeEvent(QResizeEvent* e)
{
  QMainWindow::resizeEvent(e);
  statusBar()->setSizeGripEnabled(!isMaximized());
}

void ApplicationCommonWindow::onProcessSample(const QString& theSampleName)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  setWindowTitle(ALL_CATEGORIES[myAppType] + " - " + theSampleName);
  GetCurrentSamples()->Process(theSampleName.toUtf8().data());
  myDocument3d->SetObjects(GetCurrentSamples()->Get3dObjects());
  myDocument2d->SetObjects(GetCurrentSamples()->Get2dObjects());
  myCodeView->setPlainText(GetCurrentSamples()->GetCode().ToCString());
  myResultView->setPlainText(GetCurrentSamples()->GetResult().ToCString());
  myGeomWidget->FitAll();
  QApplication::restoreOverrideCursor();
}

void ApplicationCommonWindow::onProcessExchange(const QString& theSampleName)
{
  setWindowTitle(ALL_CATEGORIES[myAppType] + " - " + theSampleName);
  int aMode = 0;
  QString aFileName = selectFileName(theSampleName, getDataExchangeDialog(theSampleName), aMode);
  if (aFileName.isEmpty())
  {
    return;
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);
  myDataExchangeSamples->SetFileName(aFileName.toUtf8().data());
  myDataExchangeSamples->SetStepType(static_cast<STEPControl_StepModelType>(aMode));
  myDataExchangeSamples->Process(theSampleName.toUtf8().data());
  myDocument3d->SetObjects(myDataExchangeSamples->Get3dObjects());
  myDocument2d->SetObjects(myDataExchangeSamples->Get2dObjects());
  myCodeView->setPlainText(myDataExchangeSamples->GetCode().ToCString());
  myResultView->setPlainText(myDataExchangeSamples->GetResult().ToCString());
  myGeomWidget->FitAll();
  QApplication::restoreOverrideCursor();
}

void ApplicationCommonWindow::onProcessOcaf(const QString& theSampleName)
{
  setWindowTitle(ALL_CATEGORIES[myAppType] + " - " + theSampleName);

  if (theSampleName.indexOf("Dialog") == 0)
  {
    int aMode = 0; // not used
    QString aFileName = selectFileName(theSampleName, getOcafDialog(theSampleName), aMode);
    if (aFileName.isEmpty())
    {
      return;
    }
    myOcafSamples->SetFileName(aFileName.toUtf8().data());
  }
  QApplication::setOverrideCursor(Qt::WaitCursor);
  myOcafSamples->Process(theSampleName.toUtf8().data());
  myDocument2d->SetObjects(myOcafSamples->Get2dObjects());
  myCodeView->setPlainText(myOcafSamples->GetCode().ToCString());
  myResultView->setPlainText(myOcafSamples->GetResult().ToCString());
  QApplication::restoreOverrideCursor();
}

void ApplicationCommonWindow::onProcessViewer3d(const QString& theSampleName)
{
  setWindowTitle(ALL_CATEGORIES[myAppType] + " - " + theSampleName);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  myViewer3dSamples->Process(theSampleName.toUtf8().data());
  myCodeView->setPlainText(myViewer3dSamples->GetCode().ToCString());
  myResultView->setPlainText(myViewer3dSamples->GetResult().ToCString());
  myGeomWidget->FitAll();
  QApplication::restoreOverrideCursor();
}

void ApplicationCommonWindow::onProcessViewer2d(const QString& theSampleName)
{
  setWindowTitle(ALL_CATEGORIES[myAppType] + " - " + theSampleName);

  Standard_Boolean anIsFileSample = Viewer2dSamples::IsFileSample(theSampleName.toUtf8().data());
  QString aFileName;
  if (anIsFileSample)
  {
    int aMode = 0; // not used
    aFileName = selectFileName(theSampleName, getOcafDialog(theSampleName), aMode);
    if (aFileName.isEmpty())
    {
      return;
    }

    myViewer2dSamples->SetFileName(aFileName.toUtf8().data());
  }
  if (!anIsFileSample || (anIsFileSample && !aFileName.isEmpty()))
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    myViewer2dSamples->Process(theSampleName.toUtf8().data());
    if (!Viewer2dSamples::IsShadedSample(theSampleName.toUtf8().data()))
    {
      myDocument2d->SetObjects(myViewer2dSamples->Get2dObjects(), Standard_False);
    }
    else
    {
      myDocument2d->SetObjects(myViewer2dSamples->Get2dObjects(), Standard_True);
    }
    myCodeView->setPlainText(myViewer2dSamples->GetCode().ToCString());
    myResultView->setPlainText(myViewer2dSamples->GetResult().ToCString());
    myGeomWidget->Show2d();
    QApplication::restoreOverrideCursor();
  }
  else
  {
    myResultView->setPlainText("No file selected!");
  }
}

QString ApplicationCommonWindow::selectFileName(const QString& theSampleName,
                                                TranslateDialog* theDialog, int& theMode)
{
  Q_UNUSED(theSampleName)

  std::shared_ptr<TranslateDialog> aDialog(theDialog);

  int ret = aDialog->exec();
  theMode = aDialog->getMode();

  qApp->processEvents();

  QString aFilename;
  QStringList aFileNameList;
  if (ret != QDialog::Accepted)
  {
    return aFilename;
  }
  aFileNameList = aDialog->selectedFiles();
  if (!aFileNameList.isEmpty())
  {
    aFilename = aFileNameList[0];
  }

  if (!QFileInfo(aFilename).completeSuffix().length())
  {
    QString selFilter = aDialog->selectedNameFilter();
    int idx = selFilter.indexOf("(*.");
    if (idx != -1)
    {
      QString tail = selFilter.mid(idx + 3);
      idx = tail.indexOf(" ");
      if (idx == -1)
      {
        idx = tail.indexOf(")");
      }
      QString ext = tail.left(idx);
      if (ext.length())
      {
        aFilename += QString(".") + ext;
      }
    }
  }

  return aFilename;
}

TranslateDialog* ApplicationCommonWindow::getDataExchangeDialog(const QString& theSampleName)
{
  TranslateDialog* aTranslateDialog = new TranslateDialog(this, 0, true);
  TCollection_AsciiString aSampleName(theSampleName.toUtf8().data());

  if (DataExchangeSamples::IsExportSample(aSampleName))
  {
    aTranslateDialog->setWindowTitle("Export file");
    aTranslateDialog->setFileMode(QFileDialog::AnyFile);
    aTranslateDialog->setAcceptMode(QFileDialog::AcceptSave);
  }
  else if (DataExchangeSamples::IsImportSample(aSampleName))
  {
    aTranslateDialog->setWindowTitle("Import file");
    aTranslateDialog->setFileMode(QFileDialog::ExistingFile);
    aTranslateDialog->setAcceptMode(QFileDialog::AcceptOpen);
  }
  QString aFormatFilter;
  if (DataExchangeSamples::IsBrepSample(aSampleName))
  {
    aFormatFilter = "BREP Files(*.brep *.rle)";
  }
  else if (DataExchangeSamples::IsStepSample(aSampleName))
  {
    aFormatFilter = "STEP Files (*.stp *.step)";
    aTranslateDialog->addMode(STEPControl_ManifoldSolidBrep, "Manifold Solid Brep");
    aTranslateDialog->addMode(STEPControl_FacetedBrep, "Faceted Brep");
    aTranslateDialog->addMode(STEPControl_ShellBasedSurfaceModel, "Shell Based Surface Model");
    aTranslateDialog->addMode(STEPControl_GeometricCurveSet, "Geometric Curve Set");
  }
  else if (DataExchangeSamples::IsIgesSample(aSampleName))
  {
    aFormatFilter = "IGES Files (*.igs *.iges)";
  }
  else if (DataExchangeSamples::IsStlSample(aSampleName))
  {
    aFormatFilter = "STL Files (*.stl)";
  }
  else if (DataExchangeSamples::IsVrmlSample(aSampleName))
  {
    aFormatFilter = "VRML Files (*.vrml)";
  }
  else if (DataExchangeSamples::IsImageSample(aSampleName))
  {
    aFormatFilter = "All Image Files (*.bmp *.gif *.jpg *.jpeg *.png *.tga)";
  }
  QStringList aFilters;
  aFilters.append(aFormatFilter);
  aFilters.append("All Files(*.*)");

  aTranslateDialog->setNameFilters(aFilters);
  aTranslateDialog->clear();
  return aTranslateDialog;
}

TranslateDialog* ApplicationCommonWindow::getOcafDialog(const QString& theSampleName)
{
  TranslateDialog* aTranslateDialog = new TranslateDialog(this, 0, true);
  TCollection_AsciiString aSampleName(theSampleName.toUtf8().data());

  if (OcafSamples::IsExportSample(aSampleName))
  {
    aTranslateDialog->setWindowTitle("Export file");
    aTranslateDialog->setFileMode(QFileDialog::AnyFile);
    aTranslateDialog->setAcceptMode(QFileDialog::AcceptSave);
  }
  else if (OcafSamples::IsImportSample(aSampleName))
  {
    aTranslateDialog->setWindowTitle("Import file");
    aTranslateDialog->setFileMode(QFileDialog::ExistingFile);
    aTranslateDialog->setAcceptMode(QFileDialog::AcceptOpen);
  }
  QStringList aFilters;
  if (OcafSamples::IsBinarySample(aSampleName))
  {
    aFilters.append("Binary OCAF Sample (*.cbf)");
  }
  if (OcafSamples::IsXmlSample(aSampleName))
  {
    aFilters.append("XML OCAF Sample (*.xml)");
  }
  aFilters.append("All Files(*.*)");

  aTranslateDialog->setNameFilters(aFilters);
  aTranslateDialog->clear();
  return aTranslateDialog;
}

QMenu* ApplicationCommonWindow::MenuFromJsonObject (const QJsonValue& theJsonValue,
                                                    const QString& theKey,
                                                    QWidget* theParent,
                                                    QSignalMapper* theMapper)
{
  QMenu* aMenu = new QMenu(theKey, theParent);
  if (theJsonValue.isObject())
  {
    QJsonObject aBranchObject = theJsonValue.toObject();
    for (const QString& aBranchKey : aBranchObject.keys())
    {
      aMenu->addMenu(MenuFromJsonObject(aBranchObject.value(aBranchKey), aBranchKey, aMenu, theMapper));
    }
  }
  else if (theJsonValue.isArray())
  {
    QJsonArray aDataArray = theJsonValue.toArray();
    for (const QJsonValue& aDataValue : aDataArray)
    {
      if (aDataValue.isObject())
      {
        QJsonObject aDataObject = aDataValue.toObject();
        QString aSampleName = aDataObject["function"].toString();
        QAction* anAction = aMenu->addAction(aSampleName);
        anAction->setText(aDataObject["text"].toString());

        theMapper->setMapping(anAction, aSampleName);
        connect(anAction, &QAction::triggered, theMapper,
          static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
      }
    }
  }
  return aMenu;
}

void ApplicationCommonWindow::MenuFormJson (const QString& thePath,
                                            QSignalMapper* theMapper,
                                            QList<QMenu*>& theMunusList)
{
  theMunusList.clear();
  QFile aJsonFile(thePath);
  QString anErrorMessage;
  if (aJsonFile.error() != QFile::NoError)
  {
    anErrorMessage = aJsonFile.errorString();
    Message::SendFail() << "QFile creating error: " << anErrorMessage.toUtf8().constData();
    return;
  }
  if (!aJsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    Message::SendFail() << "File " << thePath.toUtf8().constData() << " could not open";
    if (aJsonFile.error() != QFile::NoError)
    {
      anErrorMessage = aJsonFile.errorString();
      Message::SendFail() << "QFile opening error: " << anErrorMessage.toUtf8().constData();
    }
    return;
  }
  QString aJsonString = aJsonFile.readAll();
  aJsonFile.close();

  QJsonDocument aJsonDoc = QJsonDocument::fromJson(aJsonString.toUtf8());
  if (aJsonDoc.isObject())
  {
    QJsonObject aJsonObj = aJsonDoc.object();
    for (const QString& aKey : aJsonObj.keys())
    {
      QJsonValue aJsonValue = aJsonObj.value(aKey);
      if (aJsonValue.isObject())
      {
        theMunusList.push_back(MenuFromJsonObject(aJsonValue.toObject(), aKey, this, theMapper));
      }
    }
  }
}
