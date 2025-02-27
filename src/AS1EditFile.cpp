//----------------------------------------------------------------------------
//  Project AS1
//  RunTime Software
//  Copyright � 2000. All Rights Reserved.
//
//  SUBSYSTEM:    AS1 Application
//  FILE:         as1editfile.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TAS1EditFile (TEditFile).
//
//----------------------------------------------------------------------------

#include "as1.h"

//{{TAS1EditFile Implementation}}


//
// Build a response table for all messages/commands handled
// by TAS1EditFile derived from TEditFile.
//
DEFINE_RESPONSE_TABLE1(TAS1EditFile, TEditFile)
//{{TAS1EditFileRSP_TBL_BEGIN}}
  EV_WM_GETMINMAXINFO,
//{{TAS1EditFileRSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TAS1EditFile
// ~~~~~~~~~~
// Construction/Destruction handling.
//
TAS1EditFile::TAS1EditFile(TWindow* parent, int id, const char far* text, int x, int y, int w, int h, const char far* fileName, TModule* module)
:
  TEditFile(parent, id, text, x, y, w, h, fileName, module)
{
  // INSERT>> Your constructor code here.

}


TAS1EditFile::~TAS1EditFile()
{
  Destroy();

  // INSERT>> Your destructor code here.

}


void TAS1EditFile::SetupWindow()
{
  TEditFile::SetupWindow();

  TAS1* theApp = TYPESAFE_DOWNCAST(GetApplication(), TAS1);
  FileData = theApp->FileData;
}


//
// Paint routine for Window, Printer, and PrintPreview for an TEdit client.
//
void TAS1EditFile::Paint(TDC& dc, bool, TRect& rect)
{
  TAS1* theApp = TYPESAFE_DOWNCAST(GetApplication(), TAS1);
  if (theApp) {
    // Only paint if we're printing and we have something to paint, otherwise do nothing.
    //
    if (theApp->Printing && theApp->Printer && !rect.IsEmpty()) {
      // Use pageSize to get the size of the window to render into.  For a Window it's the client area,
      // for a printer it's the printer DC dimensions and for print preview it's the layout window.
      //
      TSize   pageSize(rect.right - rect.left, rect.bottom - rect.top);

      HFONT   hFont = (HFONT)GetWindowFont();
      TFont   font("Arial", -12);
      if (!hFont)
        dc.SelectObject(font);
      else
        dc.SelectObject(TFont(hFont));

      TEXTMETRIC  tm;
      int fHeight = dc.GetTextMetrics(tm) ? tm.tmHeight + tm.tmExternalLeading : 10;

      // How many lines of this font can we fit on a page.
      //
      int linesPerPage = MulDiv(pageSize.cy, 1, fHeight);
      if (linesPerPage) {        TPrintDialog::TData& printerData = theApp->Printer->GetSetup();

        int maxPg = ((GetNumLines() / linesPerPage) + 1.0);

        // Compute the number of pages to print.
        //
        printerData.MinPage = 1;
        printerData.MaxPage = maxPg;

        // Do the text stuff:
        //
        int   fromPage = printerData.FromPage == -1 ? 1 : printerData.FromPage;
        int   toPage = printerData.ToPage == -1 ? 1 : printerData.ToPage;
        int   currentPage = fromPage;
        TAPointer<char> buffer = new char[255];

        while (currentPage <= toPage) {
          int startLine = (currentPage - 1) * linesPerPage;
          int lineIdx = 0;
          while (lineIdx < linesPerPage) {
            // If the string is no longer valid then there's nothing more to display.
            //
            if (!GetLine(buffer, 255, startLine + lineIdx))
              break;
            dc.TabbedTextOut(TPoint(0, lineIdx * fHeight), buffer, strlen(buffer), 0, 0, 0);
            lineIdx++;
          }
          currentPage++;
        }
      }
    }
  }
}


void TAS1EditFile::EvGetMinMaxInfo(MINMAXINFO far& minmaxinfo)
{
  TAS1* theApp = TYPESAFE_DOWNCAST(GetApplication(), TAS1);
  if (theApp) {
    if (theApp->Printing) {
      minmaxinfo.ptMaxSize = TPoint(32000, 32000);
      minmaxinfo.ptMaxTrackSize = TPoint(32000, 32000);
      return;
    }
  }
  TEditFile::EvGetMinMaxInfo(minmaxinfo);
}


// Override SaveAs to store the menu choice.
//
bool TAS1EditFile::SaveAs()
{
  if (TEditFile::SaveAs()) {
    TAS1* theApp = TYPESAFE_DOWNCAST(GetApplication(), TAS1);
    theApp->SaveMenuChoice(GetFileName());
    return true;
  }
  return false;
}
