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

#ifndef TRANSLATEDIALOG_H
#define TRANSLATEDIALOG_H

#include <Standard_WarningsDisable.hxx>
#include <QComboBox>
#include <QFileDialog>
#include <QList>
#include <QListView>
#include <QShowEvent>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

//! Qt file dialog for save and restore sample files
class TranslateDialog : public QFileDialog
{
public:
  TranslateDialog(QWidget* = 0, Qt::WindowFlags flags = 0, bool = true);
  ~TranslateDialog();
  int                   getMode() const;
  void                  setMode(const int);
  void                  addMode(const int, const QString&);
  void                  clear();

protected:
  void                  showEvent(QShowEvent* event);

private:
  QListView*            findListView(const QObjectList&);

private:
  QComboBox*            myBox;
  QList<int>            myList;
};

#endif // TRANSLATEDIALOG_H
