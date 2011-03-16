// File:	QAYasaki.cxx
// Created:	Tue May 21 11:10:31 2002
// Author:	QA Admin
//		<qa@russox>


#include <QAYasaki.hxx>

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

void QAYasaki::Commands(Draw_Interpretor& theCommands) {
  char *group = "QAYasaki";

  theCommands.Add ("OCC230", "OCC230 TrimmedCurve Pnt2d Pnt2d", __FILE__, OCC230, group);
  theCommands.Add ("OCC142", "OCC142", __FILE__, OCC142, group);

  return;
}
