// Created on: 1996-04-09
// Created by: Joelle CHAUVET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _AdvApp2Var_Node_HeaderFile
#define _AdvApp2Var_Node_HeaderFile

#include <Standard.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Type.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>

//! used to store constraints on a (Ui,Vj) point
class AdvApp2Var_Node : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(AdvApp2Var_Node, Standard_Transient)
public:
  Standard_EXPORT AdvApp2Var_Node();

  Standard_EXPORT AdvApp2Var_Node(const int iu, const int iv);

  Standard_EXPORT AdvApp2Var_Node(const gp_XY& UV, const int iu, const int iv);

  //! Returns the coordinates (U,V) of the node
  const gp_XY& Coord() const { return myCoord; }

  //! changes the coordinates (U,V) to (x1,x2)
  void SetCoord(const double x1, const double x2)
  {
    myCoord.SetX(x1);
    myCoord.SetY(x2);
  }

  //! returns the continuity order in U of the node
  int UOrder() const { return myOrdInU; }

  //! returns the continuity order in V of the node
  int VOrder() const { return myOrdInV; }

  //! affects the value F(U,V) or its derivates on the node (U,V)
  void SetPoint(const int iu, const int iv, const gp_Pnt& Pt) { myTruePoints.SetValue(iu, iv, Pt); }

  //! returns the value F(U,V) or its derivates on the node (U,V)
  const gp_Pnt& Point(const int iu, const int iv) const { return myTruePoints.Value(iu, iv); }

  //! affects the error between F(U,V) and its approximation
  void SetError(const int iu, const int iv, const double error)
  {
    myErrors.SetValue(iu, iv, error);
  }

  //! returns the error between F(U,V) and its approximation
  double Error(const int iu, const int iv) const { return myErrors.Value(iu, iv); }

  //! Assign operator.
  AdvApp2Var_Node& operator=(const AdvApp2Var_Node& theOther)
  {
    myTruePoints = theOther.myTruePoints;
    myErrors     = theOther.myErrors;
    myCoord      = theOther.myCoord;
    myOrdInU     = theOther.myOrdInU;
    myOrdInV     = theOther.myOrdInV;
    return *this;
  }

private:
  AdvApp2Var_Node(const AdvApp2Var_Node& theOther);

private:
  NCollection_Array2<gp_Pnt> myTruePoints;
  NCollection_Array2<double> myErrors;
  gp_XY                      myCoord;
  int                        myOrdInU;
  int                        myOrdInV;
};

#endif // _AdvApp2Var_Node_HeaderFile
