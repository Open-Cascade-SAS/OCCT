#ifndef _Sample2D_Rectangle_HeaderFile
#define _Sample2D_Rectangle_HeaderFile

#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>

DEFINE_STANDARD_HANDLE(Sample2D_Rectangle,AIS2D_InteractiveObject)
class Sample2D_Rectangle : public AIS2D_InteractiveObject {

public:

 // Methods PUBLIC
 // 
 Standard_EXPORT Sample2D_Rectangle (Quantity_Length     XPosition ,
                     Quantity_Length     YPosition ,
                     Quantity_Length     aWidth    ,
                     Quantity_Length     anHeight  ,
                     Quantity_PlaneAngle anAngle   );


DEFINE_STANDARD_RTTI(Sample2D_Rectangle)


Standard_EXPORT virtual void SetContext(const Handle(AIS2D_InteractiveContext)& theContext);


protected:

 // Methods PROTECTED
 // 


 // Fields PROTECTED
 //


private: 

 // Methods PRIVATE
 // 
 // Fields PRIVATE
 //

Quantity_Length          myXPosition   ; 
Quantity_Length          myYPosition   ; 
Quantity_Length          myWidth       ;
Quantity_Length          myHeight      ;
Quantity_PlaneAngle      myAngle       ;

Graphic2d_Array1OfVertex myArray1OfVertex;
void  ComputeArrayOfVertex();

};



// other inCurve functions and methods (like "C++: function call" methods)
//



#endif
