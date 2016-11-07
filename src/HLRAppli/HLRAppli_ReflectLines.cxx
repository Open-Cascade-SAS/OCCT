// File:      HLRAppli_ReflectLines.cxx
// Created:   05.12.12 12:55:50
// Created by: Julia GERASIMOVA
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <HLRAppli_ReflectLines.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <BRep_Builder.hxx>
#include <gp_Ax3.hxx>
#include <BRepLib.hxx>


//=======================================================================
//function : HLRAppli_ReflectLines
//purpose  : Constructor
//=======================================================================

HLRAppli_ReflectLines::HLRAppli_ReflectLines(const TopoDS_Shape& aShape)
{
  myShape = aShape;
}

//=======================================================================
//function : SetAxes
//purpose  :
//=======================================================================

void HLRAppli_ReflectLines::SetAxes(const Standard_Real Nx,
                                   const Standard_Real Ny,
                                   const Standard_Real Nz,
                                   const Standard_Real XAt,
                                   const Standard_Real YAt,
                                   const Standard_Real ZAt,
                                   const Standard_Real XUp,
                                   const Standard_Real YUp,
                                   const Standard_Real ZUp)
{
  Standard_Boolean IsPerspective = Standard_False;
  Standard_Real aFocus = 1;
  //Prs3d_Projector aPrs3dProjector(IsPerspective, aFocus, Nx, Ny, Nz, XAt, YAt, ZAt, XUp, YUp, ZUp);

  gp_Pnt At (XAt,YAt,ZAt);
  gp_Dir Zpers (Nx,Ny,Nz);
  gp_Dir Ypers (XUp,YUp,ZUp);
  gp_Dir Xpers = Ypers.Crossed(Zpers);
  gp_Ax3 Axe (At, Zpers, Xpers);
  gp_Trsf T;
  T.SetTransformation(Axe);

  //myProjector = aPrs3dProjector.Projector();
  myProjector = HLRAlgo_Projector(T,IsPerspective,aFocus);
}

//=======================================================================
//function : Perform
//purpose  :
//=======================================================================

void HLRAppli_ReflectLines::Perform()
{
  myHLRAlgo = new HLRBRep_Algo();
  myHLRAlgo->Add( myShape, 0 );
  myHLRAlgo->Projector( myProjector );
  myHLRAlgo->Update();
  myHLRAlgo->Hide();

  /*
  HLRBRep_HLRToShape aHLRToShape( aHLRAlgo );

  myCompound = aHLRToShape.OutLineVCompound3d();

  BRepLib::SameParameter(myCompound,Precision::PConfusion(),Standard_False);
  */
}

//=======================================================================
//function : GetCompoundOf3dEdges
//purpose  :
//=======================================================================

TopoDS_Shape HLRAppli_ReflectLines::GetCompoundOf3dEdges(const HLRBRep_TypeOfResultingEdge type,
                                                         const Standard_Boolean            visible,
                                                         const Standard_Boolean            In3d) const
{
  HLRBRep_HLRToShape aHLRToShape( myHLRAlgo );

  TopoDS_Shape theCompound = aHLRToShape.CompoundOfEdges(type, visible, In3d);

  BRepLib::SameParameter(theCompound,Precision::PConfusion(),Standard_False);

  return theCompound;
}

//=======================================================================
//function : GetResult
//purpose  :
//=======================================================================

TopoDS_Shape HLRAppli_ReflectLines::GetResult() const
{
  return GetCompoundOf3dEdges(HLRBRep_OutLine, Standard_True, Standard_True);
}
