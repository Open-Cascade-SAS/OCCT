// Created on: 1999-06-22
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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



#include <ShapeProcessAPI_ApplySequence.ixx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Resource_Manager.hxx>
#include <ShapeProcess.hxx>
#include <ShapeProcess_OperLibrary.hxx>

//=======================================================================
//function : ShapeProcessAPI_ApplySequence
//purpose  : 
//=======================================================================

ShapeProcessAPI_ApplySequence::ShapeProcessAPI_ApplySequence(const Standard_CString rscName, 
							     const Standard_CString seqName)
{

  myContext = new ShapeProcess_ShapeContext(rscName);
  myContext->SetDetalisation ( TopAbs_FACE );
  TCollection_AsciiString str ( seqName );

  // initialize operators
  ShapeProcess_OperLibrary::Init ();
  
  mySeq = str;
}

//=======================================================================
//function : Context
//purpose  : 
//=======================================================================

Handle(ShapeProcess_ShapeContext)& ShapeProcessAPI_ApplySequence::Context()
{
  return myContext;
}

//=======================================================================
//function : PrepareShape
//purpose  : 
//=======================================================================

TopoDS_Shape ShapeProcessAPI_ApplySequence::PrepareShape(const TopoDS_Shape& shape,
                                                         const Standard_Boolean /*fillmap*/,
                                                         const TopAbs_ShapeEnum /*until*/)
{
  Handle(Resource_Manager) rsc = myContext->ResourceManager();
  myContext->Init(shape);
  
  TCollection_AsciiString str(mySeq);
  str += ".exec.op";
  if ( rsc->Find ( str.ToCString() ) ) {
    ShapeProcess::Perform ( myContext, mySeq.ToCString() );
  }
  
  return myContext->Result();
}

//=======================================================================
//function : ClearMap
//purpose  : 
//=======================================================================

 void ShapeProcessAPI_ApplySequence::ClearMap() 
{
  //myContext->Map().Clear();
}

//=======================================================================
//function : Map
//purpose  : 
//=======================================================================

const TopTools_DataMapOfShapeShape& ShapeProcessAPI_ApplySequence::Map() const
{
  return myContext->Map();
}

//=======================================================================
//function : PrintPreparationResult
//purpose  : 
//=======================================================================

void ShapeProcessAPI_ApplySequence::PrintPreparationResult () const
{
  Standard_Integer SS = 0, SN = 0, FF = 0, FS = 0, FN = 0;
  for (TopTools_DataMapIteratorOfDataMapOfShapeShape It (myContext->Map()); It.More(); It.Next()) {
    TopoDS_Shape keyshape = It.Key(), valueshape = It.Value();
    if (keyshape.ShapeType() == TopAbs_SHELL)
      if (valueshape.IsNull()) SN++;
      else SS++;
    else if (keyshape.ShapeType() == TopAbs_FACE)
      if (valueshape.IsNull()) FN++;
      else if (valueshape.ShapeType() == TopAbs_SHELL) FS++;
      else FF++;
  }
  
  Handle(Message_Messenger) aMessenger = myContext->Messenger();

  // mapping
  Message_Msg EPMSG100 ("PrResult.Print.MSG100"); //Mapping:
  aMessenger->Send (EPMSG100, Message_Info, Standard_True);
  Message_Msg TPMSG50 ("PrResult.Print.MSG50"); //  Shells:
  aMessenger->Send (TPMSG50, Message_Info, Standard_True);
  Message_Msg EPMSG110 ("PrResult.Print.MSG110"); //    Result is Shell                 : %d
  EPMSG110.Arg (SS);
  aMessenger->Send (EPMSG110, Message_Info, Standard_True);
  Message_Msg EPMSG150 ("PrResult.Print.MSG150"); //    No Result                       : %d
  EPMSG150.Arg (SN);
  aMessenger->Send (EPMSG150, Message_Info, Standard_True);
  
  TCollection_AsciiString tmp110 (EPMSG110.Original()), tmp150  (EPMSG150.Original());
  EPMSG110.Set (tmp110.ToCString());
  EPMSG150.Set (tmp150.ToCString());

  Message_Msg TPMSG55 ("PrResult.Print.MSG55"); //  Faces:
  aMessenger->Send (TPMSG55, Message_Info, Standard_True);
  Message_Msg EPMSG115 ("PrResult.Print.MSG115"); //    Result is Face                  : %d
  EPMSG115.Arg (FF);
  aMessenger->Send (EPMSG115, Message_Info, Standard_True);
  EPMSG110.Arg (FS);
  aMessenger->Send (EPMSG110, Message_Info, Standard_True);
  EPMSG150.Arg (FN);
  aMessenger->Send (EPMSG150, Message_Info, Standard_True);
  
  // preparation ratio
  Standard_Real SPR = 1, FPR = 1;
  Standard_Integer STotalR = SS, FTotalR  = FF + FS;
  Standard_Integer NbS = STotalR + SN, NbF = FTotalR + FN;
  if (NbS > 0) SPR = 1. * (NbS - SN) / NbS;
  if (NbF > 0) FPR = 1. * (NbF - FN) / NbF;
  Message_Msg PMSG200 ("PrResult.Print.MSG200"); //Preparation ratio:
  aMessenger->Send (PMSG200, Message_Info, Standard_True);
  Message_Msg PMSG205 ("PrResult.Print.MSG205"); //  Shells: %d per cent
  PMSG205.Arg ((Standard_Integer) (100 * SPR));
  aMessenger->Send (PMSG205, Message_Info, Standard_True);
  Message_Msg PMSG210 ("PrResult.Print.MSG210"); //  Faces : %d per cent
  PMSG210.Arg ((Standard_Integer) (100 * FPR));
  aMessenger->Send (PMSG210, Message_Info, Standard_True);
}

