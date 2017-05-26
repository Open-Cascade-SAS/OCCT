// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <Draw.hxx>
#include <DrawTrSurf.hxx>
#include <Draw_Interpretor.hxx>
#include <GccAna_Circ2d3Tan.hxx>
#include <GccEnt.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeometryTest.hxx>
#include <GeometryTest_DrawableQualifiedCurve2d.hxx>
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>
#include <stdio.h>

//=======================================================================
//function : qcircle
//purpose  : Parses command: "qcircle name x y radius [-unqualified|-enclosing|-enclosed|-outside|-noqualifier]"
//=======================================================================
static Standard_Integer qcurve (Draw_Interpretor&, Standard_Integer theArgsNb, const char** theArgVec)
{
  if (theArgsNb < 5)
  {
    cout << "Error: wrong number of arguments.\n";
    return 1;
  }

  Handle(Geom2d_Curve)  aResult2d;
  TCollection_AsciiString aPositionType;
  if (!strcmp (theArgVec[0], "qcircle"))
  {
    if (theArgsNb == 5 || theArgsNb == 6)
      aResult2d = new Geom2d_Circle (gp_Ax22d (gp_Pnt2d (Draw::Atof (theArgVec[2]), Draw::Atof (theArgVec[3])),
                                     gp_Dir2d (1,0)), Draw::Atof (theArgVec[4]));
    else if (theArgsNb == 7 || theArgsNb == 8)
      aResult2d = new Geom2d_Circle (gp_Ax22d (gp_Pnt2d (Draw::Atof (theArgVec[2]), Draw::Atof (theArgVec[3])),
                  gp_Dir2d (Draw::Atof (theArgVec[4]), Draw::Atof (theArgVec[5]))), Draw::Atof (theArgVec[6]));

    if (theArgsNb == 6)
      aPositionType = theArgVec[5];
    else if (theArgsNb == 8)
      aPositionType = theArgVec[7];
  }
  else if(!strcmp (theArgVec[0], "qline"))
  {
    if (theArgsNb < 6)
    {
      cout << "Error: wrong number of arguments.\n";
      return 1;
    }
    aResult2d = new Geom2d_Line (gp_Pnt2d (Draw::Atof (theArgVec[2]), Draw::Atof (theArgVec[3])),
                                 gp_Dir2d (Draw::Atof (theArgVec[4]), Draw::Atof (theArgVec[5])));
    if (theArgsNb == 7)
      aPositionType = theArgVec[6];
  }
  else
  {
    cout << "Error: wrong command name.\n";
    return 1;
  }

  GccEnt_Position aKindOfPosition = GccEnt_unqualified;
  if (!aPositionType.IsEmpty())
  {
    GccEnt_Position aParameterPosition;
    if (GccEnt::PositionFromString (aPositionType.ToCString(), aParameterPosition))
      aKindOfPosition = aParameterPosition;
  }

  Draw::Set (theArgVec[1], new GeometryTest_DrawableQualifiedCurve2d (aResult2d, aKindOfPosition));
  return 0;
}

//=======================================================================
//function : solutions
//purpose  : 
//=======================================================================
static Standard_Integer solutions (Draw_Interpretor& theDI, GccAna_Circ2d3Tan& theCirTan3, const char* theName)
{
  if (!theCirTan3.IsDone())
  {
    cout << "GccAna_Circ2d3Tan is not done";
    return 1;
  }

  TCollection_AsciiString aName = TCollection_AsciiString (theName) + "_";
  GccEnt_Position aQualifier1, aQualifier2, aQualifier3;
  Standard_Real aParSol, aParArg;
  gp_Pnt2d aPntSol;
  for (Standard_Integer aSolId = 1; aSolId <= theCirTan3.NbSolutions(); aSolId++)
  {
    Handle(Geom2d_Circle) aCircle = new Geom2d_Circle (theCirTan3.ThisSolution (aSolId));
    TCollection_AsciiString aSolIdName = aName;
    aSolIdName += TCollection_AsciiString (aSolId);
    DrawTrSurf::Set (aSolIdName.ToCString(), aCircle);
    theCirTan3.WhichQualifier (aSolId, aQualifier1, aQualifier2, aQualifier3);
    theDI << "circle: " << aSolIdName.ToCString() << ", " << "qualifiers: " << GccEnt::PositionToString (aQualifier1)
          << ", " << GccEnt::PositionToString (aQualifier2) << ", " << GccEnt::PositionToString (aQualifier3) << "\n";

    theDI << "  tangent points: point (parameter on solution, parameter on argument)\n";
    // the first tangent point
    if (theCirTan3.IsTheSame1 (aSolId))
      theDI << "    " << "= the solution number " << aSolId << " is equal to the first argument\n";
    else
    {
      theCirTan3.Tangency1 (aSolId, aParSol, aParArg, aPntSol);
      TCollection_AsciiString aTanPntIdName = aSolIdName + "_tp_1";
      DrawTrSurf::Set (aTanPntIdName.ToCString(), aPntSol);
      theDI << "    " << aTanPntIdName.ToCString() << " (" << aParSol << ", " << aParArg << ")\n";
    }
    // the second tangent point
    if (theCirTan3.IsTheSame2 (aSolId))
      theDI << "    " << "= the solution number " << aSolId << " is equal to the second argument\n";
    else
    {
      theCirTan3.Tangency2 (aSolId, aParSol, aParArg, aPntSol);
      TCollection_AsciiString aTanPntIdName = aSolIdName + "_tp_2";
      DrawTrSurf::Set (aTanPntIdName.ToCString(), aPntSol);
      theDI << "    " << aTanPntIdName.ToCString() << " (" << aParSol << ", " << aParArg << ")\n";
    }
    // the third tangent point
    if (theCirTan3.IsTheSame3 (aSolId))
      theDI << "    " << "= the solution number " << aSolId << " is equal to the third argument\n";
    else
    {
      theCirTan3.Tangency3 (aSolId, aParSol, aParArg, aPntSol);
      TCollection_AsciiString aTanPntIdName = aSolIdName + "_tp_3";
      DrawTrSurf::Set (aTanPntIdName.ToCString(), aPntSol);
      theDI << "    " << aTanPntIdName.ToCString() << " (" << aParSol << ", " << aParArg << ")";
    }
    if (aSolId != theCirTan3.NbSolutions())
      theDI << "\n";
  }
  return 0;
}

//=======================================================================
//function : circ2d3Tan
//purpose  : Parses command: [circ2d3Tan cname qcicrle1/qlin1/point1 qcicrle2/qlin2/point2 qcicrle3/qlin3/point3
//                            tolerance]
//=======================================================================
static Standard_Integer circ2d3Tan (Draw_Interpretor& theDI, Standard_Integer theArgsNb, const char** theArgVec)
{
  if (theArgsNb < 5)
  {
    cout << "Error: wrong number of arguments.\n";
    return 1;
  }

  Handle(GeometryTest_DrawableQualifiedCurve2d) aQCurve1 =
    Handle(GeometryTest_DrawableQualifiedCurve2d)::DownCast (Draw::Get (theArgVec[2]));
  Handle(GeometryTest_DrawableQualifiedCurve2d) aQCurve2 =
    Handle(GeometryTest_DrawableQualifiedCurve2d)::DownCast (Draw::Get (theArgVec[3]));
  Handle(GeometryTest_DrawableQualifiedCurve2d) aQCurve3 =
    Handle(GeometryTest_DrawableQualifiedCurve2d)::DownCast (Draw::Get (theArgVec[4]));

  gp_Pnt2d aPoint1, aPoint2, aPoint3;
  Standard_Boolean anIsPoint1 = DrawTrSurf::GetPoint2d (theArgVec[2], aPoint1);
  Standard_Boolean anIsPoint2 = DrawTrSurf::GetPoint2d (theArgVec[3], aPoint2);
  Standard_Boolean anIsPoint3 = DrawTrSurf::GetPoint2d (theArgVec[4], aPoint3);

  Standard_Real aTolerance = Precision::Confusion();
  if (theArgsNb > 5)
    aTolerance = Draw::Atof (theArgVec[5]);

  if (aQCurve1.IsNull()) // <point, point, point>
  {
    if (!anIsPoint1 || !anIsPoint2 || !anIsPoint3)
    {
      cout << "Error: wrong points definition.\n";
      return 1;
    }
    GccAna_Circ2d3Tan aCircBuilder (aPoint1, aPoint2, aPoint3, aTolerance);
    return solutions (theDI, aCircBuilder, theArgVec[1]);
  }

  // the first curve is not NULL
  if (aQCurve2.IsNull()) // <qcircle, point, point> or <qlin, point, point>
  {
    if (!anIsPoint2 || !anIsPoint3)
    {
      cout << "Error: wrong points definition.\n";
      return 1;
    }
    Geom2dAdaptor_Curve anAdaptorCurve1 (aQCurve1->GetCurve());
    if (anAdaptorCurve1.GetType() == GeomAbs_Circle)
    {
      GccEnt_QualifiedCirc aQualifiedCircle1 (anAdaptorCurve1.Circle(), aQCurve1->GetPosition());
      GccAna_Circ2d3Tan aCircBuilder (aQualifiedCircle1, aPoint2, aPoint3, aTolerance);
      return solutions (theDI, aCircBuilder, theArgVec[1]);
    }
    else if (anAdaptorCurve1.GetType() == GeomAbs_Line)
    {
      GccEnt_QualifiedLin aQualifiedLin1 (anAdaptorCurve1.Line(), aQCurve1->GetPosition());
      GccAna_Circ2d3Tan aCircBuilder (aQualifiedLin1, aPoint2, aPoint3, aTolerance);
      return solutions (theDI, aCircBuilder, theArgVec[1]);
    }
    cout << "Error: wrong curve type.\n";
    return 1;
  }

  // the first and the second curves are not NULL
  if (aQCurve3.IsNull()) // <qcircle, qcircle, point> or <qcircle, qlin, point> or <qlin, qlin, point>
  {
    if (!anIsPoint3)
    {
      cout << "Error: wrong point definition.\n";
      return 1;
    }
    Geom2dAdaptor_Curve anAdaptorCurve1 (aQCurve1->GetCurve());
    Geom2dAdaptor_Curve anAdaptorCurve2 (aQCurve2->GetCurve());
    if (anAdaptorCurve1.GetType() == GeomAbs_Circle && anAdaptorCurve2.GetType() == GeomAbs_Circle)
    {
      GccEnt_QualifiedCirc aQualifiedCircle1 (anAdaptorCurve1.Circle(), aQCurve1->GetPosition());
      GccEnt_QualifiedCirc aQualifiedCircle2 (anAdaptorCurve2.Circle(), aQCurve2->GetPosition());
      GccAna_Circ2d3Tan aCircBuilder (aQualifiedCircle1, aQualifiedCircle2, aPoint3, aTolerance);
      return solutions (theDI, aCircBuilder, theArgVec[1]);
    }
    else if (anAdaptorCurve1.GetType() == GeomAbs_Circle && anAdaptorCurve2.GetType() == GeomAbs_Line)
    {
      GccEnt_QualifiedCirc aQualifiedCircle1 (anAdaptorCurve1.Circle(), aQCurve1->GetPosition());
      GccEnt_QualifiedLin aQualifiedLin2 (anAdaptorCurve2.Line(), aQCurve2->GetPosition());
      GccAna_Circ2d3Tan aCircBuilder (aQualifiedCircle1, aQualifiedLin2, aPoint3, aTolerance);
      return solutions (theDI, aCircBuilder, theArgVec[1]);
    }
    else if (anAdaptorCurve1.GetType() == GeomAbs_Line && anAdaptorCurve2.GetType() == GeomAbs_Line)
    {
      GccEnt_QualifiedLin aQualifiedLin1 (anAdaptorCurve1.Line(), aQCurve1->GetPosition());
      GccEnt_QualifiedLin aQualifiedLin2 (anAdaptorCurve2.Line(), aQCurve2->GetPosition());
      GccAna_Circ2d3Tan aCircBuilder (aQualifiedLin1, aQualifiedLin2, aPoint3, aTolerance);
      return solutions (theDI, aCircBuilder, theArgVec[1]);
    }
    cout << "Error: wrong curve type.\n";
    return 1;
  }

  // the first, the second and the third curves are not NULL
  // <qcircle, qcircle, qcircle> or <qcircle, qcircle, qlin>, <qcircle, qlin, qlin>, <qlin, qlin, qlin>
  Geom2dAdaptor_Curve anAdaptorCurve1 (aQCurve1->GetCurve());
  Geom2dAdaptor_Curve anAdaptorCurve2 (aQCurve2->GetCurve());
  Geom2dAdaptor_Curve anAdaptorCurve3 (aQCurve3->GetCurve());
  if (anAdaptorCurve1.GetType() == GeomAbs_Circle && anAdaptorCurve2.GetType() == GeomAbs_Circle &&
      anAdaptorCurve3.GetType() == GeomAbs_Circle)
  {
    GccEnt_QualifiedCirc aQualifiedCircle1 (anAdaptorCurve1.Circle(), aQCurve1->GetPosition());
    GccEnt_QualifiedCirc aQualifiedCircle2 (anAdaptorCurve2.Circle(), aQCurve2->GetPosition());
    GccEnt_QualifiedCirc aQualifiedCircle3 (anAdaptorCurve3.Circle(), aQCurve3->GetPosition());
    GccAna_Circ2d3Tan aCircBuilder (aQualifiedCircle1, aQualifiedCircle2, aQualifiedCircle3, aTolerance);
    return solutions (theDI, aCircBuilder, theArgVec[1]);
  }
  if (anAdaptorCurve1.GetType() == GeomAbs_Circle && anAdaptorCurve2.GetType() == GeomAbs_Circle &&
      anAdaptorCurve3.GetType() == GeomAbs_Line)
  {
    GccEnt_QualifiedCirc aQualifiedCircle1 (anAdaptorCurve1.Circle(), aQCurve1->GetPosition());
    GccEnt_QualifiedCirc aQualifiedCircle2 (anAdaptorCurve2.Circle(), aQCurve2->GetPosition());
    GccEnt_QualifiedLin aQualifiedLin3 (anAdaptorCurve3.Line(), aQCurve3->GetPosition());
    GccAna_Circ2d3Tan aCircBuilder (aQualifiedCircle1, aQualifiedCircle2, aQualifiedLin3, aTolerance);
    return solutions (theDI, aCircBuilder, theArgVec[1]);
  }
  if (anAdaptorCurve1.GetType() == GeomAbs_Circle && anAdaptorCurve2.GetType() == GeomAbs_Line &&
      anAdaptorCurve3.GetType() == GeomAbs_Line)
  {
    GccEnt_QualifiedCirc aQualifiedCircle1 (anAdaptorCurve1.Circle(), aQCurve1->GetPosition());
    GccEnt_QualifiedLin aQualifiedLin2 (anAdaptorCurve2.Line(), aQCurve2->GetPosition());
    GccEnt_QualifiedLin aQualifiedLin3 (anAdaptorCurve3.Line(), aQCurve3->GetPosition());
    GccAna_Circ2d3Tan aCircBuilder (aQualifiedCircle1, aQualifiedLin2, aQualifiedLin3, aTolerance);
    return solutions (theDI, aCircBuilder, theArgVec[1]);
  }
  if (anAdaptorCurve1.GetType() == GeomAbs_Line && anAdaptorCurve2.GetType() == GeomAbs_Line &&
      anAdaptorCurve3.GetType() == GeomAbs_Line)
  {
    GccEnt_QualifiedLin aQualifiedLin1 (anAdaptorCurve1.Line(), aQCurve1->GetPosition());
    GccEnt_QualifiedLin aQualifiedLin2 (anAdaptorCurve2.Line(), aQCurve2->GetPosition());
    GccEnt_QualifiedLin aQualifiedLin3 (anAdaptorCurve3.Line(), aQCurve3->GetPosition());
    GccAna_Circ2d3Tan aCircBuilder (aQualifiedLin1, aQualifiedLin2, aQualifiedLin3, aTolerance);
    return solutions (theDI, aCircBuilder, theArgVec[1]);
  }

  cout << "Error: wrong curve type.\n";
  return 1;
}

//=======================================================================
//function : CurveTanCommands
//purpose  : 
//=======================================================================
void  GeometryTest::CurveTanCommands (Draw_Interpretor& theCommands)
{
  static Standard_Boolean aLoaded = Standard_False;
  if (aLoaded) return;
  aLoaded = Standard_True;
  
  DrawTrSurf::BasicCommands (theCommands);
  
  const char* aGroup;
  aGroup = "GEOMETRY tangent curves creation";

  theCommands.Add ("qcircle",
            "qcircle name {x y [ux uy] radius} [-unqualified|-enclosing|-enclosed|-outside|-noqualifier]"
    "\n\t\t: Creates qualified circle.",
   __FILE__, qcurve, aGroup);

  theCommands.Add ("qline",
            "qline name x y dx dy [-unqualified|-enclosing|-enclosed|-outside|-noqualifier]"
    "\n\t\t: Creates qualified line.",
    __FILE__, qcurve, aGroup);

  theCommands.Add ("circ2d3Tan",
            "circ2d3Tan cname {qcicrle1|qlin1|point1} {qcicrle2|qlin2|point2} {qcicrle3|qlin3|point3} [tolerance]"
    "\n\t\t: Creates 2d circles tangent to 3 arguments. The arguments are points, lines or circles."
    "\n\t\t: Possible arguments combinations:"
    "\n\t\t:            <qcircle, qcircle, qcircle>,"
    "\n\t\t:            <qcircle, qcircle, qlin>,"
    "\n\t\t:            <qcircle, qcircle, point>,"
    "\n\t\t:            <qcircle, qlin, qlin>,"
    "\n\t\t:            <qcircle, qlin, point>,"
    "\n\t\t:            <qcircle, qlin, qlin>,"
    "\n\t\t:            <qcircle, point, point>,"
    "\n\t\t:            <qlin, qlin, qlin>,"
    "\n\t\t:            <qlin, qlin, point>,"
    "\n\t\t:            <qlin, point, point>,"
    "\n\t\t:            <point, point, point>.",
    __FILE__, circ2d3Tan, aGroup);
}
