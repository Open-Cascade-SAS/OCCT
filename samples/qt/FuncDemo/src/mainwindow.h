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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <AppStd_Application.hxx>

class QAction;
class QMenu;
class GraphWidget;

static Handle(AppStd_Application) gApplication;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    static Handle(AppStd_Application) getApplication();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void model1();
    void model2();
    void compute();
    void nbThreads();
    void about();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    GraphWidget* graph;

    QMenu *computeMenu;
    QMenu *helpMenu;
    QToolBar *computeToolBar;
    QAction *model1Act;
    QAction *model2Act;
    QAction *computeAct;
    QAction *nbThreadsAct;
    QAction *exitAct;
    QAction *aboutAct;

    // Default models.
    // The Model 1 consists of a set of simple functions
    // of the same type.
    // These functions have a double parameter: relative time of execution, and
    // a "forced" parameter: another function.
    // Actually, the functions are independent, 
    // but we "force" a dependency between them to test the Function Mechanism using simple functions.
    // So, a function uses a double parameter and produces nothing.
    // I use a TDF_Reference to refer from a function to another function.
    // This way I define a dependency.
    void createDefaultModel1();

    // Another default model, topological.
    void createDefaultModel2();

    // Redraw the nodes (change their colour).            
    void redrawGraph();
};

#endif
