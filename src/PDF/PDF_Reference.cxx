// File:	PDF_Reference.cxx
// Created:	Wed Mar  1 14:33:57 2000
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>


#include <PDF_Reference.ixx>


PDF_Reference::PDF_Reference() { }

PDF_Reference::PDF_Reference(const Handle(PCollection_HAsciiString)& V)
: myValue (V) {}

Handle(PCollection_HAsciiString) PDF_Reference::ReferencedLabel() const
{ return myValue; }

void PDF_Reference::ReferencedLabel(const Handle(PCollection_HAsciiString)& V) 
{ myValue = V; }

