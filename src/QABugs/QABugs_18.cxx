// Created on: 2002-03-19
// Created by: QA Admin
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#include <QABugs.hxx>

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <AIS_Shape.hxx>

#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <DDocStd.hxx>
#include <CDF.hxx>

#include <Resource_Manager.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <Geom_Curve.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Standard_Assert.hxx>

#define DEFAULT_COLOR    Quantity_NOC_GOLDENROD

static Standard_Integer OCC267 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 3) {
    di << "ERROR OCC267: Usage : " << argv[0] << " DOC path\n";
    return 1;
  }

  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(argv[1],D)) return 1;
  TCollection_ExtendedString path (argv[2]);
  Handle(TDocStd_Application) A = DDocStd::GetApplication();

  PCDM_StoreStatus theStatus = A->SaveAs(D,path);
  if (theStatus == PCDM_SS_OK ) {
    di << "OCC267 : PCDM_StoreStatus = PCDM_SS_OK\n";
  } else {
    di << "OCC267 : PCDM_StoreStatus = Bad_Store_Status\n";
  }

  return 0;
}

static Standard_Integer OCC181 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 5) {
    di << "ERROR OCC181: Usage : " << argv[0] << " FileName path1 path2 verbose=0/1\n";
    return 1;
  }
  Standard_CString aFileName = argv[1];
  Standard_CString aDir1 = argv[2];
  Standard_CString aDir2 = argv[3];
  Standard_Integer verboseInt = Draw::Atoi(argv[4]);

  Standard_Boolean verboseBool = Standard_False;
  if (verboseInt != 0) {
   verboseBool = Standard_True;
  }

  TCollection_AsciiString Env1, Env2, CSF_ = "set env(CSF_";
  Env1 = CSF_ + aFileName + "UserDefaults) " + aDir1;
  Env2 = CSF_ + aFileName + "UserDefaults) " + aDir2;

  di.Eval(Env1.ToCString());

  Resource_Manager aManager(aFileName, verboseBool);

  di.Eval(Env2.ToCString());

  Standard_Boolean aStatus = aManager.Save();

  if (aStatus) {
    di << "\nOCC181 : Status = TRUE\n";
  } else {
    di << "\nOCC181 : Status = FALSE\n";
  }

  return 0;
}

static Standard_Integer OCC27849 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 3) {
    di << "Usage : " << argv[0] << " <environment variable name> <resource name>\n";
    return 1;
  }
  Standard_CString aEnvName = argv[1];
  Standard_CString aResName = argv[2];

  Resource_Manager aManager (aEnvName);
  if (aManager.Find (aResName))
  {
    di << aManager.Value (aResName);
  }
  else
  {
    di << "Error: could not find resource " << aResName;
  }

  return 0;
}

static Standard_Real delta_percent (Standard_Real a, Standard_Real b)
{
  Standard_Real result;
  if (b != 0.) {
    result = fabs( (a - b) / b ) * 100.;
  } else if (a != 0.) {
    result = fabs( (a - b) / a ) * 100.;
  } else {
    result = 0.;
  }
  return result;
}

static Standard_Integer OCC367 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 7) {
    di << "ERROR : Usage : " << argv[0] << " shape step goodX goodY goodZ percent_tolerance\n";
    return 1;
  }

  TopoDS_Wire myTopoDSWire = TopoDS::Wire(DBRep::Get(argv[1]));
  Standard_Real l = Draw::Atof(argv[2]);
  Standard_Real goodX = Draw::Atof(argv[3]);
  Standard_Real goodY = Draw::Atof(argv[4]);
  Standard_Real goodZ = Draw::Atof(argv[5]);
  Standard_Real percent = Draw::Atof(argv[6]);
  Standard_Boolean aStatus = Standard_False;

  // Find the first vertex of the wire
  BRepTools_WireExplorer wire_exp(myTopoDSWire);
  TopoDS_Vertex vlast;
  {
    TopoDS_Vertex vw1, vw2;
    TopExp::Vertices(myTopoDSWire,vw1,vw2);
    TopoDS_Vertex ve1, ve2;
    TopoDS_Edge edge = TopoDS::Edge(wire_exp.Current());
    TopExp::Vertices(edge,ve1,ve2);
    if (vw1.IsSame(ve1) || vw1.IsSame(ve2))
      vlast = vw1;
    else {
      Standard_ASSERT_RAISE(vw2.IsSame(ve1) || vw2.IsSame(ve2), "Disconnected vertices");
      vlast = vw2;
    }
  }
  Standard_Integer EdgeIndex = 0;
  Standard_Real FirstEdgeX, FirstEdgeY, FirstEdgeZ, deltaX, deltaY, deltaZ;
  FirstEdgeX = FirstEdgeY = FirstEdgeZ = deltaX = deltaY = deltaZ = 0.;
  for ( ; wire_exp.More(); wire_exp.Next())
    {
      EdgeIndex++;
      di << "\n\n New Edge \n"   << "\n";
      Standard_Real newufirst, newulast;
      TopoDS_Edge edge = TopoDS::Edge(wire_exp.Current());
      Standard_Real ufirst, ulast;
      Handle(Geom_Curve) acurve;
      TopoDS_Vertex ve1, ve2;
      TopExp::Vertices(edge,ve1,ve2);
      if (ve1.IsSame(vlast))
	{
          acurve = BRep_Tool::Curve(edge, ufirst, ulast);
          newufirst = ufirst;
          newulast  = ulast;
          vlast = ve2;
	}
      else
	{
          Standard_ASSERT_RAISE(ve2.IsSame(vlast), "Not the same vertex");
          Standard_ASSERT_RAISE(wire_exp.Orientation() == TopAbs_REVERSED, "Wire should be REVERSED");
          acurve = BRep_Tool::Curve( edge, ufirst, ulast );
          newufirst = acurve->ReversedParameter( ufirst );
          newulast  = acurve->ReversedParameter( ulast );
          acurve = acurve->Reversed( );
          vlast = ve1;
	}
      
      GeomAdaptor_Curve   curve;
      GCPnts_UniformAbscissa  algo;
      curve.Load(acurve);
      algo.Initialize( curve, l, newufirst, newulast );
      if (!algo.IsDone())
        di << "Not Done!!!"   << "\n";
      Standard_Integer maxIndex =algo.NbPoints();
      for (Standard_Integer Index = 1; Index<=maxIndex;Index++) {
        Standard_Real t = algo.Parameter(Index);
        gp_Pnt      pt3 = curve.Value(t);
        di << "Parameter t = " << t   << "\n";
        di << "Value Pnt = " << pt3.X()<<" " <<pt3.Y()<<" " << pt3.Z()  << "\n";
	if (EdgeIndex == 1 && Index == maxIndex) {
	  FirstEdgeX = pt3.X();
	  FirstEdgeY = pt3.Y();
	  FirstEdgeZ = pt3.Z();
	  deltaX = delta_percent(FirstEdgeX, goodX);
	  deltaY = delta_percent(FirstEdgeY, goodY);
	  deltaZ = delta_percent(FirstEdgeZ, goodZ);
	  if (deltaX <= percent && deltaY <= percent && deltaZ <= percent) {
	    aStatus = Standard_True;
	  }
	}
      }
    }
  di << "\n\nFirstEdge = " << FirstEdgeX <<" " << FirstEdgeY <<" " << FirstEdgeZ << "\n";
  di << "deltaX = " << deltaX << " deltaY = " << deltaY << " deltaZ = " << deltaZ << "\n";
  if (aStatus) {
    di << argv[0] << " : OK\n";
  } else {
    di << argv[0] << " : ERROR\n";
  }

  return 0;
}

void QABugs::Commands_18(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  theCommands.Add("OCC267", "OCC267 DOC path", __FILE__, OCC267, group);
  theCommands.Add("OCC181", "OCC181 FileName path1 path2 verbose=0/1", __FILE__, OCC181, group);
  theCommands.Add("OCC27849", "OCC27849 <resource env name> <resource name>", __FILE__, OCC27849, group);
  theCommands.Add("OCC367", "OCC367 shape step goodX goodY goodZ percent_tolerance", __FILE__, OCC367, group);

  return;
}
