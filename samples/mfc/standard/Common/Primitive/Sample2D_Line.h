#ifndef _Sample2D_Line_HeaderFile
#define _Sample2D_Line_HeaderFile

#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>

DEFINE_STANDARD_HANDLE(Sample2D_Line,AIS2D_InteractiveObject)
class Sample2D_Line : public AIS2D_InteractiveObject {

public:

 // Methods PUBLIC
 // 
Standard_EXPORT Sample2D_Line ();


DEFINE_STANDARD_RTTI(Sample2D_Line)

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

};



// other inCurve functions and methods (like "C++: function call" methods)
//



#endif
