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
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_XY.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <Standard_Real.hxx>
class gp_XY;
class gp_Pnt;



//! used to store constraints on a (Ui,Vj) point
class AdvApp2Var_Node 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT AdvApp2Var_Node();
  
  Standard_EXPORT AdvApp2Var_Node(const Standard_Integer iu, const Standard_Integer iv);
  
  Standard_EXPORT AdvApp2Var_Node(const gp_XY& UV, const Standard_Integer iu, const Standard_Integer iv);
  
  Standard_EXPORT gp_XY Coord() const;
  
  Standard_EXPORT void SetCoord (const Standard_Real x1, const Standard_Real x2);
  
  Standard_EXPORT Standard_Integer UOrder() const;
  
  Standard_EXPORT Standard_Integer VOrder() const;
  
  Standard_EXPORT void SetPoint (const Standard_Integer iu, const Standard_Integer iv, const gp_Pnt& Cte);
  
  Standard_EXPORT gp_Pnt Point (const Standard_Integer iu, const Standard_Integer iv) const;
  
  Standard_EXPORT void SetError (const Standard_Integer iu, const Standard_Integer iv, const Standard_Real error);
  
  Standard_EXPORT Standard_Real Error (const Standard_Integer iu, const Standard_Integer iv) const;




protected:





private:

  
  Standard_EXPORT AdvApp2Var_Node(const AdvApp2Var_Node& Other);


  gp_XY myCoord;
  Standard_Integer myOrdInU;
  Standard_Integer myOrdInV;
  Handle(TColgp_HArray2OfPnt) myTruePoints;
  Handle(TColStd_HArray2OfReal) myErrors;


};







#endif // _AdvApp2Var_Node_HeaderFile
