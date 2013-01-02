//---------------------------------------------------------------------------
#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Chart.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
//---------------------------------------------------------------------------
class TmainForm : public TForm
{
__published:	// IDE-managed Components
        TButton *doPointsBtn;
        TEdit *fileNumEdit;
        TLabel *Label1;
        TOpenDialog *OpenDialog1;
        TOpenDialog *OpenDialog2;
        TEdit *countEdit;
        TLabel *Label2;
        TEdit *meanVelEdit;
        TLabel *Label3;
        TEdit *meanAccEdit;
        TLabel *Label4;
        TEdit *maxVelEdit;
        TLabel *Label5;
        TButton *doGridBtn;
        TOpenDialog *OpenDialog3;
        TOpenDialog *OpenDialog4;
        TEdit *fileNum2Edit;
        TLabel *Label6;
        TEdit *meanPointsInSphereBEdit;
        TLabel *Label8;
        TEdit *meanDissEdit;
        TLabel *Label10;
        TLabel *Label11;
        TLabel *Label12;
        TLabel *Label13;
        TLabel *Label14;
        TLabel *Label15;
        TEdit *minXEdit;
        TEdit *minYEdit;
        TEdit *minZEdit;
        TEdit *meanSuccessDivEdit;
        TLabel *Label16;
        TEdit *reEdit;
        TLabel *Label17;
        TEdit *succIntTrajEdit;
        TLabel *Label20;
        TEdit *divEdit;
        TLabel *Label21;
        TEdit *accEdit;
        TLabel *Label22;
        TEdit *maxDistEdit;
        TLabel *Label23;
        TEdit *minRemainEdit;
        TLabel *Label24;
        TButton *volumeBtn;
        TOpenDialog *OpenDialog5;
        TOpenDialog *OpenDialog6;
        TEdit *volumeEdit;
        TLabel *Label25;
        TEdit *avCandEdit;
        TLabel *Label26;
        TOpenDialog *OpenDialog7;
        TOpenDialog *OpenDialog8;
        TEdit *surfaceEdit;
        TLabel *Label27;
        TEdit *avCandSurfEdit;
        TLabel *Label28;
        TLabel *Label29;
        TLabel *Label30;
        TEdit *maxDistSurfEdit;
        TEdit *minRemainSurfEdit;
        TButton *surfaceBtn;
        TOpenDialog *OpenDialog9;
        TOpenDialog *OpenDialog10;
        TEdit *lineEdit;
        TLabel *Label31;
        TEdit *avCandLineEdit;
        TLabel *Label32;
        TLabel *Label33;
        TLabel *Label34;
        TEdit *maxDistLineEdit;
        TEdit *minRemainLineEdit;
        TButton *lineBtn;
        TButton *linkBtn;
        TEdit *viscEdit;
        TLabel *Label35;
        TButton *correlBtn;
        TButton *prevXUPBtn;
        TButton *prevTrajBtn;
        TButton *structBtn;
        TButton *accuracyBtn;
        TButton *reprBtn;
        TButton *rotaBtn;
        TEdit *divaEdit;
        TLabel *Label18;
        TLabel *Label37;
        TRadioGroup *spatialRadioGroup;
        TEdit *radiusSpatEdit;
        TEdit *meanSuccessAccEdit;
        TEdit *meanSuccessDivAEdit;
        TLabel *Label7;
        TLabel *Label9;
        TEdit *deltaTEdit;
        TLabel *Label19;
        TEdit *meanPointsInSphereEdit;
        TLabel *Label38;
        TEdit *c1Edit;
        TEdit *c2Edit;
        TLabel *Label39;
        TLabel *Label40;
        TEdit *polyConstEdit;
        TLabel *Label41;
        TEdit *meanNumInTrajEdit;
        TLabel *Label42;
        TEdit *TypicalPolyOrderEdit;
        TLabel *Label43;
        TEdit *meanEffectiveNumInTrajEdit;
        TLabel *Label44;
        TLabel *Label45;
        TEdit *radiusTempEdit;
        TLabel *Label36;
        TRadioGroup *interpolRadioGroup;
        TEdit *tolMaxVelEdit;
        TLabel *Label46;
        TEdit *meanSuccessCurvEdit;
        TEdit *meanSuccessGradKEdit;
        TEdit *curvEdit;
        TEdit *gradKEdit;
        TLabel *Label47;
        TLabel *Label48;
        TLabel *Label49;
        TLabel *Label50;
        TButton *gridBtn;
        TLabel *Label51;
        TLabel *Label52;
        TLabel *Label53;
        TLabel *Label54;
        TLabel *Label55;
        TLabel *Label56;
        TEdit *deltaXEdit;
        TEdit *deltaYEdit;
        TEdit *deltaZEdit;
        TEdit *numXEdit;
        TEdit *numYEdit;
        TEdit *numZEdit;
        TLabel *Label57;
        TButton *filterGridBtn;
        TEdit *filtEdit;
        TLabel *Label58;
        void __fastcall doPointsBtnClick(TObject *Sender);
        void __fastcall doGridBtnClick(TObject *Sender);
        void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall FormClick(TObject *Sender);
        void __fastcall volumeBtnClick(TObject *Sender);
        void __fastcall surfaceBtnClick(TObject *Sender);
        void __fastcall lineBtnClick(TObject *Sender);
        void __fastcall linkBtnClick(TObject *Sender);
        void __fastcall corrBtnClick(TObject *Sender);
        void __fastcall correlBtnClick(TObject *Sender);
        void __fastcall paramBtnClick(TObject *Sender);
        void __fastcall prevXUPBtnClick(TObject *Sender);
        void __fastcall prevTrajBtnClick(TObject *Sender);
        void __fastcall structBtnClick(TObject *Sender);
        void __fastcall accuracyBtnClick(TObject *Sender);
        void __fastcall reprBtnClick(TObject *Sender);
        void __fastcall rotaBtnClick(TObject *Sender);
        void __fastcall gridBtnClick(TObject *Sender);
        void __fastcall filterGridBtnClick(TObject *Sender);

private:	// User declarations
public:		// User declarations
        __fastcall TmainForm(TComponent* Owner);
};
//--------------------------------velocity class----------------------------------
class TpointList : public TObject
{
public:

    int  numPointInGrid;
    double gridMem[50][5000][48];
    double interpolatedGrid[5000][48];
    bool PathSet;

    double Re;
    double meanUSq, meanUisq,meanDudxsq;

    int firstFile;
    int lastFile;
    double deltaT;
    int count;
    int count2;
    int count3;
    int count4;
    int count5;
    int count6;
    int count7;
    int count8;
    double vel;
    double maxVel;
    double meanVel;
    double meanAcc;
    double meanPointsInSphere;
    double meanPointsInSphereB;
    double succIntTraj;
    double meanDiss;
    int forMeanNumInTraj;
    int forEffectiveMeanNumInTraj;
    int forTypicalPolyOrder;
    String baseName;

    double point[200][3000][25];
    int numOfFrames;
    bool changed;

     double u,ux,uy,uz;
     double v,vx,vy,vz;
     double w,wx,wy,wz;
     double uxx,uyy,uzz,uxy,uxz,uyz;
     double vxx,vyy,vzz,vxy,vxz,vyz;
     double wxx,wyy,wzz,wxy,wxz,wyz;
     double uxxx,uyyy,uzzz,uxxy,uxxz,uyyx,uyyz,uzzx,uzzy,uxyz;
     double vxxx,vyyy,vzzz,vxxy,vxxz,vyyx,vyyz,vzzx,vzzy,vxyz;
     double wxxx,wyyy,wzzz,wxxy,wxxz,wyyx,wyyz,wzzx,wzzy,wxyz;

    double meaU[2];
    double meaV[2];
    double meaW[2];
    double meaAx[2];
    double meaAy[2];
    double meaAz[2];

    int arraySize;
    int maxRowIndex;
    double m0  [300][20];
    double m0T [20][300];
    double m0T0[20][20];
    double A  [300][20];
    double AT [20][300];
    double ATA[20][20];
    double B  [300][20];
    double BT [20][300];
    double BTB[20][20];
    double C[300][20];
    double CT [20][300];
    double CTC[20][20];
    double D[300][20];
    double DT [20][300];
    double DTD[20][20];
    double Y  [300];
    double Yu  [300];
    double Yv  [300];
    double Yw  [300];
    double Yu0  [300];
    double Yv0  [300];
    double Yw0  [300];
    double YuA  [300];
    double YvA  [300];
    double YwA  [300];
    double YuC  [300];
    double YvC  [300];
    double YwC  [300];
    double YuD  [300];
    double YvD  [300];
    double YwD  [300];
    double YuB  [300];
    double YvB  [300];
    double YwB  [300];
    double Yax  [300];
    double Yay  [300];
    double Yaz  [300];
    double y0  [300];
    double y1  [300];
    double y2  [300];
    double y3  [300];
    double y4  [300];
    double y5  [300];
    double y6  [300];
    double y7  [300];
    double y8  [300];
    double y9  [300];
    double y10  [300];
    double y11  [300];
    double y12  [300];
    double y13  [300];
    double y14  [300];
    double y15  [300];
    double y16  [300];
    double y17  [300];
    double y18  [300];
    double y19  [300];
    double y20  [300];
    double y21  [300];
    double y22  [300];
    double y23  [300];
    double y24  [300];
    double y25  [300];
    double y26  [300];
    double y27  [300];
    double y28  [300];
    double y29  [300];
    double y30  [300];
    double y31  [300];
    double y32  [300];
    double y33  [300];
    double y34  [300];
    double y35  [300];
    double y36  [300];
    double y37  [300];
    double y38  [300];
    double y39  [300];
    double y40  [300];
    double y41  [300];

    double X  [20];
    double RES [21];
    double m0TY[300];
    double ATY[300];
    double CTY[300];
    double DTY[300];
    double CY[300];
    double Resid[300];
    double BTY[300];
    

    double cov[300][2];
    double valid[30][3];
    double var[300][2];
    double covx[300][2];
    double varx[300][2];
    double covy[300][2];
    double vary[300][2];
    double covz[300][2];
    double varz[300][2];
    double cor[300];
    double corx[300];
    double cory[300];
    double corz[300];

    double Acov[300][2];
    double Avar[300][2];
    double Acovx[300][2];
    double Avarx[300][2];
    double Acovy[300][2];
    double Avary[300][2];
    double Acovz[300][2];
    double Avarz[300][2];
    double Acor[300];
    double Acorx[300];
    double Acory[300];
    double Acorz[300];

    
    int minDistArray[200][100];

    double pointPerRadius[100][2]; //0.1mm resolution up to 10mm.
    int dis[300];
    int dis0[300];
    int disA[300];
    int disB[300];
    int disC[300];
    int disD[300];
    bool kriging;
    bool occ[6000][2000];
    double traj[300][48];
    double trajec[51][15000][32];
    double we[300];
    int minTrajLength;
    int numInTraj;
    int noDeriv;
    double c1;
    double c2;

    double indexCounterNo[200];
    double kFluctArrayNo[200];
    double kResidArrayNo[200];
    double DuErrorArrayNo[200];
    double DvErrorArrayNo[200];
    double DwErrorArrayNo[200];
    double kDistArrayNo[200];
    double indexCounterDi[200];
    double kFluctArrayDi[200];
    double kResidArrayDi[200];
    double DuErrorArrayDi[200];
    double DvErrorArrayDi[200];
    double DwErrorArrayDi[200];
    double kNoArrayDi[200];

    double minX,minY,minZ,maxX,maxY,maxZ,deltaX,deltaY,deltaZ;

    void __fastcall setPathAndFiles();
    void __fastcall readPTVFile(int,int);
    void __fastcall readAddedFile(int,int);
    void __fastcall doCubicSplines();
    void __fastcall doCubicSplinesTwenty(bool,int);
    void __fastcall make0T(int,int);
    void __fastcall make0T0(int,int);
    void __fastcall make0TY(int,int,int);
    bool __fastcall solve0(int,int);
    void __fastcall makeAT(int,int);
    void __fastcall makeATA(int,int);
    void __fastcall makeATY(int,int,int);
    void __fastcall makeDT(int,int);
    void __fastcall makeDTD(int,int);
    void __fastcall makeDTY(int,int,int);
    bool __fastcall solve(int,int);
    void __fastcall makeBT(int,int);
    double __fastcall makeDuError(int);
    double __fastcall makeDvError(int);
    double __fastcall makeDwError(int);
    double __fastcall makekResidError(int);
    double __fastcall makeDuErrorQuad(int);
    double __fastcall makeDvErrorQuad(int);
    double __fastcall makeDwErrorQuad(int);
    double __fastcall makekResidErrorQuad(int);
    double __fastcall makeDuErrorCube(int);
    double __fastcall makeDvErrorCube(int);
    double __fastcall makeDwErrorCube(int);
    double __fastcall makekResidErrorCube(int);
    void __fastcall makeBTB(int,int);
    void __fastcall makeBTY(int,int,int);
    bool __fastcall solveB(int,int);
    void __fastcall makeCT(int,int);
    void __fastcall makeCTC(int,int);
    void __fastcall makeCTY(int,int,int);
    void __fastcall makeCXmY(int,int);
    bool __fastcall solveC(int,int);
    bool __fastcall solveD(int,int);
    void __fastcall writePTVtoFile(int);
    void __fastcall setPathAndFiles2();
    void __fastcall setPathAndFiles5();
    void __fastcall readXUAPFile(int,bool,bool);
    void __fastcall readXUAPDFile(int,bool,bool);
    void __fastcall readXUAPFileOld(int,bool);
    void __fastcall readGridFile(int,int);
    void __fastcall makeCor();
    void __fastcall makeStruct();
    void __fastcall makeStructRepr();
    void __fastcall makeCorFast();
    void __fastcall readWeights();
    void __fastcall makeKrigingWeights();
    void __fastcall autoCorrelation();
    void __fastcall autoCorrelationFast();
    void __fastcall makeKrigingWeightsInnerLoop();
    void __fastcall doLinearInterp(FILE *);
    void __fastcall followTrajPointLinQuadforAccDeriv(FILE *,int,int,bool);
    void __fastcall followTrajPointLinQuadforAccDerivGrid(FILE *,int,int,bool);
    void __fastcall FilterGrid(FILE *,int);
    void __fastcall estimateAccuracy(int);
    void __fastcall setAllMatrixesToZero(int);
    void __fastcall makeVolumes();
    void __fastcall setPathAndFiles3();
    void __fastcall setPathAndFiles4();
    void __fastcall readTrajPointFile(int);
    void __fastcall makeSurfaces();
    void __fastcall makeLines();
    void __fastcall reLink();
    void __fastcall makeFilesForCorrelations();
    void __fastcall getMean();
    void __fastcall writeXUADP(int);

    virtual __fastcall TpointList(void);   //constructor
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
extern PACKAGE TmainForm *mainForm;

//my stuff for external use  also in *.cpp

//extern TpointList *pointList;
//---------------------------------------------------------------------------
#endif
