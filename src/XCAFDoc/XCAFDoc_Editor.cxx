// Created on: 2015-05-14
// Created by: data exchange team
// Copyright (c) 2000-2015 OPEN CASCADE SAS
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

#include <XCAFDoc_Editor.hxx>

#include <BRep_Builder.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_Location.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_ShapeMapTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_VisMaterial.hxx>
#include <XCAFDoc_VisMaterialTool.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Compound.hxx>
#include <TNaming_NamedShape.hxx>

//=======================================================================
//function : Expand
//purpose  : Convert Shape to assembly
//=======================================================================
Standard_Boolean XCAFDoc_Editor::Expand (const TDF_Label& theDoc,
                                         const TDF_Label& theShape,
                                         const Standard_Boolean theRecursively)
{
  if (theDoc.IsNull() || theShape.IsNull())
  {
    return Standard_False;
  }
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDoc);
  Standard_Boolean isAutoNaming = aShapeTool->AutoNaming();
  aShapeTool->SetAutoNaming(Standard_False);

  TDF_Label aCompoundPartL = theShape;
  if (aShapeTool->IsReference(theShape))
    aShapeTool->GetReferredShape(aCompoundPartL, aCompoundPartL);

  TopoDS_Shape aS = aShapeTool->GetShape(aCompoundPartL);
  if (aShapeTool->Expand(aCompoundPartL))
  {
    //move attributes
    for(TDF_ChildIterator aPartIter(aCompoundPartL, Standard_True);
      aPartIter.More(); aPartIter.Next())
    {
      TDF_Label aChild = aPartIter.Value();
      //get part
      TDF_Label aPart;
      if(aShapeTool->GetReferredShape(aChild, aPart))
      {
        CloneMetaData(aChild, aPart, NULL);
        //remove unnecessary links
        TopoDS_Shape aShape = aShapeTool->GetShape(aChild);
        if(!aShapeTool->GetShape(aPart.Father()).IsNull())
        {
          aPart.ForgetAttribute(XCAFDoc::ShapeRefGUID());
          if (aShapeTool->GetShape(aPart.Father()).ShapeType() == TopAbs_COMPOUND)
          {
            aShapeTool->SetShape(aPart, aShape);
          }
          aPart.ForgetAttribute(XCAFDoc_ShapeMapTool::GetID());
          aChild.ForgetAllAttributes(Standard_False);
        }
        aChild.ForgetAttribute(TNaming_NamedShape::GetID());
        aChild.ForgetAttribute(XCAFDoc_ShapeMapTool::GetID());
      }
      else
      {
        // If new original shape is not created, try to process this child
        // as subshape of new part
        TDF_LabelSequence aUsers;
        if (aShapeTool->GetUsers(aChild, aUsers) > 0)
        {
          for (TDF_LabelSequence::Iterator anIter(aUsers);
            anIter.More(); anIter.Next())
          {
            TDF_Label aSubLabel = anIter.Value();
            //remove unnecessary links
            aSubLabel.ForgetAttribute(XCAFDoc::ShapeRefGUID());
            aSubLabel.ForgetAttribute(XCAFDoc_ShapeMapTool::GetID());
            CloneMetaData(aChild, aSubLabel, NULL);
          }
          aChild.ForgetAllAttributes(Standard_False);
        }
      }
    }
    //if assembly contains compound, expand it recursively(if flag theRecursively is true)
    if(theRecursively)
    {
      for(TDF_ChildIterator aPartIter(aCompoundPartL); aPartIter.More(); aPartIter.Next())
      {
        TDF_Label aPart = aPartIter.Value();
        if(aShapeTool->GetReferredShape(aPart, aPart))
        {
          TopoDS_Shape aPartShape = aShapeTool->GetShape(aPart);
          if (!aPartShape.IsNull() && aPartShape.ShapeType() == TopAbs_COMPOUND)
            Expand(theDoc, aPart, theRecursively);
        }
      }
    }
    aShapeTool->SetAutoNaming(isAutoNaming);
    return Standard_True;
  }
  aShapeTool->SetAutoNaming(isAutoNaming);
  return Standard_False;
}

//=======================================================================
//function : Expand
//purpose  : Convert all compounds in Doc to assembly
//=======================================================================
Standard_Boolean XCAFDoc_Editor::Expand (const TDF_Label& theDoc,
                                         const Standard_Boolean theRecursively)
{
  if (theDoc.IsNull())
  {
    return Standard_False;
  }
  Standard_Boolean aResult = Standard_False;
  TDF_LabelSequence aLabels;
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDoc);
  aShapeTool->GetFreeShapes(aLabels);
  for(TDF_LabelSequence::Iterator anIter(aLabels); anIter.More(); anIter.Next())
  {
    const TDF_Label aLabel = anIter.Value();
    const TopoDS_Shape aS = aShapeTool->GetShape(aLabel);
    if (!aS.IsNull() && aS.ShapeType() == TopAbs_COMPOUND && !aShapeTool->IsAssembly(aLabel))
    {
      if (Expand(theDoc, aLabel, theRecursively))
      {
        aResult = Standard_True;
      }
    }
  }
  return aResult;
}

//=======================================================================
//function : Extract
//purpose  :
//=======================================================================
Standard_Boolean XCAFDoc_Editor::Extract(const TDF_LabelSequence& theSrcLabels,
                                         const TDF_Label& theDstLabel,
                                         const Standard_Boolean theIsNoVisMat)
{
  if (theDstLabel.IsNull())
  {
    return Standard_False;
  }

  Handle(XCAFDoc_ShapeTool) aDstShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDstLabel);
  NCollection_DataMap<Handle(XCAFDoc_VisMaterial), Handle(XCAFDoc_VisMaterial)> aVisMatMap;
  for (TDF_LabelSequence::Iterator aNewRootIter(theSrcLabels);
    aNewRootIter.More(); aNewRootIter.Next())
  {
    // Shape
    TopLoc_Location aLoc;
    TDF_LabelDataMap aMap;
    const TDF_Label aSrcLabel = aNewRootIter.Value();
    Handle(XCAFDoc_ShapeTool) aSrcShapeTool = XCAFDoc_DocumentTool::ShapeTool(aSrcLabel);
    Handle(XCAFDoc_Location) aLocationAttr;
    if (aSrcLabel.FindAttribute(XCAFDoc_Location::GetID(), aLocationAttr))
    {
      aLoc = aLocationAttr->Get();
    }
    TDF_Label aCompLabel = aSrcLabel;
    aSrcShapeTool->GetReferredShape(aSrcLabel, aCompLabel);
    TDF_Label aResLabel = CloneShapeLabel(aCompLabel, aSrcShapeTool, aDstShapeTool, aMap);
    TDF_Label aNewCompLabel = aDstShapeTool->AddComponent(theDstLabel, aResLabel, aLoc);
    if (aNewCompLabel.IsNull())
    {
      TopoDS_Shape aNewShape = aDstShapeTool->GetShape(aResLabel);
      aNewShape.Move(aLoc);
      aNewCompLabel = aDstShapeTool->AddShape(aNewShape, false);
    }
    aMap.Bind(aSrcLabel, aNewCompLabel);
    aMap.Bind(aCompLabel, aResLabel);
    aDstShapeTool->UpdateAssemblies();
    // Attributes
    for (TDF_LabelDataMap::Iterator aLabelIter(aMap); aLabelIter.More(); aLabelIter.Next())
    {
      CloneMetaData(aLabelIter.Key(), aLabelIter.Value(), &aVisMatMap, true, true, true, !theIsNoVisMat);
    }
  }
  return Standard_True;
}

//=======================================================================
//function : Extract
//purpose  :
//=======================================================================
Standard_Boolean XCAFDoc_Editor::Extract(const TDF_Label& theSrcLabel,
                                         const TDF_Label& theDstLabel,
                                         const Standard_Boolean theIsNoVisMat)
{
  TDF_LabelSequence aSeq;
  aSeq.Append(theSrcLabel);
  return Extract(aSeq, theDstLabel, theIsNoVisMat);
}

// =======================================================================
// function : CloneShapeLebel
// purpose  :
// =======================================================================
TDF_Label XCAFDoc_Editor::CloneShapeLabel(const TDF_Label& theSrcLabel,
                                          const Handle(XCAFDoc_ShapeTool)& theSrcShapeTool,
                                          const Handle(XCAFDoc_ShapeTool)& theDstShapeTool,
                                          TDF_LabelDataMap& theMap)
{
  TDF_Label aNewShL;
  if (theMap.Find(theSrcLabel, aNewShL))
  {
    return aNewShL;
  }

  // Location for main assembly
  if (theSrcShapeTool->IsAssembly(theSrcLabel))
  {
    // add assembly and iterate all its components
    TopoDS_Compound aComp;
    BRep_Builder().MakeCompound(aComp);
    aNewShL = theDstShapeTool->AddShape(aComp);
    theMap.Bind(theSrcLabel, aNewShL);

    TDF_LabelSequence aComponents;
    theSrcShapeTool->GetComponents(theSrcLabel, aComponents);
    for (TDF_LabelSequence::Iterator aCompIter(aComponents); aCompIter.More(); aCompIter.Next())
    {
      TDF_Label aCompL = aCompIter.Value();
      TDF_Label aRefL;
      theSrcShapeTool->GetReferredShape(aCompL, aRefL);
      TDF_Label aCompOriginalL = CloneShapeLabel(aRefL, theSrcShapeTool, theDstShapeTool, theMap);
      Handle(XCAFDoc_Location) aLocationAttr;
      aCompL.FindAttribute(XCAFDoc_Location::GetID(), aLocationAttr);
      TDF_Label aNewCompL = theDstShapeTool->AddComponent(aNewShL, aCompOriginalL, aLocationAttr->Get());
      theMap.Bind(aCompIter.Value(), aNewCompL);
    }
    return aNewShL;
  }

  // add part
  TopoDS_Shape aShape = theSrcShapeTool->GetShape(theSrcLabel);
  aNewShL = theDstShapeTool->AddShape(aShape, false);
  theMap.Bind(theSrcLabel, aNewShL);

  // get original instead of auxiliary instance
  TDF_Label anOldOriginalL = theSrcLabel;
  if (theSrcShapeTool->IsReference(theSrcLabel))
  {
    theSrcShapeTool->GetReferredShape(theSrcLabel, anOldOriginalL);
    theDstShapeTool->GetReferredShape(aNewShL, aNewShL);
    theMap.Bind(anOldOriginalL, aNewShL);
  }
  // copy subshapes
  TDF_LabelSequence anOldSubShapes;
  theSrcShapeTool->GetSubShapes(anOldOriginalL, anOldSubShapes);
  for (TDF_LabelSequence::Iterator aSubIter(anOldSubShapes); aSubIter.More(); aSubIter.Next())
  {
    TopoDS_Shape aSubShape = theSrcShapeTool->GetShape(aSubIter.Value());
    TDF_Label aNewSubShapeL = theDstShapeTool->AddSubShape(aNewShL, aSubShape);
    theMap.Bind(aSubIter.Value(), aNewSubShapeL);
  }
  return aNewShL;
}

//=======================================================================
//function : CloneMetaData
//purpose  :
//=======================================================================
void XCAFDoc_Editor::CloneMetaData(const TDF_Label& theSrcLabel,
  const TDF_Label& theDstLabel,
  NCollection_DataMap<Handle(XCAFDoc_VisMaterial), Handle(XCAFDoc_VisMaterial)>* theVisMatMap,
  const Standard_Boolean theToCopyColor,
  const Standard_Boolean theToCopyLayer,
  const Standard_Boolean theToCopyMaterial,
  const Standard_Boolean theToCopyVisMaterial,
  const Standard_Boolean theToCopyAttribute)
{
  if (theSrcLabel == theDstLabel || theSrcLabel.IsNull() || theDstLabel.IsNull())
  {
    return;
  }
  const Standard_Boolean toCopyColor = theToCopyColor & XCAFDoc_DocumentTool::CheckColorTool(theSrcLabel);
  const Standard_Boolean toCopyLayer = theToCopyLayer & XCAFDoc_DocumentTool::CheckLayerTool(theSrcLabel);
  const Standard_Boolean toCopyMaterial = theToCopyMaterial & XCAFDoc_DocumentTool::CheckMaterialTool(theSrcLabel);
  const Standard_Boolean toCopyVisMaterial = XCAFDoc_DocumentTool::CheckVisMaterialTool(theSrcLabel);
  // Colors
  if (toCopyColor)
  {
    Handle(XCAFDoc_ColorTool) aSrcColorTool = XCAFDoc_DocumentTool::ColorTool(theSrcLabel);
    Handle(XCAFDoc_ColorTool) aDstColorTool = XCAFDoc_DocumentTool::ColorTool(theDstLabel);
    const XCAFDoc_ColorType aTypes[] = { XCAFDoc_ColorGen , XCAFDoc_ColorSurf , XCAFDoc_ColorCurv };
    for (int anInd = 0; anInd < 3; anInd++)
    {
      TDF_Label aColorL;
      aSrcColorTool->GetColor(theSrcLabel, aTypes[anInd], aColorL);
      if (!aColorL.IsNull())
      {
        Quantity_ColorRGBA aColor;
        aSrcColorTool->GetColor(aColorL, aColor);
        aDstColorTool->SetColor(theDstLabel, aColor, aTypes[anInd]);
      }
    }
    aDstColorTool->SetVisibility(theDstLabel, aSrcColorTool->IsVisible(theSrcLabel));
  }
  // Layers
  if (toCopyLayer)
  {
    Handle(XCAFDoc_LayerTool) aSrcLayerTool = XCAFDoc_DocumentTool::LayerTool(theSrcLabel);
    Handle(XCAFDoc_LayerTool) aDstLayerTool = XCAFDoc_DocumentTool::LayerTool(theDstLabel);
    TDF_LabelSequence aLayers;
    aSrcLayerTool->GetLayers(theSrcLabel, aLayers);
    for (TDF_LabelSequence::Iterator aLayerIter(aLayers); aLayerIter.More(); aLayerIter.Next())
    {
      TCollection_ExtendedString aLayerName;
      aSrcLayerTool->GetLayer(aLayerIter.Value(), aLayerName);
      aDstLayerTool->SetLayer(theDstLabel, aLayerName);
    }
  }
  // Materials
  if (toCopyMaterial)
  {
    Handle(TDataStd_TreeNode) aMatNode;
    if (theSrcLabel.FindAttribute(XCAFDoc::MaterialRefGUID(), aMatNode) && aMatNode->HasFather())
    {
      TDF_Label aMaterialL = aMatNode->Father()->Label();
      if (!aMaterialL.IsNull())
      {
        Handle(XCAFDoc_MaterialTool) aSrcMaterialTool = XCAFDoc_DocumentTool::MaterialTool(theSrcLabel);
        Handle(XCAFDoc_MaterialTool) aDstMaterialTool = XCAFDoc_DocumentTool::MaterialTool(theDstLabel);
        double aDensity = 0.0;
        Handle(TCollection_HAsciiString) aName, aDescription, aDensName, aDensValType;
        if (aSrcMaterialTool->GetMaterial(aMaterialL, aName, aDescription, aDensity, aDensName, aDensValType)
          && !aName.IsNull()
          && aName->Length() != 0)
        {
          aDstMaterialTool->SetMaterial(theDstLabel, aName, aDescription, aDensity, aDensName, aDensValType);
        }
      }
    }
  }
  // Visual Materials
  if (toCopyVisMaterial && (theToCopyVisMaterial || toCopyColor))
  {
    Handle(XCAFDoc_VisMaterialTool) aSrcVisMatTool = XCAFDoc_DocumentTool::VisMaterialTool(theSrcLabel);
    TDF_Label aVisMaterialL;
    aSrcVisMatTool->GetShapeMaterial(theSrcLabel, aVisMaterialL);
    if (!aVisMaterialL.IsNull())
    {
      Handle(XCAFDoc_VisMaterialTool) aDstVisMatTool;
      Handle(XCAFDoc_ColorTool) aDstColorTool;
      if (theToCopyVisMaterial)
      {
        aDstVisMatTool = XCAFDoc_DocumentTool::VisMaterialTool(theDstLabel);
      }
      else
      {
        aDstColorTool = XCAFDoc_DocumentTool::ColorTool(theDstLabel);
      }
      Handle(XCAFDoc_VisMaterial) aVisMatSrc = aSrcVisMatTool->GetMaterial(aVisMaterialL);
      if(theToCopyVisMaterial)
      {
        Handle(XCAFDoc_VisMaterial) aVisMatDst;
        if (theVisMatMap != NULL)
        {
          if (!theVisMatMap->Find(aVisMatSrc, aVisMatDst))
          {
            aVisMatDst = new XCAFDoc_VisMaterial();
            aVisMatDst->SetCommonMaterial(aVisMatSrc->CommonMaterial());
            aVisMatDst->SetPbrMaterial(aVisMatSrc->PbrMaterial());
            aVisMatDst->SetAlphaMode(aVisMatSrc->AlphaMode(), aVisMatSrc->AlphaCutOff());
            aVisMatDst->SetFaceCulling(aVisMatSrc->FaceCulling());
            TCollection_AsciiString aName;
            Handle(TDataStd_Name) aNodeName;
            if (aVisMatSrc->Label().FindAttribute(TDataStd_Name::GetID(), aNodeName))
            {
              aName = aNodeName->Get();
            }
            aDstVisMatTool->AddMaterial(aVisMatDst, aName);
            theVisMatMap->Bind(aVisMatSrc, aVisMatDst);
          }
        }
        else
        {
          aVisMatDst = aVisMatSrc; // consider the same document
        }
        aDstVisMatTool->SetShapeMaterial(theDstLabel, aVisMatDst->Label());
      }
      else
      {
        aDstColorTool->SetColor(theDstLabel, aVisMatSrc->BaseColor(), XCAFDoc_ColorGen);
      }
    }
  }
  // Other attributes
  if (theToCopyAttribute)
  {
    // Finds the target attributes or creates them empty
    for (TDF_AttributeIterator anAttIter(theSrcLabel); anAttIter.More(); anAttIter.Next())
    {
      const Handle(TDF_Attribute) anAttSrc = anAttIter.Value();
      // protect against color and layer coping without link to colors and layers
      if (const TDataStd_TreeNode* aTreeNode = dynamic_cast<const TDataStd_TreeNode*> (anAttSrc.get()))
      {
        (void)aTreeNode;
        continue;
      }
      else if (const XCAFDoc_GraphNode* aGraphNode = dynamic_cast<const XCAFDoc_GraphNode*> (anAttSrc.get()))
      {
        (void)aGraphNode;
        continue;
      }
      else if (const TNaming_NamedShape* aShapeAttr = dynamic_cast<const TNaming_NamedShape*> (anAttSrc.get()))
      {
        (void)aShapeAttr;
        continue;
      }
      else if (const XCAFDoc_ShapeMapTool* aShMapTool = dynamic_cast<const XCAFDoc_ShapeMapTool*> (anAttSrc.get()))
      {
        (void)aShMapTool;
        continue;
      }
      else if (const XCAFDoc_Location* aLocAttr = dynamic_cast<const XCAFDoc_Location*> (anAttSrc.get()))
      {
        (void)aLocAttr;
        continue;
      }
      Handle(TDF_Attribute) anAttDst;
      if (!theDstLabel.FindAttribute(anAttSrc->ID(), anAttDst))
      {
        anAttDst = anAttSrc->NewEmpty();
        theDstLabel.AddAttribute(anAttDst);
      }
      Handle(TDF_RelocationTable) aRT = new TDF_RelocationTable();
      anAttSrc->Paste(anAttDst, aRT);
    }
  }
  // Name
  Handle(TDataStd_Name) aNameAttr;
  theSrcLabel.FindAttribute(TDataStd_Name::GetID(), aNameAttr);
  Handle(XCAFDoc_ShapeTool) aDstShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDstLabel);
  if (!aNameAttr.IsNull())
  {
    TDF_Label aRefLabel;
    if (aNameAttr->Get().Search("=>") < 0)
    {
      TDataStd_Name::Set(theDstLabel, aNameAttr->Get());
    }
    else if (aDstShapeTool->GetReferredShape(theDstLabel, aRefLabel))
    {
      TCollection_AsciiString aRefName;
      TDF_Tool::Entry(aRefLabel, aRefName);
      aRefName.Insert(1, "=>");
      TDataStd_Name::Set(theDstLabel, aRefName);
    }
  }
  else
  {
    Standard_SStream Stream;
    TopoDS_Shape aShape = aDstShapeTool->GetShape(theDstLabel);
    TopAbs::Print(aShape.ShapeType(), Stream);
    TCollection_AsciiString aName(Stream.str().c_str());
    TDataStd_Name::Set(theDstLabel, TCollection_ExtendedString(aName));
  }
}
