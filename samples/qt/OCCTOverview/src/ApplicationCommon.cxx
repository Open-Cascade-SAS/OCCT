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
#include <QDomDocument>
#include <QDomAttr>
#include <Standard_WarningsRestore.hxx>

#include <OpenGl_GraphicDriver.hxx>
#include <OSD_Environment.hxx>

#include <stdlib.h>
#include <memory>

ApplicationCommonWindow::ApplicationCommonWindow (ApplicationType theCategory)
: QMainWindow (nullptr),
  myAppType(theCategory),
  myStdToolBar (nullptr),
  myViewBar (nullptr),
  myCasCadeBar (nullptr),
  myFilePopup (nullptr),
  myCategoryPopup (nullptr)
{
  ALL_CATEGORIES[AppType_Geometry] = "Geometry";
  ALL_CATEGORIES[AppType_Topology] = "Topology";
  ALL_CATEGORIES[AppType_Triangulation] = "Triangulation";
  ALL_CATEGORIES[AppType_DataExchange] = "DataExchange";
  ALL_CATEGORIES[AppType_Ocaf] = "OCAF";
  ALL_CATEGORIES[AppType_Viewer3d] = "3D viewer";
  ALL_CATEGORIES[AppType_Viewer2d] = "2D Viewer";

  mySampleMapper   = new QSignalMapper(this);
  myExchangeMapper = new QSignalMapper(this);
  myOcafMapper     = new QSignalMapper(this);
  myViewer3dMapper = new QSignalMapper(this);
  myViewer2dMapper = new QSignalMapper(this);

  myCategoryMapper = new QSignalMapper(this);

  connect(mySampleMapper,   SIGNAL(mapped(const QString &)), this, SLOT(onProcessSample(const QString &)));
  connect(myExchangeMapper, SIGNAL(mapped(const QString &)), this, SLOT(onProcessExchange(const QString &)));
  connect(myOcafMapper,     SIGNAL(mapped(const QString &)), this, SLOT(onProcessOcaf(const QString &)));
  connect(myViewer3dMapper, SIGNAL(mapped(const QString &)), this, SLOT(onProcessViewer3d(const QString &)));
  connect(myViewer2dMapper, SIGNAL(mapped(const QString &)), this, SLOT(onProcessViewer2d(const QString &)));

  connect(myCategoryMapper, SIGNAL(mapped(const QString &)), this, SLOT(onChangeCategory(const QString &)));

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
  myGeomWidget = new GeomWidget(myDocument3d, myDocument2d, aViewFrame);
  aViewLayout->addWidget(myGeomWidget);

  myGeomWidget->setContentsMargins(0, 0, 0, 0);
  QSplitter* aGeomTextSplitter = new QSplitter(Qt::Horizontal);

  aGeomTextSplitter->addWidget(aViewFrame);
  aGeomTextSplitter->addWidget(aCodeResultSplitter);
  aGeomTextSplitter->setStretchFactor(0, 1);
  aGeomTextSplitter->setStretchFactor(1, 1);
  QList<int> aSizeList;
  aSizeList.append(640);
  aSizeList.append(640);
  aGeomTextSplitter->setSizes(aSizeList);
  setCentralWidget(aGeomTextSplitter);

#include <Standard_WarningsDisable.hxx>
  Q_INIT_RESOURCE(Samples);
#include <Standard_WarningsRestore.hxx>

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


  MenuFormXml(":/menus/Geometry.xml",      mySampleMapper,   myGeometryMenus);
  MenuFormXml(":/menus/Topology.xml",      mySampleMapper,   myTopologyMenus);
  MenuFormXml(":/menus/Triangulation.xml", mySampleMapper,   myTriangulationMenus);
  MenuFormXml(":/menus/DataExchange.xml",  myExchangeMapper, myDataExchangeMenus);
  MenuFormXml(":/menus/Ocaf.xml",          myOcafMapper,     myOcafMenus);
  MenuFormXml(":/menus/Viewer3d.xml",      myViewer3dMapper, myViewer3dMenus);
  MenuFormXml(":/menus/Viewer2d.xml",      myViewer2dMapper, myViewer2dMenus);

  onChangeCategory(ALL_CATEGORIES[myAppType]);

  resize(1280, 560);
}

void ApplicationCommonWindow::RebuildMenu()
{
  menuBar()->clear();

  myStdActions[StdActions_FileQuit] = CreateAction("Quit", "CTRL+Q");
  connect(myStdActions[StdActions_FileQuit], SIGNAL(triggered()), this, SLOT(onCloseAllWindows()));
  myStdActions[StdActions_HelpAbout] = CreateAction("About", "F1", ":/icons/help.png");
  connect(myStdActions[StdActions_HelpAbout], SIGNAL(triggered()), this, SLOT(onAbout()));

  // populate a menu with all actions
  myFilePopup = new QMenu(this);
  myFilePopup = menuBar()->addMenu(tr("&File"));
  myFilePopup->addAction(myStdActions[StdActions_FileQuit]);

  myCategoryPopup = new QMenu(this);
  myCategoryPopup = menuBar()->addMenu(tr("&Category"));

  foreach (ApplicationType aCategory, ALL_CATEGORIES.keys())
  {
    QString aCategoryName = ALL_CATEGORIES.value(aCategory);
    QAction* anAction = myCategoryPopup->addAction(aCategoryName);
    anAction->setText(aCategoryName);
    myCategoryMapper->setMapping(anAction, aCategoryName);
    connect(anAction, SIGNAL(triggered()), myCategoryMapper, SLOT(map()));
    myCategoryPopup->addAction(anAction);
    myCategoryActions.insert(aCategory, anAction);
  }

  foreach (QMenu* aSampleMenu, GetCurrentMenus())
  {
    menuBar()->addMenu(aSampleMenu);
  }

  // add a help menu
  QMenu* aHelp = new QMenu(this);
  menuBar()->addSeparator();
  aHelp = menuBar()->addMenu(tr("&Help"));
  aHelp->addAction(myStdActions[StdActions_HelpAbout]);
}

Handle(BaseSample) ApplicationCommonWindow::GetCurrentSamples()
{
  switch (myAppType)
  {
    case AppType_Geometry:      return myGeometrySamples;
    case AppType_Topology:      return myTopologySamples;
    case AppType_Triangulation: return myTriangulationSamples;
    case AppType_DataExchange:  return myDataExchangeSamples;
    case AppType_Ocaf:          return myOcafSamples;
    case AppType_Viewer2d:      return myViewer2dSamples;
    case AppType_Viewer3d:      return myViewer3dSamples;
    case AppType_Unknown:
      break;
  }
  throw QString("Unknown Application type");
}

const QList<QMenu*>& ApplicationCommonWindow::GetCurrentMenus()
{
  switch (myAppType)
  {
    case AppType_Geometry:      return myGeometryMenus;
    case AppType_Topology:      return myTopologyMenus;
    case AppType_Triangulation: return myTriangulationMenus;
    case AppType_DataExchange:  return myDataExchangeMenus;
    case AppType_Ocaf:          return myOcafMenus;
    case AppType_Viewer2d:      return myViewer2dMenus;
    case AppType_Viewer3d:      return myViewer3dMenus;
    case AppType_Unknown:
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
    case AppType_DataExchange:
    {
      myDataExchangeSamples->AppendBottle();
      myDocument3d->SetObjects(GetCurrentSamples()->Get3dObjects());
      myGeomWidget->Show3d();
      break;
    }
    case AppType_Ocaf:
    {
      onProcessOcaf("CreateOcafDocument");
      myGeomWidget->Show3d();
      break;
    }
    case AppType_Viewer2d:
    {
      myGeomWidget->Show2d();
      break;
    }
    case AppType_Viewer3d:
    {
      myViewer3dSamples->AppendBottle();
      myDocument3d->SetObjects(GetCurrentSamples()->Get3dObjects());
      myGeomWidget->Show3d();
      break;
    }
    case AppType_Geometry:
    case AppType_Topology:
    case AppType_Triangulation:
    case AppType_Unknown:
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

QAction* ApplicationCommonWindow::CreateAction (const QString& theActionName,
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

  return aAction;
}

template <typename PointerToMemberFunction>
QAction* ApplicationCommonWindow::CreateSample (PointerToMemberFunction theHandlerMethod,
                                                const char* theActionName)
{
  QAction* anAction = new QAction(QObject::tr(theActionName), this);
  connect(anAction, SIGNAL(triggered()), this, SLOT(theHandlerMethod()));
  return anAction;
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

QMenu* ApplicationCommonWindow::MenuFromDomNode(QDomElement& theItemElement,
                                                QWidget* theParent,
                                                QSignalMapper* theMapper)
{
  QString anItemName        = theItemElement.attribute("name");
  QMenu* aMenu = new QMenu(anItemName, theParent);
  QDomElement anChildItemElement = theItemElement.firstChildElement("MenuItem");
  QDomElement anSampleElement    = theItemElement.firstChildElement("Sample");

  while(anChildItemElement.isElement())
  {
    aMenu->addMenu(MenuFromDomNode(anChildItemElement, aMenu, theMapper));
    anChildItemElement = anChildItemElement.nextSibling().toElement();
  }

  while(anSampleElement.isElement())
  {
    QString aSampleName     = anSampleElement.attribute("name");
    QString aSampleFunction = anSampleElement.attribute("function");
    QAction* anAction = aMenu->addAction(aSampleFunction);
    anAction->setText(aSampleName);
    theMapper->setMapping(anAction, aSampleFunction);
    connect(anAction, SIGNAL(triggered()), theMapper, SLOT(map()));
    anSampleElement = anSampleElement.nextSibling().toElement();
  }
  return aMenu;
}

void ApplicationCommonWindow::MenuFormXml(const QString& thePath,
                                          QSignalMapper* theMapper,
                                          QList<QMenu*>& theMunusList)
{
  QDomDocument aDomDocument;
  theMunusList.clear();
  QFile aXmlFile(thePath);
  QString anErrorMessage;
  if (aXmlFile.error() != QFile::NoError)
  {
    anErrorMessage = aXmlFile.errorString();
    Message::SendFail() << "QFile creating error: " << anErrorMessage.toUtf8().constData();
    aXmlFile.close();
    return;
  }
  if (!aXmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    Message::SendFail() << "File " << thePath.toUtf8().constData() << " could not open";
    if (aXmlFile.error() != QFile::NoError)
    {
      anErrorMessage = aXmlFile.errorString();
      Message::SendFail() << "QFile opening error: " << anErrorMessage.toUtf8().constData();
    }
    aXmlFile.close();
    return;
  }
  bool aNamespaceProcessing(false);
  QString anErrorMsg;
  int anErrorLine(0);
  int anErrorColumn(0);
  if (!aDomDocument.setContent(&aXmlFile, aNamespaceProcessing, &anErrorMsg, &anErrorLine, &anErrorColumn))
  {
    Message::SendFail() << "XML file parsing error: " <<  anErrorMsg.toStdString()
                        << " at line: " << anErrorLine << " column: " << anErrorColumn;
    aXmlFile.close();
    return;
  }
  aXmlFile.close();

  QDomElement aRootElement = aDomDocument.documentElement();
  QDomElement anItemElement = aRootElement.firstChildElement("MenuItem");
  while(!anItemElement.isNull())
  {
    theMunusList.push_back(MenuFromDomNode(anItemElement, this, theMapper));
    anItemElement = anItemElement.nextSiblingElement("MenuItem");
  }
}
