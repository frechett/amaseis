//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         filter.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for Filter (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(filter_h)              // Sentry, use file only if it's not already included.
#define filter_h
#if 0
//{{TDialog = Filter}}
class Filter : public TDialog {
  public:
    Filter(TWindow* parent, float cutoff_1, float cutoff_2, TResId resId = IDD_FILTER, TModule* module = 0);
    virtual ~Filter();
    float GetCutoff_1() {return fcutoff_1;}
    float GetCutoff_2() {return fcutoff_2;}
    void SetupWindow();
    void Handle_Ok();
    void Handle_Help();

  protected:
  	TEdit *cutoff_1;
  	TEdit *cutoff_2;
    float fcutoff_1;
    float fcutoff_2;
 DECLARE_RESPONSE_TABLE(Filter);
};
#endif

 // Dialog for getting filter parameters
class Filter : public TDialog {
  public:
  	// Input:
    // period: in seconds
    // enable: whether to use the filter
    Filter(TWindow* parent, float &period, bool &enable, const char *caption,
    	TResId resId = IDD_FILTER_LOW_PASS, TModule* module = 0);
    virtual ~Filter();
    void SetupWindow();
    void Handle_Ok();
    void Handle_Help();
    void CmSec();
    void CmHz();

  protected:
  	TEdit *Edit_Period;
    TEdit *Edit_Frequency;
    TRadioButton *sec;
    TRadioButton *hz;
    TCheckBox *enable_cb;
    const char *caption;
    float period;
    float *pperiod;
    bool enable;
    bool *penable;
    void Process();
    void PostProcess();
    void CmUpdate();
 DECLARE_RESPONSE_TABLE(Filter);
};

 // Bob McClure's long period filter
class FilterLP : public TDialog {
  public:
  	// Input:
    // Pp: Sensor period
    // Qp: Sensor Q
    // Pf: long filter period
    FilterLP(TWindow* parent,	TResId resId = IDD_FILTER_LP, TModule* module = 0);
    virtual ~FilterLP();
    void SetupWindow();
    void Handle_Ok();
    void Handle_Help();

  protected:
  	TEdit *Edit_Pp;
  	TEdit *Edit_Qp;
    TEdit *Edit_Pf;
 DECLARE_RESPONSE_TABLE(FilterLP);
};    //{{Filter}}

 // Bob McClure's Low Passfilter
class FilterLowPass : public TDialog {
  public:
  	// Input:
    // PeriodFc:	Cutoff period
    FilterLowPass(TWindow* parent, 	TResId resId = IDD_FILTER_LOW_PASS, TModule* module = 0);
    virtual ~FilterLowPass();
    void SetupWindow();
    void Handle_Ok();
    void Handle_Help();
    void CmSec();
    void CmHz();

  protected:
  	TEdit *Edit_Period;
    TEdit *Edit_Frequency;
    TRadioButton *sec;
    TRadioButton *hz;
    TCheckBox *enable;
    void Process();
    void PostProcess();
    void CmUpdate();
 DECLARE_RESPONSE_TABLE(FilterLowPass);
};

  // Bob McClure's High Pass
class FilterHighPass : public TDialog {
  public:
  	// Input:
    // PeriodFc:	Cutoff period
    FilterHighPass(TWindow* parent, 	TResId resId = IDD_FILTER_HIGH_PASS, TModule* module = 0);
    virtual ~FilterHighPass();
    void SetupWindow();
    void Handle_Ok();
    void Handle_Help();

  protected:
  	TEdit *Edit_Period;
 DECLARE_RESPONSE_TABLE(FilterHighPass);

 };    //{{Filter}}


#endif  // filter_h sentry.

