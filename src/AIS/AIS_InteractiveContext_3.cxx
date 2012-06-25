// Created on: 2001-01-09
// Created by: Sergey Altukhov
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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


// Modified: 22/03/04 ; SAN : OCC4895 High-level interface for controlling polygon offsets

#include <AIS_InteractiveContext.jxx>
// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets
#include <AIS_GlobalStatus.hxx>
// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets

//=======================================================================
//function : Erase
//purpose  : display an interactive object from the collector.
//=======================================================================
void AIS_InteractiveContext::DisplayFromCollector(const Handle(AIS_InteractiveObject)& anIObj,
												  const Standard_Boolean updateviewer)
{
	if ( !IsInCollector( anIObj ) ) return;

	if ( !HasOpenedContext() ){
		
		if ( DisplayStatus( anIObj ) == AIS_DS_Erased )
			Display( anIObj,Standard_False);

		if( updateviewer ){
			myMainVwr->Update();
			if( !myCollectorVwr.IsNull() )
				myCollectorVwr->Update();
		}
	}
}

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

  if( !anObj->HasInteractiveContext() )
    anObj->SetContext( this );

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
