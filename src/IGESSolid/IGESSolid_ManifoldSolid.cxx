//--------------------------------------------------------------------
//
//  File Name : IGESSolid_ManifoldSolid.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESSolid_ManifoldSolid.ixx>


    IGESSolid_ManifoldSolid::IGESSolid_ManifoldSolid ()    {  }


    void  IGESSolid_ManifoldSolid::Init
  (const Handle(IGESSolid_Shell)& aShell,
   const Standard_Boolean Shellflag,
   const Handle(IGESSolid_HArray1OfShell)& VoidShells,
   const Handle(TColStd_HArray1OfInteger)& VoidShellFlags)
{
  if (!VoidShells.IsNull())
    if (VoidShells->Lower()  != 1 || VoidShellFlags->Lower() != 1 ||
	VoidShells->Length() != VoidShellFlags->Length())
      Standard_DimensionError::Raise("IGESSolid_ManifoldSolid : Init");

  theShell           = aShell;
  theOrientationFlag = Shellflag;
  theVoidShells      = VoidShells;
  theOrientFlags     = VoidShellFlags;
  InitTypeAndForm(186,0);
}

    Handle(IGESSolid_Shell)  IGESSolid_ManifoldSolid::Shell () const
{
  return theShell;
}

    Standard_Boolean  IGESSolid_ManifoldSolid::OrientationFlag () const
{
  return theOrientationFlag;
}

    Standard_Integer  IGESSolid_ManifoldSolid::NbVoidShells () const
{
  return (theVoidShells.IsNull() ? 0 : theVoidShells->Length());
}

    Handle(IGESSolid_Shell)  IGESSolid_ManifoldSolid::VoidShell
  (const Standard_Integer index) const
{
  Handle(IGESSolid_Shell) ashell;    // par defaut sera Null
  if (!theVoidShells.IsNull()) ashell = theVoidShells->Value(index);
  return ashell;
}

    Standard_Boolean  IGESSolid_ManifoldSolid::VoidOrientationFlag
  (const Standard_Integer index) const
{
  if (!theOrientFlags.IsNull())    return (theOrientFlags->Value(index) != 0);
  else return Standard_False;  // pour retourner qqchose ...
}
