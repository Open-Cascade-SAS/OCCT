//File Geom2dGcc_QualifiedCurve.cxx, REG 19/08/91

#include <Geom2dGcc_QualifiedCurve.ixx>

Geom2dGcc_QualifiedCurve::
   Geom2dGcc_QualifiedCurve (const Geom2dAdaptor_Curve& Curve    ,
			     const GccEnt_Position      Qualifier) {
   TheQualified = Curve;
   TheQualifier = Qualifier;
 }

Geom2dAdaptor_Curve Geom2dGcc_QualifiedCurve::
   Qualified () const { return TheQualified; }

GccEnt_Position Geom2dGcc_QualifiedCurve::
   Qualifier () const { return TheQualifier; }

Standard_Boolean Geom2dGcc_QualifiedCurve::
   IsUnqualified () const {
     if (TheQualifier == GccEnt_unqualified ) { return Standard_True; }
     else { return Standard_False; }
   }

Standard_Boolean Geom2dGcc_QualifiedCurve::
   IsEnclosing () const {
     if (TheQualifier == GccEnt_enclosing) { return Standard_True; }
     else { return Standard_False; }
   }

Standard_Boolean Geom2dGcc_QualifiedCurve::
   IsEnclosed () const {
     if (TheQualifier == GccEnt_enclosed) { return Standard_True; }
     else { return Standard_False; }
   }

Standard_Boolean Geom2dGcc_QualifiedCurve::
   IsOutside () const {
     if (TheQualifier == GccEnt_outside) { return Standard_True; }
     else { return Standard_False; }
   }


