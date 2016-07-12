MFC samples {#samples_mfc_standard}
==========

1. Contents 
-----------------------

The directory <i> samples/mfc/standard </i> contains the following packages and files:

* Numbered packages: **01_Geometry**, **02_Modeling**, etc. provide projects and sources of samples;
* Files **All-vc(number).sln** are auxiliary utility projects depending on all other sample
projects. When such project is rebuilt, all samples and *mfcsample* library are also rebuilt.
* **Common** directory provides common source and header files for samples and dynamic-link library *mfcsample.dll.*
* **Data** directory stores data files.
* **mfcsample** directory contains project for *mfcsample.dll* library providing basic functionality used by all OCC samples. 
* File **env.bat** is called from *msvc.bat.*


2. Launching Open CASCADE Technology samples:
---------------------------------

To run the Open CASCADE Technology samples, use command:

~~~~
execute run.bat [vc10|vc11|vc12|vc14] [win32|win64] [Release|Debug] [SampleName]
~~~~

To run the **Animation** sample, use command:

~~~~
execute run.bat vc10 win64 Debug Animation
~~~~


3. Modifying and rebuilding samples:
--------------------------------------------

You can modify, compile and launch all sample projects in MS Visual C++ at once with command:

~~~~  
execute msvc.bat [vc10|vc11|vc12|vc14] [win32|win64] [Release|Debug]
~~~~

To run all sample projects in MS Visual C++ at once, use command: 

~~~~
execute msvc.bat vc10 win64 Debug
~~~~

Note: make sure that your *PATH* environment variable contains a directory, where *msdev.exe* is located.