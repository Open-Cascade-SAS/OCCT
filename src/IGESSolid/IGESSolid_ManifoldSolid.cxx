// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//--------------------------------------------------------------------
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
