// Created on: 2015-06-18
// Created by: Ilya Novikov
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

#include <XDEDRAW_GDTs.hxx>

#include <Draw.hxx>
#include <DBRep.hxx>
#include <DDocStd.hxx>

#include <STEPCAFControl_GDTProperty.hxx>

#include <TDF_Tool.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_GraphNode.hxx>

#include <XCAFDoc.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DimTol.hxx>
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDimTolObjects_DimensionObject.hxx>
#include <XCAFDoc_Datum.hxx>
#include <XCAFDimTolObjects_DatumObject.hxx>
#include <XCAFDoc_GeomTolerance.hxx>
#include <XCAFDimTolObjects_GeomToleranceObject.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDimTolObjects_DimensionObjectSequence.hxx>
#include <XCAFDimTolObjects_DatumObjectSequence.hxx>
#include <XCAFDimTolObjects_GeomToleranceObjectSequence.hxx>
#include <XCAFDimTolObjects_DatumSingleModif.hxx>
#include <XCAFDimTolObjects_DimensionModif.hxx>
#include <XCAFDimTolObjects_GeomToleranceModif.hxx>
#include <XCAFDimTolObjects_DatumModifiersSequence.hxx>
#include <XCAFDimTolObjects_DatumObject.hxx>
#include <XCAFDimTolObjects_Tool.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_HArray1OfPnt.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRep_Tool.hxx>



static Standard_Integer DumpDGTs (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XDumpDGTs Doc shape/label/all\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool= XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TCollection_AsciiString name = argv[2];
  TDF_LabelSequence aLabels;
  if(name.IsEqual("all"))
  {
    aShapeTool->GetShapes(aLabels);
    for ( Standard_Integer i=1; i <= aLabels.Length(); i++ )
    {
      aShapeTool->GetSubShapes(aLabels.Value(i), aLabels);
    }
  }
  else
  {
    TDF_Label aLabel;
    TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
    if ( !aLabel.IsNull() ) {
      aLabels.Append(aLabel);
      aShapeTool->GetSubShapes(aLabel, aLabels);
    }
    else
    {
      TopoDS_Shape aShape= DBRep::Get(argv[2]);
      if ( !aShape.IsNull() )
      {
        aShapeTool->Search(aShape, aLabel);
        if ( !aLabel.IsNull() ) {
          aLabels.Append(aLabel);
          aShapeTool->GetSubShapes(aLabel, aLabels);
        }
        else
        {
          di<<"Shape "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
          return 1;
        }
      }
    }
  }

  for ( Standard_Integer i=1; i <= aLabels.Length(); i++ )
  {
    Standard_Boolean flag = Standard_True;
    TDF_LabelSequence aGDTs;
    aDimTolTool->GetRefDimensionLabels(aLabels.Value(i), aGDTs);
    for ( Standard_Integer j=1; j <= aGDTs.Length(); j++ )
    {
      Handle(XCAFDoc_Dimension) aDimTol;
      if(aGDTs.Value(j).FindAttribute(XCAFDoc_Dimension::GetID(), aDimTol))
      {
        Handle(XCAFDimTolObjects_DimensionObject) aDimTolObj = aDimTol->GetObject();
        if(flag)
        {
          TCollection_AsciiString Entry;
          TDF_Tool::Entry(aLabels.Value(i), Entry);
          di << "\n " << Entry << " Shape."<< i;
          flag = Standard_False;
        }
        TCollection_AsciiString Entry;
        TDF_Tool::Entry(aGDTs.Value(j), Entry);
        di << "\n \t " << Entry;
        flag = Standard_False;

        di << " Dimension."<< i << "."<< j;
        if (argc > 3)
        {
          di <<" (";
          di << " T " << aDimTolObj->GetType();
          if(aDimTolObj->IsDimWithRange())
          {
            di << ", LB " << aDimTolObj->GetLowerBound();
            di << ", UB " << aDimTolObj->GetUpperBound();
          }
          else
          {
            di << ", V " << aDimTolObj->GetValue();
            if (aDimTolObj->IsDimWithPlusMinusTolerance())
            {
              di << ", VL " << aDimTolObj->GetLowerTolValue();
              di << ", VU " << aDimTolObj->GetUpperTolValue();
            }
            else if (aDimTolObj->IsDimWithClassOfTolerance())
            {
              Standard_Boolean isH;
              XCAFDimTolObjects_DimensionFormVariance aFV;
              XCAFDimTolObjects_DimensionGrade aG;
              aDimTolObj->GetClassOfTolerance(isH, aFV, aG);
              di << ", H " << (Standard_Integer)isH<< " F " << aFV << " G " << aG;
            }
          }
          if (aDimTolObj->HasQualifier())
            di << ", Q " << aDimTolObj->GetQualifier();
          if (aDimTolObj->GetType() == XCAFDimTolObjects_DimensionType_Location_Oriented)
          {
            gp_Dir aD;
            aDimTolObj->GetDirection(aD);
            di << ", D (" << aD.X() << ", " << aD.Y() << ", " << aD.Z() << ")";
          }
          XCAFDimTolObjects_DimensionModifiersSequence aModif = 
            aDimTolObj->GetModifiers();
          if (!aModif.IsEmpty())
          {
            di << ",";
            for (Standard_Integer k = aModif.Lower(); k <= aModif.Upper(); k++)
            {
              di << " M " << aModif.Value(k);
            }
          }
          di << ", P " << (Standard_Integer)!aDimTolObj->GetPath().IsNull();
          di << " )";
        }
      }
    }
    aGDTs.Clear();
    aDimTolTool->GetRefGeomToleranceLabels(aLabels.Value(i), aGDTs);
    for ( Standard_Integer j=1; j <= aGDTs.Length(); j++ )
    {
      Handle(XCAFDoc_GeomTolerance) aDimTol;
      if(aGDTs.Value(j).FindAttribute(XCAFDoc_GeomTolerance::GetID(), aDimTol))
      {
        Handle(XCAFDimTolObjects_GeomToleranceObject) aDimTolObj = aDimTol->GetObject();
        if(flag)
        {
          TCollection_AsciiString Entry;
          TDF_Tool::Entry(aLabels.Value(i), Entry);
          di << "\n " << Entry << " Shape."<< i;
          flag = Standard_False;
        }
        TCollection_AsciiString Entry;
        TDF_Tool::Entry(aGDTs.Value(j), Entry);
        di << "\n \t " << Entry;
        flag = Standard_False;

        di << " GeomTolerance."<< i << "."<< j;
        if (argc > 3)
        {
          di <<" (";
          di << " T " << aDimTolObj->GetType();
          di << " TV " << aDimTolObj->GetTypeOfValue();
          di << ", V " << aDimTolObj->GetValue();

          if (aDimTolObj->HasAxis())
          {
            gp_Ax2 anAx = aDimTolObj->GetAxis();
            di << ", A ( L (" << anAx.Location().X() << anAx.Location().Y() << anAx.Location().Z()
              << "), XD (" << anAx.XDirection().X() << anAx.XDirection().Y() << anAx.XDirection().Z()
              << "), RD (" << anAx.YDirection().X() << anAx.YDirection().Y() << anAx.YDirection().Z() << "))";
          }
          XCAFDimTolObjects_GeomToleranceModifiersSequence aModif = 
            aDimTolObj->GetModifiers();
          if (!aModif.IsEmpty())
          {
            di << ",";
            for (Standard_Integer k = aModif.Lower(); k <= aModif.Upper(); k++)
            {
              di << " M " << aModif.Value(k);
            }
          }
          if (aDimTolObj->GetMaterialRequirementModifier() != XCAFDimTolObjects_GeomToleranceMatReqModif_None)
          {
            di << ", MR " << aDimTolObj->GetMaterialRequirementModifier();
          }
          if (aDimTolObj->GetMaxValueModifier() > 0)
          {
            di << "MaxV " << aDimTolObj->GetMaxValueModifier();
          }
          if ( aDimTolObj->GetZoneModifier() != XCAFDimTolObjects_GeomToleranceZoneModif_None)
          {
            di << ", ZM " << aDimTolObj->GetZoneModifier();
            if (aDimTolObj->GetValueOfZoneModifier() > 0)
            {
              di << " ZMV " <<aDimTolObj->GetValueOfZoneModifier();
            }
          }

          di << " )";
        }
        Handle(XCAFDoc_GraphNode) aNode;
        if(aGDTs.Value(j).FindAttribute(XCAFDoc::DatumTolRefGUID(), aNode) && aNode->NbChildren() > 0)
        {
          for(Standard_Integer k = 1; k<=aNode->NbChildren(); k++)
          {
            Handle(XCAFDoc_Datum) aDatum;
            if(aNode->GetChild(k)->Label().FindAttribute(XCAFDoc_Datum::GetID(), aDatum))
            {
              Handle(XCAFDimTolObjects_DatumObject) aDatumObj = aDatum->GetObject();
              TCollection_AsciiString anEntry;
              TDF_Tool::Entry(aNode->GetChild(k)->Label(), anEntry);
              di << "\n \t \t " << anEntry;
              di << " Datum."<< i << "."<< j << "."<< k;
              if (argc > 3)
              {
                di <<" (";
                XCAFDimTolObjects_DatumModifiersSequence aModif = 
                  aDatumObj->GetModifiers();
                if (!aModif.IsEmpty())
                {
                  di << ",";
                  for (Standard_Integer iModif = aModif.Lower(); iModif <= aModif.Upper(); iModif++)
                  {
                    di << " M " << aModif.Value(iModif);
                  }
                }
                XCAFDimTolObjects_DatumModifWithValue aM;
                Standard_Real aV;
                aDatumObj->GetModifierWithValue(aM, aV);
                if (aM != XCAFDimTolObjects_DatumModifWithValue_None)
                {
                  di << ", MV" << aM << " " << aV; 
                }
                di << " )";
              }
            }
          }
        }
      }
    }
    TDF_LabelSequence aDatumL;
    if (aDimTolTool->GetRefDatumLabel(aLabels.Value(i), aDatumL))
    {
      for(Standard_Integer j = aDatumL.Lower(); j <= aDatumL.Upper(); j++)
      {
        Handle(XCAFDoc_Datum) aDatum;
        if(aDatumL.Value(j).FindAttribute(XCAFDoc_Datum::GetID(), aDatum) && 
           aDatum->GetObject()->IsDatumTarget())
        {
          Handle(XCAFDimTolObjects_DatumObject) aDatumObj = aDatum->GetObject();
          if(flag)
          {
            TCollection_AsciiString Entry;
            TDF_Tool::Entry(aLabels.Value(i), Entry);
            di << "\n " << Entry << " Shape."<< i;
            flag = Standard_False;
          }
          TCollection_AsciiString Entry;
          TDF_Tool::Entry(aDatumL.First(), Entry);
          di << "\n \t " << Entry;
          flag = Standard_False;

          di << " Datum target."<< i << "."<< j;
          if (argc > 3)
          {
            di <<" (";
            di << " T " << aDatumObj->GetDatumTargetType();
            if (aDatumObj->GetDatumTargetType() != XCAFDimTolObjects_DatumTargetType_Area)
            {
              gp_Ax2 anAx = aDatumObj->GetDatumTargetAxis();
               di << ", A ( L (" << anAx.Location().X() << anAx.Location().Y() << anAx.Location().Z()
                << "), XD (" << anAx.XDirection().X() << anAx.XDirection().Y() << anAx.XDirection().Z()
                << "), RD (" << anAx.YDirection().X() << anAx.YDirection().Y() << anAx.YDirection().Z() << "))";
              if (aDatumObj->GetDatumTargetType() != XCAFDimTolObjects_DatumTargetType_Point)
              {
                di << ", L " << aDatumObj->GetDatumTargetLength() ;
                if (aDatumObj->GetDatumTargetType() == XCAFDimTolObjects_DatumTargetType_Rectangle)
                {
                  di << ", W " << aDatumObj->GetDatumTargetWidth() ;
                }
              }
            }
            di << " )";
          }
        }
      }
    }
  }
  return 0;
}

static Standard_Integer DumpNbDGTs (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2) {
    di<<"Use: XDumpNbDGTs Doc";
    return 1;
  }

  Standard_Boolean isFull = Standard_False;
  if (argc == 3) {
    char aChar = argv[2][0];
    if (aChar == 'f')
      isFull = Standard_True;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool= XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_LabelSequence aLabels;
  aShapeTool->GetShapes(aLabels);
  for ( Standard_Integer i=1; i <= aLabels.Length(); i++ )
  {
    aShapeTool->GetSubShapes(aLabels.Value(i), aLabels);
  }

  TDF_LabelSequence aGDTs;
  aDimTolTool->GetDimensionLabels(aGDTs);
  di << "\n NbOfDimensions          : " << aGDTs.Length();
  if (isFull) {
    Standard_Integer nbSize = 0,
                     nbLocation = 0,
                     nbAngular = 0,
                     nbWithPath = 0,
                     nbCommon = 0;
    for (Standard_Integer i = 1; i <= aGDTs.Length(); i++) {
      Handle(XCAFDoc_Dimension) aDimAttr;
      if (!aGDTs.Value(i).FindAttribute(XCAFDoc_Dimension::GetID(),aDimAttr)) 
        continue;
      Handle(XCAFDimTolObjects_DimensionObject) anObject = aDimAttr->GetObject();
      if (anObject.IsNull())
        continue;
      XCAFDimTolObjects_DimensionType aDimType = anObject->GetType();
      if (aDimType == XCAFDimTolObjects_DimensionType_CommonLabel) {
        nbCommon++;
      }
      else if (STEPCAFControl_GDTProperty::IsDimensionalLocation(aDimType)) {
        nbLocation++;
      }
      else if (aDimType == XCAFDimTolObjects_DimensionType_Location_Angular) {
        nbAngular++;
        nbLocation++;
      }
      else if (aDimType == XCAFDimTolObjects_DimensionType_Location_WithPath) {
        nbLocation++;
        nbWithPath++;
      }
      else if (STEPCAFControl_GDTProperty::IsDimensionalSize(aDimType)) {
        nbSize++;
      }
      else if (aDimType == XCAFDimTolObjects_DimensionType_Size_Angular) {
        nbSize++;
        nbAngular++;
      }
      else if (aDimType == XCAFDimTolObjects_DimensionType_Size_WithPath) {
        nbSize++;
        nbWithPath++;
      }
    }
    di << "\n  NbOfDimensionalSize    : " << nbSize;
    di << "\n  NbOfDimensionalLocation: " << nbLocation;
    di << "\n  NbOfAngular            : " << nbAngular;
    di << "\n  NbOfWithPath           : " << nbWithPath;
    di << "\n  NbOfCommonLabels       : " << nbCommon;
  }

  aGDTs.Clear();
  aDimTolTool->GetGeomToleranceLabels(aGDTs);
  di << "\n NbOfTolerances          : " << aGDTs.Length();
  if (isFull) {
    Standard_Integer nbWithModif = 0,
                     nbWithMaxTol = 0,
                     nbWithDatumRef = 0;
    for (Standard_Integer i = 1; i <= aGDTs.Length(); i++) {
      Handle(XCAFDoc_GeomTolerance) aGTAttr;
      if (!aGDTs.Value(i).FindAttribute(XCAFDoc_GeomTolerance::GetID(),aGTAttr)) 
        continue;
      Handle(XCAFDimTolObjects_GeomToleranceObject) anObject = aGTAttr->GetObject();
      if (anObject.IsNull())
        continue;
      if (anObject->GetMaterialRequirementModifier() != XCAFDimTolObjects_GeomToleranceMatReqModif_None) {
        nbWithModif++;
      }
      else if (anObject->GetModifiers().Length() > 0) {
        Standard_Boolean isHasModif = Standard_False;
        for (Standard_Integer j = 1; j <= anObject->GetModifiers().Length(); j++)
          if (anObject->GetModifiers().Value(j) != XCAFDimTolObjects_GeomToleranceModif_All_Around &&
            anObject->GetModifiers().Value(j) != XCAFDimTolObjects_GeomToleranceModif_All_Over) {
            isHasModif = Standard_True;
            break;
          }
        if (isHasModif)
          nbWithModif++;
      }
      if (anObject->GetMaxValueModifier() != 0) {
        nbWithMaxTol++;
      }
      TDF_LabelSequence aDatumSeq;
      aDimTolTool->GetDatumWithObjectOfTolerLabels(aGDTs.Value(i), aDatumSeq);
      if (aDatumSeq.Length() > 0) {
        nbWithDatumRef++;
      }
    }
    di << "\n  NbOfGTWithModifiers    : " << nbWithModif;
    di << "\n  NbOfGTWithMaxTolerance : " << nbWithMaxTol;
    di << "\n  NbOfGTWithDatums       : " << nbWithDatumRef;
  }

  Standard_Integer aCounter = 0;
  Standard_Integer aCounter1 = 0;
   Standard_Integer aCounter2 = 0;

  for ( Standard_Integer i=1; i <= aLabels.Length(); i++ )
  {
    Standard_Boolean isDatum = Standard_False;
    TDF_LabelSequence aDatL;
    if(aDimTolTool->GetRefDatumLabel(aLabels.Value(i), aDatL))
    {
      for(Standard_Integer j = aDatL.Lower(); j <= aDatL.Upper(); j++)
      {
        Handle(XCAFDoc_Datum) aDat;
        if(aDatL.Value(j).FindAttribute(XCAFDoc_Datum::GetID(), aDat))
        {
          if(aDat->GetObject()->IsDatumTarget())
          {
            aCounter1++;
          }
          else
          {
            aCounter2++;
            isDatum = Standard_True;
          }
        }
      }
      if(isDatum)
        aCounter++;
    }
  }
  di << "\n NbOfDatumFeature        : " << aCounter;
  di << "\n NbOfAttachedDatum       : " << aCounter2;
  di << "\n NbOfDatumTarget         : " << aCounter1;

  return 0;
}

static Standard_Integer addDim (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XAddDimension Doc shape/label [shape/label]\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    TopoDS_Shape aShape= DBRep::Get(argv[2]);
    if ( !aShape.IsNull() )
    {
      aShapeTool->Search(aShape, aLabel);
      if ( aLabel.IsNull() )
      {
        di<<"Shape "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
        return 1;
      }
    }
  }
  TDF_Label aLabel1;
  if(argc == 4)
  {
    TDF_Tool::Label(Doc->GetData(), argv[3], aLabel1);
    if ( aLabel1.IsNull() ) 
    {
      TopoDS_Shape aShape= DBRep::Get(argv[3]);
      if ( !aShape.IsNull() )
      {
        aShapeTool->Search(aShape, aLabel1);
        if ( aLabel1.IsNull() )
        {
          di<<"Shape "<<argv[3]<<" is absent in "<<argv[1]<<"\n";
          return 1;
        }
      }
    }
  }

  TDF_Label aDimL = aDimTolTool->AddDimension();
  if(aLabel1.IsNull())
    aDimTolTool->SetDimension(aLabel, aDimL);
  else
    aDimTolTool->SetDimension(aLabel, aLabel1, aDimL);
  TCollection_AsciiString Entry;
  TDF_Tool::Entry(aDimL, Entry);
  di << Entry;
  return 0;
}

static Standard_Integer addGTol (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XAddGeomTolerance Doc shape/label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    TopoDS_Shape aShape= DBRep::Get(argv[2]);
    if ( !aShape.IsNull() )
    {
      aShapeTool->Search(aShape, aLabel);
      if ( aLabel.IsNull() )
      {
        di<<"Shape "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
        return 1;
      }
    }
  }

  TDF_Label aTolL = aDimTolTool->AddGeomTolerance();
  aDimTolTool->SetGeomTolerance(aLabel, aTolL);
  TCollection_AsciiString Entry;
  TDF_Tool::Entry(aTolL, Entry);
  di << Entry;
  return 0;
}

static Standard_Integer addDatum (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XAddDatum Doc shape1/label1 ... shapeN/labelN\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_LabelSequence aLabelSeq;
  for (Standard_Integer i = 2; i < argc; i++) {
    TDF_Label aLabel;
    TDF_Tool::Label(Doc->GetData(), argv[i], aLabel);
    if (aLabel.IsNull()) {
      TopoDS_Shape aShape = DBRep::Get(argv[i]);
      if (!aShape.IsNull())
        aShapeTool->Search(aShape, aLabel);
      if (aLabel.IsNull())
        continue;
    }
    aLabelSeq.Append(aLabel);
  }

  TDF_Label aDatumL = aDimTolTool->AddDatum();
  aDimTolTool->SetDatum(aLabelSeq, aDatumL);
  TCollection_AsciiString Entry;
  TDF_Tool::Entry(aDatumL, Entry);
  di << Entry;
  return 0;
}

static Standard_Integer setDatum (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XSetDatum Doc Datum_Label GeomTol_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Datum "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }

  TDF_Label aTol;
  TDF_Tool::Label(Doc->GetData(), argv[3], aTol);
  if ( aTol.IsNull() ) 
  {
    di<<"GeomTolerance "<<argv[3]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }

  // check datum position number
  Handle(XCAFDoc_Datum) aDatumAttr;
  if (!aLabel.FindAttribute(XCAFDoc_Datum::GetID(), aDatumAttr))
  {
    di<<"Invalid datum object\n";
    return 1;
  }
  Handle(XCAFDimTolObjects_DatumObject) aDatumObj = aDatumAttr->GetObject();
  if (aDatumObj.IsNull())
  {
    di<<"Invalid datum object\n";
    return 1;
  }

  if (aDatumObj->GetPosition() < 1 || aDatumObj->GetPosition() > 3)
  {
    di<<"Invalid datum position number: use XSetDatumPosition\n";
    return 1;
  }

  aDimTolTool->SetDatumToGeomTol(aLabel, aTol);
  return 0;
}

static Standard_Integer setDatumPosition (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XSetDatumPosition Doc Datum_Label position[1-3]\n";
    return 1;
  }

  if (Draw::Atoi(argv[3]) < 1 || Draw::Atoi(argv[3]) > 3) {
    di<<"Datum position should be 1, 2 or 3\n";
    return 1;
  }

  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Datum "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Datum) aDatum;
  if(aLabel.FindAttribute(XCAFDoc_Datum::GetID(), aDatum))
  {
    Handle(XCAFDimTolObjects_DatumObject) anObj = aDatum->GetObject();
    anObj->SetPosition(Draw::Atoi(argv[3]));
    aDatum->SetObject(anObj);
  }
  return 0;
}

static Standard_Integer getDatumPosition (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetDatumPosition Doc Datum_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Datum "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Datum) aDatum;
  if(aLabel.FindAttribute(XCAFDoc_Datum::GetID(), aDatum))
  {
    di << aDatum->GetObject()->GetPosition();
  }
  return 0;
}


static Standard_Integer getDatum (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetDatum Doc GeomTol_Label/Shape_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Label "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }

  TDF_LabelSequence aD;
  if(!aDimTolTool->GetRefDatumLabel(aLabel, aD))
  {
    aDimTolTool->GetDatumOfTolerLabels(aLabel, aD);
  }
  for(Standard_Integer i = aD.Lower(); i <= aD.Upper(); i++)
  {
    if(i>1) di<<", ";
    TCollection_AsciiString Entry;
    TDF_Tool::Entry(aD.Value(i), Entry);
    di<<Entry;
  }
  return 0;
}

static Standard_Integer addDatumModif (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XAddDatumModifier Doc Datum_Label mod1 mod2 ...\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Datum "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Datum) aDatum;
  if(aLabel.FindAttribute(XCAFDoc_Datum::GetID(), aDatum))
  {
    for(Standard_Integer i = 3; i < argc; i++)
    {
      if(Draw::Atoi(argv[i]) < 22 && Draw::Atoi(argv[i]) > -1)
      {
        Handle(XCAFDimTolObjects_DatumObject) anObj = aDatum->GetObject();
        anObj->AddModifier((XCAFDimTolObjects_DatumSingleModif)Draw::Atoi(argv[i]));
        aDatum->SetObject(anObj);
      }
    }
  }
  return 0;
}

static Standard_Integer getDatumModif (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetDatumModifiers Doc Datum_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Datum "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Datum) aDatum;
  if(aLabel.FindAttribute(XCAFDoc_Datum::GetID(), aDatum))
  {
    XCAFDimTolObjects_DatumModifiersSequence aS = aDatum->GetObject()->GetModifiers();
    for(Standard_Integer i = 1; i<=aS.Length();i++)
    {
      if (i > 1) di<<", ";
      switch(aS.Value(i)){
      case 0  : di<<"AnyCrossSection"; break;
      case 1  : di<<"AnyLongitudinalSection"; break;
      case 2  : di<<"Basic"; break;
      case 3  : di<<"ContactingFeature\n"; break;
      case 4  : di<<"DegreeOfFreedomConstraintU"; break;
      case 5  : di<<"DegreeOfFreedomConstraintV"; break;
      case 6  : di<<"DegreeOfFreedomConstraintW"; break;
      case 7  : di<<"DegreeOfFreedomConstraintX"; break;
      case 8  : di<<"DegreeOfFreedomConstraintY"; break;
      case 9  : di<<"DegreeOfFreedomConstraintZ"; break;
      case 10 : di<<"DistanceVariable"; break;
      case 11 : di<<"FreeState"; break;
      case 12 : di<<"LeastMaterialRequirement"; break;
      case 13 : di<<"Line"; break;
      case 14 : di<<"MajorDiameter"; break;
      case 15 : di<<"MaximumMaterialRequirement"; break;
      case 16 : di<<"MinorDiameter"; break;
      case 17 : di<<"Orientation"; break;
      case 18 : di<<"PitchDiameter"; break;
      case 19 : di<<"Plane"; break;
      case 20 : di<<"Point"; break;
      case 21 : di<<"Translation"; break;
      default : break;
      }
    }
  }
  return 0;
}

static Standard_Integer setDatumName (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XSetDatumName Doc Datum_Label name\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Datum "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Datum) aDatum;
  if(aLabel.FindAttribute(XCAFDoc_Datum::GetID(), aDatum))
  {
    Handle(XCAFDimTolObjects_DatumObject) anObj = aDatum->GetObject();
    anObj->SetName(new TCollection_HAsciiString(argv[3]));
    aDatum->SetObject(anObj);
  }
  return 0;
}

static Standard_Integer getDatumName (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetDatumName Doc Datum_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Datum "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Datum) aDatum;
  if(aLabel.FindAttribute(XCAFDoc_Datum::GetID(), aDatum))
  {
    di<<aDatum->GetObject()->GetName()->ToCString();
  }
  return 0;
}

static Standard_Integer setTypeOfTol (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XSetTypeOfTolerance Doc GTol_Label type\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"GeomTolerance "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if(aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
    if(Draw::Atoi(argv[3]) > -1 && Draw::Atoi(argv[3]) < 16)
    {
      Handle(XCAFDimTolObjects_GeomToleranceObject) anObj = aGeomTolerance->GetObject();
      anObj->SetType((XCAFDimTolObjects_GeomToleranceType)Draw::Atoi(argv[3]));
      aGeomTolerance->SetObject(anObj);
    }
  }
  return 0;
}

static Standard_Integer getTypeOfTol (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetTypeOfTolerance Doc GTol_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"GeomTolerance "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if(aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
      switch(aGeomTolerance->GetObject()->GetType()){
      case 0  : di<<"type is absent"; break;
      case 1  : di<<"Angularity"; break;
      case 2  : di<<"CircularRunout"; break;
      case 3  : di<<"CircularityOrRoundness"; break;
      case 4  : di<<"Coaxiality"; break;
      case 5  : di<<"Concentricity"; break;
      case 6  : di<<"Cylindricity"; break;
      case 7  : di<<"Flatness"; break;
      case 8  : di<<"Parallelism"; break;
      case 9  : di<<"Perpendicularity"; break;
      case 10 : di<<"Position"; break;
      case 11 : di<<"ProfileOfLine"; break;
      case 12 : di<<"ProfileOfSurface"; break;
      case 13 : di<<"Straightness"; break;
      case 14 : di<<"Symmetry"; break;
      case 15 : di<<"TotalRunout"; break;
      default : break;
      }
  }
  return 0;
}

static Standard_Integer setTypeOfTolVal (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XSetTypeOfToleranceValue Doc GTol_Label type\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"GeomTolerance "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if(aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
    if(Draw::Atoi(argv[3]) > -1 && Draw::Atoi(argv[3]) < 3)
    {
      Handle(XCAFDimTolObjects_GeomToleranceObject) anObj = aGeomTolerance->GetObject();
      anObj->SetTypeOfValue((XCAFDimTolObjects_GeomToleranceTypeValue)Draw::Atoi(argv[3]));
      aGeomTolerance->SetObject(anObj);
    }
  }
  return 0;
}

static Standard_Integer getTypeOfTolVal (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetTypeOfToleranceValue Doc GTol_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"GeomTolerance "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if(aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
    switch(aGeomTolerance->GetObject()->GetTypeOfValue()){
      case 0  : di<<"type is absent"; break;
      case 1  : di<<"Diameter"; break;
      case 2  : di<<"SphericalDiameter"; break;
      default : break;
      }
  }
  return 0;
}

static Standard_Integer setTolVal (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XSetToleranceValue Doc GTol_Label value\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"GeomTolerance "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if(aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
    Handle(XCAFDimTolObjects_GeomToleranceObject) anObj = aGeomTolerance->GetObject();
    anObj->SetValue(Draw::Atof(argv[3]));
    aGeomTolerance->SetObject(anObj);
  }
  return 0;
}

static Standard_Integer getTolVal (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetToleranceValue Doc GTol_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"GeomTolerance "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if(aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
    di << aGeomTolerance->GetObject()->GetValue();
  }
  return 0;
}

static Standard_Integer setMatReq (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XSetTolMaterialReq Doc GTol_Label mod\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"GeomTolerance "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if(aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
    if(Draw::Atoi(argv[3]) > -1 && Draw::Atoi(argv[3]) < 3)
    {
      Handle(XCAFDimTolObjects_GeomToleranceObject) anObj = aGeomTolerance->GetObject();
      anObj->SetMaterialRequirementModifier((XCAFDimTolObjects_GeomToleranceMatReqModif)Draw::Atoi(argv[3]));
      aGeomTolerance->SetObject(anObj);
    }
  }
  return 0;
}

static Standard_Integer getMatReq (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetTolMaterialReq Doc GTol_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"GeomTolerance "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if(aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
      switch(aGeomTolerance->GetObject()->GetMaterialRequirementModifier()){
      case 0  : di<<"modifier is absent"; break;
      case 1  : di<<"M"; break;
      case 2  : di<<"L"; break;
      default : break;
      }
  }
  return 0;
}

static Standard_Integer setZoneMod (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XSetTolZoneMod Doc GTol_Label mod\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"GeomTolerance "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if(aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
    if(Draw::Atoi(argv[3]) > -1 && Draw::Atoi(argv[3]) < 3)
    {
      Handle(XCAFDimTolObjects_GeomToleranceObject) anObj = aGeomTolerance->GetObject();
      anObj->SetZoneModifier((XCAFDimTolObjects_GeomToleranceZoneModif)Draw::Atoi(argv[3]));
      aGeomTolerance->SetObject(anObj);
    }
  }
  return 0;
}

static Standard_Integer getZoneMod (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetTolZoneMod Doc GTol_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"GeomTolerance "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if(aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
      switch(aGeomTolerance->GetObject()->GetZoneModifier()){
      case 0  : di<<"modifier is absent"; break;
      case 1  : di<<"P"; break;
      case 2  : di<<"NonUniform"; break;
      default : break;
      }
  }
  return 0;
}

static Standard_Integer setZoneModVal (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XSetTolZoneModValue Doc GTol_Label val\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"GeomTolerance "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if(aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
    Handle(XCAFDimTolObjects_GeomToleranceObject) anObj = aGeomTolerance->GetObject();
    anObj->SetValueOfZoneModifier(Draw::Atof(argv[3]));
    aGeomTolerance->SetObject(anObj);
  }
  return 0;
}

static Standard_Integer getZoneModVal (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetTolZoneModValue Doc GTol_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"GeomTolerance "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if(aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
    di << aGeomTolerance->GetObject()->GetValueOfZoneModifier();
  }
  return 0;
}

static Standard_Integer addTolModif (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XAddTolModifier Doc Tol_Label mod1 mod2 ...\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"GeomTolerance "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if(aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
    for(Standard_Integer i = 3; i < argc; i++)
    {
      if(Draw::Atoi(argv[i]) > -1 && Draw::Atoi(argv[i]) < 17)
      {
        Handle(XCAFDimTolObjects_GeomToleranceObject) anObj = aGeomTolerance->GetObject();
        anObj->AddModifier((XCAFDimTolObjects_GeomToleranceModif)Draw::Atoi(argv[i]));
        aGeomTolerance->SetObject(anObj);
      }
    }
  }
  return 0;
}

static Standard_Integer getTolModif (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetTolModifiers Doc Tol_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"GeomTolerance "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if(aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
    XCAFDimTolObjects_GeomToleranceModifiersSequence aS  = aGeomTolerance->GetObject()->GetModifiers();
    for(Standard_Integer i = 1; i <= aS.Length(); i++)
    {
      if (i > 1) di<<", ";
      switch(aS.Value(i)){
      case 0  : di<<"Any_Cross_Section"; break;
      case 1  : di<<"Common_Zone"; break;
      case 2  : di<<"Each_Radial_Element"; break;
      case 3  : di<<"Free_State"; break;
      case 4  : di<<"Least_Material_Requirement"; break;
      case 5  : di<<"Line_Element"; break;
      case 6  : di<<"Major_Diameter"; break;
      case 7  : di<<"Maximum_Material_Requirement"; break;
      case 8  : di<<"Minor_Diameter"; break;
      case 9  : di<<"Not_Convex"; break;
      case 10 : di<<"Pitch_Diameter"; break;
      case 11 : di<<"Reciprocity_Requirement"; break;
      case 12 : di<<"Separate_Requirement"; break;
      case 13 : di<<"Statistical_Tolerance"; break;
      case 14 : di<<"Tangent_Plane"; break;
      default : break;
      }
    }
  }
  return 0;
}

static Standard_Integer setTolMaxVal (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XSetTolMaxValue Doc Dim_Label val\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"GeomTolerance "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if(aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
    Handle(XCAFDimTolObjects_GeomToleranceObject) anObj = aGeomTolerance->GetObject();
    anObj->SetMaxValueModifier(Draw::Atof(argv[3]));
    aGeomTolerance->SetObject(anObj);
  }
  return 0;
}

static Standard_Integer getTolMaxVal (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetTolMaxValue Doc Dim_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"GeomTolerance "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if(aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
    di << aGeomTolerance->GetObject()->GetMaxValueModifier();
  }
  return 0;
}

static Standard_Integer setDimType (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XSetDimensionType Doc Dim_Label type\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    if(Draw::Atoi(argv[3]) > -1 && Draw::Atoi(argv[3]) < 30)
    {
      Handle(XCAFDimTolObjects_DimensionObject) anObj = aDimension->GetObject();
      anObj->SetType((XCAFDimTolObjects_DimensionType)Draw::Atoi(argv[3]));
      aDimension->SetObject(anObj);
    }
  }
  return 0;
}

static Standard_Integer getDimType (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetDimensionType Doc Dim_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
      switch(aDimension->GetObject()->GetType()){
      case 0  : di<<"type is absent"; break;
      case 1  : di<<"Location_CurvedDistance"; break;
      case 2  : di<<"Location_LinearDistance"; break;
      case 3  : di<<"Location_LinearDistance_FromCenterToOuter"; break;
      case 4  : di<<"Location_LinearDistance_FromCenterToInner"; break;
      case 5  : di<<"Location_LinearDistance_FromOuterToCenter"; break;
      case 6  : di<<"Location_LinearDistance_FromOuterToOuter"; break;
      case 7  : di<<"Location_LinearDistance_FromOuterToInner"; break;
      case 8  : di<<"Location_LinearDistance_FromInnerToCenter"; break;
      case 9  : di<<"Location_LinearDistance_FromInnerToOuter"; break;
      case 10 : di<<"Location_LinearDistance_FromInnerToInner"; break;
      case 11 : di<<"Location_Angular"; break;
      case 12 : di<<"Location_Oriented"; break;
      case 13 : di<<"Location_WithPath"; break;
      case 14 : di<<"Size_CurveLength"; break;
      case 15 : di<<"Size_Diameter"; break;
      case 16 : di<<"Size_SphericalDiameter"; break;
      case 17 : di<<"Size_Radius"; break;
      case 18 : di<<"Size_SphericalRadius"; break;
      case 19 : di<<"Size_ToroidalMinorDiameter"; break;
      case 20 : di<<"Size_ToroidalMajorDiameter"; break;
      case 21 : di<<"Size_ToroidalMinorRadius"; break;
      case 22 : di<<"Size_ToroidalMajorRadius"; break;
      case 23 : di<<"Size_ToroidalHighMajorDiameter"; break;
      case 24 : di<<"Size_ToroidalLowMajorDiameter"; break;
      case 25 : di<<"Size_ToroidalHighMajorRadius"; break;
      case 26 : di<<"Size_ToroidalLowMajorRadius"; break;
      case 27 : di<<"Size_Thickness"; break;
      case 28 : di<<"Size_Angular"; break;
      case 29 : di<<"Size_WithPath"; break;
      default : break;
      }
  }
  return 0;
}

static Standard_Integer setDimVal (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XSetDimensionValue Doc Dim_Label val\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    Handle(XCAFDimTolObjects_DimensionObject) anObj = aDimension->GetObject();
    anObj->SetValue(Draw::Atof(argv[3]));
    aDimension->SetObject(anObj);
  }
  return 0;
}

static Standard_Integer getDimVal (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetDimensionValue Doc Dim_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    di << aDimension->GetObject()->GetValue();
  }
  return 0;
}

static Standard_Integer setDimQalif (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XSetDimensionQualifier Doc Dim_Label val\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    if(Draw::Atoi(argv[3]) > -1 && Draw::Atoi(argv[3]) < 4)
    {
      Handle(XCAFDimTolObjects_DimensionObject) anObj = aDimension->GetObject();
      anObj->SetQualifier((XCAFDimTolObjects_DimensionQualifier)Draw::Atoi(argv[3]));
      aDimension->SetObject(anObj);
    }
  }
  return 0;
}

static Standard_Integer getDimQalif (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetDimensionQualifier Doc Dim_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    
    switch(aDimension->GetObject()->GetQualifier()){
      case 0  : di<<"type is absent"; break;
      case 1  : di<<"Min"; break;
      case 2  : di<<"Max"; break;
      case 3  : di<<"Avg"; break;
      default : break;
      }
  }
  return 0;
}

static Standard_Integer setDimRange (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 5) {
    di<<"Use: XSetDimensionRange Doc Dim_Label low_val up_val\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    if(Draw::Atof(argv[3]) < Draw::Atof(argv[4]))
    {
      Handle(XCAFDimTolObjects_DimensionObject) anObj = aDimension->GetObject();
      anObj->SetLowerBound(Draw::Atof(argv[3]));
      anObj->SetUpperBound(Draw::Atof(argv[4]));
      aDimension->SetObject(anObj);
    }
  }
  return 0;
}

static Standard_Integer getDimRange (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetDimensionRange Doc Dim_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    di << "lower " << aDimension->GetObject()->GetLowerBound();
    di << " upper " << aDimension->GetObject()->GetUpperBound();
  }
  return 0;
}

static Standard_Integer setDimPlusMinusTol (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 5) {
    di<<"Use: XSetDimensionPlusMinusTol Doc Dim_Label low_val up_val\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    if(Draw::Atof(argv[3]) < Draw::Atof(argv[4]))
    {
      Handle(XCAFDimTolObjects_DimensionObject) anObj = aDimension->GetObject();
      anObj->SetLowerTolValue(Draw::Atof(argv[3]));
      anObj->SetUpperTolValue(Draw::Atof(argv[4]));
      aDimension->SetObject(anObj);
    }
  }
  return 0;
}

static Standard_Integer getDimPlusMinusTol (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetDimensionPlusMinusTol Doc Dim_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    di << "lower " << aDimension->GetObject()->GetLowerTolValue();
    di << " upper " << aDimension->GetObject()->GetUpperTolValue();
  }
  return 0;
}

static Standard_Integer setDimClassTol (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 6) {
    di<<"Use: XSetDimensionClassOfTol Doc Dim_Label ishole[1/0] formVar grade\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    if(Draw::Atoi(argv[4]) > 0 && Draw::Atoi(argv[4]) < 29 && Draw::Atoi(argv[5]) > -1 && Draw::Atoi(argv[5]) < 20)
    {
      Handle(XCAFDimTolObjects_DimensionObject) anObj = aDimension->GetObject();
      anObj->SetClassOfTolerance((Draw::Atoi(argv[3]) != 0), (XCAFDimTolObjects_DimensionFormVariance)Draw::Atoi(argv[4]), (XCAFDimTolObjects_DimensionGrade)Draw::Atoi(argv[5]));
      aDimension->SetObject(anObj);
    }
  }
  return 0;
}

static Standard_Integer getDimClassTol (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetDimensionClassOfTol Doc Dim_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    Standard_Boolean h;
    XCAFDimTolObjects_DimensionFormVariance f;
    XCAFDimTolObjects_DimensionGrade g;
    if(aDimension->GetObject()->GetClassOfTolerance(h ,f, g))
    {
      if(h)
      {
        switch(f){
        case 1  : di<<"a";break;
        case 2  : di<<"b";break;
        case 3  : di<<"c";break;
        case 4  : di<<"cd";break;
        case 5  : di<<"d";break;
        case 6  : di<<"e";break;
        case 7  : di<<"ef";break;
        case 8  : di<<"f";break;
        case 9  : di<<"fg";break;
        case 10 : di<<"g";break;
        case 11 : di<<"h";break;
        case 12 : di<<"js";break;
        case 13 : di<<"j";break;
        case 14 : di<<"k";break;
        case 15 : di<<"m";break;
        case 16 : di<<"n";break;
        case 17 : di<<"p";break;
        case 18 : di<<"r";break;
        case 19 : di<<"s";break;
        case 20 : di<<"t";break;
        case 21 : di<<"u";break;
        case 22 : di<<"v";break;
        case 23 : di<<"x";break;
        case 24 : di<<"y";break;
        case 25 : di<<"z";break;
        case 26 : di<<"a";break;
        case 27 : di<<"zb";break;
        case 28 : di<<"zc";break;
        default : break;
        }
      }
      else
      {
        switch(f){
        case 1  : di<<"A";break;
        case 2  : di<<"B";break;
        case 3  : di<<"C";break;
        case 4  : di<<"CD";break;
        case 5  : di<<"D";break;
        case 6  : di<<"E";break;
        case 7  : di<<"EF";break;
        case 8  : di<<"F";break;
        case 9  : di<<"FG";break;
        case 10 : di<<"G";break;
        case 11 : di<<"H";break;
        case 12 : di<<"JS";break;
        case 13 : di<<"J";break;
        case 14 : di<<"K";break;
        case 15 : di<<"M";break;
        case 16 : di<<"N";break;
        case 17 : di<<"P";break;
        case 18 : di<<"R";break;
        case 19 : di<<"S";break;
        case 20 : di<<"T";break;
        case 21 : di<<"U";break;
        case 22 : di<<"V";break;
        case 23 : di<<"X";break;
        case 24 : di<<"Y";break;
        case 25 : di<<"Z";break;
        case 26 : di<<"ZA";break;
        case 27 : di<<"ZB";break;
        case 28 : di<<"ZC";break;
        default : break;
        }
      }
      switch(g){
      case 0  : di<<"01"; break;
      case 1  : di<<"0"; break;
      case 2  : di<<"1"; break;
      case 3  : di<<"2"; break;
      case 4  : di<<"3"; break;
      case 5  : di<<"4"; break;
      case 6  : di<<"5"; break;
      case 7  : di<<"6"; break;
      case 8  : di<<"7"; break;
      case 9  : di<<"8"; break;
      case 10 : di<<"9"; break;
      case 11 : di<<"10"; break;
      case 12 : di<<"11"; break;
      case 13 : di<<"12"; break;
      case 14 : di<<"13"; break;
      case 15 : di<<"14"; break;
      case 16 : di<<"15"; break;
      case 17 : di<<"16"; break;
      case 18 : di<<"17"; break;
      case 19 : di<<"18"; break;
      default : break;
      }
    }
  }
  return 0;
}

static Standard_Integer setDimNbOfDecimalPlaces (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 5) {
    di<<"Use: XSetDimensionNbOfDecimalPlaces Doc Dim_Label l_val r_val\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    Handle(XCAFDimTolObjects_DimensionObject) anObj = aDimension->GetObject();
    anObj->SetNbOfDecimalPlaces(Draw::Atoi(argv[3]), Draw::Atoi(argv[4]));
    aDimension->SetObject(anObj);
  }
  return 0;
}

static Standard_Integer getDimNbOfDecimalPlaces (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetDimensionNbOfDecimalPlaces Doc Dim_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    Standard_Integer l, r;
    aDimension->GetObject()->GetNbOfDecimalPlaces(l,r);
    di << l << "." << r;
  }
  return 0;
}

static Standard_Integer addDimModifier (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XAddDimensionModifiers Doc Dim_Label mod1 mod2 ...\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    for(Standard_Integer i = 3; i < argc; i++)
    {
      if(Draw::Atoi(argv[i]) > -1 && Draw::Atoi(argv[i]) < 24)
      {
        Handle(XCAFDimTolObjects_DimensionObject) anObj = aDimension->GetObject();
        anObj->AddModifier((XCAFDimTolObjects_DimensionModif)Draw::Atoi(argv[i]));
        aDimension->SetObject(anObj);
      }
    }
  }
  return 0;
}

static Standard_Integer getDimModifier (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetDimensionModifiers Doc Dim_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    XCAFDimTolObjects_DimensionModifiersSequence aS = aDimension->GetObject()->GetModifiers();
    for(Standard_Integer i = 1; i <= aS.Length(); i++)
    {
      if (i > 1) di<<", ";
      switch(aS.Value(i)){
      case 0  : di<<"ControlledRadius"; break;
      case 1  : di<<"Square"; break;
      case 2  : di<<"StatisticalTolerance"; break;
      case 3  : di<<"ContinuousFeature"; break;
      case 4  : di<<"TwoPointSize"; break;
      case 5  : di<<"LocalSizeDefinedBySphere"; break;
      case 6  : di<<"LeastSquaresAssociationCriterion"; break;
      case 7  : di<<"MaximumInscribedAssociation"; break;
      case 8  : di<<"MinimumCircumscribedAssociation"; break;
      case 9  : di<<"CircumferenceDiameter"; break;
      case 10 : di<<"AreaDiameter"; break;
      case 11 : di<<"VolumeDiameter"; break;
      case 12 : di<<"MaximumSize"; break;
      case 13 : di<<"MinimumSize"; break;
      case 14 : di<<"AverageSize"; break;
      case 15 : di<<"MedianSize"; break;
      case 16 : di<<"MidRangeSize"; break;
      case 17 : di<<"RangeOfSizes"; break;
      case 18 : di<<"AnyRestrictedPortionOfFeature"; break;
      case 19 : di<<"AnyCrossSection"; break;
      case 20 : di<<"SpecificFixedCrossSection"; break;
      case 21 : di<<"CommonTolerance"; break;
      case 22 : di<<"FreeStateCondition"; break;
      case 23 : di<<"Between"; break;
      default : break;
      }
    }
  }
  return 0;
}

static Standard_Integer addDimPath (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XSetDimensionPath Doc Dim_Label path(edge)\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    TopoDS_Edge aE = TopoDS::Edge(DBRep::Get(argv[3],TopAbs_EDGE));
    if(!aE.IsNull())
    {
      Handle(XCAFDimTolObjects_DimensionObject) anObj = aDimension->GetObject();
      anObj->SetPath(aE);
      aDimension->SetObject(anObj);
    }
  }
  return 0;
}

static Standard_Integer addDimPoints (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XSetDimensionPoints Doc Dim_Label v1 [v2]\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    Handle(XCAFDimTolObjects_DimensionObject) anObj = aDimension->GetObject();

    TopoDS_Vertex aV1 = TopoDS::Vertex(DBRep::Get(argv[3],TopAbs_VERTEX));
    if(!aV1.IsNull()) {
      anObj->SetPoint(BRep_Tool::Pnt(aV1));
    }
    if (argc == 5) {
      TopoDS_Vertex aV2 = TopoDS::Vertex(DBRep::Get(argv[4],TopAbs_VERTEX));
      if(!aV2.IsNull()) {
        anObj->SetPoint2(BRep_Tool::Pnt(aV2));
      }
    }
    aDimension->SetObject(anObj);
  }
  return 0;
}

static Standard_Integer getDimPoints (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetDimensionPoints Doc Dim_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    Handle(XCAFDimTolObjects_DimensionObject) anObj = aDimension->GetObject();
    if(anObj->HasPoint()) {
      di << anObj->GetPoint().X() << ";" << anObj->GetPoint().Y() << ";" << anObj->GetPoint().Z() << " ";
    }
    if(anObj->HasPoint2()) {
      di << anObj->GetPoint2().X() << ";" << anObj->GetPoint2().Y() << ";" << anObj->GetPoint2().Z();
    }
 }
  return 0;
}

static Standard_Integer addDimDir (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 6) {
    di<<"Use: XSetDimensionDir Doc Dim_Label x y z\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    Handle(XCAFDimTolObjects_DimensionObject) anObj = aDimension->GetObject();
    anObj->SetDirection(gp_Dir(Draw::Atof(argv[3]),Draw::Atof(argv[4]),Draw::Atof(argv[5])));
    aDimension->SetObject(anObj);
  }
  return 0;
}

static Standard_Integer getDimDir (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: XGetDimensionDir Doc Dim_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool= XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di<<"Dimension "<<argv[2]<<" is absent in "<<argv[1]<<"\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    gp_Dir dir;
    if(aDimension->GetObject()->GetDirection(dir))
    {
      di << dir.X()<< ";"<< dir.Y()<< ";"<<dir.Z(); 
    }
  }
  return 0;
}

static Standard_Integer addDimDescr (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"Use: XAddDimensionDescr Doc Dim_Label Description [DescriptionName]\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di << "Dimension "<< argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    Handle(XCAFDimTolObjects_DimensionObject) anObj = aDimension->GetObject();
    Handle(TCollection_HAsciiString) aDescription = new TCollection_HAsciiString(argv[3]);
    Handle(TCollection_HAsciiString) aDescrName = (argc == 4) ? new TCollection_HAsciiString()
        : new TCollection_HAsciiString(argv[4]);
    anObj->AddDescription(aDescription, aDescrName);
    aDimension->SetObject(anObj);
  }
  return 0;
}

static Standard_Integer getDimDescr (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetDimensionDescr Doc Dim_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di << "Dimension "<< argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_Dimension) aDimension;
  if(aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    Handle(XCAFDimTolObjects_DimensionObject) anObject = aDimension->GetObject();
    for (Standard_Integer i = 0; i < anObject->NbDescriptions(); i++) {
      Handle(TCollection_HAsciiString) aDescription = anObject->GetDescription(i);
      Handle(TCollection_HAsciiString) aDescrName = anObject->GetDescriptionName(i);
      di << "name: " << aDescrName->ToCString() << " description: " << aDescription->ToCString() << "\n";
    }
  }
  return 0;
}

static Standard_Integer addGDTPosition (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 12) {
    di << "Use: XSetGDTPosition Doc GDT_Label loc_x loc_y loc_z normal_x normal_y normal_z xdir_x xdir_y xdir_z\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di << "GDT " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  
  gp_Pnt aPoint(Draw::Atof(argv[3]), Draw::Atof(argv[4]), Draw::Atof(argv[5]));
  gp_Dir aNormal(Draw::Atof(argv[6]), Draw::Atof(argv[7]), Draw::Atof(argv[8]));
  gp_Dir aDir(Draw::Atof(argv[9]), Draw::Atof(argv[10]), Draw::Atof(argv[11]));
  gp_Ax2 aPlane(aPoint, aNormal, aDir);
  // Dimension
  Handle(XCAFDoc_Dimension) aDimension;
  if (aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    Handle(XCAFDimTolObjects_DimensionObject) anObj = aDimension->GetObject();
    anObj->SetPlane(aPlane);
    anObj->SetPointTextAttach(aPoint);
    aDimension->SetObject(anObj);
  }
  // Geometric Tolerance
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if (aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
    Handle(XCAFDimTolObjects_GeomToleranceObject) anObj = aGeomTolerance->GetObject();
    anObj->SetPlane(aPlane);
    anObj->SetPointTextAttach(aPoint);
    aGeomTolerance->SetObject(anObj);
  }
  // Datum
  Handle(XCAFDoc_Datum) aDatum;
  if (aLabel.FindAttribute(XCAFDoc_Datum::GetID(), aDatum))
  {
    Handle(XCAFDimTolObjects_DatumObject) anObj = aDatum->GetObject();
    anObj->SetPlane(aPlane);
    anObj->SetPointTextAttach(aPoint);
    aDatum->SetObject(anObj);
  }
  return 0;
}

static Standard_Integer getGDTPosition (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetGDTPosition Doc GDT_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di << "GDT " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  gp_Pnt aPoint;
  gp_Dir aNormal, aDir;
  // Dimension
  Handle(XCAFDoc_Dimension) aDimension;
  if (aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    Handle(XCAFDimTolObjects_DimensionObject) anObj = aDimension->GetObject();
    aPoint = anObj->GetPointTextAttach();
    aNormal = anObj->GetPlane().Direction();
    aDir = anObj->GetPlane().XDirection();
  }
  // Geometric Tolerance
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if (aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
    Handle(XCAFDimTolObjects_GeomToleranceObject) anObj = aGeomTolerance->GetObject();
    aPoint = anObj->GetPointTextAttach();
    aNormal = anObj->GetPlane().Direction();
    aDir = anObj->GetPlane().XDirection();
  }
  // Datum
  Handle(XCAFDoc_Datum) aDatum;
  if (aLabel.FindAttribute(XCAFDoc_Datum::GetID(), aDatum))
  {
    Handle(XCAFDimTolObjects_DatumObject) anObj = aDatum->GetObject();
    aPoint = anObj->GetPointTextAttach();
    aNormal = anObj->GetPlane().Direction();
    aDir = anObj->GetPlane().XDirection();
  }

  di << "position: " << aPoint.X() << " " << aPoint.Y() << " " << aPoint.Z() << "\n";
  di << "normal: " << aNormal.X() << " " << aNormal.Y() << " " << aNormal.Z() << "\n";
  di << "x_direction: " << aDir.X() << " " << aDir.Y() << " " << aDir.Z() << "\n";
  return 0;
}

static Standard_Integer addGDTPresentation (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 5) {
    di << "Use: XSetGDTPresentation Doc GDT_Label Shape Name\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di << "GDT " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  
  TopoDS_Shape aPresentation= DBRep::Get(argv[3]);
  Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString(argv[4]);
  // Dimension
  Handle(XCAFDoc_Dimension) aDimension;
  if (aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    Handle(XCAFDimTolObjects_DimensionObject) anObj = aDimension->GetObject();
    anObj->SetPresentation(aPresentation, aName);
    aDimension->SetObject(anObj);
  }
  // Geometric Tolerance
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if (aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
    Handle(XCAFDimTolObjects_GeomToleranceObject) anObj = aGeomTolerance->GetObject();
    anObj->SetPresentation(aPresentation, aName);
    aGeomTolerance->SetObject(anObj);
  }
  // Datum
  Handle(XCAFDoc_Datum) aDatum;
  if (aLabel.FindAttribute(XCAFDoc_Datum::GetID(), aDatum))
  {
    Handle(XCAFDimTolObjects_DatumObject) anObj = aDatum->GetObject();
    anObj->SetPresentation(aPresentation, aName);
    aDatum->SetObject(anObj);
  }
  return 0;
}

static Standard_Integer getGDTPresentation (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetGDTPresentation Doc GDT_Label Shape\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) 
  {
    di << "GDT " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  TopoDS_Shape aPresentation;
  // Dimension
  Handle(XCAFDoc_Dimension) aDimension;
  if (aLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    Handle(XCAFDimTolObjects_DimensionObject) anObj = aDimension->GetObject();
    aPresentation = anObj->GetPresentation();
  }
  // Geometric Tolerance
  Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
  if (aLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGeomTolerance))
  {
    Handle(XCAFDimTolObjects_GeomToleranceObject) anObj = aGeomTolerance->GetObject();
    aPresentation = anObj->GetPresentation();
  }
  // Datum
  Handle(XCAFDoc_Datum) aDatum;
  if (aLabel.FindAttribute(XCAFDoc_Datum::GetID(), aDatum))
  {
    Handle(XCAFDimTolObjects_DatumObject) anObj = aDatum->GetObject();
    aPresentation = anObj->GetPresentation();
  }

  DBRep::Set (argv[3], aPresentation);
  return 0;
}

//=======================================================================
//function : InitCommands
//purpose  : 
//=======================================================================

void XDEDRAW_GDTs::InitCommands(Draw_Interpretor& di) 
{
  static Standard_Boolean initactor = Standard_False;
  if (initactor)
  {
    return;
  }
  initactor = Standard_True;
  
  Standard_CString g = "XDE G&DTs commands";

  di.Add ("XDumpDGTs","XDumpDGTs Doc shape/label/all ",
    __FILE__, DumpDGTs, g);

  di.Add ("XDumpNbDGTs","XDumpNbDGTs Doc [f (full dumping)]",
    __FILE__, DumpNbDGTs, g);

  di.Add ("XAddDimension","XAddDimension Doc shape/label [shape/label]",
    __FILE__, addDim, g);

  di.Add ("XAddGeomTolerance","XAddGeomTolerance Doc shape/label",
    __FILE__, addGTol, g);

  di.Add ("XAddDatum","XAddDatum Doc shape1/label1 ... shapeN/labelN",
    __FILE__, addDatum, g);

  di.Add ("XSetDatum","XSetDatum Doc Datum_Label GeomTol_Label",
    __FILE__, setDatum, g);

  di.Add ("XGetDatum","XGetDatum Doc GeomTol_Label/Shape_Label",
    __FILE__, getDatum, g);

  di.Add ("XAddDatumModifier","XAddDatumModifier Doc Datum_Label mod1 mod2 ...\n"
    "Values:"
    "\n  0 AnyCrossSection"
    "\n  1 Any_LongitudinalSection"
    "\n  2 Basic"
    "\n  3 ContactingFeature"
    "\n  4 DegreeOfFreedomConstraintU"
    "\n  5 DegreeOfFreedomConstraintV"
    "\n  6 DegreeOfFreedomConstraintW"
    "\n  7 DegreeOfFreedomConstraintX"
    "\n  8 DegreeOfFreedomConstraintY"
    "\n  9 DegreeOfFreedomConstraintZ"
    "\n 10 DistanceVariable"
    "\n 11 FreeState"
    "\n 12 LeastMaterialRequirement"
    "\n 13 Line"
    "\n 14 MajorDiameter"
    "\n 15 MaximumMaterialRequirement"
    "\n 16 MinorDiameter"
    "\n 17 Orientation"
    "\n 18 PitchDiameter"
    "\n 19 Plane"
    "\n 20 Point"
    "\n 21 Translation",
    __FILE__, addDatumModif, g);

  di.Add ("XGetDatumModifiers","XGetDatumModifiers Doc Datum_Label",
    __FILE__, getDatumModif, g);

  di.Add ("XSetDatumName","XSetDatumName Doc Datum_Label name",
    __FILE__, setDatumName, g);

  di.Add ("XGetDatumName","XGetDatumName Doc Datum_Label",
    __FILE__, getDatumName, g);

  di.Add ("XSetDatumPosition","XSetDatumPosition Doc Datum_Label position[1-3]"
      "Set datum position number in geometric tolerance datum system",
    __FILE__, setDatumPosition, g);

  di.Add ("XGetDatumPosition","XGetDatumPosition Doc Datum_Label",
    __FILE__, getDatumPosition, g);

  di.Add ("XSetTypeOfTolerance","XSetTypeOfTolerance Doc GTol_Label type"
        "Values:\n"
      "\t  0 type is absent\n"
      "\t  1 Angularity\n"
      "\t  2 CircularRunout\n"
      "\t  3 CircularityOrRoundness\n"
      "\t  4 Coaxiality\n"
      "\t  5 Concentricity\n"
      "\t  6 Cylindricity\n"
      "\t  7 Flatness\n"
      "\t  8 Parallelism\n"
      "\t  9 Perpendicularity\n"
      "\t 10 Position\n"
      "\t 11 ProfileOfLine\n"
      "\t 12 ProfileOfSurface\n"
      "\t 13 Straightness\n"
      "\t 14 Symmetry\n"
      "\t 15 TotalRunout\n",
    __FILE__, setTypeOfTol, g);

  di.Add ("XGetTypeOfTolerance","XGetTypeOfTolerance Doc GTol_Label",
    __FILE__, getTypeOfTol, g);

  di.Add ("XSetTypeOfToleranceValue","XSetTypeOfToleranceValue Doc GTol_Label type"
        "Values:"
    "\n  0 none"
    "\n  1 Diameter"
    "\n  2 SphericalDiameter",
    __FILE__, setTypeOfTolVal, g);

  di.Add ("XGetTypeOfToleranceValue","XGetTypeOfToleranceValue Doc GTol_Label",
    __FILE__, getTypeOfTolVal, g);

  di.Add ("XSetToleranceValue","XSetToleranceValue Doc GTol_Label value",
    __FILE__, setTolVal, g);

  di.Add ("XGetToleranceValue","XGetToleranceValue Doc GTol_Label",
    __FILE__, getTolVal, g);

  di.Add ("XSetTolMaterialReq","XSetTolMaterialReq Doc GTol_Label mod"
        "Values:"
    "\n  0 none"
    "\n  1 M"
    "\n  2 L",
    __FILE__, setMatReq, g);

  di.Add ("XGetTolMaterialReq","XGetTolMaterialReq Doc GTol_Label",
    __FILE__, getMatReq, g);

  di.Add ("XSetTolZoneMod","XSetTolZoneMod Doc GTol_Label mod"
        "Values:"
    "\n  0 none"
    "\n  1 P"
    "\n  2 NonUniform",
    __FILE__, setZoneMod, g);

  di.Add ("XGetTolZoneMod","XGetTolZoneMod Doc GTol_Label",
    __FILE__, getZoneMod, g);

  di.Add ("XSetTolZoneModValue","XSetTolZoneModValue Doc GTol_Label val",
    __FILE__, setZoneModVal, g);

  di.Add ("XGetTolZoneModValue","XGetTolZoneModValue Doc GTol_Label",
    __FILE__, getZoneModVal, g);
  
  di.Add ("XAddTolModifier","XAddTolModifier Doc Tol_Label mod1 mod2 ..."
        "Values:\n"
      "\t  0 Any_Cross_Section\n"
      "\t  1 Common_Zone\n"
      "\t  2 Each_Radial_Element\n"
      "\t  3 Free_State\n"
      "\t  4 Least_Material_Requirement\n"
      "\t  5 Line_Element\n"
      "\t  6 Major_Diameter\n"
      "\t  7 Maximum_Material_Requirement\n"
      "\t  8 Minor_Diameter\n"
      "\t  9 Not_Convex\n"
      "\t 10 Pitch_Diameter\n"
      "\t 11 Reciprocity_Requirement\n"
      "\t 12 Separate_Requirement\n"
      "\t 13 Statistical_Tolerance\n"
      "\t 14 Tangent_Plane\n",
    __FILE__, addTolModif, g);

  di.Add ("XGetTolModifier","XGetTolModifier Doc Tol_Label",
    __FILE__, getTolModif, g);

  di.Add ("XSetTolMaxValue","XSetTolMaxValue Doc Dim_Label val",
    __FILE__, setTolMaxVal, g);

  di.Add ("XGetTolMaxValue","XGetTolMaxValue Doc Dim_Label val",
    __FILE__, getTolMaxVal, g);

  di.Add ("XSetDimensionType","XSetDimensionType Doc Dim_Label type"
        "Values:"
      "\t  0 type is absent\n"
      "\t  1 Location_CurvedDistance\n"
      "\t  2 Location_LinearDistance\n"
      "\t  3 Location_LinearDistance_FromCenterToOuter\n"
      "\t  4 Location_LinearDistance_FromCenterToInner\n"
      "\t  5 Location_LinearDistance_FromOuterToCenter\n"
      "\t  6 Location_LinearDistance_FromOuterToOuter\n"
      "\t  7 Location_LinearDistance_FromOuterToInner\n"
      "\t  8 Location_LinearDistance_FromInnerToCenter\n"
      "\t  9 Location_LinearDistance_FromInnerToOuter\n"
      "\t 10 Location_LinearDistance_FromInnerToInner\n"
      "\t 11 Location_Angular\n"
      "\t 12 Location_Oriented\n"
      "\t 13 Location_WithPath\n"
      "\t 14 Size_CurveLength\n"
      "\t 15 Size_Diameter\n"
      "\t 16 Size_SphericalDiameter\n"
      "\t 17 Size_Radius\n"
      "\t 18 Size_SphericalRadius\n"
      "\t 19 Size_ToroidalMinorDiameter\n"
      "\t 20 Size_ToroidalMajorDiameter\n"
      "\t 21 Size_ToroidalMinorRadius\n"
      "\t 22 Size_ToroidalMajorRadius\n"
      "\t 23 Size_ToroidalHighMajorDiameter\n"
      "\t 24 Size_ToroidalLowMajorDiameter\n"
      "\t 25 Size_ToroidalHighMajorRadius\n"
      "\t 26 Size_ToroidalLowMajorRadius\n"
      "\t 27 Size_Thickness\n"
      "\t 28 Size_Angular\n"
      "\t 29 Size_WithPath\n",
    __FILE__, setDimType, g);

  di.Add ("XGetDimensionType","XGetDimensionType Doc Dim_Label",
    __FILE__, getDimType, g);

  di.Add ("XSetDimensionValue","XSetDimensionValue Doc Dim_Label val",
    __FILE__, setDimVal, g);

  di.Add ("XGetDimensionValue","XGetDimensionValue Doc Dim_Label",
    __FILE__, getDimVal, g);

  di.Add ("XSetDimensionQualifier","XSetDimensionQualifier Doc Dim_Label val"
        "Values:"
    "\n  0 none"
    "\n  1 Min"
    "\n  2 Max"
    "\n  3 Avg",
    __FILE__, setDimQalif, g);
  
  di.Add ("XGetDimensionQualifier","XGetDimensionQualifier Doc Dim_Label",
    __FILE__, getDimQalif, g);

  di.Add ("XSetDimensionRange","XSetDimensionRange Doc Dim_Label low_val up_val",
    __FILE__, setDimRange, g);

  di.Add ("XGetDimensionRange","XGetDimensionRange Doc Dim_Label",
    __FILE__, getDimRange, g);

  di.Add ("XSetDimensionPlusMinusTol","XSetDimensionPlusMinusTol Doc Dim_Label low_val up_val",
    __FILE__, setDimPlusMinusTol, g);

  di.Add ("XGetDimensionPlusMinusTol","XGetDimensionPlusMinusTol Doc Dim_Label",
    __FILE__, getDimPlusMinusTol, g);

  di.Add ("XSetDimensionClassOfTol","XSetDimensionClassOfTol Doc Dim_Label ishole[1/0] formVar grade"
        "Values of formVar:"
        "\t 1 a\n"
        "\t 2 b\n"
        "\t 3 c\n"
        "\t 4 cd\n"
        "\t 5 d\n"
        "\t 6 e\n"
        "\t 7 ef\n"
        "\t 8 f\n"
        "\t 9 fg\n"
        "\t10 g\n"
        "\t11 h\n"
        "\t12 js\n"
        "\t13 j\n"
        "\t14 k\n"
        "\t15 m\n"
        "\t16 n\n"
        "\t17 p\n"
        "\t18 r\n"
        "\t19 s\n"
        "\t20 t\n"
        "\t21 u\n"
        "\t22 v\n"
        "\t23 x\n"
        "\t24 y\n"
        "\t25 z\n"
        "\t26 za\n"
        "\t27 zb\n"
        "\t28 zc\n\n"
        "Values of grade:"
        "\t 0 01\n"
        "\t 1 0\n"
        "\t 2 1\n"
        "\t 3 2d\n"
        "\t 4 3\n"
        "\t 5 4\n"
        "\t 6 5f\n"
        "\t 7 76\n"
        "\t 8 7g\n"
        "\t 9 8\n"
        "\t10 9\n"
        "\t11 10js\n"
        "\t12 11j\n"
        "\t13 12k\n"
        "\t14 13m\n"
        "\t15 14n\n"
        "\t16 15p\n"
        "\t17 16r\n"
        "\t18 17s\n"
        "\t19 18t\n",
    __FILE__, setDimClassTol, g);

  di.Add ("XGetDimensionClassOfTol","XGetDimensionClassOfTol Doc Dim_Label",
    __FILE__, getDimClassTol, g);

  di.Add ("XSetDimensionNbOfDecimalPlaces","XSetDimensionNbOfDecimalPlaces Doc Dim_Label l_val r_val",
    __FILE__, setDimNbOfDecimalPlaces, g);

  di.Add ("XGetDimensionNbOfDecimalPlaces","XGetDimensionNbOfDecimalPlaces Doc Dim_Label",
    __FILE__, getDimNbOfDecimalPlaces, g);

  di.Add ("XAddDimensionModifiers","XAddDimensionModifiers Doc Dim_Label mod1 mod2 ..."
        "Values:"
      "\t 0 ControlledRadius\n"
      "\t 1 Square\n"
      "\t 2 StatisticalTolerance\n"
      "\t 3 ContinuousFeature\n"
      "\t 4 TwoPointSize\n"
      "\t 5 LocalSizeDefinedBySphere\n"
      "\t 6 LeastSquaresAssociationCriterion\n"
      "\t 7 MaximumInscribedAssociation\n"
      "\t 8 MinimumCircumscribedAssociation\n"
      "\t 9 CircumferenceDiameter\n"
      "\t10 AreaDiameter\n"
      "\t11 VolumeDiameter\n"
      "\t12 MaximumSize\n"
      "\t13 MinimumSize\n"
      "\t14 AverageSize\n"
      "\t15 MedianSize\n"
      "\t16 MidRangeSize\n"
      "\t17 RangeOfSizes\n"
      "\t18 AnyRestrictedPortionOfFeature\n"
      "\t19 AnyCrossSection\n"
      "\t20 SpecificFixedCrossSection\n"
      "\t21 CommonTolerance\n"
      "\t22 FreeStateCondition\n"
      "\t23 Between\n",
    __FILE__, addDimModifier, g);

  di.Add ("XGetDimensionModifiers","XGetDimensionModifiers Doc Dim_Label",
    __FILE__, getDimModifier, g);
  
  di.Add ("XSetDimensionPath","XSetDimensionPath Doc Dim_Label path(edge)",
    __FILE__, addDimPath, g);

  di.Add ("XSetDimensionPoints","XSetDimensionPoints Doc Dim_Label v1 [v2]",
    __FILE__, addDimPoints, g);

  di.Add ("XGetDimensionPoints","XGetDimensionPoints Doc Dim_Label",
    __FILE__, getDimPoints, g);

  di.Add ("XSetDimensionDir","XSetDimensionDir Doc Dim_Label x y z",
    __FILE__, addDimDir, g);

  di.Add ("XGetDimensionDir","XGetDimensionDir Doc Dim_Label",
    __FILE__, getDimDir, g);

  di.Add ("XAddDimensionDescr","XAddDimensionDescr Doc Dim_Label Description [DescriptionName]\n"
    "Add named text description to given Dimension, if DescriptionName is missed"
    "name will be an empty string.",
    __FILE__, addDimDescr, g);

  di.Add ("XGetDimensionDescr","XGetDimensionDescr Doc Dim_Label\n"
    "Return all descriptions of given Dimension.",
    __FILE__, getDimDescr, g);

  di.Add ("XSetGDTPosition","XSetGDTPosition Doc GDT_Label loc_x loc_y loc_z normal_x normal_y normal_z xdir_x xdir_y xdir_z"
    "Set plane to display dimension parallel to and point to display text (loc)",
    __FILE__, addGDTPosition, g);

  di.Add ("XGetGDTPosition","XGetGDTPosition Doc GDT_Label"
    "Returns text position and plane, parallel to which dimension is displayed",
    __FILE__, getGDTPosition, g);

  di.Add ("XSetGDTPresentation","XSetGDTPresentation Doc GDT_Label Shape Name"
    "Set presentation with given name for dimension",
    __FILE__, addGDTPresentation, g);

  di.Add ("XGetGDTPresentation","XGetGDTPresentation Doc GDT_Label Shape"
    "Returns Presentation into Shape",
    __FILE__, getGDTPresentation, g);
}
