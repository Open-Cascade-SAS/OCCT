========================================================================
       Sample Open Cascade Demo Presentation Application
========================================================================


This application is a one from a set of applications intended to show 
some features of Open Cascade.
You can see how a particular feature is realized in the source code.
The WNT implementation uses Microsoft Foundation Classes.

/////////////////////////////////////////////////////////////////////////////
Summary of what you will find in each of the files that
make up this application.
Below the string <Project> should be substituted by the name of the
particular project downloaded by you.

<Project>/
    This folder contains source files specific to this project.

<Project>.dsp
    This file (the project file) contains information at the project level
    and is used to build a project.

<Project>/<Project>_Presentation.h, <Project>/<Project>_Presentation.cpp
    These files contain the project presentation class <Project>_Presentation,
		which inherits OCCDemo_Presentation and determines the samples and
		the behaviour of all samples in the presentation. 
		<Project>_Presentation.cpp contains also the initialisation of the
		pointer OCCDemo_Presentation::Current with the new object of type
    <Project>_Presentation.

Common/
    This folder contains source files common to all possibly downloaded
		projects.

Common/OCCDemo_Presentation.h, Common/OCCDemo_Presentation.cpp
    These files contain the abstract class OCCDemo_Presentation which is
		the base for all project presentation classes. It serves as a link 
		between a platform independent project class and the platform dependent
		classes responsible for drawing objects on the screen (mainly 
    COCCDemoDoc class). OCCDemo_Presentation class contains static field 
		Current which points to a particular object of type <Project>_Presentation. 
		This pointer is initialized where <Project>_Presentation is implemented, 
		and is used by the class COCCDemoDoc.

Common/WNT/OCCDemo.h, Common/WNT/OCCDemo.cpp
    These files contain the main application class COCCDemoApp.

Common/WNT/MainFrm.h, Common/WNT/MainFrm.cpp
    These files contain the frame class CMainFrame, which is derived from
    CFrameWnd and controls all SDI frame features.

Common/WNT/OCCDemoDoc.h, Common/WNT/OCCDemoDoc.cpp
    These files contain COCCDemoDoc class which inherits CDocument. It
		controls the sequence of samples using the pointer
		OCCDemo_Presentation::Current. It also contains the objects of types
		AIS_InteractiveContext and V3d_Viewer to store and manipulate the 
		drawable objects. It also manipulates by the object of type CResultDialog
		in which the source code and results of samples are shown.

Common/WNT/OCCDemoView.h, Common/WNT/OCCDemoView.cpp
    These files contain COCCDemoView class which inherits CView. It processes
		all mouse and keyboard events dedicated to the 3d View.

Common/WNT/ResultDialog.h, Common/WNT/ResultDialog.cpp
    These files contain the class CResultDialog which is used to show
    the source code and results of samples.

Common/WNT/StdAfx.h, Common/WNT/StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named <Project>.pch and a precompiled types file named StdAfx.obj.

Common/WNT/Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

Common/WNT/OCCDemo.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses. It includes the icons and bitmaps that are stored
    in the RES subdirectory.

Common/WNT/res/OCCDemo.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file OCCDemo.rc.

Common/WNT/res/OCCDemo.rc2
    This file might contain resources that are not edited by Microsoft 
		Visual C++. Really it is empty.

Common/WNT/res/Toolbar.bmp, Common/WNT/res/Toolbar1.bmp
    These files contain bitmaps used to create tiled images for the toolbars.

Common/ISession/ISession_Curve.h, Common/ISession/ISession_Curve.cpp
    These files define the class ISession_Curve which is derived from
		AIS_InteractiveObject. It is intended to display Curves directly
		(not via Edges) in the AIS_Interactive_Context.

Common/ISession/ISession_Surface.h, Common/ISession/ISession_Surface.cpp
    These files define the class ISession_Surface which is derived from
		AIS_InteractiveObject. It is intended to display Surfaces directly
		(not via Faces) in the AIS_Interactive_Context.
