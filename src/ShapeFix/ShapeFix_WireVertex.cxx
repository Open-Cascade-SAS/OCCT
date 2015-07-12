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

//szv#4 S4163

#include <BRep_Builder.hxx>
#include <gp_Pnt.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_WireVertex.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_WireVertex.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_HArray1OfShape.hxx>

//ied_modif_for_compil_Nov-19-1998
//=======================================================================
//function : ShapeFix_WireVertex
//purpose  : 
//=======================================================================
ShapeFix_WireVertex::ShapeFix_WireVertex()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShapeFix_WireVertex::Init (const TopoDS_Wire& wire, 
				const Standard_Real preci) 
{
  Handle(ShapeExtend_WireData) sbwd = new ShapeExtend_WireData ( wire );
  Init ( sbwd, preci );
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShapeFix_WireVertex::Init (const Handle(ShapeExtend_WireData)& sbwd, 
				const Standard_Real preci) 
{
  myAnalyzer.Load ( sbwd );
  myAnalyzer.SetPrecision ( preci );
  myAnalyzer.Analyze();
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void ShapeFix_WireVertex::Init (const ShapeAnalysis_WireVertex& sawv) 
{
  myAnalyzer = sawv;
}

//=======================================================================
//function : Analyzer
//purpose  : 
//=======================================================================

const ShapeAnalysis_WireVertex& ShapeFix_WireVertex::Analyzer() const
{
  return myAnalyzer;
}

//=======================================================================
//function : WireData
//purpose  : 
//=======================================================================

const Handle(ShapeExtend_WireData)& ShapeFix_WireVertex::WireData() const
{
  return myAnalyzer.WireData();
}

//=======================================================================
//function : Wire
//purpose  : 
//=======================================================================

TopoDS_Wire ShapeFix_WireVertex::Wire() const
{
  return myAnalyzer.WireData()->Wire();
}

//=======================================================================
//function : FixSame
//purpose  : 
//=======================================================================

Standard_Integer ShapeFix_WireVertex::FixSame() 
{
  //  FixSame : prend les status "SameCoord" et "Close" et les force a "Same"
  //  reprendre l edge et forcer le vertex. Evt changer sa tolerance. Et voila
  if ( ! myAnalyzer.IsDone() ) return 0;
  
  Standard_Integer nbfix = 0;
  BRep_Builder B;

  Handle(ShapeExtend_WireData) sbwd = myAnalyzer.WireData();
  Standard_Integer i, nb = sbwd->NbEdges();

  for (i = 1; i <= nb; i ++) {
    Standard_Integer j = (i == nb ? 1 : i+1);
    Standard_Integer stat = myAnalyzer.Status(i);
    if (stat != 1 && stat != 2) continue;
    // Ici on prend un vertex et on le generalise aux deux edges
    TopoDS_Edge   E1 = sbwd->Edge (i);
    TopoDS_Edge   E2 = sbwd->Edge (j);

    ShapeAnalysis_Edge sae;
    TopoDS_Vertex V1 = sae.LastVertex  ( E1 );
    TopoDS_Vertex V2 = sae.FirstVertex ( E2 );
    if (V1 == V2)  {  
      myAnalyzer.SetSameVertex(i);  
      continue;  
    }    // deja fait ...
    if (stat == 2) {
      // OK mais en reprenant les tolerances
      Handle(Geom_Curve) crv;  
      Standard_Real cf,cl;
      sae.Curve3d ( sbwd->Edge(i), crv, cf, cl );
      B.UpdateVertex (V1,cl,E1,myAnalyzer.Precision());
      sae.Curve3d ( sbwd->Edge(j), crv, cf, cl );
      B.UpdateVertex (V1,cf,E2,myAnalyzer.Precision());
    }
    // Et remettre ce vtx en commun
    V1.Orientation (E2.Orientation());
    B.Add (E2,V1);
    V1.Orientation (E1.Orientation());  V1.Reverse();
    B.Add (E1,V1);
    myAnalyzer.SetSameVertex(i);    // conclusion
    nbfix ++;
  }
  return nbfix;
}

//=======================================================================
//function : Fix
//purpose  : 
//=======================================================================

Standard_Integer ShapeFix_WireVertex::Fix() 
{
  //  Ici le grand jeu : on repasse partout
  //  stat = 0 (OK) ou <0 (KO) : on passe
  //  stat = 1 ou 2 : on reprend le vtx d origine
  //  sinon on en refait un ...

  //  MAIS ATTENTION : on fait du neuf ... forcement. Donc nouvelles edges
  //   auxquelles on remet les Vertex (assez facile)
  //   Donc deux passes : 1 refaire les VTX  et 2 les remettre dans les edges
  if ( ! myAnalyzer.IsDone() ) return 0;

  Handle(ShapeExtend_WireData) sbwd = myAnalyzer.WireData();
  
  Standard_Integer i, nb = sbwd->NbEdges();
  Standard_Integer nbfix = 0;
  for (i = 1; i <= nb; i ++) {
    //    On note les valeurs
    //szv#4:S4163:12Mar99 optimized
    if (myAnalyzer.Status(i) > 0) nbfix ++;
  }
  if (nbfix == 0) return 0;

  BRep_Builder B;

  Handle(TopTools_HArray1OfShape) VI = new TopTools_HArray1OfShape (1,nb);
  Handle(TopTools_HArray1OfShape) VJ = new TopTools_HArray1OfShape (1,nb);
  Handle(TopTools_HArray1OfShape) EF = new TopTools_HArray1OfShape (1,nb);
  Handle(TColStd_HArray1OfReal)   UI = new TColStd_HArray1OfReal   (1,nb);
  Handle(TColStd_HArray1OfReal)   UJ = new TColStd_HArray1OfReal   (1,nb);

  for (i = 1; i <= nb; i ++) {
    //    On note les valeurs
    Standard_Integer j = (i == nb ? 1 : i+1);
    Standard_Integer stat = myAnalyzer.Status (i);

    ShapeAnalysis_Edge sae;
    TopoDS_Vertex V1 = sae.LastVertex ( sbwd->Edge(i) );
    TopoDS_Vertex V2 = sae.FirstVertex ( sbwd->Edge(j) );
    VI->SetValue (i,V1);  
    VJ->SetValue (j,V2);

    TopoDS_Edge E = sbwd->Edge(i);
//    E.EmptyCopy();   trop d ennuis
    EF->SetValue (i,E);

//    if (stat <= 0) continue;
//    TopoDS_Edge   E1 = STW.Edge (i);
//    TopoDS_Edge   E2 = STW.Edge (j);
    Standard_Real upre = myAnalyzer.UPrevious(i);
    Standard_Real ufol = myAnalyzer.UFollowing(j);

    Handle(Geom_Curve) crv;  
    Standard_Real cf,cl;
    //szv#4:S4163:12Mar99 optimized
    if (stat < 4) {
      sae.Curve3d ( sbwd->Edge(i), crv, cf, cl );
      upre = cl;
    }
    if (stat < 3 || stat == 4) {
      sae.Curve3d ( sbwd->Edge(j), crv, cf, cl );
      ufol = cf;
    }

    UI->SetValue (i,upre);  
    UJ->SetValue (j,ufol);
//    nbfix ++;
  }

  if (nbfix == 0) return nbfix;

  // EmptyCopy pas bon : KK sur les Range (dommage, le reste est bon)
  // Donc on garde l original mais on change les vertex
  // En effet, avant de "ajouter" des vertex, il faut enlever ceux d avant
  // Sinon on garde en double !

  for (i = 1; i <= nb; i ++) {
    TopoDS_Edge E1 = TopoDS::Edge (EF->Value (i));
    TopoDS_Vertex VA,VB;
    E1.Orientation (TopAbs_FORWARD);
    TopExp::Vertices (E1,VA,VB);
    E1.Free(Standard_True);
    B.Remove (E1,VA);
    B.Remove (E1,VB);
  }

  Standard_Real Prec = myAnalyzer.Precision();
  for (i = 1; i <= nb; i ++) {
    //    On y va pour de bon
    //    Changer les coords ?
    Standard_Integer j = (i == nb ? 1 : i+1);
    Standard_Integer stat = myAnalyzer.Status (i);
//    if (stat <= 0) continue;

    TopoDS_Vertex V1 = TopoDS::Vertex (VI->Value(i));
    TopoDS_Vertex V2 = TopoDS::Vertex (VJ->Value(j));
    TopoDS_Edge   E1 = TopoDS::Edge (EF->Value (i));
    TopoDS_Edge   E2 = TopoDS::Edge (EF->Value (j));
    Standard_Real upre = UI->Value(i);
    Standard_Real ufol = UJ->Value(j);

    if (stat > 2) 
      B.UpdateVertex (V1, gp_Pnt(myAnalyzer.Position(i)), Prec);

    //    ce qui suit : seulement si vertex a reprendre
    if (stat > 0) {
      B.UpdateVertex (V1,upre,E1,Prec);
      B.UpdateVertex (V1,ufol,E2,Prec);
      V1.Orientation (TopAbs_FORWARD);
//    V1.Orientation (E2.Orientation());
    }

    //    Comme on a deshabille les edges, il faut tout remettre
    E2.Free (Standard_True);  // sur place
    B.Add (E2,V1);
    V1.Orientation (TopAbs_REVERSED);
//    V1.Orientation (E1.Orientation());    V1.Reverse();
    E1.Free (Standard_True);  // sur place
    B.Add (E1,V1);

    myAnalyzer.SetSameVertex (i);    // conclusion
//    nbfix ++;
  }

//  pour finir, MAJ du STW
  for (i = 1; i <= nb; i ++) sbwd->Set (TopoDS::Edge(EF->Value(i)),i);

  return nbfix;
}
