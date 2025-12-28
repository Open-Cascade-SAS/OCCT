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
#include <gp_Ax3.hxx>
#include <BRepLib.hxx>

//=================================================================================================

HLRAppli_ReflectLines::HLRAppli_ReflectLines(const TopoDS_Shape& aShape)
{
  myShape = aShape;
}

//=================================================================================================

void HLRAppli_ReflectLines::SetAxes(const double Nx,
                                    const double Ny,
                                    const double Nz,
                                    const double XAt,
                                    const double YAt,
                                    const double ZAt,
                                    const double XUp,
                                    const double YUp,
                                    const double ZUp)
{
  bool IsPerspective = false;
  double    aFocus        = 1;
  // Prs3d_Projector aPrs3dProjector(IsPerspective, aFocus, Nx, Ny, Nz, XAt, YAt, ZAt, XUp, YUp,
  // ZUp);

  gp_Pnt  At(XAt, YAt, ZAt);
  gp_Dir  Zpers(Nx, Ny, Nz);
  gp_Dir  Ypers(XUp, YUp, ZUp);
  gp_Dir  Xpers = Ypers.Crossed(Zpers);
  gp_Ax3  Axe(At, Zpers, Xpers);
  gp_Trsf T;
  T.SetTransformation(Axe);

  // myProjector = aPrs3dProjector.Projector();
  myProjector = HLRAlgo_Projector(T, IsPerspective, aFocus);
}

//=================================================================================================

void HLRAppli_ReflectLines::Perform()
{
  myHLRAlgo = new HLRBRep_Algo();
  myHLRAlgo->Add(myShape, 0);
  myHLRAlgo->Projector(myProjector);
  myHLRAlgo->Update();
  myHLRAlgo->Hide();

  /*
  HLRBRep_HLRToShape aHLRToShape( aHLRAlgo );

  myCompound = aHLRToShape.OutLineVCompound3d();

  BRepLib::SameParameter(myCompound,Precision::PConfusion(),false);
  */
}

//=================================================================================================

TopoDS_Shape HLRAppli_ReflectLines::GetCompoundOf3dEdges(const HLRBRep_TypeOfResultingEdge type,
                                                         const bool            visible,
                                                         const bool In3d) const
{
  HLRBRep_HLRToShape aHLRToShape(myHLRAlgo);

  TopoDS_Shape theCompound = aHLRToShape.CompoundOfEdges(type, visible, In3d);

  BRepLib::SameParameter(theCompound, Precision::PConfusion(), false);

  return theCompound;
}

//=================================================================================================

TopoDS_Shape HLRAppli_ReflectLines::GetResult() const
{
  return GetCompoundOf3dEdges(HLRBRep_OutLine, true, true);
}
