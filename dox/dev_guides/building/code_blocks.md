Building with Code::Blocks on Mac OS X {#occt_dev_guides__building_code_blocks}
======================================

This file describes steps to build OCCT libraries from a complete source package
on Mac OS X with **Code::Blocks**.

If you build OCCT from bare sources (as in Git repository) or do some 
changes affecting CDL files, you need to use WOK to re-generate header files
and build scripts / projects. See \ref occt_dev_guides__building_wok for instructions.

Before building OCCT, you need to install the required third-party libraries; see
paragraph 1 of \ref occt_dev_guides__building for details.

1. Add paths to the mandatory 3rd-party products (**Tcl/Tk** and **FreeType**) in file 
   *custom.sh* located in <i>\<OCCT_ROOT_DIR\></i>. For this:

   1.1. Add paths to the includes in variable *CSF_OPT_INC*;

   1.2. Add paths to the binary libraries in variable  *CSF_OPT_LIB64*;
   
   All paths should be separated by ":" symbol. 

2. Add paths to the optional 3rd-party libraries (**TBB, gl2ps** and **FreeImage**) 
   in the aforementioned environment variables *CSF_OPT_INC* and 
   *CSF_OPT_LIB64* from file *custom.sh*.

   If you want to build OCCT without the optional libraries perform the 
   following steps:

   2.1 Disable unnecessary library in custom.sh by setting the corresponding 
       variable <i>HAVE_\<LIBRARY_NAME\></i> to *false*. 

~~~~~	   
export HAVE_GL2PS=false
~~~~~

   2.2 Remove this library from Linker settings in **Code::Blocks** for each project 
       that uses it: right click on the required project, choose **Build options**, 
       go to **Linker settings** tab in the opened window , select unnecessary 
       libraries and click **Delete** button.

3. Open Terminal application

4. Enter <i> \<OCCT_ROOT_DIR\></i>:

~~~~~
   cd \<OCCT_ROOT_DIR\>
~~~~~

5. To start **Code::Blocks**, run the command <i>/codeblocks.sh</i>

6. To build all toolkits, click **Build->Build workspace** in the menu bar.


To start *DRAWEXE*, which has been built with **Code::Blocks** on Mac OS X, perform 
the following steps:

1.Open Terminal application

2.Enter <i>\<OCCT_ROOT_DIR\></i>:

~~~~~
   cd \<OCCT_ROOT_DIR\>
~~~~~

3.Run the script
~~~~~
   ./draw_cbp.sh cbp [d]
~~~~~
   Option *d* is used if OCCT has been built in **Debug** mode.
