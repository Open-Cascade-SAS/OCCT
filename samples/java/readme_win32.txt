
1. Description of the tree structure created:
---------------------------------------------

<installation directory>
    |
    |-- win32 <Platform dependent files>
    |    |
    |    `-- lib  <Shareables files: one <package>.dll for each package>
    |    
    |-- adm
    |    |
    |    `- win32 <Compiling projects for the win32 platform>
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


You can use JRE from version 1.6 and higher

To run the Open CASCADE Technology samples use run.bat file.


3. Modifying and rebuilding the executables:
--------------------------------------------

You can rebuild all the Sample distribution with : 
    > use the Samples.sln delivered on adm/win32


To modify Java source files go to the java directory and edit .java files.
To recompile changed files use build.bat file.

To modify C++ source files go to the src directory and find the package
you want to modify.



