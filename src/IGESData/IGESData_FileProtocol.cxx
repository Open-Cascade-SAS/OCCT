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

#include  <IGESData_FileProtocol.ixx>


IGESData_FileProtocol::IGESData_FileProtocol ()    {  }

    void  IGESData_FileProtocol::Add (const Handle(IGESData_Protocol)& protocol)
{
  if      (theresource.IsNull()) theresource = protocol;
  else if (theresource->IsInstance(protocol->DynamicType())) return; // passer
  else if (!thenext.IsNull()) thenext->Add(protocol);
  else {
    thenext = new IGESData_FileProtocol;
    thenext->Add(protocol);
  }
}

    Standard_Integer  IGESData_FileProtocol::NbResources () const
{
  Standard_Integer nb = (theresource.IsNull() ? 0 : 1);
  if (!thenext.IsNull()) nb += thenext->NbResources();
  return nb;
}

    Handle(Interface_Protocol) IGESData_FileProtocol::Resource
  (const Standard_Integer num) const
{
  Handle(IGESData_Protocol) res;
  if (num == 1) return theresource;
  else if (!thenext.IsNull()) return thenext->Resource(num-1);
  return res;  // Null
}
