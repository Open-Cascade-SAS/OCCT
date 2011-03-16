// File:	StdSelect_SensitiveText2d.cxx
// Created:	Fri Apr 21 15:15:02 1995
// Author:	Robert COUBLANC
//		<rob@fidox>


#include <StdSelect_SensitiveText2d.ixx>
#include <StdSelect_TextProjector2d.hxx>
#include <gp_Pnt2d.hxx>


StdSelect_SensitiveText2d::
StdSelect_SensitiveText2d (const Handle(SelectBasics_EntityOwner)& anOwnerId, 
			   const TCollection_ExtendedString& aString, 
			   const Standard_Real XPos, 
			   const Standard_Real YPos, 
			   const Standard_Real Angle, 
			   const Standard_Integer aFontIndex):
Select2D_SensitiveEntity(anOwnerId),
mytext(aString),
myxpos(XPos),
myypos(YPos),
myangle(Angle),
myfont(aFontIndex) {}



void StdSelect_SensitiveText2d::Convert(const Handle(Select2D_Projector)& aPrj)
{
  if(!(Handle(StdSelect_TextProjector2d)::DownCast(aPrj)).IsNull())
    {
      gp_Pnt2d FirstP,LastP;
      Handle(StdSelect_TextProjector2d)::DownCast(aPrj)
	->Convert(mytext,myxpos,myypos,FirstP,LastP,myfont);
      myinitbox.Set(FirstP);
      myinitbox.Add(LastP);
    }
}

void StdSelect_SensitiveText2d::Areas(SelectBasics_ListOfBox2d& aresult)
{
  if(myangle==0.) aresult.Append(myinitbox);
  else {
    gp_Trsf2d T; T.SetRotation(gp_Pnt2d(myxpos,myypos),myangle);
    aresult.Append(myinitbox.Transformed(T));
  }
}

Standard_Boolean StdSelect_SensitiveText2d::Matches(const Standard_Real X, 
						    const Standard_Real Y, 
						    const Standard_Real aTol, 
						    Standard_Real& DMin)
{
  return Standard_True;
}

Standard_Boolean StdSelect_SensitiveText2d::Matches
       (const Standard_Real  XMin,
	const Standard_Real  YMin,
	const Standard_Real  XMax,
	const Standard_Real  YMax,
	const Standard_Real aTol) {

	 return Standard_True;
	 
       }

Standard_Boolean StdSelect_SensitiveText2d::Matches
       (const TColgp_Array1OfPnt2d& aPoly,
	const Bnd_Box2d& aBox,
	const Standard_Real aTol) {

	 return Standard_True;
	 
       }
