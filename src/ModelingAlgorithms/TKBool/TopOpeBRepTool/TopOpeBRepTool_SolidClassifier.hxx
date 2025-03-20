// Created on: 1996-08-27
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepTool_SolidClassifier_HeaderFile
#define _TopOpeBRepTool_SolidClassifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopOpeBRepTool_PSoClassif.hxx>
#include <TopTools_IndexedDataMapOfShapeAddress.hxx>
#include <TopAbs_State.hxx>
#include <BRep_Builder.hxx>
class gp_Pnt;

class TopOpeBRepTool_SolidClassifier
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepTool_SolidClassifier();

  Standard_EXPORT void Clear();

  Standard_EXPORT ~TopOpeBRepTool_SolidClassifier();

  Standard_EXPORT void LoadSolid(const TopoDS_Solid& S);

  //! compute the position of point <P> regarding with the
  //! geometric domain of the solid <S>.
  Standard_EXPORT TopAbs_State Classify(const TopoDS_Solid& S,
                                        const gp_Pnt&       P,
                                        const Standard_Real Tol);

  Standard_EXPORT void LoadShell(const TopoDS_Shell& S);

  //! compute the position of point <P> regarding with the
  //! geometric domain of the shell <S>.
  Standard_EXPORT TopAbs_State Classify(const TopoDS_Shell& S,
                                        const gp_Pnt&       P,
                                        const Standard_Real Tol);

  Standard_EXPORT TopAbs_State State() const;

protected:
private:
  TopOpeBRepTool_PSoClassif             myPClassifier;
  TopTools_IndexedDataMapOfShapeAddress myShapeClassifierMap;
  TopAbs_State                          myState;
  TopoDS_Shell                          myShell;
  TopoDS_Solid                          mySolid;
  BRep_Builder                          myBuilder;
};

#endif // _TopOpeBRepTool_SolidClassifier_HeaderFile
