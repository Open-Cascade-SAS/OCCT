// Created on: 2000-01-19
// Created by: data exchange team
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

#include <BRepTools_ReShape.hxx>
#include <Interface_Static.hxx>
#include <Resource_Manager.hxx>
#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <ShapeAlgo_ToolContainer.hxx>
#include <ShapeExtend_MsgRegistrator.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeProcess_ShapeContext.hxx>
#include <TopExp_Explorer.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_TransientListBinder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeBinder.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <XSAlgo_AlgoContainer.hxx>
#include <XSAlgo_ShapeProcessor.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XSAlgo_AlgoContainer, Standard_Transient)

//=================================================================================================

void XSAlgo_AlgoContainer::PrepareForTransfer() const
{
  XSAlgo_ShapeProcessor::PrepareForTransfer();
}

//=================================================================================================

TopoDS_Shape XSAlgo_AlgoContainer::ProcessShape(const TopoDS_Shape&          theShape,
                                                const Standard_Real          thePrec,
                                                const Standard_Real          theMaxTol,
                                                const Standard_CString       thePrscfile,
                                                const Standard_CString       thePseq,
                                                Handle(Standard_Transient)&  theInfo,
                                                const Message_ProgressRange& theProgress,
                                                const Standard_Boolean       theNonManifold,
                                                const TopAbs_ShapeEnum theDetailingLevel) const
{
  if (theShape.IsNull())
  {
    return theShape;
  }

  Handle(ShapeProcess_ShapeContext) aContext = Handle(ShapeProcess_ShapeContext)::DownCast(theInfo);
  if (aContext.IsNull())
  {
    Standard_CString aRscfile = Interface_Static::CVal(thePrscfile);
    aContext                  = new ShapeProcess_ShapeContext(theShape, aRscfile);
    if (!aContext->ResourceManager()->IsInitialized())
    {
      // If resource file wasn't found, use static values instead
      Interface_Static::FillMap(aContext->ResourceManager()->GetMap());
    }
    aContext->SetDetalisation(theDetailingLevel);
  }
  aContext->SetNonManifold(theNonManifold);
  theInfo = aContext;

  Standard_CString aSeq = Interface_Static::CVal(thePseq);
  if (!aSeq)
    aSeq = thePseq;

  // if resource file is not loaded or does not define <seq>.exec.op,
  // do default fixes
  Handle(Resource_Manager) aRsc = aContext->ResourceManager();
  TCollection_AsciiString  aStr(aSeq);
  aStr += ".exec.op";
  if (!aRsc->Find(aStr.ToCString()))
  {
    // if reading, do default ShapeFix
    if (!strncmp(thePseq, "read.", 5))
    {
      try
      {
        OCC_CATCH_SIGNALS
        Handle(ShapeExtend_MsgRegistrator) aMsg = new ShapeExtend_MsgRegistrator;
        Handle(ShapeFix_Shape) aSfs = ShapeAlgo::AlgoContainer()->ToolContainer()->FixShape();
        aSfs->Init(theShape);
        aSfs->SetMsgRegistrator(aMsg);
        aSfs->SetPrecision(thePrec);
        aSfs->SetMaxTolerance(theMaxTol);
        aSfs->FixFaceTool()->FixWireTool()->FixSameParameterMode() = Standard_False;
        aSfs->FixSolidTool()->CreateOpenSolidMode()                = Standard_False;
        aSfs->Perform(theProgress);

        TopoDS_Shape aShape = aSfs->Shape();
        if (!aShape.IsNull() && aShape != theShape)
        {
          aContext->RecordModification(aSfs->Context(), aMsg);
          aContext->SetResult(aShape);
        }
      }
      catch (Standard_Failure const& anException)
      {
        (void)anException;
      }
      return aContext->Result();
    }
    // for writing, define default sequence of DirectFaces
    else if (!strncmp(thePseq, "write.", 6))
    {
      aRsc->SetResource(aStr.ToCString(), "DirectFaces");
    }
  }

  // Define runtime tolerances and do Shape Processing
  aRsc->SetResource("Runtime.Tolerance", thePrec);
  aRsc->SetResource("Runtime.MaxTolerance", theMaxTol);

  if (!ShapeProcess::Perform(aContext, aSeq, theProgress))
    return theShape; // return original shape

  return aContext->Result();
}

//=================================================================================================

Standard_Boolean XSAlgo_AlgoContainer::CheckPCurve(const TopoDS_Edge&     theEdge,
                                                   const TopoDS_Face&     theFace,
                                                   const Standard_Real    thePrecision,
                                                   const Standard_Boolean theIsSeam) const
{
  return XSAlgo_ShapeProcessor::CheckPCurve(theEdge, theFace, thePrecision, theIsSeam);
}

//=================================================================================================

void XSAlgo_AlgoContainer::MergeTransferInfo(const Handle(Transfer_TransientProcess)& TP,
                                             const Handle(Standard_Transient)&        info,
                                             const Standard_Integer startTPitem) const
{
  Handle(ShapeProcess_ShapeContext) context = Handle(ShapeProcess_ShapeContext)::DownCast(info);
  if (context.IsNull())
    return;

  const TopTools_DataMapOfShapeShape& map = context->Map();
  Handle(ShapeExtend_MsgRegistrator)  msg = context->Messages();
  if (map.Extent() <= 0 && (msg.IsNull() || msg->MapShape().Extent() <= 0))
    return;

  Standard_Integer i = (startTPitem > 0 ? startTPitem : 1);
  for (; i <= TP->NbMapped(); i++)
  {
    Handle(Transfer_Binder)          bnd = TP->MapItem(i);
    Handle(TransferBRep_ShapeBinder) sb  = Handle(TransferBRep_ShapeBinder)::DownCast(bnd);
    if (sb.IsNull() || sb->Result().IsNull())
      continue;

    TopoDS_Shape orig = sb->Result();

    if (map.IsBound(orig))
    {
      sb->SetResult(map.Find(orig));
    }
    else if (!orig.Location().IsIdentity())
    {
      TopLoc_Location aNullLoc;
      TopoDS_Shape    atmpSh = orig.Located(aNullLoc);
      if (map.IsBound(atmpSh))
        sb->SetResult(map.Find(atmpSh));
    }
    else
    {
      // Some of edges may be modified.
      BRepTools_ReShape aReShape;
      Standard_Boolean  hasModifiedEdges = Standard_False;
      TopExp_Explorer   anExpSE(orig, TopAbs_EDGE);

      // Remember modifications.
      for (; anExpSE.More(); anExpSE.Next())
      {
        if (map.IsBound(anExpSE.Current()))
        {
          hasModifiedEdges            = Standard_True;
          TopoDS_Shape aModifiedShape = map.Find(anExpSE.Current());
          aReShape.Replace(anExpSE.Current(), aModifiedShape);
        }
      }

      // Apply modifications and store result in binder.
      if (hasModifiedEdges)
      {
        TopoDS_Shape aRes = aReShape.Apply(orig);
        sb->SetResult(aRes);
      }
    }

    // update messages
    if (!msg.IsNull())
    {
      const ShapeExtend_DataMapOfShapeListOfMsg& msgmap = msg->MapShape();
      if (msgmap.IsBound(orig))
      {
        const Message_ListOfMsg& msglist = msgmap.Find(orig);
        for (Message_ListIteratorOfListOfMsg iter(msglist); iter.More(); iter.Next())
        {
          const Message_Msg& mess = iter.Value();
          sb->AddWarning(TCollection_AsciiString(mess.Value()).ToCString(),
                         TCollection_AsciiString(mess.Original()).ToCString());
        }
      }
    }
  }
}

//=================================================================================================

void XSAlgo_AlgoContainer::MergeTransferInfo(const Handle(Transfer_FinderProcess)& FP,
                                             const Handle(Standard_Transient)&     info) const
{
  Handle(ShapeProcess_ShapeContext) context = Handle(ShapeProcess_ShapeContext)::DownCast(info);
  if (context.IsNull())
    return;

  const TopTools_DataMapOfShapeShape&           map = context->Map();
  TopTools_DataMapIteratorOfDataMapOfShapeShape ShapeShapeIterator(map);
  Handle(ShapeExtend_MsgRegistrator)            msg = context->Messages();

  for (; ShapeShapeIterator.More(); ShapeShapeIterator.Next())
  {

    TopoDS_Shape orig = ShapeShapeIterator.Key(), res = ShapeShapeIterator.Value();
    Handle(TransferBRep_ShapeMapper) resMapper = TransferBRep::ShapeMapper(FP, res);
    Handle(Transfer_Binder)          resBinder = FP->Find(resMapper);

    if (resBinder.IsNull())
    {
      resBinder = new TransferBRep_ShapeBinder(res);
      // if <orig> shape was split, put entities corresponding to new shapes
      //  into Transfer_TransientListBinder.
      if (orig.ShapeType() > res.ShapeType())
      {
        TopoDS_Shape                         sub;
        Handle(Transfer_TransientListBinder) TransientListBinder = new Transfer_TransientListBinder;
        for (TopoDS_Iterator it(res); it.More(); it.Next())
        {
          Handle(Transfer_Finder) subMapper = TransferBRep::ShapeMapper(FP, it.Value());
          if (subMapper.IsNull())
            continue;

          Handle(Standard_Transient) tr = FP->FindTransient(subMapper);
          if (tr.IsNull())
            continue;
          TransientListBinder->AddResult(tr);
          sub = it.Value();
        }
        if (TransientListBinder->NbTransients() == 1)
          resBinder = new TransferBRep_ShapeBinder(sub);
        else if (TransientListBinder->NbTransients() > 1)
        {
          resBinder->AddResult(TransientListBinder);
        }
      }
    }

    Handle(TransferBRep_ShapeMapper) origMapper = TransferBRep::ShapeMapper(FP, orig);
    Handle(Transfer_Binder)          origBinder = FP->Find(origMapper);
    if (origBinder.IsNull())
    {
      FP->Bind(origMapper, resBinder);
    }
    else
    {
      origBinder->AddResult(resBinder);
    }

    // update messages
    if (!msg.IsNull())
    {
      const ShapeExtend_DataMapOfShapeListOfMsg& msgmap = msg->MapShape();
      if (msgmap.IsBound(orig))
      {
        const Message_ListOfMsg& msglist = msgmap.Find(orig);
        for (Message_ListIteratorOfListOfMsg iter(msglist); iter.More(); iter.Next())
        {
          const Message_Msg& mess = iter.Value();
          resBinder->AddWarning(TCollection_AsciiString(mess.Value()).ToCString(),
                                TCollection_AsciiString(mess.Original()).ToCString());
        }
      }
    }
  }
}
