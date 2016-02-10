Advanced Function Mechanism sample application
============================================== 

This sample demonstrates a simple way of using an advanced function mechanism of Ocaf.

Description
=========== 
The sample application represents a window demontsrating calculation of two simple models by the advanced function mechanism. The models are displayed as a graph (colored circles connected to each other). The links between circles define the dependences of the sample functions. Having pressed the button "Compute" the user may see how the function mechanism calculates the functions - the calculated circles change the color to yellow (indicating the process of calculation) and then, to blue - the function is computed. It is possible to define the number of threads to be used by the function mechanism (up to 4, just a limitation of this sample application). Having chosen 4 threads, for example, the user may see how the functions are calculated by 4 at once.

Compilation
===========
Run genproj.bat in a command-line to generate Visual Studio projects. For example, for Visual Studio 2010 call this line:
>genproj vc10 win32 debug
It generates VCPROJ (or VCXPROJ) files. Then, call the Visual Studio:
msvc vc10 win32 debug

Usage
=====
There are 4 menu-items:
Model \ Model1 - chooses the 1st test model. The application clears the previously selected model and displays a set of connected green circles (functions). You may move the circles by mouse for more convenient view.
Model \ Model2 - chooses the 2nd test model. It behaves the same as the Model1 described above.
Model \ Compute - runs the calculation of the chosen model in multi-threaded mode.
Model \ Number of threads - defines the number of threads (up to 4, for this sample application) to be used for calculation of a chosen model.

Implementation
==============
The models (Model1 and Model2) are hard-coded in MainWindow.cpp in the methods createDefaultModel1() and createDefaultModel2().
The Model1 represents a set of dependent "simple" functions. By "simple" I mean the function without a meaning. In other words, it doesn't matter what the function do, it is important how it depends on other functions and how it is being calculated by the function mechanism.
The Model2 has more meaning. It represents a process of creation of a shape. A Circle depends on a Point, A Prism is built on a Circle, ...

