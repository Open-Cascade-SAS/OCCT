/* 
/* Copyright (c) 1997-1999 Matra Datavision
 Copyright (c) 1999-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
*/ 

%{
#include <ExprIntrp_yaccintrf.hxx>

extern void ExprIntrp_EndOfFuncDef();
extern void ExprIntrp_EndOfRelation();
extern void ExprIntrp_AssignVariable();
extern void ExprIntrp_EndOfAssign();
extern void ExprIntrp_Deassign();
extern void ExprIntrp_SumOperator();
extern void ExprIntrp_MinusOperator();
extern void ExprIntrp_ProductOperator();
extern void ExprIntrp_DivideOperator();
extern void ExprIntrp_ExpOperator();
extern void ExprIntrp_UnaryMinusOperator();
extern void ExprIntrp_UnaryPlusOperator();
extern void ExprIntrp_VariableIdentifier();
extern void ExprIntrp_NumValue();
extern void ExprIntrp_EndFunction();
extern void ExprIntrp_EndDerFunction();
extern void ExprIntrp_EndDifferential();
extern void ExprIntrp_EndDiffFunction();
extern void ExprIntrp_EndFuncArg();
extern void ExprIntrp_NextFuncArg();
extern void ExprIntrp_StartFunction();
extern void ExprIntrp_DefineFunction();
extern void ExprIntrp_StartDerivate();
extern void ExprIntrp_EndDerivate();
extern void ExprIntrp_DiffVar();
extern void ExprIntrp_DiffDegree();
extern void ExprIntrp_VerDiffDegree();
extern void ExprIntrp_DiffDegreeVar();
extern void ExprIntrp_StartDifferential();
extern void ExprIntrp_StartFunction();
extern void ExprIntrp_EndFuncArg();
extern void ExprIntrp_NextFuncArg();
extern void ExprIntrp_VariableIdentifier();
extern void ExprIntrp_Derivation();
extern void ExprIntrp_EndDerivation();
extern void ExprIntrp_DerivationValue();
extern void ExprIntrp_ConstantIdentifier();
extern void ExprIntrp_ConstantDefinition();
extern void ExprIntrp_VariableIdentifier();
extern void ExprIntrp_NumValue();
extern void ExprIntrp_Sumator();
extern void ExprIntrp_VariableIdentifier();
extern void ExprIntrp_Productor();
extern void ExprIntrp_EndOfEqual();

// disable MSVC warnings in bison code
#ifdef _MSC_VER
#pragma warning(disable:4131 4244 4127 4702)
#endif

%}

%token SUMOP MINUSOP DIVIDEOP EXPOP MULTOP PARENTHESIS BRACKET ENDPARENTHESIS ENDBRACKET VALUE IDENTIFIER COMMA DIFFERENTIAL DERIVATE DERIVKEY ASSIGNOP DEASSIGNKEY EQUALOP RELSEPARATOR CONSTKEY SUMKEY PRODKEY
%start exprentry
%left SUMOP MINUSOP
%left DIVIDEOP MULTOP
%left EXPOP
%{
%}
%%

exprentry          : GenExpr
                   | Assignment  
                   | Deassignment
                   | FunctionDefinition  {ExprIntrp_EndOfFuncDef();}
                   | RelationList {ExprIntrp_EndOfRelation();}
                   ;

Assignment         : IDENTIFIER {ExprIntrp_AssignVariable();} ASSIGNOP GenExpr {ExprIntrp_EndOfAssign();}
                   ;

Deassignment       : DEASSIGNKEY BRACKET IDENTIFIER {ExprIntrp_Deassign();} ENDBRACKET
                   ;

GenExpr            : GenExpr SUMOP  GenExpr {ExprIntrp_SumOperator();}
                   | GenExpr MINUSOP GenExpr {ExprIntrp_MinusOperator();}
                   | GenExpr MULTOP GenExpr {ExprIntrp_ProductOperator();}
                   | GenExpr DIVIDEOP GenExpr {ExprIntrp_DivideOperator();}
                   | GenExpr EXPOP GenExpr {ExprIntrp_ExpOperator();}
                   | PARENTHESIS GenExpr ENDPARENTHESIS
                   | BRACKET GenExpr ENDBRACKET
                   | MINUSOP GenExpr {ExprIntrp_UnaryMinusOperator();}
                   | SUMOP GenExpr {ExprIntrp_UnaryPlusOperator();}
                   | SingleExpr
                   | Derivation
                   | ConstantDefinition
                   | Sumator
                   | Productor
                   ;

SingleExpr         : Single
                   | Function
                   ;


Single             : IDENTIFIER  {ExprIntrp_VariableIdentifier();}
                   | VALUE  {ExprIntrp_NumValue();}
                   ;

Function           : funcident PARENTHESIS ListGenExpr ENDPARENTHESIS {ExprIntrp_EndFunction();}
                   | DerFunctionId PARENTHESIS ListGenExpr ENDPARENTHESIS {ExprIntrp_EndDerFunction();}
                   | DiffFuncId {ExprIntrp_EndDifferential();} PARENTHESIS ListGenExpr ENDPARENTHESIS {ExprIntrp_EndDiffFunction();}
                   ;

ListGenExpr        : GenExpr {ExprIntrp_EndFuncArg();} 
                   | GenExpr COMMA {ExprIntrp_NextFuncArg();} ListGenExpr 
                   ;

funcident          : IDENTIFIER  {ExprIntrp_StartFunction();}
                   ;

FunctionDefinition : FunctionDef {ExprIntrp_DefineFunction();} ASSIGNOP GenExpr
                   ;

DerFunctionId      : IDENTIFIER {ExprIntrp_StartDerivate();} DERIVATE {ExprIntrp_EndDerivate();}
                   ;

DiffFuncId         : DIFFERENTIAL DiffId DIVIDEOP DIFFERENTIAL IDENTIFIER {ExprIntrp_DiffVar();}
                   | DIFFERENTIAL VALUE {ExprIntrp_DiffDegree();} DiffId DIVIDEOP DIFFERENTIAL VALUE {ExprIntrp_VerDiffDegree();} IDENTIFIER {ExprIntrp_DiffDegreeVar();}
                   ;

DiffId             : IDENTIFIER {ExprIntrp_StartDifferential();}
                   | DiffFuncId
                   ;

FunctionDef        : IDENTIFIER {ExprIntrp_StartFunction();} BRACKET ListArg ENDBRACKET
                   ;

ListArg            : unarg {ExprIntrp_EndFuncArg();}
                   | unarg COMMA {ExprIntrp_NextFuncArg();} ListArg
                   ;

unarg              : IDENTIFIER {ExprIntrp_VariableIdentifier();}
                   ;

Derivation         : DERIVKEY BRACKET GenExpr COMMA IDENTIFIER {ExprIntrp_Derivation();} ENDBRACKET {ExprIntrp_EndDerivation();}
                   | DERIVKEY BRACKET GenExpr COMMA IDENTIFIER {ExprIntrp_Derivation();} COMMA VALUE {ExprIntrp_DerivationValue();} ENDBRACKET {ExprIntrp_EndDerivation();}
                   ;

ConstantDefinition : CONSTKEY BRACKET IDENTIFIER {ExprIntrp_ConstantIdentifier();} COMMA VALUE {ExprIntrp_ConstantDefinition();} ENDBRACKET
                   ;

Sumator            : SUMKEY BRACKET GenExpr COMMA IDENTIFIER {ExprIntrp_VariableIdentifier();} COMMA GenExpr COMMA GenExpr COMMA VALUE {ExprIntrp_NumValue();} ENDBRACKET {ExprIntrp_Sumator();}
                   ;

Productor          : PRODKEY BRACKET GenExpr COMMA IDENTIFIER {ExprIntrp_VariableIdentifier();} COMMA GenExpr COMMA GenExpr COMMA VALUE {ExprIntrp_NumValue();} ENDBRACKET {ExprIntrp_Productor();}
                   ;

RelationList       : SingleRelation 
                   | SingleRelation RELSEPARATOR RelationList
                   | SingleRelation '\n' RelationList
                   ;

SingleRelation     : GenExpr EQUALOP GenExpr {ExprIntrp_EndOfEqual();}
                   ;

