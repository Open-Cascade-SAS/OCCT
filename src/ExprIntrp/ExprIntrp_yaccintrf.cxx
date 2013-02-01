// Copyright (c) 1997-1999 Matra Datavision
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <ExprIntrp_yaccintrf.hxx>
#include <ExprIntrp_yaccanal.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_NamedExpression.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_NamedConstant.hxx>
#include <Expr_NamedFunction.hxx>
#include <Expr_NumericValue.hxx>
#include <Expr_UnaryFunction.hxx>
#include <Expr_BinaryFunction.hxx>
#include <Expr_PolyFunction.hxx>
#include <Expr_Exponentiate.hxx>
#include <Expr_Absolute.hxx>
#include <Expr_ArcCosine.hxx>
#include <Expr_ArcSine.hxx>
#include <Expr_ArcTangent.hxx>
#include <Expr_ArgCosh.hxx>
#include <Expr_ArgSinh.hxx>
#include <Expr_ArgTanh.hxx>
#include <Expr_Cosh.hxx>
#include <Expr_Cosine.hxx>
#include <Expr_Exponential.hxx>
#include <Expr_LogOf10.hxx>
#include <Expr_LogOfe.hxx>
#include <Expr_Sign.hxx>
#include <Expr_Sine.hxx>
#include <Expr_Sinh.hxx>
#include <Expr_SquareRoot.hxx>
#include <Expr_Tangent.hxx>
#include <Expr_Tanh.hxx>
#include <Expr_Equal.hxx>
#include <Expr_SystemRelation.hxx>
#include <Expr_UnknownIterator.hxx>
#include <Expr_FunctionDerivative.hxx>
#include <Expr.hxx>                    // ATTENTION POUR PROTECTION BUG STACK

#include <Expr_SequenceOfGeneralExpression.hxx>
#include <Expr_Operators.hxx>
#include <ExprIntrp_SyntaxError.hxx>
#include <Expr_Array1OfNamedUnknown.hxx>
#include <Expr_Array1OfGeneralExpression.hxx>
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

extern char yysbuf[];
extern char *yysptr;
static char ExprIntrp_assname[30];
TCollection_AsciiString ExprIntrp_funcdefname;
static Standard_Integer ExprIntrp_nbargs;
static Standard_Integer ExprIntrp_nbdiff;


extern "C" void ExprIntrp_StartFunction()
{
  int nbcar;
  char funcname[100];
  nbcar = ExprIntrp_GetResult(funcname);
  TCollection_AsciiString name(funcname);
  ExprIntrp_Recept.PushName(name);
  ExprIntrp_nbargs = 0;
}

extern "C" void ExprIntrp_StartDerivate()
{
  int nbcar;
  char funcname[100];
  nbcar = ExprIntrp_GetResult(funcname);
  TCollection_AsciiString name(funcname);
  ExprIntrp_Recept.PushName(name);
}

extern "C" void ExprIntrp_EndDerivate()
{
  int degree;
  degree = ExprIntrp_GetDegree();
  ExprIntrp_Recept.PushValue(degree);
}

extern "C" void ExprIntrp_Derivation()
{
  ExprIntrp_Recept.PushValue(1);
  char name[100];
  ExprIntrp_GetResult(name);
  TCollection_AsciiString thename(name);
  Handle(Expr_NamedExpression) namexp = ExprIntrp_Recept.GetNamed(thename);
  if (namexp.IsNull()) {
    namexp = new Expr_NamedUnknown(thename);
  }
  if (!namexp->IsKind(STANDARD_TYPE(Expr_NamedUnknown))) {
    ExprIntrp_SyntaxError::Raise();
  }
  ExprIntrp_Recept.Push(namexp);
}

extern "C" void ExprIntrp_DerivationValue()
{
  char num[30];
  int nbcar;
  nbcar = ExprIntrp_GetResult(num);
  Standard_Integer degree = ExprIntrp_Recept.PopValue();
  degree = atoi(num);
  ExprIntrp_Recept.PushValue(degree);
}

extern "C" void ExprIntrp_EndDerivation()
{
  Standard_Integer degree = ExprIntrp_Recept.PopValue();
  Handle(Expr_NamedUnknown) var = Handle(Expr_NamedUnknown)::DownCast(ExprIntrp_Recept.Pop());
  Handle(Expr_GeneralExpression) exp = ExprIntrp_Recept.Pop();
  exp = exp->NDerivative(var,degree);
  ExprIntrp_Recept.Push(exp);
}

extern "C" void ExprIntrp_StartDifferential()
{
  ExprIntrp_StartDerivate();
  ExprIntrp_nbdiff = 0;
}

extern "C" void ExprIntrp_DiffDegreeVar()
{
  char name[100];
  ExprIntrp_GetResult(name);
#ifndef WNT 
  if (strncasecmp(name,"X",1)) {
#else
  if (  name[ 0 ] != 'X' && name[ 0 ] != 'x'  ) {
#endif  // WNT
    ExprIntrp_SyntaxError::Raise();
  }
  char* s = name;
  s++;
  Standard_Integer rank = atoi(s);
  ExprIntrp_Recept.PushValue(rank);
  ExprIntrp_nbdiff++;
}

extern "C" void ExprIntrp_DiffVar()
{
  ExprIntrp_Recept.PushValue(1);
  ExprIntrp_DiffDegreeVar();
}

extern "C" void ExprIntrp_DiffDegree()
{
  char name[100];
  ExprIntrp_GetResult(name);
  Standard_Integer deg = atoi(name);
  ExprIntrp_Recept.PushValue(deg);
}

extern "C" void ExprIntrp_VerDiffDegree()
{
  char name[100];
  ExprIntrp_GetResult(name);
  Standard_Integer deg = atoi(name);
  Standard_Integer thedeg = ExprIntrp_Recept.PopValue();
  if (deg != thedeg) {
    ExprIntrp_SyntaxError::Raise();
  }
  ExprIntrp_Recept.PushValue(deg);
}

extern "C" void ExprIntrp_EndDifferential()
{
  TCollection_AsciiString name = ExprIntrp_Recept.PopName();
  Handle(Expr_GeneralFunction) thefunc = ExprIntrp_Recept.GetFunction(name);
  if (thefunc.IsNull()) {
    ExprIntrp_SyntaxError::Raise();
  }
  Standard_Integer rank,degree;
  Handle(Expr_NamedUnknown) thediff;
  Standard_Integer nbvars = thefunc->NbOfVariables();

  for (Standard_Integer i=1; i<= ExprIntrp_nbdiff; i++) {
    rank = ExprIntrp_Recept.PopValue();
    degree = ExprIntrp_Recept.PopValue();
    if ((rank > nbvars) || (rank < 1)) {
      ExprIntrp_SyntaxError::Raise();
    }
    thediff = thefunc->Variable(rank);
    thefunc = new Expr_FunctionDerivative(thefunc,thediff,degree);
  }
  ExprIntrp_Recept.PushFunction(thefunc);
}

extern "C" void ExprIntrp_EndDiffFunction()
{
  Handle(Expr_GeneralFunction) thefunc = ExprIntrp_Recept.PopFunction();
  if (thefunc.IsNull()) {
    ExprIntrp_SyntaxError::Raise();
  }
  Standard_Integer nbargs = thefunc->NbOfVariables();
  if (nbargs == 1) {
    Handle(Expr_GeneralExpression) op = ExprIntrp_Recept.Pop();
    Handle(Expr_UnaryFunction) res = 
      new Expr_UnaryFunction(thefunc,op);
    ExprIntrp_Recept.Push(res);
  }
  else if (nbargs == 2) {
    Handle(Expr_GeneralExpression) arg2 = ExprIntrp_Recept.Pop();
    Handle(Expr_GeneralExpression) arg1 = ExprIntrp_Recept.Pop();
    if (arg1.IsNull()) {
      ExprIntrp_SyntaxError::Raise();
    }
    Handle(Expr_BinaryFunction) res =
      new Expr_BinaryFunction(thefunc,arg1,arg2);
    ExprIntrp_Recept.Push(res);
  }
  else {
    Expr_Array1OfGeneralExpression tabarg(1,nbargs);
    Handle(Expr_GeneralExpression) arg;
    for (Standard_Integer i = 1; i<= nbargs; i++) {
      arg = ExprIntrp_Recept.Pop();
      if (arg.IsNull()) {
	ExprIntrp_SyntaxError::Raise();
      }
      tabarg(nbargs-i+1) = arg;
    }
    Handle(Expr_PolyFunction) res = 
      new Expr_PolyFunction(thefunc,tabarg);
    ExprIntrp_Recept.Push(res);
  }
}
  
static Handle(Expr_GeneralExpression) ExprIntrp_StandardFunction(const TCollection_AsciiString& name, const Handle(Expr_GeneralExpression)& op)
{
// return standard functions equivalent corresponding to <name> 
// with given operand <op> if exists. Returns null value if not.
// <name> is not case sensitive

  Handle(Expr_GeneralExpression) res;
  if ((name == "abs") || (name == "Abs")) {
    res = new Expr_Absolute(op);
  }
  else if ((name == "acos") || (name == "ACos")) {
    res = new Expr_ArcCosine(op);
  }
  else if ((name == "asin") || (name == "ASin")) {
    res = new Expr_ArcSine(op);
  }
  else if ((name == "atan") || (name == "ATan")) {
    res = new Expr_ArcTangent(op);
  }
  else if ((name == "acosh") || (name == "ACosh")) {
    res = new Expr_ArgCosh(op);
  }
  else if ((name == "asinh") || (name == "ASinh")) {
    res = new Expr_ArgSinh(op);
  }
  else if ((name == "atanh") || (name == "ATanh")) {
    res = new Expr_ArgTanh(op);
  }
  else if ((name == "cosh") || (name == "Cosh")) {
    res = new Expr_Cosh(op);
  }
  else if ((name == "cos") || (name == "Cos")) {
    res = new Expr_Cosine(op);
  }
  else if ((name == "exp") || (name == "Exp")) {
    res = new Expr_Exponential(op);
  }
  else if (name == "log") {
    res = new Expr_LogOf10(op);
  }
  else if ((name == "Log") || (name == "Ln")) {
    res = new Expr_LogOfe(op);
  }
  else if ((name == "sign") || (name == "Sign")) {
    res = new Expr_Sign(op);
  }
  else if ((name == "sin") || (name == "Sin")) {
    res = new Expr_Sine(op);
  }
  else if ((name == "sinh") || (name == "Sinh")) {
    res = new Expr_Sinh(op);
  }
  else if ((name == "sqrt") || (name == "Sqrt")) {
    res = new Expr_SquareRoot(op);
  }
  else if ((name == "tan") || (name == "Tan")) {
    res = new Expr_Tangent(op);
  }
  else if ((name == "tanh") || (name == "Tanh")) {
    res = new Expr_Tanh(op);
  }
  return res;
}


extern "C" void ExprIntrp_EndDerFunction()
{
  TCollection_AsciiString name = ExprIntrp_Recept.PopName();
  Handle(Expr_GeneralExpression) op = ExprIntrp_Recept.Pop();
  Handle(Expr_GeneralExpression) resstand = ExprIntrp_StandardFunction(name,op);

  if (!resstand.IsNull()) {
    Handle(Expr_NamedUnknown) var;
    Expr_UnknownIterator rit(resstand);
    while (rit.More()) {
      if (!var.IsNull()) {
	ExprIntrp_SyntaxError::Raise();
      }
      else {
	var = rit.Value();
	if (var->IsAssigned()) {
	  var.Nullify();
	}
      }
      rit.Next();
    }
    if (var.IsNull()) {
      ExprIntrp_SyntaxError::Raise();
    }
    else {
      Handle(Expr_GeneralExpression) res = resstand->NDerivative(var,ExprIntrp_Recept.PopValue());
      ExprIntrp_Recept.Push(res);
    }
  }
  else {
    Handle(Expr_NamedFunction) thefunc = ExprIntrp_Recept.GetFunction(name);
    if (thefunc.IsNull()) {
      ExprIntrp_SyntaxError::Raise();
    }
    Standard_Integer nbargs = thefunc->NbOfVariables();
    if (nbargs != 1) {
      ExprIntrp_SyntaxError::Raise();
    }
    Handle(Expr_NamedUnknown) var = thefunc->Variable(1);
    Handle(Expr_FunctionDerivative) thefuncder = 
      new Expr_FunctionDerivative(thefunc,var,ExprIntrp_Recept.PopValue());
    Handle(Expr_UnaryFunction) res = 
      new Expr_UnaryFunction(thefuncder,op);
    ExprIntrp_Recept.Push(res);
  }
}
    
extern "C" void ExprIntrp_EndFunction()
{
  TCollection_AsciiString name = ExprIntrp_Recept.PopName();
  Handle(Expr_GeneralExpression) op = ExprIntrp_Recept.Pop();

  Handle(Expr_GeneralExpression) resstand = ExprIntrp_StandardFunction(name,op);
  if (!resstand.IsNull()) {
    ExprIntrp_Recept.Push(resstand->ShallowSimplified());
  }
  else {
    Handle(Expr_NamedFunction) thefunc = ExprIntrp_Recept.GetFunction(name);
    if (thefunc.IsNull()) {
      ExprIntrp_SyntaxError::Raise();
      }
    Standard_Integer nbargs = thefunc->NbOfVariables();
    if (nbargs == 1) {
      Handle(Expr_UnaryFunction) res = 
	new Expr_UnaryFunction(thefunc,op);
      ExprIntrp_Recept.Push(res);
    }
    else if (nbargs == 2) {
      Handle(Expr_GeneralExpression) arg1 = ExprIntrp_Recept.Pop();
      if (arg1.IsNull()) {
	ExprIntrp_SyntaxError::Raise();
      }
      Handle(Expr_BinaryFunction) res =
	new Expr_BinaryFunction(thefunc,arg1,op);
      ExprIntrp_Recept.Push(res);
    }
    else {
      Expr_Array1OfGeneralExpression tabarg(1,nbargs);
      Handle(Expr_GeneralExpression) arg;
      tabarg(nbargs) = op;
      for (Standard_Integer i = 1; i< nbargs; i++) {
	arg = ExprIntrp_Recept.Pop();
	if (arg.IsNull()) {
	  ExprIntrp_SyntaxError::Raise();
	}
	tabarg(nbargs-i) = arg;
      }
      Handle(Expr_PolyFunction) res = 
	new Expr_PolyFunction(thefunc,tabarg);
      ExprIntrp_Recept.Push(res);
    }
  }
}

extern "C" void ExprIntrp_NextFuncArg()
{
  ExprIntrp_nbargs++;
}

extern "C" void ExprIntrp_EndFuncArg()
{
  ExprIntrp_nbargs++;
}

extern "C" void ExprIntrp_SumOperator()
{
  Handle(Expr_GeneralExpression) op2 = ExprIntrp_Recept.Pop();
  Handle(Expr_GeneralExpression) op1 = ExprIntrp_Recept.Pop();
  Handle(Expr_Sum) sres = op1 + op2;
  Handle(Expr_GeneralExpression) res = sres->ShallowSimplified();
  ExprIntrp_Recept.Push(res);
}

extern "C" void ExprIntrp_MinusOperator()
{
  Handle(Expr_GeneralExpression) op2 = ExprIntrp_Recept.Pop();
  Handle(Expr_GeneralExpression) op1 = ExprIntrp_Recept.Pop();
  Handle(Expr_Difference) res = op1 - op2;
  ExprIntrp_Recept.Push(res->ShallowSimplified());
}

extern "C" void ExprIntrp_DivideOperator()
{
  Handle(Expr_GeneralExpression) op2 = ExprIntrp_Recept.Pop();
  Handle(Expr_GeneralExpression) op1 = ExprIntrp_Recept.Pop();
  Handle(Expr_Division) res = op1 / op2;
  ExprIntrp_Recept.Push(res->ShallowSimplified());
}

extern "C" void ExprIntrp_ExpOperator()
{
  Handle(Expr_GeneralExpression) op2 = ExprIntrp_Recept.Pop();
  Handle(Expr_GeneralExpression) op1 = ExprIntrp_Recept.Pop();
  Handle(Expr_Exponentiate) res = new Expr_Exponentiate(op1,op2);
  ExprIntrp_Recept.Push(res->ShallowSimplified());
}

extern "C" void ExprIntrp_ProductOperator()
{
  Handle(Expr_GeneralExpression) op2 = ExprIntrp_Recept.Pop();
  Handle(Expr_GeneralExpression) op1 = ExprIntrp_Recept.Pop();
  Handle(Expr_Product) res = op1 * op2;
  ExprIntrp_Recept.Push(res->ShallowSimplified());
}

extern "C" void ExprIntrp_UnaryMinusOperator()
{
  Handle(Expr_GeneralExpression) op = ExprIntrp_Recept.Pop();
  Handle(Expr_UnaryMinus) res = new Expr_UnaryMinus(op);
  ExprIntrp_Recept.Push(res->ShallowSimplified());
}

extern "C" void ExprIntrp_VariableIdentifier()
{
  char name[30];
  int nbcar;
  nbcar = ExprIntrp_GetResult(name);
  TCollection_AsciiString thename(name);
  Handle(Expr_NamedExpression) nameexp = ExprIntrp_Recept.GetNamed(thename);
  if (nameexp.IsNull()) {
    nameexp = new Expr_NamedUnknown(thename);
    ExprIntrp_Recept.Use(nameexp);
  }
  ExprIntrp_Recept.Push(nameexp);
}

extern "C" void ExprIntrp_NumValue()
{
  char num[30];
  int nbcar;
  nbcar = ExprIntrp_GetResult(num);
  Standard_Real value = Atof(num);
  Handle(Expr_NumericValue) nval = new Expr_NumericValue(value);
  ExprIntrp_Recept.Push(nval);
}

extern "C" void ExprIntrp_AssignVariable()
{
  int nbcar;
  nbcar = ExprIntrp_GetResult(ExprIntrp_assname);
}

extern "C" void ExprIntrp_Deassign()
{
  int nbcar;
  char name[100];
  nbcar = ExprIntrp_GetResult(name);
  TCollection_AsciiString thename(name);
  Handle(Expr_NamedExpression) nameexp = ExprIntrp_Recept.GetNamed(thename);
  if (nameexp.IsNull()) {
    ExprIntrp_SyntaxError::Raise();
  }
  if (!nameexp->IsKind(STANDARD_TYPE(Expr_NamedUnknown))) {
    ExprIntrp_SyntaxError::Raise();
  }
  Handle(Expr_NamedUnknown) var = Handle(Expr_NamedUnknown)::DownCast(nameexp);
  var->Deassign();
}

extern "C" void ExprIntrp_DefineFunction()
{
  ExprIntrp_funcdefname = ExprIntrp_Recept.PopName();
  ExprIntrp_Recept.PushValue(ExprIntrp_nbargs);
}

extern "C" void ExprIntrp_close()
{
  ExprIntrp_stop_string();
}

extern "C" void ExprIntrperror(char* msg)
{
  ExprIntrp_close();
  ExprIntrp_SyntaxError::Raise(msg);
}


extern "C" void ExprIntrp_EndOfEqual()
{
  Handle(Expr_GeneralExpression) memb2 = ExprIntrp_Recept.Pop();
  Handle(Expr_GeneralExpression) memb1 = ExprIntrp_Recept.Pop();
  Handle(Expr_Equal) res = new Expr_Equal(memb1,memb2);
  ExprIntrp_Recept.PushRelation(res);
}

extern "C" void ExprIntrp_EndOfRelation()
{
  Handle(Expr_SystemRelation) sys;
  Handle(Expr_GeneralRelation) currel;
  Handle(Expr_GeneralRelation) oldrel;
  while (!ExprIntrp_Recept.IsRelStackEmpty()) {
    currel = ExprIntrp_Recept.PopRelation();
    if (!sys.IsNull()) {
      sys->Add(currel);
    }
    else if (!oldrel.IsNull()) {
      sys = new Expr_SystemRelation(oldrel);
      sys->Add(currel);
    }
    else {
      oldrel = currel;
    }
  }
  if (sys.IsNull()) {
    ExprIntrp_Recept.PushRelation(currel);
  }
  else {
    ExprIntrp_Recept.PushRelation(sys);
  }
}

extern "C" void ExprIntrp_EndOfAssign()
{
  Handle(Expr_NamedExpression) namexp = ExprIntrp_Recept.GetNamed(ExprIntrp_assname);
  Handle(Expr_NamedUnknown) namu;
  if (namexp.IsNull()) {
    namu = new Expr_NamedUnknown(ExprIntrp_assname);
    ExprIntrp_Recept.Use(namu);
  }
  else {
    if (!namexp->IsKind(STANDARD_TYPE(Expr_NamedUnknown))) {
      ExprIntrp_SyntaxError::Raise();
    }
    namu = Handle(Expr_NamedUnknown)::DownCast(namexp);
  }
  namu->Assign(ExprIntrp_Recept.Pop());
}

extern "C" void ExprIntrp_EndOfFuncDef()
{
  Handle(Expr_GeneralExpression) theexp = ExprIntrp_Recept.Pop();
  Standard_Integer nbargs = ExprIntrp_Recept.PopValue();
  Expr_Array1OfNamedUnknown vars(1,nbargs);
  Expr_Array1OfNamedUnknown internvars(1,nbargs);
  Standard_Integer i;
  for (i=nbargs; i > 0; i--) {
    vars(i) = Handle(Expr_NamedUnknown)::DownCast(ExprIntrp_Recept.Pop());
    internvars(i) = Handle(Expr_NamedUnknown)::DownCast(vars(i)->Copy());
  }
  theexp = Expr::CopyShare(theexp);  // ATTENTION, PROTECTION BUG STACK
  for (i=1; i<= nbargs; i++) {
    if (theexp->Contains(vars(i))) {
      theexp->Replace(vars(i),internvars(i));
    }
    else {
      if (theexp == vars(i)) {
	theexp = internvars(i);
      }
    }
  }
  Handle(Expr_NamedFunction) thefunc = 
    new Expr_NamedFunction(ExprIntrp_funcdefname,
			   theexp,
			   internvars);
  ExprIntrp_Recept.Use(thefunc);
}

extern "C" void ExprIntrp_ConstantIdentifier()
{
  char name[100];
  ExprIntrp_GetResult(name);
  TCollection_AsciiString thename(name);
  ExprIntrp_Recept.PushName(thename);
}

extern "C" void ExprIntrp_ConstantDefinition()
{
  TCollection_AsciiString name = ExprIntrp_Recept.PopName();
  char num[30];
  int nbcar;
  nbcar = ExprIntrp_GetResult(num);
  Standard_Real val = Atof(num);
  Handle(Expr_NamedConstant) theconst = new Expr_NamedConstant(name,val);
  ExprIntrp_Recept.Use(theconst);
  ExprIntrp_Recept.Push(theconst);
}


extern "C" void ExprIntrp_Sumator()
{
  Handle(Expr_NumericValue) number = Handle(Expr_NumericValue)::DownCast(ExprIntrp_Recept.Pop());
  Standard_Integer nb = (Standard_Integer) number->GetValue();
  Handle(Expr_GeneralExpression) inc = ExprIntrp_Recept.Pop();
  Handle(Expr_GeneralExpression) first = ExprIntrp_Recept.Pop();
  Handle(Expr_NamedUnknown) var = Handle(Expr_NamedUnknown)::DownCast(ExprIntrp_Recept.Pop());
  Handle(Expr_GeneralExpression) theexp = ExprIntrp_Recept.Pop();
  Standard_Boolean thesame = (theexp == var);
  Handle(Expr_GeneralExpression) cur = Expr::CopyShare(first);
  Handle(Expr_GeneralExpression) res;
  Handle(Expr_GeneralExpression) member;
  Expr_SequenceOfGeneralExpression seq;
  for (Standard_Integer i=1; i<= nb; i++) {
    if (thesame) {
      member = cur;
    }
    else {
      member = Expr::CopyShare(theexp);
      member->Replace(var,cur);
    }
    seq.Append(member);
    cur = (cur + inc)->ShallowSimplified();
  }
  res = new Expr_Sum(seq);
  ExprIntrp_Recept.Push(res->ShallowSimplified());
}

extern "C" void ExprIntrp_Productor()
{
  Handle(Expr_NumericValue) number = Handle(Expr_NumericValue)::DownCast(ExprIntrp_Recept.Pop());
  Standard_Integer nb = (Standard_Integer) number->GetValue();
  Handle(Expr_GeneralExpression) inc = ExprIntrp_Recept.Pop();
  Handle(Expr_GeneralExpression) first = ExprIntrp_Recept.Pop();
  Handle(Expr_NamedUnknown) var = Handle(Expr_NamedUnknown)::DownCast(ExprIntrp_Recept.Pop());
  Handle(Expr_GeneralExpression) theexp = ExprIntrp_Recept.Pop();
  Standard_Boolean thesame = (theexp == var);
  Handle(Expr_GeneralExpression) cur = Expr::CopyShare(first);
  Handle(Expr_GeneralExpression) res;
  Handle(Expr_GeneralExpression) member;
  Expr_SequenceOfGeneralExpression seq;
  for (Standard_Integer i=1; i<= nb; i++) {
    if (thesame) {
      member = cur;
    }
    else {
      member = Expr::CopyShare(theexp);
      member->Replace(var,cur);
    }
    seq.Append(member);
    cur = (cur + inc)->ShallowSimplified();
  }
  res = new Expr_Product(seq);
  ExprIntrp_Recept.Push(res->ShallowSimplified());
}
