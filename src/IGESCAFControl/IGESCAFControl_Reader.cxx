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
#include <IGESData_LevelListEntity.hxx>
#include <IGESGraph_Color.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Quantity_Color.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_ShapeMapTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

//=======================================================================
//function : Transfer
//purpose  : basic working method
//=======================================================================
static void checkColorRange (Standard_Real& theCol)
{
  if ( theCol < 0. ) theCol = 0.;
  if ( theCol > 100. ) theCol = 100.;
}

static inline Standard_Boolean IsComposite (const TopoDS_Shape& theShape)
{
  if( theShape.ShapeType() == TopAbs_COMPOUND)
  {
    if(!theShape.Location().IsIdentity())
      return Standard_True;
    TopoDS_Iterator anIt( theShape, Standard_False, Standard_False );
    
    for (; anIt.More() ; anIt.Next()) 
    {
      if( IsComposite (anIt.Value()))
        return Standard_True;
    }

  }
  return Standard_False;
}

//=======================================================================
//function : AddCompositeShape
//purpose  : Recursively adds composite shapes (TopoDS_Compounds) into the XDE document.
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
static void AddCompositeShape (const Handle(XCAFDoc_ShapeTool)& theSTool,
                               const TopoDS_Shape& theShape,
                               Standard_Boolean theConsiderLoc,
                               TopTools_MapOfShape& theMap)
{
  TopoDS_Shape aShape = theShape;
  TopLoc_Location aLoc = theShape.Location();
  if (!theConsiderLoc && !aLoc.IsIdentity())
    aShape.Location( TopLoc_Location() );
  if (!theMap.Add (aShape)) 
    return;

  TopoDS_Iterator anIt( theShape, Standard_False, Standard_False );
  Standard_Boolean aHasCompositeSubShape = Standard_False;
  TopoDS_Compound aSimpleShape;
  BRep_Builder aB;
  aB.MakeCompound( aSimpleShape);
  TopoDS_Compound aCompShape;
  aB.MakeCompound( aCompShape);
  Standard_Integer nbSimple = 0;

  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape& aSubShape = anIt.Value();
    if (IsComposite (aSubShape)) {
      aHasCompositeSubShape = Standard_True;
      AddCompositeShape( theSTool, aSubShape,Standard_False ,theMap );
      aB.Add( aCompShape, aSubShape);
    }
    else
    {
      aB.Add(aSimpleShape, aSubShape);
      nbSimple++;
    }
  }
  //case of hybrid shape
  if( nbSimple && aHasCompositeSubShape)
  {
    theSTool->AddShape( aSimpleShape,  Standard_False, Standard_False  );
    TopoDS_Compound aNewShape;
    aB.MakeCompound(aNewShape);
    aB.Add(aNewShape, aSimpleShape);
    aB.Add(aNewShape,aCompShape);
    //if (!aLoc.IsIdentity())
    //  aNewShape.Location(aLoc );
    aNewShape.Orientation(theShape.Orientation());
    theSTool->AddShape( aNewShape,  aHasCompositeSubShape, Standard_False  );
  }
  else
    theSTool->AddShape( aShape,  aHasCompositeSubShape, Standard_False  );
  return;
}

Standard_Boolean IGESCAFControl_Reader::Transfer (Handle(TDocStd_Document) &doc)
{
  // read all shapes
  Standard_Integer num;// = NbRootsForTransfer();
  //if ( num <=0 ) return Standard_False;
  //for ( Standard_Integer i=1; i <= num; i++ ) {
  //  TransferOneRoot ( i );
  //}
  
  TransferRoots(); // replaces the above
  num = NbShapes();
  if ( num <=0 ) return Standard_False;

  // and insert them to the document
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool( doc->Main() );
  if(STool.IsNull()) return Standard_False;
  Standard_Integer i;
  for(i=1; i<=num; i++) {
    TopoDS_Shape sh = Shape ( i );
    // ---- HERE -- to add check [ assembly / hybrid model ]
    if( !IsComposite (sh))
      STool->AddShape( sh, Standard_False );
    else {
      TopTools_MapOfShape aMap;
      AddCompositeShape( STool, sh,Standard_True, aMap );
      
    }
  }
  
  // added by skl 13.10.2003
  const Handle(Interface_InterfaceModel) &Model = WS()->Model();
  const Handle(XSControl_TransferReader) &TR = WS()->TransferReader();
  const Handle(Transfer_TransientProcess) &TP = TR->TransientProcess();
  Standard_Boolean IsCTool = Standard_True;
  Handle(XCAFDoc_ColorTool) CTool = XCAFDoc_DocumentTool::ColorTool(doc->Main());
  if(CTool.IsNull()) IsCTool = Standard_False;
  Standard_Boolean IsLTool = Standard_True;
  Handle(XCAFDoc_LayerTool) LTool = XCAFDoc_DocumentTool::LayerTool(doc->Main());
  if(LTool.IsNull()) IsLTool = Standard_False;

  Standard_Integer nb = Model->NbEntities();
  for(i=1; i<=nb; i++) {
    Handle(IGESData_IGESEntity) ent = Handle(IGESData_IGESEntity)::DownCast ( Model->Value(i) );
    if ( ent.IsNull() ) continue;
    Handle(Transfer_Binder) binder = TP->Find ( ent );
    if ( binder.IsNull() ) continue;
    TopoDS_Shape S = TransferBRep::ShapeResult (binder);
    if ( S.IsNull() ) continue;

    Standard_Boolean IsColor = Standard_False;
    Quantity_Color col;
    if( GetColorMode() && IsCTool ) {
      // read colors
      if(ent->DefColor()==IGESData_DefValue ||
         ent->DefColor()==IGESData_DefReference) {
        // color is assigned
        // decode color and set to document
        IsColor = Standard_True;
        if ( ent->DefColor() == IGESData_DefValue ) {
          col = IGESCAFControl::DecodeColor ( ent->RankColor() );
        }
        else {
          Handle(IGESGraph_Color) color = Handle(IGESGraph_Color)::DownCast ( ent->Color() );
          if ( color.IsNull() ) {
#ifdef OCCT_DEBUG
            cout << "Error: Unrecognized type of color definition" << endl;
#endif
            IsColor = Standard_False;
          }
          else {
            Standard_Real r, g, b;
            color->RGBIntensity ( r, g, b );
            checkColorRange ( r );
            checkColorRange ( g );
            checkColorRange ( b );
            col.SetValues ( 0.01*r, 0.01*g, 0.01*b, Quantity_TOC_RGB );
          }
        }
      }
    }

    TDF_Label L;

    Standard_Boolean IsFound;
    if(IsColor) {
      CTool->AddColor(col);
      IsFound = STool->SearchUsingMap(S,L,Standard_False,Standard_True);
    }
    else {
      IsFound = STool->SearchUsingMap(S,L,Standard_False,Standard_False);
    }
    if(!IsFound) {
      if(IsColor) {
        for (TopoDS_Iterator it(S); it.More(); it.Next()) {
          if(STool->SearchUsingMap(it.Value(),L,Standard_False,Standard_True)) {
            CTool->SetColor(L,col,XCAFDoc_ColorGen);
            if( GetLayerMode() && IsLTool ) {
              // read layers
              // set a layers to the document
              IGESData_DefList aDeflist = ent->DefLevel();
              switch (aDeflist) {
              case IGESData_DefOne : {
                TCollection_ExtendedString aLayerName ( ent->Level() );
                LTool->SetLayer( L, aLayerName );
                break;
              }
              case IGESData_DefSeveral : {
                Handle(IGESData_LevelListEntity) aLevelList = ent->LevelList();
                Standard_Integer layerNb = aLevelList->NbLevelNumbers();
                for ( Standard_Integer ilev = 1; ilev <= layerNb; ilev++ ) {
                  TCollection_ExtendedString aLayerName ( aLevelList->LevelNumber(ilev) );
                  LTool->SetLayer( L, aLayerName );
                }
                break;
              }
                default : break;
              }
            }
          }
        }
      }
    }
    else {
      if(IsColor) {
        CTool->SetColor(L,col,XCAFDoc_ColorGen);
      }
      if(GetNameMode()) {
        // read names
        if(ent->HasName()) {
          TCollection_AsciiString string = ent->NameValue()->String();
          string.LeftAdjust();
          string.RightAdjust();
          TCollection_ExtendedString str(string);
          TDataStd_Name::Set(L,str);
        }
      }
      if( GetLayerMode() && IsLTool ) {
        // read layers
        // set a layers to the document
        IGESData_DefList aDeflist = ent->DefLevel();
        switch (aDeflist) {
        case IGESData_DefOne : {
          TCollection_ExtendedString aLayerName ( ent->Level() );
          LTool->SetLayer( L, aLayerName );
          break;
        }
        case IGESData_DefSeveral : {
          Handle(IGESData_LevelListEntity) aLevelList = ent->LevelList();
          Standard_Integer layerNb = aLevelList->NbLevelNumbers();
          for ( Standard_Integer ilev = 1; ilev <= layerNb; ilev++ ) {
            TCollection_ExtendedString aLayerName ( aLevelList->LevelNumber(ilev) );
            LTool->SetLayer( L, aLayerName );
          }
          break;
        }
          default : break;
        }
      }
    }

    //Checks that current entity is a subfigure
    Handle(IGESBasic_SubfigureDef) aSubfigure = Handle(IGESBasic_SubfigureDef)::DownCast (ent);
    if (GetNameMode() && !aSubfigure.IsNull() && STool->Search (S, L, Standard_True, Standard_True))
    {
      //In this case we attach subfigure name to the label, instead of default "COMPOUND"
      Handle(TCollection_HAsciiString) aName = aSubfigure->Name();
      aName->LeftAdjust();
      aName->RightAdjust();
      TCollection_ExtendedString anExtStrName (aName->ToCString());
      TDataStd_Name::Set (L, anExtStrName);
    }

  }

  CTool->ReverseChainsOfTreeNodes();

  // end added by skl 13.10.2003

  // Update assembly compounds
  STool->UpdateAssemblies();

  return Standard_True;
}
  

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Reader::Perform (const Standard_CString filename,
						 Handle(TDocStd_Document) &doc)
{
  if ( ReadFile ( filename ) != IFSelect_RetDone ) return Standard_False;
  return Transfer ( doc );
}
