#ifndef _Sample2D_Curve_HeaderFile
#define _Sample2D_Curve_HeaderFile

#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>


#include <Quantity_Length.hxx>
#include <Aspect_TypeOfline.hxx>
#include <Aspect_WidthOfline.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <Handle_PrsMgr_PresentationManager2d.hxx>
#include <Handle_Graphic2d_GraphicObject.hxx>
#include <Handle_SelectMgr_Selection.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
#include <Standard_CString.hxx>

class TColGeom2d_HSequenceOfCurve;
class PrsMgr_PresentationManager2d;
class Graphic2d_GraphicObject;
class SelectMgr_Selection;
class ISession2D_ObjectOwner;

#include "Geom2d_Curve.hxx"

#include "AIS2D_InteractiveObject.hxx"  
DEFINE_STANDARD_HANDLE(Sample2D_Curve,AIS2D_InteractiveObject)
class Sample2D_Curve : public AIS2D_InteractiveObject {

public:

 // Methods PUBLIC
 // 
   Standard_EXPORT Sample2D_Curve(const Handle_Geom2d_Curve aGeom2dCurve,
                  const Standard_Integer aTypeOfline = 1,
                  const Standard_Integer aWidthOfLine = 1,
                  const Standard_Integer aColorIndex = 4);

inline   Standard_Integer  GetTypeOfLineIndex
                              () const;
inline   void               SetTypeOfLineIndex
                              (const Standard_Integer aNewTypeOfLineIndex) ;
inline   Standard_Integer GetWidthOfLineIndex
                              () const;
inline   void               SetWidthOfLineIndex
                              (const Standard_Integer aNewWidthOfLineIndex) ;
inline   Standard_Integer   GetColorIndex
                              () const;
inline   void               SetColorIndex
                              (const Standard_Integer aNewColorIndex) ;

DEFINE_STANDARD_RTTI(Sample2D_Curve)


protected:

 // Methods PROTECTED
 // 


 // Fields PROTECTED
 //


private: 

 // Methods PRIVATE
 // 
virtual  void SetContext(const Handle(AIS2D_InteractiveContext)& theContext) ;


 // Fields PRIVATE
 //
Handle_Geom2d_Curve myGeom2dCurve;
Standard_Integer   myTypeOfLineIndex;
Standard_Integer  myWidthOfLineIndex;
Standard_Integer    myColorIndex;


};



// other inCurve functions and methods (like "C++: function call" methods)
//


inline  Standard_Integer Sample2D_Curve::GetTypeOfLineIndex() const
{
  return myTypeOfLineIndex ;
}
 
inline  void Sample2D_Curve::SetTypeOfLineIndex(const Standard_Integer aNewTypeOfLineIndex) 
{
  myTypeOfLineIndex = aNewTypeOfLineIndex;
}

inline  Standard_Integer Sample2D_Curve::GetWidthOfLineIndex() const
{
  return myWidthOfLineIndex ;
}

inline  void Sample2D_Curve::SetWidthOfLineIndex(const Standard_Integer aNewWidthOfLineIndex) 
{
  myWidthOfLineIndex = aNewWidthOfLineIndex;
}

inline  Standard_Integer Sample2D_Curve::GetColorIndex() const
{
  return myColorIndex ;
}

inline  void Sample2D_Curve::SetColorIndex(const Standard_Integer aNewColorIndex) 
{
  myColorIndex = aNewColorIndex;
}

 


#endif
