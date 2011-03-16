//File GccEnt_QualifiedCirc.cxx, REG 19/07/91

#include <GccEnt_QualifiedCirc.ixx>


gp_Circ2d GccEnt_QualifiedCirc::
   Qualified () const { return TheQualified; }

GccEnt_Position GccEnt_QualifiedCirc::
   Qualifier () const { return TheQualifier; }

Standard_Boolean GccEnt_QualifiedCirc::
   IsUnqualified () const {
     if (TheQualifier == GccEnt_unqualified ) { return Standard_True; }
     else { return Standard_False; }
   }

Standard_Boolean GccEnt_QualifiedCirc::
   IsEnclosing () const {
     if (TheQualifier == GccEnt_enclosing) { return Standard_True; }
     else { return Standard_False; }
   }

Standard_Boolean GccEnt_QualifiedCirc::
   IsEnclosed () const {
     if (TheQualifier == GccEnt_enclosed) { return Standard_True; }
     else { return Standard_False; }
   }
Standard_Boolean GccEnt_QualifiedCirc::
   IsOutside () const {
     if (TheQualifier == GccEnt_outside) { return Standard_True; }
     else { return Standard_False; }
   }

GccEnt_QualifiedCirc::
   GccEnt_QualifiedCirc (const gp_Circ2d&      Qualified,
			 const GccEnt_Position Qualifier) {

   TheQualified = Qualified;
   TheQualifier = Qualifier;
 }
