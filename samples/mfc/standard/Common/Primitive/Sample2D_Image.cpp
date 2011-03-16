#include "stdafx.h"

#include "Sample2D_Image.h"
#include <Graphic2d_View.hxx>

IMPLEMENT_STANDARD_HANDLE(Sample2D_Image,AIS2D_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(Sample2D_Image,AIS2D_InteractiveObject)

#include <Graphic2d_ImageFile.hxx>
Sample2D_Image::Sample2D_Image(OSD_File& aFile,
                               const Quantity_Length X,       // = 0.0
                               const Quantity_Length Y,       // = 0.0
                               const Quantity_Length adx,     // = 0.0
                               const Quantity_Length ady,     // = 0.0
                               const Aspect_CardinalPoints aTypeOfPlacement,// = Aspect_CP_Center
                               const Quantity_Factor aScale)  // = 1.0
    :AIS2D_InteractiveObject()
{

  myFile            = aFile            ;
  myX               = X                ;
  myY               = Y                ;
  myDx              = adx              ;  
  myDy              = ady              ;     
  myTypeOfPlacement = aTypeOfPlacement ;
  myScale           = aScale           ;

}

Sample2D_Image::Sample2D_Image(Standard_CString& aFileName,
                               const Quantity_Length X,       // = 0.0
                               const Quantity_Length Y,       // = 0.0
                               const Quantity_Length adx,     // = 0.0
                               const Quantity_Length ady,     // = 0.0
                               const Aspect_CardinalPoints aTypeOfPlacement,// = Aspect_CP_Center
                               const Quantity_Factor aScale) // = 1.0)
    :AIS2D_InteractiveObject()
{
  TCollection_AsciiString TheDependentName(aFileName);
  OSD_Path aPath(TheDependentName);
  OSD_File aFile(aPath);
  myFile            = aFile            ;
  myX               = X                ;
  myY               = Y                ;
  myDx              = adx              ;  
  myDy              = ady              ;     
  myTypeOfPlacement = aTypeOfPlacement ;
  myScale           = aScale           ;

  //Attach a graphic view to this object
}

void Sample2D_Image::SetContext(const Handle(AIS2D_InteractiveContext)& theContext) 
{

  if(theContext.IsNull() || theContext->CurrentViewer().IsNull()) return;

  AIS2D_InteractiveObject::SetContext(theContext);
  Graphic2d_GraphicObject::SetView(theContext->CurrentViewer()->View());


  Handle(Graphic2d_ImageFile) aGraphic2dImageFile = new Graphic2d_ImageFile (
                             this,
			                 myFile,
			                 myX, 
			                 myY,
			                 myDx,
			                 myDy,
			                 myTypeOfPlacement,
			                 myScale); 
  if (myIsZoomable)
    aGraphic2dImageFile->SetZoomable(Standard_True);
}

Standard_CString Sample2D_Image::GetFileName() const
{
  static OSD_Path aPath;
  myFile.Path(aPath);
  return aPath.Name().ToCString();
}

void Sample2D_Image::SetFileName(const Standard_CString aFileName) 
{
  TCollection_AsciiString TheDependentName(aFileName);
  OSD_Path aPath(TheDependentName);
  OSD_File aFile(aPath);
  myFile = aFile;
}




