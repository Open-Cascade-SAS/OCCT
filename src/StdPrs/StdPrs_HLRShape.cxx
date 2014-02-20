// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <BRepAdaptor_Curve.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <StdPrs_HLRShape.hxx>
#include <StdPrs_HLRToolShape.hxx>
#include <StdPrs_DeflectionCurve.hxx>
#include <TopoDS_Shape.hxx>
#include <TColgp_SequenceOfPnt.hxx>

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void StdPrs_HLRShape::Add (const Handle(Prs3d_Presentation)& thePresentation,
                           const TopoDS_Shape&               theShape,
                           const Handle(Prs3d_Drawer)&       theDrawer,
                           const Handle(Prs3d_Projector)&    theProjector)
{
  StdPrs_HLRToolShape aTool(theShape, theProjector->Projector());
  Standard_Integer aNbEdges = aTool.NbEdges();
  Standard_Integer anI;
  Standard_Real anU1, anU2;
  BRepAdaptor_Curve aCurve;
  Standard_Real aDeviation = theDrawer->MaximalChordialDeviation();
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(thePresentation);

  aGroup->SetPrimitivesAspect(theDrawer->SeenLineAspect()->Aspect());
  
  Standard_Real anAngle = theDrawer->DeviationAngle();
  TColgp_SequenceOfPnt aPoints;
  for (anI = 1; anI <= aNbEdges; ++anI)
  {
    for(aTool.InitVisible(anI); aTool.MoreVisible(); aTool.NextVisible())
    {
      aTool.Visible(aCurve, anU1, anU2);
      StdPrs_DeflectionCurve::Add(thePresentation, aCurve,
                                  anU1, anU2, aDeviation, aPoints, anAngle);
    }
  }

  if(theDrawer->DrawHiddenLine())
  {
    aGroup->SetPrimitivesAspect(theDrawer->HiddenLineAspect()->Aspect());
    
    for (anI = 1; anI <= aNbEdges; ++anI)
    {
      for (aTool.InitHidden(anI); aTool.MoreHidden(); aTool.NextHidden())
      {
        aTool.Hidden(aCurve, anU1, anU2);
        StdPrs_DeflectionCurve::Add(thePresentation, aCurve,
                                    anU1, anU2, aDeviation, aPoints, anAngle);
      }
    }
  }
} 
