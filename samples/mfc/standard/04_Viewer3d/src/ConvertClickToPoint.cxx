#include <stdafx.h>

gp_Pnt ConvertClickToPoint(Standard_Real x, Standard_Real y, Handle(V3d_View) aView)
{
	
	Standard_Real XEye,YEye,ZEye,XAt,YAt,ZAt;
	aView->Eye(XEye,YEye,ZEye);
	aView->At(XAt,YAt,ZAt);
	gp_Pnt EyePoint(XEye,YEye,ZEye);
	gp_Pnt AtPoint(XAt,YAt,ZAt);

	gp_Vec EyeVector(EyePoint,AtPoint);
	gp_Dir EyeDir(EyeVector);

	gp_Pln PlaneOfTheView = gp_Pln(AtPoint,EyeDir);
	Standard_Real X,Y,Z;
	aView->Convert((int)x,(int)y,X,Y,Z);
	gp_Pnt ConvertedPoint(X,Y,Z);
	gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(PlaneOfTheView,ConvertedPoint);
	
	gp_Pnt ResultPoint = ElSLib::Value(ConvertedPointOnPlane.X(),
									ConvertedPointOnPlane.Y(),
									PlaneOfTheView);
	return ResultPoint;
}


