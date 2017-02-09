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


#include <Bnd_Box2d.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <Message_Msg.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressSentry.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend.hxx>
#include <ShapeExtend_BasicMsgRegistrator.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Shell.hxx>
#include <ShapeFix_Solid.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeFix_Solid,ShapeFix_Root)

//======================================================
//function : ShapeFix_Solid
//purpose  : 
//=======================================================================
ShapeFix_Solid::ShapeFix_Solid()
{
  myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);
  myFixShellMode = -1;
  myFixShellOrientationMode = -1;
  myFixShell = new ShapeFix_Shell;
  myCreateOpenSolidMode = Standard_False;
}

//=======================================================================
//function : ShapeFix_Solid
//purpose  : 
//=======================================================================

ShapeFix_Solid::ShapeFix_Solid(const TopoDS_Solid& solid)
{
  myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);
  myFixShellMode = -1;
  myFixShellOrientationMode = -1;
  myFixShell = new ShapeFix_Shell;
  myCreateOpenSolidMode = Standard_False;
  Init(solid);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

 void ShapeFix_Solid::Init(const TopoDS_Solid& solid) 
{
  mySolid = solid;
  //mySolid = TopoDS::Solid(shape.EmptyCopied());
  //BRep_Builder B;
 // for( TopoDS_Iterator iter(solid); iter.More(); iter.Next()) 
 //  B.Add(mySolid,TopoDS::Shell(iter.Value()));
  myShape = solid;
}
#ifdef OCCT_DEBUG_GET_MIDDLE_POINT
//=======================================================================
//function : GetMiddlePoint
//purpose  : 
//=======================================================================
static void GetMiddlePoint(const TopoDS_Shape& aShape, gp_Pnt& pmid)
{
  TopExp_Explorer aExp(aShape,TopAbs_EDGE);
  gp_XYZ center(0.0,0.0,0.0);
  Standard_Integer numpoints =0;
  for( ; aExp.More(); aExp.Next()) {
    TopoDS_Edge e1 = TopoDS::Edge(aExp.Current());
    Standard_Real f,l;
    Handle(Geom_Curve) c3d = BRep_Tool::Curve(e1,f,l);
    if(!c3d.IsNull()) {
      for(Standard_Integer i =1 ; i <=5; i++) {
        Standard_Real param = f+(l-f)/4*(i-1);
        gp_Pnt pt;
        numpoints++;
        c3d->D0(param,pt);
        center+=pt.XYZ();
      
      }
    }
  }
  center /= numpoints;
  pmid.SetXYZ(center);
}
#endif
//=======================================================================
//function : CollectSolids
//purpose  : 
//=======================================================================
static void CollectSolids(const TopTools_SequenceOfShape& aSeqShells , 
                          TopTools_IndexedDataMapOfShapeListOfShape& anIndexedMapShellHoles,
                          TopTools_DataMapOfShapeInteger& theMapStatus)
{
  TopTools_MapOfShape aMapHoles;
  TopTools_DataMapOfShapeListOfShape aMapShellHoles;
  for ( Standard_Integer i1 = 1; i1 <= aSeqShells.Length(); i1++ ) {
    TopoDS_Shell aShell1 = TopoDS::Shell(aSeqShells.Value(i1));
    TopTools_ListOfShape lshells;
    aMapShellHoles.Bind(aShell1,lshells);
  }
  //Finds roots shells and hole shells.
  for ( Standard_Integer i = 1; i <= aSeqShells.Length(); i++ ) {
    TopoDS_Shell aShell1 =  TopoDS::Shell(aSeqShells.Value(i));
    TopExp_Explorer aExpEdges(aShell1,TopAbs_EDGE);
    if(!BRep_Tool::IsClosed(aShell1) || !aExpEdges.More()) continue;
    TopoDS_Solid solid;
    BRep_Builder B;
    B.MakeSolid (solid);
    B.Add (solid,aShell1);
    try {
      OCC_CATCH_SIGNALS
     TopAbs_State infinstatus = TopAbs_UNKNOWN;
     BRepClass3d_SolidClassifier bsc3d (solid);
     Standard_Integer st = 0;
     if(!theMapStatus.IsBound(aShell1)) {
         
         bsc3d.PerformInfinitePoint(Precision::Confusion());
         infinstatus = bsc3d.State();
         
         if(infinstatus != TopAbs_UNKNOWN &&  infinstatus !=TopAbs_ON)
           st = (infinstatus == TopAbs_IN ? 1 :2);
         theMapStatus.Bind(aShell1,st);
         
       
       }
      else {
        st = theMapStatus.Find(aShell1);
        if(st)
          infinstatus =  (theMapStatus.Find(aShell1) == 1 ? TopAbs_IN : TopAbs_OUT);
      }
      if(!st) continue;
      for ( Standard_Integer j = 1; j <= aSeqShells.Length(); j++ ) {
        if(i==j) continue;
        TopoDS_Shape aShell2 = aSeqShells.Value(j);
        if(!BRep_Tool::IsClosed(aShell2)) continue;
        if(aMapHoles.Contains(aShell2)) continue;
        if(aMapShellHoles.IsBound(aShell2)) {
          Standard_Boolean isAnalysis = Standard_False;
          const TopTools_ListOfShape& ls = aMapShellHoles.Find(aShell2);
          for(TopTools_ListIteratorOfListOfShape li(ls); li.More() && !isAnalysis; li.Next())
            isAnalysis = li.Value().IsSame(aShell1);
          if(isAnalysis) continue;
        }
        TopAbs_State pointstatus = TopAbs_UNKNOWN;
        Standard_Integer numon =0;
        TopTools_IndexedMapOfShape amapVert;
        for(TopExp_Explorer aExpVert(aShell2,TopAbs_VERTEX); aExpVert.More() && amapVert.Extent() < 10; aExpVert.Next())
          amapVert.Add(aExpVert.Current());
        for(Standard_Integer k = 1; k <= amapVert.Extent() && 
            (pointstatus ==TopAbs_UNKNOWN || (pointstatus ==TopAbs_ON && numon < 3)); k++) {
          TopoDS_Vertex aV = TopoDS::Vertex(amapVert.FindKey(k));
          gp_Pnt aPf =  BRep_Tool::Pnt(aV);
          bsc3d.Perform(aPf,Precision::Confusion());
          pointstatus =bsc3d.State();
          if(pointstatus ==TopAbs_ON) numon++;
        }
        
        if(numon == 3 && pointstatus ==TopAbs_ON) {
#ifdef OCCT_DEBUG_GET_MIDDLE_POINT
          gp_Pnt pmid;
          GetMiddlePoint(aShell2,pmid);
          bsc3d.Perform(pmid,Precision::Confusion());
#endif
          pointstatus = /*(bsc3d.State() == TopAbs_IN ? TopAbs_IN :*/TopAbs_OUT;
        }
        if(pointstatus != infinstatus) {
          aMapShellHoles.ChangeFind(aShell1).Append(aShell2);
          if( aMapHoles.Contains(aShell2))
            aMapHoles.Remove(aShell2);
          else aMapHoles.Add(aShell2);
        }
      }
    }
    catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
      cout << "Warning: ShapeFix_Solid::SolidFromShell: Exception: ";
      anException.Print(cout); cout << endl;
#endif
      (void)anException;
      continue;
    }
  }
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape aItShellHoles( aMapShellHoles);
  for(; aItShellHoles.More();aItShellHoles.Next()) {
    if(aMapHoles.Contains(aItShellHoles.Key())) continue;
    const TopTools_ListOfShape& lHoles =aItShellHoles.Value();
    if(lHoles.IsEmpty()) continue;
    for(TopTools_ListIteratorOfListOfShape lItHoles(lHoles);lItHoles.More(); lItHoles.Next()) {
      if(aMapHoles.Contains(lItHoles.Value())) {
        const TopTools_ListOfShape& lUnHoles = aMapShellHoles.Find(lItHoles.Value());
        for(TopTools_ListIteratorOfListOfShape lItUnHoles(lUnHoles);lItUnHoles.More(); lItUnHoles.Next()) 
          aMapHoles.Remove(lItUnHoles.Value());
      }
    }
  }
  for(TopTools_MapIteratorOfMapOfShape aIterHoles(aMapHoles);aIterHoles.More(); aIterHoles.Next())
    aMapShellHoles.UnBind (aIterHoles.Key());

  for (Standard_Integer i = 1; i <= aSeqShells.Length(); ++i) {
    const TopoDS_Shape& aShell1 = aSeqShells.Value (i);
    if (aMapShellHoles.IsBound (aShell1)) {
      const TopTools_ListOfShape& ls = aMapShellHoles.Find (aShell1);
      anIndexedMapShellHoles.Add (aShell1, ls);
    }
  }
}
//=======================================================================
//function : CreateSolids
//purpose  : 
//=======================================================================

static Standard_Boolean CreateSolids(const TopoDS_Shape theShape,TopTools_IndexedMapOfShape& aMapSolids)
{
  TopTools_SequenceOfShape aSeqShells;
  Standard_Boolean isDone = Standard_False;

  for(TopExp_Explorer aExpShell(theShape,TopAbs_SHELL); aExpShell.More(); aExpShell.Next()) {
    aSeqShells.Append(aExpShell.Current());
  }
  TopTools_IndexedDataMapOfShapeListOfShape aMapShellHoles;
  TopTools_DataMapOfShapeInteger aMapStatus;
  CollectSolids(aSeqShells,aMapShellHoles,aMapStatus);
  TopTools_IndexedDataMapOfShapeShape ShellSolid;
  //Defines correct orientation of shells
  for (Standard_Integer i = 1; i <= aMapShellHoles.Extent(); ++i) {
    TopoDS_Shell aShell = TopoDS::Shell(aMapShellHoles.FindKey(i));
    TopExp_Explorer aExpEdges(aShell,TopAbs_EDGE);
    if(!BRep_Tool::IsClosed(aShell) || !aExpEdges.More()) {
      ShellSolid.Add(aShell,aShell);
      isDone = Standard_True;
      continue;
    }
    BRep_Builder B;
    TopAbs_State infinstatus = TopAbs_UNKNOWN;
    TopoDS_Solid aSolid;
    B.MakeSolid (aSolid);
    B.Add (aSolid,aShell);
    if(aMapStatus.IsBound(aShell)) { 
      Standard_Integer st = aMapStatus.Find(aShell);
      if(st)
        infinstatus =  (aMapStatus.Find(aShell) == 1 ? TopAbs_IN : TopAbs_OUT);
    }

    else {
      
    try {
      OCC_CATCH_SIGNALS
      BRepClass3d_SolidClassifier bsc3d (aSolid);
      bsc3d.PerformInfinitePoint(Precision::Confusion());
      infinstatus = bsc3d.State();
      }
    catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
      cout << "Warning: ShapeFix_Solid::SolidFromShell: Exception: ";
      anException.Print(cout); cout << endl;
#endif
      (void)anException;
      ShellSolid.Add(aShell,aSolid);
      continue;
    } 
  }
  if (infinstatus == TopAbs_IN) {
    isDone = Standard_True;
    aShell.Reverse();
    TopoDS_Solid aTmpSolid;
    B.MakeSolid (aTmpSolid);
    B.Add (aTmpSolid,aShell);
    aSolid = aTmpSolid;
  }
    
    const TopTools_ListOfShape& lHoles = aMapShellHoles (i);
    for(TopTools_ListIteratorOfListOfShape lItHoles(lHoles); lItHoles.More();lItHoles.Next()) {
      TopoDS_Shell aShellHole = TopoDS::Shell(lItHoles.Value());
      if(aMapStatus.IsBound(aShellHole)) {
        infinstatus = (aMapStatus.Find(aShellHole) == 1 ? TopAbs_IN : TopAbs_OUT);
      }
      else {
        TopoDS_Solid solid;
        B.MakeSolid (solid);
        B.Add (solid,aShellHole);
        BRepClass3d_SolidClassifier bsc3dHol (solid);
        bsc3dHol.PerformInfinitePoint(Precision::Confusion());
        infinstatus = bsc3dHol.State();
      }
      if (infinstatus == TopAbs_OUT) {
        aShellHole.Reverse();
        isDone = Standard_True;
      }
      B.Add(aSolid,aShellHole);
    }
    ShellSolid.Add(aShell,aSolid);
  }
  //Creation of compsolid from shells containing shared faces. 
  TopTools_IndexedDataMapOfShapeListOfShape aMapFaceShells;
  TopExp::MapShapesAndAncestors(theShape,TopAbs_FACE,TopAbs_SHELL,aMapFaceShells);
  for(Standard_Integer i =1; i <= aMapFaceShells.Extent(); i++) {
    const TopTools_ListOfShape& lshells = aMapFaceShells.FindFromIndex(i);
    if(lshells.Extent() <2) continue;
    TopoDS_CompSolid aCompSolid;
    BRep_Builder aB;
    aB.MakeCompSolid(aCompSolid);
    isDone = (theShape.ShapeType() != TopAbs_COMPSOLID || isDone);
    Standard_Integer nbSol = 0;

    for(TopTools_ListIteratorOfListOfShape lItSh(lshells);lItSh.More(); lItSh.Next()) {
      if(ShellSolid.Contains(lItSh.Value())) {
        const TopoDS_Shape& aShape = ShellSolid.FindFromKey(lItSh.Value());
        TopExp_Explorer aExpSol(aShape, TopAbs_SOLID);
       
        for(;aExpSol.More(); aExpSol.Next())
        {
          aB.Add(aCompSolid,aExpSol.Current());
          nbSol++;
        }
      
      }
    }
    if(nbSol >1)
    {
      for(TopTools_ListIteratorOfListOfShape lItSh1(lshells);lItSh1.More(); lItSh1.Next()) 
      {
        if(ShellSolid.Contains(lItSh1.Value())) 
          ShellSolid.ChangeFromKey(lItSh1.Value()) = aCompSolid;
      }
    }
    
  }
  for(Standard_Integer kk =1 ; kk <= ShellSolid.Extent();kk++)
    if(!aMapSolids.Contains(ShellSolid.FindFromIndex(kk)))
       aMapSolids.Add(ShellSolid.FindFromIndex(kk));
  isDone = (aMapSolids.Extent() >1 || isDone);
  return isDone;
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_Solid::Perform(const Handle(Message_ProgressIndicator)& theProgress) 
{

  Standard_Boolean status = Standard_False;
  if ( Context().IsNull() )
    SetContext ( new ShapeBuild_ReShape );
  myFixShell->SetContext(Context());

  Standard_Integer NbShells = 0;
  TopoDS_Shape S = Context()->Apply ( myShape );

  // Calculate number of underlying shells
  Standard_Integer aNbShells = 0;
  for ( TopExp_Explorer aExpSh(S, TopAbs_SHELL); aExpSh.More(); aExpSh.Next() )
    aNbShells++;

  // Start progress scope (no need to check if progress exists -- it is safe)
  Message_ProgressSentry aPSentry(theProgress, "Fixing solid stage", 0, 2, 1);

  if ( NeedFix(myFixShellMode) )
  {
    // Start progress scope (no need to check if progress exists -- it is safe)
    Message_ProgressSentry aPSentryFixShell(theProgress, "Fixing shell", 0, aNbShells, 1);

    // Fix shell by shell using ShapeFix_Shell tool
    for ( TopExp_Explorer aExpSh(S, TopAbs_SHELL); aExpSh.More() && aPSentryFixShell.More(); aExpSh.Next(), aPSentryFixShell.Next() )
    { 
      TopoDS_Shape sh = aExpSh.Current();

      myFixShell->Init( TopoDS::Shell(sh) );
      if ( myFixShell->Perform(theProgress) )
      {
        status = Standard_True;
        myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
      }
      NbShells += myFixShell->NbShells();
    }

    // Halt algorithm in case of user's abort
    if ( !aPSentryFixShell.More() )
      return Standard_False;
    }
  else 
  {
    NbShells = aNbShells;
  }

  // Switch to the second stage
  aPSentry.Next();

  if (!NeedFix(myFixShellOrientationMode))
  {
    myShape = Context()->Apply(myShape);
    return status;
  }

  if(NbShells ==1) {
    TopoDS_Shape tmpShape = Context()->Apply(myShape);
    TopExp_Explorer aExp(tmpShape,TopAbs_SHELL);
    Standard_Boolean isClosed = Standard_False;
    if(aExp.More()) {
      TopoDS_Shell  aShtmp = TopoDS::Shell(aExp.Current());
      ShapeAnalysis_FreeBounds sfb(aShtmp);
      TopoDS_Compound aC1 = sfb.GetClosedWires();
      TopoDS_Compound aC2 = sfb.GetOpenWires();
      Standard_Integer numedge =0;
      TopExp_Explorer aExp1(aC1,TopAbs_EDGE); 
      for( ; aExp1.More(); aExp1.Next())
        numedge++;
      for(aExp1.Init(aC2,TopAbs_EDGE) ; aExp1.More(); aExp1.Next())
        numedge++;
      isClosed = (!numedge);
      aShtmp.Closed(isClosed);
    }
      
    if(isClosed || myCreateOpenSolidMode) {
      TopoDS_Iterator itersh(tmpShape);
      TopoDS_Shell aShell;
      if(itersh.More() && itersh.Value().ShapeType() == TopAbs_SHELL)
        aShell = TopoDS::Shell(itersh.Value());
      if(!aShell.IsNull()) {
        TopoDS_Solid aSol = SolidFromShell(aShell);
        if(ShapeExtend::DecodeStatus(myStatus,ShapeExtend_DONE2)) {
          SendWarning (Message_Msg ("FixAdvSolid.FixOrientation.MSG20"));// Orientaion of shell was corrected.
          Context()->Replace(tmpShape,aSol);
          tmpShape = aSol;
        }
      }
      mySolid  = TopoDS::Solid(tmpShape);
    }
    else {
      status = Standard_True;
      myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE3 );
      TopoDS_Iterator aIt(tmpShape,Standard_False);
      Context()->Replace(tmpShape,aIt.Value());
      SendFail (Message_Msg ("FixAdvSolid.FixShell.MSG10")); // Solid can not be created from open shell. 
    }
  }
  else {
    TopoDS_Shape aResShape = Context()->Apply(myShape);
    TopTools_SequenceOfShape aSeqShells;
    TopTools_IndexedMapOfShape aMapSolids;
    if(CreateSolids(aResShape,aMapSolids)) {
      SendWarning (Message_Msg ("FixAdvSolid.FixOrientation.MSG20"));// Orientaion of shell was corrected.. 
      if(aMapSolids.Extent() ==1) {
        TopoDS_Shape aResSol = aMapSolids.FindKey(1);
        if(aResShape.ShapeType() == TopAbs_SHELL && myCreateOpenSolidMode) {
          TopoDS_Solid solid;
          BRep_Builder B;
          B.MakeSolid (solid);
          B.Add (solid,aResSol);
          mySolid = solid;
        }
        else {
          mySolid =aResSol;
          if(aResSol.ShapeType() == TopAbs_SHELL)
            SendFail (Message_Msg ("FixAdvSolid.FixShell.MSG10")); // Solid can not be created from open shell. 
        }
        Context()->Replace(aResShape,mySolid);
      }
      else if(aMapSolids.Extent() >1) {
        SendWarning (Message_Msg ("FixAdvSolid.FixOrientation.MSG30"));// Bad connected solid a few solids were created.
        BRep_Builder aB;
        TopoDS_Compound aComp;
        aB.MakeCompound(aComp);
        Message_ProgressSentry aPSentryCreatingSolid(theProgress, "Creating solid",
                                        0, aMapSolids.Extent(), 1);
        for(Standard_Integer i =1; (i <= aMapSolids.Extent()) && (aPSentryCreatingSolid.More());
            i++, aPSentryCreatingSolid.Next())
        {
          TopoDS_Shape aResSh =aMapSolids.FindKey(i);
          if(aResShape.ShapeType() == TopAbs_SHELL && myCreateOpenSolidMode) {
            aResSh.Closed(Standard_False);
            TopoDS_Solid solid;
            BRep_Builder B;
            B.MakeSolid (solid);
            B.Add (solid,aResSh);
            aResSh = solid;
          }
          else if (aResShape.ShapeType() == TopAbs_SHELL)
            SendFail(Message_Msg ("FixAdvSolid.FixShell.MSG10")); // Solid can not be created from open shell. 
          aB.Add(aComp,aResSh);
          
        }
        if ( !aPSentryCreatingSolid.More() )
          return Standard_False; // aborted execution
        Context()->Replace(aResShape,aComp);
      }
    }
  }
  myShape = Context()->Apply(myShape);
  return status;
}
//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape ShapeFix_Solid::Shape() 
{
 return myShape; 
}
//=======================================================================
//function : SolidFromShell
//purpose  : 
//=======================================================================

TopoDS_Solid ShapeFix_Solid::SolidFromShell (const TopoDS_Shell& shell) 
{
  TopoDS_Shell sh = shell;
  if (!sh.Free ()) sh.Free(Standard_True);

  TopoDS_Solid solid;
  BRep_Builder B;
  B.MakeSolid (solid);
  B.Add (solid,sh);
//   Pas encore fini : il faut une bonne orientation
  try {
    OCC_CATCH_SIGNALS
    BRepClass3d_SolidClassifier bsc3d (solid);
    Standard_Real t = Precision::Confusion();    // tolerance moyenne
    bsc3d.PerformInfinitePoint(t);
    
    if (bsc3d.State() == TopAbs_IN) {
      //         Ensuite, inverser C-A-D REPRENDRE LES SHELLS
      //         (l inversion du solide n est pas bien prise en compte)
      sh = shell;
      if (!sh.Free ()) sh.Free(Standard_True);
      TopoDS_Solid soli2;
      B.MakeSolid (soli2);    // on recommence
      sh.Reverse();
      B.Add (soli2,sh);
      solid = soli2;
      myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE2 );
    }
  }
  catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
    cout << "Warning: ShapeFix_Solid::SolidFromShell: Exception: ";
    anException.Print(cout); cout << endl;
#endif
    (void)anException;
    return solid;
  }
  return solid;
}

//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_Solid::Status (const ShapeExtend_Status theStatus) const
{
  return ShapeExtend::DecodeStatus (myStatus, theStatus);
}

//=======================================================================
//function : Solid
//purpose  : 
//=======================================================================

 TopoDS_Shape ShapeFix_Solid::Solid() const
{
 return mySolid;
}

//=======================================================================
//function : SetMsgRegistrator
//purpose  : 
//=======================================================================

void ShapeFix_Solid::SetMsgRegistrator(const Handle(ShapeExtend_BasicMsgRegistrator)& msgreg)
{
  ShapeFix_Root::SetMsgRegistrator ( msgreg );
  myFixShell->SetMsgRegistrator ( msgreg );
}

//=======================================================================
//function : SetPrecision
//purpose  : 
//=======================================================================

void ShapeFix_Solid::SetPrecision (const Standard_Real preci) 
{
  ShapeFix_Root::SetPrecision ( preci );
  myFixShell->SetPrecision ( preci );
}

//=======================================================================
//function : SetMinTolerance
//purpose  : 
//=======================================================================

void ShapeFix_Solid::SetMinTolerance (const Standard_Real mintol) 
{
  ShapeFix_Root::SetMinTolerance ( mintol );
  myFixShell->SetMinTolerance ( mintol );
}

//=======================================================================
//function : SetMaxTolerance
//purpose  : 
//=======================================================================

void ShapeFix_Solid::SetMaxTolerance (const Standard_Real maxtol) 
{
  ShapeFix_Root::SetMaxTolerance ( maxtol );
  myFixShell->SetMaxTolerance ( maxtol );
}
