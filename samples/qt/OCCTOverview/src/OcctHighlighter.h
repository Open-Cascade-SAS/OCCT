// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#ifndef OCCTHIGHLIGHTER_H
#define OCCTHIGHLIGHTER_H

#include <Standard_Macro.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QRegExp>
#include <QSyntaxHighlighter>
#include <QString>
#include <QTextDocument>
#include <QTextCharFormat>
#include <Standard_WarningsRestore.hxx>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

//! Implements C++ and OCCT objects syntax
//! highlighting for sample code window
class OcctHighlighter: public QSyntaxHighlighter
{
  Q_OBJECT
public:

  OcctHighlighter(QTextDocument* theParent = 0);

protected:
  void highlightBlock(const QString& theText) Standard_OVERRIDE;

private:
  struct HighlightingRule
  {
    QRegExp myPattern;
    QTextCharFormat myFormat;
  };

private:
  QVector<HighlightingRule> myHighlightingRules;
  // QRegExp (Qt4+) introduced by the patch as alternative to QRegularExpression 
  // (Qt5+) for compatibility reasons. QRegExp will be moved in future Qt6 to 
  // a qt5compat module: QRegExp -> Qt5::QRegExp
  QRegExp myCommentStartExpression;
  QRegExp myCommentEndExpression;

  QTextCharFormat myKeywordFormat;
  QTextCharFormat mySingleLineCommentFormat;
  QTextCharFormat myMultiLineCommentFormat;
  QTextCharFormat myQuotationFormat;
  QTextCharFormat myFunctionFormat;
  QTextCharFormat myOcctFormat;
  QTextCharFormat myMemberFormat;
  QTextCharFormat myLocalFormat;
  QTextCharFormat myHelperFormat;
};

#endif
