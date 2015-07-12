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

#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_WireVertex.hxx>
#include <ShapeExtend_WireData.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

//ied_modif_for_compil_Nov-19-1998
//=======================================================================
//function : ShapeAnalysis_WireVertex
//purpose  : 
//=======================================================================
ShapeAnalysis_WireVertex::ShapeAnalysis_WireVertex()
{
  myDone = Standard_False;
  myPreci = Precision::Confusion();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireVertex::Init(const TopoDS_Wire& wire, const Standard_Real preci)
{
  Init (new ShapeExtend_WireData (wire), preci);
}
  
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireVertex::Init(const Handle(ShapeExtend_WireData)& sbwd, const Standard_Real /*preci*/)
{
  Standard_Integer nb = sbwd->NbEdges();
  if (nb == 0) return;
  myDone = Standard_False;
  myWire = sbwd;
  myStat = new TColStd_HArray1OfInteger (1,nb);  myStat->Init(0);
  myPos  = new TColgp_HArray1OfXYZ (1,nb);
  myUPre = new TColStd_HArray1OfReal (1,nb);     myUPre->Init(0.0);
  myUFol = new TColStd_HArray1OfReal (1,nb);     myUFol->Init(0.0);
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireVertex::Load(const TopoDS_Wire& wire) 
{
  Init (wire, myPreci);
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireVertex::Load(const Handle(ShapeExtend_WireData)& sbwd) 
{
  Init (sbwd, myPreci);
}

//=======================================================================
//function : SetPrecision
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireVertex::SetPrecision(const Standard_Real preci) 
{
  myPreci = preci;
  myDone = Standard_False;
}

//=======================================================================
//function : Analyze
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireVertex::Analyze() 
{
  if (myStat.IsNull()) return;
  myDone = Standard_True;
  //  Analyse des vertex qui se suivent
  Handle(Geom_Curve) c1, c2;
  Standard_Real cf, cl, upre, ufol;
  Standard_Integer i, j, nb = myStat->Length(), stat;
  ShapeAnalysis_Edge EA;
  for (i = 1; i <= nb; i ++) {
    stat = -1;  // au depart

    j = (i == nb ? 1 : i+1);
    TopoDS_Edge   E1 = myWire->Edge (i);
    TopoDS_Edge   E2 = myWire->Edge (j);
    TopoDS_Vertex V1 = EA.LastVertex (myWire->Edge (i));
    TopoDS_Vertex V2 = EA.FirstVertex (myWire->Edge (j));
    gp_Pnt PV1 = BRep_Tool::Pnt (V1);
    gp_Pnt PV2 = BRep_Tool::Pnt (V2);
    Standard_Real tol1 = BRep_Tool::Tolerance (V1);
    Standard_Real tol2 = BRep_Tool::Tolerance (V2);
    EA.Curve3d (myWire->Edge (i),c1,cf,upre);
    EA.Curve3d (myWire->Edge (j),c2,ufol,cl);
    if (c1.IsNull() || c2.IsNull()) continue;  // on ne peut rien faire ...
    gp_Pnt P1 = c1->Value (upre);
    gp_Pnt P2 = c2->Value (ufol);

    //   Est-ce que le jeu de vertex convient ? (meme si V1 == V2, on verifie)
    Standard_Real d1 = PV1.Distance (P1);
    Standard_Real d2 = PV2.Distance (P2);
    Standard_Real dd = PV1.Distance (PV2);
    if (d1 <= tol1 && d2 <= tol2 && dd <= (tol1+tol2)) stat = 1;
    else if (d1 <= myPreci && d2 <= myPreci && dd <= myPreci) stat = 2;
    myStat->SetValue (i,-1);  // par defaut
    if (stat > 0)  {  if (V1 == V2) stat = 0;  }
    if (stat >= 0) {  myStat->SetValue (i,stat);  continue;  }
    //    Restent les autres cas !

    //    Une edge se termine sur l autre : il faudra simplement relimiter
    //    Projection calculee sur une demi-edge (pour eviter les pbs de couture)
    gp_Pnt PJ1,PJ2;
    Standard_Real U1,U2;
    Standard_Real dj1 = ShapeAnalysis_Curve().Project (c1,P2,myPreci,PJ1,U1,(cf+upre)/2,upre);
    Standard_Real dj2 = ShapeAnalysis_Curve().Project (c2,P1,myPreci,PJ2,U2,ufol,(ufol+cl)/2);
    if (dj1 <= myPreci)       {  SetStart (i,PJ1.XYZ(),U1);  continue;  }
    else if (dj2 <= myPreci)  {  SetEnd   (i,PJ2.XYZ(),U2);  continue;  }

    //    Restent a verifier les intersections et prolongations !
  }
}

//=======================================================================
//function : SetSameVertex
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireVertex::SetSameVertex(const Standard_Integer num) 
{
  myStat->SetValue (num,0);
}

//=======================================================================
//function : SetSameCoords
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireVertex::SetSameCoords(const Standard_Integer num) 
{
  myStat->SetValue (num,1);
}

//=======================================================================
//function : SetClose
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireVertex::SetClose(const Standard_Integer num) 
{
  myStat->SetValue (num,2);
}

//=======================================================================
//function : SetEnd
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireVertex::SetEnd(const Standard_Integer num,const gp_XYZ& pos,const Standard_Real ufol) 
{
  myStat->SetValue (num,3);
  myPos->SetValue  (num,pos);
  myUFol->SetValue (num,ufol);
}

//=======================================================================
//function : SetStart
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireVertex::SetStart(const Standard_Integer num,const gp_XYZ& pos,const Standard_Real upre) 
{
  myStat->SetValue (num,4);
  myPos->SetValue  (num,pos);
  myUFol->SetValue (num,upre);
}

//=======================================================================
//function : SetInters
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireVertex::SetInters(const Standard_Integer num, const gp_XYZ& pos,
					  const Standard_Real upre, const Standard_Real ufol) 
{
  myStat->SetValue (num,5);
  myPos->SetValue  (num,pos);
  myUPre->SetValue (num,upre);
  myUFol->SetValue (num,ufol);
}

//=======================================================================
//function : SetDisjoined
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireVertex::SetDisjoined(const Standard_Integer num) 
{
  myStat->SetValue (num,-1);
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeAnalysis_WireVertex::IsDone() const
{
  return myDone;
}

//=======================================================================
//function : Precision
//purpose  : 
//=======================================================================

 Standard_Real ShapeAnalysis_WireVertex::Precision() const
{
  return myPreci;
}

//=======================================================================
//function : NbEdges
//purpose  : 
//=======================================================================

 Standard_Integer ShapeAnalysis_WireVertex::NbEdges() const
{
  return myWire->NbEdges();
}

//=======================================================================
//function : WireData
//purpose  : 
//=======================================================================

const Handle(ShapeExtend_WireData)& ShapeAnalysis_WireVertex::WireData() const
{
  return myWire;
}

//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

 Standard_Integer ShapeAnalysis_WireVertex::Status(const Standard_Integer num) const
{
  return myStat->Value(num);
}

//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

 gp_XYZ ShapeAnalysis_WireVertex::Position(const Standard_Integer num) const
{
  return myPos->Value(num);
}

//=======================================================================
//function : UPrevious
//purpose  : 
//=======================================================================

//szv#4:S4163:12Mar99 was bug: returned Integer
 Standard_Real ShapeAnalysis_WireVertex::UPrevious(const Standard_Integer num) const
{
  return myUPre->Value(num);
}

//=======================================================================
//function : UFollowing
//purpose  : 
//=======================================================================

//szv#4:S4163:12Mar99 was bug: returned Integer
 Standard_Real ShapeAnalysis_WireVertex::UFollowing(const Standard_Integer num) const
{
  return myUFol->Value(num);
}

//=======================================================================
//function : Data
//purpose  : 
//=======================================================================

 Standard_Integer ShapeAnalysis_WireVertex::Data(const Standard_Integer num, gp_XYZ& pos,
						 Standard_Real& upre, Standard_Real& ufol) const
{
  pos  = myPos->Value(num);
  upre = myUPre->Value(num);
  ufol = myUFol->Value(num);
  return myStat->Value(num);
}

//=======================================================================
//function : NextStatus
//purpose  : 
//=======================================================================

 Standard_Integer ShapeAnalysis_WireVertex::NextStatus(const Standard_Integer stat,
						       const Standard_Integer num) const
{
  //szv#4:S4163:12Mar99 optimized
  if (!myStat.IsNull()) {
    Standard_Integer i,nb = myStat->Length();
    for (i = num+1; i <= nb; i ++) if (myStat->Value(i) == stat) return i;
  }
  return 0;
}

//=======================================================================
//function : NextCriter
//purpose  : 
//=======================================================================

 Standard_Integer ShapeAnalysis_WireVertex::NextCriter(const Standard_Integer crit,
						       const Standard_Integer num) const
{
  //szv#4:S4163:12Mar99 optimized
  if (!myStat.IsNull()) {
    Standard_Integer i,nb = myStat->Length();
    for (i = num+1; i <= nb; i ++) {
      Standard_Integer stat = myStat->Value(i);
      if ((crit == -1 && stat < 0) ||
	  (crit == 0 && stat == 0) ||
	  (crit == 1 && stat >  0) ||
	  (crit == 2 && (stat >= 0 && stat <= 2)) ||
	  (crit == 3 && (stat == 1 || stat == 2)) ||
	  (crit == 4 && stat >  2)) return i;
    }
  }
  return 0;
}
