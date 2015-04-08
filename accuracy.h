//---------------------------------------------------------------------------
#ifndef accuracyH
#define accuracyH
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
class TaccuracyForm : public TForm
{
__published:	// IDE-managed Components
        TChart *Chart1;
        TLineSeries *Series1;
        TLineSeries *Series2;
        TChart *Chart2;
        TLineSeries *Series3;
        TLineSeries *Series4;
        TChart *Chart3;
        TLineSeries *Series5;
        TChart *Chart4;
        TLineSeries *Series6;
        TChart *Chart5;
        TLineSeries *Series7;
        TChart *Chart6;
        TLineSeries *Series8;
        TLineSeries *Series9;
        TLineSeries *Series10;
        TLineSeries *Series11;
        TLineSeries *Series12;
        TEdit *fileEdit;
        TLabel *Label1;
        TEdit *pointEdit;
        TLabel *Label2;
        TRadioGroup *orderRadioGroup;
        TEdit *stepEdit;
        TLabel *Label3;
        TEdit *stepFileEdit;
        TLabel *Label4;
private:	// User declarations
public:		// User declarations
        __fastcall TaccuracyForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TaccuracyForm *accuracyForm;
//---------------------------------------------------------------------------
#endif
