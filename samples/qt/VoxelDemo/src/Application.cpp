#include "Application.h"
#include "ConversionThread.h"
#include "Timer.h"

#include <QPixmap.h>
#include <QToolButton.h>
#include <QWhatsThis.h>
#include <QMenu.h>
#include <QMenuBar.h>
#include <QStatusBar.h>
#include <QApplication.h>
#include <QFileDialog.h>
#include <QMessageBox.h>
#include <QInputDialog.h>
#include <QCloseEvent>

#include <Voxel_BoolDS.hxx>
#include <Voxel_ColorDS.hxx>
#include <Voxel_FloatDS.hxx>
#include <Voxel_OctBoolDS.hxx>
#include <Voxel_ROctBoolDS.hxx>
#include <Voxel_BooleanOperation.hxx>
#include <Voxel_CollisionDetection.hxx>
#include <Voxel_FastConverter.hxx>
#include <Voxel_Writer.hxx>
#include <Voxel_Reader.hxx>
#include <VoxelClient_VisDrawer.h>

#include <BRepTools.hxx>
#include <BRepBndLib.hxx>
#include <BRep_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>

#include <Windows.h>

Application::Application()
    : QMainWindow( 0 )
{
    // File
    QMenu * file = menuBar()->addMenu( "&File" );

    QAction* a;
    // Box
    a = new QAction("Box", this);
    connect(a, SIGNAL(triggered()), this, SLOT(box()));
    file->addAction(a);
    // Cylinder
    a = new QAction("Cylinder", this);
    connect(a, SIGNAL(triggered()), this, SLOT(cylinder()));
    file->addAction(a);
    // Torus
    a = new QAction("Torus", this);
    connect(a, SIGNAL(triggered()), this, SLOT(torus()));
    file->addAction(a);
    // Sphere
    a = new QAction("Sphere", this);
    connect(a, SIGNAL(triggered()), this, SLOT(sphere()));
    file->addAction(a);
    // Load shape...
    a = new QAction("Load shape...", this);
    a->setShortcut(tr("Ctrl+O"));
    connect(a, SIGNAL(triggered()), this, SLOT(choose()));
    file->addAction(a);

    file->addSeparator();

    // Open
    a = new QAction("Open", this);
    connect(a, SIGNAL(triggered()), this, SLOT(open()));
    file->addAction(a);

    // Save
    a = new QAction("Save", this);
    connect(a, SIGNAL(triggered()), this, SLOT(save()));
    file->addAction(a);

    file->addSeparator();

    // Quit
    a = new QAction("&Quit", this);
    a->setShortcut(tr("Ctrl+Q"));
    connect(a, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
    file->addAction(a);

    menuBar()->addSeparator();


#ifdef TEST
    QMenu * test = menuBar()->addMenu( "Test" );

    a = new QAction("Test boolean", this);
    connect(a, SIGNAL(triggered()), this, SLOT(testBoolDS()));
    test->addAction(a);

    a = new QAction("Test color", this);
    connect(a, SIGNAL(triggered()), this, SLOT(testColorDS()));
    test->addAction(a);

    a = new QAction("Test float", this);
    connect(a, SIGNAL(triggered()), this, SLOT(testFloatDS()));
    test->addAction(a);

    a = new QAction("Test boolean / 8", this);
    connect(a, SIGNAL(triggered()), this, SLOT(testOctBoolDS()));
    test->addAction(a);

    a = new QAction("Test boolean / 8 / 8..", this);
    connect(a, SIGNAL(triggered()), this, SLOT(testROctBoolDS()));
    test->addAction(a);

    test->addSeparator();

    a = new QAction("Test fusion of booleans", this);
    connect(a, SIGNAL(triggered()), this, SLOT(testFuseBoolDS()));
    test->addAction(a);

    a = new QAction("Test fusion of colors", this);
    connect(a, SIGNAL(triggered()), this, SLOT(testFuseColorDS()));
    test->addAction(a);

    a = new QAction("Test fusion of floating-points", this);
    connect(a, SIGNAL(triggered()), this, SLOT(testFuseFloatDS()));
    test->addAction(a);

    a = new QAction("Test cutting of booleans", this);
    connect(a, SIGNAL(triggered()), this, SLOT(testCutBoolDS()));
    test->addAction(a);

    a = new QAction("Test cutting of booleans", this);
    connect(a, SIGNAL(triggered()), this, SLOT(testCutColorDS()));
    test->addAction(a);

    a = new QAction("Test cutting of floating-points", this);
    connect(a, SIGNAL(triggered()), this, SLOT(testCutFloatDS()));
    test->addAction(a);

#endif // TEST

    QMenu * converter = menuBar()->addMenu( "Converter" );

#ifdef TEST
    
    a = new QAction("Number of splits along X", this);
    connect(a, SIGNAL(triggered()), this, SLOT(setNbX()));
    converter->addAction(a);

    a = new QAction("Number of splits along Y", this);
    connect(a, SIGNAL(triggered()), this, SLOT(setNbY()));
    converter->addAction(a);

    a = new QAction("Number of splits along Z", this);
    connect(a, SIGNAL(triggered()), this, SLOT(setNbZ()));
    converter->addAction(a);

    converter->addSeparator();

    a = new QAction("Side of scanning", this);
    connect(a, SIGNAL(triggered()), this, SLOT(setScanSide()));
    converter->addAction(a);

    converter->addSeparator();

    a = new QAction("Volumic value of 1bit voxels", this);
    connect(a, SIGNAL(triggered()), this, SLOT(setVolumicBoolValue()));
    converter->addAction(a);
    
    a = new QAction("Volumic value of 4bit voxels", this);
    connect(a, SIGNAL(triggered()), this, SLOT(setVolumicColorValue()));
    converter->addAction(a);

    converter->addSeparator();

#endif // TEST

    a = new QAction("Convert to 1bit voxels", this);
    connect(a, SIGNAL(triggered()), this, SLOT(convert2bool()));
    converter->addAction(a);

    a = new QAction("Convert to 4bit voxels", this);
    connect(a, SIGNAL(triggered()), this, SLOT(convert2color()));
    converter->addAction(a);

    QMenu * vis = menuBar()->addMenu( "&Visualization" );

    a = new QAction("Points", this);
    connect(a, SIGNAL(triggered()), this, SLOT(displayPoints()));
    vis->addAction(a);

#ifdef TEST
    
    a = new QAction("Nearest points", this);
    connect(a, SIGNAL(triggered()), this, SLOT(displayNearestPoints()));
    vis->addAction(a);

#endif // TEST

    a = new QAction("Boxes", this);
    connect(a, SIGNAL(triggered()), this, SLOT(displayBoxes()));
    vis->addAction(a);

#ifdef TEST

    a = new QAction("Nearest boxes", this);
    connect(a, SIGNAL(triggered()), this, SLOT(displayNearestBoxes()));
    vis->addAction(a);

#endif // TEST
    
    vis->addSeparator();

    a = new QAction("Point size", this);
    connect(a, SIGNAL(triggered()), this, SLOT(setPointSize()));
    vis->addAction(a);
    
    a = new QAction("Quadrangle size (%)", this);
    connect(a, SIGNAL(triggered()), this, SLOT(setQuadrangleSize()));
    vis->addAction(a);

    vis->addSeparator();

    a = new QAction("Color min value", this);
    connect(a, SIGNAL(triggered()), this, SLOT(setColorMinValue()));
    vis->addAction(a);

    a = new QAction("Color max value", this);
    connect(a, SIGNAL(triggered()), this, SLOT(setColorMaxValue()));
    vis->addAction(a);

#ifdef TEST

    vis->addSeparator();

    a = new QAction("Use GL lists", this);
    connect(a, SIGNAL(triggered()), this, SLOT(setUsageOfGLlists()));
    vis->addAction(a);

#endif // TEST

    vis->addSeparator();

    a = new QAction("Displayed X min", this);
    connect(a, SIGNAL(triggered()), this, SLOT(setDisplayedXMin()));
    vis->addAction(a);

    a = new QAction("Displayed X max", this);
    connect(a, SIGNAL(triggered()), this, SLOT(setDisplayedXMax()));
    vis->addAction(a);

    a = new QAction("Displayed Y min", this);
    connect(a, SIGNAL(triggered()), this, SLOT(setDisplayedYMin()));
    vis->addAction(a);

    a = new QAction("Displayed Y max", this);
    connect(a, SIGNAL(triggered()), this, SLOT(setDisplayedYMax()));
    vis->addAction(a);

    a = new QAction("Displayed Z min", this);
    connect(a, SIGNAL(triggered()), this, SLOT(setDisplayedZMin()));
    vis->addAction(a);

    a = new QAction("Displayed Z max", this);
    connect(a, SIGNAL(triggered()), this, SLOT(setDisplayedZMax()));
    vis->addAction(a);


    QMenu * demo = menuBar()->addMenu( "Demo" );

    a = new QAction("Waves", this);
    connect(a, SIGNAL(triggered()), this, SLOT(displayWaves()));
    demo->addAction(a);

    a = new QAction("Cut", this);
    connect(a, SIGNAL(triggered()), this, SLOT(displayCut()));
    demo->addAction(a);

    a = new QAction("Collisions", this);
    connect(a, SIGNAL(triggered()), this, SLOT(displayCollisions()));
    demo->addAction(a);


    QMenu * help = menuBar()->addMenu( "Help" );

    a = new QAction("About", this);
    a->setShortcut(tr("F1"));
    connect(a, SIGNAL(triggered()), this, SLOT(about()));
    help->addAction(a);


    myViewer = new Viewer( this );
    myViewer->setFocus();
    setCentralWidget( myViewer );
    statusBar()->showMessage( "Ready", 2000 );

	myNbX = 100;
	myNbY = 100;
	myNbZ = 100;

    myScanSide = 7;

    myVolumicBoolValue = false;
    myVolumicColorValue = 0;

    myQuadrangleSize = 40;
    
    myColorMinValue = 1;
    myColorMaxValue = 15;

	myBoolVoxels = 0;
	myColorVoxels = 0;
  myColorScale = new AIS_ColorScale;
  myColorScale->SetZLayer (Graphic3d_ZLayerId_TopOSD);
  myColorScale->SetTransformPersistence (Graphic3d_TMF_2d, gp_Pnt (-1,-1,0));


    myDisplayedXMin = -DBL_MAX;
    myDisplayedXMax =  DBL_MAX;
    myDisplayedYMin = -DBL_MAX;
    myDisplayedYMax =  DBL_MAX;
    myDisplayedZMin = -DBL_MAX;
    myDisplayedZMax =  DBL_MAX;

    resize( 450, 600 );

    myViewer->getIC()->SetAutoActivateSelection (Standard_False);
}

Application::~Application()
{
	if (myBoolVoxels)
		delete myBoolVoxels;
	if (myColorVoxels)
		delete myColorVoxels;
}

void Application::choose()
{
    QString fn = QFileDialog::getOpenFileName( this, QString::null, QString::null, "*.brep");
    if ( !fn.isEmpty() )
		load( fn );
    else
		statusBar()->showMessage( "Loading aborted", 2000 );
}

void Application::load( const QString &fileName )
{
    QFile f( fileName );
    if ( !f.open( QIODevice::ReadOnly ) )
		return;

    // Read shape
	TopoDS_Shape S;
	BRep_Builder B;
	if (!BRepTools::Read(S, (char*) fileName.constData(), B))
		statusBar()->showMessage( "Loading failed", 2000 );
    
    load(S);
}

void Application::open()
{
    QString fn = QFileDialog::getOpenFileName( this, QString::null, QString::null, "*.vx");
    if ( fn.isEmpty() || !QFile::exists(fn) )
    {
		statusBar()->showMessage( "Open aborted", 2000 );
        return;
    }

    Timer timer;
    timer.Start();

    // Read the voxels
    Voxel_Reader reader;
    if (!reader.Read((char*)fn.constData()))
    {
		statusBar()->showMessage( "Open failed... sorry", 2000 );
        return;
    }

    timer.Stop();
    timer.Print("Open");

    // Release current voxels
    if (myBoolVoxels)
    {
        delete myBoolVoxels;
        myBoolVoxels = 0;
    }
    if (myColorVoxels)
    {
        delete myColorVoxels;
        myColorVoxels = 0;
    }

    // Take the voxels
    if (reader.IsBoolVoxels())
    {
        myBoolVoxels = (Voxel_BoolDS*) reader.GetBoolVoxels();
        myViewer->getSelector().SetVoxels(*myBoolVoxels);
    }
    else if (reader.IsColorVoxels())
    {
        myColorVoxels = (Voxel_ColorDS*) reader.GetColorVoxels();
        myViewer->getSelector().SetVoxels(*myColorVoxels);
    }
    
    // Display the voxels
    myViewer->getIC()->EraseAll(false);
    Voxel_DS* ds = myBoolVoxels;
    if (!ds)
        ds = myColorVoxels;
    if (ds)
    {
        myDisplayedXMin = ds->GetX() - 10.0 * Precision::Confusion();
        myDisplayedXMax = ds->GetX() + ds->GetXLen() + 10.0 * Precision::Confusion();
        myDisplayedYMin = ds->GetY() - 10.0 * Precision::Confusion();
        myDisplayedYMax = ds->GetY() + ds->GetYLen() + 10.0 * Precision::Confusion();
        myDisplayedZMin = ds->GetZ() - 10.0 * Precision::Confusion();
        myDisplayedZMax = ds->GetZ() + ds->GetZLen() + 10.0 * Precision::Confusion();
    }

    // Init visual data
    initPrs();

    // Set voxels and display
    Handle(Poly_Triangulation) empty;
    myVoxels->SetBoolVoxels(myBoolVoxels);
	myVoxels->SetColorVoxels(myColorVoxels);
    myVoxels->SetTriangulation(empty);
    if (myViewer->getIC()->IsDisplayed(myVoxels))
        myViewer->getIC()->Redisplay(myVoxels, false);
    else
	    myViewer->getIC()->Display(myVoxels, false);
   
    // Color scale
    if (myColorVoxels)
        displayColorScale();
    else
      myViewer->getIC()->Erase(myColorScale);

	myViewer->getView()->FitAll();

    statusBar()->showMessage( "Ready.", 2000 );
}

void Application::save()
{
    QString fn = QFileDialog::getSaveFileName( this, QString::null, QString::null, "*.vx");
    if ( fn.isEmpty() )
    {
		statusBar()->showMessage( "Storage aborted", 2000 );
        return;
    }
    if (fn.indexOf(".vx", -1, Qt::CaseInsensitive) == -1)
        fn += ".vx";

    Timer timer;
    timer.Start();

    // Write the voxels
    Voxel_Writer writer;
    writer.SetFormat(Voxel_VFF_BINARY);
    if (myBoolVoxels)
        writer.SetVoxels(*myBoolVoxels);
    else if (myColorVoxels)
        writer.SetVoxels(*myColorVoxels);
    else
    {
		statusBar()->showMessage( "Nothing to store", 2000 );
        return;
    }
    if (!writer.Write((char*)fn.constData()))
    {
		statusBar()->showMessage( "Storage failed... sorry", 2000 );
        return;
    }

    timer.Stop();
    timer.Print("Save");

    statusBar()->showMessage( "Saved.", 2000 );
}

void Application::closeEvent( QCloseEvent* ce )
{
    ce->accept();
}

void Application::about()
{
    QMessageBox::about( this, "Voxel demo-application",
			"This example demonstrates simple usage of "
			"voxel models of Open CASCADE.");
}

void Application::testBoolDS()
{
	Timer timer;
	int ix, iy, iz;
	int nbx = 100, nby = 100, nbz = 100;


	// 1. BoolDS:

	timer.Start();

	Voxel_BoolDS ds(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				if (ix & 0x01)
					ds.Set(ix, iy, iz, false);
				else
					ds.Set(ix, iy, iz, true);
			}
		}
	}

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				bool value = ds.Get(ix, iy, iz) == Standard_True;
				if (ix & 0x01)
				{
					if (value != false)
						cout<<"Wrong value!"<<endl;
				}
				else
				{
					if (value != true)
						cout<<"Wrong value!"<<endl;
				}
			}
		}
	}

	timer.Stop();
	timer.Print("BoolDS");
}

void Application::testColorDS()
{
	Timer timer;
	int ix, iy, iz;
	int nbx = 100, nby = 100, nbz = 100;


	// 1. ColorDS:

	timer.Start();

	Voxel_ColorDS ds(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
                if (ix & 0x01)
    			    ds.Set(ix, iy, iz, 8);
                else
                    ds.Set(ix, iy, iz, 7);
			}
		}
	}

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				unsigned char value = ds.Get(ix, iy, iz);
                if (ix & 0x01)
                {
				    if (value != 8)
					    cout<<"Wrong value!"<<endl;
                }
                else
                {
				    if (value != 7)
					    cout<<"Wrong value!"<<endl;
                }
			}
		}
	}

	timer.Stop();
	timer.Print("ColorDS");
}

void Application::testFloatDS()
{
	Timer timer;
	int ix, iy, iz;
	int nbx = 100, nby = 100, nbz = 100;


	// 1. FloatDS:

	timer.Start();

	Voxel_FloatDS ds(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
                if (ix & 0x01)
    			    ds.Set(ix, iy, iz, 8.8f);
                else
                    ds.Set(ix, iy, iz, 7.7f);
			}
		}
	}

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				float value = ds.Get(ix, iy, iz);
                if (ix & 0x01)
                {
				    if (value != 8.8f)
					    cout<<"Wrong value!"<<endl;
                }
                else
                {
				    if (value != 7.7f)
					    cout<<"Wrong value!"<<endl;
                }
			}
		}
	}

	timer.Stop();
	timer.Print("FloatDS");
}

void Application::testOctBoolDS()
{
	Timer timer;
	int ix, iy, iz;
	int nbx = 30, nby = 30, nbz = 30;


	// 1. OctBoolDS:

	timer.Start();

	Voxel_OctBoolDS ds(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				if (ix & 0x01)
                {
					ds.Set(ix, iy, iz, true);
                }
				else
                {
                    for (int i = 0; i < 8; i++)
                    {
                        if (i & 0x01)
					        ds.Set(ix, iy, iz, i, true);
                        else
                            ds.Set(ix, iy, iz, i, false);
                    }
                }
			}
		}
	}

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				if (ix & 0x01)
				{
    				bool value = ds.Get(ix, iy, iz) == Standard_True;
					if (value != true)
						cout<<"Wrong value!"<<endl;
				}
				else
				{
                    for (int i = 0; i < 8; i++)
                    {
                        if (i & 0x01)
                        {
            				bool value = ds.Get(ix, iy, iz, i) == Standard_True;
					        if (value != true)
						        cout<<"Wrong value!"<<endl;
                        }
                        else
                        {
            				bool value = ds.Get(ix, iy, iz, i) == Standard_True;
					        if (value != false)
						        cout<<"Wrong value!"<<endl;
                        }
                    }
				}
			}
		}
	}

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				if (ix & 0x01)
				{
                    for (int i = 0; i < 8; i++)
                    {
                        ds.Set(ix, iy, iz, i, true);
                    }
				}
                else
                {
                    for (int i = 0; i < 8; i++)
                    {
                        ds.Set(ix, iy, iz, i, false);
                    }
                }
			}
		}
	}

    ds.OptimizeMemory();

	timer.Stop();
	timer.Print("OctBoolDS");
}

void Application::testROctBoolDS()
{
	Timer timer;
	int ix, iy, iz, i, j;
	int nbx = 30, nby = 30, nbz = 30;

	// 1. ROctBoolDS:

	timer.Start();

	Voxel_ROctBoolDS ds(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

    for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
                ds.Set(ix, iy, iz, true);
			}
		}
	}

    for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
                for (i = 0; i < 8; i++)
                {
                    for (j = 0; j < 8; j++)
                    {
                        ds.Set(ix, iy, iz, i, j, true);
                    }
                }
			}
		}
	}

    ds.OptimizeMemory();

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
                if (ds.Deepness(ix, iy, iz) == 0)
                {
                    bool value = ds.Get(ix, iy, iz);
                    if (value != true)
                        cout<<"Wrong value..."<<endl;
                }
                if (ds.Deepness(ix, iy, iz) == 1)
                {
                    for (i = 0; i < 8; i++)
                    {
                        bool value = ds.Get(ix, iy, iz, i);
                        if (value != true)
                            cout<<"Wrong value..."<<endl;
                    }
                }
                if (ds.Deepness(ix, iy, iz) == 2)
                {
                    for (i = 0; i < 8; i++)
                    {
                        for (j = 0; j < 8; j++)
                        {
                            bool value = ds.Get(ix, iy, iz, i, j);
                            if (value != true)
                                cout<<"Wrong value..."<<endl;
                        }
                    }
                }
			}
		}
	}

	timer.Stop();
	timer.Print("ROctBoolDS");


    // Test converter
    TopoDS_Shape S = BRepPrimAPI_MakeSphere(100.0);

    timer.Start();

    int progress = 0;
    Voxel_ROctBoolDS* ds2 = new Voxel_ROctBoolDS;
    Voxel_FastConverter converter(S, *ds2, 0.1, myNbX, myNbY, myNbZ, 1);
    converter.Convert(progress);
    ds2->OptimizeMemory();

	timer.Stop();
	timer.Print("ROctBoolDS::converter");


    // Display 
    myViewer->getIC()->EraseAll(false);
    initPrs();
    myVoxels->SetBoolVoxels(0);
	myVoxels->SetColorVoxels(0);
    Handle(Poly_Triangulation) empty;
    myVoxels->SetTriangulation(empty);
    myVoxels->SetROctBoolVoxels(ds2);
    myViewer->getIC()->Display(myVoxels, false);
    myViewer->getIC()->Erase(myColorScale);
	myViewer->getView()->FitAll();
    myViewer->getSelector().SetVoxels(*ds2);
}

void Application::testFuseBoolDS()
{
	Timer timer;
	int ix, iy, iz;
	int nbx = 100, nby = 100, nbz = 100;


	// 1. Set two BoolDS:

	timer.Start();

	Voxel_BoolDS ds1(0, 0, 0, 1, 1, 1, nbx, nby, nbz);
	Voxel_BoolDS ds2(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				if (ix & 0x01)
					ds2.Set(ix, iy, iz, false);
				else
					ds2.Set(ix, iy, iz, true);
			}
		}
	}

    // 2. Fuse them

    Voxel_BooleanOperation fuser;
    if (!fuser.Fuse(ds1, ds2))
        cout<<"The operation failed..."<<endl;

    // 3. Check result

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				bool value = ds1.Get(ix, iy, iz) == Standard_True;
				if (ix & 0x01)
				{
					if (value != false)
				    	cout<<"Wrong value!"<<endl;
				}
				else
				{
					if (value != true)
						cout<<"Wrong value!"<<endl;
				}
			}
		}
	}

	timer.Stop();
	timer.Print("Fusion of BoolDS");
}

void Application::testFuseColorDS()
{
	Timer timer;
	int ix, iy, iz;
	int nbx = 100, nby = 100, nbz = 100;


	// 1. Set two ColorDS:

	timer.Start();

	Voxel_ColorDS ds1(0, 0, 0, 1, 1, 1, nbx, nby, nbz);
	Voxel_ColorDS ds2(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				ds1.Set(ix, iy, iz, 11);
			}
		}
	}
	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				if (ix & 0x01)
					ds2.Set(ix, iy, iz, 3);
				else
					ds2.Set(ix, iy, iz, 5);
			}
		}
	}

    // 2. Fuse them

    Voxel_BooleanOperation fuser;
    if (!fuser.Fuse(ds1, ds2))
        cout<<"The operation failed..."<<endl;

    // 3. Check result

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				unsigned char value = ds1.Get(ix, iy, iz);
				if (ix & 0x01)
				{
					if (value != 14)
				    	cout<<"Wrong value!"<<endl;
				}
				else
				{
					if (value != 15)
						cout<<"Wrong value!"<<endl;
				}
			}
		}
	}

	timer.Stop();
	timer.Print("Fusion of ColorDS");
}

void Application::testFuseFloatDS()
{
	Timer timer;
	int ix, iy, iz;
	int nbx = 100, nby = 100, nbz = 100;


	// 1. Set two FloatDS:

	timer.Start();

	Voxel_FloatDS ds1(0, 0, 0, 1, 1, 1, nbx, nby, nbz);
	Voxel_FloatDS ds2(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				ds1.Set(ix, iy, iz, 11.1f);
			}
		}
	}
	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				if (ix & 0x01)
					ds2.Set(ix, iy, iz, 3.3f);
				else
					ds2.Set(ix, iy, iz, 5.5f);
			}
		}
	}

    // 2. Fuse them

    Voxel_BooleanOperation fuser;
    if (!fuser.Fuse(ds1, ds2))
        cout<<"The operation failed..."<<endl;

    // 3. Check result

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				float value = ds1.Get(ix, iy, iz);
				if (ix & 0x01)
				{
                    if (fabs(value - 14.4f) > 0.001)
				    	cout<<"Wrong value!"<<endl;
				}
				else
				{
                    if (fabs(value - 16.6f) > 0.001)
						cout<<"Wrong value!"<<endl;
				}
			}
		}
	}

	timer.Stop();
	timer.Print("Fusion of FloatDS");
}

void Application::testCutBoolDS()
{
	Timer timer;
	int ix, iy, iz;
	int nbx = 100, nby = 100, nbz = 100;


	// 1. Set two BoolDS:

	timer.Start();

	Voxel_BoolDS ds1(0, 0, 0, 1, 1, 1, nbx, nby, nbz);
	Voxel_BoolDS ds2(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				ds1.Set(ix, iy, iz, true);
			}
		}
	}
	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				if (ix & 0x01)
					ds2.Set(ix, iy, iz, false);
				else
					ds2.Set(ix, iy, iz, true);
			}
		}
	}

    // 2. Cut them

    Voxel_BooleanOperation cutter;
    if (!cutter.Cut(ds1, ds2))
        cout<<"The operation failed..."<<endl;

    // 3. Check result

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				bool value = ds1.Get(ix, iy, iz) == Standard_True;
				if (ix & 0x01)
				{
					if (value != true)
				    	cout<<"Wrong value!"<<endl;
				}
				else
				{
					if (value != false)
						cout<<"Wrong value!"<<endl;
				}
			}
		}
	}

	timer.Stop();
	timer.Print("Cut of BoolDS");
}

void Application::testCutColorDS()
{
	Timer timer;
	int ix, iy, iz;
	int nbx = 100, nby = 100, nbz = 100;


	// 1. Set two ColorDS:

	timer.Start();

	Voxel_ColorDS ds1(0, 0, 0, 1, 1, 1, nbx, nby, nbz);
	Voxel_ColorDS ds2(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				ds1.Set(ix, iy, iz, 11);
			}
		}
	}
	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				if (ix & 0x01)
					ds2.Set(ix, iy, iz, 3);
				else
					ds2.Set(ix, iy, iz, 5);
			}
		}
	}

    // 2. Cut them

    Voxel_BooleanOperation cutter;
    if (!cutter.Cut(ds1, ds2))
        cout<<"The operation failed..."<<endl;

    // 3. Check result

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				unsigned char value = ds1.Get(ix, iy, iz);
				if (ix & 0x01)
				{
					if (value != 8)
				    	cout<<"Wrong value!"<<endl;
				}
				else
				{
					if (value != 6)
						cout<<"Wrong value!"<<endl;
				}
			}
		}
	}

	timer.Stop();
	timer.Print("Cut of ColorDS");
}

void Application::testCutFloatDS()
{
	Timer timer;
	int ix, iy, iz;
	int nbx = 100, nby = 100, nbz = 100;


	// 1. Set two FloatDS:

	timer.Start();

	Voxel_FloatDS ds1(0, 0, 0, 1, 1, 1, nbx, nby, nbz);
	Voxel_FloatDS ds2(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				ds1.Set(ix, iy, iz, 11.1f);
			}
		}
	}
	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				if (ix & 0x01)
					ds2.Set(ix, iy, iz, 3.3f);
				else
					ds2.Set(ix, iy, iz, 5.5f);
			}
		}
	}

    // 2. Cut them

    Voxel_BooleanOperation cutter;
    if (!cutter.Cut(ds1, ds2))
        cout<<"The operation failed..."<<endl;

    // 3. Check result

	for (ix = 0; ix < nbx; ix++)
	{
		for (iy = 0; iy < nby; iy++)
		{
			for (iz = 0; iz < nbz; iz++)
			{
				float value = ds1.Get(ix, iy, iz);
				if (ix & 0x01)
				{
                    if (fabs(value - 7.8f) > 0.001)
				    	cout<<"Wrong value!"<<endl;
				}
				else
				{
                    if (fabs(value - 5.6f) > 0.001)
						cout<<"Wrong value!"<<endl;
				}
			}
		}
	}

	timer.Stop();
	timer.Print("Cut of FloatDS");
}

void Application::convert2bool()
{
    convert(0);
}

void Application::convert2color()
{
    convert(1);
}

void Application::convert(const int ivoxel)
{
	TopoDS_Shape S;
	if (!myShape.IsNull())
		S = myShape->Shape();
    if (S.IsNull())
	{
		QMessageBox::warning( this, "Voxel demo-application", "No shape for conversion!");
		return;
	}

    switch (ivoxel)
    {
        case 0:
        {
	        if (!myBoolVoxels)
		        myBoolVoxels = new Voxel_BoolDS;
            if (myColorVoxels)
            {
                delete myColorVoxels;
                myColorVoxels = 0;
            }
            break;
        }
        case 1:
        {
	        if (!myColorVoxels)
		        myColorVoxels = new Voxel_ColorDS;
            if (myBoolVoxels)
            {
                delete myBoolVoxels;
                myBoolVoxels = 0;
            }
            break;
        }
    }

    switch (ivoxel)
    {
        case 0:
        {
            Timer timer;
            timer.Start();

            /*
            int progress;
            Voxel_Converter converter(S, *myBoolVoxels, myNbX, myNbY, myNbZ);
	        if (!converter.Convert(progress, myVolumicBoolValue, myScanSide))
	        {
		        QMessageBox::warning( this, "Voxel demo-application", "Conversion failed...");
		        return;
	        }
            */

            /*
            Voxel_Converter converter(S, *myBoolVoxels, myNbX, myNbY, myNbZ, 2);
            ConversionThread thread1, thread2;
            
            thread1.setConverter(&converter);
            thread2.setConverter(&converter);

            thread1.setVolumicValue(myVolumicBoolValue);
            thread2.setVolumicValue(myVolumicBoolValue);

            thread1.setScanSide(myScanSide);
            thread2.setScanSide(myScanSide);

            thread1.setThreadIndex(1);
            thread2.setThreadIndex(2);

            thread1.start();
            thread2.start();

            while (thread1.running() || thread2.running())
            {
                ::Sleep(100);
            }
            */

            /*
            int progress;
            Voxel_FastConverter converter(S, *myBoolVoxels, 0.1, myNbX, myNbY, myNbZ, 1);
            converter.Convert(progress, 1);
            //if (myVolumicBoolValue)
            //    converter.FillInVolume(myVolumicBoolValue);
            */

            Voxel_FastConverter converter(S, *myBoolVoxels, 0.1, myNbX, myNbY, myNbZ, 2);
            ConversionThread thread1, thread2;
            
            thread1.setConverter(&converter);
            thread2.setConverter(&converter);

            thread1.setThreadIndex(1);
            thread2.setThreadIndex(2);

            thread1.start();
            thread2.start();

            while (thread1.isRunning() || thread2.isRunning())
            {
                ::Sleep(100);
            }

            timer.Print("Converter");

            myViewer->getSelector().SetVoxels(*myBoolVoxels);
            break;
        }
        case 1:
        {

            Timer timer;
            timer.Start();

            /*
            int progress;
            Voxel_Converter converter(S, *myColorVoxels, myNbX, myNbY, myNbZ);
	        if (!converter.Convert(progress, myVolumicColorValue, myScanSide))
	        {
		        QMessageBox::warning( this, "Voxel demo-application", "Conversion failed...");
		        return;
	        }
            */

            /*
            Voxel_Converter converter(S, *myColorVoxels, myNbX, myNbY, myNbZ, 2);
            ConversionThread thread1, thread2;
            
            thread1.setConverter(&converter);
            thread2.setConverter(&converter);

            thread1.setVolumicValue(myVolumicColorValue);
            thread2.setVolumicValue(myVolumicColorValue);

            thread1.setScanSide(myScanSide);
            thread2.setScanSide(myScanSide);

            thread1.setThreadIndex(1);
            thread2.setThreadIndex(2);

            thread1.start();
            thread2.start();

            while (thread1.running() || thread2.running())
            {
                ::Sleep(100);
            }
            */

            /*
            int progress;
            Voxel_FastConverter converter(S, *myColorVoxels, myNbX, myNbY, myNbZ, 1);
            converter.Convert(progress, 1);
            if (myVolumicColorValue)
                converter.FillInVolume(myVolumicColorValue);
            */

            Voxel_FastConverter converter(S, *myColorVoxels, 0.1, myNbX, myNbY, myNbZ, 2);
            ConversionThread thread1, thread2;
            
            thread1.setConverter(&converter);
            thread2.setConverter(&converter);

            thread1.setThreadIndex(1);
            thread2.setThreadIndex(2);

            thread1.start();
            thread2.start();

            while (thread1.isRunning() || thread2.isRunning())
            {
                ::Sleep(100);
            }

            timer.Print("Converter");


            // Set color for demonstration
            double maxd = 
                fabs(myColorVoxels->GetX()) > fabs(myColorVoxels->GetY()) ? 
                fabs(myColorVoxels->GetX()) : fabs(myColorVoxels->GetY());
            maxd = maxd > fabs(myColorVoxels->GetZ()) ? maxd : fabs(myColorVoxels->GetZ());
            maxd = maxd > fabs(myColorVoxels->GetX() + myColorVoxels->GetXLen()) ?
                maxd : fabs(myColorVoxels->GetX() + myColorVoxels->GetXLen());
            maxd = maxd > fabs(myColorVoxels->GetY() + myColorVoxels->GetYLen()) ?
                maxd : fabs(myColorVoxels->GetY() + myColorVoxels->GetYLen());
            maxd = maxd > fabs(myColorVoxels->GetZ() + myColorVoxels->GetZLen()) ?
                maxd : fabs(myColorVoxels->GetZ() + myColorVoxels->GetZLen());
            for (int ix = 0; ix < myNbX; ix++)
            {
                for (int iy = 0; iy < myNbY; iy++)
                {
                    for (int iz = 0; iz < myNbZ; iz++)
                    {
                        unsigned char value = myColorVoxels->Get(ix, iy, iz);
                        if (value)
                        {
                            double xc, yc, zc, xd, yd, zd;
                            myColorVoxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            xd = fabs(xc);
                            yd = fabs(yc);
                            zd = fabs(zc);
                            double mind = xd < yd ? xd : yd;
                            mind = zd < mind ? zd : mind;
                            value = unsigned char(15.0 * (maxd - mind) / maxd);
                            if (value <= 0)
                                value = 1;
                            myColorVoxels->Set(ix, iy, iz, value);
                        }
                    }
                }
            }

            myViewer->getSelector().SetVoxels(*myColorVoxels);
            break;
        }
    }

    myViewer->getIC()->EraseAll(false);

    Voxel_DS* ds = myBoolVoxels;
    if (!ds)
        ds = myColorVoxels;
    if (ds)
    {
        myDisplayedXMin = ds->GetX() - 10.0 * Precision::Confusion();
        myDisplayedXMax = ds->GetX() + ds->GetXLen() + 10.0 * Precision::Confusion();
        myDisplayedYMin = ds->GetY() - 10.0 * Precision::Confusion();
        myDisplayedYMax = ds->GetY() + ds->GetYLen() + 10.0 * Precision::Confusion();
        myDisplayedZMin = ds->GetZ() - 10.0 * Precision::Confusion();
        myDisplayedZMax = ds->GetZ() + ds->GetZLen() + 10.0 * Precision::Confusion();
    }

    // Init visual data
    initPrs();

    // Set voxels and display
    Handle(Poly_Triangulation) empty;
    myVoxels->SetBoolVoxels(myBoolVoxels);
	myVoxels->SetColorVoxels(myColorVoxels);
    myVoxels->SetTriangulation(empty);
    if (myViewer->getIC()->IsDisplayed(myVoxels))
        myViewer->getIC()->Redisplay(myVoxels, false);
    else
	    myViewer->getIC()->Display(myVoxels, false);
    
    // Color scale
    if (myColorVoxels)
        displayColorScale();
    else
      myViewer->getIC()->Erase(myColorScale);

	myViewer->getView()->FitAll();
}

void Application::setNbX()
{
	bool ok;
	myNbX = 
		QInputDialog::getInteger(this, "Voxel demo-application", "Number of splits in X-direction:", myNbX,
														 1, 100000, 1, &ok);
}

void Application::setNbY()
{
	bool ok;
	myNbY = 
		QInputDialog::getInteger(this, "Voxel demo-application", "Number of splits in X-direction:", myNbY,
														 1, 100000, 1, &ok);
}

void Application::setNbZ()
{
	bool ok;
	myNbZ = 
		QInputDialog::getInteger(this, "Voxel demo-application", "Number of splits in X-direction:", myNbZ,
														 1, 100000, 1, &ok);
}

void Application::setColorMinValue()
{
	bool ok;
	myColorMinValue = 
		QInputDialog::getInteger(this, "Voxel demo-application", "Minimum value for color [0 .. 15]:", myColorMinValue,
								 0, 15, 1, &ok);
    if (!myVoxels.IsNull())
        myVoxels->SetColorRange(myColorMinValue, myColorMaxValue);
}

void Application::setColorMaxValue()
{
	bool ok;
	myColorMaxValue = 
		QInputDialog::getInteger(this, "Voxel demo-application", "Maximum value for color [0 .. 15]:", myColorMaxValue,
								 0, 15, 1, &ok);
    if (!myVoxels.IsNull())
        myVoxels->SetColorRange(myColorMinValue, myColorMaxValue);
}

void Application::setUsageOfGLlists()
{
    int res = QMessageBox::question( this, "Voxel demo-application", "Press Yes to use GL lists and No not to use them.", QMessageBox::Yes, QMessageBox::No);
    if (!myVoxels.IsNull())
        myVoxels->SetUsageOfGLlists(res == QMessageBox::Yes);
}

void Application::setDisplayedXMin()
{
	myDisplayedXMin = QInputDialog::getDouble(this, "Voxel demo-application", "Minimum X value:", myDisplayedXMin);
    if (!myVoxels.IsNull())
    {
        myVoxels->SetSizeRange(myDisplayedXMin, myDisplayedXMax,
                               myDisplayedYMin, myDisplayedYMax,
                               myDisplayedZMin, myDisplayedZMax);
    }
}

void Application::setDisplayedXMax()
{
	myDisplayedXMax = QInputDialog::getDouble(this, "Voxel demo-application", "Maximum X value:", myDisplayedXMax);
    if (!myVoxels.IsNull())
    {
        myVoxels->SetSizeRange(myDisplayedXMin, myDisplayedXMax,
                               myDisplayedYMin, myDisplayedYMax,
                               myDisplayedZMin, myDisplayedZMax);
    }
}

void Application::setDisplayedYMin()
{
	myDisplayedYMin = QInputDialog::getDouble(this, "Voxel demo-application", "Minimum Y value:", myDisplayedYMin);
    if (!myVoxels.IsNull())
    {
        myVoxels->SetSizeRange(myDisplayedXMin, myDisplayedXMax,
                               myDisplayedYMin, myDisplayedYMax,
                               myDisplayedZMin, myDisplayedZMax);
    }
}

void Application::setDisplayedYMax()
{
	myDisplayedYMax = QInputDialog::getDouble(this, "Voxel demo-application", "Maximum Y value:", myDisplayedYMax);
    if (!myVoxels.IsNull())
    {
        myVoxels->SetSizeRange(myDisplayedXMin, myDisplayedXMax,
                               myDisplayedYMin, myDisplayedYMax,
                               myDisplayedZMin, myDisplayedZMax);
    }
}

void Application::setDisplayedZMin()
{
	myDisplayedZMin = QInputDialog::getDouble(this, "Voxel demo-application", "Minimum Z value:", myDisplayedZMin);
    if (!myVoxels.IsNull())
    {
        myVoxels->SetSizeRange(myDisplayedXMin, myDisplayedXMax,
                               myDisplayedYMin, myDisplayedYMax,
                               myDisplayedZMin, myDisplayedZMax);
    }
}

void Application::setDisplayedZMax()
{
	myDisplayedZMax = QInputDialog::getDouble(this, "Voxel demo-application", "Maximum Z value:", myDisplayedZMax);
    if (!myVoxels.IsNull())
    {
        myVoxels->SetSizeRange(myDisplayedXMin, myDisplayedXMax,
                               myDisplayedYMin, myDisplayedYMax,
                               myDisplayedZMin, myDisplayedZMax);
    }
}

void Application::setScanSide()
{
	myScanSide = 
		QInputDialog::getInteger(this, "Voxel demo-application", "Side of scanning (1: +X side, 2: +Y side, 3: +Z side, 4: +X & +Y sides, .. 7: +X, +Y,& +Z sides):", 
                                 myScanSide, 1, 7, 1);
}

void Application::setVolumicBoolValue()
{
	myVolumicBoolValue = 
		QInputDialog::getInteger(this, "Voxel demo-application", "Volumic value on voxelization [0 .. 1]:", 
                                 myVolumicBoolValue, 0, 1, 1);
}

void Application::setVolumicColorValue()
{
	myVolumicColorValue = 
		QInputDialog::getInteger(this, "Voxel demo-application", "Volumic value on voxelization [0 .. 15]:", 
                                 myVolumicColorValue, 0, 15, 1);
}

void Application::setQuadrangleSize()
{
	myQuadrangleSize = 
		QInputDialog::getInteger(this, "Voxel demo-application", "Size of quadrangles (0% .. 100%):", 
                                 myQuadrangleSize, 1, 100, 10);
    if (!myVoxels.IsNull())
    {
        myVoxels->SetQuadrangleSize(myQuadrangleSize);
    }
}

void Application::setPointSize()
{
	myPointSize = 
		QInputDialog::getInteger(this, "Voxel demo-application", "Size of points (1 .. 10):", 
                                 myPointSize, 1, 10, 1);
    if (!myVoxels.IsNull())
    {
        myVoxels->SetPointSize(myPointSize);
    }
}

void Application::display(Voxel_VoxelDisplayMode mode)
{
    if (myVoxels.IsNull() || !myViewer->getIC()->IsDisplayed(myVoxels))
    {
		QMessageBox::warning( this, "Voxel demo-application", "Voxels are not displayed");
		return;
	}

    myVoxels->SetDisplayMode(mode);

    if (myColorVoxels)
        displayColorScale();
    else
        myViewer->getIC()->Erase(myColorScale);

    myViewer->getIC()->Redisplay(myVoxels, true);
}

void Application::displayPoints()
{
    display(Voxel_VDM_POINTS);
}

void Application::displayNearestPoints()
{
    display(Voxel_VDM_NEARESTPOINTS);
}

void Application::displayBoxes()
{
    display(Voxel_VDM_BOXES);
}

void Application::displayNearestBoxes()
{
    display(Voxel_VDM_NEARESTBOXES);
}

void Application::displayColorScale()
{
  if (myColorScale.IsNull())
  {
    myColorScale = new AIS_ColorScale;
  }
	if (!myColorScale.IsNull())
	{
        int nb_colors = 1<<4 /* 4 bits */;
        myColorScale->SetRange(0, nb_colors - 1);
        myColorScale->SetNumberOfIntervals(nb_colors);
		myColorScale->SetPosition(0.01, 0.5 - 0.01);
		myColorScale->SetSize(0.5 - 0.01, 0.5 - 0.01);
	}
  myViewer->getIC()->Display(myColorScale);
}

void Application::displayWaves()
{
    myViewer->getIC()->EraseAll(false);

    // Make voxels
    if (myBoolVoxels)
    {
        delete myBoolVoxels;
        myBoolVoxels = 0;
    }
    if (myColorVoxels)
        delete myColorVoxels;

    int nbx = 500, nby = 50, nbz = 50;
    double xlen = 100.0, ylen = 100.0, zlen = 20.0;
    double dx = xlen / (double) nbx, dy = ylen / (double) nby, dz = zlen / (double) nbz;
    myColorVoxels = new Voxel_ColorDS(0.0, 0.0, 0.0, xlen, ylen, zlen, nbx, nby, nbz);

    // Initial state - no colors
    int ix, iy, iz;
    for (ix = 0; ix < nbx; ix++)
    {
        for (iy = 0; iy < nby; iy++)
        {
            for (iz = 0; iz < nbz; iz++)
            {
                myColorVoxels->Set(ix, iy, iz, 0);
            }
        }
    }

    // Init visual data
    initPrs();
    myVoxels->SetDisplayMode(Voxel_VDM_POINTS);
    myVoxels->SetUsageOfGLlists(false);
	myVoxels->SetBoolVoxels(myBoolVoxels);
	myVoxels->SetColorVoxels(myColorVoxels);
    if (myViewer->getIC()->IsDisplayed(myVoxels))
        myViewer->getIC()->Redisplay(myVoxels, false);
    else
	    myViewer->getIC()->Display(myVoxels, false);
    myViewer->getView()->FitAll();

    // Prepare arrays of values
    // X&Z values
    int i = 0, di = 5 /* nb waves */;
    int* zvalues = new int[nbx];
    unsigned char* xvalues = new unsigned char[nbx];
    for (ix = 0; ix < nbx; ix++, i += di)
    {
        if (i > nbx || i < 0)
        {
            di *= -1;
            i += di;
        }
        double rad = -M_PI / 2.0 + double(i) / (double) nbx * M_PI;
        double c = cos(rad);
        xvalues[ix] = 15.0 * c;
        if (xvalues[ix] == 0)
            xvalues[ix] = 1;
        zvalues[ix] = (nbz - 2) * c;
    }

    // Make waves
    unsigned char value = 0;
    for (i = 0; i <= 100; i++)
    {
        for (ix = 0; ix < nbx; ix++)
        {
            int ixi = ix + i;
            if (ixi >= nbx)
                ixi -= nbx;
            for (iz = 0; iz < nbz; iz++)
            {
                value = 0;
                if (iz < zvalues[ixi])
                    value = xvalues[ixi];
                for (iy = 0; iy < nby; iy++)
                {
                    myColorVoxels->Set(ix, iy, iz, value);
                }
            }
        }
        myViewer->getIC()->Redisplay(myVoxels, true);
        qApp->processEvents();
    }

    delete[] xvalues;
    delete[] zvalues;
}

void Application::initPrs()
{
	if (myVoxels.IsNull())
    {
		myVoxels = new VoxelClient_PrsGl();
        myVoxels->SetDisplayMode(Voxel_VDM_POINTS);
        myVoxels->SetColor(Quantity_NOC_WHITE);
        myVoxels->SetPointSize(1.0);
        myVoxels->SetSmoothPoints(false);
        myVoxels->SetQuadrangleSize(myQuadrangleSize);
        myVoxels->SetColorRange(myColorMinValue, myColorMaxValue);
        // Colors of ColorDS
        int nb_colors = 16 /* 4 bits */;
        Handle(Quantity_HArray1OfColor) colors = new Quantity_HArray1OfColor(0, nb_colors - 1);
        for (int icolor = 0; icolor < nb_colors; icolor++)
        {
            Quantity_Color color;
            AIS_ColorScale::FindColor(icolor, 0, nb_colors - 1, nb_colors, color);
            colors->SetValue(icolor, color);
        }
        myVoxels->SetColors(colors);
        myViewer->setPrs(myVoxels);
    }
    else
    {
        myViewer->getIC()->RecomputePrsOnly(myVoxels, false);
    }
}

void Application::box()
{
    gp_Ax2 axes(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
    TopoDS_Shape S = BRepPrimAPI_MakeBox(axes, 100, 100, 100);
    load(S);
}

void Application::cylinder()
{
    TopoDS_Shape S = BRepPrimAPI_MakeCylinder(50, 100);
    load(S);
}

void Application::torus()
{
    TopoDS_Shape S = BRepPrimAPI_MakeTorus(100, 20);
    load(S);
}

void Application::sphere()
{
    TopoDS_Shape S = BRepPrimAPI_MakeSphere(100);
    load(S);
}

void Application::load(const TopoDS_Shape& S)
{
	myViewer->getIC()->EraseAll(false);

    // Delete voxels of previous shape.
    if (myBoolVoxels)
    {
        delete myBoolVoxels;
        myBoolVoxels = 0;
    }
    if (myColorVoxels)
    {
        delete myColorVoxels;
        myColorVoxels = 0;
    }
    
    // Set view size
    Bnd_Box box;
    double xmin, ymin, zmin, xmax, ymax, zmax, length = 0;
    BRepBndLib::Add(S, box);
    box.Get(xmin, ymin, zmin, xmax, ymax, zmax);
    length = xmax - xmin > ymax - ymin ? xmax - xmin : ymax - ymin;
    length = length > zmax - zmin ? length : zmax - zmin;
    length *= 2.0;
    myViewer->getView()->SetSize(length);
    myViewer->getView()->SetZSize(length);

    // Display shape
	if (myShape.IsNull())
	{
		myShape = new AIS_Shape(S);
		myShape->SetDisplayMode(1);
	}
	else
	{
		myShape->Set(S);
        myViewer->getIC()->RecomputePrsOnly(myShape, false);
	}
    if (myViewer->getIC()->IsDisplayed(myShape))
        myViewer->getIC()->Redisplay(myShape, false);
    else
	    myViewer->getIC()->Display(myShape, false);
	myViewer->getView()->FitAll();
}

void Application::displayCut()
{
    myViewer->getIC()->EraseAll(false);

    // Make a sphere with a lot of toruses, 
    // cut the toruses from the sphere.
    TopoDS_Shape sphere = BRepPrimAPI_MakeSphere(100.0);
    TopoDS_Shape torus1 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt( 80,   0, 20), gp::DZ()), 30, 10);
    TopoDS_Shape torus2 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(  0,  80, 20), gp::DZ()), 30, 10);
    TopoDS_Shape torus3 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(-80,   0, 20), gp::DZ()), 30, 10);
    TopoDS_Shape torus4 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(  0, -80, 20), gp::DZ()), 30, 10);

    // Compute bounding box of the shapes
    Bnd_Box box;
    BRepBndLib::Add(sphere, box);
    BRepBndLib::Add(torus1, box);
    BRepBndLib::Add(torus2, box);
    BRepBndLib::Add(torus3, box);
    BRepBndLib::Add(torus4, box);

    // Nullify voxels
    if (myColorVoxels)
    {
        delete myColorVoxels;
        myColorVoxels = 0;
    }
    if (myBoolVoxels)
    {
        delete myBoolVoxels;
        myBoolVoxels = 0;
    }

    Timer timer;
    timer.Start();

    // Create a cube of voxels
    int nbx = 100, nby = 100, nbz = 100;
    double xmin, ymin, zmin, xmax, ymax, zmax;
    box.Get(xmin, ymin, zmin, xmax, ymax, zmax);
    myColorVoxels = new Voxel_ColorDS(xmin, ymin, zmin, 
                                      xmax - xmin, ymax - ymin, zmax - zmin,
                                      nbx, nby, nbz);
    Voxel_ColorDS vtorus(xmin, ymin, zmin, 
                         xmax - xmin, ymax - ymin, zmax - zmin,
                         nbx, nby, nbz);

    // Make a cube of voxels for the sphere.
    int progress;
    Voxel_FastConverter converter(sphere, *myColorVoxels, 0.1, nbx, nby, nbz);
    converter.Convert(progress);
    converter.FillInVolume(15);
   
    // Torus 1
    Voxel_FastConverter converter1(torus1, vtorus, 0.1, nbx, nby, nbz);
    converter1.Convert(progress);
    converter1.FillInVolume(3);
   
    // Torus 2
    Voxel_FastConverter converter2(torus2, vtorus, 0.1, nbx, nby, nbz);
    converter2.Convert(progress);
    converter2.FillInVolume(7);
   
    // Torus 3
    Voxel_FastConverter converter3(torus3, vtorus, 0.1, nbx, nby, nbz);
    converter3.Convert(progress);
    converter3.FillInVolume(10);
   
    // Torus 4
    Voxel_FastConverter converter4(torus4, vtorus, 0.1, nbx, nby, nbz);
    converter4.Convert(progress);
    converter4.FillInVolume(12);

    // Cut
    Voxel_BooleanOperation cutter;
    cutter.Cut(*myColorVoxels, vtorus);

    // Remove volumic voxels
    converter.FillInVolume(0);

    timer.Stop();
    timer.Print("Cut");

    // Display
    initPrs();
    myVoxels->SetDisplayMode(Voxel_VDM_POINTS);
    myVoxels->SetUsageOfGLlists(true);
	myVoxels->SetBoolVoxels(myBoolVoxels);
	myVoxels->SetColorVoxels(myColorVoxels);
    if (myViewer->getIC()->IsDisplayed(myVoxels))
        myViewer->getIC()->Redisplay(myVoxels, false);
    else
	    myViewer->getIC()->Display(myVoxels, false);
    myViewer->getView()->FitAll();
}

void Application::displayCollisions()
{
    myViewer->getIC()->EraseAll(false);

    // Make a big box with a lot of small spheres inside.
    double x = 0.0, y = 0.0, z = 0.0, xlen = 100.0, ylen = 100.0, zlen = 100.0, r = 10.0;
    gp_Pnt P1(x, y, z); // center point of moving sphere (S1).
    TopoDS_Shape B   = BRepPrimAPI_MakeBox(gp_Pnt(x-r, y-r, z-r), gp_Pnt(xlen+r, ylen+r, zlen+r));
    TopoDS_Shape S1  = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(x,       y,       z),       gp::DZ()), r / 2.0);
    TopoDS_Shape S2  = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen/2., y,       z),       gp::DZ()), r);
    TopoDS_Shape S3  = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen,    y,       z),       gp::DZ()), r);
    TopoDS_Shape S4  = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(x,       ylen/2., z),       gp::DZ()), r);
    TopoDS_Shape S5  = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen/2., ylen/2., z),       gp::DZ()), r);
    TopoDS_Shape S6  = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen,    ylen/2., z),       gp::DZ()), r);
    TopoDS_Shape S7  = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(x,       ylen,    z),       gp::DZ()), r);
    TopoDS_Shape S8  = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen/2., ylen,    z),       gp::DZ()), r);
    TopoDS_Shape S9  = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen,    ylen,    z),       gp::DZ()), r);
    TopoDS_Shape S10 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(x,       y,       zlen/2.), gp::DZ()), r);
    TopoDS_Shape S11 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen/2., y,       zlen/2.), gp::DZ()), r);
    TopoDS_Shape S12 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen,    y,       zlen/2.), gp::DZ()), r);
    TopoDS_Shape S13 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(x,       ylen/2., zlen/2.), gp::DZ()), r);
    TopoDS_Shape S14 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen/2., ylen/2., zlen/2.), gp::DZ()), r);
    TopoDS_Shape S15 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen,    ylen/2., zlen/2.), gp::DZ()), r);
    TopoDS_Shape S16 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(x,       ylen,    zlen/2.), gp::DZ()), r);
    TopoDS_Shape S17 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen/2., ylen,    zlen/2.), gp::DZ()), r);
    TopoDS_Shape S18 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen,    ylen,    zlen/2.), gp::DZ()), r);
    TopoDS_Shape S19 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(x,       y,       zlen),    gp::DZ()), r);
    TopoDS_Shape S20 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen/2., y,       zlen),    gp::DZ()), r);
    TopoDS_Shape S21 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen,    y,       zlen),    gp::DZ()), r);
    TopoDS_Shape S22 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(x,       ylen/2., zlen),    gp::DZ()), r);
    TopoDS_Shape S23 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen/2., ylen/2., zlen),    gp::DZ()), r);
    TopoDS_Shape S24 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen,    ylen/2., zlen),    gp::DZ()), r);
    TopoDS_Shape S25 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(x,       ylen,    zlen),    gp::DZ()), r);
    TopoDS_Shape S26 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen/2., ylen,    zlen),    gp::DZ()), r);
    TopoDS_Shape S27 = BRepPrimAPI_MakeSphere(gp_Ax2(gp_Pnt(xlen,    ylen,    zlen),    gp::DZ()), r);

    // Planes of the big box
    gp_Ax2 xminusPlane(gp_Pnt(x,    y,    z),    gp::DX());
    gp_Ax2 xplusPlane (gp_Pnt(xlen, y,    z),    gp::DX());
    gp_Ax2 yminusPlane(gp_Pnt(x,    y,    z),    gp::DY());
    gp_Ax2 yplusPlane (gp_Pnt(x,    ylen, z),    gp::DY());
    gp_Ax2 zminusPlane(gp_Pnt(x,    y,    z),    gp::DZ());
    gp_Ax2 zplusPlane (gp_Pnt(x,    y,    zlen), gp::DZ());

    // Nullify voxels
    if (myColorVoxels)
    {
        delete myColorVoxels;
        myColorVoxels = 0;
    }
    if (myBoolVoxels)
    {
        delete myBoolVoxels;
        myBoolVoxels = 0;
    }

    // Prepare visualization
    initPrs();
    myVoxels->SetDisplayMode(Voxel_VDM_POINTS);
    myVoxels->SetColor(Quantity_NOC_RED);
    myVoxels->SetPointSize(4);
    myVoxels->SetSmoothPoints(false);
    myVoxels->SetUsageOfGLlists(false);
	myVoxels->SetColorVoxels(myColorVoxels);

    // Display all shapes
    double transparency = 0.9;
    Handle(AIS_Shape) aisB   = new AIS_Shape(B);
    Handle(AIS_Shape) aisS1  = new AIS_Shape(S1);
    Handle(AIS_Shape) aisS2  = new AIS_Shape(S2);
    Handle(AIS_Shape) aisS3  = new AIS_Shape(S3);
    Handle(AIS_Shape) aisS4  = new AIS_Shape(S4);
    Handle(AIS_Shape) aisS5  = new AIS_Shape(S5);
    Handle(AIS_Shape) aisS6  = new AIS_Shape(S6);
    Handle(AIS_Shape) aisS7  = new AIS_Shape(S7);
    Handle(AIS_Shape) aisS8  = new AIS_Shape(S8);
    Handle(AIS_Shape) aisS9  = new AIS_Shape(S9);
    Handle(AIS_Shape) aisS10 = new AIS_Shape(S10);
    Handle(AIS_Shape) aisS11 = new AIS_Shape(S11);
    Handle(AIS_Shape) aisS12 = new AIS_Shape(S12);
    Handle(AIS_Shape) aisS13 = new AIS_Shape(S13);
    Handle(AIS_Shape) aisS14 = new AIS_Shape(S14);
    Handle(AIS_Shape) aisS15 = new AIS_Shape(S15);
    Handle(AIS_Shape) aisS16 = new AIS_Shape(S16);
    Handle(AIS_Shape) aisS17 = new AIS_Shape(S17);
    Handle(AIS_Shape) aisS18 = new AIS_Shape(S18);
    Handle(AIS_Shape) aisS19 = new AIS_Shape(S19);
    Handle(AIS_Shape) aisS20 = new AIS_Shape(S20);
    Handle(AIS_Shape) aisS21 = new AIS_Shape(S21);
    Handle(AIS_Shape) aisS22 = new AIS_Shape(S22);
    Handle(AIS_Shape) aisS23 = new AIS_Shape(S23);
    Handle(AIS_Shape) aisS24 = new AIS_Shape(S24);
    Handle(AIS_Shape) aisS25 = new AIS_Shape(S25);
    Handle(AIS_Shape) aisS26 = new AIS_Shape(S26);
    Handle(AIS_Shape) aisS27 = new AIS_Shape(S27);
    aisS1-> SetDisplayMode(1);
    aisS2-> SetDisplayMode(1);
    aisS3-> SetDisplayMode(1);
    aisS4-> SetDisplayMode(1);
    aisS5-> SetDisplayMode(1);
    aisS6-> SetDisplayMode(1);
    aisS7-> SetDisplayMode(1);
    aisS8-> SetDisplayMode(1);
    aisS9-> SetDisplayMode(1);
    aisS10->SetDisplayMode(1);
    aisS11->SetDisplayMode(1);
    aisS12->SetDisplayMode(1);
    aisS13->SetDisplayMode(1);
    aisS14->SetDisplayMode(1);
    aisS15->SetDisplayMode(1);
    aisS16->SetDisplayMode(1);
    aisS17->SetDisplayMode(1);
    aisS18->SetDisplayMode(1);
    aisS19->SetDisplayMode(1);
    aisS20->SetDisplayMode(1);
    aisS21->SetDisplayMode(1);
    aisS22->SetDisplayMode(1);
    aisS23->SetDisplayMode(1);
    aisS24->SetDisplayMode(1);
    aisS25->SetDisplayMode(1);
    aisS26->SetDisplayMode(1);
    aisS27->SetDisplayMode(1);
    aisS1-> SetTransparency(2.0 * transparency / 3.0);
    aisS2-> SetTransparency(transparency);
    aisS3-> SetTransparency(transparency);
    aisS4-> SetTransparency(transparency);
    aisS5-> SetTransparency(transparency);
    aisS6-> SetTransparency(transparency);
    aisS7-> SetTransparency(transparency);
    aisS8-> SetTransparency(transparency);
    aisS9-> SetTransparency(transparency);
    aisS10->SetTransparency(transparency);
    aisS11->SetTransparency(transparency);
    aisS12->SetTransparency(transparency);
    aisS13->SetTransparency(transparency);
    aisS14->SetTransparency(transparency);
    aisS15->SetTransparency(transparency);
    aisS16->SetTransparency(transparency);
    aisS17->SetTransparency(transparency);
    aisS18->SetTransparency(transparency);
    aisS19->SetTransparency(transparency);
    aisS20->SetTransparency(transparency);
    aisS21->SetTransparency(transparency);
    aisS22->SetTransparency(transparency);
    aisS23->SetTransparency(transparency);
    aisS24->SetTransparency(transparency);
    aisS25->SetTransparency(transparency);
    aisS26->SetTransparency(transparency);
    aisS27->SetTransparency(transparency);
    myViewer->getIC()->Display(aisB, false);
    myViewer->getIC()->Display(aisS1, false);
    myViewer->getIC()->Display(aisS2, false);
    myViewer->getIC()->Display(aisS3, false);
    myViewer->getIC()->Display(aisS4, false);
    myViewer->getIC()->Display(aisS5, false);
    myViewer->getIC()->Display(aisS6, false);
    myViewer->getIC()->Display(aisS7, false);
    myViewer->getIC()->Display(aisS8, false);
    myViewer->getIC()->Display(aisS9, false);
    myViewer->getIC()->Display(aisS10, false);
    myViewer->getIC()->Display(aisS11, false);
    myViewer->getIC()->Display(aisS12, false);
    myViewer->getIC()->Display(aisS13, false);
    myViewer->getIC()->Display(aisS14, false);
    myViewer->getIC()->Display(aisS15, false);
    myViewer->getIC()->Display(aisS16, false);
    myViewer->getIC()->Display(aisS17, false);
    myViewer->getIC()->Display(aisS18, false);
    myViewer->getIC()->Display(aisS19, false);
    myViewer->getIC()->Display(aisS20, false);
    myViewer->getIC()->Display(aisS21, false);
    myViewer->getIC()->Display(aisS22, false);
    myViewer->getIC()->Display(aisS23, false);
    myViewer->getIC()->Display(aisS24, false);
    myViewer->getIC()->Display(aisS25, false);
    myViewer->getIC()->Display(aisS26, false);
    myViewer->getIC()->Display(aisS27, false);

    // Prepare computer of collisions
    double deflection = 0.1;
    int nbx = 100, nby = 100, nbz = 100;
    
    Voxel_CollisionDetection coldet(deflection, nbx, nby, nbz);
    coldet.SetUsageOfVolume(false);
    coldet.KeepCollisions(false);
    coldet.AddShape(S1);
    coldet.AddShape(S2);
    coldet.AddShape(S3);
    coldet.AddShape(S4);
    coldet.AddShape(S5);
    coldet.AddShape(S6);
    coldet.AddShape(S7);
    coldet.AddShape(S8);
    coldet.AddShape(S9);
    coldet.AddShape(S10);
    coldet.AddShape(S11);
    coldet.AddShape(S12);
    coldet.AddShape(S13);
    coldet.AddShape(S14);
    coldet.AddShape(S15);
    coldet.AddShape(S16);
    coldet.AddShape(S17);
    coldet.AddShape(S18);
    coldet.AddShape(S19);
    coldet.AddShape(S20);
    coldet.AddShape(S21);
    coldet.AddShape(S22);
    coldet.AddShape(S23);
    coldet.AddShape(S24);
    coldet.AddShape(S25);
    coldet.AddShape(S26);
    coldet.AddShape(S27);
    //coldet.AddShape(BRepPrimAPI_MakeBox(gp_Pnt(x, y, z), gp_Pnt(xlen, ylen, zlen)));
    
    Bnd_Box box;
    BRepBndLib::Add(B, box);
    coldet.SetBoundaryBox(box);

    coldet.Voxelize();

    // Move one of the spheres inside the box
    // and compute collisions
    gp_Trsf trsf;
    gp_Vec vmove(1, 0.5, 0.25);

    int imove = 0, nb_moves = 900;
    while (imove < nb_moves)
    {
        // Move
        trsf.SetTranslation(vmove);
        TopLoc_Location loc(trsf);
        S1.Move(loc);
        P1.Translate(vmove);
        
        // Check whether S1 is inside the big box
        // Detect the plane S1 touches to.
        if (P1.X() < x)
            vmove.Mirror(xminusPlane);
        else if (P1.X() > xlen)
            vmove.Mirror(xplusPlane);
        else if (P1.Y() < y)
            vmove.Mirror(yminusPlane);
        else if (P1.Y() > ylen)
            vmove.Mirror(yplusPlane);
        else if (P1.Z() < z)
            vmove.Mirror(zminusPlane);
        else if (P1.Z() > zlen)
            vmove.Mirror(zplusPlane);

        // Compute collisions
        coldet.ReplaceShape(1, S1);
        coldet.Voxelize(1); // only the first sphere (S1)
        coldet.Compute();
        myBoolVoxels = &((Voxel_BoolDS&) coldet.GetCollisions());

        // Redisplay S1
        aisS1->Set(S1);
        myViewer->getIC()->Redisplay(aisS1, false);

        // Display the collisions
        myVoxels->SetBoolVoxels(myBoolVoxels);
        if (myViewer->getIC()->IsDisplayed(myVoxels))
            myViewer->getIC()->Redisplay(myVoxels, true);
        else
        {
	        myViewer->getIC()->Display(myVoxels, false);
            myViewer->getView()->FitAll();
        }

        imove++;
        qApp->processEvents();
    }

    // Copy the result of collision detection
    int ix, iy, iz;
    myBoolVoxels = new Voxel_BoolDS(coldet.GetCollisions().GetX(), 
                                    coldet.GetCollisions().GetY(), 
                                    coldet.GetCollisions().GetZ(), 
                                    coldet.GetCollisions().GetXLen(), 
                                    coldet.GetCollisions().GetYLen(), 
                                    coldet.GetCollisions().GetZLen(), 
                                    nbx, nby, nbz);
    for (ix = 0; ix < nbx; ix++)
    {
        for (iy = 0; iy < nby; iy++)
        {
            for (iz = 0; iz < nbz; iz++)
            {
                if (coldet.GetCollisions().Get(ix, iy, iz))
                    myBoolVoxels->Set(ix, iy, iz, Standard_True);
            }
        }
    }
    myVoxels->SetBoolVoxels(myBoolVoxels);
}