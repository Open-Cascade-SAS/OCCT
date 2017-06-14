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
#include <BRepMesh_IncrementalMesh.hxx>
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
    if (argc != 2) {
      di << "Usage : " << argv[0] << "OCC527 shape\n";
      return 1;
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

        //
        di << "BRepAlgoAPI_Section aSection(aFace,pl,Standard_False)\n";
        BRepAlgoAPI_Section aSection(aFace, pl, Standard_False);
        aSection.Approximation(Standard_True);
        aSection.Build();
        Standard_Boolean IsDone = aSection.IsDone();

        if (IsDone)
        {
          const TopoDS_Shape& aResult = aSection.Shape();
          if (!aResult.IsNull())
          {
            double lmaxdist = 0.0;
            TopExp_Explorer aExp2;
            for (aExp2.Init(aResult, TopAbs_VERTEX); aExp2.More(); aExp2.Next())
            {
              TopoDS_Vertex aV = TopoDS::Vertex(aExp2.Current());
              Standard_Real  toler = BRep_Tool::Tolerance(aV);
              double dist = pl.Distance(BRep_Tool::Pnt(aV));
              if (dist > lmaxdist) lmaxdist = dist;
              // If section was built check distance beetwen vertexes and plane of the one
              str[0] = 0;
              if (dist > toler)
                Sprintf(str, "Dist=%f, Toler=%f, Param=%f FAULTY\n", dist, toler, gzmax);
              else
                Sprintf(str, "Dist=%f, Toler=%f, Param=%f\n", dist, toler, gzmax);
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

void QABugs::Commands_2(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  theCommands.Add("OCC527", "OCC527 shape", __FILE__, OCC527, group);
  return;
}
