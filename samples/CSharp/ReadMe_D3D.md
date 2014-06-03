Direct3D CSharp sample {#samples_csharp_direct3d}
================== 
	
This sample demonstrates simple way of using OCCT and DirectX libraries in .Net application
whitten using CSharp and Windows Presentation Foundation (WPF).

The connection between .Net, OCCT (C++) and DirectX level is provided by proxy libraies,
OCCProxy and D3DProxy, written in C++/CLI. The proxy OCCProxy library contains single ref class
encapsulating OCCT viewer and providing functionality to manipulate this viewer
and to import / export OCCT shapes from / to several supported formats of CAD 
files (IGES, STEP, BREP). And the proxy D3DProxy library contains helper methods for rendering 
via DirectX.  

Organizing of user interface in this sample is provided by Windows Presentation Foundation (WPF).
And it has the same functionality as the standard OCCT Import/Export sample. The project is 
called "IE_WPF_D3D".

Note a few important details:

- to build this sample you should to download and install DirectX SDK
  http://www.microsoft.com/en-us/download/details.aspx?id=6812

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
Note that project files are provided only for VS 2010, you can open them in
newer versions of Visual Studio using automatic converter.
After conversion check option "Target framework" in properties of C# projects 
(tab "Application") to ensure that it corresponds to the version set in
the properties of the C++ projects (e.g. ".Net Framework 4.0" for VS 2010).

Run run_wpf-D3D.bat to launch the corresponding sample.

Note that all batch scripts use configuration defined in OCCT custom.bat file
as default; you can provide arguments specifying VS version, bitness, and mode
to override these settings, e.g.:

> msvc.bat vc10 win64 Debug
