// Created on: 2000-08-16
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
#include <IGESBasic_SubfigureDef.hxx>
#include <IGESCAFControl.hxx>
#include <IGESCAFControl_Reader.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <IGESGraph_Color.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Quantity_Color.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_ShapeProcessor.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>
#include <UnitsMethods.hxx>

//=================================================================================================

static void checkColorRange(double& theCol)
{
  if (theCol < 0.)
    theCol = 0.;
  if (theCol > 100.)
    theCol = 100.;
}

static inline bool IsComposite(const TopoDS_Shape& theShape)
{
  if (theShape.ShapeType() == TopAbs_COMPOUND)
  {
    if (!theShape.Location().IsIdentity())
      return true;
    TopoDS_Iterator anIt(theShape, false, false);

    for (; anIt.More(); anIt.Next())
    {
      if (IsComposite(anIt.Value()))
        return true;
    }
  }
  return false;
}

//=======================================================================
// function : AddCompositeShape
// purpose  : Recursively adds composite shapes (TopoDS_Compounds) into the XDE document.
//           If the compound does not contain nested compounds then adds it
//           as no-assembly (i.e. no individual labels for sub-shapes), as this
//           combination is often encountered in IGES (e.g. Group of Trimmed Surfaces).
//           If the compound does contain nested compounds then adds it as an
//           assembly.
//           The construction happens bottom-up, i.e. the most deep sub-shapes are added
//           first.
//           If theIsTop is False (in a recursive call) then sub-shapes are added without
//           a location. This is to ensure that no extra label in the XDE document is
//           created for an instance (as otherwise, XDE will consider it as a free
//           shape). Correct location and instance will be created when adding a parent
//           compound.
//           theMap is used to avoid visiting the same compound.
//=======================================================================
static void AddCompositeShape(const occ::handle<XCAFDoc_ShapeTool>& theSTool,
                              const TopoDS_Shape&              theShape,
                              bool                 theConsiderLoc,
                              NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&             theMap)
{
  TopoDS_Shape           aShape = theShape;
  const TopLoc_Location& aLoc   = theShape.Location();
  if (!theConsiderLoc && !aLoc.IsIdentity())
    aShape.Location(TopLoc_Location());
  if (!theMap.Add(aShape))
    return;

  TopoDS_Iterator  anIt(theShape, false, false);
  bool aHasCompositeSubShape = false;
  TopoDS_Compound  aSimpleShape;
  BRep_Builder     aB;
  aB.MakeCompound(aSimpleShape);
  TopoDS_Compound aCompShape;
  aB.MakeCompound(aCompShape);
  int nbSimple = 0;

  for (; anIt.More(); anIt.Next())
  {
    const TopoDS_Shape& aSubShape = anIt.Value();
    if (IsComposite(aSubShape))
    {
      aHasCompositeSubShape = true;
      AddCompositeShape(theSTool, aSubShape, false, theMap);
      aB.Add(aCompShape, aSubShape);
    }
    else
    {
      aB.Add(aSimpleShape, aSubShape);
      nbSimple++;
    }
  }
  // case of hybrid shape
  if (nbSimple && aHasCompositeSubShape)
  {
    theSTool->AddShape(aSimpleShape, false, false);

    TopoDS_Compound aNewShape;
    aB.MakeCompound(aNewShape);
    aB.Add(aNewShape, aSimpleShape);
    aB.Add(aNewShape, aCompShape);

    if (!aLoc.IsIdentity())
      aNewShape.Location(aLoc);
    aNewShape.Orientation(theShape.Orientation());
    theSTool->AddShape(aNewShape, aHasCompositeSubShape, false);
  }
  else
    theSTool->AddShape(aShape, aHasCompositeSubShape, false);
  return;
}

//=================================================================================================

bool IGESCAFControl_Reader::Transfer(const occ::handle<TDocStd_Document>& doc,
                                                 const Message_ProgressRange&    theProgress)
{
  // read all shapes
  int num; // = NbRootsForTransfer();
  // if ( num <=0 ) return false;
  // for ( int i=1; i <= num; i++ ) {
  //   TransferOneRoot ( i );
  // }

  // set units
  occ::handle<IGESData_IGESModel> aModel = occ::down_cast<IGESData_IGESModel>(WS()->Model());

  double aScaleFactorMM = 1.;
  if (!XCAFDoc_DocumentTool::GetLengthUnit(doc, aScaleFactorMM, UnitsMethods_LengthUnit_Millimeter))
  {
    XSAlgo_ShapeProcessor::PrepareForTransfer(); // update unit info
    aScaleFactorMM = UnitsMethods::GetCasCadeLengthUnit();
    // set length unit to the document
    XCAFDoc_DocumentTool::SetLengthUnit(doc, aScaleFactorMM, UnitsMethods_LengthUnit_Millimeter);
  }
  aModel->ChangeGlobalSection().SetCascadeUnit(aScaleFactorMM);
  TransferRoots(theProgress); // replaces the above
  num = NbShapes();
  if (num <= 0)
    return false;

  // and insert them to the document
  occ::handle<XCAFDoc_ShapeTool> STool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
  if (STool.IsNull())
    return false;
  int i;
  for (i = 1; i <= num; i++)
  {
    TopoDS_Shape sh = Shape(i);
    // ---- HERE -- to add check [ assembly / hybrid model ]
    if (!IsComposite(sh))
      STool->AddShape(sh, false);
    else
    {
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
      AddCompositeShape(STool, sh, true, aMap);
    }
  }

  // added by skl 13.10.2003
  const occ::handle<XSControl_TransferReader>&  TR      = WS()->TransferReader();
  const occ::handle<Transfer_TransientProcess>& TP      = TR->TransientProcess();
  bool                         IsCTool = true;
  occ::handle<XCAFDoc_ColorTool>                CTool   = XCAFDoc_DocumentTool::ColorTool(doc->Main());
  if (CTool.IsNull())
    IsCTool = false;
  bool          IsLTool = true;
  occ::handle<XCAFDoc_LayerTool> LTool   = XCAFDoc_DocumentTool::LayerTool(doc->Main());
  if (LTool.IsNull())
    IsLTool = false;

  int nb = aModel->NbEntities();
  for (i = 1; i <= nb; i++)
  {
    occ::handle<IGESData_IGESEntity> ent = occ::down_cast<IGESData_IGESEntity>(aModel->Value(i));
    if (ent.IsNull())
      continue;
    occ::handle<Transfer_Binder> binder = TP->Find(ent);
    if (binder.IsNull())
      continue;
    TopoDS_Shape S = TransferBRep::ShapeResult(binder);
    if (S.IsNull())
      continue;

    bool IsColor = false;
    Quantity_Color   col;
    if (GetColorMode() && IsCTool)
    {
      // read colors
      if (ent->DefColor() == IGESData_DefValue || ent->DefColor() == IGESData_DefReference)
      {
        // color is assigned
        // decode color and set to document
        IsColor = true;
        if (ent->DefColor() == IGESData_DefValue)
        {
          col = IGESCAFControl::DecodeColor(ent->RankColor());
        }
        else
        {
          occ::handle<IGESGraph_Color> color = occ::down_cast<IGESGraph_Color>(ent->Color());
          if (color.IsNull())
          {
#ifdef OCCT_DEBUG
            std::cout << "Error: Unrecognized type of color definition" << std::endl;
#endif
            IsColor = false;
          }
          else
          {
            double r, g, b;
            color->RGBIntensity(r, g, b);
            checkColorRange(r);
            checkColorRange(g);
            checkColorRange(b);
            col.SetValues(0.01 * r, 0.01 * g, 0.01 * b, Quantity_TOC_sRGB);
          }
        }
      }
    }

    TDF_Label L;

    bool IsFound;
    if (IsColor)
    {
      CTool->AddColor(col);
      IsFound = STool->SearchUsingMap(S, L, false, true);
    }
    else
    {
      IsFound = STool->SearchUsingMap(S, L, false, false);
    }
    if (!IsFound)
    {
      if (IsColor)
      {
        for (TopoDS_Iterator it(S); it.More(); it.Next())
        {
          if (STool->SearchUsingMap(it.Value(), L, false, true))
          {
            CTool->SetColor(L, col, XCAFDoc_ColorGen);
            if (GetLayerMode() && IsLTool)
            {
              // read layers
              // set a layers to the document
              IGESData_DefList aDeflist = ent->DefLevel();
              switch (aDeflist)
              {
                case IGESData_DefOne: {
                  TCollection_ExtendedString aLayerName(ent->Level());
                  LTool->SetLayer(L, aLayerName);
                  break;
                }
                case IGESData_DefSeveral: {
                  occ::handle<IGESData_LevelListEntity> aLevelList = ent->LevelList();
                  int                 layerNb    = aLevelList->NbLevelNumbers();
                  for (int ilev = 1; ilev <= layerNb; ilev++)
                  {
                    TCollection_ExtendedString aLayerName(aLevelList->LevelNumber(ilev));
                    LTool->SetLayer(L, aLayerName);
                  }
                  break;
                }
                default:
                  break;
              }
            }
          }
        }
      }
    }
    else
    {
      if (IsColor)
      {
        CTool->SetColor(L, col, XCAFDoc_ColorGen);
      }
      if (GetNameMode())
      {
        // read names
        if (ent->HasName())
        {
          TCollection_AsciiString string = ent->NameValue()->String();
          string.LeftAdjust();
          string.RightAdjust();
          TCollection_ExtendedString str(string);
          TDataStd_Name::Set(L, str);
        }
      }
      if (GetLayerMode() && IsLTool)
      {
        // read layers
        // set a layers to the document
        IGESData_DefList aDeflist = ent->DefLevel();
        switch (aDeflist)
        {
          case IGESData_DefOne: {
            TCollection_ExtendedString aLayerName(ent->Level());
            LTool->SetLayer(L, aLayerName);
            break;
          }
          case IGESData_DefSeveral: {
            occ::handle<IGESData_LevelListEntity> aLevelList = ent->LevelList();
            int                 layerNb    = aLevelList->NbLevelNumbers();
            for (int ilev = 1; ilev <= layerNb; ilev++)
            {
              TCollection_ExtendedString aLayerName(aLevelList->LevelNumber(ilev));
              LTool->SetLayer(L, aLayerName);
            }
            break;
          }
          default:
            break;
        }
      }
    }

    // Checks that current entity is a subfigure
    occ::handle<IGESBasic_SubfigureDef> aSubfigure = occ::down_cast<IGESBasic_SubfigureDef>(ent);
    if (GetNameMode() && !aSubfigure.IsNull() && !aSubfigure->Name().IsNull()
        && STool->Search(S, L, true, true))
    {
      // In this case we attach subfigure name to the label, instead of default "COMPOUND"
      occ::handle<TCollection_HAsciiString> aName = aSubfigure->Name();
      aName->LeftAdjust();
      aName->RightAdjust();
      TCollection_ExtendedString anExtStrName(aName->ToCString());
      TDataStd_Name::Set(L, anExtStrName);
    }
  }

  CTool->ReverseChainsOfTreeNodes();

  // Update assembly compounds
  STool->UpdateAssemblies();

  return true;
}

//=================================================================================================

bool IGESCAFControl_Reader::Perform(const char* const          filename,
                                                const occ::handle<TDocStd_Document>& doc,
                                                const Message_ProgressRange&    theProgress)
{
  if (ReadFile(filename) != IFSelect_RetDone)
    return false;
  return Transfer(doc, theProgress);
}
