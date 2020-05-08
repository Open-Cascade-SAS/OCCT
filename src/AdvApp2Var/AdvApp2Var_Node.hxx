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

#include <gp_XY.hxx>
#include <Standard.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>

//! used to store constraints on a (Ui,Vj) point
class AdvApp2Var_Node : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(AdvApp2Var_Node, Standard_Transient)
public:

  Standard_EXPORT AdvApp2Var_Node();
  
  Standard_EXPORT AdvApp2Var_Node(const Standard_Integer iu, const Standard_Integer iv);
  
  Standard_EXPORT AdvApp2Var_Node(const gp_XY& UV, const Standard_Integer iu, const Standard_Integer iv);

  //! Returns the coordinates (U,V) of the node
  const gp_XY& Coord() const { return myCoord; }

  //! changes the coordinates (U,V) to (x1,x2)
  void SetCoord (const Standard_Real x1, const Standard_Real x2)
  {
    myCoord.SetX(x1);
    myCoord.SetY(x2);
  }

  //! returns the continuity order in U of the node
  Standard_Integer UOrder() const { return myOrdInU; }

  //! returns the continuity order in V of the node
  Standard_Integer VOrder() const { return myOrdInV; }

  //! affects the value F(U,V) or its derivates on the node (U,V)
  void SetPoint (const Standard_Integer iu, const Standard_Integer iv, const gp_Pnt& Pt)
  {
    myTruePoints.SetValue(iu, iv, Pt);
  }

  //! returns the value F(U,V) or its derivates on the node (U,V)
  const gp_Pnt& Point (const Standard_Integer iu, const Standard_Integer iv) const
  {
    return myTruePoints.Value(iu, iv);
  }

  //! affects the error between F(U,V) and its approximation
  void SetError (const Standard_Integer iu, const Standard_Integer iv, const Standard_Real error)
  {
    myErrors.SetValue(iu, iv, error);
  }

  //! returns the error between F(U,V) and its approximation
  Standard_Real Error (const Standard_Integer iu, const Standard_Integer iv) const { return myErrors.Value(iu, iv); }

  //! Assign operator.
  AdvApp2Var_Node& operator= (const AdvApp2Var_Node& theOther)
  {
    myTruePoints = theOther.myTruePoints;
    myErrors = theOther.myErrors;
    myCoord  = theOther.myCoord;
    myOrdInU = theOther.myOrdInU;
    myOrdInV = theOther.myOrdInV;
    return *this;
  }

private:

  AdvApp2Var_Node (const AdvApp2Var_Node& theOther);

private:

  TColgp_Array2OfPnt myTruePoints;
  TColStd_Array2OfReal myErrors;
  gp_XY myCoord;
  Standard_Integer myOrdInU;
  Standard_Integer myOrdInV;

};

#endif // _AdvApp2Var_Node_HeaderFile
