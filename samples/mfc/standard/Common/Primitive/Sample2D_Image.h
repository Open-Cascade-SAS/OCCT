#ifndef _Sample2D_Image_HeaderFile
#define _Sample2D_Image_HeaderFile
#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>

#include <OSD_File.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
class Sample2D_Image;
DEFINE_STANDARD_HANDLE(Sample2D_Image,AIS_Shape)
class Sample2D_Image : public AIS_Shape {

public:

Standard_EXPORT Sample2D_Image(TCollection_AsciiString& aFileName,
                               const Standard_Real X = 0.0,
                               const Standard_Real Y = 0.0,
                               const Standard_Real aScale = 1.0);
Standard_EXPORT inline virtual  void GetCoord(Standard_Real& X,Standard_Real& Y) const;
Standard_EXPORT inline virtual  void SetCoord(const Standard_Real X,const Standard_Real Y) ;
Standard_EXPORT inline   Standard_Real GetScale() const;
Standard_EXPORT inline   void SetScale(const Standard_Real aNewScale) ;
Standard_EXPORT virtual void SetContext(const Handle(AIS_InteractiveContext)& theContext) ;

DEFINE_STANDARD_RTTIEXT(Sample2D_Image,AIS_Shape)
private:
  void MakeShape();
protected:
TopoDS_Face myFace;
TCollection_AsciiString myFilename;
Standard_Real myX;
Standard_Real myY;
Standard_Real myScale;

};

// other inline functions and methods (like "C++: function call" methods)
//

inline  void Sample2D_Image::GetCoord(Standard_Real& X, Standard_Real& Y) const
{
  X = myX;
  Y = myY;
}

inline  void Sample2D_Image::SetCoord(const Standard_Real X, const Standard_Real Y) 
{
  myX = X;
  myY = Y;
}

inline Standard_Real Sample2D_Image::GetScale() const
{
  return myScale;
}

inline void Sample2D_Image::SetScale(const Standard_Real aNewScale) 
{
  myScale = aNewScale;
}



#endif
