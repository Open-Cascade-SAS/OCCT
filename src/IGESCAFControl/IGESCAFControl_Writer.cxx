// Created on: 2000-08-17
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

#include <IGESCAFControl_Writer.ixx>
#include <XCAFDoc_ShapeTool.hxx>
#include <TDF_LabelSequence.hxx>
#include <TopoDS_Shape.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFPrs.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <TransferBRep.hxx>
#include <Transfer_FinderProcess.hxx>
#include <IGESGraph_Color.hxx>
#include <IGESCAFControl.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopoDS_Iterator.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDF_Label.hxx>
#include <Transfer_TransientListBinder.hxx>
#include <Standard_Transient.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <IGESGraph_DefinitionLevel.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Iterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDataStd_Name.hxx>
#include <IGESData_NameEntity.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TColStd_HSequenceOfExtendedString.hxx>

//=======================================================================
//function : IGESCAFControl_Writer
//purpose  : 
//=======================================================================

IGESCAFControl_Writer::IGESCAFControl_Writer () :
       myColorMode( Standard_True ),
       myNameMode ( Standard_True ),
       myLayerMode( Standard_True )
{
}

//=======================================================================
//function : IGESCAFControl_Writer
//purpose  : 
//=======================================================================

IGESCAFControl_Writer::IGESCAFControl_Writer (const Handle(XSControl_WorkSession)& WS,
					      const Standard_Boolean /*scratch*/ )
{
  // this code does things in a wrong way, it should be vice-versa
  WS->SetModel ( Model() );
  WS->SetMapWriter ( TransferProcess() );
  myColorMode = Standard_True;
  myNameMode = Standard_True;
  myLayerMode = Standard_True;
  
//  SetWS (WS,scratch); // this should be the only required command here
}

//=======================================================================
//function : Transfer
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Writer::Transfer (const Handle(TDocStd_Document) &doc)
{  
  // translate free top-level shapes of the DECAF document
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool( doc->Main() );
  if ( STool.IsNull() ) return Standard_False;

  TDF_LabelSequence labels;
  STool->GetFreeShapes ( labels );
  return Transfer (labels);
}  
  
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Writer::Perform (const Handle(TDocStd_Document) &doc,
						 const Standard_CString filename)
{
  if ( ! Transfer ( doc ) ) return Standard_False;
  return Write ( filename ) == IFSelect_RetDone;
}
  
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Writer::Perform (const Handle(TDocStd_Document) &doc,
						 const TCollection_AsciiString &filename)
{
  if ( ! Transfer ( doc ) ) return Standard_False;
  return Write ( filename.ToCString() ) == IFSelect_RetDone;
}
  
//=======================================================================
//function : Transfer
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Writer::Transfer (const TDF_LabelSequence& labels)
{  
  if ( labels.Length() <=0 ) return Standard_False;
  for ( Standard_Integer i=1; i <= labels.Length(); i++ ) {
    TopoDS_Shape shape = XCAFDoc_ShapeTool::GetShape ( labels.Value(i) );
    if ( ! shape.IsNull() ) 
      AddShape ( shape );
//      IGESControl_Writer::Transfer ( shape );
  }
  
  // write colors
  if ( GetColorMode() )
    WriteAttributes ( labels );

  // write layers
  if ( GetLayerMode() )
    WriteLayers ( labels );
  
  // write names
  if ( GetNameMode() )
    WriteNames( labels );
  
  // refresh graph
//  WS()->ComputeGraph ( Standard_True );
  ComputeModel();
  
  return Standard_True;
}

//=======================================================================
//function : WriteAttributes
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Writer::WriteAttributes (const TDF_LabelSequence& labels) 
{
  // Iterate on labels
  if ( labels.Length() <=0 ) return Standard_False;
  for ( Standard_Integer i=1; i <= labels.Length(); i++ ) {
    TDF_Label L = labels.Value(i);

    // collect color settings
    XCAFPrs_DataMapOfShapeStyle settings;
    TopLoc_Location loc;
    XCAFPrs::CollectStyleSettings ( L, loc, settings );
    if ( settings.Extent() <=0 ) continue;
    
    // get a target shape and try to find corresponding context
    // (all the colors set under that label will be put into that context)
    TopoDS_Shape S;
    if ( ! XCAFDoc_ShapeTool::GetShape ( L, S ) ) continue;
        
    // iterate on subshapes and create IGES styles 
    XCAFPrs_DataMapOfStyleTransient colors;
    TopTools_MapOfShape Map;
    const XCAFPrs_Style inherit;
    MakeColors ( S, settings, colors, Map, inherit );
  }
  
  return Standard_True;
}

//=======================================================================
//function : MakeColors
//purpose  : 
//=======================================================================

void IGESCAFControl_Writer::MakeColors (const TopoDS_Shape &S, 
					const XCAFPrs_DataMapOfShapeStyle &settings,
					XCAFPrs_DataMapOfStyleTransient &colors,
					TopTools_MapOfShape &Map,
					const XCAFPrs_Style &inherit) 
{
  // skip already processed shapes
  if ( ! Map.Add ( S ) ) return;
  
  // check if shape has its own style (or inherits from ancestor)
  XCAFPrs_Style style = inherit;
  if ( settings.IsBound(S) ) {
    XCAFPrs_Style own = settings.Find(S);
    if ( own.IsSetColorCurv() ) style.SetColorCurv ( own.GetColorCurv() );
    if ( own.IsSetColorSurf() ) style.SetColorSurf ( own.GetColorSurf() );
  }
  
  // analyze whether current entity should get a color 
  Standard_Boolean hasColor = Standard_False;
  Quantity_Color col;
  if ( S.ShapeType() == TopAbs_FACE ) {
    if ( style.IsSetColorSurf() ) {
      hasColor = Standard_True;
      col = style.GetColorSurf();
    }
  }
  else if ( S.ShapeType() == TopAbs_EDGE || S.ShapeType() == TopAbs_WIRE ) {
    if ( style.IsSetColorCurv() ) {
      hasColor = Standard_True;
      col = style.GetColorCurv();
    }
  }
  
  // if color has to be assigned, try to do this
  if ( hasColor ) {
    Handle(IGESGraph_Color) colent;
    Standard_Integer rank = IGESCAFControl::EncodeColor ( col );
    if ( ! rank ) {
      XCAFPrs_Style c; // style used as key in the map
      c.SetColorSurf ( col );
      if ( colors.IsBound ( c ) ) {
	colent = Handle(IGESGraph_Color)::DownCast ( colors.Find(c) );
      }
      else {
	Handle(TCollection_HAsciiString) str = 
	  new TCollection_HAsciiString ( col.StringName ( col.Name() ) );
	colent = new IGESGraph_Color;
	colent->Init ( col.Red() * 100., col.Green() * 100., col.Blue() * 100., str );
	AddEntity ( colent );
	colors.Bind ( c, colent );
      }
    }
    Handle(Transfer_FinderProcess) FP = TransferProcess();
    Handle(IGESData_IGESEntity) ent;
    Handle(TransferBRep_ShapeMapper) mapper = TransferBRep::ShapeMapper ( FP, S );
    if ( FP->FindTypedTransient ( mapper, STANDARD_TYPE(IGESData_IGESEntity), ent ) ) {
      ent->InitColor ( colent, rank );
    }
    else {
      // may be S was splited during shape process
      Handle(Transfer_Binder) bnd = FP->Find ( mapper );
      if ( ! bnd.IsNull() ) {
	Handle(Transfer_TransientListBinder) TransientListBinder =
	  //Handle(Transfer_TransientListBinder)::DownCast( bnd->Next(Standard_True) );
          Handle(Transfer_TransientListBinder)::DownCast( bnd );
	Standard_Integer i=0, nb=0;
	if (! TransientListBinder.IsNull() ) {
	  nb = TransientListBinder->NbTransients();
	  for (i=1; i<=nb; i++) {
	    Handle(Standard_Transient) t = TransientListBinder->Transient(i);
	    ent = Handle(IGESData_IGESEntity)::DownCast(t);
	    if (!ent.IsNull()) ent->InitColor ( colent, rank );
	  }
	}
	/* // alternative: consider recursive mapping S -> compound -> entities
	else {
	  TopoDS_Shape comp = TransferBRep::ShapeResult(bnd);
	  if ( ! comp.IsNull() && comp.ShapeType() < S.ShapeType() ) 
	    for ( TopoDS_Iterator it(comp); it.More(); it.Next() ) {
	      MakeColors ( it.Value(), settings, colors, Map, style );
	    }
	}
	*/
      }
    }
  }

  // iterate on subshapes (except vertices :)
  if ( S.ShapeType() == TopAbs_EDGE ) return;
  for ( TopoDS_Iterator it(S); it.More(); it.Next() ) {
    MakeColors ( it.Value(), settings, colors, Map, style );
  }
}


static void AttachLayer (const Handle(Transfer_FinderProcess) &FP,
			 const Handle(XCAFDoc_LayerTool)& LTool,
			 const TopoDS_Shape& aSh,
			 const Standard_Integer localIntName)
{

  TopTools_SequenceOfShape shseq;
  if ( aSh.ShapeType() == TopAbs_COMPOUND ) {
    TopoDS_Iterator aShIt(aSh);
    for ( ; aShIt.More(); aShIt.Next() ) {
      TopoDS_Shape newSh = aShIt.Value();
      Handle(TColStd_HSequenceOfExtendedString) shLayers = new TColStd_HSequenceOfExtendedString;
      if (! LTool->GetLayers( newSh, shLayers) || newSh.ShapeType() == TopAbs_COMPOUND )
	AttachLayer(FP, LTool, newSh, localIntName);
    }
    return;
  } else if ( aSh.ShapeType() == TopAbs_SOLID || aSh.ShapeType() == TopAbs_SHELL ) {
    for (TopExp_Explorer exp(aSh,TopAbs_FACE) ; exp.More(); exp.Next()) {
      TopoDS_Face entSh = TopoDS::Face (exp.Current());
      shseq.Append(entSh);
    }
  } else {
    shseq.Append(aSh);
  }
  
  for (Standard_Integer i = 1; i <= shseq.Length(); i++ ) {
    TopoDS_Shape localShape = shseq.Value(i);
    Handle(IGESData_IGESEntity) Igesent;
    Handle(TransferBRep_ShapeMapper) mapper = TransferBRep::ShapeMapper ( FP, localShape );
    if ( FP->FindTypedTransient ( mapper, STANDARD_TYPE(IGESData_IGESEntity), Igesent ) ) {
      Igesent->InitLevel( 0, localIntName );
// #ifdef DEB
//       cout << "Init layer " << localIntName << " for "<< localShape.TShape()->DynamicType()->Name() << endl;
// #endif
    }
#ifdef DEB
    else cout << "Warning: Can't find entity for shape in mapper" << endl;
#endif
  }
}


static void MakeLayers (const Handle(Transfer_FinderProcess) &FP, 
			const Handle(XCAFDoc_ShapeTool)& STool,
			const Handle(XCAFDoc_LayerTool)& LTool,
			const TDF_LabelSequence& aShapeLabels,
			const Standard_Integer localIntName) 
{
  for ( Standard_Integer j = 1; j <= aShapeLabels.Length(); j++ ) {
    TDF_Label aShapeLabel = aShapeLabels.Value(j);
    TopoDS_Shape aSh;
    if ( ! STool->GetShape ( aShapeLabel, aSh ) ) continue;
    AttachLayer (FP, LTool, aSh, localIntName);
  }
}



//=======================================================================
//function : WriteLayers
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Writer::WriteLayers (const TDF_LabelSequence& labels) 
{
  if ( labels.Length() <=0 ) return Standard_False;
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool( labels(1) );
  if ( STool.IsNull() ) return Standard_False;
  Handle(XCAFDoc_LayerTool) LTool = XCAFDoc_DocumentTool::LayerTool( labels(1) );
  if ( LTool.IsNull() ) return Standard_False;
  
  Standard_Integer globalIntName = 0;
  TDF_LabelSequence aLayerLabels;
  LTool->GetLayerLabels( aLayerLabels );
  
  Handle(Transfer_FinderProcess) FP = TransferProcess();
  for ( Standard_Integer i = 1; i <= aLayerLabels.Length(); i++ ){
    TDF_Label aOneLayerL = aLayerLabels.Value(i);
    if ( aOneLayerL.IsNull() ) continue;
    TCollection_ExtendedString localName;
    LTool->GetLayer(aOneLayerL, localName);
    Standard_Integer localIntName = 0;
    TCollection_AsciiString asciiName(localName,'?');
    if (asciiName.IsIntegerValue() ) {
      localIntName = asciiName.IntegerValue();
      if (globalIntName < localIntName) globalIntName = localIntName;
      
      TDF_LabelSequence aShapeLabels;
      LTool->GetShapesOfLayer( aOneLayerL, aShapeLabels );
      if ( aShapeLabels.Length() <= 0 ) continue;
      MakeLayers(FP, STool, LTool, aShapeLabels, localIntName);
    }
  }
  
  for ( Standard_Integer i1 = 1; i1 <= aLayerLabels.Length(); i1++ ) {
    TDF_Label aOneLayerL = aLayerLabels.Value(i1);
    if ( aOneLayerL.IsNull() ) continue;
    TCollection_ExtendedString localName; 
    LTool->GetLayer(aOneLayerL, localName);
    Standard_Integer localIntName = 0;
    TCollection_AsciiString asciiName(localName,'?');
    if (asciiName.IsIntegerValue() ) continue;
    TDF_LabelSequence aShapeLabels;
    LTool->GetShapesOfLayer( aOneLayerL, aShapeLabels );
    if ( aShapeLabels.Length() <= 0 ) continue;
    localIntName = ++globalIntName;
    MakeLayers (FP, STool, LTool, aShapeLabels, localIntName);
  }
  
  return Standard_True;
}


//=======================================================================
//function : WriteNames
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Writer::WriteNames (const TDF_LabelSequence& labels) 
{
  if ( labels.Length() <=0 ) return Standard_False;
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool( labels(1) );
  if ( STool.IsNull() ) return Standard_False;
  TDF_ChildIterator labelShIt(STool->BaseLabel() , Standard_True);
  for (; labelShIt.More(); labelShIt.Next() ) {
    TDF_Label shLabel = labelShIt.Value();
    Handle(TDataStd_Name) aName;
    if ( !shLabel.FindAttribute( TDataStd_Name::GetID(), aName) ) continue;
    TCollection_ExtendedString shName = aName->Get();
    TopoDS_Shape Sh;
    if ( !STool->GetShape(shLabel, Sh) ) continue;
    Handle(Transfer_FinderProcess) FP = TransferProcess();
    Handle(IGESData_IGESEntity) Igesent;
    Handle(TransferBRep_ShapeMapper) mapper = TransferBRep::ShapeMapper ( FP, Sh );
    if ( FP->FindTypedTransient ( mapper, STANDARD_TYPE(IGESData_IGESEntity), Igesent ) ) {
      Handle(TCollection_HAsciiString) HAname = new TCollection_HAsciiString ( "        " );
      Standard_Integer len = 8 - shName.Length();
      if ( len <0 ) len = 0;
      for ( Standard_Integer k=1; len <8; k++, len++ ) {
	HAname->SetValue ( len+1, IsAnAscii(shName.Value(k)) ? (Standard_Character)shName.Value(k) : '?' );
      }
      Igesent->SetLabel( HAname );
//       Handle(IGESData_NameEntity) aNameent = new IGESData_NameEntity;
//       Igesent->AddProperty(aNameent);
   }
  }
  return Standard_True;
}


//=======================================================================
//function : SetColorMode
//purpose  : 
//=======================================================================

void IGESCAFControl_Writer::SetColorMode (const Standard_Boolean colormode)
{
  myColorMode = colormode;
}

//=======================================================================
//function : GetColorMode
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Writer::GetColorMode () const
{
  return myColorMode;
}

//=======================================================================
//function : SetNameMode
//purpose  : 
//=======================================================================

void IGESCAFControl_Writer::SetNameMode (const Standard_Boolean namemode)
{
  myNameMode = namemode;
}

//=======================================================================
//function : GetNameMode
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Writer::GetNameMode () const
{
  return myNameMode;
}

//=======================================================================
//function : SetLayerMode
//purpose  : 
//=======================================================================

void IGESCAFControl_Writer::SetLayerMode (const Standard_Boolean layermode)
{
  myLayerMode = layermode;
}

//=======================================================================
//function : GetLayerMode
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Writer::GetLayerMode () const
{
  return myLayerMode;
}
