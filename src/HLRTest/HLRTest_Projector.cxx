// Created on: 1995-04-05
// Created by: Christophe MARION
// Copyright (c) 1995-1999 Matra Datavision
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

#include <HLRTest_Projector.hxx>

#include <Draw_Display.hxx>
#include <gp_Ax3.hxx>
#include <HLRAlgo_Projector.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HLRTest_Projector, Draw_Drawable3D)

//=================================================================================================

HLRTest_Projector::HLRTest_Projector(const HLRAlgo_Projector& P)
    : myProjector(P)
{
  //
}

//=================================================================================================

void HLRTest_Projector::DrawOn(Draw_Display&) const
{
  //
}

//=================================================================================================

Handle(Draw_Drawable3D) HLRTest_Projector::Copy() const
{
  return new HLRTest_Projector(myProjector);
}

//=================================================================================================

void HLRTest_Projector::Dump(Standard_OStream& S) const
{
  S << "Projector : \n";
  if (myProjector.Perspective())
    S << "perspective, focal = " << myProjector.Focus() << "\n";

  for (Standard_Integer i = 1; i <= 3; i++)
  {

    for (Standard_Integer j = 1; j <= 4; j++)
    {
      S << std::setw(15) << myProjector.Transformation().Value(i, j);
    }
    S << "\n";
  }
  S << std::endl;
}

//=================================================================================================

void HLRTest_Projector::Save(Standard_OStream& theStream) const
{
  theStream << (myProjector.Perspective() ? "1" : "0") << "\n";
  if (myProjector.Perspective())
  {
    theStream << myProjector.Focus() << "\n";
  }

  const gp_Trsf aTransformation    = myProjector.Transformation();
  const gp_XYZ  aTranslationVector = aTransformation.TranslationPart();
  const gp_Mat  aMatrix            = aTransformation.VectorialPart();

  theStream << aMatrix(1, 1) << " ";
  theStream << aMatrix(1, 2) << " ";
  theStream << aMatrix(1, 3) << " ";
  theStream << aTranslationVector.Coord(1) << " ";
  theStream << "\n";
  theStream << aMatrix(2, 1) << " ";
  theStream << aMatrix(2, 2) << " ";
  theStream << aMatrix(2, 3) << " ";
  theStream << aTranslationVector.Coord(2) << " ";
  theStream << "\n";
  theStream << aMatrix(3, 1) << " ";
  theStream << aMatrix(3, 2) << " ";
  theStream << aMatrix(3, 3) << " ";
  theStream << aTranslationVector.Coord(3) << " ";
  theStream << "\n";
}

//=================================================================================================

Handle(Draw_Drawable3D) HLRTest_Projector::Restore(Standard_IStream& theStream)
{
  Standard_Boolean aPerspective = false;
  Standard_Real    aFocus       = 1.0;
  Standard_Real    aDirVect1[3], aDirVect2[3], aDirVect3[3];
  Standard_Real    aTranslationVector[3];
  theStream >> aPerspective;
  if (aPerspective)
  {
    theStream >> aFocus;
  }
  theStream >> aDirVect1[0] >> aDirVect1[1] >> aDirVect1[2] >> aTranslationVector[0];
  theStream >> aDirVect2[0] >> aDirVect2[1] >> aDirVect2[2] >> aTranslationVector[1];
  theStream >> aDirVect3[0] >> aDirVect3[1] >> aDirVect3[2] >> aTranslationVector[2];

  gp_Dir aDir1(aDirVect1[0], aDirVect1[1], aDirVect1[2]);
  gp_Dir aDir2(aDirVect2[0], aDirVect2[1], aDirVect2[2]);
  gp_Dir aDir3(aDirVect3[0], aDirVect3[1], aDirVect3[2]);
  gp_Ax3 anAxis(gp_Pnt(0, 0, 0), aDir3, aDir1);
  aDir3.Cross(aDir1);
  if (aDir3.Dot(aDir2) < 0.0)
  {
    anAxis.YReverse();
  }
  gp_Trsf aTransformation;
  aTransformation.SetTransformation(anAxis);
  aTransformation.SetTranslationPart(
    gp_Vec(aTranslationVector[0], aTranslationVector[1], aTranslationVector[2]));

  HLRAlgo_Projector         anAlgoProtector(aTransformation, aPerspective, aFocus);
  Handle(HLRTest_Projector) aTestProjector = new HLRTest_Projector(anAlgoProtector);
  return aTestProjector;
}

//=================================================================================================

void HLRTest_Projector::Whatis(Draw_Interpretor& I) const
{
  I << "projector";
}
