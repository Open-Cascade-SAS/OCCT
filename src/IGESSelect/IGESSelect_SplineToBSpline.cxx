#include <IGESSelect_SplineToBSpline.ixx>
#include <IGESData_IGESEntity.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Check.hxx>



IGESSelect_SplineToBSpline::IGESSelect_SplineToBSpline
  (const Standard_Boolean tryC2)
{
  thetryc2 = tryC2;  thefound = Standard_False;
}

Standard_Boolean  IGESSelect_SplineToBSpline::OptionTryC2 () const
{
  return thetryc2;
}


Standard_Boolean  IGESSelect_SplineToBSpline::Perform
  (const Interface_Graph& G, const Handle(Interface_Protocol)&,
   Interface_CheckIterator& checks,
   Handle(Interface_InterfaceModel)& newmod)
{
  Standard_Integer nbe = G.Size();
  thefound = Standard_False;
  themap.Nullify();
  for (Standard_Integer i = 1; i <= nbe; i ++) {
    DeclareAndCast(IGESData_IGESEntity,ent,G.Entity(i));
    if (ent.IsNull()) continue;
    Standard_Integer it = ent->TypeNumber();
    if (it == 112 || it == 126) {
      thefound = Standard_True;
#ifdef DEB
      cout<<"IGESSelect_SplineToBSpline : n0."<<i
	<< (it == 112 ? ", Curve" : ", Surface")<<" to convert"<<endl;
#endif
    }
  }
  newmod.Nullify();
  if (!thefound) return Standard_True;

//  Il faudrait convertir ...
  checks.CCheck(0)->AddFail("IGESSelect_SplineToBSpline : not yet implemented");
  return Standard_False;
}


Standard_Boolean  IGESSelect_SplineToBSpline::Updated
  (const Handle(Standard_Transient)& entfrom,
   Handle(Standard_Transient)& entto) const
{
  if (!thefound) {
    entto = entfrom;
    return Standard_True;
  }
  if (themap.IsNull()) return Standard_False;
  return themap->Search(entfrom,entto);
}


TCollection_AsciiString  IGESSelect_SplineToBSpline::Label () const
{
  if (thetryc2) return TCollection_AsciiString
    ("Convert Spline Forms to BSpline, trying to recover C1-C2 continuity");
  else return TCollection_AsciiString ("Convert Spline Forms to BSpline");
}
