// Created on: 1991-04-24
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _Draw_HeaderFile
#define _Draw_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class TCollection_AsciiString;
class Draw_Drawable3D;
class Draw_ProgressIndicator;
class Draw_Drawable3D;
class Draw_Drawable2D;
class Draw_Color;
class Draw_Display;
class Draw_Segment3D;
class Draw_Segment2D;
class Draw_Marker3D;
class Draw_Marker2D;
class Draw_Axis3D;
class Draw_Axis2D;
class Draw_Text3D;
class Draw_Text2D;
class Draw_Circle3D;
class Draw_Circle2D;
class Draw_Number;
class Draw_Chronometer;
class Draw_Grid;
class Draw_Box;
class Draw_ProgressIndicator;
class Draw_Printer;


//! MAQUETTE DESSIN MODELISATION
class Draw 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void Load (Draw_Interpretor& theDI, const TCollection_AsciiString& theKey, const TCollection_AsciiString& theResourceFileName, TCollection_AsciiString& theDefaultsDirectory, TCollection_AsciiString& theUserDefaultsDirectory, const Standard_Boolean Verbose = Standard_False);
  
  //! Sets a variable. Display it if <Disp> is true.
  Standard_EXPORT static void Set (const Standard_CString Name, const Handle(Draw_Drawable3D)& D, const Standard_Boolean Disp);
  
  //! Sets a    variable,  a  null   handle    clear the
  //! vartiable. Automatic display is context driven.
  Standard_EXPORT static void Set (const Standard_CString Name, const Handle(Draw_Drawable3D)& D);
  
  //! Sets a numeric variable.
  Standard_EXPORT static void Set (const Standard_CString Name, const Standard_Real val);

  //! Returns main DRAW interpretor.
  Standard_EXPORT static Draw_Interpretor& GetInterpretor();
  
  //! Returns a variable  value.  Null if  the  variable
  //! does not exist, a warning  is printed if  Complain
  //! is True.
  //!
  //! The name "."   does a graphic  selection.   If the
  //! selection is a variable <Name> is overwritten with
  //! the name of the variable.
  Standard_EXPORT static Handle(Draw_Drawable3D) Get (Standard_CString& Name, const Standard_Boolean Complain = Standard_True);
  
  //! Gets a   numeric  variable. Returns  True   if the
  //! variable exist.
  Standard_EXPORT static Standard_Boolean Get (const Standard_CString Name, Standard_Real& val);
  
  //! Sets a TCL sting variable
  Standard_EXPORT static void Set (const Standard_CString Name, const Standard_CString val);
  
  //! Converts numeric expression, that can involve DRAW
  //! variables, to real value.
  Standard_EXPORT static Standard_Real Atof (const Standard_CString Name);
  
  //! Converts numeric expression, that can involve DRAW
  //! variables, to integer value.
  //! Implemented as cast of Atof() to integer.
  Standard_EXPORT static Standard_Integer Atoi (const Standard_CString Name);
  
  //! Returns last graphic selection description.
  Standard_EXPORT static void LastPick (Standard_Integer& view, Standard_Integer& X, Standard_Integer& Y, Standard_Integer& button);
  
  //! Asks to repaint the screen after the current command.
  Standard_EXPORT static void Repaint();
  
  //! sets progress indicator
  Standard_EXPORT static void SetProgressBar (const Handle(Draw_ProgressIndicator)& thePI);
  
  //! gets progress indicator
  Standard_EXPORT static Handle(Draw_ProgressIndicator) GetProgressBar();
  
  //! Defines all Draw commands
  Standard_EXPORT static void Commands (Draw_Interpretor& I);
  
  //! Defines Draw basic commands
  Standard_EXPORT static void BasicCommands (Draw_Interpretor& I);
  
  //! Defines Draw variables handling commands.
  Standard_EXPORT static void VariableCommands (Draw_Interpretor& I);
  
  //! Defines Draw variables handling commands.
  Standard_EXPORT static void GraphicCommands (Draw_Interpretor& I);
  
  //! Defines Loads Draw plugins commands.
  Standard_EXPORT static void PloadCommands (Draw_Interpretor& I);
  
  //! Defines Draw unit commands
  Standard_EXPORT static void UnitCommands (Draw_Interpretor& I);




protected:





private:




friend class Draw_Drawable3D;
friend class Draw_Drawable2D;
friend class Draw_Color;
friend class Draw_Display;
friend class Draw_Segment3D;
friend class Draw_Segment2D;
friend class Draw_Marker3D;
friend class Draw_Marker2D;
friend class Draw_Axis3D;
friend class Draw_Axis2D;
friend class Draw_Text3D;
friend class Draw_Text2D;
friend class Draw_Circle3D;
friend class Draw_Circle2D;
friend class Draw_Number;
friend class Draw_Chronometer;
friend class Draw_Grid;
friend class Draw_Box;
friend class Draw_ProgressIndicator;
friend class Draw_Printer;

};







#endif // _Draw_HeaderFile
