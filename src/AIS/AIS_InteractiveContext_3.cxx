// Created on: 2001-01-09
// Created by: Sergey Altukhov
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

// Modified: 22/03/04 ; SAN : OCC4895 High-level interface for controlling polygon offsets

#include <AIS_GlobalStatus.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Prs3d_BasicAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Quantity_Color.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Filter.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets
// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets
// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets
//=======================================================================
//function : SetPolygonOffsets 
//purpose  : 
//======================================================================= 
void AIS_InteractiveContext::SetPolygonOffsets(
                             const Handle(AIS_InteractiveObject)& anObj,
                             const Standard_Integer               aMode,
                             const Standard_ShortReal             aFactor,
                             const Standard_ShortReal             aUnits,
                             const Standard_Boolean               updateviewer) 
{
  if ( anObj.IsNull() )
    return;

  setContextToObject (anObj);
  anObj->SetPolygonOffsets( aMode, aFactor, aUnits );

  if ( updateviewer ) {
    if( myObjects.IsBound( anObj ) ) {
      Handle(AIS_GlobalStatus) STATUS = myObjects(anObj);
      if ( STATUS->GraphicStatus() == AIS_DS_Displayed )
        myMainVwr->Update();
    }
  }
}


//=======================================================================
//function : HasPolygonOffsets 
//purpose  : 
//=======================================================================
Standard_Boolean AIS_InteractiveContext::HasPolygonOffsets(const Handle(AIS_InteractiveObject)& anObj) const
{
  return ( !anObj.IsNull() && anObj->HasPolygonOffsets() );
}

//=======================================================================
//function : PolygonOffsets 
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::PolygonOffsets(
                             const Handle(AIS_InteractiveObject)& anObj,
                             Standard_Integer&                    aMode,
                             Standard_ShortReal&                  aFactor,
                             Standard_ShortReal&                  aUnits) const 
{
  if ( HasPolygonOffsets( anObj ) )
    anObj->PolygonOffsets( aMode, aFactor, aUnits );
}
// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::DumpJson (Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myObjects.Size())

  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, mgrSelector.get())
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myMainPM.get())
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myMainVwr.get())
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myMainSel.get())
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myLastActiveView)

  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myLastPicked.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myToHilightSelected)

  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, mySelection.get())
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myFilters.get())
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myDefaultDrawer.get())

  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myStyles[Prs3d_TypeOfHighlight_Selected])
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myStyles[Prs3d_TypeOfHighlight_Dynamic])
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myStyles[Prs3d_TypeOfHighlight_LocalSelected])
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myStyles[Prs3d_TypeOfHighlight_LocalDynamic])
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myStyles[Prs3d_TypeOfHighlight_SubIntensity])

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myDetectedSeq.Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myCurDetected)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myCurHighlighted)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myPickingStrategy)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myAutoHilight)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsAutoActivateSelMode)
}
