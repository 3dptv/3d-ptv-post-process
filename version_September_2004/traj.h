//---------------------------------------------------------------------------
#ifndef trajH
#define trajH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Chart.hpp>
#include <ExtCtrls.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
//---------------------------------------------------------------------------
class TtrajForm : public TForm
{
__published:	// IDE-managed Components
        TEdit *startFileEdit;
        TLabel *Label1;
        TEdit *startPointEdit;
        TLabel *Label2;
        TChart *Chart1;
        TChart *Chart2;
        TChart *Chart3;
        TLineSeries *Series1;
        TLineSeries *Series2;
        TLineSeries *Series3;
        TLineSeries *Series5;
        TLineSeries *Series6;
        TLineSeries *Series7;
        TLineSeries *Series8;
        TLineSeries *Series9;
        TLineSeries *Series10;
        TLineSeries *Series11;
        TLineSeries *Series12;
        TChart *Chart4;
        TPointSeries *Series16;
        TPointSeries *Series17;
        TPointSeries *Series18;
        TChart *Chart5;
        TPointSeries *Series19;
        TPointSeries *Series20;
        TPointSeries *Series21;
        TEdit *polyOrderEdit;
        TLabel *Label9;
        TLineSeries *Series4;
        TLineSeries *Series13;
        TLineSeries *Series14;
        TLineSeries *Series15;
        TLineSeries *Series28;
        TLineSeries *Series29;
        TLineSeries *Series30;
        TLineSeries *Series31;
        TLineSeries *Series32;
        TLineSeries *Series33;
        TButton *Button1;
        TCheckBox *changedFlag;
        TCheckBox *polyCheck;
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall startFileEditChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TtrajForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TtrajForm *trajForm;
//---------------------------------------------------------------------------
#endif
