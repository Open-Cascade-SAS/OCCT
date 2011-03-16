// File GccEnt_QualifiedLin.cxx, REG 02/08/91

#include <GccEnt_QualifiedLin.ixx>

gp_Lin2d GccEnt_QualifiedLin::
   Qualified () const { return TheQualified; }

GccEnt_Position GccEnt_QualifiedLin::
   Qualifier () const { return TheQualifier; }

Standard_Boolean GccEnt_QualifiedLin::
   IsUnqualified () const {
   if (TheQualifier == GccEnt_unqualified) { return Standard_True; }
   else { return Standard_False; }
 }

Standard_Boolean GccEnt_QualifiedLin::
   IsEnclosed () const {
   return (TheQualifier == GccEnt_enclosed);
 }


Standard_Boolean GccEnt_QualifiedLin::
   IsOutside () const {
   if (TheQualifier == GccEnt_outside) { return Standard_True; }
   else { return Standard_False; }
 }

GccEnt_QualifiedLin::
   GccEnt_QualifiedLin (const gp_Lin2d&       Qualified,
			const GccEnt_Position Qualifier) {
   TheQualified = Qualified;
   TheQualifier = Qualifier;
 }


