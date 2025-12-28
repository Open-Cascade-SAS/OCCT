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
#include <TopoDS_Shape.hxx>

//! Used to display BRep objects using the DrawTrSurf
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
  //! Sets <S> in the variable <Name>.
  //! Overwrite the variable if already set.
  Standard_EXPORT static void Set(const char* Name, const TopoDS_Shape& S);

  //! Returns the shape in the variable.
  //! @param[in][out] theName   variable name, or "." to pick up shape interactively (the picked
  //! name will be returned then)
  //! @param[in] theType        shape type filter; function will return NULL if shape has different
  //! type
  //! @param[in] theToComplain  when TRUE, prints a message on cout if the variable is not set
  static TopoDS_Shape Get(const char*& theName,
                          TopAbs_ShapeEnum  theType       = TopAbs_SHAPE,
                          bool  theToComplain = false)
  {
    return getShape(theName, theType, theToComplain);
  }

  //! Returns the shape in the variable.
  //! @param[in][out] theName   variable name, or "." to pick up shape interactively (the picked
  //! name will be returned then)
  //! @param[in] theType        shape type filter; function will return NULL if shape has different
  //! type
  //! @param[in] theToComplain  when TRUE, prints a message on cout if the variable is not set
  static TopoDS_Shape Get(TCollection_AsciiString& theName,
                          TopAbs_ShapeEnum         theType       = TopAbs_SHAPE,
                          bool         theToComplain = false)
  {
    const char* aNamePtr = theName.ToCString();
    TopoDS_Shape     aShape   = getShape(aNamePtr, theType, theToComplain);
    if (aNamePtr != theName.ToCString())
    {
      theName = aNamePtr;
    }
    return aShape;
  }

  //! Returns the shape in the variable.
  //! @param[in] theName  variable name
  //! @param[in] theType  shape type filter; function will return NULL if shape has different type
  //! @param[in] theToComplain  when TRUE, prints a message on cout if the variable is not set
  static TopoDS_Shape GetExisting(const TCollection_AsciiString& theName,
                                  TopAbs_ShapeEnum               theType       = TopAbs_SHAPE,
                                  bool               theToComplain = false)
  {
    if (theName.Length() == 1 && theName.Value(1) == '.')
    {
      return TopoDS_Shape();
    }

    const char* aNamePtr = theName.ToCString();
    return getShape(aNamePtr, theType, theToComplain);
  }

  //! Defines the basic commands.
  Standard_EXPORT static void BasicCommands(Draw_Interpretor& theCommands);

  //! Return global parameters.
  Standard_EXPORT static DBRep_Params& Parameters();

  //! True if HLR, False if wireframe.
  static bool HLRMode() { return Parameters().WithHLR; }

  //! True if display Rg1Lines.
  static bool Rg1Mode() { return Parameters().WithRg1; }

  //! True if display RgNLines.
  static bool RgNMode() { return Parameters().WithRgN; }

  //! True if display HiddenLines.
  static bool HidMode() { return Parameters().WithHid; }

  //! discretisation angle for edges.
  static double HLRAngle() { return Parameters().HLRAngle; }

  //! number of iso in U and V
  static int NbIsos() { return Parameters().NbIsos; }

  //! Discretization number of points for curves
  static int Discretisation() { return Parameters().Discretization; }

protected:
  //! Returns the shape in the variable.
  //! @param[in][out] theName   variable name, or "." to pick up shape interactively (the picked
  //! name will be returned then)
  //! @param[in] theType        shape type filter; function will return NULL if shape has different
  //! type
  //! @param[in] theToComplain  when TRUE, prints a message on cout if the variable is not set
  Standard_EXPORT static TopoDS_Shape getShape(const char*& theName,
                                               TopAbs_ShapeEnum  theType,
                                               bool  theToComplain);
};

#endif // _DBRep_HeaderFile
