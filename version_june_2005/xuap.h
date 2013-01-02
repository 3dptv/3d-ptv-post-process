//---------------------------------------------------------------------------
#ifndef xuapH
#define xuapH
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
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TxuapForm : public TForm
{
__published:	// IDE-managed Components
        TChart *Chart1;
        TLineSeries *Series1;
        TLineSeries *Series2;
        TLineSeries *Series3;
        TButton *closeBtn;
        TEdit *startFileEdit;
        TLabel *Label1;
        TEdit *startPointEdit;
        TLabel *Label2;
        TChart *Chart2;
        TChart *Chart3;
        TLineSeries *Series4;
        TLineSeries *Series5;
        TLineSeries *Series6;
        TLineSeries *Series7;
        TLineSeries *Series8;
        TLineSeries *Series9;
        TLineSeries *Series10;
        TLineSeries *Series11;
        TLineSeries *Series12;
        TUpDown *UpDown1;
        TEdit *minEdit;
        TLabel *Label3;
        TLabel *Label4;
        TEdit *maxEdit;
        TEdit *avXEdit;
        TEdit *avYEdit;
        TEdit *avZEdit;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label9;
        TEdit *maxAccEdit;
        TLabel *Label10;
        TEdit *minAccEdit;
        TLabel *Label11;
        void __fastcall closeBtnClick(TObject *Sender);
        void __fastcall UpDown1Changing(TObject *Sender,
          bool &AllowChange);
private:	// User declarations
public:		// User declarations
        __fastcall TxuapForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TxuapForm *xuapForm;
//---------------------------------------------------------------------------
#endif
