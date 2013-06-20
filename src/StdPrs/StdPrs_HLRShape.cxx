// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
