// Created on: 2000-08-16
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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


#include <TDF_Label.hxx>
#include <IGESCAFControl_Reader.ixx>
#include <TopoDS_Shape.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XSControl_TransferReader.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Transfer_Binder.hxx>
#include <TransferBRep.hxx>
#include <Quantity_Color.hxx>
#include <IGESCAFControl.hxx>
#include <IGESGraph_Color.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TopoDS_Iterator.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_Name.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <TCollection_HAsciiString.hxx>
#include <XCAFDoc_ShapeMapTool.hxx>
#include <IGESBasic_SubfigureDef.hxx>

//=======================================================================
//function : IGESCAFControl_Reader
//purpose  : 
//=======================================================================

IGESCAFControl_Reader::IGESCAFControl_Reader () :
       myColorMode( Standard_True ),
       myNameMode ( Standard_True ),
       myLayerMode( Standard_True )
{
}


//=======================================================================
//function : IGESCAFControl_Reader
//purpose  : 
//=======================================================================

IGESCAFControl_Reader::IGESCAFControl_Reader (const Handle(XSControl_WorkSession)& WS,
					      const Standard_Boolean scratch)
{
  SetWS (WS,scratch);
  myColorMode = Standard_True;
  myNameMode = Standard_True;
  myLayerMode = Standard_True;
}


//=======================================================================
//function : Transfer
//purpose  : basic working method
//=======================================================================
static void checkColorRange (Standard_Real& theCol)
{
  if ( theCol < 0. ) theCol = 0.;
  if ( theCol > 100. ) theCol = 100.;
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
    STool->AddShape ( sh, Standard_False );
  }
  
  // added by skl 13.10.2003
  Handle(Interface_InterfaceModel) Model = WS()->Model();
  //WS()->TransferReader()->SetTransientProcess(TransientProcess());
  Handle(XSControl_TransferReader) TR = WS()->TransferReader();
  Handle(Transfer_TransientProcess) TP = TR->TransientProcess();
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
#ifdef DEB
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

  // read colors
//  if ( GetColorMode() )
//    ReadColors ( doc );
  
  // read names
//  if ( GetNameMode() )
//    ReadNames ( doc );
  
  // read layers
//  if ( GetLayerMode() )
//    ReadLayers ( doc );
    

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
  

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Reader::Perform (const TCollection_AsciiString &filename,
						 Handle(TDocStd_Document) &doc)
{
  if ( ReadFile ( filename.ToCString() ) != IFSelect_RetDone ) return Standard_False;
  return Transfer ( doc );
}


//=======================================================================
//function : ReadColors
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Reader::ReadColors (Handle(TDocStd_Document)& Doc) const
{

  Handle(Interface_InterfaceModel) Model = WS()->Model();

  //WS()->TransferReader()->SetTransientProcess(TransientProcess()); // !!!!!!!!!
  
  Handle(XSControl_TransferReader) TR = WS()->TransferReader();
  Handle(Transfer_TransientProcess) TP = /*TransientProcess();*/TR->TransientProcess();
  Handle(XCAFDoc_ColorTool) CTool = XCAFDoc_DocumentTool::ColorTool( Doc->Main() );
/*
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool( Doc->Main() );
  if ( STool.IsNull() ) return Standard_False;
  Handle(XCAFDoc_ShapeMapTool) SMTool = XCAFDoc_ShapeMapTool::Set(STool->Label());
  SMTool->ComputeMaps();

  Standard_Integer nb = Model->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(IGESData_IGESEntity) ent = Handle(IGESData_IGESEntity)::DownCast ( Model->Value(i) );
    if ( ent.IsNull() ) continue;
    if ( ent->DefColor() != IGESData_DefValue && 
	 ent->DefColor() != IGESData_DefReference ) continue; // no color assigned

    // find tartet shape
    Handle(Transfer_Binder) binder = TP->Find ( ent );
    if ( binder.IsNull() ) continue;
    TopoDS_Shape S = TransferBRep::ShapeResult (binder);
    if ( S.IsNull() ) continue;

    // decode color and set to document
    Quantity_Color col;
    if ( ent->DefColor() == IGESData_DefValue ) {
      col = IGESCAFControl::DecodeColor ( ent->RankColor() );
    }
    else {
      Handle(IGESGraph_Color) color = Handle(IGESGraph_Color)::DownCast ( ent->Color() );
      if ( color.IsNull() ) {
	cout << "Error: Unrecognized type of color definition" << endl;
	continue;
      }
      
      Standard_Real r, g, b;
      color->RGBIntensity ( r, g, b );
      col.SetValues ( 0.01*r, 0.01*g, 0.01*b, Quantity_TOC_RGB );
    }
    
    TDF_Label L;
    cout<<"i="<<i<<endl;
    if(SMTool->Search(S,L)) {
      cout<<"      find Instance"<<endl;
      CTool->SetColor(L, col, XCAFDoc_ColorGen);
    }
    if(L.IsNull()) {
      cout<<"L1 is Null"<<endl;
      if(STool->Search(S,L,Standard_False,Standard_False,Standard_True)) {
        cout<<"     add new label1 :"<<L<<endl;
	CTool->SetColor(L, col, XCAFDoc_ColorGen);
      }
    }
    if(L.IsNull()) {
    //else {
      cout<<"      try to find splitting"<<endl;
      // may be S is compound of shapes resulting from splitting
      for (TopoDS_Iterator it(S); it.More(); it.Next()) {
        //TDF_Label L1;
        if(!SMTool->Search(it.Value(),L)) continue; //break-?
        cout<<"      find splitting"<<endl;
	CTool->SetColor(L, col, XCAFDoc_ColorGen);
      }
      if(L.IsNull()) {
        for (TopoDS_Iterator it(S); it.More(); it.Next()) {
          if(STool->Search(S,L,Standard_False,Standard_False,Standard_True)) {
            cout<<"     add new label2 :"<<L<<endl;
            CTool->SetColor(L, col, XCAFDoc_ColorGen);
          }
        }
      }
    }
//    cout<<"L.Dump():"<<L<<endl;
//    if(L.IsNull()) {
//      cout<<"L2 is Null"<<endl;
//      if(STool->Search(S,L,Standard_False,Standard_False,Standard_True)) {
//        cout<<"     add new label2 :"<<L<<endl;
//	CTool->SetColor(L, col, XCAFDoc_ColorGen);
//      }
//    }
  }
*/  
  return Standard_True;
}

//=======================================================================
//function : ReadNames
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Reader::ReadNames (Handle(TDocStd_Document)& Doc) const
{
  Handle(Interface_InterfaceModel) Model = WS()->Model();

  //WS()->TransferReader()->SetTransientProcess(TransientProcess()); // !!!!!!!!!
  
  Handle(XSControl_TransferReader) TR = WS()->TransferReader();
  Handle(Transfer_TransientProcess) TP = /*TransientProcess();*/TR->TransientProcess();
/*
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool( Doc->Main() );
  if ( STool.IsNull() ) return Standard_False;
  Handle(XCAFDoc_ShapeMapTool) SMTool = XCAFDoc_ShapeMapTool::Set(STool->Label());
  SMTool->ComputeMaps();

  Standard_Integer nb = Model->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(IGESData_IGESEntity) ent = Handle(IGESData_IGESEntity)::DownCast ( Model->Value(i) );
    if ( ent.IsNull() || ! ent->HasName() ) continue; //not only Entity Label (f.18) but Name Property also

    // find target shape
    Handle(Transfer_Binder) binder = TP->Find ( ent );
    if ( binder.IsNull() ) continue;
    TopoDS_Shape S = TransferBRep::ShapeResult (binder);
    if ( S.IsNull() ) continue;

    TDF_Label L;
    //if ( ! STool->Search ( S, L, Standard_True, Standard_True, Standard_False ) ) continue;
    if ( ! SMTool->Search ( S, L, Standard_True, Standard_True) ) continue;
    
    // set a name to the document
    TCollection_AsciiString string = ent->NameValue()->String();
    string.LeftAdjust();
    string.RightAdjust();
    TCollection_ExtendedString str ( string );
    TDataStd_Name::Set ( L, str );
  }
*/
  return Standard_True;
}

//=======================================================================
//function : ReadLayers
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Reader::ReadLayers (Handle(TDocStd_Document)& Doc) const
{
  Handle(Interface_InterfaceModel) Model = WS()->Model();

  //WS()->TransferReader()->SetTransientProcess(TransientProcess()); // !!!!!!!!!
  
  Handle(XSControl_TransferReader) TR = WS()->TransferReader();
  Handle(Transfer_TransientProcess) TP = /*TransientProcess();*/TR->TransientProcess();
/*
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool( Doc->Main() );
  if ( STool.IsNull() ) return Standard_False;
  Handle(XCAFDoc_ShapeMapTool) SMTool = XCAFDoc_ShapeMapTool::Set(STool->Label());
  SMTool->ComputeMaps();

  Handle(XCAFDoc_LayerTool) LTool = XCAFDoc_DocumentTool::LayerTool( Doc->Main() );
  if ( LTool.IsNull() ) return Standard_False;

  Standard_Integer nb = Model->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(IGESData_IGESEntity) ent = Handle(IGESData_IGESEntity)::DownCast ( Model->Value(i) );

    if ( ent.IsNull() ) continue;

    // find target shape
    Handle(Transfer_Binder) binder = TP->Find ( ent );
    if ( binder.IsNull() ) continue;
    TopoDS_Shape S = TransferBRep::ShapeResult (binder);
    if ( S.IsNull() ) continue;

    TDF_Label L;
    //if ( ! STool->Search ( S, L, Standard_True, Standard_True, Standard_False ) ) continue;
    if( !SMTool->Search(S, L, Standard_True, Standard_True) ) continue;
    
    // set a layers to the document
    IGESData_DefList aDeflist = ent->DefLevel();
    
    switch (aDeflist) {
    case IGESData_DefOne : {
      TCollection_ExtendedString aLayerName ( ent->Level() );
      LTool->SetLayer( L, aLayerName );
#ifdef DEB
// 	cout << "Added layer " << aLayerName << endl;
#endif
      break;
    }
    case IGESData_DefSeveral : {
      Handle(IGESData_LevelListEntity) aLevelList = ent->LevelList();
      Standard_Integer layerNb = aLevelList->NbLevelNumbers();
      for ( Standard_Integer ilev = 1; ilev <= layerNb; ilev++ ) {
	TCollection_ExtendedString aLayerName ( aLevelList->LevelNumber(ilev) );
	LTool->SetLayer( L, aLayerName );
#ifdef DEB
// 	cout << "Added layer " << aLayerName << endl;
#endif
      }
      break;
    }
      default : break;
    }
	  
  }
*/
  return Standard_True;
}


//=======================================================================
//function : SetColorMode
//purpose  : 
//=======================================================================

void IGESCAFControl_Reader::SetColorMode (const Standard_Boolean colormode)
{
  myColorMode = colormode;
}

//=======================================================================
//function : GetColorMode
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Reader::GetColorMode () const
{
  return myColorMode;
}

//=======================================================================
//function : SetNameMode
//purpose  : 
//=======================================================================

void IGESCAFControl_Reader::SetNameMode (const Standard_Boolean namemode)
{
  myNameMode = namemode;
}

//=======================================================================
//function : GetNameMode
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Reader::GetNameMode () const
{
  return myNameMode;
}

//=======================================================================
//function : SetLayerMode
//purpose  : 
//=======================================================================

void IGESCAFControl_Reader::SetLayerMode (const Standard_Boolean layermode)
{
  myLayerMode = layermode;
}

//=======================================================================
//function : GetLayerMode
//purpose  : 
//=======================================================================

Standard_Boolean IGESCAFControl_Reader::GetLayerMode () const
{
  return myLayerMode;
}

