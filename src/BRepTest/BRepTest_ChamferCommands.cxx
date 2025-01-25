// Created on: 1995-09-25
// Created by: Stagiaire Flore Lautheanne
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

#include <Draw_Interpretor.hxx>
#include <BRepTest.hxx>
#include <Draw.hxx>
#include <DBRep.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>

#include <Precision.hxx>

//===============================================================================
// function : chamf_throat_with_penetration
// purpose  : command to construct chamfers with constant throat with penetration
//            on several edges
//            Here the chamfer is propagated on tangential edges to the
//            required edge
//===============================================================================

static Standard_Integer chamf_throat_with_penetration(Draw_Interpretor& di,
                                                      Standard_Integer  narg,
                                                      const char**      a)
{
  if (narg < 7)
    return 1;

  TopoDS_Shape S = DBRep::Get(a[2]);
  if (S.IsNull())
    return 1;

  TopoDS_Edge      E;
  TopoDS_Face      F;
  Standard_Real    offset, throat;
  Standard_Integer i     = 3;
  Standard_Integer NbArg = 4;

  BRepFilletAPI_MakeChamfer aMCh(S);
  aMCh.SetMode(ChFiDS_ConstThroatWithPenetrationChamfer);

  while (i + NbArg <= narg)
  {
    TopoDS_Shape aLocalEdge(DBRep::Get(a[i], TopAbs_EDGE));
    E = TopoDS::Edge(aLocalEdge);
    TopoDS_Shape aLocalFace(DBRep::Get(a[i + 1], TopAbs_FACE));
    F = TopoDS::Face(aLocalFace);
    //      E = TopoDS::Edge(DBRep::Get(a[i], TopAbs_EDGE));
    //      F = TopoDS::Face(DBRep::Get(a[i + 1], TopAbs_FACE));
    if (!E.IsNull() && !F.IsNull() && (aMCh.Contour(E) == 0))
    {
      offset = Draw::Atof(a[i + 2]);
      throat = Draw::Atof(a[i + 3]);

      if (offset > Precision::Confusion() && throat > offset)
        aMCh.Add(offset, throat, E, F);
    }
    i += NbArg;
  }

  // compute the chamfer and display the result
  if (aMCh.NbContours() == 0)
  {
    // std::cout<<"No suitable edges to chamfer"<<std::endl;
    di << "No suitable edges to chamfer\n";
    return 1;
  }
  else
    aMCh.Build();

  if (aMCh.IsDone())
  {
    DBRep::Set(a[1], aMCh);
    return 0;
  }
  else
  {
    // std::cout<<"compute of chamfer failed"<<std::endl;
    di << "compute of chamfer failed\n";
    return 1;
  }
}

//===============================================================================
// function : chamf_throat
// purpose  : command to construct chamfers with constant throat on several edges
//            Here the chamfer is propagated on tangential edges to the
//            required edge
//===============================================================================

static Standard_Integer chamf_throat(Draw_Interpretor& di, Standard_Integer narg, const char** a)
{
  if (narg < 5)
    return 1;

  TopoDS_Shape S = DBRep::Get(a[2]);
  if (S.IsNull())
    return 1;

  TopoDS_Edge      E;
  Standard_Real    throat;
  Standard_Integer i = 3;

  BRepFilletAPI_MakeChamfer aMCh(S);
  aMCh.SetMode(ChFiDS_ConstThroatChamfer);

  while (i + 1 < narg)
  {
    TopoDS_Shape aLocalEdge(DBRep::Get(a[i], TopAbs_EDGE));
    E = TopoDS::Edge(aLocalEdge);
    if (!E.IsNull() && (aMCh.Contour(E) == 0))
    {
      throat = Draw::Atof(a[i + 1]);

      if (throat > Precision::Confusion())
        aMCh.Add(throat, E);
    }
    i += 2;
  }

  // compute the chamfer and display the result
  if (aMCh.NbContours() == 0)
  {
    // std::cout<<"No suitable edges to chamfer"<<std::endl;
    di << "No suitable edges to chamfer\n";
    return 1;
  }
  else
    aMCh.Build();

  if (aMCh.IsDone())
  {
    DBRep::Set(a[1], aMCh);
    return 0;
  }
  else
  {
    // std::cout<<"compute of chamfer failed"<<std::endl;
    di << "compute of chamfer failed\n";
    return 1;
  }
}

//=========================================================================
// function : chamfer
// purpose  : command to construct chamfers on several edges
//            Here the chamfer is propagated on tangential edges to the
//            required edge
//=========================================================================

static Standard_Integer chamfer(Draw_Interpretor& di, Standard_Integer narg, const char** a)
{
  // check the argument number of the command
  if (narg == 1)
  {
    di << " help for chamf : \n";
    di
      << "   Construction by equal distances from edge          :  chamf newname shape edge dist\n";
    di << "   Construction by two distances from edge            :  chamf newname shape edge face "
          "dist1 dist2\n";
    di << "   Construction by distance from edge and given angle :  chamf newname shape edge face "
          "A dist angle\n";
  }
  else
  {
    if (narg < 5)
      return 1;

    TopoDS_Shape S = DBRep::Get(a[2]);
    if (S.IsNull())
      return 1;

    TopoDS_Edge      E;
    TopoDS_Face      F;
    Standard_Real    d1, d2, angle;
    Standard_Integer i = 3;

    BRepFilletAPI_MakeChamfer aMCh(S);

    while (i + 1 < narg)
    {
      TopoDS_Shape aLocalEdge(DBRep::Get(a[i], TopAbs_EDGE));
      if (aLocalEdge.IsNull())
        return 1;
      E = TopoDS::Edge(aLocalEdge);
      TopoDS_Shape aLocalFace(DBRep::Get(a[i + 1], TopAbs_FACE));
      if (aLocalFace.IsNull())
      {
        // symmetric chamfer (one distance)
        d1 = atof(a[i + 1]);
        if (aMCh.Contour(E) == 0 && d1 > Precision::Confusion())
          aMCh.Add(d1, E);
        i += 2;
      }
      else
      {
        F = TopoDS::Face(aLocalFace);

        if (i + 3 < narg)
        {
          if (!strcasecmp(a[i + 2], "A") && i + 4 < narg)
          {
            // chamfer with distance and angle
            d1    = Draw::Atof(a[i + 3]);
            angle = Draw::Atof(a[i + 4]);
            angle *= M_PI / 180.;
            if (aMCh.Contour(E) == 0 && d1 > Precision::Confusion()
                && angle > Precision::Confusion() && M_PI / 2 - angle > Precision::Confusion())
              aMCh.AddDA(d1, angle, E, F);
            i += 5;
          }
          else
          {
            // chamfer with two distances
            d1 = Draw::Atof(a[i + 2]);
            d2 = Draw::Atof(a[i + 3]);
            if (aMCh.Contour(E) == 0 && d1 > Precision::Confusion() && d2 > Precision::Confusion())
              aMCh.Add(d1, d2, E, F);
            i += 4;
          }
        }
      }
    }

    // compute the chamfer and display the result
    if (aMCh.NbContours() == 0)
    {
      // std::cout<<"No suitable edges to chamfer"<<std::endl;
      di << "No suitable edges to chamfer\n";
      return 1;
    }
    else
      aMCh.Build();

    if (aMCh.IsDone())
    {
      DBRep::Set(a[1], aMCh);
      return 0;
    }
    else
    {
      // std::cout<<"compute of chamfer failed"<<std::endl;
      di << "compute of chamfer failed\n";
      return 1;
    }
  }

  return 0;
}

//=================================================================================================

void BRepTest::ChamferCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done)
    return;
  done = Standard_True;

  DBRep::BasicCommands(theCommands);

  const char* g = "TOPOLOGY Fillet construction commands";

  //  theCommands.Add("chamf",
  //		  "chamf newname shape edge1 face1 dist1_1 dist1_2 edge2 face2 dist2_1 dist2_2 ...
  //",__FILE__,chamfer,g);

  theCommands.Add("chamf", "for help call chamf without arguments", __FILE__, chamfer, g);

  theCommands.Add("chamf_throat",
                  "chamf_throat result shape edge throat",
                  __FILE__,
                  chamf_throat,
                  g);

  theCommands.Add("chamf_throat_with_penetration",
                  "chamf_throat_with_penetration result shape edge face offset throat",
                  __FILE__,
                  chamf_throat_with_penetration,
                  g);
}
