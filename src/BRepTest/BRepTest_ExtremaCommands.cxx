// Created on: 1995-09-08
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

// modified by mps (juillet 96) : ajout de la commande distmini

#include <DBRep.hxx>
#include <BRepTest.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepExtrema_Poly.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
//#ifdef WNT
#include <stdio.h>
//#endif

//=======================================================================
//function : distance
//purpose  : 
//=======================================================================

static Standard_Integer distance (Draw_Interpretor& di,
				  Standard_Integer n,
				  const char** a)
{
  if (n < 3) return 1;

  const char *name1 = a[1];
  const char *name2 = a[2];

  TopoDS_Shape S1 = DBRep::Get(name1);
  TopoDS_Shape S2 = DBRep::Get(name2);
  if (S1.IsNull() || S2.IsNull()) return 1;
  gp_Pnt P1,P2;
  Standard_Real D;
  if (!BRepExtrema_Poly::Distance(S1,S2,P1,P2,D)) return 1;
  //cout << " distance : " << D << endl;
  di << " distance : " << D << "\n";
  TopoDS_Edge E = BRepLib_MakeEdge(P1,P2);
  DBRep::Set("distance",E);
  return 0;
}
static Standard_Integer distmini(Draw_Interpretor& di, Standard_Integer n, const char** a)
{ 
  Standard_Integer i1;
//  gp_Pnt P;

  if (n != 4) return 1;

  const char *ns1 = (a[2]), *ns2 = (a[3]), *ns0 = (a[1]);
  TopoDS_Shape S1(DBRep::Get(ns1)), S2(DBRep::Get(ns2))  ;
  BRepExtrema_DistShapeShape dst(S1 ,S2 );


  if (dst.IsDone()) 
      { 
#ifdef DEB
         //dst.Dump(cout);
	Standard_SStream aSStream;
	dst.Dump(aSStream);
	di << aSStream;
#endif

	 char named[100];
	 Sprintf(named, "%s%s" ,ns0,"_val");
	 char* tempd = named;
	 Draw::Set(tempd,dst.Value());
         di << named << " ";

         for (i1=1; i1<= dst.NbSolution(); i1++)
	   {
             gp_Pnt P1,P2;
	     P1 = (dst.PointOnShape1(i1));
	     P2 = (dst.PointOnShape2(i1));
             if (dst.Value()<=1.e-9) 
             {
              TopoDS_Vertex V =BRepLib_MakeVertex(P1);
              char namev[100];
              if (i1==1) 
	      Sprintf(namev, "%s" ,ns0);
              else Sprintf(namev, "%s%d" ,ns0,i1);
	      char* tempv = namev;
	      DBRep::Set(tempv,V);
              di << namev << " ";
              }
             else
	     {char name[100];
              TopoDS_Edge E = BRepLib_MakeEdge (P1, P2);
	      if (i1==1)
              {Sprintf(name,"%s",ns0);}
              else {Sprintf(name,"%s%d",ns0,i1);}
	      char* temp = name;
	      DBRep::Set(temp,E);
	      di << name << " " ;
             }
           }
      }
  
  else di << "probleme"<< "\n";
  //else cout << "probleme"<< endl;
  return 0;
}

//=======================================================================
//function : ExtremaCommands
//purpose  : 
//=======================================================================

void  BRepTest::ExtremaCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  const char* g = "TOPOLOGY Extrema commands";

  theCommands.Add("dist","dist Shape1 Shape2"   ,__FILE__,distance,g);

  
  theCommands.Add("distmini",
                  "distmini name Shape1 Shape2",
		  __FILE__,
		  distmini,g);
}












