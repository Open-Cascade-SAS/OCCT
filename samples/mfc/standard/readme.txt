
1. Description of the tree structure:
---------------------------------------------

    |
    |-- 01_Geometry                
    |-- 02_Modeling
    |-- 03_Viewer2d
    |-- 04_Viewer3d                          	<Projects and sources of samples>
    |-- 05_ImportExport 			   
    |-- 06_Ocaf
    |-- 07_Triangulation
    |-- 08_HLR
    |-- 09_Animation
    |-- 10_Convert                        
    |    
    |-- All-vc"number".sln            			< Auxilary utility project. It depends on all other sample
    |                  			   projects, so when rebuilded it rebuilds all samples and 
    |                   			   mfcsample library. >
    |
    |-- Common 			< Common source and header files for samples and
    |    				   dynamic-link library mfcsample.dll. >
    | 
    |-- Data            			< Data files. >   
    |
    |-- mfcsample			< Project for mfcsample.dll. This library is used by all 
    |			 	   samples. It contains basic funcionality that commonly 
    |				   needed in every OCC sample. >    
    |
    |
    |-- env.bat       		This file is called from Run_MSVC.bat.

				   
2. Launching Open CASCADE Technology samples:
---------------------------------

To run the Open CASCADE Technology samples:
    
   > execute Run.bat [vc8|vc9|vc10] [win32|win64] [Release|Debug] [SampleName]

To run the Animation sample:

   > execute Run.bat vc8 win32 Debug Animation

3. Modifying and rebuilding samples:
--------------------------------------------

You can modify, compile and launch all sample projects in MS Visual C++ at once:
  
    > execute Run_MSVC.bat [vc8|vc9|vc10] [win32|win64] [Release|Debug]

To run all sample projects in MS Visual C++ at once: 
    
    > execute Run_MSVC.bat vc8 win32 Debug


Note: make sure your PATH environment variable contains a directory, 
      msdev.exe is located in.
