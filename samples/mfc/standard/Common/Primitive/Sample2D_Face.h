#ifndef _Sample2D_Face_HeaderFile
#define _Sample2D_Face_HeaderFile

#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>
#include "TopoDS_Face.hxx"

#include <Standard_Macro.hxx>

DEFINE_STANDARD_HANDLE(Sample2D_Face,AIS2D_InteractiveObject)
class Sample2D_Face : public AIS2D_InteractiveObject {

public:

 // Methods PUBLIC
 // 
Standard_EXPORT Sample2D_Face (TopoDS_Face& aFace);

DEFINE_STANDARD_RTTI(Sample2D_Face)

//           myFace
private:
TopoDS_Face myFace;
public :
TopoDS_Face& Face() { return myFace;};
void SetFace(TopoDS_Face& aFace) { myFace= aFace;};

//           myMarkerIndex
private:
Standard_Integer myMarkerIndex;
public :
Standard_Integer& MarkerIndex() { return myMarkerIndex;};
void SetMarkerIndex(Standard_Integer& aMarkerIndex) { myMarkerIndex= aMarkerIndex;};

//           myMarkerWidth
private:
Quantity_Length myMarkerWidth;
public :
Quantity_Length& MarkerWidth() { return myMarkerWidth;};
void SetMarkerWidth(Quantity_Length& aMarkerWidth) { myMarkerWidth= aMarkerWidth;};

//           myMarkerHeight
private:
Quantity_Length myMarkerHeight;
public :
Quantity_Length& MarkerHeight() { return myMarkerHeight;};
void SetMarkerHeight(Quantity_Length& aMarkerHeight) { myMarkerHeight= aMarkerHeight;};


private:

Standard_Integer myFORWARDColorIndex;
Standard_Integer myREVERSEDColorIndex;
Standard_Integer myINTERNALColorIndex;
Standard_Integer myEXTERNALColorIndex;
Standard_Integer myWidthIndex;
Standard_Integer myTypeIndex ;
public :
Standard_Integer& FORWARDColorIndex() { return myFORWARDColorIndex;};
void SetFORWARDColorIndex(Standard_Integer& aFORWARDColorIndex) { myFORWARDColorIndex= aFORWARDColorIndex;};
public :
Standard_Integer& REVERSEDColorIndex() { return myREVERSEDColorIndex;};
void SetREVERSEDColorIndex(Standard_Integer& aREVERSEDColorIndex) { myREVERSEDColorIndex= aREVERSEDColorIndex;};
public :
Standard_Integer& INTERNALColorIndex() { return myINTERNALColorIndex;};
void SetINTERNALColorIndex(Standard_Integer& aINTERNALColorIndex) { myINTERNALColorIndex= aINTERNALColorIndex;};
public :
Standard_Integer& EXTERNALColorIndex() { return myEXTERNALColorIndex;};
void SetEXTERNALColorIndex(Standard_Integer& aEXTERNALColorIndex) { myEXTERNALColorIndex= aEXTERNALColorIndex;};
public :
Standard_Integer& WidthIndex() { return myWidthIndex;};
void SetWidthIndex(Standard_Integer& aWidthIndex) { myWidthIndex= aWidthIndex;};
public :
Standard_Integer& TypeIndex() { return myTypeIndex;};
void SetTypeIndex(Standard_Integer& aTypeIndex) { myTypeIndex= aTypeIndex;};


private: 
 // Methods PRIVATE
 // 

virtual  void SetContext(const Handle(AIS2D_InteractiveContext)& theContext) ;


};



// other inline functions and methods (like "C++: function call" methods)
//



#endif
