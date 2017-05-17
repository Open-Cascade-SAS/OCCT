// Created on: 1998-10-07
// Created by: Denis PASCAL
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <DPrsStd.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_ColorKind.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_Label.hxx>
#include <TDF_Data.hxx> 
#include <DDF.hxx>
#include <DDocStd.hxx>
#include <ViewerTest.hxx>


#include <V3d_View.hxx>


// for AIS

#include <TPrsStd_AISPresentation.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <ViewerTest.hxx>
#include <AIS_InteractiveContext.hxx> 
#include <AIS_InteractiveObject.hxx>
#include <ViewerTest.hxx>
#include <V3d_View.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Graphic3d_NameOfMaterial.hxx>

#include <TDataXtd_Axis.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDataXtd_Point.hxx>
#include <Standard_PCharacter.hxx>

//#include <TSketchStd_Geometry.hxx>
//#include <TSketchStd_Edge.hxx>

//=======================================================================
//function : DPrsStd_AISDisplay
//purpose  : DDisplay (DOC,entry, not_update)
//=======================================================================

static Standard_Integer DPrsStd_AISDisplay (Draw_Interpretor&,
					  Standard_Integer nb, 
					  const char** arg) 
{
  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(arg[1],D)) return 1; 
  TDF_Label L;
  if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;
  Handle(TPrsStd_AISPresentation) prs;
  if(!L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) )
    return 1;
  prs->Display(nb == 3);
  TPrsStd_AISViewer::Update(L);
  return 0;
}

//=======================================================================
//function : DPrsStd_AISRemove
//purpose  : AISRemove (DOC,entry)
//=======================================================================

static Standard_Integer DPrsStd_AISRemove (Draw_Interpretor& di,
					   Standard_Integer nb, 
					   const char** arg) 
{   
   if (nb == 3) {     
     Handle(TDocStd_Document) D;
     if (!DDocStd::GetDocument(arg[1],D)) return 1; 
     TDF_Label L;
     if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;
     Handle(TPrsStd_AISPresentation) P;
     if(!L.FindAttribute(TPrsStd_AISPresentation::GetID(), P)) return 1;
     P->Erase(Standard_True);
     TPrsStd_AISViewer::Update(L);
     return 0;
   }
   di << "DPrsStd_AISRedisplay : Error\n";
   return 1;
}


//=======================================================================
//function : DPrsStd_AISErase
//purpose  : AISErase (DOC,entry)
//=======================================================================

static Standard_Integer DPrsStd_AISErase (Draw_Interpretor& di,
					Standard_Integer nb, 
					const char** arg) 
{   
  if (nb == 3) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1; 
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;
    Handle(TPrsStd_AISPresentation) prs;
    if(!L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) return 1;  
    prs->Erase();
    TPrsStd_AISViewer::Update(L);
    return 0;
  }
  di << "DPrsStd_AISErase : Error\n";
  return 1;
}

//=======================================================================
//function : DPrsStd_AISUpdate
//purpose  : AISUpdate (DOC,entry)
//=======================================================================

static Standard_Integer DPrsStd_AISUpdate (Draw_Interpretor& di,
					 Standard_Integer nb, 
					 const char** arg) 
{   
  if (nb == 3) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1; 
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;
    Handle(TPrsStd_AISPresentation) prs;
    if(!L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) return 1;  
    prs->Update();
    TPrsStd_AISViewer::Update(L);
    return 0;
  }
  di << "DPrsStd_AISUpdate : Error\n";
  return 1;
}


//=======================================================================
//function : DPrsStd_AISSet
//purpose  : AISSet (DOC,entry, id)
//=======================================================================

static Standard_Integer DPrsStd_AISSet (Draw_Interpretor& di,
					Standard_Integer nb, 
					const char** arg) 
{   
  if (nb == 4) {        
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1; 
    Standard_GUID guid;
    TCollection_ExtendedString str = arg[3];
#ifdef OCCT_DEBUG
    cout << "Inputed parameter > " << str   << endl;
#endif
    if ( str == "A" )  //axis
      guid = TDataXtd_Axis::GetID();     //"2a96b601-ec8b-11d0-bee7-080009dc3333" 
    else if( str ==  "C")        //constraint 
      guid = TDataXtd_Constraint::GetID();    //"2a96b602-ec8b-11d0-bee7-080009dc3333" 
    else if( str == "NS" )        //namedshape
      guid = TNaming_NamedShape::GetID();     //"c4ef4200-568f-11d1-8940-080009dc3333" 
    else if( str == "G" )        //geometry
      guid = TDataXtd_Geometry::GetID();      //"2a96b604-ec8b-11d0-bee7-080009dc3333"
    else if( str == "PL" )        //plane
      guid = TDataXtd_Plane::GetID();    //"2a96b60c-ec8b-11d0-bee7-080009dc3333"
    else if( str == "PT" )        //point
      guid = TDataXtd_Point::GetID();    //"2a96b60d-ec8b-11d0-bee7-080009dc3333"
//    else if( str == "SG" )        //TSketch_Geometry
//      guid = TSketchStd_Geometry::GetID();    //"b3aac909-5b78-11d1-8940-080009dc3333"
//    else if( str == "E" )        //TSketch_Edge
//      guid = TSketchStd_Edge::GetID();           //"b3aac90a-5b78-11d1-8940-080009dc3333"

    Handle(TPrsStd_AISPresentation) prs= TPrsStd_AISPresentation::Set(L, guid);
#ifdef OCCT_DEBUG
    cout << "Driver GUID = ";
    prs->GetDriverGUID().ShallowDump(cout);
    cout << "\n";
#endif
    Standard_Character resS[37];
    Standard_PCharacter presS;
    presS=resS;
    guid.ToCString(presS);
    di<<resS;
    return 0; 
  }
  di << "DPrsStd_AISSet : Error\n";
  return 1; 
}

//=======================================================================
//function : DPrsStd_AISDriver
//purpose  : AISDriver (DOC,entry, [ID])
//=======================================================================

static Standard_Integer DPrsStd_AISDriver (Draw_Interpretor& di,
					   Standard_Integer nb, 
					   const char** arg) 
{   
  if (nb >= 3) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1; 

    Standard_GUID guid;
    Handle(TPrsStd_AISPresentation) prs;
    if(L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) {
      if( nb == 3 ) {
	guid = prs->GetDriverGUID();
	Standard_Character str[37];
	Standard_PCharacter pstr;
	pstr=str;
	guid.ToCString( pstr );
	di << str ;
	return 0; 
      }
      else {
	TCollection_ExtendedString str = arg[3];
#ifdef OCCT_DEBUG
	cout << "Inputed parameter > " << str   << endl;
#endif
	if ( str == "A" )  //axis
	  guid = TDataXtd_Axis::GetID();     //"2a96b601-ec8b-11d0-bee7-080009dc3333" 
	else if( str ==  "C")        //constraint 
	  guid = TDataXtd_Constraint::GetID();    //"2a96b602-ec8b-11d0-bee7-080009dc3333" 
	else if( str == "NS" )        //namedshape
	  guid = TNaming_NamedShape::GetID();     //"c4ef4200-568f-11d1-8940-080009dc3333" 
	else if( str == "G" )        //geometry
	  guid = TDataXtd_Geometry::GetID();      //"2a96b604-ec8b-11d0-bee7-080009dc3333"
	else if( str == "PL" )        //plane
	  guid = TDataXtd_Plane::GetID();    //"2a96b60c-ec8b-11d0-bee7-080009dc3333"
	else if( str == "PT" )        //point
	  guid = TDataXtd_Point::GetID();    //"2a96b60d-ec8b-11d0-bee7-080009dc3333"
//	else if( str == "SG" )        //TSketch_Geometry
//	  guid = TSketchStd_Geometry::GetID();    //"b3aac909-5b78-11d1-8940-080009dc3333"
//	else if( str == "E" )        //TSketch_Edge
//	  guid = TSketchStd_Edge::GetID();           //"b3aac90a-5b78-11d1-8940-080009dc3333"

        prs->SetDriverGUID(guid);
	Standard_Character resS[37];
	Standard_PCharacter presS;
	//modified by NIZNHY-PKV Tue Apr 22 16:15:02 2008f
	presS=resS;
	//modified by NIZNHY-PKV Tue Apr 22 16:15:05 2008t
	guid.ToCString( presS );
	di << resS ;
        return 0;
      }
    }
  }
  di << "DPrsStd_AISDriver : Error\n";
  return 1;  
}

//=======================================================================
//function : DPrsStd_AISUnset
//purpose  : AISUnset (DOC,entry)
//=======================================================================

static Standard_Integer DPrsStd_AISUnset (Draw_Interpretor& di,
					  Standard_Integer nb, 
					  const char** arg) 
{   
  if (nb == 3) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1; 
    TPrsStd_AISPresentation::Unset(L);
    TPrsStd_AISViewer::Update(L);
    return 0; 
  }
  di << "DPrsStd_AISDriver : Error\n";
  return 1;  
}


//=======================================================================
//function : DPrsStd_AISTransparency 
//purpose  : AISTransparency  (DOC,entry,[real])
//=======================================================================

static Standard_Integer DPrsStd_AISTransparency (Draw_Interpretor& di,
						 Standard_Integer nb, 
						 const char** arg) 
{
  if (nb >= 3 ) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;  

    Handle(TPrsStd_AISViewer) viewer;
    if( !TPrsStd_AISViewer::Find(L, viewer) ) return 1;  

    Handle(TPrsStd_AISPresentation) prs;
    if(L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) {   
      if( nb == 4 ) {
	prs->SetTransparency(Draw::Atof(arg[3]));
	TPrsStd_AISViewer::Update(L);
      }
      else {
       if (prs->HasOwnTransparency()){ 
         di << "Transparency = " << prs->Transparency() << "\n";
         di<<prs->Transparency();
       }
       else{
         di << "DPrsStd_AISTransparency: Warning : Transparency wasn't set\n";
         di<<(-1);
       }
      }
      return 0;
    }
  }
  di << "DPrsStd_AISTransparency : Error"   << "\n";
  return 1;
}

//=======================================================================
//function : DPrsStd_AISDefaultTransparency
//purpose  : AISDefaultTransparency (DOC,entry)
//=======================================================================

static Standard_Integer DPrsStd_AISDefaultTransparency (Draw_Interpretor& di,
							Standard_Integer nb, 
							const char** arg) 
{   
  if (nb == 3) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;   
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;   

    Handle(TPrsStd_AISPresentation) prs;
    if(L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) {    
      prs->UnsetTransparency();
      TPrsStd_AISViewer::Update(L);
      return 0;
    }
  }
  di << "DPrsStd_AISDefaultTransparency : Error"   << "\n";
  return 1;
}

//=======================================================================
//function : DPrsStd_AISColor 
//purpose  : AISColor (DOC,entry,[color])
//=======================================================================

static Standard_Integer DPrsStd_AISColor (Draw_Interpretor& di,
					  Standard_Integer nb, 
					  const char** arg) 
{
  if (nb >= 3) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;  

    Handle(TPrsStd_AISViewer) viewer;
    if( !TPrsStd_AISViewer::Find(L, viewer) ) return 1;  

    Handle(TPrsStd_AISPresentation) prs;
    if(L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) {   
      if( nb == 4 ) {
	prs->SetColor((Quantity_NameOfColor)Draw::Atoi(arg[3]));
	TPrsStd_AISViewer::Update(L);
      }
      else
       if (prs->HasOwnColor()){ 
         di << "Color = " << prs->Color() << "\n";
         di<<prs->Color();
       }
       else{
         di << "DPrsStd_AISColor: Warning : Color wasn't set\n";
         di<<(-1);
       }
      return 0; 
    }
  }
  di << "DPrsStd_AISColor : Error"   << "\n";
  return 1;
}

//=======================================================================
//function : DPrsStd_AISDefaultColor 
//purpose  : AISDefaultColor (DOC,entry)
//=======================================================================

static Standard_Integer DPrsStd_AISDefaultColor (Draw_Interpretor& di,
						 Standard_Integer nb, 
						 const char** arg) 
{
  if (nb == 3) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;  

    Handle(TPrsStd_AISViewer) viewer;
    if( !TPrsStd_AISViewer::Find(L, viewer) ) return 1;  

    Handle(TPrsStd_AISPresentation) prs;
    if(L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) {   
      prs->UnsetColor();
      TPrsStd_AISViewer::Update(L);
      return 0; 
    }
  }
  di << "DPrsStd_AISDefaultColor : Error"   << "\n";
  return 1;
}

//=======================================================================
//function : DPrsStd_AISMaterial
//purpose  : AISMaterial (DOC,entry,[material])
//=======================================================================

static Standard_Integer DPrsStd_AISMaterial (Draw_Interpretor& di,
					     Standard_Integer nb, 
					     const char** arg) 
{
  if (nb >= 3) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;  

    Handle(TPrsStd_AISViewer) viewer;
    if( !TPrsStd_AISViewer::Find(L, viewer) ) return 1;  

    Handle(TPrsStd_AISPresentation) prs;
    if(L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) {   
      if( nb == 4 ) {
	prs->SetMaterial((Graphic3d_NameOfMaterial)Draw::Atoi(arg[3]));
	TPrsStd_AISViewer::Update(L);
      }
      else {
       if (prs->HasOwnMaterial()){ 
         di << "Material = " << prs->Material() << "\n";
         di<<prs->Material();
       }
       else{
         di << "DPrsStd_AISMaterial: Warning : Material wasn't set\n";
         di<<(-1);
       }
      }
      return 0;
    }
  }
  di << "DPrsStd_AISMaterial : Error"   << "\n";
  return 1;
}

//=======================================================================
//function : DPrsStd_AISDefaultMaterial
//purpose  : AISDefaultMaterial (DOC,entry)
//=======================================================================

static Standard_Integer DPrsStd_AISDefaultMaterial (Draw_Interpretor& di,
						    Standard_Integer nb, 
						    const char** arg) 
{
  if (nb == 3) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;  

    Handle(TPrsStd_AISViewer) viewer;
    if( !TPrsStd_AISViewer::Find(L, viewer) ) return 1;  

    Handle(TPrsStd_AISPresentation) prs;
    if(L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) {   
      prs->UnsetMaterial();
      TPrsStd_AISViewer::Update(L);
      return 0; 
    }
  }
  di << "DPrsStd_AISDefaultMaterial : Error"   << "\n";
  return 1;
}

//=======================================================================
//function : DPrsStd_AISHasOwnColor 
//purpose  : AISHasOwnColor (DOC,entry)
//return   : Boolean 
//=======================================================================

static Standard_Integer DPrsStd_AISHasOwnColor (Draw_Interpretor& di,
					        Standard_Integer nb, 
					        const char** arg) 
{
  if (nb >= 3) {     
    if (nb > 3)
      di << "DPrsStd_AISHasOwnColor : Warning : too many arguments\n";

    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;  

    Handle(TPrsStd_AISViewer) viewer;
    if( !TPrsStd_AISViewer::Find(L, viewer) ) return 1;  

    Handle(TPrsStd_AISPresentation) prs;
    if(L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) {   
      di<<Standard_Integer(prs->HasOwnColor()); 
      return 0; 
    }

  }
  di << "DPrsStd_AISHasOwnColor : Error"   << "\n";
  return 1;
}


//=======================================================================
//function : DPrsStd_AISHasOwnMaterial 
//purpose  : AISHasOwnMaterial (DOC,entry)
//return   : Boolean 
//=======================================================================

static Standard_Integer DPrsStd_AISHasOwnMaterial (Draw_Interpretor& di,
					           Standard_Integer nb, 
					           const char** arg) 
{
  if (nb >= 3) {     
    if (nb > 3)
      di << "DPrsStd_AISHasOwnMaterial : Warning : too many arguments\n";

    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;  

    Handle(TPrsStd_AISViewer) viewer;
    if( !TPrsStd_AISViewer::Find(L, viewer) ) return 1;  

    Handle(TPrsStd_AISPresentation) prs;
    if(L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) {   
      di<<Standard_Integer(prs->HasOwnMaterial()); 
      return 0; 
    }

  }
  di << "DPrsStd_AISHasOwnMaterial : Error"   << "\n";
  return 1;
}


//=======================================================================
//function : DPrsStd_AISHasOwnTransparency 
//purpose  : AISHasOwnColor (DOC,entry)
//return   : Boolean 
//=======================================================================

static Standard_Integer DPrsStd_AISHasOwnTransparency (Draw_Interpretor& di,
					               Standard_Integer nb, 
					               const char** arg) 
{
  if (nb >= 3) {     
    if (nb > 3)
      di << "DPrsStd_AISHasOwnTransparency : Warning : too many arguments\n";

    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;  

    Handle(TPrsStd_AISViewer) viewer;
    if( !TPrsStd_AISViewer::Find(L, viewer) ) return 1;  

    Handle(TPrsStd_AISPresentation) prs;
    if(L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) {   
      di<<Standard_Integer(prs->HasOwnTransparency()); 
      return 0; 
    }

  }
  di << "DPrsStd_AISHasOwnTransparency : Error"   << "\n";
  return 1;
}

//=======================================================================
//function : DPrsStd_AISMode
//purpose  : AISMode (DOC,entry,[Mode])
//=======================================================================
static Standard_Integer DPrsStd_AISMode(Draw_Interpretor& di,
                                        Standard_Integer nb,
					                    const char** arg)
{
  TDF_Label L;
  Handle(TDocStd_Document) D;
  Handle(TPrsStd_AISPresentation) prs;
  if (nb >= 3 && nb <= 4)
  {
    if (!DDocStd::GetDocument(arg[1],D)) 
      return 1;
    if (!DDF::FindLabel(D->GetData(),arg[2],L))
      return 1;
    if (!L.FindAttribute(TPrsStd_AISPresentation::GetID(), prs))
      return 1;
    if (nb == 4)
    {
      Standard_Integer mode = Draw::Atoi(arg[3]);
      prs->SetMode(mode);
      TPrsStd_AISViewer::Update(L);
    }
    else if (nb == 3)
    {
      Standard_Integer mode = prs->Mode();
      di<<mode;
    }
    return 0; 
  }
  di<<"DPrsStd_AISMode : Error\n";
  return 1;
}

//=======================================================================
//function : DPrsStd_AISSelMode
//purpose  : AISSelMode (DOC,entry,[SelMode])
//=======================================================================
static Standard_Integer DPrsStd_AISSelMode(Draw_Interpretor& di,
                                           Standard_Integer nb,
					                       const char** arg)
{
  TDF_Label L;
  Handle(TDocStd_Document) D;
  Handle(TPrsStd_AISPresentation) prs;
  if (nb >= 3 && nb <= 4)
  {
    if (!DDocStd::GetDocument(arg[1],D)) 
      return 1;
    if (!DDF::FindLabel(D->GetData(),arg[2],L))
      return 1;
    if (!L.FindAttribute(TPrsStd_AISPresentation::GetID(), prs))
      return 1;
    if (nb == 4)
    {
      // Set selection mode.
      Standard_Integer selMode = Draw::Atoi(arg[3]);
      prs->SetSelectionMode(selMode);
      TPrsStd_AISViewer::Update(L);
    }
    else if (nb == 3)
    {
      // Print selection mode.
      Standard_Integer selMode = prs->SelectionMode();
      di<<selMode;
    }
    return 0; 
  }
  di<<"DPrsStd_AISSelMode : Error\n";
  return 1;
}

//=======================================================================
//function : AISPresentationCommands
//purpose  :
//=======================================================================


void DPrsStd::AISPresentationCommands (Draw_Interpretor& theCommands)
{  
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  const char* g = "DPrsStd : standard presentation commands" ;  
 
  // standard commands working on AISPresentation

  theCommands.Add ("AISDisplay", 
                   "AISDisplay (DOC, entry, [not_update])",
		   __FILE__, DPrsStd_AISDisplay, g);  

  theCommands.Add ("AISErase", 
                   "AISErase (DOC, entry)",
		   __FILE__, DPrsStd_AISErase, g);    

  theCommands.Add ("AISUpdate", 
                   "AISUpdate (DOC, entry)",
		   __FILE__, DPrsStd_AISUpdate, g);  

  theCommands.Add ("AISSet", 
                   "AISSet (DOC, entry, ID)",
		   __FILE__, DPrsStd_AISSet, g);  

  theCommands.Add ("AISDriver", 
                   "AISDriver (DOC, entry, [ID]) - returns DriverGUID stored in attribute or sets new one",
		   __FILE__, DPrsStd_AISDriver, g);  

  theCommands.Add ("AISUnset", 
                   "AISUnset (DOC, entry)",
		   __FILE__, DPrsStd_AISUnset, g);   

  theCommands.Add ("AISTransparency", 
                   "AISTransparency (DOC, entry, [real])",
		   __FILE__, DPrsStd_AISTransparency, g); 

  theCommands.Add ("AISDefaultTransparency", 
                   "AISDefaultTransparency (DOC, entry)",
		   __FILE__, DPrsStd_AISDefaultTransparency, g);

  theCommands.Add ("AISHasOwnTransparency", 
                   "AISHasOwnTransparency (DOC, entry)  |  AISHasOwnTransparency return Boolean",
		   __FILE__, DPrsStd_AISHasOwnTransparency, g);

  theCommands.Add ("AISDefaultColor", 
                   "AISDefaultColor (DOC, entry)",
		   __FILE__, DPrsStd_AISDefaultColor, g);

  theCommands.Add ("AISColor", 
                   "AISColor (DOC, entry, [color])",
		   __FILE__, DPrsStd_AISColor, g);

  theCommands.Add ("AISHasOwnColor", 
                   "AISHasOwnColor (DOC, entry)  |  AISHasOwnColor return Boolean",
		   __FILE__, DPrsStd_AISHasOwnColor, g);

  theCommands.Add ("AISMaterial", 
                   "AISMaterial (DOC, entry, [material])",
		   __FILE__, DPrsStd_AISMaterial, g); 

  theCommands.Add ("AISDefaultMaterial", 
                   "AISDefaultMaterial (DOC, entry)",
		   __FILE__, DPrsStd_AISDefaultMaterial, g); 

  theCommands.Add ("AISHasOwnMaterial", 
                   "AISHasOwnMaterial (DOC, entry)  |  AISHasOwnMaterial return Boolean",
		   __FILE__, DPrsStd_AISHasOwnMaterial, g);

  theCommands.Add ("AISRemove", 
                   "AISRemove (DOC, entry)",
		   __FILE__, DPrsStd_AISRemove, g);

  theCommands.Add ("AISMode", 
                   "AISMode (DOC, entry, [Mode])",
		   __FILE__, DPrsStd_AISMode, g);

  theCommands.Add ("AISSelMode", 
                   "AISSelMode (DOC, entry, [SelMode])",
		   __FILE__, DPrsStd_AISSelMode, g);
}
