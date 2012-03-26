// Created on: 2002-05-21
// Created by: QA Admin
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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



#include <QABugs.hxx>

#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <gp_Pnt2d.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <DrawTrSurf.hxx>

#include <PCollection_HAsciiString.hxx>

//static Standard_Integer OCC230 (Draw_Interpretor& /*di*/, Standard_Integer /*argc*/, const char ** /*argv*/)
static Standard_Integer OCC230 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 4) {
    di << "ERROR OCC230: Usage : " << argv[0] << " TrimmedCurve Pnt2d Pnt2d" << "\n";
    return 1;
  }

  gp_Pnt2d P1, P2;
  if ( !DrawTrSurf::GetPoint2d(argv[2],P1)) {
    di << "ERROR OCC230: " << argv[2] << " is not Pnt2d" << "\n";
    return 1;
  }
  if ( !DrawTrSurf::GetPoint2d(argv[3],P2)) {
    di << "ERROR OCC230: " << argv[3] << " is not Pnt2d" << "\n";
    return 1;
  }

  GCE2d_MakeSegment MakeSegment(P1,P2);
  Handle(Geom2d_TrimmedCurve) TrimmedCurve = MakeSegment.Value();
  DrawTrSurf::Set(argv[1], TrimmedCurve);
  return 0;
}

static Standard_Integer OCC142 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** /*argv*/)
{
  for(Standard_Integer i= 0;i <= 20;i++){
    Handle(PCollection_HAsciiString) pstr = new PCollection_HAsciiString("TEST");
    pstr->Clear();
  }
  di << "OCC142: OK" << "\n";
  return 0;
}

void QABugs::Commands_19(Draw_Interpretor& theCommands) {
  char *group = "QABugs";

  theCommands.Add ("OCC230", "OCC230 TrimmedCurve Pnt2d Pnt2d", __FILE__, OCC230, group);
  theCommands.Add ("OCC142", "OCC142", __FILE__, OCC142, group);

  return;
}
