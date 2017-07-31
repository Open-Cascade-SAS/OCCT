// Created on: 2000-08-22
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <BRep_Builder.hxx>
#include <BRepTools_Modifier.hxx>
#include <Message_ListIteratorOfListOfMsg.hxx>
#include <Message_ListOfMsg.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend_DataMapOfShapeListOfMsg.hxx>
#include <ShapeExtend_MsgRegistrator.hxx>
#include <ShapeProcess_ShapeContext.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeProcess_ShapeContext,ShapeProcess_Context)

//=======================================================================
//function : ShapeProcess_ShapeContext
//purpose  : 
//=======================================================================
ShapeProcess_ShapeContext::ShapeProcess_ShapeContext (const Standard_CString file,
                                                      const Standard_CString seq) 
     : ShapeProcess_Context ( file, seq ), myUntil(TopAbs_FACE),
       myNonManifold(Standard_False)
{
}

//=======================================================================
//function : ShapeProcess_ShapeContext
//purpose  : 
//=======================================================================

ShapeProcess_ShapeContext::ShapeProcess_ShapeContext (const TopoDS_Shape &S,
                                                      const Standard_CString file,
                                                      const Standard_CString seq) 
     : ShapeProcess_Context ( file, seq ), myUntil(TopAbs_FACE),
       myNonManifold(Standard_False)
{
  Init ( S );
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShapeProcess_ShapeContext::Init (const TopoDS_Shape &S)
{
  myMap.Clear();
  myMsg = new ShapeExtend_MsgRegistrator;
  myShape = S;
  myResult = myShape;//.Nullify();
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape &ShapeProcess_ShapeContext::Shape () const
{
  return myShape;
}

//=======================================================================
//function : Result
//purpose  : 
//=======================================================================

const TopoDS_Shape &ShapeProcess_ShapeContext::Result () const
{
  return myResult;
}

//=======================================================================
//function : Map
//purpose  : 
//=======================================================================

const TopTools_DataMapOfShapeShape &ShapeProcess_ShapeContext::Map () const
{
  return myMap;
}

//=======================================================================
//function : Messages
//purpose  : 
//=======================================================================

Handle(ShapeExtend_MsgRegistrator) &ShapeProcess_ShapeContext::Messages () 
{
  return myMsg;
}

//=======================================================================
//function : Messages
//purpose  : 
//=======================================================================

const Handle(ShapeExtend_MsgRegistrator) &ShapeProcess_ShapeContext::Messages () const
{
  return myMsg;
}

//=======================================================================
//function : SetDetalisation
//purpose  : 
//=======================================================================

void ShapeProcess_ShapeContext::SetDetalisation (const TopAbs_ShapeEnum level)
{
  myUntil = level;
}

//=======================================================================
//function : GetDetalisation
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum ShapeProcess_ShapeContext::GetDetalisation () const
{
  return myUntil;
}
       
//=======================================================================
//function : SetResult
//purpose  : 
//=======================================================================

void ShapeProcess_ShapeContext::SetResult (const TopoDS_Shape &res) 
{
  myResult = res;
  myMap.Bind ( myShape, myResult );
}

//=======================================================================
//function : RecordModification
//purpose  : 
//=======================================================================

/*
#ifdef OCCT_DEBUG
static void DumpMap (const TopTools_DataMapOfShapeShape &map)
{
  cout << "----" << endl;
  cout << "Map:" << endl;
  for (TopTools_DataMapIteratorOfDataMapOfShapeShape It (map); It.More(); It.Next()) {
    TopoDS_Shape S0 = It.Key(), S = It.Value();
    cout << S0.TShape()->DynamicType()->Name() << "\t" << *(void**)&S0.TShape() << 
      "     \t->    " << S.TShape()->DynamicType()->Name() << "\t" << *(void**)&S.TShape() << endl;
  }
  cout << "----" << endl;
}	     
#endif
*/

static void RecModif (const TopoDS_Shape &S, 
		      const TopTools_DataMapOfShapeShape &repl,
		      const Handle(ShapeExtend_MsgRegistrator) &msg,
		      TopTools_DataMapOfShapeShape &map,
		      Handle(ShapeExtend_MsgRegistrator) &myMsg,
		      const TopAbs_ShapeEnum until)
{
  TopoDS_Shape r = S;
  //gka  -modification to keep history for shape with location (OCC21617)
  TopLoc_Location aShLoc = S.Location();
  TopLoc_Location aNullLoc;
  r.Location(aNullLoc);

  if ( map.IsBound ( r ) ) 
    r = map.Find ( r );
  if ( ! r.IsNull() ) {
    TopoDS_Shape res = r;
    
    if ( repl.IsBound ( r.Located(aShLoc) ) ) {
      res = repl.Find (  r.Located(aShLoc) );
      // it is supposed that map is created for r having FORWARD orientation
      // hence, if it is reversed, result should be reversed too
      // INTERNAL or EXTERNAL orientations are not allowed
      if ( r.Orientation() != TopAbs_FORWARD ) 
	res.Reverse();
     
    }
    // Treat special case: if S was split, r will be a compound of
    // resulting shapes, each to be checked separately
    // It is supposed that repl does not contain such splitting
    else if ( r.ShapeType() < S.ShapeType() ) {
      TopoDS_Shape result = r.EmptyCopied();
      result.Orientation(TopAbs_FORWARD); // protect against INTERNAL or EXTERNAL shapes
      Standard_Boolean modif = Standard_False;
      BRep_Builder B;
      for ( TopoDS_Iterator it(r,Standard_False); it.More(); it.Next() ) {
	TopoDS_Shape sh = it.Value();
	if ( repl.IsBound(sh) ) {
	  TopoDS_Shape newsh = repl.Find(sh);
	  if ( ! newsh.IsNull() ) B.Add ( result, newsh );
	  modif = Standard_True;
	}
	else B.Add ( result, sh );
      }
      if ( modif )
      {
        if (result.ShapeType() == TopAbs_WIRE || result.ShapeType() == TopAbs_SHELL)
          result.Closed (BRep_Tool::IsClosed (result));
        res = result;
      }
    }
    
    if ( res != r ) map.Bind ( S.Located(aNullLoc), res );
  }
  
  // update messages (messages must be taken from each level in the substitution map)
  if ( ! r.IsNull() && ! myMsg.IsNull() && 
       ! msg.IsNull() && msg->MapShape().Extent() >0 )
  {
    const ShapeExtend_DataMapOfShapeListOfMsg& msgmap = msg->MapShape();
    if ( msgmap.IsBound( r )) {
      const Message_ListOfMsg &msglist = msgmap.Find (r);
      for (Message_ListIteratorOfListOfMsg iter (msglist); iter.More(); iter.Next())
        myMsg->Send ( S, iter.Value(), Message_Warning );
    }
    else if ( msgmap.IsBound( S )) {
      const Message_ListOfMsg &msglist = msgmap.Find (S);
      for (Message_ListIteratorOfListOfMsg iter (msglist); iter.More(); iter.Next())
        myMsg->Send ( S, iter.Value(), Message_Warning );
    }
  }

  if ( until == TopAbs_SHAPE || S.ShapeType() >= until ) return;

  for ( TopoDS_Iterator it(S); it.More(); it.Next() ) {
    RecModif ( it.Value(), repl, msg, map, myMsg, until );
  }
}

void ShapeProcess_ShapeContext::RecordModification (const TopTools_DataMapOfShapeShape &repl,
                                                    const Handle(ShapeExtend_MsgRegistrator)& msg)
{
  if ( repl.Extent() <=0 ) return;
  RecModif ( myShape, repl, msg, myMap, myMsg, myUntil );
  if ( myMap.IsBound(myShape) ) myResult = myMap.Find ( myShape );
#ifdef OCCT_DEBUG
//  cout << "Modifier: " << endl; DumpMap (myMap);
#endif
}

//=======================================================================
//function : RecordModification
//purpose  : 
//=======================================================================

static void RecModif (const TopoDS_Shape &S, 
		      const Handle(ShapeBuild_ReShape) &repl,
		      const Handle(ShapeExtend_MsgRegistrator) &msg,
		      TopTools_DataMapOfShapeShape &map,
		      Handle(ShapeExtend_MsgRegistrator) &myMsg,
		      const TopAbs_ShapeEnum until)
{
  if(S.IsNull())
    return;
  //gka  -modification to keep history for shape with location (OCC21617)
  TopLoc_Location aNullLoc;
  TopoDS_Shape aS = S.Located(aNullLoc);
  TopoDS_Shape r = aS;
 
  if ( map.IsBound ( r ) ) 
    r = map.Find ( r );
  if ( ! r.IsNull() ) {
    TopoDS_Shape res;
    if ( repl->Status (r, res, Standard_True ) && res != r ) 
      map.Bind ( aS, res );
    
    // Treat special case: if S was split, r will be a compound of
    // resulting shapes, recursive procedure should be applied
    else if ( r.ShapeType() < S.ShapeType() ) {
      res = repl->Apply ( r, (TopAbs_ShapeEnum)((Standard_Integer)S.ShapeType()+1) );
      if ( res != r ) map.Bind ( aS, res );
    }
  }

  // update messages (messages must be taken from each level in the substitution map)
  if ( ! r.IsNull() && ! myMsg.IsNull() && 
       ! msg.IsNull() && msg->MapShape().Extent() >0 ) {
    TopoDS_Shape cur, next = r;
    const ShapeExtend_DataMapOfShapeListOfMsg& msgmap = msg->MapShape();
    if ( msgmap.IsBound( S ))
      next = S;
    do {
      cur = next;
      if (msgmap.IsBound (cur)) {
	const Message_ListOfMsg &msglist = msgmap.Find (cur);
	for (Message_ListIteratorOfListOfMsg iter (msglist); iter.More(); iter.Next()) {
	  myMsg->Send ( S, iter.Value(), Message_Warning );
	}
      }
      next = repl->Value (cur);
    } while ( ! next.IsNull() && cur != next);
  }

  if ( until == TopAbs_SHAPE || S.ShapeType() >= until ) return;
  
  for ( TopoDS_Iterator it(S,Standard_False/*,Standard_False*/); it.More(); it.Next() ) {
    RecModif ( it.Value(), repl, msg, map, myMsg, until );
  }
}

void ShapeProcess_ShapeContext::RecordModification (const Handle(ShapeBuild_ReShape) &repl,
                                                    const Handle(ShapeExtend_MsgRegistrator) &msg)
{
  
  RecModif ( myShape, repl, msg, myMap, myMsg, myUntil );
  if ( myMap.IsBound(myShape) ) 
  {
    myResult = myMap.Find ( myShape );
    myResult.Location(myShape.Location());
  }
#ifdef OCCT_DEBUG
//  cout << "ReShape: " << endl; DumpMap (myMap);
#endif
}

//=======================================================================
//function : RecordModification
//purpose  : 
//=======================================================================

void ShapeProcess_ShapeContext::RecordModification (const Handle(ShapeBuild_ReShape) &repl)
{
  Handle(ShapeExtend_MsgRegistrator) msg;
  RecordModification ( repl, msg );
}

//=======================================================================
//function : AddMessage
//purpose  : 
//=======================================================================

void ShapeProcess_ShapeContext::AddMessage (const TopoDS_Shape &S,
                                            const Message_Msg &msg,
                                            const Message_Gravity grv) 
{
  if ( ! myMsg.IsNull() ) myMsg->Send ( S, msg, grv );
}

//=======================================================================
//function : RecordModification
//purpose  : 
//=======================================================================

static void ExplodeModifier (const TopoDS_Shape &S, 
			     const BRepTools_Modifier &repl,
			     TopTools_DataMapOfShapeShape &map,
			     const TopAbs_ShapeEnum until)
{
  TopoDS_Shape res = repl.ModifiedShape ( S );
  
  if ( res != S ) 
  {
   map.Bind ( S, res );
 }
  if ( until == TopAbs_SHAPE || S.ShapeType() >= until ) return;
  for ( TopoDS_Iterator it(S); it.More(); it.Next() ) {
    ExplodeModifier ( it.Value(), repl, map, until );
  }
}

void ShapeProcess_ShapeContext::RecordModification (const TopoDS_Shape &S, 
                                                    const BRepTools_Modifier &repl,
                                                    const Handle(ShapeExtend_MsgRegistrator)& msg)
{
  TopTools_DataMapOfShapeShape map;
  ExplodeModifier ( S, repl, map, myUntil );
  RecordModification ( map, msg );
}

//=======================================================================
//function : GetContinuity
//purpose  : 
//=======================================================================

Standard_Boolean ShapeProcess_ShapeContext::GetContinuity (const Standard_CString param,
                                                           GeomAbs_Shape &cont) const
{
  TCollection_AsciiString str;
  if ( ! GetString ( param, str ) ) return Standard_False;
  
  str.LeftAdjust();
  str.RightAdjust();
  str.UpperCase();
  
  if      ( str.IsEqual ( "C0" ) ) cont = GeomAbs_C0;
  else if ( str.IsEqual ( "G1" ) ) cont = GeomAbs_G1;
  else if ( str.IsEqual ( "C1" ) ) cont = GeomAbs_C1;
  else if ( str.IsEqual ( "G2" ) ) cont = GeomAbs_G2;
  else if ( str.IsEqual ( "C2" ) ) cont = GeomAbs_C2;
  else if ( str.IsEqual ( "C3" ) ) cont = GeomAbs_C3;
  else if ( str.IsEqual ( "CN" ) ) cont = GeomAbs_CN;
  else return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : ContinuityVal
//purpose  : 
//=======================================================================

GeomAbs_Shape ShapeProcess_ShapeContext::ContinuityVal (const Standard_CString param,
                                                        const GeomAbs_Shape def) const
{
  GeomAbs_Shape val;
  return GetContinuity ( param, val ) ? val : def;
}

//=======================================================================
//function : PrintStatistics
//purpose  : 
//=======================================================================

void ShapeProcess_ShapeContext::PrintStatistics () const
{
  Standard_Integer SS = 0, SN = 0, FF = 0, FS = 0, FN = 0;
  for (TopTools_DataMapIteratorOfDataMapOfShapeShape It (myMap); It.More(); It.Next()) {
    TopoDS_Shape keyshape = It.Key(), valueshape = It.Value();
    if (keyshape.ShapeType() == TopAbs_SHELL)
      if (valueshape.IsNull()) SN++;
      else SS++;
    else if (keyshape.ShapeType() == TopAbs_FACE)
    {
      if (valueshape.IsNull()) FN++;
      else if (valueshape.ShapeType() == TopAbs_SHELL) FS++;
      else FF++;
    }
  }
  
  // mapping
  Message_Msg EPMSG100 ("PrResult.Print.MSG100"); //Mapping:
  Messenger()->Send (EPMSG100, Message_Info);
  Message_Msg TPMSG50 ("PrResult.Print.MSG50"); //  Shells:
  Messenger()->Send (TPMSG50, Message_Info);
  Message_Msg EPMSG110 ("PrResult.Print.MSG110"); //    Result is Shell                 : %d
  EPMSG110.Arg (SS);
  Messenger()->Send (EPMSG110, Message_Info);
  Message_Msg EPMSG150 ("PrResult.Print.MSG150"); //    No Result                       : %d
  EPMSG150.Arg (SN);
  Messenger()->Send (EPMSG150.Get(), Message_Info);
  
  TCollection_AsciiString tmp110 (EPMSG110.Original()), tmp150  (EPMSG150.Original());
  EPMSG110.Set (tmp110.ToCString());
  EPMSG150.Set (tmp150.ToCString());

  Message_Msg TPMSG55 ("PrResult.Print.MSG55"); //  Faces:
  Messenger()->Send (TPMSG55, Message_Info);
  Message_Msg EPMSG115 ("PrResult.Print.MSG115"); //    Result is Face                  : %d
  EPMSG115.Arg (FF);
  Messenger()->Send (EPMSG115, Message_Info);
  EPMSG110.Arg (FS);
  Messenger()->Send (EPMSG110, Message_Info);
  EPMSG150.Arg (FN);
  Messenger()->Send (EPMSG150, Message_Info);
  
  // preparation ratio
  Standard_Real SPR = 1, FPR = 1;
  Standard_Integer STotalR = SS, FTotalR  = FF + FS;
  Standard_Integer NbS = STotalR + SN, NbF = FTotalR + FN;
  if (NbS > 0) SPR = 1. * (NbS - SN) / NbS;
  if (NbF > 0) FPR = 1. * (NbF - FN) / NbF;
  Message_Msg PMSG200 ("PrResult.Print.MSG200"); //Preparation ratio:
  Messenger()->Send (PMSG200, Message_Info);
  Message_Msg PMSG205 ("PrResult.Print.MSG205"); //  Shells: %d per cent
  PMSG205.Arg ((Standard_Integer) (100. * SPR));
  Messenger()->Send (PMSG205, Message_Info);
  Message_Msg PMSG210 ("PrResult.Print.MSG210"); //  Faces : %d per cent
  PMSG210.Arg ((Standard_Integer) (100. * FPR));
  Messenger()->Send (PMSG210, Message_Info);
}
