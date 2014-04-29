OCCT CSharp sample {#samples_csharp}
================== 
	
This sample demonstrates simple way of using OCCT libraries in .Net application
whitten using CSharp and Windows Forms or Windows Presentation Foundation (WPF).

The connection between .Net and OCCT (C++) level is provided by proxy library,
OCCProxy, written in C++/CLI. The proxy library contains single ref class
encapsulating OCCT viewer and providing functionality to manipulate this viewer
and to import / export OCCT shapes from / to several supported formats of CAD 
files (IGES, STEP, BREP). 

The sample implements two approaches for organizing user interface with C#.
Both applications provide the same functionality as the standard OCCT 
Import/Export sample.
First project is called "IE_WinForms" and uses Windows Forms for GUI.
Second application is called "IE_WPF" and uses Windows Presentation Foundation.

Note a few important details:

- to encapsulate C++ class into a field of ref class, template class 
  NCollection_Haft provided by OCCT is used
  
- in order to work consistently on 64-bit systems with OCCT libraries built in 
  32-bit mode, C# assemblies need to have platform target explicitly set to "x86"
  (in project Properties / Build)
  
- this sample demonstrates indirect method of wrapping C++ to C# using manually
  created proxy library. Alternative method is available, wrapping individual
  OCCT classes to C# equivalents so that their full API is available to C# user
  and the code can be programmed on C# level similarly to C++ one. See desciption
  of OCCT C# Wrapper in Advanced Samples and Tools on OCCT web site at 
  http://www.opencascade.org/support/products/advsamples

- in WPF sample, WinForms control is used to encapsulate OCC viewer since WPF 
  does not provide necessary interface to embed OpenGl view. Other possible
  solution could be to render OpenGl scene in off-screen buffer and map it
  to WPF control as image. That approach would allow using all WPF features in
  control embedding OCCT viewer.

Run msvc.bat to start MS Visual Studio for building the sample.
Note that project files are provided only for VS 2008, you can open them in
newer versions of Visual Studio using automatic converter.
After conversion check option "Target framework" in properties of C# projects 
(tab "Application") to ensure that it corresponds to the version set in
the properties of the C++ projects (e.g. ".Net Framework 4.0" for VS 2010).

Run run_winforms.bat or run_wpf.bat to launch the corresponding sample.

Note that all batch scripts use configuration defined in OCCT custom.bat file
as default; you can provide arguments specifying VS version, bitness, and mode
to override these settings, e.g.:

> msvc.bat vc9 win64 Debug
