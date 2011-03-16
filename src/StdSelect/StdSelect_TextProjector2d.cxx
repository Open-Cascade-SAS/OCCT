// File:	StdSelect_TextProjector2d.cxx
// Created:	Fri Apr 21 17:17:45 1995
// Author:	Robert COUBLANC
//		<rob@fidox>


#include <StdSelect_TextProjector2d.ixx>
#include <Aspect_WindowDriver.hxx>


//==================================================
// Function: 
// Purpose :
//==================================================

StdSelect_TextProjector2d::
StdSelect_TextProjector2d(const Handle(V2d_View)& aView):
myview(aView)
{
}

//==================================================
// Function: 
// Purpose :
//==================================================

void StdSelect_TextProjector2d::
Convert(const gp_Pnt2d& aPointIn, gp_Pnt2d& aPointOut) const 
{
  aPointOut = aPointIn.Transformed(mytrsf);
}

//==================================================
// Function: 
// Purpose :
//==================================================

void StdSelect_TextProjector2d
::Convert(const TCollection_ExtendedString& aText, 
	  const Standard_Real XPos, 
	  const Standard_Real YPos, 
	  gp_Pnt2d& MinPoint, 
	  gp_Pnt2d& MaxPoint, 
	  const Standard_Integer afont) const 
{
  Standard_ShortReal myL,myH;
  MinPoint = gp_Pnt2d(XPos,YPos).Transformed(mytrsf);
  myview->Driver()->TextSize(aText,myL,myH,afont);
  Standard_Real myWinL=myview->Convert(myL);
  Standard_Real myWinH=myview->Convert(myH);

  MaxPoint.SetCoord(XPos+myWinL,YPos+myWinH);
  MaxPoint.Transform(mytrsf);
}



