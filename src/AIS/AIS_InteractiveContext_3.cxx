// File:	AIS_InteractiveContext_3.cxx
// Created:	Tue Jan 09 17:21:39 2001
// Author:	Sergey Altukhov
//			<s-altukhov@nnov.matra-dtv.fr>

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
                             const Standard_Real                  aFactor,
                             const Standard_Real                  aUnits,
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
                             Standard_Real&                       aFactor,
                             Standard_Real&                       aUnits) const 
{
  if ( HasPolygonOffsets( anObj ) )
    anObj->PolygonOffsets( aMode, aFactor, aUnits );
}
// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
