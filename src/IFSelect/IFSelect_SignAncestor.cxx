// File:	IFSelect_SignAncestor.cxx
// Created:	Wed Feb 17 17:22:38 1999
// Author:	Pavel DURANDIN
//		<pdn@friendox.nnov.matra-dtv.fr>


#include <IFSelect_SignAncestor.ixx>
#include <Standard_Type.hxx>
#include <Interface_Macros.hxx>

IFSelect_SignAncestor::IFSelect_SignAncestor (const Standard_Boolean nopk) 
     : IFSelect_SignType (nopk) {  }
     
Standard_Boolean IFSelect_SignAncestor::Matches(const Handle(Standard_Transient)& ent,
						const Handle(Interface_InterfaceModel)& model,
						const TCollection_AsciiString& text,
						const Standard_Boolean exact) const
{
  if (ent.IsNull()) return Standard_False;
  DeclareAndCast(Standard_Type,atype,ent);
  if (atype.IsNull()) atype = ent->DynamicType();
  return atype->SubType(text.ToCString());
}
						
