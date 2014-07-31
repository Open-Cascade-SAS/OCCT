Building with Xcode {#occt_dev_guides__building_xcode}
===================

This file describes steps to build OCCT libraries from a complete source package
on Mac OS X with **Xcode**.

If you build OCCT from bare sources (as in Git repository) or do some 
changes affecting CDL files, you need to use WOK to re-generate header files
and build scripts / projects. See \ref occt_dev_guides__building_wok for instructions.

Before building OCCT, you need to install the required third-party libraries; see
paragraph 1 of \ref occt_dev_guides__building for details.

1. Add paths to the mandatory 3rd-party products (**Tcl/Tk** and **FreeType**) 
   in file *custom.sh* located in <i>\<OCCT_ROOT_DIR\> </i>. For this:

   1.1. Add paths to the includes in variable *CSF_OPT_INC*;

   1.2. Add paths to the binary libraries in variable  *CSF_OPT_LIB64*;

   All paths should be separated by ":" symbol. 

2. Add paths to the optional 3rd-party libraries (**TBB, gl2ps** and **FreeImage**) 
   in the aforementioned environment variables *CSF_OPT_INC* and *CSF_OPT_LIB64* from file *custom.sh*.

   If you want to build OCCT without the optional libraries perform the following steps:

   2.1 Disable unnecessary library in *custom.sh* by setting the corresponding 
       variable <i>HAVE_<LIBRARY_NAME></i> to *false*. 
~~~~~
       export HAVE_GL2PS=false
~~~~~
   2.2 Remove this library from Project navigator in Xcode for each project that 
       uses it: choose the required project, right click on the unnecessary 
       library and select **Delete** button.

3. Open Terminal application.

4. Enter <i>\<OCCT_ROOT_DIR\></i>:
~~~~~
   cd \<OCCT_ROOT_DIR\>
~~~~~
5. To start **Xcode**, run command  <i>/xcode.sh</i>

6. To build a certain toolkit, select it in **Scheme** drop-down list in Xcode 
   toolbar, press **Product** in the menu and click **Build** button. 

   To build the entire OCCT, create a new empty project (select **File -> 
   New -> Project -> "Empty project** in the menu. Input the project name, 
   e.g. *OCCT*, click **Next** and **Create** buttons). Drag and drop the *OCCT* 
   folder in the created *OCCT* project in the Project navigator. Select 
   **File -> New -> Target -> Aggregate** in the menu. Enter the project name 
   (e.g. <i>OCCT</i>) and click **Finish**. The **Build Phases** tab will open.  
   Click "+" button to add the necessary toolkits to the target project. 
   It is possible to select all toolkits by pressing **Command+A** combination. 

To start *DRAWEXE*, which has been built with Xcode on Mac OS X, perform the following steps:

1.Open Terminal application

2.Enter <i>\<OCCT_ROOT_DIR\></i>:
~~~~~
   cd \<OCCT_ROOT_DIR\>
~~~~~

3.Run the script
~~~~~
   ./draw_cbp.sh xcd [d]
~~~~~
   Option *d* is used if OCCT has been built in **Debug** mode.
