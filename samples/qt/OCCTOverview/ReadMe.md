Qt: OCCT Overview (C++|Qt Widgets) {#samples_qt_overview}
==========

The Overview Qt application provides code snippets for basic usage of C++ API of various OCCT functionality.
The samples are organized in several categories according to relevant module of OCCT:

 * Geometry
 * Topology
 * Triangulation
 * DataExchange
 * OCAF
 * Viewer 2d
 * Viewer 3d

Each sample presents geometry view, C++ code fragment and sample output window.
This sample is described in the @ref samples__novice_guide "Novice guide" for new users.

@figure{sample_overview_qt.png}

1. Contents
-----------------------

The directory <i> samples/qt/OCCTOverview </i> contains the folders and files of the Qt OCCT Overview application:

* Files **OCCTOverview.pro** and **OCCTOverview0.pro** are Qt project files.
* File **genproj.bat** to denerate MS Visual Studio project.
* File **msvc.bat**  to run MS Visual Studio project.
* File **make.sh** to build of the application on Linux.
* Files **run.bat** and **run.sh** to run the application.
* Files **env.bat** and **custom.bat** are called from *genproj.bat*, *msvc.bat*, *run.bat*.
  File *custom.bat* should be defined by user to provide paths to QT directory and OCCT installation directory (see *custom.bat.template*).
* **src** and **res** directories provide source and resources files.

The directory <i> samples/OCCTOverview/code </i> contains the source code of samples.

2. How to build Qt OCCT Overview application
---------------------------------

* Edit custom.bat file. It is necessary to define following variables:
  * **QTDIR** path to where Qt is installed
  * **CASROOT** path to where Open CASCADE binaries are installed.

* Build the application:

    * On Windows:
        * Generate project files: `> genproj.bat vc141 win64 Debug`
        * Launch MS Visual Studio: `> msvc.bat vc141 win64 Debug`
        * Build the application using MS Visual Studio.

    * On Linux: Launch building of the application by make.sh script

3. Running the application
--------------------------

* On Windows:
~~~~
 > run.bat vc141 win64 Debug
~~~~

* On Linux:
~~~~
 > run.sh
~~~~

4. How to use the OCCT Overview application:
---------------------------------

* To select a samples category use the *Category* menu.
* To run concrete sample using the menu to the right of the category menu.
* See the source code in the *Sample code* window. Сopy the code if needed.
* See the sample output in the *Output* window if it exist.
* Zoom, pan and rotate a geometry in the mail window using the mouse.

See hints how to use the mouse in down hints panel.
