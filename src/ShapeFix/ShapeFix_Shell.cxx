// Created on: 1998-08-12
// Created by: Galina KULIKOVA
// Copyright (c) 1998-1999 Matra Datavision
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

// pdn 17.12.98 ie_exhaust-A.stp

#include <Bnd_Array1OfBox.hxx>
#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <Message_Msg.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressSentry.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Shell.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend.hxx>
#include <ShapeExtend_BasicMsgRegistrator.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_Shell.hxx>
#include <Standard_Type.hxx>
#include <TColStd_DataMapOfIntegerListOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeInteger.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeFix_Shell,ShapeFix_Root)

//=======================================================================
//function : ShapeFix_Shell
//purpose  : 
//=======================================================================
ShapeFix_Shell::ShapeFix_Shell()
{    
  myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);
  myFixFaceMode = -1;
  myFixOrientationMode = -1;
  myFixFace = new ShapeFix_Face;
  myNbShells =0;
  myNonManifold = Standard_False;
}

//=======================================================================
//function : ShapeFix_Shell
//purpose  : 
//=======================================================================

ShapeFix_Shell::ShapeFix_Shell(const TopoDS_Shell& shape)
{
  myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);
  myFixFaceMode = -1;
  myFixOrientationMode = -1;
  myFixFace = new ShapeFix_Face;
  Init(shape);
  myNonManifold = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShapeFix_Shell::Init(const TopoDS_Shell& shell) 
{
  myShape = shell;
  myShell = shell;
  myNbShells =0;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_Shell::Perform(const Handle(Message_ProgressIndicator)& theProgress) 
{
  Standard_Boolean status = Standard_False;
  if ( Context().IsNull() )
    SetContext(new ShapeBuild_ReShape);
  myFixFace->SetContext(Context());

  if ( NeedFix(myFixFaceMode) )
  {
    TopoDS_Shape S = Context()->Apply(myShell);

    // Get the number of faces for progress indication
    Standard_Integer aNbFaces = 0;
    for ( TopExp_Explorer aFaceExp(S, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next() )
      ++aNbFaces;

    // Start progress scope (no need to check if progress exists -- it is safe)
    Message_ProgressSentry aPSentry(theProgress, "Fixing face", 0, aNbFaces, 1);

    for( TopoDS_Iterator iter(S); iter.More() && aPSentry.More(); iter.Next(), aPSentry.Next() )
    { 
      TopoDS_Shape sh = iter.Value();
      TopoDS_Face tmpFace = TopoDS::Face(sh);
      myFixFace->Init(tmpFace);
      if ( myFixFace->Perform() )
      {
        status = Standard_True;
        myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
      }
    }

    // Halt algorithm in case of user's abort
    if ( !aPSentry.More() )
      return Standard_False;
  }

  TopoDS_Shape newsh = Context()->Apply(myShell);
  if ( NeedFix ( myFixOrientationMode) )
    FixFaceOrientation(TopoDS::Shell(newsh), Standard_True, myNonManifold);

  TopoDS_Shape aNewsh = Context()->Apply (newsh);
  ShapeAnalysis_Shell aSas;
  for (TopExp_Explorer aShellExp (aNewsh, TopAbs_SHELL); aShellExp.More(); aShellExp.Next())
  {
    TopoDS_Shell aCurShell = TopoDS::Shell (aShellExp.Current());
    if (aCurShell.Closed())
    {
      aSas.LoadShells (aCurShell);
      aSas.CheckOrientedShells (aCurShell, Standard_True);
      if (aSas.HasFreeEdges())
      {
        aCurShell.Closed (Standard_False);
        SendWarning (Message_Msg ("FixAdvShell.FixClosedFlag.MSG0"));//Shell has incorrect flag isClosed
      }
      aSas.Clear();
	}
  }

  if ( status )
    myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
  if(Status(ShapeExtend_DONE2))
    status = Standard_True;
  return status;
}

//=======================================================================
// function : GetFreeEdges
// purpose  : 
//=======================================================================
static Standard_Boolean GetFreeEdges(const TopoDS_Shape& aShape,TopTools_MapOfShape& MapEdges)
{
  for(TopExp_Explorer aExpF(aShape,TopAbs_FACE); aExpF.More(); aExpF.Next()) {
    for(TopExp_Explorer aExpE(aExpF.Current(),TopAbs_EDGE); aExpE.More(); aExpE.Next()) {
      TopoDS_Edge edge = TopoDS::Edge(aExpE.Current());
      if(!MapEdges.Contains(edge))
        MapEdges.Add(edge);
      else  MapEdges.Remove(edge);
    }
  }
  return !MapEdges.IsEmpty();
}
//=======================================================================
// function : GetShells
// purpose  : If mode isMultiConnex = Standard_True gets max possible shell for 
//            exception of multiconnexity parts.
//            Else if this mode is equal to Standard_False maximum possible 
//            shell will be created without taking account of multiconnexity.
//            In this function map face - shell and sequence of mebius faces is formed.
//=======================================================================
static  Standard_Boolean GetShells(TopTools_SequenceOfShape& Lface,
                                   const TopTools_MapOfShape& aMapMultiConnectEdges,
                                   TopTools_SequenceOfShape& aSeqShells,
                                   TopTools_DataMapOfShapeShape& aMapFaceShells,
                                   TopTools_SequenceOfShape& ErrFaces) 
{
  Standard_Boolean done = Standard_False;
  if(!Lface.Length()) return Standard_False;
  TopoDS_Shell nshell;
  TopTools_MapOfShape dire, reve;
  BRep_Builder B;
  B.MakeShell(nshell);
  Standard_Boolean isMultiConnex = !aMapMultiConnectEdges.IsEmpty();
  Standard_Integer i=1, j=1;
  TopTools_SequenceOfShape aSeqUnconnectFaces;
  for( ; i<=Lface.Length(); i++)  {
    TopTools_MapOfShape dtemp, rtemp;
    Standard_Integer nbbe=0, nbe = 0;
    TopoDS_Face F1 = TopoDS::Face(Lface.Value(i));
    for(TopExp_Explorer expe(F1, TopAbs_EDGE); expe.More(); expe.Next()) {
      TopoDS_Edge edge = TopoDS::Edge(expe.Current());
      
      // if multiconnexity mode is equal to Standard_True faces contains
      // the same multiconnexity edges are not added to one shell.
      if(isMultiConnex && aMapMultiConnectEdges.Contains(edge))
        continue;
      
      if((edge.Orientation() == TopAbs_FORWARD && dire.Contains(edge))
        || (edge.Orientation() == TopAbs_REVERSED && reve.Contains(edge))) 
        nbbe++;
      else if((edge.Orientation() == TopAbs_FORWARD && reve.Contains(edge))
        || (edge.Orientation() == TopAbs_REVERSED && dire.Contains(edge)))   
        nbe++;
      
      if(dire.Contains(edge)) dire.Remove(edge);
      else 
        if(reve.Contains(edge)) reve.Remove(edge);
        else {
          if(edge.Orientation() == TopAbs_FORWARD) dtemp.Add(edge);
          if(edge.Orientation() == TopAbs_REVERSED) rtemp.Add(edge);
        }
    }
    if(!nbbe && !nbe && dtemp.IsEmpty() && rtemp.IsEmpty()) 
      continue;
    
    // if face can not be added to shell it added to sequence of error faces.
    
    if( nbe != 0 && nbbe != 0) {
      ErrFaces.Append(F1);
      Lface.Remove(i);
      j++;
      continue;
    }
    
    // Addition of face to shell. In the dependance of orientation faces in the shell 
    //  added face can be reversed.

    if((nbe != 0 || nbbe != 0) || j == 1) {
      if(nbbe != 0) {
        F1.Reverse();
        for(TopTools_MapIteratorOfMapOfShape ite(dtemp); ite.More(); ite.Next()) 
          reve.Add(ite.Key());
        for(TopTools_MapIteratorOfMapOfShape ite1(rtemp); ite1.More(); ite1.Next())
          dire.Add(ite1.Key());
        done = Standard_True;
      }
      else {
        for(TopTools_MapIteratorOfMapOfShape ite(dtemp); ite.More(); ite.Next()) 
          dire.Add(ite.Key());
        for(TopTools_MapIteratorOfMapOfShape ite1(rtemp); ite1.More(); ite1.Next())
          reve.Add(ite1.Key());
      }
      j++;
      B.Add(nshell,F1);
      aMapFaceShells.Bind(F1,nshell);
      Lface.Remove(i);
      
      // check if closed shell is obtained in multy connex mode and add to sequence of 
      // shells and new shell begin to construct.
      // (check is n*2)
      if(isMultiConnex && BRep_Tool::IsClosed (nshell)) {
        nshell.Closed (Standard_True);
        aSeqShells.Append(nshell);
        TopoDS_Shell nshellnext;
        B.MakeShell(nshellnext);
        nshell = nshellnext;
        j=1;
      }
        
      i=0;
    }
    //if shell contains of one face. This face is added to sequence of faces.
    // This shell is removed.
    if(Lface.Length() && i == Lface.Length() && j <=2) {
      TopoDS_Iterator aItf(nshell,Standard_False);
      if(aItf.More()) {
        aSeqUnconnectFaces.Append(aItf.Value());
        aMapFaceShells.UnBind(aItf.Value());
      }
      TopoDS_Shell nshellnext;
      B.MakeShell(nshellnext);
      nshell = nshellnext;
      i=0;
      j=1;
    }
  }
  Standard_Boolean isContains = Standard_False;
  for(Standard_Integer k =1 ; k <= aSeqShells.Length() && !isContains; k++)
    isContains = nshell.IsSame(aSeqShells.Value(k));
  if(!isContains) {
    Standard_Integer numFace =0;
    TopoDS_Shape aFace;
    for(TopoDS_Iterator aItf(nshell,Standard_False) ; aItf.More(); aItf.Next()) {
      aFace = aItf.Value();
      numFace++;
    }
    if(numFace >1) {
      // close all closed shells in no multy connex mode
      if(!isMultiConnex)
        nshell.Closed (BRep_Tool::IsClosed (nshell));
      aSeqShells.Append(nshell);
    }
    else if(numFace == 1) {
      if(aMapFaceShells.IsBound(aFace))
        aMapFaceShells.UnBind(aFace);
      Lface.Append(aFace);
    }
  }
  
  //Sequence of faces Lface contains faces which can not be added to obtained shells.
  for(Standard_Integer j1 =1; j1 <= aSeqUnconnectFaces.Length(); j1++) {
    Lface.Append(aSeqUnconnectFaces);
  }
  
  return done;
}
//=======================================================================
// function : AddMultiConexityFaces
// purpose  : In this function faces have only of multiconnexity boundary
//            are added to shells having free boundary contains the same 
//            multiconnexity edges as faces.
//=======================================================================
static Standard_Boolean AddMultiConexityFaces(TopTools_SequenceOfShape& Lface,
                                              const TopTools_MapOfShape& aMapMultiConnectEdges,
                                              TopTools_SequenceOfShape& SeqShells,
                                              const TopTools_DataMapOfShapeShape& aMapFaceShells,
                                              const TopTools_IndexedDataMapOfShapeListOfShape& aMapEdgeFaces,
                                              TopTools_SequenceOfShape& ErrFaces,
                                              const Standard_Boolean NonManifold)
{
  Standard_Boolean done = Standard_False;
//  BRep_Builder aB;
  TopTools_SequenceOfShape llPosibleShells;
  TopTools_SequenceOfShape AddShapes; 
  for(Standard_Integer i1 = 1 ; i1<=Lface.Length();i1++ )  {
   
    TopoDS_Shape aShape = Lface.Value(i1);
    
    Standard_Integer aNbMultEdges =0;
    
    //Finds faces having only multiconnexity boundary.
    for(TopoDS_Iterator aItWires(aShape,Standard_False);  aItWires.More();  aItWires.Next()) {
      Standard_Integer aNbEdges =0;
      for(TopoDS_Iterator aItEdges(aItWires.Value(),Standard_False);  aItEdges.More();  aItEdges.Next(),aNbEdges++) {
        TopoDS_Shape edge = aItEdges.Value();
        if(!aMapMultiConnectEdges.Contains(edge)) continue;
        aNbMultEdges++;
      }
      if(!aNbMultEdges) continue;
    
      if(aNbMultEdges == aNbEdges)
        AddShapes.Append(aShape);
      else llPosibleShells.Append(aShape);
    }
  }
  
  // Attemp to create shell from unconnected which have not only multiconnexity boundary.
  TopTools_SequenceOfShape aTmpShells;
  if(!llPosibleShells.IsEmpty()) {
    TopTools_MapOfShape aMap;
    TopTools_SequenceOfShape aTmp;
    TopTools_DataMapOfShapeShape aTmpFaceShell;
    if(GetShells(llPosibleShells,aMap,aTmpShells,aTmpFaceShell,aTmp)) {
      for(Standard_Integer kk =1; kk <= aTmpShells.Length(); kk++) {
        TopoDS_Shape aSh = aTmpShells.Value(kk);
        TopTools_MapOfShape mapEdges;
        if(GetFreeEdges(aSh,mapEdges)) {
          Standard_Integer nbedge =0;
          for(TopTools_MapIteratorOfMapOfShape amapIter(mapEdges);amapIter.More(); amapIter.Next()) {
            if( aMapMultiConnectEdges.Contains(amapIter.Key()))
              nbedge++;
          }
          if(nbedge && nbedge == mapEdges.Extent())
            AddShapes.Append(aSh);
        }
      }
    }
  }
  
  //Add choosen faces to shells.
  for(Standard_Integer k1 =1; k1 <= AddShapes.Length(); k1++) {
    TopTools_DataMapOfShapeInteger MapOtherShells;
    TopTools_MapOfShape dire,reve;
    TopoDS_Shape aSh = AddShapes.Value(k1);
    TopTools_MapOfShape mapEdges;
    if(!GetFreeEdges(aSh,mapEdges)) continue;
    TopTools_ListOfShape lfaces;
    
    //Fill MapOtherShells which will contain shells with orientation in which selected shape aSh will be add.
    
    for(TopTools_MapIteratorOfMapOfShape amapIter(mapEdges);amapIter.More(); amapIter.Next()) {
      if(!aMapMultiConnectEdges.Contains(amapIter.Key())) continue;
      TopoDS_Edge edge = TopoDS::Edge(amapIter.Key());
      if( edge.Orientation() == TopAbs_FORWARD) dire.Add(edge);
      else reve.Add(edge);
      TopTools_ListOfShape lf;
      lf = aMapEdgeFaces.FindFromKey(edge);
      lfaces.Append(lf);
    }
    for(TopTools_ListIteratorOfListOfShape aItl(lfaces) ; aItl.More(); aItl.Next()) {
      TopoDS_Shape aF = aItl.Value();
      if(!aMapFaceShells.IsBound( aF)) continue;
     
      TopoDS_Shape aOthershell;
      aOthershell = aMapFaceShells.Find(aF);
      if(MapOtherShells.IsBound(aOthershell)) continue;
      if(!NonManifold && BRep_Tool::IsClosed(aOthershell))
        continue;
      
      TopTools_MapOfShape mapShellEdges;
      GetFreeEdges(aOthershell,mapShellEdges);
      Standard_Boolean isAdd = Standard_True;
      for(TopTools_MapIteratorOfMapOfShape amapIter1(mapEdges);amapIter1.More() && isAdd ; amapIter1.Next()) 
        isAdd = mapShellEdges.Contains(amapIter1.Key());
      
      if(!isAdd) continue;
      Standard_Integer nbdir =0, nbrev=0;
      
      //add only free face whome all edges contains in the shell as open boundary.
      for(TopTools_MapIteratorOfMapOfShape aIte( mapShellEdges);aIte.More() ;aIte.Next()) {
        TopoDS_Edge edgeS = TopoDS::Edge(aIte.Key());
        if(!aMapMultiConnectEdges.Contains(edgeS)) continue;
        if( (edgeS.Orientation() == TopAbs_FORWARD && dire.Contains(edgeS)) || (edgeS.Orientation() == TopAbs_REVERSED && reve.Contains(edgeS)))  nbrev++;
        else if((edgeS.Orientation() == TopAbs_FORWARD && reve.Contains(edgeS))
                || (edgeS.Orientation() == TopAbs_REVERSED && dire.Contains(edgeS))) nbdir++;
      }
      if(nbdir && nbrev) {
        ErrFaces.Append(aSh);
        continue;
      }
      if(nbdir || nbrev) {
        Standard_Integer isReverse =(nbrev ? 1: 0);
        MapOtherShells.Bind(aOthershell,isReverse);
      }
      
    }
    if(MapOtherShells.IsEmpty()) {
//      i1++;
      continue;
    }
    
    //Adds face to open shells containg the same multishared edges.
    //For nonmanifold mode creation ine shell from face and shells containing the same multishared edges.
    // If one face can be added to a few shells (case of compsolid) face will be added to each shell.
    done = Standard_True;
    Standard_Integer FirstRev = 0,FirstInd =0;
    Standard_Integer ind =0;
    for(Standard_Integer l =1; l <= SeqShells.Length(); l++) {
      if(!MapOtherShells.IsBound(SeqShells.Value(l))) continue;
      ind++;
      Standard_Integer isRev = MapOtherShells.Find(SeqShells.Value(l));
      TopoDS_Shape anewShape = (isRev ? aSh.Reversed() :aSh);
     
      BRep_Builder aB1;
      TopoDS_Shape aShell = SeqShells.Value(l);
      if(ind ==1 || !NonManifold) {
        if(ind ==1) {
          FirstRev = isRev;
          FirstInd = l;
        }
        for(TopExp_Explorer aE(anewShape,TopAbs_FACE); aE.More(); aE.Next())
          aB1.Add(aShell,aE.Current());
        SeqShells.ChangeValue(l) = aShell;
      }
      else if(NonManifold) {
        Standard_Boolean isReversed = !((!(isRev) && !FirstRev) || ((isRev) && FirstRev));
        aShell = SeqShells.Value(FirstInd);
        for(TopoDS_Iterator aItF(SeqShells.Value(l),Standard_False); aItF.More(); aItF.Next()) {
          TopoDS_Shape nF = ( isReversed ? aItF.Value().Reversed() : aItF.Value());
          aB1.Add(aShell,nF);
        }
        SeqShells.ChangeValue(FirstInd) = aShell;
        SeqShells.Remove(l--);
      }
    }
      
    dire.Clear();
    reve.Clear();
    for(TopExp_Explorer aEt(aSh,TopAbs_FACE); aEt.More(); aEt.Next()) {
      for(Standard_Integer kk =1 ; kk <= Lface.Length(); kk++) {
        if(aEt.Current().IsSame(Lface.Value(kk)))
          Lface.Remove(kk--);
      }
    }
  }
  return done;
}

//=======================================================================
// function : BoxIn
// purpose  : Check if one face contains inside other.
//=======================================================================
static Standard_Integer BoxIn(const Bnd_Box& theBox1,const Bnd_Box& theBox2)
{
     Standard_Integer aNumIn = 0;
     Standard_Real aXmin1,aYmin1,aXmax1,aYmax1,aXmin2,aYmin2,aXmax2,aYmax2,aZmin1,aZmax1,aZmin2,aZmax2;
     theBox1.Get(aXmin1,aYmin1,aZmin1,aXmax1,aYmax1,aZmax1);
     theBox2.Get(aXmin2,aYmin2,aZmin2,aXmax2,aYmax2,aZmax2);
     if(aXmin1 == aXmin2 && aXmax1 == aXmax2 && aYmin1 == aYmin2 && aYmax1 == aYmax2 &&
        aZmin1 == aZmin2 && aZmax1 == aZmax2)
       aNumIn = 0;
     else if( aXmin1 >= aXmin2 && aXmax1 <= aXmax2 && aYmin1 >= aYmin2 && aYmax1 <= aYmax2 &&
        aZmin1 >= aZmin2 && aZmax1 <= aZmax2)  
        aNumIn = 1;
     else if( aXmin1 <= aXmin2 && aXmax1 >= aXmax2 && aYmin1 <= aYmin2 && aYmax1 >= aYmax2 && aZmin1 <= aZmin2 && aZmax1 >= aZmax2)
       aNumIn = 2;
     return aNumIn;
}
//=======================================================================
// function : GetClosedShells
// purpose  : Check if one shell is a part from other shell.
//            For case of compsolid when afew shells are created from
//            the same set of faces.
//=======================================================================
static void GetClosedShells(TopTools_SequenceOfShape& Shells, TopTools_SequenceOfShape& aRemainShells)
{
  Bnd_Array1OfBox aBoxes(1,Shells.Length());
  for(Standard_Integer i =1; i <= Shells.Length(); i++) {
    Bnd_Box Box;
    BRepBndLib::AddClose(Shells.Value(i),Box);
    aBoxes.SetValue(i,Box);
  }
  TColStd_MapOfInteger aMapNum;
  for(Standard_Integer j = 1; j <= aBoxes.Length(); j++) {
    for(Standard_Integer k = j+1; k <= aBoxes.Length(); k++) {
      Standard_Integer NumIn = BoxIn(aBoxes.Value(j),aBoxes.Value(k));
      switch(NumIn) {
      case 1:aMapNum.Add(k); break;
      case 2: aMapNum.Add(j); break;
        default : break;
      }
    }
  }
  for(Standard_Integer i1 =1; i1 <= Shells.Length(); i1++) {
    if(!aMapNum.Contains(i1))
      aRemainShells.Append(Shells.Value(i1));
  }
  
}
//=======================================================================
// function : GlueClosedCandidate
// purpose  :Attemt firstly to create closed shells from sequence of open shells.
//=======================================================================
static void GlueClosedCandidate(TopTools_SequenceOfShape& OpenShells,
                                                    const TopTools_MapOfShape& aMapMultiConnectEdges,
                                                    TopTools_SequenceOfShape& aSeqNewShells)
                                                    
{
  // Creating new shells if some open shells contain the same free boundary.
  for(Standard_Integer i = 1 ; i < OpenShells.Length();i++ )  {
    TopoDS_Shape aShell = OpenShells.Value(i);
    TopTools_MapOfShape mapEdges1;
    TopTools_MapOfShape dire,reve;
    if(!GetFreeEdges(aShell,mapEdges1)) continue;
    
    for(TopTools_MapIteratorOfMapOfShape aIte( mapEdges1);aIte.More() ;aIte.Next()) {
      TopoDS_Edge edge1 = TopoDS::Edge(aIte.Key());
      if(!aMapMultiConnectEdges.Contains(edge1)) break;
      if(edge1.Orientation() == TopAbs_FORWARD) dire.Add(edge1);
      else if(edge1.Orientation() == TopAbs_REVERSED) reve.Add(edge1);
    }
    if(mapEdges1.Extent() >(dire.Extent() + reve.Extent())) continue;
    
    //Filling map MapOtherShells which contains candidate to creation of closed shell
    // with aShell.
    NCollection_DataMap<TopoDS_Shape, Standard_Boolean, TopTools_ShapeMapHasher> MapOtherShells;
    for(Standard_Integer j = i+1 ; j <= OpenShells.Length();j++ )  {
      Standard_Boolean isAddShell = Standard_True;
      Standard_Boolean isReversed = Standard_False;
      Standard_Integer nbedge =0;
      TopTools_MapOfShape mapEdges2;
      TopoDS_Shape aShell2 = OpenShells.Value(j);
      if(!GetFreeEdges(aShell2,mapEdges2)) continue;
      for(TopTools_MapIteratorOfMapOfShape aIte2( mapEdges2);aIte2.More() && isAddShell;aIte2.Next()) {
        TopoDS_Edge edge2 = TopoDS::Edge(aIte2.Key());
        if(!aMapMultiConnectEdges.Contains(edge2)) {
          isAddShell = Standard_False;
          break;
          //continue;
        }
        isAddShell = (dire.Contains(edge2) || reve.Contains(edge2)); 
        if((edge2.Orientation() == TopAbs_FORWARD && dire.Contains(edge2))
           || (edge2.Orientation() == TopAbs_REVERSED && reve.Contains(edge2)))
          isReversed = Standard_True;
        nbedge++;
      }
      
      if(!isAddShell) continue;
      MapOtherShells.Bind(OpenShells.Value(j),isReversed);
    }
    if(MapOtherShells.IsEmpty()) continue;
    
    
    if (!MapOtherShells.IsEmpty())
    {
      // Case of compsolid when more than two shells have the same free boundary.
      TopTools_SequenceOfShape aSeqCandidate;
      aSeqCandidate.Append(OpenShells.Value(i));
      
      for (NCollection_DataMap<TopoDS_Shape, Standard_Boolean, TopTools_ShapeMapHasher>::Iterator aIt(MapOtherShells); aIt.More(); aIt.Next())
      {
        aSeqCandidate.Append(aIt.Key());
      }
      
      //Creation all possibly shells from choosen candidate.And
      // addition of them to temporary sequence.
      
      TopTools_SequenceOfShape aTmpSeq;
      for(Standard_Integer k =1; k <= aSeqCandidate.Length(); k++) {
        
        for(Standard_Integer l = k+1; l <= aSeqCandidate.Length(); l++) {
          TopoDS_Shell aNewSh;
          BRep_Builder aB;
          aB.MakeShell(aNewSh);
          for(TopoDS_Iterator aIt1(aSeqCandidate.Value(k),Standard_False); aIt1.More(); aIt1.Next())
            aB.Add(aNewSh,aIt1.Value());
          Standard_Boolean isRev = MapOtherShells.Find(aSeqCandidate.Value(l));
          if(k !=1) {
            isRev = (isRev == MapOtherShells.Find(aSeqCandidate.Value(k)));
          }
          for(TopExp_Explorer aExp(aSeqCandidate.Value(l),TopAbs_FACE); aExp.More(); aExp.Next()) {
            TopoDS_Shape aFace = (isRev ? aExp.Current().Reversed(): aExp.Current());
            aB.Add(aNewSh,aFace);
          }
          aTmpSeq.Append(aNewSh);
        }
      }
      
      //Choice from temporary sequence shells contains different set of faces (case of compsolid) 
      TopTools_SequenceOfShape aRemainShells;
      GetClosedShells(aTmpSeq,aRemainShells);
      aSeqNewShells.Append(aRemainShells);
      
      for(Standard_Integer j1 = i+1 ; j1 <= OpenShells.Length();j1++ )  {
        if(!MapOtherShells.IsBound(OpenShells.Value(j1))) continue;
        OpenShells.Remove(j1--);
      }
      
    }
    else {
      BRep_Builder aB;
      TopoDS_Shape aNewShell = aShell;
      TopoDS_Shape addShell;
      Standard_Boolean isReversed = Standard_False;
      for(Standard_Integer j1 = i+1 ; j1 <= OpenShells.Length();j1++ )  {
        if(!MapOtherShells.Find (OpenShells.Value(j1), isReversed)) continue;
        addShell = OpenShells.Value(j1);
        OpenShells.Remove(j1);
        break;
      }
      
      for(TopExp_Explorer aExpF(addShell,TopAbs_FACE); aExpF.More(); aExpF.Next()) {
        TopoDS_Shape aFace = aExpF.Current();
        if(isReversed)
          aFace.Reverse();
        aB.Add(aNewShell,aFace);
      }
      aSeqNewShells.Append(aNewShell);
    }
    
    //OpenShells.ChangeValue(i) = aShell;
    OpenShells.Remove(i--);
  }
}
//=======================================================================
// function : CreateNonManifoldShells
// purpose  : Attempt to create max possible shells from open shells.
//=======================================================================

static void CreateNonManifoldShells(TopTools_SequenceOfShape& SeqShells,
                              const TopTools_MapOfShape& aMapMultiConnectEdges)
{
  TopTools_IndexedDataMapOfShapeListOfShape aMap;
  for(Standard_Integer i =1 ; i <= SeqShells.Length(); i++) {
    TopoDS_Shape aShell = SeqShells.Value(i);
    TopTools_IndexedMapOfShape medeg;
    TopExp::MapShapes(aShell,TopAbs_EDGE,medeg);
    for(TopTools_MapIteratorOfMapOfShape mit(aMapMultiConnectEdges); mit.More(); mit.Next()) {
    //for(TopExp_Explorer aExp(aShell,TopAbs_EDGE); aExp.More(); aExp.Next(),nbe++) {
      //TopoDS_Shape ae = aExp.Current();
      TopoDS_Shape ae =mit.Key();
      //if( aMapMultiConnectEdges.Contains(aExp.Current())) {
      if(medeg.Contains(ae)) {
        if(aMap.Contains(ae))
         aMap.ChangeFromKey(ae).Append(aShell);
        else {
          TopTools_ListOfShape al;
          al.Append(aShell);
          aMap.Add(ae,al);
        }
      }
    }
  }
  TopTools_IndexedDataMapOfShapeShape aMapShells;
  for(Standard_Integer j =1; j <= aMap.Extent(); j++) {
    const TopTools_ListOfShape& LShells = aMap.FindFromIndex(j);
    TopoDS_Shell aNewShell;
    BRep_Builder aB;
    aB.MakeShell(aNewShell);
    TopTools_MapOfShape mapmerge;
    Standard_Boolean ismerged = Standard_False;
    Standard_Integer num = 1;
    for(TopTools_ListIteratorOfListOfShape alit(LShells); alit.More();alit.Next(),num++) { 
      if(!aMapShells.Contains(alit.Value())) {
        for(TopExp_Explorer aEf(alit.Value(),TopAbs_FACE); aEf.More(); aEf.Next()) {
          aB.Add(aNewShell,aEf.Current());
        }
        ismerged = Standard_True;
        mapmerge.Add(alit.Value());
      }
      else if(ismerged) {
        TopoDS_Shape arshell = aMapShells.FindFromKey(alit.Value());
        while(aMapShells.Contains(arshell)){
          TopoDS_Shape ss = aMapShells.FindFromKey(arshell);
          if(ss.IsSame(arshell)) break;
          arshell = ss;
        }
         
        if(!mapmerge.Contains(arshell)) {
          for(TopExp_Explorer aEf(arshell,TopAbs_FACE); aEf.More(); aEf.Next()) {
            aB.Add(aNewShell,aEf.Current());
          }
          mapmerge.Add(arshell);
        }
      }
      else {
        TopoDS_Shape arshell = aMapShells.FindFromKey(alit.Value());
         while(aMapShells.Contains(arshell)) {
          TopoDS_Shape ss = aMapShells.FindFromKey(arshell);
          if(ss.IsSame(arshell)) break;
          arshell = ss;
        }
        if(num == 1) {
          for(TopExp_Explorer aEf(arshell,TopAbs_FACE); aEf.More(); aEf.Next()) 
            aB.Add(aNewShell,aEf.Current());
          
          mapmerge.Add(arshell);
        }
        else if(!mapmerge.Contains(arshell)) {
          for(TopExp_Explorer aEf(arshell,TopAbs_FACE); aEf.More(); aEf.Next()) {
            aB.Add(aNewShell,aEf.Current());
          }
          mapmerge.Add(arshell);
        }
      }
    }
    if(mapmerge.Extent() >1 || ismerged) {
      for(TopTools_MapIteratorOfMapOfShape alit1(mapmerge); alit1.More();alit1.Next()) {
        TopoDS_Shape oldShell = alit1.Key();
         //while(aMapShells.Contains(oldShell)) {
         //  TopoDS_Shape ss = aMapShells.FindFromKey(oldShell);
         //  if(ss.IsSame(oldShell)) break;
         //  oldShell = ss;
         //}
        aMapShells.Add(oldShell,aNewShell);
      }
    }
  }
  TopTools_IndexedMapOfShape MapNewShells;
  for(Standard_Integer nn = 1;nn <= SeqShells.Length(); nn++) {
    if(aMapShells.Contains(SeqShells.Value(nn))) {
      TopoDS_Shape aNewShell = aMapShells.FindFromKey(SeqShells.Value(nn));
      while(aMapShells.Contains(aNewShell)) {
        TopoDS_Shape ss = aMapShells.FindFromKey(aNewShell);
        if(ss.IsSame(aNewShell)) break;
        aNewShell = ss;
      }
      MapNewShells.Add(aNewShell);
      
      SeqShells.Remove(nn--);
    }
    
  }
  for(Standard_Integer ii =1; ii <= MapNewShells.Extent(); ii++)
    SeqShells.Append(MapNewShells.FindKey(ii));
}
//=======================================================================
// function : CreateClosedShell
// purpose  : Attempt to create max possible shells from open shells.
//=======================================================================

static void CreateClosedShell(TopTools_SequenceOfShape& OpenShells,
                              const TopTools_MapOfShape& aMapMultiConnectEdges)
{
  TopTools_SequenceOfShape aNewShells;
  //Attemt firstly to create closed shells.
  GlueClosedCandidate(OpenShells,aMapMultiConnectEdges,aNewShells);
  
  // Creating new shells if some open shells contain the multishared same edges.
  for(Standard_Integer i = 1 ; i < OpenShells.Length();i++ )  {
    Standard_Boolean isAddShell = Standard_False;
    TopoDS_Shape aShell = OpenShells.Value(i);
    Standard_Boolean isReversed = Standard_False;
    for(Standard_Integer j = i+1 ; j <= OpenShells.Length();j++ )  {
      TopTools_MapOfShape mapEdges1;
      TopTools_MapOfShape dire,reve;
      if(!GetFreeEdges(aShell,mapEdges1)) break;
      for(TopTools_MapIteratorOfMapOfShape aIte( mapEdges1);aIte.More() ;aIte.Next()) {
        TopoDS_Edge edge1 = TopoDS::Edge(aIte.Key());
        if(!aMapMultiConnectEdges.Contains(edge1)) continue;
        if(edge1.Orientation() == TopAbs_FORWARD) dire.Add(edge1);
        else if(edge1.Orientation() == TopAbs_REVERSED) reve.Add(edge1);
      }
      if(dire.IsEmpty() &&  reve.IsEmpty()) break;
      TopTools_MapOfShape mapEdges2;
      TopoDS_Shape aShell2 = OpenShells.Value(j);
      if(!GetFreeEdges(aShell2,mapEdges2)) continue;
      for(TopTools_MapIteratorOfMapOfShape aIte2( mapEdges2);aIte2.More() ;aIte2.Next()) {
        TopoDS_Edge edge2 = TopoDS::Edge(aIte2.Key());
        if(!aMapMultiConnectEdges.Contains(edge2)) continue;
        if(!dire.Contains(edge2) && !reve.Contains(edge2)) continue;
        isAddShell = Standard_True;
        if((edge2.Orientation() == TopAbs_FORWARD && dire.Contains(edge2))
           || (edge2.Orientation() == TopAbs_REVERSED && reve.Contains(edge2)))
          isReversed = Standard_True;
      }
      
      if(!isAddShell) continue;
      BRep_Builder aB;
      
      for(TopExp_Explorer aExpF21(OpenShells.Value(j),TopAbs_FACE); aExpF21.More(); aExpF21.Next()) {
        TopoDS_Shape aFace = aExpF21.Current();
        if(isReversed)
          aFace.Reverse();
        aB.Add( aShell,aFace);
      }
      
      OpenShells.ChangeValue(i) = aShell;
      OpenShells.Remove(j--);
    }
  }
 
  OpenShells.Append(aNewShells);
  
}
  

//=======================================================================
// function : FixFaceOrientation
// purpose  : 
//=======================================================================
  
Standard_Boolean ShapeFix_Shell::FixFaceOrientation(
    const TopoDS_Shell& shell,
    const Standard_Boolean isAccountMultiConex,
    const Standard_Boolean NonManifold) 
{
  //myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);
  Standard_Boolean done = Standard_False;
  TopTools_SequenceOfShape aSeqShells;
  TopTools_SequenceOfShape aErrFaces; // Compound of faces like to Mebiuce leaf.
  TopTools_SequenceOfShape Lface;
  TopTools_DataMapOfShapeShape aMapFaceShells;
  myShell = shell;
  myShape = shell;
  Standard_Integer aNumMultShell =0;
  Standard_Integer nbF = 0;
  TopTools_MapOfShape aMapAdded;
  for (TopoDS_Iterator iter(shell); iter.More(); iter.Next(),nbF++) 
  {
    if(aMapAdded.Add(iter.Value()))
      Lface.Append(iter.Value());
  }
  if(Lface.Length() < nbF)
    done = Standard_True;

  TopTools_IndexedDataMapOfShapeListOfShape aMapEdgeFaces;
  TopExp::MapShapesAndAncestors(myShell,TopAbs_EDGE,TopAbs_FACE,aMapEdgeFaces);
  TopTools_MapOfShape aMapMultiConnectEdges;
  Standard_Boolean isFreeBoundaries = Standard_False;
  for(Standard_Integer k = 1; k <= aMapEdgeFaces.Extent(); k++) {
    const Standard_Integer aFaceCount = aMapEdgeFaces.FindFromIndex(k).Extent();
    if (!isFreeBoundaries && aFaceCount == 1) {
      TopoDS_Edge E = TopoDS::Edge(aMapEdgeFaces.FindKey(k));
      if (!BRep_Tool::Degenerated(E))
        isFreeBoundaries = Standard_True;
    }
    //Finds multishared edges
    else if (isAccountMultiConex && aFaceCount > 2)
      aMapMultiConnectEdges.Add(aMapEdgeFaces.FindKey(k));
  }
  if (BRep_Tool::IsClosed(myShell)? isFreeBoundaries : !isFreeBoundaries)
  {
    myShell.Closed (!isFreeBoundaries);
    SendWarning (Message_Msg ("FixAdvShell.FixClosedFlag.MSG0"));//Shell has incorrect flag isClosed
  }
  Standard_Boolean isGetShells = Standard_True;
  //Gets possible shells with taking in account of multiconnexity.
  while(isGetShells && Lface.Length()) {
    TopTools_SequenceOfShape aTmpSeqShells;
    if(GetShells(Lface, aMapMultiConnectEdges, aTmpSeqShells,aMapFaceShells,aErrFaces)) {
      done = Standard_True;
    }
    isGetShells = !aTmpSeqShells.IsEmpty();
    if(isGetShells) 
      aSeqShells.Append(aTmpSeqShells);
  }
  if(!done)
    done = (aSeqShells.Length() >1);
  Standard_Boolean aIsDone = Standard_False;
  if(Lface.Length() > 0 && aSeqShells.Length()) {
    for(Standard_Integer jj =1; jj <= Lface.Length(); jj++) {
      if(aMapFaceShells.IsBound(Lface.Value(jj)))
        aMapFaceShells.UnBind(Lface.Value(jj));
    }
    
    //Addition of faces having only multiconnexity boundary to shells having holes
    // containing only the multiconnexity edges
    aIsDone = AddMultiConexityFaces(Lface,aMapMultiConnectEdges,aSeqShells,aMapFaceShells,
                                    aMapEdgeFaces,aErrFaces,NonManifold);
  }
  aNumMultShell = aSeqShells.Length();
  if (!aErrFaces.IsEmpty())  {
    
    //if Shell contains of Mebius faces one shell will be created from each those face.
    BRep_Builder B;
    B.MakeCompound(myErrFaces);
    TopoDS_Compound aCompShells;
    B.MakeCompound(aCompShells);
    for(Standard_Integer n =1; n <= aErrFaces.Length(); n++)
      B.Add(myErrFaces,aErrFaces.Value(n));
    if(aNumMultShell) {
      if(aNumMultShell == 1) {
        B.Add(aCompShells,aSeqShells.Value(1));
        for(Standard_Integer n1 =1; n1 <= aErrFaces.Length(); n1++) {
          TopoDS_Shell aSh;
          B.MakeShell(aSh);
          B.Add(aSh,aErrFaces.Value(n1));
           B.Add(aCompShells,aSh);
        }
        myShape = aCompShells;
      }
      else {
        for(Standard_Integer i =1; i <= aSeqShells.Length(); i++)
          B.Add(aCompShells,aSeqShells.Value(i));
        for(Standard_Integer n1 =1; n1 <= aErrFaces.Length(); n1++) {
          TopoDS_Shell aSh;
          B.MakeShell(aSh);
          B.Add(aSh,aErrFaces.Value(n1));
           B.Add(aCompShells,aSh);
        }
        myShape = aCompShells;
      }
    }
    
    done = Standard_True;
    myStatus = ShapeExtend::EncodeStatus (ShapeExtend_FAIL);
    SendWarning ( Message_Msg ( "FixAdvShell.FixOrientation.MSG20" ) );// Impossible to orient faces in shell, several shells created
    return Standard_True;
  }
  if(aNumMultShell >1) {
    TopTools_SequenceOfShape OpenShells;
    for(Standard_Integer i1 =1; i1 <= aSeqShells.Length(); i1++) {
      TopoDS_Shape aShell = aSeqShells.Value(i1);
      if(!BRep_Tool::IsClosed(aShell)) {
        OpenShells.Append(aShell);
        aSeqShells.Remove(i1--);
       }
    }
    if(OpenShells.Length() >1) 
      //Attempt of creation closed shell from open shells with taking into account multiconnexity.
      CreateClosedShell(OpenShells,aMapMultiConnectEdges);
    aSeqShells.Append(OpenShells);
    
  }
  
  // In the case if NonManifold is equal to Standard_True one non-manifold shell will be created.
  //Else compound from shells will be created if length of sequence of shape >1.
  if(Lface.Length()) {
   
    for(Standard_Integer i = 1; i <= Lface.Length();i++) {
      BRep_Builder aB;
      TopoDS_Shell OneShell;
      aB.MakeShell(OneShell);
      aB.Add(OneShell, Lface.Value(i));
      aSeqShells.Append(OneShell);
    }
      
  }
  if(NonManifold && aSeqShells.Length() >1 ) {
    CreateNonManifoldShells(aSeqShells,aMapMultiConnectEdges);
  }
  if(!done)
    done = (aSeqShells.Length() >1 || aIsDone);
  if(aSeqShells.Length() == 1) {
    myShell = TopoDS::Shell(aSeqShells.Value(1));
    myShape =myShell;
    myNbShells =1;
  }
  else {
    BRep_Builder B;
    TopoDS_Compound aCompShells;
    B.MakeCompound(aCompShells);
    for(Standard_Integer i =1; i <= aSeqShells.Length(); i++)
      B.Add(aCompShells,aSeqShells.Value(i));  
    myShape = aCompShells;
    myNbShells =  aSeqShells.Length();
  }
  if(done) {
    myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE2);
    if(!Context().IsNull())
      Context()->Replace(shell, myShape);
    if ( myNbShells == 1 )
      SendWarning ( Message_Msg ( "FixAdvShell.FixOrientation.MSG0" ) );// Faces were incorrectly oriented in the shell, corrected
    else
      SendWarning ( Message_Msg ( "FixAdvShell.FixOrientation.MSG30" ) );// Improperly connected shell split into parts
    return Standard_True;
  }
  else return Standard_False;
}

//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_Shell::Status(const ShapeExtend_Status status) const
{
  return ShapeExtend::DecodeStatus (myStatus, status);
}

//=======================================================================
//function : Shell
//purpose  : 
//=======================================================================

TopoDS_Shell ShapeFix_Shell::Shell() 
{
 return myShell; 
}
//=======================================================================
//function : Shell
//purpose  : 
//=======================================================================

TopoDS_Shape ShapeFix_Shell::Shape() 
{
 return myShape; 
}

//=======================================================================
//function : ErrorFaces
//purpose  : 
//=======================================================================

TopoDS_Compound ShapeFix_Shell::ErrorFaces() const
{
  return myErrFaces;
}

//=======================================================================
//function : SetMsgRegistrator
//purpose  : 
//=======================================================================

void ShapeFix_Shell::SetMsgRegistrator(const Handle(ShapeExtend_BasicMsgRegistrator)& msgreg)
{
  ShapeFix_Root::SetMsgRegistrator ( msgreg );
  myFixFace->SetMsgRegistrator ( msgreg );
}

//=======================================================================
//function : SetPrecision
//purpose  : 
//=======================================================================

void ShapeFix_Shell::SetPrecision (const Standard_Real preci) 
{
  ShapeFix_Root::SetPrecision ( preci );
  myFixFace->SetPrecision ( preci );
}

//=======================================================================
//function : SetMinTolerance
//purpose  : 
//=======================================================================

void ShapeFix_Shell::SetMinTolerance (const Standard_Real mintol) 
{
  ShapeFix_Root::SetMinTolerance ( mintol );
  myFixFace->SetMinTolerance ( mintol );
}

//=======================================================================
//function : SetMaxTolerance
//purpose  : 
//=======================================================================

void ShapeFix_Shell::SetMaxTolerance (const Standard_Real maxtol) 
{
  ShapeFix_Root::SetMaxTolerance ( maxtol );
  myFixFace->SetMaxTolerance ( maxtol );
}

//=======================================================================
//function : NbShells
//purpose  : 
//=======================================================================

Standard_Integer ShapeFix_Shell::NbShells() const
{
  return myNbShells;
}

//=======================================================================
//function : SetNonManifoldFlag
//purpose  : 
//=======================================================================

void ShapeFix_Shell::SetNonManifoldFlag(const Standard_Boolean isNonManifold)
{
    myNonManifold = isNonManifold;
}
