
1. Description of the tree structure created:
---------------------------------------------

<installation directory>
    |
    |-- $STATION <Platform dependent files>
    |    |
    |    |-- lib  <Shareables files: one lib<package>.so for each package>
    |    |
    |    `-- obj  <Object and dependency files: .o and .d>
    |    
    |-- adm
    |    |
    |    `- $STATION <Compiling scripts for the $STATION platform>
    |    
    |-- drv <Additional includes files>
    |    |
    |    |-- ...
    |    
    |-- inc <Includes files>
    |    
    |-- java <Java sources files and executables>
    |    |
    |    |-- ...
    |    
    `-- src <C++ sources files>
         |
         |-- ...



2. Launching the Open CASCADE Technology samples:
---------------------------------


You can use JRE from version 1.3 and higher

To run the Open CASCADE Technology samples use run.csh file.


3. Modifying and rebuilding the executables:
--------------------------------------------

You can rebuild all the Sample distribution with : REBUILD.COMP
    > csh -f REBUILD.COMP

To modify Java source files go to the java directory and edit .java files.
To recompile changed files use build.csh file.

To modify C++ source files go to the src directory and find the package
you want to modify.

