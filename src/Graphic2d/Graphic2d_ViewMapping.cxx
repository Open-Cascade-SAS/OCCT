#include <Graphic2d_ViewMapping.ixx>
#include <Aspect_Units.hxx>

Graphic2d_ViewMapping::Graphic2d_ViewMapping ()
: myXCenter (0.), myYCenter (0.), mySize (1. METER),
myInitialXCenter(0.),myInitialYCenter(0),myInitialSize(1.) {
}

void Graphic2d_ViewMapping::SetViewMapping (const Quantity_Length aXCenter, const Quantity_Length aYCenter, const Quantity_Length aSize) {

	myXCenter	= aXCenter;
	myYCenter	= aYCenter;
	mySize		= aSize;

}

void Graphic2d_ViewMapping::SetCenter
(const Quantity_Length aXCenter, const Quantity_Length aYCenter) {

	myXCenter	= aXCenter;
	myYCenter	= aYCenter;

}

void Graphic2d_ViewMapping::SetSize (const Quantity_Length aSize) {

	mySize		= aSize;

}

void Graphic2d_ViewMapping::SetViewMappingDefault () {

	myInitialXCenter	= myXCenter;
	myInitialYCenter	= myYCenter;
	myInitialSize		= mySize;

}

void Graphic2d_ViewMapping::ViewMappingDefault (Quantity_Length& XCenter,
					Quantity_Length& YCenter,
					Quantity_Length& aSize) const {
	XCenter	= myInitialXCenter;
	YCenter	= myInitialYCenter;
	aSize	= myInitialSize;

}

void Graphic2d_ViewMapping::ViewMappingReset () {

	myXCenter	= myInitialXCenter;
	myYCenter	= myInitialYCenter;
	mySize	= myInitialSize;

}

void Graphic2d_ViewMapping::ViewMapping
           (Quantity_Length& XCenter,
	    Quantity_Length& YCenter,
	    Quantity_Length& Size) const {

	      XCenter = myXCenter;
	      YCenter = myYCenter;
	      Size = mySize;
}
void Graphic2d_ViewMapping::Center
           (Quantity_Length& XCenter,
	    Quantity_Length& YCenter) const {

	      XCenter = myXCenter;
	      YCenter = myYCenter;
}

Quantity_Factor Graphic2d_ViewMapping::Zoom () const {

	return mySize/myInitialSize;

}
