#ifndef _Sample2D_Image_HeaderFile
#define _Sample2D_Image_HeaderFile
#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>

#include <OSD_File.hxx>
#include <Quantity_Length.hxx>
#include <Aspect_CardinalPoints.hxx>
#include <Quantity_Factor.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Handle_PrsMgr_PresentationManager2d.hxx>
#include <Handle_Graphic2d_GraphicObject.hxx>
#include <Standard_Integer.hxx>
#include <Handle_SelectMgr_Selection.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
#include <AIS2D_InteractiveObject.hxx>

DEFINE_STANDARD_HANDLE(Sample2D_Image,AIS2D_InteractiveObject)
class Sample2D_Image : public AIS2D_InteractiveObject {

public:

 // Methods PUBLIC
 // 
Standard_EXPORT Sample2D_Image(OSD_File& aFile,
                               const Quantity_Length X = 0.0,
                               const Quantity_Length Y = 0.0,
                               const Quantity_Length adx = 0.0,
                               const Quantity_Length ady = 0.0,
                               const Aspect_CardinalPoints aTypeOfPlacement = Aspect_CP_Center,
                               const Quantity_Factor aScale = 1.0);
Standard_EXPORT Sample2D_Image(Standard_CString& aFileName,
                               const Quantity_Length X = 0.0,
                               const Quantity_Length Y = 0.0,
                               const Quantity_Length adx = 0.0,
                               const Quantity_Length ady = 0.0,
                               const Aspect_CardinalPoints aTypeOfPlacement = Aspect_CP_Center,
                               const Quantity_Factor aScale = 1.0);
Standard_EXPORT inline virtual  OSD_File GetFile() const;
Standard_EXPORT inline virtual  void SetFile(const OSD_File& anImage) ;
Standard_EXPORT virtual  Standard_CString GetFileName() const;
Standard_EXPORT virtual  void SetFileName(const Standard_CString anImageName) ;
Standard_EXPORT inline virtual  void GetCoord(Quantity_Length& X,Quantity_Length& Y) const;
Standard_EXPORT inline virtual  void SetCoord(const Quantity_Length X,const Quantity_Length Y) ;
Standard_EXPORT inline virtual  void GetOffset(Quantity_Length& adX,Quantity_Length& adY) const;
Standard_EXPORT inline virtual  void SetOffset(const Quantity_Length adX,const Quantity_Length adY) ;
Standard_EXPORT inline virtual  Aspect_CardinalPoints GetTypeOfPlacement() const;
Standard_EXPORT inline virtual  void SetTypeOfPlacement(const Aspect_CardinalPoints aTypeOfPlacement = Aspect_CP_Center) ;
Standard_EXPORT inline   Quantity_Factor GetScale() const;
Standard_EXPORT inline   void SetScale(const Quantity_Factor aNewScale) ;
Standard_EXPORT inline   Standard_Boolean GetIsZoomable() const;
Standard_EXPORT inline   void SetIsZoomable(const Standard_Boolean aIsZoomable) ;

Standard_EXPORT virtual void SetContext(const Handle(AIS2D_InteractiveContext)& theContext) ;

DEFINE_STANDARD_RTTI(Sample2D_Image)

protected:

 // Methods PROTECTED
 // 

 // Fields PROTECTED
 //
OSD_File              myFile;
Quantity_Length       myX;
Quantity_Length       myY;
Quantity_Length       myDx;
Quantity_Length       myDy;
Aspect_CardinalPoints myTypeOfPlacement;
Quantity_Factor       myScale;
Standard_Boolean      myIsZoomable;

private: 

 // Methods PRIVATE
 // 

 // Fields PRIVATE
 //

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

inline  OSD_File Sample2D_Image::GetFile() const
{
  return myFile;
}
 
inline  void Sample2D_Image::SetFile(const OSD_File& aFile) 
{
  myFile = aFile;
}

inline  void Sample2D_Image::GetOffset(Quantity_Length& adX,Quantity_Length& adY) const
{
  adX = myDx;
  adY = myDy;
}
 
inline  void Sample2D_Image::SetOffset(const Quantity_Length adX,const Quantity_Length adY) 
{
  myDx = adX;
  myDy = adY;
}
 
inline  Aspect_CardinalPoints Sample2D_Image::GetTypeOfPlacement() const
{
  return myTypeOfPlacement;
}
 
inline  void Sample2D_Image::SetTypeOfPlacement(const Aspect_CardinalPoints aTypeOfPlacement) 
{
  myTypeOfPlacement = aTypeOfPlacement;
}


inline  Quantity_Factor Sample2D_Image::GetScale() const
{
  return myScale;
}

inline  void Sample2D_Image::SetScale(const Quantity_Factor aNewScale) 
{
  myScale = aNewScale;
}

inline  Standard_Boolean Sample2D_Image::GetIsZoomable() const
{
  return myIsZoomable;
}

inline  void Sample2D_Image::SetIsZoomable(const Standard_Boolean aIsZoomable) 
{
  myIsZoomable = aIsZoomable;
}


#endif
