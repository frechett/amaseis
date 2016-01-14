// class TWindowPrintout
// ~~~~~ ~~~~~~~~~~~~~~~
class TWindowPrintout : public TPrintout {
  public:
    TWindowPrintout(const char* title, TWindow* window);

    void GetDialogInfo(int& minPage, int& maxPage,
                       int& selFromPage, int& selToPage);
    void PrintPage(int page, TRect& rect, unsigned flags);
    void SetBanding(bool b)
    {
      Banding = b;
    }
    bool HasPage(int pageNumber)
    {
      return pageNumber == 1;
    }

  protected:
    TWindow* Window;
    bool     Scale;
};
