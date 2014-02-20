// Created on: 2002-07-18
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
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <BRepBndLib.hxx>
#include <gp_Pln.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepAlgo_Section.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>

#include <stdio.h>

//=======================================================================
//function : OCC527 
//purpose  : 
//=======================================================================
static Standard_Integer OCC527(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  try
  {
    OCC_CATCH_SIGNALS
    // 1. Verify amount of arguments of the command
    //if (argc < 2) { di << "OCC527 FAULTY. Use : OCC527 shape "; return 0;} 
    if (argc < 2 || argc > 3) {
      di << "Usage : " << argv[0] << " shape [BRepAlgoAPI/BRepAlgo = 1/0]" << "\n";
      return 1;
    }
    Standard_Boolean IsBRepAlgoAPI = Standard_True;
    if (argc == 3) {
      Standard_Integer IsB = Draw::Atoi(argv[2]);
      if (IsB != 1) {
	IsBRepAlgoAPI = Standard_False;
//#if ! defined(BRepAlgo_def04)
//	di << "Error: There is not BRepAlgo_Section class" << "\n";
//	return 1;
//#endif
      }
    }
    
    // 2. Get selected shape
    TopoDS_Shape aShape = DBRep::Get(argv[1]);
    if(aShape.IsNull()) { di << "OCC527 FAULTY. Entry shape is NULL"; return 0;} 

    // 3. Explode entry shape on faces and build sections from Zmin to Zmax with step aStep
    const Standard_Real Zmin = -40.228173882121, Zmax = 96.408126285268, aStep = 1.0;
    char str[100]; str[0] = 0; Sprintf(str,"Test range: [%f, %f] with step %f\n",Zmin,Zmax,aStep); di << str;
    int nbf = 0;
    TopExp_Explorer aExp1;
    for (aExp1.Init(aShape,TopAbs_FACE); aExp1.More(); aExp1.Next())
    {
      // Process one face
      str[0] = 0; Sprintf(str,"Face #%d: \t",nbf++); di << str;
      TopoDS_Face aFace = TopoDS::Face(aExp1.Current());
      
      // Build BndBox in order to avoid try of building section 
      // if plane of the one does not intersect BndBox of the face
      Bnd_Box aFaceBox;
      BRepBndLib::Add(aFace,aFaceBox);
      Standard_Real X1,X2,Y1,Y2,Z1,Z2;
      aFaceBox.Get(X1,Y1,Z1,X2,Y2,Z2);

      // Build sections from Zmin to Zmax with step aStep
      double gmaxdist = 0.0, gzmax = Zmax;
      for (double zcur = Zmax; zcur > Zmin; zcur -= aStep)
      {
        // If plane of the section does not intersect BndBox of the face do nothing
        if(zcur < Z1 || zcur > Z2 ) continue;
                
        // Build current section
        gp_Pln pl(0,0,1,-zcur);
//#if ! defined(BRepAlgoAPI_def01)
//        BRepAlgoAPI_Section aSection(aFace,pl,Standard_False);
//#else
//        BRepAlgo_Section aSection(aFace,pl,Standard_False);
//#endif
//        aSection.Approximation(Standard_True);
//        aSection.Build();
//        // If section was built meassure distance between vertexes and plane of the one. Max distance is stored.
//        if (aSection.IsDone())
	
	Standard_Boolean IsDone;
	TopoDS_Shape aResult;
	if (IsBRepAlgoAPI) {
	  di << "BRepAlgoAPI_Section aSection(aFace,pl,Standard_False)" <<"\n";
	  BRepAlgoAPI_Section aSection(aFace,pl,Standard_False);
	  aSection.Approximation(Standard_True);
	  aSection.Build();
	  IsDone = aSection.IsDone();
	  aResult = aSection.Shape();
	} else {
	  di << "BRepAlgo_Section aSection(aFace,pl,Standard_False)" <<"\n";
	  BRepAlgo_Section aSection(aFace,pl,Standard_False);
	  aSection.Approximation(Standard_True);
	  aSection.Build();
	  IsDone = aSection.IsDone();
	  aResult = aSection.Shape();
	}

        if (IsDone)
	  {
//	    TopoDS_Shape aResult = aSection.Shape();
	    if (!aResult.IsNull())
	      {
            double lmaxdist = 0.0;
            TopExp_Explorer aExp2;
            for (aExp2.Init(aResult,TopAbs_VERTEX); aExp2.More(); aExp2.Next())
            {
              TopoDS_Vertex aV = TopoDS::Vertex(aExp2.Current());
              Standard_Real  toler = BRep_Tool::Tolerance(aV);
              double dist = pl.Distance(BRep_Tool::Pnt(aV));
              if (dist > lmaxdist) lmaxdist = dist;
      // If section was built check distance beetwen vertexes and plane of the one
      str[0] =0;
//       if (wasBuilt) 
//       {
//         if(gmaxdist > Precision::Confusion())
//           Sprintf(str,"Dist=%f, Param=%f FAULTY\n",gmaxdist,gzmax);
//         else
//           Sprintf(str,"Dist=%f, Param=%f\n",gmaxdist,gzmax);
        if(dist > toler)
          Sprintf(str,"Dist=%f, Toler=%f, Param=%f FAULTY\n",dist,toler,gzmax);
        else
          Sprintf(str,"Dist=%f, Toler=%f, Param=%f\n",dist,toler,gzmax);
//       }
//       else Sprintf(str,"No result\n");
      di << str;
            }
            if (lmaxdist > gmaxdist)
            {
              gmaxdist = lmaxdist;
              gzmax = zcur;
            }
          }
        }
      }
    }
  }
  catch (Standard_Failure) {di << "OCC527 Exception \n" ;return 0;}
  
  return 0;
}

#include <StlMesh_Mesh.hxx>
#include <StlTransfer.hxx>
//=======================================================================
//function : OCC1048
//purpose  : 
//=======================================================================
static Standard_Integer OCC1048 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  // Verify amount of arguments of the command
  if (argc < 2) { di << "Usage : " << argv[0] <<" shape"; return 1;}

  TopoDS_Shape aShape = DBRep::Get(argv[1]);

  Standard_Real theDeflection = 0.006;
  Handle(StlMesh_Mesh) theStlMesh = new StlMesh_Mesh;
  StlTransfer::BuildIncrementalMesh(aShape, theDeflection, Standard_False, theStlMesh);

  Standard_Integer NBTRIANGLES = theStlMesh->NbTriangles();
  di<<"Info: Number of triangles = "<<NBTRIANGLES<<"\n";

  return 0;
}

void QABugs::Commands_2(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  //theCommands.Add("OCC527", "OCC527 shape", __FILE__, OCC527, group);  
  theCommands.Add("OCC527", "OCC527 shape [BRepAlgoAPI/BRepAlgo = 1/0]", __FILE__, OCC527, group);  
  theCommands.Add("OCC1048", "OCC1048 shape", __FILE__, OCC1048, group);  
  return;
}
