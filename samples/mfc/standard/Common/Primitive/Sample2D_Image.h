#ifndef _Sample2D_Image_HeaderFile
#define _Sample2D_Image_HeaderFile
#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>

#include <OSD_File.hxx>
#include <Quantity_Length.hxx>
#include <Quantity_Factor.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
class Sample2D_Image;
DEFINE_STANDARD_HANDLE(Sample2D_Image,AIS_TexturedShape)
class Sample2D_Image : public AIS_TexturedShape {

public:

Standard_EXPORT Sample2D_Image(TCollection_AsciiString& aFileName,
                               const Quantity_Length X = 0.0,
                               const Quantity_Length Y = 0.0,
                               const Quantity_Factor aScale = 1.0);
Standard_EXPORT inline virtual  void GetCoord(Quantity_Length& X,Quantity_Length& Y) const;
Standard_EXPORT inline virtual  void SetCoord(const Quantity_Length X,const Quantity_Length Y) ;
Standard_EXPORT inline   Quantity_Factor GetScale() const;
Standard_EXPORT inline   void SetScale(const Quantity_Factor aNewScale) ;
Standard_EXPORT virtual void SetContext(const Handle(AIS_InteractiveContext)& theContext) ;

DEFINE_STANDARD_RTTIEXT(Sample2D_Image,AIS_TexturedShape)
private:
  void MakeShape();
protected:
TopoDS_Face myFace;
TCollection_AsciiString myFilename;
Quantity_Length       myX;
Quantity_Length       myY;
Quantity_Factor       myScale;

};

// other inline functions and methods (like "C++: function call" methods)
//

inline  void Sample2D_Image::GetCoord(Quantity_Length& X, Quantity_Length& Y) const
{
  X = myX;
  Y = myY;
}

inline  void Sample2D_Image::SetCoord(const Quantity_Length X, const Quantity_Length Y) 
{
  myX = X;
  myY = Y;
}

inline  Quantity_Factor Sample2D_Image::GetScale() const
{
  return myScale;
}

inline  void Sample2D_Image::SetScale(const Quantity_Factor aNewScale) 
{
  myScale = aNewScale;
}



#endif
