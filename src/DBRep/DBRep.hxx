// Created on: 1991-06-25
// Created by: Christophe MARION
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

#ifndef _DBRep_HeaderFile
#define _DBRep_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_CString.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Boolean.hxx>
#include <Draw_Interpretor.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class TopoDS_Shape;
class DBRep_Edge;
class DBRep_Face;
class DBRep_HideData;
class DBRep_DrawableShape;
class DBRep_IsoBuilder;


//! Used to display BRep objects  using the DrawTrSurf
//! package.
//! The DrawableShape is a Display object build from a
//! Shape.
//! Provides methods to manage a directory of named shapes.
//! Provides a set of Draw commands for Shapes.
class DBRep 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creation of isoparametric curves.
  //! Implements ProgressIndicator for DRAW
  //! Sets  <S> in the  variable  <Name>.  Overwrite the
  //! variable if already set.
  Standard_EXPORT static void Set (const Standard_CString Name, const TopoDS_Shape& S);
  
  //! Returns the shape in the variable  <Name>. Returns
  //! a null shape if the variable is not set or  not of
  //! the given <Typ>.  If <Complain> is  True a message
  //! is printed on cout if the variable is not set.
  Standard_EXPORT static TopoDS_Shape Get (Standard_CString& Name, const TopAbs_ShapeEnum Typ = TopAbs_SHAPE, const Standard_Boolean Complain = Standard_False);
  
  //! Defines the basic commands.
  Standard_EXPORT static void BasicCommands (Draw_Interpretor& theCommands);
  
  //! True if HLR, False if wireframe.
  Standard_EXPORT static Standard_Boolean HLRMode();
  
  //! True if display Rg1Lines.
  Standard_EXPORT static Standard_Boolean Rg1Mode();
  
  //! True if display RgNLines.
  Standard_EXPORT static Standard_Boolean RgNMode();
  
  //! True if display HiddenLines.
  Standard_EXPORT static Standard_Boolean HidMode();
  
  //! discretisation angle for edges.
  Standard_EXPORT static Standard_Real HLRAngle();
  
  //! number of iso in U and V
  Standard_EXPORT static Standard_Integer NbIsos();
  
  //! discretisation number of points for curves
  //! set progress indicator
  //! get progress indicator
  Standard_EXPORT static Standard_Integer Discretisation();




protected:





private:




friend class DBRep_Edge;
friend class DBRep_Face;
friend class DBRep_HideData;
friend class DBRep_DrawableShape;
friend class DBRep_IsoBuilder;

};







#endif // _DBRep_HeaderFile
