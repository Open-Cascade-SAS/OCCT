/****************************************************************************
**
** Copyright (C) 2004-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** Licensees holding a valid Qt License Agreement may use this file in
** accordance with the rights, responsibilities and obligations
** contained therein.  Please consult your licensing agreement or
** contact sales@trolltech.com if any conditions of this licensing
** agreement are not clear to you.
**
** Further information about Qt licensing is available at:
** http://www.trolltech.com/products/qt/licensing.html or by
** contacting info@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>
#include <QtGlobal>
#if QT_VERSION > QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#endif

#include "mainwindow.h"
#include "graphwidget.h"
#include "node.h"
#include "edge.h"

#include "SimpleDriver.h"
#include "PointDriver.h"
#include "CircleDriver.h"
#include "PrismDriver.h"
#include "ConeDriver.h"
#include "CylinderDriver.h"
#include "ShapeSaverDriver.h"

#include <TDocStd_Document.hxx>
#include <TFunction_DriverTable.hxx>
#include <TFunction_IFunction.hxx>
#include <TFunction_GraphNode.hxx>
#include <TFunction_DoubleMapOfIntegerLabel.hxx>
#include <TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel.hxx>
#include <TFunction_Scope.hxx>

#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <OSD_Timer.hxx>

#include <TDF_Tool.hxx>
#include <TDF_Reference.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_ChildIterator.hxx>

#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#ifdef __GNUC__
#include <unistd.h>
#endif

MainWindow::MainWindow()
{
    graph = new GraphWidget(this);
    setCentralWidget(graph);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    // Create a new document
    createDefaultModel1();
}

Handle(AppStd_Application) MainWindow::getApplication()
{
    if (gApplication.IsNull())
        gApplication = new AppStd_Application();
    return gApplication;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void MainWindow::model1()
{
    // Clean the view
    if (!graph->getDocument().IsNull())
    {
        QList<QGraphicsItem*> items = graph->scene()->items();
        for (int i = 0; i < items.size(); i++)
        {
            QGraphicsItem* item = items.at(i);
            graph->scene()->removeItem(item);
        }
    
        // Close the document
        getApplication()->Close(graph->getDocument());
    }

    // Create a new document
    createDefaultModel1();
}

void MainWindow::model2()
{
    // Clean the view
    if (!graph->getDocument().IsNull())
    {
        QList<QGraphicsItem*> items = graph->scene()->items();
        for (int i = 0; i < items.size(); i++)
        {
            QGraphicsItem* item = items.at(i);
            graph->scene()->removeItem(item);
        }
    
        // Close the document
        getApplication()->Close(graph->getDocument());
    }

    // Create a new document
    createDefaultModel2();
}

static void prepareFunctions(GraphWidget* graph)
{
    if (!graph->getDocument().IsNull())
    {
        TDF_Label L = graph->getDocument()->Main().Root();
        TDF_ChildIterator itr(L, true);
        for (; itr.More(); itr.Next())
        {
            Handle(TFunction_GraphNode) G;
            if (itr.Value().FindAttribute(TFunction_GraphNode::GetID(), G))
            {
                G->SetStatus(TFunction_ES_NotExecuted);
            }
        }
    }
}

// Redraw the nodes (change their colour).            
void MainWindow::redrawGraph()
{
    QList<QGraphicsItem*> items = graph->scene()->items();
    for (int i = 0; i < items.size(); i++)
    {
        QGraphicsItem* item = items.at(i);
        item->update();
    }
}

void MainWindow::compute()
{
    OSD_Timer aTimer;
    double seconds, CPUTime;
    int hours, minutes;
    aTimer.Start();

    int i = 0, nb = 1; // number of repetitions (for test-purpose).

    // Run computation
    while (i++ < nb)
    {
        prepareFunctions(graph);
        graph->compute();

        while (!graph->isFinished())
        {
            // Redraw the nodes (change their colour).            
            redrawGraph();

            // Process user-events.
            qApp->processEvents();
            #ifdef __GNUC__
               sleep(0.001);
            #else
               ::Sleep(100);
            #endif
        }
    }

    aTimer.Show(seconds, minutes, hours, CPUTime);
    cout << "Execution of "<<graph->getNbThreads()<< " threads took          " << hours << " hours, " << minutes << " minutes, " << seconds << " seconds" << endl;

    // Redraw the nodes (change their colour).            
    redrawGraph();
}

void MainWindow::nbThreads()
{
    bool ok;
    int nb = QInputDialog::getInt(this, tr("Number of threads"), tr("(1 - 4): "),
                                      graph->getNbThreads(), 1, 4, 1, &ok);
    if (ok)
        graph->setNbThreads(nb);
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("Test-application of the advanced Function Mechanism"),
                      tr("The <b>Application</b> runs different models "
                      "in single and multi-threaded modes. "
                      "It shows graphically the result of computation."));
}

void MainWindow::createActions()
{
    model1Act = new QAction(QIcon("images/open.png"), tr("Model 1"), this);
    model1Act->setStatusTip(tr("Model 1"));
    connect(model1Act, SIGNAL(triggered()), this, SLOT(model1()));

    model2Act = new QAction(QIcon("images/open.png"), tr("Model 2"), this);
    model2Act->setStatusTip(tr("Model 2"));
    connect(model2Act, SIGNAL(triggered()), this, SLOT(model2()));

    computeAct = new QAction(QIcon("images/new.png"), tr("Compute"), this);
    computeAct->setStatusTip(tr("Compute"));
    connect(computeAct, SIGNAL(triggered()), this, SLOT(compute()));

    nbThreadsAct = new QAction(tr("Number of threads"), this);
    nbThreadsAct->setStatusTip(tr("Number of threads"));
    connect(nbThreadsAct, SIGNAL(triggered()), this, SLOT(nbThreads()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus()
{
    computeMenu = menuBar()->addMenu(tr("&Model"));
    computeMenu->addAction(model1Act);
    computeMenu->addAction(model2Act);
    computeMenu->addSeparator();
    computeMenu->addAction(computeAct);
    computeMenu->addAction(nbThreadsAct);
    computeMenu->addSeparator();
    computeMenu->addAction(exitAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void MainWindow::createToolBars()
{
    computeToolBar = addToolBar(tr("Model"));
    computeToolBar->addAction(model1Act);
    computeToolBar->addAction(model2Act);
    computeToolBar->addAction(computeAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createDefaultModel1()
{
    Handle(AppStd_Application) app = MainWindow::getApplication();
    Handle(TDocStd_Document) doc;
    app->NewDocument("XmlOcaf", doc);
    TDF_Label mainLabel = doc->Main();

    // Initialize function drivers
    TFunction_DriverTable::Get()->AddDriver(SimpleDriver::GetID(), new SimpleDriver());

    // Set a logbook.
    Handle(TFunction_Logbook) logbook = TFunction_Logbook::Set(mainLabel);

    // Create a tree of functions
    TDF_Label L1 = mainLabel.FindChild(1);
    TDF_Label L2 = mainLabel.FindChild(2);
    TDF_Label L3 = mainLabel.FindChild(3);
    TDF_Label L4 = mainLabel.FindChild(4);
    TDF_Label L5 = mainLabel.FindChild(5);
    TDF_Label L6 = mainLabel.FindChild(6);
    TDF_Label L7 = mainLabel.FindChild(7);
    double time = 2;
    // 1:
    TFunction_IFunction::NewFunction(L1, SimpleDriver::GetID());
    TDataStd_Name::Set(L1, "1");
    TDataStd_Real::Set(L1, time); // Argument
    TDF_Reference::Set(L1.FindChild(2).FindChild(1), L1); // Result
    TFunction_IFunction iFunc1(L1);
    iFunc1.GetGraphNode()->SetStatus(TFunction_ES_NotExecuted);
    // 2:
    TFunction_IFunction::NewFunction(L2, SimpleDriver::GetID());
    TDataStd_Name::Set(L2, "2");
    TDataStd_Real::Set(L2, time); // Argument
    TDF_Reference::Set(L2.FindChild(2).FindChild(1), L2); // Result
    TFunction_IFunction iFunc2(L2);
    iFunc2.GetGraphNode()->SetStatus(TFunction_ES_NotExecuted);
    // 3:
    TFunction_IFunction::NewFunction(L3, SimpleDriver::GetID());
    TDataStd_Name::Set(L3, "3");
    TDataStd_Real::Set(L3, time); // Argument
    TDF_Reference::Set(L3.FindChild(1).FindChild(1), L1); // Argument: F3 -> F1
    TDF_Reference::Set(L3.FindChild(2).FindChild(1), L3); // Result
    TFunction_IFunction iFunc3(L3);
    iFunc3.GetGraphNode()->SetStatus(TFunction_ES_NotExecuted);
    // 4:
    TFunction_IFunction::NewFunction(L4, SimpleDriver::GetID());
    TDataStd_Name::Set(L4, "4");
    TDataStd_Real::Set(L4, time); // Argument
    TDF_Reference::Set(L4.FindChild(1).FindChild(1), L2); // Argument F4 -> F2
    TDF_Reference::Set(L4.FindChild(1).FindChild(2), L3); // Argument F4 -> F3
    TDF_Reference::Set(L4.FindChild(2).FindChild(1), L4); // Result
    TFunction_IFunction iFunc4(L4);
    iFunc4.GetGraphNode()->SetStatus(TFunction_ES_NotExecuted);
    // 5:
    TFunction_IFunction::NewFunction(L5, SimpleDriver::GetID());
    TDataStd_Name::Set(L5, "5");
    TDataStd_Real::Set(L5, time); // Argument
    TDF_Reference::Set(L5.FindChild(1).FindChild(1), L4); // Argument F5 -> F4
    TDF_Reference::Set(L5.FindChild(2).FindChild(1), L5); // Result
    TFunction_IFunction iFunc5(L5);
    iFunc5.GetGraphNode()->SetStatus(TFunction_ES_NotExecuted);
    // 6:
    TFunction_IFunction::NewFunction(L6, SimpleDriver::GetID());
    TDataStd_Name::Set(L6, "6");
    TDataStd_Real::Set(L6, time); // Argument
    TDF_Reference::Set(L6.FindChild(1).FindChild(1), L4); // Argument F6 ->F4
    TDF_Reference::Set(L6.FindChild(2).FindChild(1), L6); // Result
    TFunction_IFunction iFunc6(L6);
    iFunc6.GetGraphNode()->SetStatus(TFunction_ES_NotExecuted);
    // 7:
    TFunction_IFunction::NewFunction(L7, SimpleDriver::GetID());
    TDataStd_Name::Set(L7, "7");
    TDataStd_Real::Set(L7, time); // Argument
    TDF_Reference::Set(L7.FindChild(1).FindChild(1), L4); // Argument F7 -> F4
    TDF_Reference::Set(L7.FindChild(2).FindChild(1), L7); // Result
    TFunction_IFunction iFunc7(L7);
    iFunc7.GetGraphNode()->SetStatus(TFunction_ES_NotExecuted);

    // Construct the dependencies between functions.
    TFunction_IFunction::UpdateDependencies(mainLabel);

    // Set the functions 1 and 2 modified
    iFunc1.GetLogbook()->SetTouched(L1);
    iFunc2.GetLogbook()->SetTouched(L2);

    // Draw the model
    graph->createModel(doc);
}

void MainWindow::createDefaultModel2()
{
    Handle(AppStd_Application) app = MainWindow::getApplication();
    Handle(TDocStd_Document) doc;
    app->NewDocument("BinOcaf", doc);
    //app->Open("W:\\TestFM\\model2.cbf", doc);
    TDF_Label mainLabel = doc->Main();

    // Initialize function drivers
    TFunction_DriverTable::Get()->AddDriver(PointDriver::GetID(), new PointDriver());
    TFunction_DriverTable::Get()->AddDriver(CircleDriver::GetID(), new CircleDriver());
    TFunction_DriverTable::Get()->AddDriver(PrismDriver::GetID(), new PrismDriver());
    TFunction_DriverTable::Get()->AddDriver(ConeDriver::GetID(), new ConeDriver());
    TFunction_DriverTable::Get()->AddDriver(CylinderDriver::GetID(), new CylinderDriver());
    TFunction_DriverTable::Get()->AddDriver(ShapeSaverDriver::GetID(), new ShapeSaverDriver());

    // Create a tree of functions
    TDF_Label Lpoint1  = mainLabel.FindChild(1);
    TDF_Label Lpoint2  = mainLabel.FindChild(2);
    TDF_Label Lpoint3  = mainLabel.FindChild(3);
    TDF_Label Lpoint4  = mainLabel.FindChild(4);
    TDF_Label Lcircle1 = mainLabel.FindChild(5);
    TDF_Label Lcircle2 = mainLabel.FindChild(6);
    TDF_Label Lcircle3 = mainLabel.FindChild(7);
    TDF_Label Lcircle4 = mainLabel.FindChild(8);
    TDF_Label Lprism1  = mainLabel.FindChild(9);
    TDF_Label Lprism2  = mainLabel.FindChild(10);
    TDF_Label Lprism3  = mainLabel.FindChild(11);
    TDF_Label Lprism4  = mainLabel.FindChild(12);
    TDF_Label Lcone1   = mainLabel.FindChild(13);
    TDF_Label Lcyl1    = mainLabel.FindChild(14);
    TDF_Label Lcyl2    = mainLabel.FindChild(15);
    TDF_Label Lcyl3    = mainLabel.FindChild(16);
    TDF_Label Lcyl4    = mainLabel.FindChild(17);
    TDF_Label Lcyl5    = mainLabel.FindChild(18);
    TDF_Label Lcyl6    = mainLabel.FindChild(19);
    TDF_Label Lcyl7    = mainLabel.FindChild(20);
    TDF_Label Lcyl8    = mainLabel.FindChild(21);
    TDF_Label Lcone2   = mainLabel.FindChild(22);
    TDF_Label Lshape1  = mainLabel.FindChild(23);

    // Set a logbook.
    Handle(TFunction_Logbook) logbook = TFunction_Logbook::Set(mainLabel);

    // Points:
    // Point 1:
    TFunction_IFunction::NewFunction(Lpoint1, PointDriver::GetID());
    TDataStd_Name::Set(Lpoint1, "P1");
    Handle(TDataStd_RealArray) arr1 = TDataStd_RealArray::Set(Lpoint1, 1, 3);
    arr1->SetValue(1, -50);
    arr1->SetValue(2, -50);
    arr1->SetValue(3, 0);
    TDF_Reference::Set(Lpoint1.FindChild(1).FindChild(1), Lpoint1); // Argument
    TDF_Reference::Set(Lpoint1.FindChild(2).FindChild(1), Lpoint1); // Result
    TFunction_IFunction iFuncPoint1(Lpoint1);
    iFuncPoint1.SetStatus(TFunction_ES_NotExecuted);
    // Point 2:
    TFunction_IFunction::NewFunction(Lpoint2, PointDriver::GetID());
    TDataStd_Name::Set(Lpoint2, "P2");
    Handle(TDataStd_RealArray) arr2 = TDataStd_RealArray::Set(Lpoint2, 1, 3);
    arr2->SetValue(1, 50);
    arr2->SetValue(2, -50);
    arr2->SetValue(3, 0);
    TDF_Reference::Set(Lpoint2.FindChild(1).FindChild(1), Lpoint2); // Argument
    TDF_Reference::Set(Lpoint2.FindChild(2).FindChild(1), Lpoint2); // Result
    TFunction_IFunction iFuncPoint2(Lpoint2);
    iFuncPoint2.SetStatus(TFunction_ES_NotExecuted);
    // Point 3:
    TFunction_IFunction::NewFunction(Lpoint3, PointDriver::GetID());
    TDataStd_Name::Set(Lpoint3, "P3");
    Handle(TDataStd_RealArray) arr3 = TDataStd_RealArray::Set(Lpoint3, 1, 3);
    arr3->SetValue(1, 50);
    arr3->SetValue(2, 50);
    arr3->SetValue(3, 0);
    TDF_Reference::Set(Lpoint3.FindChild(1).FindChild(1), Lpoint3); // Argument
    TDF_Reference::Set(Lpoint3.FindChild(2).FindChild(1), Lpoint3); // Result
    TFunction_IFunction iFuncPoint3(Lpoint3);
    iFuncPoint3.SetStatus(TFunction_ES_NotExecuted);
    // Point 4:
    TFunction_IFunction::NewFunction(Lpoint4, PointDriver::GetID());
    TDataStd_Name::Set(Lpoint4, "P4");
    Handle(TDataStd_RealArray) arr4 = TDataStd_RealArray::Set(Lpoint4, 1, 3);
    arr4->SetValue(1, -50);
    arr4->SetValue(2, 50);
    arr4->SetValue(3, 0);
    TDF_Reference::Set(Lpoint4.FindChild(1).FindChild(1), Lpoint4); // Argument
    TDF_Reference::Set(Lpoint4.FindChild(2).FindChild(1), Lpoint4); // Result
    TFunction_IFunction iFuncPoint4(Lpoint4);
    iFuncPoint4.SetStatus(TFunction_ES_NotExecuted);
    // Circles:
    // Circle 1:
    TFunction_IFunction::NewFunction(Lcircle1, CircleDriver::GetID());
    TDataStd_Name::Set(Lcircle1, "C1");
    TDataStd_Real::Set(Lcircle1, 10);
    TDF_Reference::Set(Lcircle1.FindChild(1).FindChild(1), Lpoint1); // Argument: point
    TDF_Reference::Set(Lcircle1.FindChild(2).FindChild(1), Lcircle1); // Result
    TFunction_IFunction iFuncCircle1(Lcircle1);
    iFuncCircle1.SetStatus(TFunction_ES_NotExecuted);
    // Circle 2:
    TFunction_IFunction::NewFunction(Lcircle2, CircleDriver::GetID());
    TDataStd_Name::Set(Lcircle2, "C2");
    TDataStd_Real::Set(Lcircle2, 20);
    TDF_Reference::Set(Lcircle2.FindChild(1).FindChild(1), Lpoint2); // Argument: point
    TDF_Reference::Set(Lcircle2.FindChild(2).FindChild(1), Lcircle2); // Result
    TFunction_IFunction iFuncCircle2(Lcircle2);
    iFuncCircle2.SetStatus(TFunction_ES_NotExecuted);
    // Circle 3:
    TFunction_IFunction::NewFunction(Lcircle3, CircleDriver::GetID());
    TDataStd_Name::Set(Lcircle3, "C3");
    TDataStd_Real::Set(Lcircle3, 30);
    TDF_Reference::Set(Lcircle3.FindChild(1).FindChild(1), Lpoint3); // Argument: point
    TDF_Reference::Set(Lcircle3.FindChild(2).FindChild(1), Lcircle3); // Result
    TFunction_IFunction iFuncCircle3(Lcircle3);
    iFuncCircle3.SetStatus(TFunction_ES_NotExecuted);
    // Circle 4:
    TFunction_IFunction::NewFunction(Lcircle4, CircleDriver::GetID());
    TDataStd_Name::Set(Lcircle4, "C4");
    TDataStd_Real::Set(Lcircle4, 40);
    TDF_Reference::Set(Lcircle4.FindChild(1).FindChild(1), Lpoint4); // Argument: point
    TDF_Reference::Set(Lcircle4.FindChild(2).FindChild(1), Lcircle4); // Result
    TFunction_IFunction iFuncCircle4(Lcircle4);
    iFuncCircle4.SetStatus(TFunction_ES_NotExecuted);
    // Prisms:
    // Prism 1:
    TFunction_IFunction::NewFunction(Lprism1, PrismDriver::GetID());
    TDataStd_Name::Set(Lprism1, "Pr1");
    TDataStd_Real::Set(Lprism1, 30);
    TDF_Reference::Set(Lprism1.FindChild(1).FindChild(1), Lcircle1); // Argument: point
    TDF_Reference::Set(Lprism1.FindChild(2).FindChild(1), Lprism1); // Result
    TDF_Reference::Set(Lprism1.FindChild(2).FindChild(2), Lprism1.FindChild(3)); // Result (top)
    TFunction_IFunction iFuncPrism1(Lprism1);
    iFuncPrism1.SetStatus(TFunction_ES_NotExecuted);
    // Prism 2:
    TFunction_IFunction::NewFunction(Lprism2, PrismDriver::GetID());
    TDataStd_Name::Set(Lprism2, "Pr2");
    TDataStd_Real::Set(Lprism2, 30);
    TDF_Reference::Set(Lprism2.FindChild(1).FindChild(1), Lcircle2); // Argument: point
    TDF_Reference::Set(Lprism2.FindChild(2).FindChild(1), Lprism2); // Result
    TDF_Reference::Set(Lprism2.FindChild(2).FindChild(2), Lprism2.FindChild(3)); // Result (top)
    TFunction_IFunction iFuncPrism2(Lprism2);
    iFuncPrism2.SetStatus(TFunction_ES_NotExecuted);
    // Prism 3:
    TFunction_IFunction::NewFunction(Lprism3, PrismDriver::GetID());
    TDataStd_Name::Set(Lprism3, "Pr3");
    TDataStd_Real::Set(Lprism3, 30);
    TDF_Reference::Set(Lprism3.FindChild(1).FindChild(1), Lcircle3); // Argument: point
    TDF_Reference::Set(Lprism3.FindChild(2).FindChild(1), Lprism3); // Result
    TDF_Reference::Set(Lprism3.FindChild(2).FindChild(2), Lprism3.FindChild(3)); // Result (top)
    TFunction_IFunction iFuncPrism3(Lprism3);
    iFuncPrism3.SetStatus(TFunction_ES_NotExecuted);
    // Prism 4:
    TFunction_IFunction::NewFunction(Lprism4, PrismDriver::GetID());
    TDataStd_Name::Set(Lprism4, "Pr4");
    TDataStd_Real::Set(Lprism4, 30);
    TDF_Reference::Set(Lprism4.FindChild(1).FindChild(1), Lcircle4); // Argument: point
    TDF_Reference::Set(Lprism4.FindChild(2).FindChild(1), Lprism4); // Result
    TDF_Reference::Set(Lprism4.FindChild(2).FindChild(2), Lprism4.FindChild(3)); // Result (top)
    TFunction_IFunction iFuncPrism4(Lprism4);
    iFuncPrism4.SetStatus(TFunction_ES_NotExecuted);
    // Cone 1:
    TFunction_IFunction::NewFunction(Lcone1, ConeDriver::GetID());
    TDataStd_Name::Set(Lcone1, "Co1");
    TDataStd_Real::Set(Lcone1, 20);
    TDF_Reference::Set(Lcone1.FindChild(1).FindChild(1), Lprism1.FindChild(3)); // Argument: top
    TDF_Reference::Set(Lcone1.FindChild(1).FindChild(2), Lprism2.FindChild(3)); // Argument: top
    TDF_Reference::Set(Lcone1.FindChild(1).FindChild(3), Lprism3.FindChild(3)); // Argument: top
    TDF_Reference::Set(Lcone1.FindChild(1).FindChild(4), Lprism4.FindChild(3)); // Argument: top
    TDF_Reference::Set(Lcone1.FindChild(2).FindChild(1), Lcone1); // Result
    TDF_Reference::Set(Lcone1.FindChild(2).FindChild(2), Lcone1.FindChild(3)); // Result
    TFunction_IFunction iFuncCone1(Lcone1);
    iFuncCone1.SetStatus(TFunction_ES_NotExecuted);
    // Cylinders:
    // Cylinder 1:
    TFunction_IFunction::NewFunction(Lcyl1, CylinderDriver::GetID());
    TDataStd_Name::Set(Lcyl1, "Cyl1");
    Handle(TDataStd_RealArray) carr1 = TDataStd_RealArray::Set(Lcyl1, 1, 3);
    carr1->SetValue(1, 10.0);
    carr1->SetValue(2, 45.0);
    carr1->SetValue(3, 20.0);
    TDF_Reference::Set(Lcyl1.FindChild(1).FindChild(1), Lcone1.FindChild(3)); // Argument: top
    TDF_Reference::Set(Lcyl1.FindChild(2).FindChild(1), Lcyl1); // Result
    TDF_Reference::Set(Lcyl1.FindChild(2).FindChild(2), Lcyl1.FindChild(3)); // Result
    TFunction_IFunction iFuncCyl1(Lcyl1);
    iFuncCyl1.SetStatus(TFunction_ES_NotExecuted);
    // Cylinder 2:
    TFunction_IFunction::NewFunction(Lcyl2, CylinderDriver::GetID());
    TDataStd_Name::Set(Lcyl2, "Cyl2");
    Handle(TDataStd_RealArray) carr2 = TDataStd_RealArray::Set(Lcyl2, 1, 3);
    carr2->SetValue(1, 10.0);
    carr2->SetValue(2, 90.0);
    carr2->SetValue(3, 20.0);
    TDF_Reference::Set(Lcyl2.FindChild(1).FindChild(1), Lcone1.FindChild(3)); // Argument: top
    TDF_Reference::Set(Lcyl2.FindChild(2).FindChild(1), Lcyl2); // Result
    TDF_Reference::Set(Lcyl2.FindChild(2).FindChild(2), Lcyl2.FindChild(3)); // Result
    TFunction_IFunction iFuncCyl2(Lcyl2);
    iFuncCyl2.SetStatus(TFunction_ES_NotExecuted);
    // Cylinder 3:
    TFunction_IFunction::NewFunction(Lcyl3, CylinderDriver::GetID());
    TDataStd_Name::Set(Lcyl3, "Cyl3");
    Handle(TDataStd_RealArray) carr3 = TDataStd_RealArray::Set(Lcyl3, 1, 3);
    carr3->SetValue(1, 10.0);
    carr3->SetValue(2, 135.0);
    carr3->SetValue(3, 20.0);
    TDF_Reference::Set(Lcyl3.FindChild(1).FindChild(1), Lcone1.FindChild(3)); // Argument: top
    TDF_Reference::Set(Lcyl3.FindChild(2).FindChild(1), Lcyl3); // Result
    TDF_Reference::Set(Lcyl3.FindChild(2).FindChild(2), Lcyl3.FindChild(3)); // Result
    TFunction_IFunction iFuncCyl3(Lcyl3);
    iFuncCyl3.SetStatus(TFunction_ES_NotExecuted);
    // Cylinder 4:
    TFunction_IFunction::NewFunction(Lcyl4, CylinderDriver::GetID());
    TDataStd_Name::Set(Lcyl4, "Cyl4");
    Handle(TDataStd_RealArray) carr4 = TDataStd_RealArray::Set(Lcyl4, 1, 3);
    carr4->SetValue(1, 10.0);
    carr4->SetValue(2, 180.0);
    carr4->SetValue(3, 20.0);
    TDF_Reference::Set(Lcyl4.FindChild(1).FindChild(1), Lcone1.FindChild(3)); // Argument: top
    TDF_Reference::Set(Lcyl4.FindChild(2).FindChild(1), Lcyl4); // Result
    TDF_Reference::Set(Lcyl4.FindChild(2).FindChild(2), Lcyl4.FindChild(3)); // Result
    TFunction_IFunction iFuncCyl4(Lcyl4);
    iFuncCyl4.SetStatus(TFunction_ES_NotExecuted);
    // Cylinder 5:
    TFunction_IFunction::NewFunction(Lcyl5, CylinderDriver::GetID());
    TDataStd_Name::Set(Lcyl5, "Cyl5");
    Handle(TDataStd_RealArray) carr5 = TDataStd_RealArray::Set(Lcyl5, 1, 3);
    carr5->SetValue(1, 10.0);
    carr5->SetValue(2, 225.0);
    carr5->SetValue(3, 20.0);
    TDF_Reference::Set(Lcyl5.FindChild(1).FindChild(1), Lcone1.FindChild(3)); // Argument: top
    TDF_Reference::Set(Lcyl5.FindChild(2).FindChild(1), Lcyl5); // Result
    TDF_Reference::Set(Lcyl5.FindChild(2).FindChild(2), Lcyl5.FindChild(3)); // Result
    TFunction_IFunction iFuncCyl5(Lcyl5);
    iFuncCyl5.SetStatus(TFunction_ES_NotExecuted);
    // Cylinder 6:
    TFunction_IFunction::NewFunction(Lcyl6, CylinderDriver::GetID());
    TDataStd_Name::Set(Lcyl6, "Cyl6");
    Handle(TDataStd_RealArray) carr6 = TDataStd_RealArray::Set(Lcyl6, 1, 3);
    carr6->SetValue(1, 10.0);
    carr6->SetValue(2, 270.0);
    carr6->SetValue(3, 20.0);
    TDF_Reference::Set(Lcyl6.FindChild(1).FindChild(1), Lcone1.FindChild(3)); // Argument: top
    TDF_Reference::Set(Lcyl6.FindChild(2).FindChild(1), Lcyl6); // Result
    TDF_Reference::Set(Lcyl6.FindChild(2).FindChild(2), Lcyl6.FindChild(3)); // Result
    TFunction_IFunction iFuncCyl6(Lcyl6);
    iFuncCyl6.SetStatus(TFunction_ES_NotExecuted);
    // Cylinder 7:
    TFunction_IFunction::NewFunction(Lcyl7, CylinderDriver::GetID());
    TDataStd_Name::Set(Lcyl7, "Cyl7");
    Handle(TDataStd_RealArray) carr7 = TDataStd_RealArray::Set(Lcyl7, 1, 3);
    carr7->SetValue(1, 10.0);
    carr7->SetValue(2, 315.0);
    carr7->SetValue(3, 20.0);
    TDF_Reference::Set(Lcyl7.FindChild(1).FindChild(1), Lcone1.FindChild(3)); // Argument: top
    TDF_Reference::Set(Lcyl7.FindChild(2).FindChild(1), Lcyl7); // Result
    TDF_Reference::Set(Lcyl7.FindChild(2).FindChild(2), Lcyl7.FindChild(3)); // Result
    TFunction_IFunction iFuncCyl7(Lcyl7);
    iFuncCyl7.SetStatus(TFunction_ES_NotExecuted);
    // Cylinder 8:
    TFunction_IFunction::NewFunction(Lcyl8, CylinderDriver::GetID());
    TDataStd_Name::Set(Lcyl8, "Cyl8");
    Handle(TDataStd_RealArray) carr8 = TDataStd_RealArray::Set(Lcyl8, 1, 3);
    carr8->SetValue(1, 10.0);
    carr8->SetValue(2, 0.0);
    carr8->SetValue(3, 20.0);
    TDF_Reference::Set(Lcyl8.FindChild(1).FindChild(1), Lcone1.FindChild(3)); // Argument: top
    TDF_Reference::Set(Lcyl8.FindChild(2).FindChild(1), Lcyl8); // Result
    TDF_Reference::Set(Lcyl8.FindChild(2).FindChild(2), Lcyl8.FindChild(3)); // Result
    TFunction_IFunction iFuncCyl8(Lcyl8);
    iFuncCyl8.SetStatus(TFunction_ES_NotExecuted);
    // Cone 2:
    TFunction_IFunction::NewFunction(Lcone2, ConeDriver::GetID());
    TDataStd_Name::Set(Lcone2, "Co2");
    TDataStd_Real::Set(Lcone2, 30);
    TDF_Reference::Set(Lcone2.FindChild(1).FindChild(1), Lcyl1.FindChild(3)); // Argument: top
    TDF_Reference::Set(Lcone2.FindChild(1).FindChild(2), Lcyl3.FindChild(3)); // Argument: top
    TDF_Reference::Set(Lcone2.FindChild(1).FindChild(3), Lcyl5.FindChild(3)); // Argument: top
    TDF_Reference::Set(Lcone2.FindChild(1).FindChild(4), Lcyl7.FindChild(3)); // Argument: top
    TDF_Reference::Set(Lcone2.FindChild(2).FindChild(1), Lcone2); // Result
    TDF_Reference::Set(Lcone2.FindChild(2).FindChild(2), Lcone2.FindChild(3)); // Result
    TFunction_IFunction iFuncCone2(Lcone2);
    iFuncCone2.SetStatus(TFunction_ES_NotExecuted);
    // Shape saver 1:
    TFunction_IFunction::NewFunction(Lshape1, ShapeSaverDriver::GetID());
    TDataStd_Name::Set(Lshape1, "Sh");
    TFunction_IFunction iFuncShape1(Lshape1);
    iFuncShape1.SetStatus(TFunction_ES_NotExecuted);
    // Arguments of this functions - results of all functions
    int iTag = 1;
    const TFunction_DoubleMapOfIntegerLabel& all = iFuncShape1.GetAllFunctions();
    TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel itrm(all);
    for (; itrm.More(); itrm.Next())
    {
        TFunction_IFunction iFunc(itrm.Key2());
        TDF_LabelList res;
        iFunc.Results(res);
        TDF_ListIteratorOfLabelList itrl(res);
        for (; itrl.More(); itrl.Next(), iTag++)
        {
            TDF_Reference::Set(Lshape1.FindChild(1).FindChild(iTag), itrl.Value());
        }
    }

    // Construct the dependencies between functions.
    TFunction_IFunction::UpdateDependencies(mainLabel);

    // Set the functions 1 .. 4 modified
    TFunction_IFunction(mainLabel).GetLogbook()->SetTouched(Lpoint1);
    TFunction_IFunction(mainLabel).GetLogbook()->SetTouched(Lpoint2);
    TFunction_IFunction(mainLabel).GetLogbook()->SetTouched(Lpoint3);
    TFunction_IFunction(mainLabel).GetLogbook()->SetTouched(Lpoint4);

    // Draw the model
    graph->createModel(doc);

    //app->SaveAs(doc, "W:\\TestFM\\model2.cbf");
}
