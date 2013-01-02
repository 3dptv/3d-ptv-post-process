//---------------------------------------------------------------------------
#ifndef structReprH
#define structReprH
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
class TstructReprForm : public TForm
{
__published:	// IDE-managed Components
        TChart *Chart1;
        TLineSeries *Series1;
        TLineSeries *Series2;
        TLineSeries *Series3;
        TLineSeries *Series4;
        TEdit *KolConstEdit;
        TLabel *Label1;
        TEdit *fileNumEdit;
        TLabel *Label2;
        TChart *Chart2;
        TLineSeries *Series5;
        TLineSeries *Series6;
        TChart *Chart3;
        TLineSeries *Series7;
        TLineSeries *Series8;
        TChart *Chart4;
        TLineSeries *Series9;
        TLineSeries *Series10;
        TChart *Chart5;
        TLineSeries *Series11;
        TLineSeries *Series12;
        TLineSeries *Series13;
        void __fastcall fitOneBtnClick(TObject *Sender);
        void __fastcall fitTwoBtnClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TstructReprForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TstructReprForm *structReprForm;
//---------------------------------------------------------------------------
#endif
