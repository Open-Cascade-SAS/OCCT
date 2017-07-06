// Created on: 1997-04-17
// Created by: Christophe MARION
// Copyright (c) 1997-1999 Matra Datavision
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


#include <HLRAlgo.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_Data.hxx>
#include <HLRBRep_Hider.hxx>
#include <HLRBRep_InternalAlgo.hxx>
#include <HLRBRep_ShapeBounds.hxx>
#include <HLRBRep_ShapeToHLR.hxx>
#include <HLRTopoBRep_OutLiner.hxx>
#include <Standard_Transient.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(HLRBRep_InternalAlgo,Standard_Transient)

extern Standard_Integer nbPtIntersection;   // total P.I.
extern Standard_Integer nbSegIntersection;  // total S.I
extern Standard_Integer nbClassification;   // total classification
extern Standard_Integer nbOkIntersection;   // pairs of intersecting edges
extern Standard_Integer nbCal1Intersection; // pairs of unrejected edges
extern Standard_Integer nbCal2Intersection; // true intersections (not vertex)
extern Standard_Integer nbCal3Intersection; // curve-surface intersections

static Standard_Integer HLRBRep_InternalAlgo_TRACE = Standard_True;
static Standard_Integer HLRBRep_InternalAlgo_TRACE10 = Standard_True; 

//=======================================================================
//function : HLRBRep_InternalAlgo
//purpose  : 
//=======================================================================

HLRBRep_InternalAlgo::HLRBRep_InternalAlgo () :
myDebug       (Standard_False)
{
}

//=======================================================================
//function : HLRBRep_InternalAlgo
//purpose  : 
//=======================================================================

HLRBRep_InternalAlgo::
HLRBRep_InternalAlgo (const Handle(HLRBRep_InternalAlgo)& A)
{
  myDS          = A->DataStructure();
  myProj        = A->Projector();
  myShapes      = A->SeqOfShapeBounds();
  myDebug       = A->Debug();
}

//=======================================================================
//function : Projector
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::Projector (const HLRAlgo_Projector& P)
{
  myProj = P;
}

//=======================================================================
//function : Projector
//purpose  : 
//=======================================================================

HLRAlgo_Projector & HLRBRep_InternalAlgo::Projector ()
{ return myProj; }

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::Update ()
{
  if (!myShapes.IsEmpty()) {
    Standard_Integer n = myShapes.Length();
    Handle(HLRBRep_Data) *DS = new Handle(HLRBRep_Data) [n];

    Standard_Integer i,dv,de,df,nv=0,ne=0,nf=0;

    for (i = 1; i <= n; i++) {
      HLRBRep_ShapeBounds& SB = myShapes(i);
      try {
        OCC_CATCH_SIGNALS
	DS[i-1] = HLRBRep_ShapeToHLR::Load(SB.Shape(),
					   myProj,
					   myMapOfShapeTool,
					   SB.NbOfIso());
	dv = DS[i-1]->NbVertices();
	de = DS[i-1]->NbEdges   ();
	df = DS[i-1]->NbFaces   ();
      }
      catch(Standard_Failure const& anException) {
        if (myDebug)
        {
          cout << "An exception was catched when preparing the Shape " << i;
          cout << " and computing its OutLines " << endl;
          cout << anException << endl;
        }
	DS[i-1] = new HLRBRep_Data(0,0,0);
	dv = 0;
	de = 0;
	df = 0;
      }

      SB = HLRBRep_ShapeBounds
	(SB.Shape(),SB.ShapeData(),SB.NbOfIso(),1,dv,1,de,1,df);
      nv += dv;
      ne += de;
      nf += df;
    }

    if (n == 1) myDS = DS[0];
    else {
      myDS = new HLRBRep_Data(nv,ne,nf);
      nv = 0;
      ne = 0;
      nf = 0;

      for (i = 1; i <= n; i++) {
	HLRBRep_ShapeBounds& SB = myShapes(i);
	SB.Sizes(dv,de,df);
	SB.Translate(nv,ne,nf);
	myDS->Write(DS[i-1],nv,ne,nf);
	nv += dv;
	ne += de;
	nf += df;
      }
    }

    delete [] DS;

    myDS->Update(myProj);

    HLRAlgo_EdgesBlock::MinMaxIndices ShapMin, ShapMax, MinMaxShap;
    HLRAlgo_EdgesBlock::MinMaxIndices TheMin, TheMax;
    HLRBRep_Array1OfEData& aEDataArray = myDS->EDataArray();
    HLRBRep_Array1OfFData& aFDataArray = myDS->FDataArray();

    for (i = 1; i <= n; i++) {
      Standard_Boolean FirstTime = Standard_True;
      HLRBRep_ShapeBounds& SB = myShapes(i);
      Standard_Integer v1,v2,e1,e2,f1,f2;
      SB.Bounds(v1,v2,e1,e2,f1,f2);

      for (Standard_Integer e = e1; e <= e2; e++) {
        HLRBRep_EdgeData ed = aEDataArray.ChangeValue(e);
        HLRAlgo::DecodeMinMax(ed.MinMax(), TheMin, TheMax);
	if (FirstTime) {
	  FirstTime = Standard_False;
	  HLRAlgo::CopyMinMax(TheMin, TheMax, ShapMin, ShapMax);
	}
	else
	  HLRAlgo::AddMinMax(TheMin, TheMax, ShapMin, ShapMax);
      }

      for (Standard_Integer f = f1; f <= f2; f++) {
        HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
        HLRAlgo::DecodeMinMax(fd.Wires()->MinMax(), TheMin, TheMax);
	HLRAlgo::AddMinMax(TheMin, TheMax, ShapMin, ShapMax);
      }
      HLRAlgo::EncodeMinMax(ShapMin, ShapMax, MinMaxShap);
      SB.UpdateMinMax(MinMaxShap);
    }
  }
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::Load (const Handle(HLRTopoBRep_OutLiner)& S,
				 const Handle(Standard_Transient)& SData, 
				 const Standard_Integer nbIso)
{ 
  myShapes.Append(HLRBRep_ShapeBounds(S,SData,nbIso,0,0,0,0,0,0));
  myDS.Nullify();
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::Load (const Handle(HLRTopoBRep_OutLiner)& S,
				 const Standard_Integer nbIso)
{ 
  myShapes.Append(HLRBRep_ShapeBounds(S,nbIso,0,0,0,0,0,0));
  myDS.Nullify();
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer HLRBRep_InternalAlgo::
Index (const Handle(HLRTopoBRep_OutLiner)& S) const
{
  Standard_Integer n = myShapes.Length();

  for (Standard_Integer i = 1; i <= n; i++)
    if (myShapes(i).Shape() == S) return i;

  return 0;
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::Remove (const Standard_Integer I)
{
  Standard_OutOfRange_Raise_if
    (I == 0 || I > myShapes.Length(),
     "HLRBRep_InternalAlgo::Remove : unknown Shape");
  myShapes.Remove(I);
  
  myMapOfShapeTool.Clear();
  myDS.Nullify();
}

//=======================================================================
//function : ShapeData
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::ShapeData (const Standard_Integer I,
				      const Handle(Standard_Transient)& SData)
{
  Standard_OutOfRange_Raise_if
    (I == 0 || I > myShapes.Length(),
     "HLRBRep_InternalAlgo::ShapeData : unknown Shape");

  myShapes(I).ShapeData(SData);
}

//=======================================================================
//function : SeqOfShapeBounds
//purpose  : 
//=======================================================================

HLRBRep_SeqOfShapeBounds & HLRBRep_InternalAlgo::SeqOfShapeBounds ()
{
  return myShapes;
}

//=======================================================================
//function : NbShapes
//purpose  : 
//=======================================================================

Standard_Integer HLRBRep_InternalAlgo::NbShapes () const
{ return myShapes.Length(); }

//=======================================================================
//function : ShapeBounds
//purpose  : 
//=======================================================================

HLRBRep_ShapeBounds & HLRBRep_InternalAlgo::
ShapeBounds (const Standard_Integer I)
{
  Standard_OutOfRange_Raise_if
    (I == 0 || I > myShapes.Length(),
     "HLRBRep_InternalAlgo::ShapeBounds : unknown Shape");

  return myShapes(I);
}

//=======================================================================
//function : InitEdgeStatus
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::InitEdgeStatus () 
{
  Standard_Boolean visible;
  HLRBRep_FaceIterator faceIt;
  
  HLRBRep_Array1OfEData& aEDataArray = myDS->EDataArray();
  HLRBRep_Array1OfFData& aFDataArray = myDS->FDataArray();
  Standard_Integer ne = myDS->NbEdges();
  Standard_Integer nf = myDS->NbFaces();

  for (Standard_Integer e = 1; e <= ne; e++) {
    HLRBRep_EdgeData ed = aEDataArray.ChangeValue(e);
    if (ed.Selected()) ed.Status().ShowAll();
  }
//  for (Standard_Integer f = 1; f <= nf; f++) {
  Standard_Integer f;
  for ( f = 1; f <= nf; f++) {
    HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
    if (fd.Selected()) {
      
      for (faceIt.InitEdge(fd);
	   faceIt.MoreEdge();
	   faceIt.NextEdge()) {
	HLRBRep_EdgeData* edf = &(myDS->EDataArray().ChangeValue(faceIt.Edge()));
	if (edf->Selected()) edf->Status().HideAll();
      }
    }
  }

  for (f = 1; f <= nf; f++) {
    HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
    visible = Standard_True;
    if (fd.Selected() && fd.Closed()) {
      if      ( fd.Side())      visible =  Standard_False;
      else if ( !fd.WithOutL()) {
	switch (fd.Orientation()) {
	case TopAbs_REVERSED : visible =  fd.Back()   ; break;
	case TopAbs_FORWARD  : visible = !fd.Back()   ; break;
	case TopAbs_EXTERNAL :
	case TopAbs_INTERNAL : visible =  Standard_True; break;
        }
      }
    } 
    if (visible) {
      
      for (faceIt.InitEdge(fd);
	   faceIt.MoreEdge();
	   faceIt.NextEdge()) {
	Standard_Integer E = faceIt.Edge();
	HLRBRep_EdgeData* edf = &(myDS->EDataArray().ChangeValue(E));
	if ( edf->Selected() &&
	    !edf->Vertical())
	  edf->Status().ShowAll();
      }
    }
  }
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::Select ()
{
  if (!myDS.IsNull()) {
    HLRBRep_Array1OfEData& aEDataArray = myDS->EDataArray();
    HLRBRep_Array1OfFData& aFDataArray = myDS->FDataArray();
    Standard_Integer ne = myDS->NbEdges();
    Standard_Integer nf = myDS->NbFaces();
    
    for (Standard_Integer e = 1; e <= ne; e++) {
      HLRBRep_EdgeData ed = aEDataArray.ChangeValue(e);
      ed.Selected(Standard_True);
    }
    
    for (Standard_Integer f = 1; f <= nf; f++) {
      HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
      fd.Selected(Standard_True);
    }
  }
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::Select (const Standard_Integer I)
{
  if (!myDS.IsNull()) { 
    Standard_OutOfRange_Raise_if
      (I == 0 || I > myShapes.Length(),
       "HLRBRep_InternalAlgo::Select : unknown Shape");

    Standard_Integer v1,v2,e1,e2,f1,f2;
    myShapes(I).Bounds(v1,v2,e1,e2,f1,f2);
    
    HLRBRep_Array1OfEData& aEDataArray = myDS->EDataArray();
    HLRBRep_Array1OfFData& aFDataArray = myDS->FDataArray();
    Standard_Integer ne = myDS->NbEdges();
    Standard_Integer nf = myDS->NbFaces();
    
    for (Standard_Integer e = 1; e <= ne; e++) {
      HLRBRep_EdgeData& ed = aEDataArray.ChangeValue(e);
      ed.Selected(e >= e1 && e <= e2);
    }
    
    for (Standard_Integer f = 1; f <= nf; f++) {
      HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
      fd.Selected(f >= f1 && f <= f2);
    }
  }
}

//=======================================================================
//function : SelectEdge
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::SelectEdge (const Standard_Integer I)
{
  if (!myDS.IsNull()) {
    Standard_OutOfRange_Raise_if
      (I == 0 || I > myShapes.Length(),
       "HLRBRep_InternalAlgo::SelectEdge : unknown Shape");
    
    Standard_Integer v1,v2,e1,e2,f1,f2;
    myShapes(I).Bounds(v1,v2,e1,e2,f1,f2);
    
    HLRBRep_Array1OfEData& aEDataArray = myDS->EDataArray();
    Standard_Integer ne = myDS->NbEdges();
    
    for (Standard_Integer e = 1; e <= ne; e++) {
      HLRBRep_EdgeData& ed = aEDataArray.ChangeValue(e);
      ed.Selected(e >= e1 && e <= e2);
    }
  }
}

//=======================================================================
//function : SelectFace
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::SelectFace (const Standard_Integer I)
{
  if (!myDS.IsNull()) {
    Standard_OutOfRange_Raise_if
      (I == 0 || I > myShapes.Length(),
       "HLRBRep_InternalAlgo::SelectFace : unknown Shape");
    
    Standard_Integer v1,v2,e1,e2,f1,f2;
    myShapes(I).Bounds(v1,v2,e1,e2,f1,f2);
    
    HLRBRep_Array1OfFData& aFDataArray = myDS->FDataArray();
    Standard_Integer nf = myDS->NbFaces();
    
    for (Standard_Integer f = 1; f <= nf; f++) {
      HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
      fd.Selected(f >= f1 && f <= f2);
    }
  }
}

//=======================================================================
//function : ShowAll
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::ShowAll ()
{
  if (!myDS.IsNull()) {
    HLRBRep_Array1OfEData& aEDataArray = myDS->EDataArray();
    Standard_Integer ne = myDS->NbEdges();

    for (Standard_Integer ie = 1; ie <= ne; ie++) {
      HLRBRep_EdgeData& ed = aEDataArray.ChangeValue(ie);
      ed.Status().ShowAll();
    }
  }
}

//=======================================================================
//function : ShowAll
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::ShowAll (const Standard_Integer I)
{
  if (!myDS.IsNull()) {
    Standard_OutOfRange_Raise_if
      (I == 0 || I > myShapes.Length(),
       "HLRBRep_InternalAlgo::ShowAll : unknown Shape");
    
    Select(I);
    
    HLRBRep_Array1OfEData& aEDataArray = myDS->EDataArray();
    Standard_Integer ne = myDS->NbEdges();
    
    for (Standard_Integer e = 1; e <= ne; e++) {
      HLRBRep_EdgeData& ed = aEDataArray.ChangeValue(e);
      if (ed.Selected()) ed.Status().ShowAll();
    }
  }
}

//=======================================================================
//function : HideAll
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::HideAll ()
{
  if (!myDS.IsNull()) {
    HLRBRep_Array1OfEData& aEDataArray = myDS->EDataArray();
    Standard_Integer ne = myDS->NbEdges();
    
    for (Standard_Integer ie = 1; ie <= ne; ie++) {
      HLRBRep_EdgeData& ed = aEDataArray.ChangeValue(ie);
      ed.Status().HideAll();
    }
  }
}

//=======================================================================
//function : HideAll
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::HideAll (const Standard_Integer I)
{
  if (!myDS.IsNull()) {
    Standard_OutOfRange_Raise_if
      (I == 0 || I > myShapes.Length(),
       "HLRBRep_InternalAlgo::HideAll : unknown Shape");
    
    Select(I);
    
    HLRBRep_Array1OfEData& aEDataArray = myDS->EDataArray();
    Standard_Integer ne = myDS->NbEdges();
    
    for (Standard_Integer e = 1; e <= ne; e++) {
      HLRBRep_EdgeData& ed = aEDataArray.ChangeValue(e);
      if (ed.Selected()) ed.Status().HideAll();
    }
  }
}

//=======================================================================
//function : PartialHide
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::PartialHide ()
{
  if (!myDS.IsNull()) {
    Standard_Integer i,n = myShapes.Length();

    if (myDebug)
      cout << " Partial hiding" << endl << endl;

    for (i = 1; i <= n; i++)
      Hide(i);
    
    Select();
  }
}

//=======================================================================
//function : Hide
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::Hide ()
{
  if (!myDS.IsNull()) {
    Standard_Integer i,j,n = myShapes.Length();

    if (myDebug)
      cout << " Total hiding" << endl;

    for (i = 1; i <= n; i++)
      Hide(i);
    
    for (i = 1; i <= n; i++)
      for (j = 1; j <= n; j++)
	if (i != j) Hide(i,j);
    
    Select();
  }
}

//=======================================================================
//function : Hide
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::Hide (const Standard_Integer I)
{
  if (!myDS.IsNull()) {
    Standard_OutOfRange_Raise_if
      (I == 0 || I > myShapes.Length(),
       "HLRBRep_InternalAlgo::Hide : unknown Shape");
    
    if (myDebug)
      cout << " hiding the shape " << I << " by itself" << endl;
    
    Select(I);
    InitEdgeStatus();
    HideSelected(I,Standard_True);
  }
}

//=======================================================================
//function : Hide
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::Hide (const Standard_Integer I,
				 const Standard_Integer J)
{
  if (!myDS.IsNull()) {
    Standard_OutOfRange_Raise_if
      (I == 0 || I > myShapes.Length() ||
       J == 0 || J > myShapes.Length(),
       "HLRBRep_InternalAlgo::Hide : unknown Shapes");

    if (I == J) Hide(I);
    else {
      HLRAlgo_EdgesBlock::MinMaxIndices* MinMaxShBI = &myShapes(I).MinMax();
      HLRAlgo_EdgesBlock::MinMaxIndices* MinMaxShBJ = &myShapes(J).MinMax();
      if (((MinMaxShBJ->Max[0] - MinMaxShBI->Min[0]) & 0x80008000) == 0 &&
	  ((MinMaxShBI->Max[0] - MinMaxShBJ->Min[0]) & 0x80008000) == 0 &&
	  ((MinMaxShBJ->Max[1] - MinMaxShBI->Min[1]) & 0x80008000) == 0 &&
	  ((MinMaxShBI->Max[1] - MinMaxShBJ->Min[1]) & 0x80008000) == 0 &&
	  ((MinMaxShBJ->Max[2] - MinMaxShBI->Min[2]) & 0x80008000) == 0 &&
	  ((MinMaxShBI->Max[2] - MinMaxShBJ->Min[2]) & 0x80008000) == 0 &&
	  ((MinMaxShBJ->Max[3] - MinMaxShBI->Min[3]) & 0x80008000) == 0 &&
	  ((MinMaxShBI->Max[3] - MinMaxShBJ->Min[3]) & 0x80008000) == 0 &&
	  ((MinMaxShBJ->Max[4] - MinMaxShBI->Min[4]) & 0x80008000) == 0 &&
	  ((MinMaxShBI->Max[4] - MinMaxShBJ->Min[4]) & 0x80008000) == 0 &&
	  ((MinMaxShBJ->Max[5] - MinMaxShBI->Min[5]) & 0x80008000) == 0 &&
	  ((MinMaxShBI->Max[5] - MinMaxShBJ->Min[5]) & 0x80008000) == 0 &&
	  ((MinMaxShBJ->Max[6] - MinMaxShBI->Min[6]) & 0x80008000) == 0 &&
	  ((MinMaxShBJ->Max[7] - MinMaxShBI->Min[7]) & 0x80008000) == 0) {
	if (myDebug) {
	  cout << " hiding the shape " << I;
	  cout << " by the shape : " << J << endl;
	}
	SelectEdge(I);
	SelectFace(J);
	HideSelected(I,Standard_False);
      }
    }
  }
}

//=======================================================================
//function : HideSelected
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::HideSelected (const Standard_Integer I,
					 const Standard_Boolean SideFace)
{
  Standard_Integer e,f,j,nbVisEdges,nbSelEdges,nbSelFaces,nbCache;
  Standard_Integer nbFSide,nbFSimp;

#ifdef OCCT_DEBUG
  if (myDebug) {
    nbPtIntersection = 0;
    nbSegIntersection = 0;
    nbOkIntersection = 0;
    nbClassification = 0;
    nbCal1Intersection = 0;
    nbCal2Intersection = 0;
    nbCal3Intersection = 0;
  }
#endif

  HLRBRep_ShapeBounds& SB = myShapes(I);
  Standard_Integer v1,v2,e1,e2,f1,f2;
  SB.Bounds(v1,v2,e1,e2,f1,f2);

  if (e2 >= e1) {
    myDS->InitBoundSort(SB.MinMax(),e1,e2);
    HLRBRep_Hider Cache(myDS);
    HLRBRep_Array1OfEData& aEDataArray = myDS->EDataArray();
    HLRBRep_Array1OfFData& aFDataArray = myDS->FDataArray();
    Standard_Integer ne = myDS->NbEdges();
    Standard_Integer nf = myDS->NbFaces();
    
    if (myDebug) {
      nbVisEdges = 0;
      nbSelEdges = 0;
      nbSelFaces = 0;
      nbCache = 0;
      nbFSide = 0;
      nbFSimp = 0;
      
      for (e = 1; e <= ne; e++) {
        HLRBRep_EdgeData& ed = aEDataArray.ChangeValue(e);
        if (ed.Selected()) {
	  nbSelEdges++;
	  if (!ed.Status().AllHidden()) nbVisEdges++;
	}
      }
      
      for (f = 1; f <= nf; f++) {
        HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
        if (fd.Selected()) {
	  nbSelFaces++;
	  if (fd.Hiding()) nbCache++;
	  if (fd.Side  ()) nbFSide++;
	  if (fd.Simple()) nbFSimp++;
	}
      }
      
      if (myDebug)
      {
        cout << endl;
        cout << "Vertices  : " << setw(5) << myDS->NbVertices() << endl;
        cout << "Edges     : " << setw(5) << myDS->NbEdges()    << " , ";
        cout << "Selected  : " << setw(5) << nbSelEdges         << " , ";
        cout << "Visibles  : " << setw(5) << nbVisEdges         << endl;
        cout << "Faces     : " << setw(5) << myDS->NbFaces()    << " , ";
        cout << "Selected  : " << setw(5) << nbSelFaces         << " , ";
        cout << "Simple    : " << setw(5) << nbFSimp            << endl;
        if (SideFace)
          cout << "Side      : " << setw(5) << nbFSide            << " , ";
        cout << "Cachantes : " << setw(5) << nbCache            << endl << endl;
      }
    }

    if (nf == 0)
      return;

    Standard_Integer QWE=0,QWEQWE;
    QWEQWE=nf/10;

    if (SideFace) {
      j = 0;
      
      for (f = 1; f <= nf; f++) {
        HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
        if (fd.Selected()) {
	  if (fd.Side()) {
	    if(HLRBRep_InternalAlgo_TRACE10) { 
	      if(++QWE>QWEQWE) { 
		QWE=0; 
                if (myDebug)
                  cout<<"*";
	      } 
	    }
	    else {  
	      if (myDebug && HLRBRep_InternalAlgo_TRACE) {
		j++;
		cout << " OwnHiding " << j << " of face : " << f << endl;
	      }
	    }
	    Cache.OwnHiding(f);
	  }
	}
      }
    }
    

//--
    TColStd_Array1OfInteger Val(1, nf);
    TColStd_Array1OfReal    Size(1, nf);
    TColStd_Array1OfInteger Index(1, nf);


    for (f = 1; f <= nf; f++) {
      HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
      if (fd.Plane())          Val(f) = 10;
      else if(fd.Cylinder())  Val(f)=9;
      else if(fd.Cone())      Val(f)=8;
      else if(fd.Sphere())    Val(f)=7;
      else if(fd.Torus())     Val(f)=6;
      else Val(f)=0;
      if(fd.Cut())            Val(f)-=10;
      if(fd.Side())           Val(f)-=100;
      if(fd.WithOutL())       Val(f)-=20;
  
      Size(f)=fd.Size();
    }

    for(Standard_Integer tt=1;tt<=nf;tt++) { 
      Index(tt)=tt;
    }

    //-- ======================================================================
/*    Standard_Boolean TriOk; //-- a refaire
    do { 
      Standard_Integer t,tp1;
      TriOk=Standard_True;
      for(t=1,tp1=2;t<nf;t++,tp1++) { 
	if(Val(Index(t))<Val(Index(tp1))) {
	  Standard_Integer q=Index(t); Index(t)=Index(tp1); Index(tp1)=q;
	  TriOk=Standard_False;
	}
	else if(Val(Index(t))==Val(Index(tp1))) { 
	  if(Size(Index(t))<Size(Index(tp1))) { 
	    Standard_Integer q=Index(t); Index(t)=Index(tp1); Index(tp1)=q;
	    TriOk=Standard_False;
	  }
	}
      }
    }
    while(TriOk==Standard_False);
*/
    //-- ======================================================================
    if(nf>2)  {
      Standard_Integer i,ir,k,l;
      Standard_Integer rra;
      l=(nf>>1)+1;
      ir=nf;
      for(;;) { 
	if(l>1) { 
	  rra=Index(--l); 
	} 
	else {    
	  rra=Index(ir); 
	  Index(ir)=Index(1);
	  if(--ir == 1) { 
	    Index(1)=rra;
	    break;
	  }
	}
	i=l;
	k=l+l;
	while(k<=ir) { 
	  if(k<ir) { 
	    if(Val(Index(k)) > Val(Index(k+1)))
	      k++;
	    else if(Val(Index(k)) == Val(Index(k+1))) { 
	      if(Size(Index(k)) > Size(Index(k+1))) 
		k++;
	    }
	  }
	  if(Val(rra) > Val(Index(k))) { 
	    Index(i)=Index(k);
	    i=k;
	    k<<=1;
	  }
	  else if((Val(rra) == Val(Index(k))) && (Size(rra) > Size(Index(k)))) { 
	    Index(i)=Index(k);
	    i=k;
	    k<<=1;
	  }
	  else {  
	    k=ir+1;
	  }
	}
	Index(i)=rra;
      }
    }

    j = 0;
    
    QWE=0;
    for (f = 1; f <= nf; f++) {
      Standard_Integer fi = Index(f);
      HLRBRep_FaceData& fd = aFDataArray.ChangeValue(fi);
      if (fd.Selected()) {
	if (fd.Hiding()) {
	  if(HLRBRep_InternalAlgo_TRACE10 && HLRBRep_InternalAlgo_TRACE==Standard_False) { 
	    if(++QWE>QWEQWE) { 
	      if (myDebug)
                cout<<".";
	      QWE=0;
	    }
	  }
	  else if (myDebug && HLRBRep_InternalAlgo_TRACE) {
	    static int rty=0;
	    j++;
	    printf("%6d",fi); fflush(stdout);
	    if(++rty>25) { rty=0; printf("\n"); } 
	  }
	  Cache.Hide(fi,myMapOfShapeTool);
	}
      }
    }
    
#ifdef OCCT_DEBUG
    if (myDebug) {
      nbFSimp = 0;
      
      for (f = 1; f <= nf; f++) {
        HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
        if (fd.Selected() && fd.Simple())
	  nbFSimp++;
      }

      cout << "\n";
      cout << "Simple Faces                  : ";
      cout << nbFSimp  << "\n";
      cout << "Intersections calculees       : ";
      cout << nbCal2Intersection << "\n";
      cout << "Intersections Ok              : ";
      cout << nbOkIntersection << "\n";
      cout << "Points                        : ";
      cout << nbPtIntersection << "\n";
      cout << "Segments                      : ";
      cout << nbSegIntersection << "\n";
      cout << "Classification                : ";
      cout << nbClassification << "\n";
      cout << "Intersections curve-surface   : ";
      cout << nbCal3Intersection << "\n";
      cout << endl << endl;
    }
#endif
  }
}

//=======================================================================
//function : Debug
//purpose  : 
//=======================================================================

void HLRBRep_InternalAlgo::Debug (const Standard_Boolean deb)
{ myDebug = deb; }

//=======================================================================
//function : Debug
//purpose  : 
//=======================================================================

Standard_Boolean HLRBRep_InternalAlgo::Debug () const
{ return myDebug; }

//=======================================================================
//function : DataStructure
//purpose  : 
//=======================================================================

Handle(HLRBRep_Data) HLRBRep_InternalAlgo::DataStructure () const
{ return myDS; }
