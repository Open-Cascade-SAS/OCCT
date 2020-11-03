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

#include <DBRep_Params.hxx>
#include <Draw_Interpretor.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>

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
  
  //! Returns the shape in the variable.
  //! @param theName [in] [out] variable name, or "." to pick up shape interactively (the picked name will be returned then)
  //! @param theType [in]       shape type filter; function will return NULL if shape has different type
  //! @param theToComplain [in] when TRUE, prints a message on cout if the variable is not set
  static TopoDS_Shape Get (Standard_CString& theName, TopAbs_ShapeEnum theType = TopAbs_SHAPE, Standard_Boolean theToComplain = Standard_False)
  {
    return getShape (theName, theType, theToComplain);
  }

  //! Returns the shape in the variable.
  //! @param theName [in] [out] variable name, or "." to pick up shape interactively (the picked name will be returned then)
  //! @param theType [in]       shape type filter; function will return NULL if shape has different type
  //! @param theToComplain [in] when TRUE, prints a message on cout if the variable is not set
  static TopoDS_Shape Get (TCollection_AsciiString& theName, TopAbs_ShapeEnum theType = TopAbs_SHAPE, Standard_Boolean theToComplain = Standard_False)
  {
    Standard_CString aNamePtr = theName.ToCString();
    TopoDS_Shape aShape = getShape (aNamePtr, theType, theToComplain);
    if (aNamePtr != theName.ToCString())
    {
      theName = aNamePtr;
    }
    return aShape;
  }

  //! Returns the shape in the variable.
  //! @param theName [in] variable name
  //! @param theType [in] shape type filter; function will return NULL if shape has different type
  //! @param theToComplain [in] when TRUE, prints a message on cout if the variable is not set
  static TopoDS_Shape GetExisting (const TCollection_AsciiString& theName, TopAbs_ShapeEnum theType = TopAbs_SHAPE, Standard_Boolean theToComplain = Standard_False)
  {
    if (theName.Length() == 1
     && theName.Value (1) == '.')
    {
      return TopoDS_Shape();
    }

    Standard_CString aNamePtr = theName.ToCString();
    return getShape (aNamePtr, theType, theToComplain);
  }

  //! Defines the basic commands.
  Standard_EXPORT static void BasicCommands (Draw_Interpretor& theCommands);

  //! Return global parameters.
  Standard_EXPORT static DBRep_Params& Parameters();

  //! True if HLR, False if wireframe.
  static Standard_Boolean HLRMode() { return Parameters().WithHLR; }

  //! True if display Rg1Lines.
  static Standard_Boolean Rg1Mode() { return Parameters().WithRg1; }

  //! True if display RgNLines.
  static Standard_Boolean RgNMode() { return Parameters().WithRgN; }

  //! True if display HiddenLines.
  static Standard_Boolean HidMode() { return Parameters().WithHid; }

  //! discretisation angle for edges.
  static Standard_Real HLRAngle() { return Parameters().HLRAngle; }

  //! number of iso in U and V
  static Standard_Integer NbIsos() { return Parameters().NbIsos; }

  //! Discretization number of points for curves
  static Standard_Integer Discretisation() { return Parameters().Discretization; }

protected:

  //! Returns the shape in the variable.
  //! @param theName [in] [out] variable name, or "." to pick up shape interactively (the picked name will be returned then)
  //! @param theType [in]       shape type filter; function will return NULL if shape has different type
  //! @param theToComplain [in] when TRUE, prints a message on cout if the variable is not set
  Standard_EXPORT static TopoDS_Shape getShape (Standard_CString& theName,
                                                TopAbs_ShapeEnum theType,
                                                Standard_Boolean theToComplain);

};

#endif // _DBRep_HeaderFile
