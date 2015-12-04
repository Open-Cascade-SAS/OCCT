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

// last modified by SRN 01/08/2000    

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <Geom_Transformation.hxx>
#include <Prs3d_Presentation.hxx>
#include <PrsMgr_Presentation.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TDataStd.hxx>
#include <TDF_Label.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TopLoc_Location.hxx>
#include <TPrsStd_DriverTable.hxx>
#include <TPrsStd_NamedShapeDriver.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TPrsStd_NamedShapeDriver,TPrsStd_Driver)

#undef OPTIM_UPDATE  // If this variable is defined there will be done
//                      more otimized update of AIS_Shape. If an object was
//                      erased in the viewer and it's location was changed
//                      but topological data wasn't then when displayed only 
//                      the object's presentation will be moved to new location
//                      without recompute. The shape in AIS_Shape will 
//                      be the previous one with the old location.       
//                      NOTE! After selection of sub shapes of the object
//                      they will have THE OLD LOCATION and it has to be
//                      compared with location of AIS_Shape that will contain
//                      the right location of shape. 
 

//=======================================================================
//function :
//purpose  : 
//=======================================================================
TPrsStd_NamedShapeDriver::TPrsStd_NamedShapeDriver()
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean TPrsStd_NamedShapeDriver::Update (const TDF_Label& aLabel,
						  Handle(AIS_InteractiveObject)& AIS) 
{
  Handle(TNaming_NamedShape) NS;

  if( !aLabel.FindAttribute(TNaming_NamedShape::GetID(), NS) )  {
    return Standard_False;  
  }

  //TopoDS_Shape S = TNaming_Tool::CurrentShape (NS);
  TopoDS_Shape S = TNaming_Tool::GetShape (NS);
  if(S.IsNull()){ 
    return Standard_False; 
  }  
  TopLoc_Location L  = S.Location();

  Handle(AIS_Shape) AISShape;
  if (AIS.IsNull()) AISShape = new AIS_Shape(S);
  else {
    AISShape = Handle(AIS_Shape)::DownCast(AIS);
    if (AISShape.IsNull()) {
      AISShape = new AIS_Shape(S);
    }
    else {
      TopoDS_Shape oldShape = AISShape->Shape();
      if(oldShape != S) {
	AISShape->ResetTransformation();

#ifdef OPTIM_UPDATE
	Handle(AIS_InteractiveContext) ctx = AISShape->GetContext();
	if(S.IsPartner(oldShape) && (!ctx.IsNull() && !ctx->IsDisplayed(AISShape))) {
	  if(L != oldShape.Location()) ctx->SetLocation(AISShape, L);
	}
	else {
	  AISShape->Set(S);
	  AISShape->UpdateSelection();
	  AISShape->SetToUpdate(); 	  
	}
#else
	AISShape->Set(S);
	AISShape->UpdateSelection();
	AISShape->SetToUpdate();      
#endif
      }
    }

    AISShape->SetInfiniteState(S.Infinite());
  }
  AIS = AISShape;
  return Standard_True;
 
}

