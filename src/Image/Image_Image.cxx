#include <Image_Image.ixx>

#ifdef TRACE
static int ImageCount = 0 ;
#endif

Image_Image::Image_Image ( const Handle(Standard_Type)& aPixelType )
{
#ifdef TRACE
  cout << "\tCreate a new Image ( Count : " << ++ImageCount << " )" << endl
       << flush ;
#endif

  myPixelType = aPixelType ;
}

Standard_Integer Image_Image::Size() const {

  return ( Width() * Height() ) ;

}

void Image_Image::InternalDup( const Handle(Image_Image)& aImage ) 
{ // myPixelType is set in Image_Image Constructor
}

Handle(Standard_Type) Image_Image::PixelType() const { return myPixelType ; }

void Image_Image::Destroy ()
{
#ifdef TRACE
  cout << "\tDelete an Image ( Count : " << --ImageCount << " )" << endl
       << flush ;
#endif
}

void Image_Image::RowColor( const Standard_Integer Y,
			    Quantity_Array1OfColor&  PR) const {

  Standard_Integer TheLength = Min (PR.Length(), Width() );
  Standard_Integer L         = PR.Lower() ;
  Standard_Integer X         = LowerX() ;

  for (Standard_Integer i=0; i< TheLength; i++) {
    PR(L+i) = PixelColor(X+i,Y);
  }

}

Handle(Quantity_HArray1OfColor) Image_Image::RowColor( 
		const Standard_Integer Y ) const {

  Standard_Integer TheLength = Width() ;
  Standard_Integer X         = LowerX() ;
  Handle(Quantity_HArray1OfColor) PR = 
		new Quantity_HArray1OfColor( 0, TheLength-1) ;

  for (Standard_Integer i=0; i< TheLength; i++) {
    PR->SetValue(i,PixelColor(X+i,Y));
  }

  return PR ;
}

