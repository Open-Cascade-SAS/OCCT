// Author: Kirill Gavrilov
// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#include <XCAFPrs_DocumentExplorer.hxx>

#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_VisMaterialTool.hxx>
#include <XCAFPrs_DocumentIdIterator.hxx>

namespace
{
//! Return merged style for the child node.
static XCAFPrs_Style mergedStyle(const Handle(XCAFDoc_ColorTool)&       theColorTool,
                                 const Handle(XCAFDoc_VisMaterialTool)& theVisMatTool,
                                 const XCAFPrs_Style&                   theParenStyle,
                                 const TDF_Label&                       theLabel,
                                 const TDF_Label&                       theRefLabel)
{
  if (theColorTool.IsNull())
  {
    return theParenStyle;
  }

  XCAFPrs_Style aStyle = theParenStyle;
  if (Handle(XCAFDoc_VisMaterial) aVisMat = theVisMatTool->GetShapeMaterial(theRefLabel))
  {
    aStyle.SetMaterial(aVisMat);
  }
  Quantity_ColorRGBA aColor;
  if (theColorTool->GetColor(theRefLabel, XCAFDoc_ColorGen, aColor))
  {
    aStyle.SetColorCurv(aColor.GetRGB());
    aStyle.SetColorSurf(aColor);
  }
  if (theColorTool->GetColor(theRefLabel, XCAFDoc_ColorSurf, aColor))
  {
    aStyle.SetColorSurf(aColor);
  }
  if (theColorTool->GetColor(theRefLabel, XCAFDoc_ColorCurv, aColor))
  {
    aStyle.SetColorCurv(aColor.GetRGB());
  }

  if (theLabel != theRefLabel)
  {
    // override Reference style with Instance style when defined (bad model?)
    if (Handle(XCAFDoc_VisMaterial) aVisMat = theVisMatTool->GetShapeMaterial(theLabel))
    {
      aStyle.SetMaterial(aVisMat);
    }
    if (theColorTool->GetColor(theLabel, XCAFDoc_ColorGen, aColor))
    {
      aStyle.SetColorCurv(aColor.GetRGB());
      aStyle.SetColorSurf(aColor);
    }
    if (theColorTool->GetColor(theLabel, XCAFDoc_ColorSurf, aColor))
    {
      aStyle.SetColorSurf(aColor);
    }
    if (theColorTool->GetColor(theLabel, XCAFDoc_ColorCurv, aColor))
    {
      aStyle.SetColorCurv(aColor.GetRGB());
    }
  }

  return aStyle;
}
} // namespace

// =======================================================================
// function : DefineChildId
// purpose  :
// =======================================================================
TCollection_AsciiString XCAFPrs_DocumentExplorer::DefineChildId(
  const TDF_Label&               theLabel,
  const TCollection_AsciiString& theParentId)
{
  TCollection_AsciiString anEntryId;
  TDF_Tool::Entry(theLabel, anEntryId);
  return !theParentId.IsEmpty() ? theParentId + "/" + anEntryId + "." : anEntryId + ".";
}

// =======================================================================
// function : FindLabelFromPathId
// purpose  :
// =======================================================================
TDF_Label XCAFPrs_DocumentExplorer::FindLabelFromPathId(const Handle(TDocStd_Document)& theDocument,
                                                        const TCollection_AsciiString&  theId,
                                                        TopLoc_Location& theParentLocation,
                                                        TopLoc_Location& theLocation)
{
  theParentLocation = TopLoc_Location();
  theLocation       = TopLoc_Location();
  TDF_Label anInstanceLabel;
  for (XCAFPrs_DocumentIdIterator anPathIter(theId); anPathIter.More();)
  {
    TDF_Label aSubLabel;
    {
      const TCollection_AsciiString& anOcafId = anPathIter.Value();
      TDF_Tool::Label(theDocument->Main().Data(), anOcafId, aSubLabel);
      if (aSubLabel.IsNull())
      {
        return TDF_Label();
      }
    }

    anPathIter.Next();
    if (!anPathIter.More())
    {
      theParentLocation = theLocation;
    }

    TopLoc_Location aLocTrsf = XCAFDoc_ShapeTool::GetLocation(aSubLabel);
    theLocation              = theLocation * aLocTrsf;
    anInstanceLabel          = aSubLabel;
  }
  return anInstanceLabel;
}

// =======================================================================
// function : FindShapeFromPathId
// purpose  :
// =======================================================================
TopoDS_Shape XCAFPrs_DocumentExplorer::FindShapeFromPathId(
  const Handle(TDocStd_Document)& theDocument,
  const TCollection_AsciiString&  theId)
{
  TopLoc_Location aLocation;
  TDF_Label       anInstanceLabel = FindLabelFromPathId(theDocument, theId, aLocation);
  if (anInstanceLabel.IsNull())
  {
    return TopoDS_Shape();
  }

  TDF_Label aRefLabel = anInstanceLabel;
  XCAFDoc_ShapeTool::GetReferredShape(anInstanceLabel, aRefLabel);
  if (aRefLabel.IsNull())
  {
    return TopoDS_Shape();
  }

  TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(aRefLabel);
  if (aShape.IsNull())
  {
    return TopoDS_Shape();
  }

  aShape.Location(aLocation);
  return aShape;
}

// =======================================================================
// function : XCAFPrs_DocumentExplorer
// purpose  :
// =======================================================================
XCAFPrs_DocumentExplorer::XCAFPrs_DocumentExplorer()
    : myTop(-1),
      myHasMore(Standard_False),
      myFlags(XCAFPrs_DocumentExplorerFlags_None)
{
  //
}

// =======================================================================
// function : XCAFPrs_DocumentExplorer
// purpose  :
// =======================================================================
XCAFPrs_DocumentExplorer::XCAFPrs_DocumentExplorer(const Handle(TDocStd_Document)&     theDocument,
                                                   const XCAFPrs_DocumentExplorerFlags theFlags,
                                                   const XCAFPrs_Style&                theDefStyle)
    : myTop(-1),
      myHasMore(Standard_False),
      myFlags(XCAFPrs_DocumentExplorerFlags_None)
{
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  TDF_LabelSequence         aRootLabels;
  aShapeTool->GetFreeShapes(aRootLabels);
  Init(theDocument, aRootLabels, theFlags, theDefStyle);
}

// =======================================================================
// function : XCAFPrs_DocumentExplorer
// purpose  :
// =======================================================================
XCAFPrs_DocumentExplorer::XCAFPrs_DocumentExplorer(const Handle(TDocStd_Document)&     theDocument,
                                                   const TDF_LabelSequence&            theRoots,
                                                   const XCAFPrs_DocumentExplorerFlags theFlags,
                                                   const XCAFPrs_Style&                theDefStyle)
    : myTop(-1),
      myHasMore(Standard_False),
      myFlags(XCAFPrs_DocumentExplorerFlags_None)
{
  Init(theDocument, theRoots, theFlags, theDefStyle);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void XCAFPrs_DocumentExplorer::Init(const Handle(TDocStd_Document)&     theDocument,
                                    const TDF_Label&                    theRoot,
                                    const XCAFPrs_DocumentExplorerFlags theFlags,
                                    const XCAFPrs_Style&                theDefStyle)
{
  TDF_LabelSequence aSeq;
  aSeq.Append(theRoot);
  Init(theDocument, aSeq, theFlags, theDefStyle);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void XCAFPrs_DocumentExplorer::Init(const Handle(TDocStd_Document)&     theDocument,
                                    const TDF_LabelSequence&            theRoots,
                                    const XCAFPrs_DocumentExplorerFlags theFlags,
                                    const XCAFPrs_Style&                theDefStyle)
{
  if ((theFlags & XCAFPrs_DocumentExplorerFlags_NoStyle) == 0)
  {
    myColorTool  = XCAFDoc_DocumentTool::ColorTool(theDocument->Main());
    myVisMatTool = XCAFDoc_DocumentTool::VisMaterialTool(theDocument->Main());
  }
  else
  {
    myColorTool.Nullify();
    myVisMatTool.Nullify();
  }

  myDefStyle = theDefStyle;
  myRoots    = theRoots;
  myRootIter = TDF_LabelSequence::Iterator(myRoots);
  myFlags    = theFlags;
  initRoot();
}

// =======================================================================
// function : initRoot
// purpose  :
// =======================================================================
void XCAFPrs_DocumentExplorer::initRoot()
{
  for (;;)
  {
    // reset the stack
    for (Standard_Integer aStackIter = 0; aStackIter <= myTop; ++aStackIter)
    {
      myNodeStack.SetValue(aStackIter, XCAFPrs_DocumentNode());
    }
    myTop = -1;
    if (!myRootIter.More())
    {
      myHasMore = Standard_False;
      initCurrent(Standard_False);
      return;
    }

    const TDF_Label& aRootLab = myRootIter.Value();
    if (aRootLab.IsNull())
    {
      // assert - invalid input
      // Standard_ProgramError::Raise ("XCAFPrs_DocumentExplorer - NULL label in the input");
      myRootIter.Next();
      continue;
    }

    myHasMore           = Standard_True;
    TDF_Label aRefLabel = aRootLab;
    XCAFDoc_ShapeTool::GetReferredShape(aRootLab, aRefLabel);
    if (XCAFDoc_ShapeTool::IsAssembly(aRefLabel))
    {
      Next();
    }
    else
    {
      initCurrent(Standard_False);
    }
    return;
  }
}

// =======================================================================
// function : initCurrent
// purpose  :
// =======================================================================
void XCAFPrs_DocumentExplorer::initCurrent(Standard_Boolean theIsAssembly)
{
  myCurrent = XCAFPrs_DocumentNode();
  if (theIsAssembly)
  {
    if (myTop < 0)
    {
      Standard_ProgramError::Raise("XCAFPrs_DocumentExplorer - internal error");
    }
    myCurrent = myNodeStack.Value(myTop);
  }
  else if (myTop < 0)
  {
    if (!myRootIter.More())
    {
      return;
    }

    myCurrent.Label    = myRootIter.Value();
    myCurrent.RefLabel = myCurrent.Label;
    XCAFDoc_ShapeTool::GetReferredShape(myCurrent.Label, myCurrent.RefLabel);
    myCurrent.LocalTrsf = XCAFDoc_ShapeTool::GetLocation(myCurrent.Label);
    myCurrent.Location  = myCurrent.LocalTrsf;
    myCurrent.Style =
      mergedStyle(myColorTool, myVisMatTool, myDefStyle, myCurrent.Label, myCurrent.RefLabel);
    myCurrent.Id = DefineChildId(myCurrent.Label, TCollection_AsciiString());
  }
  else
  {
    const XCAFPrs_DocumentNode& aTopNodeInStack = myNodeStack.Value(myTop);
    myCurrent.Label                             = aTopNodeInStack.ChildIter.Value();
    myCurrent.RefLabel                          = myCurrent.Label;
    XCAFDoc_ShapeTool::GetReferredShape(myCurrent.Label, myCurrent.RefLabel);
    myCurrent.LocalTrsf = XCAFDoc_ShapeTool::GetLocation(myCurrent.Label);
    myCurrent.Location  = aTopNodeInStack.Location * myCurrent.LocalTrsf;
    myCurrent.Style     = mergedStyle(myColorTool,
                                  myVisMatTool,
                                  aTopNodeInStack.Style,
                                  myCurrent.Label,
                                  myCurrent.RefLabel);
    myCurrent.Id        = DefineChildId(myCurrent.Label, aTopNodeInStack.Id);
  }
}

// =======================================================================
// function : Next
// purpose  :
// =======================================================================
void XCAFPrs_DocumentExplorer::Next()
{
  if (!myHasMore)
  {
    Standard_ProgramError::Raise("XCAFPrs_DocumentExplorer::Next() - out of range");
    return; // assert
  }

  if (myTop < 0)
  {
    const TDF_Label& aRootLab  = myRootIter.Value();
    TDF_Label        aRefLabel = aRootLab;
    XCAFDoc_ShapeTool::GetReferredShape(aRootLab, aRefLabel);
    if (!XCAFDoc_ShapeTool::IsAssembly(aRefLabel))
    {
      // already visited once
      myRootIter.Next();
      initRoot();
      return;
    }

    // push and try to find
    myTop = 0;
    XCAFPrs_DocumentNode aNodeInStack;
    aNodeInStack.IsAssembly = Standard_True;
    aNodeInStack.Label      = aRootLab;
    aNodeInStack.RefLabel   = aRefLabel;
    aNodeInStack.ChildIter  = TDF_ChildIterator(aNodeInStack.RefLabel);
    aNodeInStack.LocalTrsf  = XCAFDoc_ShapeTool::GetLocation(aNodeInStack.Label);
    aNodeInStack.Location   = aNodeInStack.LocalTrsf;
    aNodeInStack.Style =
      mergedStyle(myColorTool, myVisMatTool, myDefStyle, aNodeInStack.Label, aNodeInStack.RefLabel);
    aNodeInStack.Id = DefineChildId(aNodeInStack.Label, TCollection_AsciiString());
    myNodeStack.SetValue(0, aNodeInStack);
    if ((myFlags & XCAFPrs_DocumentExplorerFlags_OnlyLeafNodes) == 0)
    {
      initCurrent(Standard_True);
      return;
    }
  }
  else
  {
    if (!myCurrent.IsAssembly)
    {
      myNodeStack.ChangeValue(myTop).ChildIter.Next();
    }
  }

  for (;;)
  {
    if (myNodeStack.Value(myTop).ChildIter.More())
    {
      const TDF_Label& aNodeTop = myNodeStack.Value(myTop).ChildIter.Value();
      if (aNodeTop.IsNull() || (!aNodeTop.HasChild() && !aNodeTop.HasAttribute()))
      {
        myNodeStack.ChangeValue(myTop).ChildIter.Next();
        continue;
      }

      TDF_Label aRefLabel = aNodeTop;
      XCAFDoc_ShapeTool::GetReferredShape(aNodeTop, aRefLabel);
      if (!XCAFDoc_ShapeTool::IsAssembly(aRefLabel))
      {
        myHasMore = Standard_True;
        initCurrent(Standard_False);
        return;
      }
      else if (aRefLabel.HasAttribute() || aRefLabel.HasChild())
      {
        const XCAFPrs_DocumentNode& aParent = myNodeStack.Value(myTop);
        ++myTop;

        XCAFPrs_DocumentNode aNodeInStack;
        aNodeInStack.IsAssembly = Standard_True;
        aNodeInStack.Label      = aNodeTop;
        aNodeInStack.RefLabel   = aRefLabel;
        aNodeInStack.LocalTrsf  = XCAFDoc_ShapeTool::GetLocation(aNodeInStack.Label);
        aNodeInStack.Location   = aParent.Location * aNodeInStack.LocalTrsf;
        aNodeInStack.Style      = mergedStyle(myColorTool,
                                         myVisMatTool,
                                         aParent.Style,
                                         aNodeInStack.Label,
                                         aNodeInStack.RefLabel);
        aNodeInStack.Id         = DefineChildId(aNodeInStack.Label, aParent.Id);
        aNodeInStack.ChildIter  = TDF_ChildIterator(aNodeInStack.RefLabel);
        myNodeStack.SetValue(myTop, aNodeInStack);
        if ((myFlags & XCAFPrs_DocumentExplorerFlags_OnlyLeafNodes) == 0)
        {
          initCurrent(Standard_True);
          return;
        }
      }
      else
      {
        myNodeStack.ChangeValue(myTop).ChildIter.Next();
      }
    }
    else
    {
      myNodeStack.SetValue(myTop, XCAFPrs_DocumentNode());
      --myTop;
      if (myTop < 0)
      {
        myRootIter.Next();
        initRoot();
        return;
      }

      myNodeStack.ChangeValue(myTop).ChildIter.Next();
    }
  }
}
