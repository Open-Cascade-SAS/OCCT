#ifndef _Sample2D_Markers_HeaderFile
#define _Sample2D_Markers_HeaderFile

#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>

DEFINE_STANDARD_HANDLE(Sample2D_Markers,AIS2D_InteractiveObject)
class Sample2D_Markers : public AIS2D_InteractiveObject {

enum Sample2D_CurrentTypeOfMarker {
Sample2D_CTOM_Generic,
Sample2D_CTOM_Polyline,
Sample2D_CTOM_Circle,
Sample2D_CTOM_Ellips
} ;

public:

 // Methods PUBLIC
 // 
 // generic marker
 Standard_EXPORT Sample2D_Markers (Standard_Integer    anIndex    , 
                   Quantity_Length     aXPosition ,
                   Quantity_Length     aYPosition ,
                   Quantity_Length     aWidth     ,
                   Quantity_Length     anHeight   ,
                   Quantity_PlaneAngle anAngle    );

 // Polyline marker
 Standard_EXPORT Sample2D_Markers (Quantity_Length          aXPosition , 
                   Quantity_Length          aYPosition ,
                   const Graphic2d_Array1OfVertex& aListVertex);
 // circle marker
 Standard_EXPORT Sample2D_Markers (Quantity_Length aXPosition , 
                   Quantity_Length aYPosition ,
                   Quantity_Length X          ,
                   Quantity_Length Y          ,
		   Quantity_Length Radius    );
 // ellips marker
 Standard_EXPORT Sample2D_Markers (Quantity_Length     aXPosition  , 
                   Quantity_Length     aYPosition  ,
                   Quantity_Length     X           ,
                   Quantity_Length     Y           ,
		   Quantity_Length     MajorRadius ,
                   Quantity_Length     MinorRadius ,
                   Quantity_PlaneAngle anAngle     );

DEFINE_STANDARD_RTTI(Sample2D_Markers)


protected:

 // Methods PROTECTED
 // 


 // Fields PROTECTED
 //


private: 

 // Methods PRIVATE
 // 
Standard_EXPORT virtual void SetContext(const Handle(AIS2D_InteractiveContext)& theContext);

 // Fields PRIVATE
 //
Sample2D_CurrentTypeOfMarker myCurrentTypeOfMarker;

Quantity_Length          myXPosition   ; 
Quantity_Length          myYPosition   ; 
// specific generic markers
Standard_Integer         myIndex       ;
Quantity_Length          myWidth       ;
Quantity_Length          myHeight      ;
// specific polyline marker
Graphic2d_Array1OfVertex myListVertex  ;
// specific circle & ellips markers
Quantity_Length          myX           ;
Quantity_Length          myY           ;
// specific circle marker
Quantity_Length          myRadius      ;
// specific ellips marker
Quantity_Length          myMajorRadius ;
Quantity_Length          myMinorRadius ;

Quantity_PlaneAngle      myAngle       ;


};



// other inCurve functions and methods (like "C++: function call" methods)
//



#endif
