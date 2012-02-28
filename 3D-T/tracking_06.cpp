//////////////////////////////////////////////////////////////////////////////
//
// this code was written by Beat Luthi in Risø National Laboratory, Denamrk
//                                     July 2006
//
// is represents an attempt to finally glue tracks together that have
// only one missing gap
//
// as a side effect the conventional tracking is replaced by a (probably )simpler one:
//
// main principle: (next neighbour) + (best fit to quadratic spline) 
// where the spline is fitted to the last 4 points plus the candiadate
//
// this algorithm produces a lot more long tracks than the previous codes
// we used from both Risø and ETH
//
// please send your comments and questions to: 
//
// beat.luthi@risoe.dk
//
//////////////////////////////////////////////////////////////////////////////

/*

This software links 3D particle positions of consequtivee time steps. 

Copyright (C) 2006 Beat Luthi, Risø, Nat. Lab, Denmark

This program is free software; you can redistribute it and/or modify it under 
the terms of the GNU General Public License v2, as published by the Free 
Software Foundation, provided that the above copyright notices and this 
permission notice appear in all copies of the software and related documentation.
You may charge a fee for the physical act of transferring a copy, and you may at 
your option offer warranty protection in exchange for a fee.

You may not copy, modify, sublicense, or distribute the Program except as 
expressly provided under this License.

This program is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR 
A PARTICULAR PURPOSE.

*/


#include "stdafx.h"

Struct_pointList pointList;

static void read_rt_is(int from, int to);
static void read_pt3(int how_many);
static void read_pt3_pln(int how_many);
static void track_with_splines(int frame);
static void track_gluing_gaps(int frame);
static double try_points(double x[10][3],int pos_in_poly);
static void project_over_gap(bool forward,double x[10][3]);
static void project_in_gap(double x[10][3]);
static void find_neighbour(int cam,double x, double y, double z,int frame, int pos_in_poly,double max_dist);
static void compute_velocities(int frame);
static void getVel(int frame,int i);
static void compute_statistics(int start,int end);
static void write_binary(int start,int end);
static void write_ASCII(int start, int end);
static void remove_kinks(int start, int end);
static void write_traj(int start, int end);
double mod(double x, double y);

//void make_A_ij(double centerX,double centerY,double centerZ,double radius);
//void polynomial_fit(int length);
//void write_point();

double Imq(int j, int m, int n, int i, double x, double y, double z);
double Req(int j, int m, int n, int i, double x, double y, double z);
double Imp(int j, int m, int n, int i, double x, double y, double z);
double Rep(int j, int m, int n, int i, double x, double y, double z);
double Power(double x,int n);
double Sqrt(double x);

void makeAT(int n, int m);
void makeATA(int n, int m);
void makeATY(int n, int m,int wh);
void solveA(int n, int m);

void makeAT_poly(int index,int n, int m);
void makeATA_poly(int index,int n, int m);
void makeATY_poly(int n, int m,int index,int ii);
void solveA_poly(int index,int n, int m);



int main(int argc, char *argv[])
{
	char garb[10];
	/////////////////////////////////////////////////////////////
	//sprintf (pointList.experiment, "Z:/macroTracking/rec_060524_10_1.q");
	sprintf (pointList.experiment, argv[1]);
	//rec_060524_18_1.cc.pt3

	pointList.firstFile                = 10000;
	pointList.lastFile                 = 19999;//20000;
	pointList.max_num_per_frame        = 2000; //as statically used in stdafx.h!

    //fact
	pointList.delta_t                  = 0.021;

	//controls
	pointList.link_tolerance           = 0.8;//0.2 is cleaner but too restrictive, checked with MATLAB plot_new_tracks.m
	pointList.jump_tolerance           = 2.5; // is nicely tuned and checked with MATLAB plot_new_tracks.m
	pointList.max_vel                  = 0.12;//0.12;//atof(argv[2]);//

	pointList.gluing                   = true;
	pointList.track                    = true;
	pointList.iterate                  = false;
	pointList.remove_kinks             = true;
	pointList.write_binary             = true;
	pointList.write_traj               = false; 
	pointList.output                   = false;
	pointList.same_cam_config          = true;
	/////////////////////////////////////////////////////////////////

	//check with Jakob's code
	//C:/risoe_ptv_res_velacccorrelation inp.txt >new.dat
	//and then with Mathematica nb C:/risoe_ptv/Mathematica_stuff/Lagrangian_auto_correl.nb

	if(pointList.track){
       /*for (int i=pointList.firstFile;i<pointList.lastFile;i=i++){
	      read_rt_is(i,i-pointList.firstFile);
	      if(mod((double)i,(double)100)==0){
	         cout << "done reading points of frame: ........."<<i<<"\n";
	      }
       }*/
	   if(pointList.iterate){
           //read in already linked stuff to continue on already tracked stuff
		   read_pt3_pln(pointList.lastFile-pointList.firstFile+1);
	   }
	   else{
		   if(pointList.output){
	           cout << "reading binary points\n";
		   }
          read_pt3(pointList.lastFile-pointList.firstFile+1);
	   }

       //tracking
	   if(pointList.output){
	       cout << "\n";
	   }
	   for(int i=pointList.firstFile;i<pointList.lastFile;i=i++){
	       track_with_splines(i-pointList.firstFile); 
		   if(pointList.output){
		       if(pointList.output){
			       if(mod((double)i,(double)100)==0){
		               cout << "done linking points of frame: ........."<<i<<"\n";
		           }
			   }
		   }
       }

	   //gluing
	   pointList.num_glues=0;
	   if(pointList.gluing){
		   if(pointList.output){
		       cout << "\n";
		   }
	       for(int i=pointList.firstFile;i<pointList.lastFile;i=i++){
	           track_gluing_gaps(i-pointList.firstFile);
		       if(pointList.output){
			       if(mod((double)i,(double)100)==0){
		               cout << "done gluing points of frame: ........."<<i<<"\n";
		           }
			   }
           }
	   }

	   //remove kinks
	   if(pointList.remove_kinks){
		   if(pointList.output){
		       cout << "\n";
		   }
	       remove_kinks(3,pointList.lastFile-pointList.firstFile-2);
	   }

	}
	else{
		//read in already linked stuff to e.g. compare with previous tracking
		read_pt3_pln(pointList.lastFile-pointList.firstFile+1);
		//gluing
	    pointList.num_glues=0;
	    if(pointList.gluing){
		   if(pointList.output){
			    cout << "\n";
		   }
	       for(int i=pointList.firstFile;i<pointList.lastFile;i=i++){
	           track_gluing_gaps(i-pointList.firstFile);
		       if(pointList.output){
			       if(mod((double)i,(double)100)==0){
		               cout << "done gluing points of frame: ........."<<i<<"\n";
		           }
			   }
           }
	    }
		//remove kinks
	   if(pointList.remove_kinks){
		   if(pointList.output){
		       cout << "\n";
		   }
	       remove_kinks(3,pointList.lastFile-pointList.firstFile-2);
	   }
	}

    

    //do velocities
	compute_velocities(pointList.lastFile-pointList.firstFile); 
	

	//write out
	if(pointList.output){
	    cout << "\n";
	}
	//write_ASCII(1,pointList.lastFile-pointList.firstFile); //writes ptv_is files
	if(pointList.write_traj){
	    write_traj(1,pointList.lastFile-pointList.firstFile);
	}
	if(pointList.write_binary){
	    write_binary(0,pointList.lastFile-pointList.firstFile+1);
	    if(pointList.output){
		    cout << "done writing files\n";
		}
	}

    //do statistics
	compute_statistics(5,pointList.lastFile-pointList.firstFile-5);
	cout << "\n";
	cout << "summary for linking of run: ...."<<pointList.experiment<<"\n";
	cout << "number of processed frames: ...."<<pointList.lastFile-pointList.firstFile+1<<"\n";
	cout << "rms u: ........................."<<pointList.rms_u<<"\n";
	cout << "max allowed velocity: .........."<<pointList.max_vel<<"\n";
    cout << "max velocity: .................."<<pointList.max_meas_vel<<"\n";
	cout << "av. links per frame: ..........."<<pointList.links_per_frame<<"\n";
	cout << "av. trajectory length: ........."<<pointList.av_traj_length<<"\n";
	cout << "rms trajectory length: ........."<<pointList.rms_traj_length<<"\n";	   
	cout << "max. trajectory length: ........"<<pointList.max_traj_length<<"\n";
	cout << "min. trajectory length: ........"<<pointList.min_traj_length<<"\n";
	cout << "av. gluwed traj. per frame: ...."<<pointList.num_glues<<"\n";

	//scanf("Please hit a key  %s", garb);
	
	/*
	for(int i=pointList.firstFile;i<pointList.lastFile;i=i++){
	    track_polynomials(i) 
    }
	*/
}
static void read_pt3_pln(int how_many){
    float x,y,z;
	int frame,numPoints,numPoints_pln,ok,c;
	short int left,right;
	bool not_finished;

    char filename_pt3[256];
	FILE *fPts_pt3;
	char filename_pln[256];
	FILE *fPts_pln;
	
	if(pointList.iterate){
        c=sprintf (filename_pt3, pointList.experiment);
	    c+=sprintf (filename_pt3+c, "_new.pt3");
	    c=sprintf (filename_pln, pointList.experiment);
	    c+=sprintf (filename_pln+c, "_new.pln");
	}
	else{
	    c=sprintf (filename_pt3, pointList.experiment);
	    c+=sprintf (filename_pt3+c, ".pt3");
	    c=sprintf (filename_pln, pointList.experiment);
	    c+=sprintf (filename_pln+c, ".pln");
	}

    fPts_pt3  = fopen(filename_pt3,"rb");
	fPts_pln  = fopen(filename_pln,"rb");

	not_finished=true;
	frame=0;
    while(frame<how_many && not_finished){
		ok=fread(&numPoints,sizeof(int),1,fPts_pt3);
		pointList.numPointsPerFrame[frame]=numPoints;
        if(!(ok==1)){
			not_finished=false;}
        if(not_finished){
            for(int i=0;i<numPoints;i++){
                ok=fread(&x,sizeof(float),1,fPts_pt3);
                ok=fread(&y,sizeof(float),1,fPts_pt3);
                ok=fread(&z,sizeof(float),1,fPts_pt3);
				pointList.pointPos[frame][i][0]=0.001*(double)x;
                pointList.pointPos[frame][i][1]=0.001*(double)y;
				pointList.pointPos[frame][i][2]=0.001*(double)z;
            }
			pointList.lastFile=10000+frame;
        }
		ok=fread(&numPoints_pln,sizeof(int),1,fPts_pln);
		if(numPoints_pln!=numPoints){
			if(pointList.output){
                cout << "something is fishy with the binary files!\n";
			}
		}
		if(!(ok==1)){not_finished=false;}
        if(not_finished){
            for(int i=0;i<numPoints;i++){
                ok=fread(&left,2,1,fPts_pln);
				pointList.pointAddress[frame][i][0]=(int)left;
            }
			for(int i=0;i<numPoints;i++){
				ok=fread(&right,2,1,fPts_pln);
                pointList.pointAddress[frame][i][1]=(int)right;
            }
			if(pointList.output){
			    if(mod((double)frame,(double)100)==0){
		            cout << "done reading frame: ........."<<10000+frame<<"\n";
		        }
			}
        }
		frame++;
	}
    fclose(fPts_pt3);
	fclose(fPts_pln);
}

static void read_pt3(int how_many){
    float x,y,z;
	int cam;
	int frame,numPoints,ok,c;
	bool not_finished;

    char filename[256];
	FILE *fPts;
	
	c=sprintf (filename, pointList.experiment);
	c+=sprintf (filename+c, ".pt3");

    fPts  = fopen(filename,"rb");

	not_finished=true;
	frame=0;
    while(frame<how_many && not_finished){
		ok=fread(&numPoints,sizeof(int),1,fPts);
		pointList.numPointsPerFrame[frame]=numPoints;
        if(!(ok==1)){not_finished=false;}
        if(not_finished){
            for(int i=0;i<numPoints;i++){
                ok=fread(&x,sizeof(float),1,fPts);
                ok=fread(&y,sizeof(float),1,fPts);
                ok=fread(&z,sizeof(float),1,fPts);
				//ok=fread(&cam,sizeof(int),1,fPts);
				pointList.pointPos[frame][i][0]=0.001*(double)x;
                pointList.pointPos[frame][i][1]=0.001*(double)y;
				pointList.pointPos[frame][i][2]=0.001*(double)z;
				pointList.pointCam[frame][i]=0;//cam;
				pointList.pointAddress[frame][i][0]=-1; 
		        pointList.pointAddress[frame][i][1]=-1;
            }
			pointList.lastFile=10000+frame;
			if(pointList.output){
			    if(mod((double)frame,(double)100)==0){
		            cout << "done reading frame: ........."<<10000+frame<<"\n";
		        }
			}
        }
		frame++;
	}
    fclose(fPts);
}

static void read_rt_is(int from, int to)
{
    FILE *fpp;
    char filename[256];
    
    int numOfPoints=0;
    double x,y,z,dummy;
	
	/////////////////////////////////////////////////////////////////
	sprintf (filename, "C:/risoe_ptv/res/rt_is.%5d", from);
	/////////////////////////////////////////////////////////////////
	fpp = fopen(filename,"r");
	fscanf (fpp, "%d\0", &numOfPoints);
	pointList.numPointsPerFrame[to]=numOfPoints;
	if (pointList.numPointsPerFrame[to]>pointList.max_num_per_frame){
        pointList.numPointsPerFrame[to]=pointList.max_num_per_frame;
	}
	for(int i=0;i<numOfPoints;i++){
		fscanf (fpp, "%d\0", &dummy);
        fscanf (fpp, "%lf\0", &x); 
        fscanf (fpp, "%lf\0", &y); 
        fscanf (fpp, "%lf\0", &z); 
		fscanf (fpp, "%lf\0", &dummy);
		fscanf (fpp, "%d\0", &dummy);
		fscanf (fpp, "%d\0", &dummy);
		fscanf (fpp, "%d\0", &dummy);
		fscanf (fpp, "%d\0", &dummy);
        
        pointList.pointAddress[to][i][0]=-1; 
		pointList.pointAddress[to][i][1]=-1;
        pointList.pointPos[to][i][0]=0.001*x;
		pointList.pointPos[to][i][1]=0.001*y;
		pointList.pointPos[to][i][2]=0.001*z;
	}
	fclose(fpp);
}
static void find_neighbour(int cam,double x, double y, double z,int frame, int pos_in_poly,double max_dist){
    
	double dist,dx,dy,dz;

	for(int i=0;i<pointList.numPointsPerFrame[frame];i++){
		if(!pointList.same_cam_config){cam=pointList.pointCam[frame][i];}
		if(cam==pointList.pointCam[frame][i]){
		dx=pointList.pointPos[frame][i][0]-x;
		 if(fabs(dx)<max_dist){
            dy=pointList.pointPos[frame][i][1]-y;
			if(fabs(dy)<max_dist){
				dz=pointList.pointPos[frame][i][2]-z;
				if(fabs(dz)<max_dist){
                   dist=pow(dx*dx+dy*dy+dz*dz,0.5);
				   if(dist<max_dist){
					   if(pos_in_poly==6){ //because only pos_in_poly=6 is going backwards in time..(is called from track gluing gaps)
						   if(pointList.pointAddress[frame][i][1]<0){
							   if(pointList.num_cand[pos_in_poly]<20){
                                   pointList.cand_neighbours[pos_in_poly][pointList.num_cand[pos_in_poly]]=i;
                                   pointList.distance[pos_in_poly][pointList.num_cand[pos_in_poly]]=dist;
					               pointList.num_cand[pos_in_poly]++;
							   }
						   }
					   }
					   else{
						   if(pointList.pointAddress[frame][i][0]<0){
							   if(pointList.num_cand[pos_in_poly]<20){
                                   pointList.cand_neighbours[pos_in_poly][pointList.num_cand[pos_in_poly]]=i;
                                   pointList.distance[pos_in_poly][pointList.num_cand[pos_in_poly]]=dist;
					               pointList.num_cand[pos_in_poly]++;
						       }
						       else{
								   if(pointList.output){
                                       cout << "too many cand. point in find-neighbour\n";
								   }
						       }
						   }
					   }
				   }
				}
			}
		}
		}
	}
}

static void project_over_gap(bool forward,double p[10][3]){

    double cons[3][3];
	double time;

	for(int i=0;i<5;i++){
        for(int j=0;j<3;j++){
			pointList.A[i][j]=pow((double)(i)*pointList.delta_t,(double)(j));
        }
		for(int comp=0;comp<3;comp++){
			pointList.Y[i][comp]=p[i][comp];
		}
    }

	makeAT(5,3);
    makeATA(5,3);
    makeATY(5,3,1);
    solveA(5,3);
	cons[0][0]=pointList.X[0];
	cons[0][1]=pointList.X[1];
	cons[0][2]=pointList.X[2];

	makeAT(5,3);
    makeATA(5,3);
    makeATY(5,3,2);
    solveA(5,3);
	cons[1][0]=pointList.X[0];
	cons[1][1]=pointList.X[1];
	cons[1][2]=pointList.X[2];

	makeAT(5,3);
    makeATA(5,3);
    makeATY(5,3,3);
    solveA(5,3);
	cons[2][0]=pointList.X[0];
	cons[2][1]=pointList.X[1];
	cons[2][2]=pointList.X[2];

    pointList.x=0.;
	pointList.y=0.;
	pointList.z=0.;
	
	if(forward){time=6.;}
	else{time=-2.;}

    for(int k=0;k<3;k++){				
        pointList.x=pointList.x+cons[0][k]*pow(time*pointList.delta_t,(double)(k));
    }
	for(int k=0;k<3;k++){				
        pointList.y=pointList.y+cons[1][k]*pow(time*pointList.delta_t,(double)(k));
    }
	for(int k=0;k<3;k++){				
        pointList.z=pointList.z+cons[2][k]*pow(time*pointList.delta_t,(double)(k));
    }
}

static void remove_kinks(int start, int end){
    double cons[3][3];
	int trajectory[1000][2];
	double smooth_point[1000][3];
	double change[1000],av_change;
	double weight[1000];
	int ind,point_ind,frame_ind,iii;
	double dx1,dx2,dy1,dy2,dz1,dz2,dist1,dist2,cosine;
    
	for(int i=start;i<end;i++){
		if(pointList.output){
		    if(mod((double)i,(double)100)==0){
		        cout << "done removing kinks of frame: ........."<<10000+i<<"\n";
		    }
		}
	    for(int j=0;j<pointList.numPointsPerFrame[i];j++){
			ind=0;
			point_ind=j;
			frame_ind=i;
			if(pointList.pointAddress[i][j][0]<0 && pointList.pointAddress[i][j][1]>-1){
				trajectory[ind][0]=frame_ind;
				trajectory[ind][1]=point_ind;
				while(pointList.pointAddress[frame_ind][point_ind][1]>-1 && frame_ind<end){
					point_ind=pointList.pointAddress[frame_ind][point_ind][1];
					frame_ind++;
					ind++;
					trajectory[ind][0]=frame_ind;
					trajectory[ind][1]=point_ind;
					if(ind>6){ //e.g. if at least 5 connected points
	                    for(int ii=0;ii<7;ii++){
						    iii=ind-7+ii;
                            for(int jj=0;jj<3;jj++){
			                    pointList.A[ii][jj]=pow((double)(ii-3)*pointList.delta_t,(double)(jj));
                            }
		                    for(int comp=0;comp<3;comp++){
							    pointList.Y[ii][comp]=pointList.pointPos[trajectory[iii][0]][trajectory[iii][1]][comp];
		                    }
                        }

	                    makeAT(7,3);
                        makeATA(7,3);
                        makeATY(7,3,1);
                        solveA(7,3);
	                    cons[0][0]=pointList.X[0];
	                    cons[0][1]=pointList.X[1];
	                    cons[0][2]=pointList.X[2];

	                    makeAT(7,3);
                        makeATA(7,3);
                        makeATY(7,3,2);
                        solveA(7,3);
	                    cons[1][0]=pointList.X[0];
	                    cons[1][1]=pointList.X[1];
	                    cons[1][2]=pointList.X[2];

	                    makeAT(7,3);
                        makeATA(7,3);
                        makeATY(7,3,3);
                        solveA(7,3);
	                    cons[2][0]=pointList.X[0];
	                    cons[2][1]=pointList.X[1];
	                    cons[2][2]=pointList.X[2];

                        smooth_point[ind-4][0]=0.;
	                    smooth_point[ind-4][1]=0.;
	                    smooth_point[ind-4][2]=0.;
	
                        smooth_point[ind-4][0]=cons[0][0];
                        smooth_point[ind-4][1]=cons[1][0];				
                        smooth_point[ind-4][2]=cons[2][0];
				    }
				}
			}
			//now update points with smoothed points
			if(ind>6){
				//find worst points, mark them and then compute smooth again without worst points!
			    av_change=0;;
				for(int k=3;k<ind-3;k++){
					change[k]=
						pow(
						pow(pointList.pointPos[trajectory[k][0]][trajectory[k][1]][0]-smooth_point[k][0],2.)+
						pow(pointList.pointPos[trajectory[k][0]][trajectory[k][1]][1]-smooth_point[k][1],2.)+
						pow(pointList.pointPos[trajectory[k][0]][trajectory[k][1]][2]-smooth_point[k][2],2.),0.5);
				    av_change=av_change+change[k];
			    }
				av_change=av_change/(double)(ind-6);
				for(int k=0;k<1000;k++){weight[k]=1.;}
				for(int k=3;k<ind-3;k++){
					//begin compute cosine
                    dx1=pointList.pointPos[trajectory[k][0]][trajectory[k][1]][0]-pointList.pointPos[trajectory[k-1][0]][trajectory[k-1][1]][0];
					dy1=pointList.pointPos[trajectory[k][0]][trajectory[k][1]][1]-pointList.pointPos[trajectory[k-1][0]][trajectory[k-1][1]][1];
					dz1=pointList.pointPos[trajectory[k][0]][trajectory[k][1]][2]-pointList.pointPos[trajectory[k-1][0]][trajectory[k-1][1]][2];
                    dist1=pow(dx1*dx1+dy1*dy1+dz1*dz1,0.5);
					dx1=dx1/dist1;dy1=dy1/dist1;dz1=dz1/dist1;
					dx2=pointList.pointPos[trajectory[k+1][0]][trajectory[k+1][1]][0]-pointList.pointPos[trajectory[k][0]][trajectory[k][1]][0];
					dy2=pointList.pointPos[trajectory[k+1][0]][trajectory[k+1][1]][1]-pointList.pointPos[trajectory[k][0]][trajectory[k][1]][1];
					dz2=pointList.pointPos[trajectory[k+1][0]][trajectory[k+1][1]][2]-pointList.pointPos[trajectory[k][0]][trajectory[k][1]][2];
					dist2=pow(dx2*dx2+dy2*dy2+dz2*dz2,0.5);
					dx2=dx2/dist2;dy2=dy2/dist2;dz2=dz2/dist2;
					cosine=dx1*dx2+dy1*dy2+dz1*dz2;
					//end compute cosine

					if(cosine<0.&& dist1>5*av_change && dist2>5*av_change){
						weight[k]=0.1;
					}
					else{
						weight[k]=1.;
					}
				}
				for(int k=7;k<ind;k++){
	                for(int ii=0;ii<7;ii++){//for(int ii=0;ii<7;ii++){
						iii=k-7+ii;
                        for(int jj=0;jj<3;jj++){
			                pointList.A[ii][jj]=weight[iii]*pow((double)(ii-3)*pointList.delta_t,(double)(jj));
                        }
		                for(int comp=0;comp<3;comp++){
							pointList.Y[ii][comp]=weight[iii]*pointList.pointPos[trajectory[iii][0]][trajectory[iii][1]][comp];
		                }
                    }

	                makeAT(7,3);
                    makeATA(7,3);
                    makeATY(7,3,1);
                    solveA(7,3);
	                cons[0][0]=pointList.X[0];
	                cons[0][1]=pointList.X[1];
	                cons[0][2]=pointList.X[2];

	                makeAT(7,3);
                    makeATA(7,3);
                    makeATY(7,3,2);
                    solveA(7,3);
	                cons[1][0]=pointList.X[0];
	                cons[1][1]=pointList.X[1];
	                cons[1][2]=pointList.X[2];

	                makeAT(7,3);
                    makeATA(7,3);
                    makeATY(7,3,3);
                    solveA(7,3);
	                cons[2][0]=pointList.X[0];
	                cons[2][1]=pointList.X[1];
	                cons[2][2]=pointList.X[2];

                    smooth_point[k-4][0]=0.;
	                smooth_point[k-4][1]=0.;
	                smooth_point[k-4][2]=0.;
	
                    smooth_point[k-4][0]=cons[0][0];
                    smooth_point[k-4][1]=cons[1][0];				
                    smooth_point[k-4][2]=cons[2][0];

					//if(weight[k-4]==0.1){///smooths only at position of kinks
				    for(int comp=0;comp<3;comp++){
					    pointList.pointPos[trajectory[k-4][0]][trajectory[k-4][1]][comp]=smooth_point[k-4][comp];
				    }
					//}
			    }
			}
		}
	}
}

static void project_in_gap(double p[10][3]){

    double cons[3][3];
	double time;

	for(int i=0;i<5;i++){
        for(int j=0;j<3;j++){
			pointList.A[i][j]=pow((double)(i)*pointList.delta_t,(double)(j));
        }
		for(int comp=0;comp<3;comp++){
			pointList.Y[i][comp]=p[i][comp];
		}
    }
	for(int i=5;i<10;i++){
        for(int j=0;j<3;j++){
			pointList.A[i][j]=pow((double)(i+1)*pointList.delta_t,(double)(j));
        }
		for(int comp=0;comp<3;comp++){
			pointList.Y[i][comp]=p[i][comp];
		}
    }

	makeAT(10,3);
    makeATA(10,3);
    makeATY(10,3,1);
    solveA(10,3);
	cons[0][0]=pointList.X[0];
	cons[0][1]=pointList.X[1];
	cons[0][2]=pointList.X[2];

	makeAT(10,3);
    makeATA(10,3);
    makeATY(10,3,2);
    solveA(10,3);
	cons[1][0]=pointList.X[0];
	cons[1][1]=pointList.X[1];
	cons[1][2]=pointList.X[2];

	makeAT(10,3);
    makeATA(10,3);
    makeATY(10,3,3);
    solveA(10,3);
	cons[2][0]=pointList.X[0];
	cons[2][1]=pointList.X[1];
	cons[2][2]=pointList.X[2];

    pointList.x=0.;
	pointList.y=0.;
	pointList.z=0.;
	
	time=5.;

    for(int k=0;k<3;k++){				
        pointList.x=pointList.x+cons[0][k]*pow(time*pointList.delta_t,(double)(k));
    }
	for(int k=0;k<3;k++){				
        pointList.y=pointList.y+cons[1][k]*pow(time*pointList.delta_t,(double)(k));
    }
	for(int k=0;k<3;k++){				
        pointList.z=pointList.z+cons[2][k]*pow(time*pointList.delta_t,(double)(k));
    }
}

static double try_points(double p[10][3],int pos_in_poly){

	double cons[3][3],B[5][3];
	double residum,dist,travel_dist;
	double diff[3][5];

	for(int i=0;i<5;i++){
        for(int j=0;j<3;j++){
			pointList.A[i][j]=pow((double)(i)*pointList.delta_t,(double)(j));
        }
		for(int comp=0;comp<3;comp++){
            pointList.Y[i][comp]=p[i][comp];
		}
    }

	makeAT(5,3);
    makeATA(5,3);
    makeATY(5,3,1);
    solveA(5,3);
	cons[0][0]=pointList.X[0];
	cons[0][1]=pointList.X[1];
	cons[0][2]=pointList.X[2];

	makeAT(5,3);
    makeATA(5,3);
    makeATY(5,3,2);
    solveA(5,3);
	cons[1][0]=pointList.X[0];
	cons[1][1]=pointList.X[1];
	cons[1][2]=pointList.X[2];

	makeAT(5,3);
    makeATA(5,3);
    makeATY(5,3,3);
    solveA(5,3);
	cons[2][0]=pointList.X[0];
	cons[2][1]=pointList.X[1];
	cons[2][2]=pointList.X[2];


	for(int i=0;i<5;i++){
		for(int j=0;j<3;j++){
	     	B[i][j]=0;
		}
	}
	for(int i=0;i<5;i++){
		for(int comp=0;comp<3;comp++){
            for(int k=0;k<3;k++){				
                B[i][comp]=B[i][comp]
					+cons[comp][k]*pow((double)i*pointList.delta_t,(double)(k));
            }
        }
	}

	for(int i=0;i<5;i++){
		diff[0][i]=B[i][0]-pointList.Y[i][0];
		diff[1][i]=B[i][1]-pointList.Y[i][1];
		diff[2][i]=B[i][2]-pointList.Y[i][2];
	}
	dist=pow(pow(diff[0][pos_in_poly],2.)+pow(diff[1][pos_in_poly],2.)+pow(diff[2][pos_in_poly],2.),0.5);
	
	if(pointList.output){
	    if(pos_in_poly==0){
	        cout << "something strange happens in try_points\n";
	    }
	}
	if(pos_in_poly==4){
	    travel_dist=pow(
			pow(pointList.Y[3][0]-pointList.Y[2][0],2.)+
			pow(pointList.Y[3][1]-pointList.Y[2][1],2.)+
			pow(pointList.Y[3][2]-pointList.Y[2][2],2.),0.5);
	}
	if(pos_in_poly>0 && pos_in_poly<4){
	    travel_dist=0.5*(
			pow(
			pow(pointList.Y[pos_in_poly-1][0]-pointList.Y[pos_in_poly-2][0],2.)+
			pow(pointList.Y[pos_in_poly-1][1]-pointList.Y[pos_in_poly-2][1],2.)+
			pow(pointList.Y[pos_in_poly-1][2]-pointList.Y[pos_in_poly-2][2],2.),0.5)+
			pow(
			pow(pointList.Y[pos_in_poly][0]-pointList.Y[pos_in_poly-1][0],2.)+
			pow(pointList.Y[pos_in_poly][1]-pointList.Y[pos_in_poly-1][1],2.)+
			pow(pointList.Y[pos_in_poly][2]-pointList.Y[pos_in_poly-1][2],2.),0.5));
	}
	
	residum=dist/travel_dist;//dist/max(travel_dist,pointList.pos_error);//0.1*pointList.pos_error;//residum/5.;//
       
	return residum;
}


static void track_gluing_gaps(int frame){
    
	int num_links,frame_ind,point_ind;
	int how_many_cand;
	int best_cand;
	double best_dist,dist_before,dist_between,dist_after,larger_dist;
	double x[10][3];
	int cam;


	for(int i=0;i<pointList.numPointsPerFrame[frame];i++){

		if(pointList.pointAddress[frame][i][1]<0){
			best_cand=-1;
			best_dist=10000;
			how_many_cand=0;
		    //find out if it has 4 links to the past
		    frame_ind=frame;
		    point_ind=i;
		    num_links=0;
		    while(pointList.pointAddress[frame_ind][point_ind][0]>-1 && num_links<4){
                point_ind=pointList.pointAddress[frame_ind][point_ind][0];
		   	    frame_ind--;
			    num_links++;
		    }
			if(num_links==4){
		        frame_ind=frame;
		        point_ind=i;
				pointList.cand_neighbours[4][0]=point_ind;
				for(int j=3;j>-1;j--){
					point_ind=pointList.pointAddress[frame_ind][point_ind][0];
                    frame_ind--;
                    pointList.cand_neighbours[j][0]=point_ind;
				}
				//project over gap with info from 5 points
				for(int c=0;c<3;c++){
					x[0][c]=pointList.pointPos[frame-4][pointList.cand_neighbours[0][0]][c];
					x[1][c]=pointList.pointPos[frame-3][pointList.cand_neighbours[1][0]][c];
					x[2][c]=pointList.pointPos[frame-2][pointList.cand_neighbours[2][0]][c];
					x[3][c]=pointList.pointPos[frame-1][pointList.cand_neighbours[3][0]][c];
					x[4][c]=pointList.pointPos[frame  ][pointList.cand_neighbours[4][0]][c];
				}
				cam=pointList.pointCam[frame  ][pointList.cand_neighbours[4][0]];
				project_over_gap(true,x);

                pointList.num_cand[5]=0;
			    find_neighbour(cam,pointList.x,pointList.y,pointList.z,frame+2,5,pointList.max_vel*pointList.delta_t);
				//try to jump back from these points
				for(int j=0;j<pointList.num_cand[5];j++){
                    //find out if it has 4 links to the future
					frame_ind=frame+2;
		            point_ind=pointList.cand_neighbours[5][j];
		            num_links=0;
		            while(pointList.pointAddress[frame_ind][point_ind][1]>-1 && num_links<4){
                        point_ind=pointList.pointAddress[frame_ind][point_ind][1];
		   	            frame_ind++;
			            num_links++;
		            }
			        if(num_links==4){
						frame_ind=frame+2;
		                point_ind=pointList.cand_neighbours[5][j];
				        pointList.cand_neighbours[0][0]=point_ind;
				        for(int jj=1;jj<5;jj++){
					        point_ind=pointList.pointAddress[frame_ind][point_ind][1];
                            frame_ind++;
                            pointList.cand_neighbours[jj][0]=point_ind;
				        }
           		        //project over gap (backwards) with info from 5 points
                        for(int c=0;c<3;c++){
					        x[0][c]=pointList.pointPos[frame+2][pointList.cand_neighbours[0][0]][c];
					        x[1][c]=pointList.pointPos[frame+3][pointList.cand_neighbours[1][0]][c];
					        x[2][c]=pointList.pointPos[frame+4][pointList.cand_neighbours[2][0]][c];
					        x[3][c]=pointList.pointPos[frame+5][pointList.cand_neighbours[3][0]][c];
					        x[4][c]=pointList.pointPos[frame+6][pointList.cand_neighbours[4][0]][c];
				        }
						cam=pointList.pointCam[frame+2][pointList.cand_neighbours[0][0]];
				        project_over_gap(false,x);

						pointList.num_cand[6]=0;
			            find_neighbour(cam,pointList.x,pointList.y,pointList.z,frame,6,pointList.max_vel*pointList.delta_t);
                        
                        for(int k=0;k<pointList.num_cand[6];k++){
							//if any of these points are close to pointPos[frame][i] then YES!
							if(i==pointList.cand_neighbours[6][k]){
								how_many_cand++;
								if(pointList.distance[6][k]<best_dist){
									best_dist=pointList.distance[6][k];
                                    best_cand=pointList.cand_neighbours[5][j];
								}
							}
							
						}
					}
				}
			}
			if(best_cand>-1){
				//compute point in gap
				point_ind=i;
				frame_ind=frame;
				for(int c=0;c<3;c++){
					x[4][c]=pointList.pointPos[frame_ind][point_ind][c];
				}
				point_ind=pointList.pointAddress[frame_ind][point_ind][0];
				frame_ind--;
				for(int c=0;c<3;c++){
					x[3][c]=pointList.pointPos[frame_ind][point_ind][c];
				}
				point_ind=pointList.pointAddress[frame_ind][point_ind][0];
				frame_ind--;
				for(int c=0;c<3;c++){
					x[2][c]=pointList.pointPos[frame_ind][point_ind][c];
				}
				point_ind=pointList.pointAddress[frame_ind][point_ind][0];
				frame_ind--;
				for(int c=0;c<3;c++){
					x[1][c]=pointList.pointPos[frame_ind][point_ind][c];
				}
				point_ind=pointList.pointAddress[frame_ind][point_ind][0];
				frame_ind--;
				for(int c=0;c<3;c++){
					x[0][c]=pointList.pointPos[frame_ind][point_ind][c];
				}
				point_ind=best_cand;
				frame_ind=frame+2;
				for(int c=0;c<3;c++){
					x[5][c]=pointList.pointPos[frame_ind][point_ind][c];
				}
				point_ind=pointList.pointAddress[frame_ind][point_ind][1];
				frame_ind++;
				for(int c=0;c<3;c++){
					x[6][c]=pointList.pointPos[frame_ind][point_ind][c];
				}
				point_ind=pointList.pointAddress[frame_ind][point_ind][1];
				frame_ind++;
				for(int c=0;c<3;c++){
					x[7][c]=pointList.pointPos[frame_ind][point_ind][c];
				}
				point_ind=pointList.pointAddress[frame_ind][point_ind][1];
				frame_ind++;
				for(int c=0;c<3;c++){
					x[8][c]=pointList.pointPos[frame_ind][point_ind][c];
				}
				point_ind=pointList.pointAddress[frame_ind][point_ind][1];
				frame_ind++;
				for(int c=0;c<3;c++){
					x[9][c]=pointList.pointPos[frame_ind][point_ind][c];
				}
                //last safety check if jump is not too big
				dist_before=
					pow(
			        pow(x[4][0]-x[3][0],2.)+
			        pow(x[4][1]-x[3][1],2.)+
			        pow(x[4][2]-x[3][2],2.),0.5);
				dist_after=
					pow(
			        pow(x[6][0]-x[5][0],2.)+
			        pow(x[6][1]-x[5][1],2.)+
			        pow(x[6][2]-x[5][2],2.),0.5);
				dist_between=
					pow(
			        pow(x[5][0]-x[4][0],2.)+
			        pow(x[5][1]-x[4][1],2.)+
			        pow(x[5][2]-x[4][2],2.),0.5);
				if(dist_before>dist_after){larger_dist=dist_before;}
				else{larger_dist=dist_after;}
				if(dist_between<pointList.jump_tolerance*larger_dist){
					pointList.num_glues++;
				    project_in_gap(x);
				    pointList.x=pointList.x;
	                pointList.y=pointList.y;
	                pointList.z=pointList.z;

				    //store this new point and link it.
				    pointList.pointPos[frame+1][pointList.numPointsPerFrame[frame+1]][0]=pointList.x;
				    pointList.pointPos[frame+1][pointList.numPointsPerFrame[frame+1]][1]=pointList.y;
				    pointList.pointPos[frame+1][pointList.numPointsPerFrame[frame+1]][2]=pointList.z;

				    pointList.pointAddress[frame][i][1]=pointList.numPointsPerFrame[frame+1];
                    pointList.pointAddress[frame+1][pointList.numPointsPerFrame[frame+1]][1]=best_cand;
				    pointList.pointAddress[frame+2][best_cand][0]=pointList.numPointsPerFrame[frame+1];
				    pointList.pointAddress[frame+1][pointList.numPointsPerFrame[frame+1]][0]=i;

				    pointList.numPointsPerFrame[frame+1]++;
				}
			}
		}
	}
}

static void track_with_splines(int frame){
    
	int pos_in_poly,frame_ind,point_ind,ind;
	double x[5][3];
	double residum,best_residum;
	int best_cand[5];

	for(int i=0;i<pointList.numPointsPerFrame[frame];i++){

		if(pointList.pointAddress[frame][i][1]<0){
		    //find out how far back it is already linked to def. 'pos_in_poly'
		    frame_ind=frame;
		    point_ind=i;
		    pos_in_poly=1;
		    best_cand[0]=-1;
		    best_cand[1]=-1;
		    best_cand[2]=-1;
		    best_cand[3]=-1;
		    best_cand[4]=-1;
		    while(pointList.pointAddress[frame_ind][point_ind][0]>-1 && pos_in_poly<4){
                point_ind=pointList.pointAddress[frame_ind][point_ind][0];
		   	    frame_ind--;
			    pos_in_poly++;
		    }
		    frame_ind=frame;
		    point_ind=i;
		    ind=1;
		    while(pos_in_poly-ind>-1 ){
                pointList.cand_neighbours[pos_in_poly-ind][0]=point_ind;
			    pointList.num_cand[pos_in_poly-ind]=1;
                point_ind=pointList.pointAddress[frame_ind][point_ind][0];
			    frame_ind--;
			    ind++;
		    }
		    //go and search for 'pos_in_poly' up to 4
		    frame_ind=frame;
		    point_ind=i;
		    best_residum=100000;
		    int s=pos_in_poly;
		    pointList.num_cand[s]=0;
		    for(int j=0;j<20;j++){pointList.cand_neighbours[s][j]=-1;}
		    find_neighbour(pointList.pointCam[frame_ind][point_ind],pointList.pointPos[frame_ind][point_ind][0],pointList.pointPos[frame_ind][point_ind][1],pointList.pointPos[frame_ind][point_ind][2],frame_ind+1,s,pointList.max_vel*pointList.delta_t);
			if(s+1<5){
			    for(int k=0;k<pointList.num_cand[s];k++){
					frame_ind=frame+1;
		            point_ind=pointList.cand_neighbours[pos_in_poly][k];
					pointList.num_cand[s+1]=0;
		            for(int j=0;j<20;j++){pointList.cand_neighbours[s+1][j]=-1;}
                    find_neighbour(pointList.pointCam[frame_ind][point_ind],pointList.pointPos[frame_ind][point_ind][0],pointList.pointPos[frame_ind][point_ind][1],pointList.pointPos[frame_ind][point_ind][2],frame_ind+1,s+1,pointList.max_vel*pointList.delta_t);
                    if(s+2<5){
					    for(int l=0;l<pointList.num_cand[s+1];l++){
					        frame_ind=frame+2;
		                    point_ind=pointList.cand_neighbours[pos_in_poly+1][l];
					        pointList.num_cand[s+2]=0;
		                    for(int j=0;j<20;j++){pointList.cand_neighbours[s+2][j]=-1;}
                            find_neighbour(pointList.pointCam[frame_ind][point_ind],pointList.pointPos[frame_ind][point_ind][0],pointList.pointPos[frame_ind][point_ind][1],pointList.pointPos[frame_ind][point_ind][2],frame_ind+1,s+2,pointList.max_vel*pointList.delta_t);
							if(s+3<5){
					            for(int n=0;n<pointList.num_cand[s+2];n++){
					                frame_ind=frame+3;
		                            point_ind=pointList.cand_neighbours[pos_in_poly+2][n];
					                pointList.num_cand[s+3]=0;
		                            for(int j=0;j<20;j++){pointList.cand_neighbours[s+3][j]=-1;}
                                    find_neighbour(pointList.pointCam[frame_ind][point_ind],pointList.pointPos[frame_ind][point_ind][0],pointList.pointPos[frame_ind][point_ind][1],pointList.pointPos[frame_ind][point_ind][2],frame_ind+1,s+3,pointList.max_vel*pointList.delta_t);
						            //compute spline for candites in pos 1,2,3,4
									for(int m=0;m<pointList.num_cand[s+3];m++){
										for(int c=0;c<3;c++){
											x[0][c]=pointList.pointPos[frame  ][pointList.cand_neighbours[0][0]][c];
											x[1][c]=pointList.pointPos[frame+1][pointList.cand_neighbours[1][k]][c];
											x[2][c]=pointList.pointPos[frame+2][pointList.cand_neighbours[2][l]][c];
											x[3][c]=pointList.pointPos[frame+3][pointList.cand_neighbours[3][n]][c];
											x[4][c]=pointList.pointPos[frame+4][pointList.cand_neighbours[4][m]][c];
										}
										residum=try_points(x,pos_in_poly);
										if(residum<pointList.link_tolerance){
											if(residum<best_residum){
                                                 best_residum=residum;
												 best_cand[0]=pointList.cand_neighbours[0][0];
												 best_cand[1]=pointList.cand_neighbours[1][k];
												 best_cand[2]=pointList.cand_neighbours[2][l];
												 best_cand[3]=pointList.cand_neighbours[3][n];
												 best_cand[4]=pointList.cand_neighbours[4][m];
											}
										}
									}
								}
					        }
							else{ //compute spline for candites in pos 2,3,4
                                for(int n=0;n<pointList.num_cand[s+2];n++){
									for(int c=0;c<3;c++){
										x[0][c]=pointList.pointPos[frame-1][pointList.cand_neighbours[0][0]][c];
										x[1][c]=pointList.pointPos[frame+0][pointList.cand_neighbours[1][0]][c];
										x[2][c]=pointList.pointPos[frame+1][pointList.cand_neighbours[2][k]][c];
										x[3][c]=pointList.pointPos[frame+2][pointList.cand_neighbours[3][l]][c];
										x[4][c]=pointList.pointPos[frame+3][pointList.cand_neighbours[4][n]][c];
									}
									residum=try_points(x,pos_in_poly);
									if(residum<pointList.link_tolerance){
										if(residum<best_residum){
                                            best_residum=residum;
										    best_cand[0]=pointList.cand_neighbours[0][0];
											best_cand[1]=pointList.cand_neighbours[1][0];
											best_cand[2]=pointList.cand_neighbours[2][k];
											best_cand[3]=pointList.cand_neighbours[3][l];
											best_cand[4]=pointList.cand_neighbours[4][n];
										}
									}
								}
					        }
						}
					}
					else{ //compute spline for candites in pos 3,4
                        for(int l=0;l<pointList.num_cand[s+1];l++){
							for(int c=0;c<3;c++){
								x[0][c]=pointList.pointPos[frame-2][pointList.cand_neighbours[0][0]][c];
								x[1][c]=pointList.pointPos[frame-1][pointList.cand_neighbours[1][0]][c];
								x[2][c]=pointList.pointPos[frame+0][pointList.cand_neighbours[2][0]][c];
								x[3][c]=pointList.pointPos[frame+1][pointList.cand_neighbours[3][k]][c];
								x[4][c]=pointList.pointPos[frame+2][pointList.cand_neighbours[4][l]][c];
							}
							residum=try_points(x,pos_in_poly);
							if(residum<pointList.link_tolerance){
								if(residum<best_residum){
                                    best_residum=residum;
								    best_cand[0]=pointList.cand_neighbours[0][0];
									best_cand[1]=pointList.cand_neighbours[1][0];
									best_cand[2]=pointList.cand_neighbours[2][0];
									best_cand[3]=pointList.cand_neighbours[3][k];
									best_cand[4]=pointList.cand_neighbours[4][l];
								}
							}
						}
					}
				}
			}
			else{ //compute spline for candites in pos 4
                for(int k=0;k<pointList.num_cand[s];k++){
					for(int c=0;c<3;c++){
						x[0][c]=pointList.pointPos[frame-3][pointList.cand_neighbours[0][0]][c];
						x[1][c]=pointList.pointPos[frame-2][pointList.cand_neighbours[1][0]][c];
						x[2][c]=pointList.pointPos[frame-1][pointList.cand_neighbours[2][0]][c];
						x[3][c]=pointList.pointPos[frame+0][pointList.cand_neighbours[3][0]][c];
						x[4][c]=pointList.pointPos[frame+1][pointList.cand_neighbours[4][k]][c];
					}
					residum=try_points(x,pos_in_poly);
					if(residum<pointList.link_tolerance){
						if(residum<best_residum){
                            best_residum=residum;
						    best_cand[0]=pointList.cand_neighbours[0][0];
						    best_cand[1]=pointList.cand_neighbours[1][0];
							best_cand[2]=pointList.cand_neighbours[2][0];
							best_cand[3]=pointList.cand_neighbours[3][0];
							best_cand[4]=pointList.cand_neighbours[4][k];
						}
					}
				}
			}
		    //NOW LINK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		    if(best_residum<pointList.link_tolerance){
                 pointList.pointAddress[frame][i][1]=best_cand[pos_in_poly];
			     pointList.pointAddress[frame+1][best_cand[pos_in_poly]][0]=i;
		    }
		}
	}
}



static void compute_velocities(int end){

	pointList.vel_count=0;
    for(int i=1;i<end;i=i++){
	    for(int j=0;j<pointList.numPointsPerFrame[i];j++){
		   getVel(i,j);
	   } 
    }
	pointList.rms_u=pow(pointList.rms_u/(double)pointList.vel_count,0.5);
}




double Sqrt(double x)
{
	return sqrt(x);
}
 

double Power(double x,int n)
{
	return pow(x,n);
/*	switch (n) {
	case 2:
		return x*x;
		break;
	case 3:
		return x*x*x;
		break;
	default:
		nrerror("???? Error in Power");
		break;
	}*/
}

double Rep(int j, int m, int n, int i, double x, double y, double z)
{
	double ex;
	int code;
        double Pi=3.141592653589793;

	code = i*1000+j*100+m*10+n;

/* Re p */
switch (code) {
case 1100 :
ex = 0;
break;
case 2100 :
ex = 0;
break;
case 3100 :
ex = Sqrt(3/Pi)/2.;
break;
case 1101 :
ex = (Sqrt(21/(2.*Pi))*x*z)/2.;
break;
case 2101 :
ex = (Sqrt(21/(2.*Pi))*y*z)/2.;
break;
case 3101 :
ex = -(Sqrt(21/(2.*Pi))*(-1 + 2*Power(x,2) + 2*Power(y,2) + Power(z,2)))/2.;
break;
case 1110 :
ex = -Sqrt(3/(2.*Pi))/2.;
break;
case 2110 :
ex = 0;
break;
case 3110 :
ex = 0;
break;
case 1111 :
ex = (Sqrt(21/Pi)*(-1 + Power(x,2) + 2*Power(y,2) + 2*Power(z,2)))/4.;
break;
case 2111 :
ex = -(Sqrt(21/Pi)*x*y)/4.;
break;
case 3111 :
ex = -(Sqrt(21/Pi)*x*z)/4.;
break;
case 1200 :
ex = -(Sqrt(5/(2.*Pi))*x)/2.;
break;
case 2200 :
ex = -(Sqrt(5/(2.*Pi))*y)/2.;
break;
case 3200 :
ex = Sqrt(5/(2.*Pi))*z;
break;
case 1201 :
ex = (3*Sqrt(15/(2.*Pi))*x*(-1 + Power(x,2) + Power(y,2) + \
3*Power(z,2)))/4.;
break;
case 2201 :
ex = (3*Sqrt(15/(2.*Pi))*y*(-1 + Power(x,2) + Power(y,2) + \
3*Power(z,2)))/4.;
break;
case 3201 :
ex = (-3*Sqrt(15/(2.*Pi))*z*(-1 + 2*Power(x,2) + 2*Power(y,2) + \
Power(z,2)))/2.;
break;
case 1210 :
ex = -(Sqrt(15/Pi)*z)/4.;
break;
case 2210 :
ex = 0;
break;
case 3210 :
ex = -(Sqrt(15/Pi)*x)/4.;
break;
case 1211 :
ex = (3*Sqrt(5/Pi)*z*(-3 + Power(x,2) + 5*Power(y,2) + 5*Power(z,2)))/8.;
break;
case 2211 :
ex = (-3*Sqrt(5/Pi)*x*y*z)/2.;
break;
case 3211 :
ex = (3*Sqrt(5/Pi)*x*(-3 + 5*Power(x,2) + 5*Power(y,2) + Power(z,2)))/8.;
break;
case 1220 :
ex = (Sqrt(15/Pi)*x)/4.;
break;
case 2220 :
ex = -(Sqrt(15/Pi)*y)/4.;
break;
case 3220 :
ex = 0;
break;
case 1221 :
ex = (-3*Sqrt(5/Pi)*x*(-3 + 3*Power(x,2) + 7*Power(y,2) + 5*Power(z,2)))/8.;
break;
case 2221 :
ex = (3*Sqrt(5/Pi)*y*(-3 + 7*Power(x,2) + 3*Power(y,2) + 5*Power(z,2)))/8.;
break;
case 3221 :
ex = (3*Sqrt(5/Pi)*(Power(x,2) - Power(y,2))*z)/4.;
break;
case 1300 :
ex = -(Sqrt(21/Pi)*x*z)/2.;
break;
case 2300 :
ex = -(Sqrt(21/Pi)*y*z)/2.;
break;
case 3300 :
ex = -(Sqrt(21/Pi)*(Power(x,2) + Power(y,2) - 2*Power(z,2)))/4.;
break;
case 1301 :
ex = (Sqrt(231/Pi)*x*z*(-4 + 3*Power(x,2) + 3*Power(y,2) + \
8*Power(z,2)))/8.;
break;
case 2301 :
ex = (Sqrt(231/Pi)*y*z*(-4 + 3*Power(x,2) + 3*Power(y,2) + \
8*Power(z,2)))/8.;
break;
case 3301 :
ex = (Sqrt(231/Pi)*(3*Power(x,4) + 3*Power(y,4) + Power(x,2)*(-2 + \
6*Power(y,2) - 6*Power(z,2)) - 4*Power(z,2)*(-1 + Power(z,2)) - \
2*Power(y,2)*(1 + 3*Power(z,2))))/8.;
break;
case 1310 :
ex = (Sqrt(7/Pi)*(3*Power(x,2) + Power(y,2) - 4*Power(z,2)))/8.;
break;
case 2310 :
ex = (Sqrt(7/Pi)*x*y)/4.;
break;
case 3310 :
ex = -(Sqrt(7/Pi)*x*z);
break;
case 1311 :
ex = -(Sqrt(77/Pi)*(6*Power(x,4) + 3*Power(y,4) + 8*Power(z,2) - \
12*Power(z,4) - Power(y,2)*(2 + 9*Power(z,2)) + Power(x,2)*(-6 + \
9*Power(y,2) + 9*Power(z,2))))/16.;
break;
case 2311 :
ex = -(Sqrt(77/Pi)*x*y*(-4 + 3*Power(x,2) + 3*Power(y,2) + \
18*Power(z,2)))/16.;
break;
case 3311 :
ex = (Sqrt(77/Pi)*x*z*(-16 + 27*Power(x,2) + 27*Power(y,2) + \
12*Power(z,2)))/16.;
break;
case 1320 :
ex = (Sqrt(35/(2.*Pi))*x*z)/2.;
break;
case 2320 :
ex = -(Sqrt(35/(2.*Pi))*y*z)/2.;
break;
case 3320 :
ex = (Sqrt(35/(2.*Pi))*(Power(x,2) - Power(y,2)))/4.;
break;
case 1321 :
ex = -(Sqrt(385/(2.*Pi))*x*z*(-4 + 3*Power(x,2) + 9*Power(y,2) + \
6*Power(z,2)))/8.;
break;
case 2321 :
ex = (Sqrt(385/(2.*Pi))*y*z*(-4 + 9*Power(x,2) + 3*Power(y,2) + \
6*Power(z,2)))/8.;
break;
case 3321 :
ex = (Sqrt(385/(2.*Pi))*(2*Power(x,2) - 3*Power(x,4) - 2*Power(y,2) + \
3*Power(y,4)))/8.;
break;
case 1330 :
ex = (Sqrt(105/Pi)*(-Power(x,2) + Power(y,2)))/8.;
break;
case 2330 :
ex = (Sqrt(105/Pi)*x*y)/4.;
break;
case 3330 :
ex = 0;
break;
case 1331 :
ex = (Sqrt(1155/Pi)*(2*Power(x,4) + Power(y,2)*(2 - 3*Power(y,2) - \
3*Power(z,2)) + Power(x,2)*(-2 + 3*Power(y,2) + 3*Power(z,2))))/16.;
break;
case 2331 :
ex = -(Sqrt(1155/Pi)*x*y*(-4 + 7*Power(x,2) + 3*Power(y,2) + \
6*Power(z,2)))/16.;
break;
case 3331 :
ex = -(Sqrt(1155/Pi)*x*(Power(x,2) - 3*Power(y,2))*z)/16.;
break;
case 1400 :
ex = (9*x*(Power(x,2) + Power(y,2) - 4*Power(z,2)))/(8.*Sqrt(Pi));
break;
case 2400 :
ex = (9*y*(Power(x,2) + Power(y,2) - 4*Power(z,2)))/(8.*Sqrt(Pi));
break;
case 3400 :
ex = (3*z*(-3*Power(x,2) - 3*Power(y,2) + 2*Power(z,2)))/(2.*Sqrt(Pi));
break;
case 1401 :
ex = (-3*Sqrt(65/Pi)*x*(3*Power(x,4) + 3*Power(y,4) + 4*Power(z,2)*(3 - \
5*Power(z,2)) - 3*Power(y,2)*(1 + Power(z,2)) + Power(x,2)*(6*Power(y,2) - \
3*(1 + Power(z,2)))))/16.;
break;
case 2401 :
ex = (-3*Sqrt(65/Pi)*y*(3*Power(x,4) + 3*Power(y,4) + 4*Power(z,2)*(3 - \
5*Power(z,2)) - 3*Power(y,2)*(1 + Power(z,2)) + Power(x,2)*(6*Power(y,2) - \
3*(1 + Power(z,2)))))/16.;
break;
case 3401 :
ex = (-3*Sqrt(65/Pi)*z*(-9*Power(x,4) - 9*Power(y,4) + 4*Power(z,2)*(-1 + \
Power(z,2)) + 2*Power(y,2)*(3 + Power(z,2)) + 2*Power(x,2)*(3 - 9*Power(y,2) \
+ Power(z,2))))/8.;
break;
case 1410 :
ex = (3*Sqrt(5/Pi)*z*(9*Power(x,2) + 3*Power(y,2) - 4*Power(z,2)))/16.;
break;
case 2410 :
ex = (9*Sqrt(5/Pi)*x*y*z)/8.;
break;
case 3410 :
ex = (9*Sqrt(5/Pi)*x*(Power(x,2) + Power(y,2) - 4*Power(z,2)))/16.;
break;
case 1411 :
ex = (-3*Sqrt(13/Pi)*z*(39*Power(x,4) + 21*Power(y,4) + 4*Power(z,2)*(5 - \
7*Power(z,2)) - Power(y,2)*(15 + 7*Power(z,2)) + Power(x,2)*(-45 + \
60*Power(y,2) + 67*Power(z,2))))/32.;
break;
case 2411 :
ex = (-3*Sqrt(13/Pi)*x*y*z*(-15 + 9*Power(x,2) + 9*Power(y,2) + \
37*Power(z,2)))/16.;
break;
case 3411 :
ex = (-3*Sqrt(13/Pi)*x*(21*Power(x,4) + 21*Power(y,4) + 60*Power(z,2) - \
52*Power(z,4) + 3*Power(x,2)*(-5 + 14*Power(y,2) - 29*Power(z,2)) - \
3*Power(y,2)*(5 + 29*Power(z,2))))/32.;
break;
case 1420 :
ex = (-3*Sqrt(5/(2.*Pi))*x*(Power(x,2) - 3*Power(z,2)))/4.;
break;
case 2420 :
ex = (3*Sqrt(5/(2.*Pi))*y*(Power(y,2) - 3*Power(z,2)))/4.;
break;
case 3420 :
ex = (9*Sqrt(5/(2.*Pi))*(Power(x,2) - Power(y,2))*z)/4.;
break;
case 1421 :
ex = (3*Sqrt(13/(2.*Pi))*x*(5*Power(x,4) + 2*Power(y,4) - \
33*Power(y,2)*Power(z,2) + 3*Power(z,2)*(5 - 7*Power(z,2)) + Power(x,2)*(-5 \
+ 7*Power(y,2) - 2*Power(z,2))))/8.;
break;
case 2421 :
ex = (-3*Sqrt(13/(2.*Pi))*y*(2*Power(x,4) + 5*Power(y,4) + \
Power(x,2)*(7*Power(y,2) - 33*Power(z,2)) + 3*Power(z,2)*(5 - 7*Power(z,2)) \
- Power(y,2)*(5 + 2*Power(z,2))))/8.;
break;
case 3421 :
ex = (-3*Sqrt(13/(2.*Pi))*(Power(x,2) - Power(y,2))*z*(-15 + 23*Power(x,2) + \
23*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 1430 :
ex = (-9*Sqrt(35/Pi)*(Power(x,2) - Power(y,2))*z)/16.;
break;
case 2430 :
ex = (9*Sqrt(35/Pi)*x*y*z)/8.;
break;
case 3430 :
ex = (-3*Sqrt(35/Pi)*x*(Power(x,2) - 3*Power(y,2)))/16.;
break;
case 1431 :
ex = (3*Sqrt(91/Pi)*z*(13*Power(x,4) - 3*Power(y,2)*(-5 + 7*Power(y,2) + \
7*Power(z,2)) + 3*Power(x,2)*(-5 + 8*Power(y,2) + 7*Power(z,2))))/32.;
break;
case 2431 :
ex = (-3*Sqrt(91/Pi)*x*y*z*(-15 + 25*Power(x,2) + 9*Power(y,2) + \
21*Power(z,2)))/16.;
break;
case 3431 :
ex = (3*Sqrt(91/Pi)*x*(Power(x,2) - 3*Power(y,2))*(-5 + 7*Power(x,2) + \
7*Power(y,2) - Power(z,2)))/32.;
break;
case 1440 :
ex = (3*Sqrt(35/(2.*Pi))*x*(Power(x,2) - 3*Power(y,2)))/8.;
break;
case 2440 :
ex = (3*Sqrt(35/(2.*Pi))*y*(-3*Power(x,2) + Power(y,2)))/8.;
break;
case 3440 :
ex = 0;
break;
case 1441 :
ex = (-3*Sqrt(91/(2.*Pi))*x*(5*Power(x,4) + Power(y,2)*(15 - 23*Power(y,2) - \
21*Power(z,2)) + Power(x,2)*(-5 - 2*Power(y,2) + 7*Power(z,2))))/16.;
break;
case 2441 :
ex = (3*Sqrt(91/(2.*Pi))*y*(23*Power(x,4) + Power(y,2)*(5 - 5*Power(y,2) - \
7*Power(z,2)) + Power(x,2)*(-15 + 2*Power(y,2) + 21*Power(z,2))))/16.;
break;
case 3441 :
ex = (3*Sqrt(91/(2.*Pi))*(Power(x,4) - 6*Power(x,2)*Power(y,2) + \
Power(y,4))*z)/8.;
break;
default:
fprintf(stderr,"not implemented");
break;
}
return ex;

}

double Imp(int j, int m, int n, int i, double x, double y, double z)
{
	double ex;
	int code;
        double Pi=3.141592653589793;

	code = i*1000+j*100+m*10+n;

/* Im p */
switch (code) {
case 1110 :
ex = 0;
break;
case 2110 :
ex = -Sqrt(3/(2.*Pi))/2.;
break;
case 3110 :
ex = 0;
break;
case 1111 :
ex = -(Sqrt(21/Pi)*x*y)/4.;
break;
case 2111 :
ex = (Sqrt(21/Pi)*(-1 + 2*Power(x,2) + Power(y,2) + 2*Power(z,2)))/4.;
break;
case 3111 :
ex = -(Sqrt(21/Pi)*y*z)/4.;
break;
case 1210 :
ex = 0;
break;
case 2210 :
ex = -(Sqrt(15/Pi)*z)/4.;
break;
case 3210 :
ex = -(Sqrt(15/Pi)*y)/4.;
break;
case 1211 :
ex = (-3*Sqrt(5/Pi)*x*y*z)/2.;
break;
case 2211 :
ex = (3*Sqrt(5/Pi)*z*(-3 + 5*Power(x,2) + Power(y,2) + 5*Power(z,2)))/8.;
break;
case 3211 :
ex = (3*Sqrt(5/Pi)*y*(-3 + 5*Power(x,2) + 5*Power(y,2) + Power(z,2)))/8.;
break;
case 1220 :
ex = (Sqrt(15/Pi)*y)/4.;
break;
case 2220 :
ex = (Sqrt(15/Pi)*x)/4.;
break;
case 3220 :
ex = 0;
break;
case 1221 :
ex = (-3*Sqrt(5/Pi)*y*(-3 + Power(x,2) + 5*Power(y,2) + 5*Power(z,2)))/8.;
break;
case 2221 :
ex = (-3*Sqrt(5/Pi)*x*(-3 + 5*Power(x,2) + Power(y,2) + 5*Power(z,2)))/8.;
break;
case 3221 :
ex = (3*Sqrt(5/Pi)*x*y*z)/2.;
break;
case 1310 :
ex = (Sqrt(7/Pi)*x*y)/4.;
break;
case 2310 :
ex = (Sqrt(7/Pi)*(Power(x,2) + 3*Power(y,2) - 4*Power(z,2)))/8.;
break;
case 3310 :
ex = -(Sqrt(7/Pi)*y*z);
break;
case 1311 :
ex = -(Sqrt(77/Pi)*x*y*(-4 + 3*Power(x,2) + 3*Power(y,2) + \
18*Power(z,2)))/16.;
break;
case 2311 :
ex = -(Sqrt(77/Pi)*(3*Power(x,4) + 6*Power(y,4) + 8*Power(z,2) - \
12*Power(z,4) + Power(x,2)*(-2 + 9*Power(y,2) - 9*Power(z,2)) + \
Power(y,2)*(-6 + 9*Power(z,2))))/16.;
break;
case 3311 :
ex = (Sqrt(77/Pi)*y*z*(-16 + 27*Power(x,2) + 27*Power(y,2) + \
12*Power(z,2)))/16.;
break;
case 1320 :
ex = (Sqrt(35/(2.*Pi))*y*z)/2.;
break;
case 2320 :
ex = (Sqrt(35/(2.*Pi))*x*z)/2.;
break;
case 3320 :
ex = (Sqrt(35/(2.*Pi))*x*y)/2.;
break;
case 1321 :
ex = -(Sqrt(385/(2.*Pi))*y*z*(-2 + 3*Power(y,2) + 3*Power(z,2)))/4.;
break;
case 2321 :
ex = -(Sqrt(385/(2.*Pi))*x*z*(-2 + 3*Power(x,2) + 3*Power(z,2)))/4.;
break;
case 3321 :
ex = -(Sqrt(385/(2.*Pi))*x*y*(-2 + 3*Power(x,2) + 3*Power(y,2)))/4.;
break;
case 1330 :
ex = -(Sqrt(105/Pi)*x*y)/4.;
break;
case 2330 :
ex = (Sqrt(105/Pi)*(-Power(x,2) + Power(y,2)))/8.;
break;
case 3330 :
ex = 0;
break;
case 1331 :
ex = (Sqrt(1155/Pi)*x*y*(-4 + 3*Power(x,2) + 7*Power(y,2) + \
6*Power(z,2)))/16.;
break;
case 2331 :
ex = (Sqrt(1155/Pi)*(3*Power(x,4) + Power(y,2)*(2 - 2*Power(y,2) - \
3*Power(z,2)) + Power(x,2)*(-2 - 3*Power(y,2) + 3*Power(z,2))))/16.;
break;
case 3331 :
ex = (Sqrt(1155/Pi)*y*(-3*Power(x,2) + Power(y,2))*z)/16.;
break;
case 1410 :
ex = (9*Sqrt(5/Pi)*x*y*z)/8.;
break;
case 2410 :
ex = (3*Sqrt(5/Pi)*z*(3*Power(x,2) + 9*Power(y,2) - 4*Power(z,2)))/16.;
break;
case 3410 :
ex = (9*Sqrt(5/Pi)*y*(Power(x,2) + Power(y,2) - 4*Power(z,2)))/16.;
break;
case 1411 :
ex = (-3*Sqrt(13/Pi)*x*y*z*(-15 + 9*Power(x,2) + 9*Power(y,2) + \
37*Power(z,2)))/16.;
break;
case 2411 :
ex = (-3*Sqrt(13/Pi)*z*(21*Power(x,4) + 39*Power(y,4) + 4*Power(z,2)*(5 - \
7*Power(z,2)) + Power(x,2)*(-15 + 60*Power(y,2) - 7*Power(z,2)) + \
Power(y,2)*(-45 + 67*Power(z,2))))/32.;
break;
case 3411 :
ex = (-3*Sqrt(13/Pi)*y*(21*Power(x,4) + 21*Power(y,4) + 60*Power(z,2) - \
52*Power(z,4) + 3*Power(x,2)*(-5 + 14*Power(y,2) - 29*Power(z,2)) - \
3*Power(y,2)*(5 + 29*Power(z,2))))/32.;
break;
case 1420 :
ex = (-3*Sqrt(5/(2.*Pi))*y*(3*Power(x,2) + Power(y,2) - 6*Power(z,2)))/8.;
break;
case 2420 :
ex = (-3*Sqrt(5/(2.*Pi))*x*(Power(x,2) + 3*Power(y,2) - 6*Power(z,2)))/8.;
break;
case 3420 :
ex = (9*Sqrt(5/(2.*Pi))*x*y*z)/2.;
break;
case 1421 :
ex = (3*Sqrt(13/(2.*Pi))*y*(13*Power(x,4) + 7*Power(y,4) + 6*Power(z,2)*(5 - \
7*Power(z,2)) - 5*Power(y,2)*(1 + 7*Power(z,2)) + Power(x,2)*(-15 + \
20*Power(y,2) + 27*Power(z,2))))/16.;
break;
case 2421 :
ex = (3*Sqrt(13/(2.*Pi))*x*(7*Power(x,4) + 13*Power(y,4) + 6*Power(z,2)*(5 - \
7*Power(z,2)) + 5*Power(x,2)*(-1 + 4*Power(y,2) - 7*Power(z,2)) + \
3*Power(y,2)*(-5 + 9*Power(z,2))))/16.;
break;
case 3421 :
ex = (-3*Sqrt(13/(2.*Pi))*x*y*z*(-15 + 23*Power(x,2) + 23*Power(y,2) + \
9*Power(z,2)))/4.;
break;
case 1430 :
ex = (-9*Sqrt(35/Pi)*x*y*z)/8.;
break;
case 2430 :
ex = (-9*Sqrt(35/Pi)*(Power(x,2) - Power(y,2))*z)/16.;
break;
case 3430 :
ex = (3*Sqrt(35/Pi)*y*(-3*Power(x,2) + Power(y,2)))/16.;
break;
case 1431 :
ex = (3*Sqrt(91/Pi)*x*y*z*(-15 + 9*Power(x,2) + 25*Power(y,2) + \
21*Power(z,2)))/16.;
break;
case 2431 :
ex = (3*Sqrt(91/Pi)*z*(21*Power(x,4) + Power(y,2)*(15 - 13*Power(y,2) - \
21*Power(z,2)) - 3*Power(x,2)*(5 + 8*Power(y,2) - 7*Power(z,2))))/32.;
break;
case 3431 :
ex = (-3*Sqrt(91/Pi)*y*(-3*Power(x,2) + Power(y,2))*(-5 + 7*Power(x,2) + \
7*Power(y,2) - Power(z,2)))/32.;
break;
case 1440 :
ex = (-3*Sqrt(35/(2.*Pi))*y*(-3*Power(x,2) + Power(y,2)))/8.;
break;
case 2440 :
ex = (3*Sqrt(35/(2.*Pi))*x*(Power(x,2) - 3*Power(y,2)))/8.;
break;
case 3440 :
ex = 0;
break;
case 1441 :
ex = (-3*Sqrt(91/(2.*Pi))*y*(13*Power(x,4) + Power(y,2)*(5 - 7*Power(y,2) - \
7*Power(z,2)) + Power(x,2)*(-15 + 22*Power(y,2) + 21*Power(z,2))))/16.;
break;
case 2441 :
ex = (-3*Sqrt(91/(2.*Pi))*x*(7*Power(x,4) + Power(y,2)*(15 - 13*Power(y,2) - \
21*Power(z,2)) + Power(x,2)*(-5 - 22*Power(y,2) + 7*Power(z,2))))/16.;
break;
case 3441 :
ex = (3*Sqrt(91/(2.*Pi))*x*y*(Power(x,2) - Power(y,2))*z)/2.;
break;
default:
fprintf(stderr,"not implemented");
break;
}
return ex;

}

double Req(int j, int m, int n, int i, double x, double y, double z)
{
	double ex;
	int code;
        double Pi=3.141592653589793;

	code = i*1000+j*100+m*10+n;

/* Re q */
switch (code) {
case 1100 :
ex = (Sqrt(15/(2.*Pi))*y)/2.;
break;
case 2100 :
ex = -(Sqrt(15/(2.*Pi))*x)/2.;
break;
case 3100 :
ex = 0;
break;
case 1101 :
ex = (3*Sqrt(3/(2.*Pi))*y*(-5 + 7*Power(x,2) + 7*Power(y,2) + \
7*Power(z,2)))/4.;
break;
case 2101 :
ex = (-3*Sqrt(3/(2.*Pi))*x*(-5 + 7*Power(x,2) + 7*Power(y,2) + \
7*Power(z,2)))/4.;
break;
case 3101 :
ex = 0;
break;
case 1110 :
ex = 0;
break;
case 2110 :
ex = -(Sqrt(15/Pi)*z)/4.;
break;
case 3110 :
ex = (Sqrt(15/Pi)*y)/4.;
break;
case 1111 :
ex = 0;
break;
case 2111 :
ex = (-3*Sqrt(3/Pi)*z*(-5 + 7*Power(x,2) + 7*Power(y,2) + 7*Power(z,2)))/8.;
break;
case 3111 :
ex = (3*Sqrt(3/Pi)*y*(-5 + 7*Power(x,2) + 7*Power(y,2) + 7*Power(z,2)))/8.;
break;
case 1200 :
ex = (Sqrt(105/(2.*Pi))*y*z)/2.;
break;
case 2200 :
ex = -(Sqrt(105/(2.*Pi))*x*z)/2.;
break;
case 3200 :
ex = 0;
break;
case 1201 :
ex = (Sqrt(165/(2.*Pi))*y*z*(-7 + 9*Power(x,2) + 9*Power(y,2) + \
9*Power(z,2)))/4.;
break;
case 2201 :
ex = -(Sqrt(165/(2.*Pi))*x*z*(-7 + 9*Power(x,2) + 9*Power(y,2) + \
9*Power(z,2)))/4.;
break;
case 3201 :
ex = 0;
break;
case 1210 :
ex = -(Sqrt(35/Pi)*x*y)/4.;
break;
case 2210 :
ex = (Sqrt(35/Pi)*(Power(x,2) - Power(z,2)))/4.;
break;
case 3210 :
ex = (Sqrt(35/Pi)*y*z)/4.;
break;
case 1211 :
ex = -(Sqrt(55/Pi)*x*y*(-7 + 9*Power(x,2) + 9*Power(y,2) + \
9*Power(z,2)))/8.;
break;
case 2211 :
ex = (Sqrt(55/Pi)*(Power(x,2) - Power(z,2))*(-7 + 9*Power(x,2) + \
9*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 3211 :
ex = (Sqrt(55/Pi)*y*z*(-7 + 9*Power(x,2) + 9*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 1220 :
ex = (Sqrt(35/Pi)*y*z)/4.;
break;
case 2220 :
ex = (Sqrt(35/Pi)*x*z)/4.;
break;
case 3220 :
ex = -(Sqrt(35/Pi)*x*y)/2.;
break;
case 1221 :
ex = (Sqrt(55/Pi)*y*z*(-7 + 9*Power(x,2) + 9*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 2221 :
ex = (Sqrt(55/Pi)*x*z*(-7 + 9*Power(x,2) + 9*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 3221 :
ex = -(Sqrt(55/Pi)*x*y*(-7 + 9*Power(x,2) + 9*Power(y,2) + \
9*Power(z,2)))/4.;
break;
case 1300 :
ex = (-3*Sqrt(21/Pi)*y*(Power(x,2) + Power(y,2) - 4*Power(z,2)))/8.;
break;
case 2300 :
ex = (3*Sqrt(21/Pi)*x*(Power(x,2) + Power(y,2) - 4*Power(z,2)))/8.;
break;
case 3300 :
ex = 0;
break;
case 1301 :
ex = (Sqrt(273/Pi)*y*(-11*Power(x,4) - 11*Power(y,4) + 4*Power(z,2)*(-9 + \
11*Power(z,2)) + Power(y,2)*(9 + 33*Power(z,2)) + Power(x,2)*(9 - \
22*Power(y,2) + 33*Power(z,2))))/16.;
break;
case 2301 :
ex = (Sqrt(273/Pi)*x*(11*Power(x,4) + 11*Power(y,4) + 36*Power(z,2) - \
44*Power(z,4) + Power(x,2)*(-9 + 22*Power(y,2) - 33*Power(z,2)) - \
3*Power(y,2)*(3 + 11*Power(z,2))))/16.;
break;
case 3301 :
ex = 0;
break;
case 1310 :
ex = (-15*Sqrt(7/Pi)*x*y*z)/8.;
break;
case 2310 :
ex = (3*Sqrt(7/Pi)*z*(11*Power(x,2) + Power(y,2) - 4*Power(z,2)))/16.;
break;
case 3310 :
ex = (-3*Sqrt(7/Pi)*y*(Power(x,2) + Power(y,2) - 4*Power(z,2)))/16.;
break;
case 1311 :
ex = (-5*Sqrt(91/Pi)*x*y*z*(-9 + 11*Power(x,2) + 11*Power(y,2) + \
11*Power(z,2)))/16.;
break;
case 2311 :
ex = (Sqrt(91/Pi)*z*(121*Power(x,4) + 11*Power(y,4) + 36*Power(z,2) - \
44*Power(z,4) + 11*Power(x,2)*(-9 + 12*Power(y,2) + 7*Power(z,2)) - \
3*Power(y,2)*(3 + 11*Power(z,2))))/32.;
break;
case 3311 :
ex = (Sqrt(91/Pi)*y*(-11*Power(x,4) - 11*Power(y,4) + 4*Power(z,2)*(-9 + \
11*Power(z,2)) + Power(y,2)*(9 + 33*Power(z,2)) + Power(x,2)*(9 - \
22*Power(y,2) + 33*Power(z,2))))/32.;
break;
case 1320 :
ex = (3*Sqrt(35/(2.*Pi))*y*(Power(x,2) - Power(y,2) + 2*Power(z,2)))/8.;
break;
case 2320 :
ex = (-3*Sqrt(35/(2.*Pi))*x*(Power(x,2) - Power(y,2) - 2*Power(z,2)))/8.;
break;
case 3320 :
ex = (-3*Sqrt(35/(2.*Pi))*x*y*z)/2.;
break;
case 1321 :
ex = (Sqrt(455/(2.*Pi))*y*(11*Power(x,4) - 11*Power(y,4) + 2*Power(z,2)*(-9 \
+ 11*Power(z,2)) + Power(y,2)*(9 + 11*Power(z,2)) + Power(x,2)*(-9 + \
33*Power(z,2))))/16.;
break;
case 2321 :
ex = (Sqrt(455/(2.*Pi))*x*(-11*Power(x,4) + 11*Power(y,4) + 2*Power(z,2)*(-9 \
+ 11*Power(z,2)) + Power(x,2)*(9 + 11*Power(z,2)) + Power(y,2)*(-9 + \
33*Power(z,2))))/16.;
break;
case 3321 :
ex = -(Sqrt(455/(2.*Pi))*x*y*z*(-9 + 11*Power(x,2) + 11*Power(y,2) + \
11*Power(z,2)))/4.;
break;
case 1330 :
ex = (-3*Sqrt(105/Pi)*x*y*z)/8.;
break;
case 2330 :
ex = (-3*Sqrt(105/Pi)*(Power(x,2) - Power(y,2))*z)/16.;
break;
case 3330 :
ex = (-3*Sqrt(105/Pi)*y*(-3*Power(x,2) + Power(y,2)))/16.;
break;
case 1331 :
ex = -(Sqrt(1365/Pi)*x*y*z*(-9 + 11*Power(x,2) + 11*Power(y,2) + \
11*Power(z,2)))/16.;
break;
case 2331 :
ex = -(Sqrt(1365/Pi)*(Power(x,2) - Power(y,2))*z*(-9 + 11*Power(x,2) + \
11*Power(y,2) + 11*Power(z,2)))/32.;
break;
case 3331 :
ex = -(Sqrt(1365/Pi)*y*(-3*Power(x,2) + Power(y,2))*(-9 + 11*Power(x,2) + \
11*Power(y,2) + 11*Power(z,2)))/32.;
break;
case 1400 :
ex = (-3*Sqrt(55/Pi)*y*z*(3*Power(x,2) + 3*Power(y,2) - 4*Power(z,2)))/8.;
break;
case 2400 :
ex = (3*Sqrt(55/Pi)*x*z*(3*Power(x,2) + 3*Power(y,2) - 4*Power(z,2)))/8.;
break;
case 3400 :
ex = 0;
break;
case 1401 :
ex = (-15*Sqrt(3/Pi)*y*z*(39*Power(x,4) + 39*Power(y,4) + 44*Power(z,2) - \
52*Power(z,4) + Power(x,2)*(-33 + 78*Power(y,2) - 13*Power(z,2)) - \
Power(y,2)*(33 + 13*Power(z,2))))/16.;
break;
case 2401 :
ex = (15*Sqrt(3/Pi)*x*z*(39*Power(x,4) + 39*Power(y,4) + 44*Power(z,2) - \
52*Power(z,4) + Power(x,2)*(-33 + 78*Power(y,2) - 13*Power(z,2)) - \
Power(y,2)*(33 + 13*Power(z,2))))/16.;
break;
case 3401 :
ex = 0;
break;
case 1410 :
ex = (9*Sqrt(11/Pi)*x*y*(Power(x,2) + Power(y,2) - 6*Power(z,2)))/16.;
break;
case 2410 :
ex = (-3*Sqrt(11/Pi)*(3*Power(x,4) - 3*Power(y,2)*Power(z,2) + 4*Power(z,4) \
+ 3*Power(x,2)*(Power(y,2) - 7*Power(z,2))))/16.;
break;
case 3410 :
ex = (-3*Sqrt(11/Pi)*y*z*(3*Power(x,2) + 3*Power(y,2) - 4*Power(z,2)))/16.;
break;
case 1411 :
ex = (9*Sqrt(15/Pi)*x*y*(13*Power(x,4) + 13*Power(y,4) + 66*Power(z,2) - \
78*Power(z,4) + Power(x,2)*(-11 + 26*Power(y,2) - 65*Power(z,2)) - \
Power(y,2)*(11 + 65*Power(z,2))))/32.;
break;
case 2411 :
ex = (-3*Sqrt(15/Pi)*(39*Power(x,6) + 3*Power(x,4)*(-11 + 26*Power(y,2) - \
78*Power(z,2)) + Power(z,2)*(-39*Power(y,4) - 44*Power(z,2) + 52*Power(z,4) \
+ Power(y,2)*(33 + 13*Power(z,2))) + Power(x,2)*(39*Power(y,4) + \
231*Power(z,2) - 221*Power(z,4) - 3*Power(y,2)*(11 + 91*Power(z,2)))))/32.;
break;
case 3411 :
ex = (-3*Sqrt(15/Pi)*y*z*(39*Power(x,4) + 39*Power(y,4) + 44*Power(z,2) - \
52*Power(z,4) + Power(x,2)*(-33 + 78*Power(y,2) - 13*Power(z,2)) - \
Power(y,2)*(33 + 13*Power(z,2))))/32.;
break;
case 1420 :
ex = (3*Sqrt(11/(2.*Pi))*y*z*(3*Power(x,2) - 4*Power(y,2) + \
3*Power(z,2)))/4.;
break;
case 2420 :
ex = (-3*Sqrt(11/(2.*Pi))*x*z*(4*Power(x,2) - 3*(Power(y,2) + \
Power(z,2))))/4.;
break;
case 3420 :
ex = (3*Sqrt(11/(2.*Pi))*x*y*(Power(x,2) + Power(y,2) - 6*Power(z,2)))/4.;
break;
case 1421 :
ex = (3*Sqrt(15/(2.*Pi))*y*z*(39*Power(x,4) - 52*Power(y,4) - 33*Power(z,2) \
+ 39*Power(z,4) + Power(y,2)*(44 - 13*Power(z,2)) + Power(x,2)*(-33 - \
13*Power(y,2) + 78*Power(z,2))))/8.;
break;
case 2421 :
ex = (-3*Sqrt(15/(2.*Pi))*x*z*(52*Power(x,4) - 39*Power(y,4) + 33*Power(z,2) \
- 39*Power(z,4) + Power(y,2)*(33 - 78*Power(z,2)) + Power(x,2)*(-44 + \
13*Power(y,2) + 13*Power(z,2))))/8.;
break;
case 3421 :
ex = (3*Sqrt(15/(2.*Pi))*x*y*(13*Power(x,4) + 13*Power(y,4) + 66*Power(z,2) \
- 78*Power(z,4) + Power(x,2)*(-11 + 26*Power(y,2) - 65*Power(z,2)) - \
Power(y,2)*(11 + 65*Power(z,2))))/8.;
break;
case 1430 :
ex = (-3*Sqrt(77/Pi)*x*y*(Power(x,2) - 3*Power(y,2) + 6*Power(z,2)))/16.;
break;
case 2430 :
ex = (3*Sqrt(77/Pi)*(Power(x,4) + 3*Power(y,2)*Power(z,2) - \
3*Power(x,2)*(Power(y,2) + Power(z,2))))/16.;
break;
case 3430 :
ex = (-9*Sqrt(77/Pi)*y*(-3*Power(x,2) + Power(y,2))*z)/16.;
break;
case 1431 :
ex = (-3*Sqrt(105/Pi)*x*y*(13*Power(x,4) - 39*Power(y,4) - 66*Power(z,2) + \
78*Power(z,4) + Power(y,2)*(33 + 39*Power(z,2)) + Power(x,2)*(-11 - \
26*Power(y,2) + 91*Power(z,2))))/32.;
break;
case 2431 :
ex = (3*Sqrt(105/Pi)*(13*Power(x,6) + 3*Power(y,2)*Power(z,2)*(-11 + \
13*Power(y,2) + 13*Power(z,2)) - Power(x,4)*(11 + 26*Power(y,2) + \
26*Power(z,2)) + Power(x,2)*(-39*Power(y,4) + 33*Power(z,2) - 39*Power(z,4) \
+ Power(y,2)*(33 - 39*Power(z,2)))))/32.;
break;
case 3431 :
ex = (-9*Sqrt(105/Pi)*y*(-3*Power(x,2) + Power(y,2))*z*(-11 + 13*Power(x,2) \
+ 13*Power(y,2) + 13*Power(z,2)))/32.;
break;
case 1440 :
ex = (-3*Sqrt(77/(2.*Pi))*y*(-3*Power(x,2) + Power(y,2))*z)/8.;
break;
case 2440 :
ex = (3*Sqrt(77/(2.*Pi))*x*(Power(x,2) - 3*Power(y,2))*z)/8.;
break;
case 3440 :
ex = (-3*Sqrt(77/(2.*Pi))*x*y*(Power(x,2) - Power(y,2)))/2.;
break;
case 1441 :
ex = (-3*Sqrt(105/(2.*Pi))*y*(-3*Power(x,2) + Power(y,2))*z*(-11 + \
13*Power(x,2) + 13*Power(y,2) + 13*Power(z,2)))/16.;
break;
case 2441 :
ex = (3*Sqrt(105/(2.*Pi))*x*(Power(x,2) - 3*Power(y,2))*z*(-11 + \
13*Power(x,2) + 13*Power(y,2) + 13*Power(z,2)))/16.;
break;
case 3441 :
ex = (-3*Sqrt(105/(2.*Pi))*x*y*(Power(x,2) - Power(y,2))*(-11 + \
13*Power(x,2) + 13*Power(y,2) + 13*Power(z,2)))/4.;
break;
default:
fprintf(stderr,"not implemented");
break;
}
return ex;

}

double Imq(int j, int m, int n, int i, double x, double y, double z)
{
	double ex;
	int code;
        double Pi=3.141592653589793;

	code = i*1000+j*100+m*10+n;

/* Im q */
switch (code) {
case 1110 :
ex = (Sqrt(15/Pi)*z)/4.;
break;
case 2110 :
ex = 0;
break;
case 3110 :
ex = -(Sqrt(15/Pi)*x)/4.;
break;
case 1111 :
ex = (3*Sqrt(3/Pi)*z*(-5 + 7*Power(x,2) + 7*Power(y,2) + 7*Power(z,2)))/8.;
break;
case 2111 :
ex = 0;
break;
case 3111 :
ex = (-3*Sqrt(3/Pi)*x*(-5 + 7*Power(x,2) + 7*Power(y,2) + 7*Power(z,2)))/8.;
break;
case 1210 :
ex = (Sqrt(35/Pi)*(-Power(y,2) + Power(z,2)))/4.;
break;
case 2210 :
ex = (Sqrt(35/Pi)*x*y)/4.;
break;
case 3210 :
ex = -(Sqrt(35/Pi)*x*z)/4.;
break;
case 1211 :
ex = -(Sqrt(55/Pi)*(Power(y,2) - Power(z,2))*(-7 + 9*Power(x,2) + \
9*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 2211 :
ex = (Sqrt(55/Pi)*x*y*(-7 + 9*Power(x,2) + 9*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 3211 :
ex = -(Sqrt(55/Pi)*x*z*(-7 + 9*Power(x,2) + 9*Power(y,2) + \
9*Power(z,2)))/8.;
break;
case 1220 :
ex = -(Sqrt(35/Pi)*x*z)/4.;
break;
case 2220 :
ex = (Sqrt(35/Pi)*y*z)/4.;
break;
case 3220 :
ex = (Sqrt(35/Pi)*(Power(x,2) - Power(y,2)))/4.;
break;
case 1221 :
ex = -(Sqrt(55/Pi)*x*z*(-7 + 9*Power(x,2) + 9*Power(y,2) + \
9*Power(z,2)))/8.;
break;
case 2221 :
ex = (Sqrt(55/Pi)*y*z*(-7 + 9*Power(x,2) + 9*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 3221 :
ex = (Sqrt(55/Pi)*(Power(x,2) - Power(y,2))*(-7 + 9*Power(x,2) + \
9*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 1310 :
ex = (-3*Sqrt(7/Pi)*z*(Power(x,2) + 11*Power(y,2) - 4*Power(z,2)))/16.;
break;
case 2310 :
ex = (15*Sqrt(7/Pi)*x*y*z)/8.;
break;
case 3310 :
ex = (3*Sqrt(7/Pi)*x*(Power(x,2) + Power(y,2) - 4*Power(z,2)))/16.;
break;
case 1311 :
ex = (Sqrt(91/Pi)*z*(-11*Power(x,4) - 121*Power(y,4) + Power(y,2)*(99 - \
77*Power(z,2)) + 4*Power(z,2)*(-9 + 11*Power(z,2)) + Power(x,2)*(9 - \
132*Power(y,2) + 33*Power(z,2))))/32.;
break;
case 2311 :
ex = (5*Sqrt(91/Pi)*x*y*z*(-9 + 11*Power(x,2) + 11*Power(y,2) + \
11*Power(z,2)))/16.;
break;
case 3311 :
ex = (Sqrt(91/Pi)*x*(11*Power(x,4) + 11*Power(y,4) + 36*Power(z,2) - \
44*Power(z,4) + Power(x,2)*(-9 + 22*Power(y,2) - 33*Power(z,2)) - \
3*Power(y,2)*(3 + 11*Power(z,2))))/32.;
break;
case 1320 :
ex = (3*Sqrt(35/(2.*Pi))*x*(Power(y,2) - Power(z,2)))/4.;
break;
case 2320 :
ex = (-3*Sqrt(35/(2.*Pi))*y*(Power(x,2) - Power(z,2)))/4.;
break;
case 3320 :
ex = (3*Sqrt(35/(2.*Pi))*(Power(x,2) - Power(y,2))*z)/4.;
break;
case 1321 :
ex = (Sqrt(455/(2.*Pi))*x*(Power(y,2) - Power(z,2))*(-9 + 11*Power(x,2) + \
11*Power(y,2) + 11*Power(z,2)))/8.;
break;
case 2321 :
ex = -(Sqrt(455/(2.*Pi))*y*(Power(x,2) - Power(z,2))*(-9 + 11*Power(x,2) + \
11*Power(y,2) + 11*Power(z,2)))/8.;
break;
case 3321 :
ex = (Sqrt(455/(2.*Pi))*(Power(x,2) - Power(y,2))*z*(-9 + 11*Power(x,2) + \
11*Power(y,2) + 11*Power(z,2)))/8.;
break;
case 1330 :
ex = (3*Sqrt(105/Pi)*(Power(x,2) - Power(y,2))*z)/16.;
break;
case 2330 :
ex = (-3*Sqrt(105/Pi)*x*y*z)/8.;
break;
case 3330 :
ex = (-3*Sqrt(105/Pi)*x*(Power(x,2) - 3*Power(y,2)))/16.;
break;
case 1331 :
ex = (Sqrt(1365/Pi)*(Power(x,2) - Power(y,2))*z*(-9 + 11*Power(x,2) + \
11*Power(y,2) + 11*Power(z,2)))/32.;
break;
case 2331 :
ex = -(Sqrt(1365/Pi)*x*y*z*(-9 + 11*Power(x,2) + 11*Power(y,2) + \
11*Power(z,2)))/16.;
break;
case 3331 :
ex = -(Sqrt(1365/Pi)*x*(Power(x,2) - 3*Power(y,2))*(-9 + 11*Power(x,2) + \
11*Power(y,2) + 11*Power(z,2)))/32.;
break;
case 1410 :
ex = (3*Sqrt(11/Pi)*(3*Power(y,4) - 21*Power(y,2)*Power(z,2) + 4*Power(z,4) \
+ 3*Power(x,2)*(Power(y,2) - Power(z,2))))/16.;
break;
case 2410 :
ex = (-9*Sqrt(11/Pi)*x*y*(Power(x,2) + Power(y,2) - 6*Power(z,2)))/16.;
break;
case 3410 :
ex = (3*Sqrt(11/Pi)*x*z*(3*Power(x,2) + 3*Power(y,2) - 4*Power(z,2)))/16.;
break;
case 1411 :
ex = (3*Sqrt(15/Pi)*(39*Power(y,6) - 44*Power(z,4) + 52*Power(z,6) + \
39*Power(x,4)*(Power(y,2) - Power(z,2)) - 3*Power(y,4)*(11 + 78*Power(z,2)) \
+ Power(y,2)*(231*Power(z,2) - 221*Power(z,4)) + Power(x,2)*(78*Power(y,4) + \
Power(z,2)*(33 + 13*Power(z,2)) - 3*Power(y,2)*(11 + 91*Power(z,2)))))/32.;
break;
case 2411 :
ex = (-9*Sqrt(15/Pi)*x*y*(13*Power(x,4) + 13*Power(y,4) + 66*Power(z,2) - \
78*Power(z,4) + Power(x,2)*(-11 + 26*Power(y,2) - 65*Power(z,2)) - \
Power(y,2)*(11 + 65*Power(z,2))))/32.;
break;
case 3411 :
ex = (3*Sqrt(15/Pi)*x*z*(39*Power(x,4) + 39*Power(y,4) + 44*Power(z,2) - \
52*Power(z,4) + Power(x,2)*(-33 + 78*Power(y,2) - 13*Power(z,2)) - \
Power(y,2)*(33 + 13*Power(z,2))))/32.;
break;
case 1420 :
ex = (3*Sqrt(11/(2.*Pi))*x*z*(Power(x,2) + 15*Power(y,2) - \
6*Power(z,2)))/8.;
break;
case 2420 :
ex = (-3*Sqrt(11/(2.*Pi))*y*z*(15*Power(x,2) + Power(y,2) - \
6*Power(z,2)))/8.;
break;
case 3420 :
ex = (-3*Sqrt(11/(2.*Pi))*(Power(x,2) - Power(y,2))*(Power(x,2) + Power(y,2) \
- 6*Power(z,2)))/8.;
break;
case 1421 :
ex = (3*Sqrt(15/(2.*Pi))*x*z*(13*Power(x,4) + Power(x,2)*(-11 + \
208*Power(y,2) - 65*Power(z,2)) + 3*(65*Power(y,4) + 22*Power(z,2) - \
26*Power(z,4) + Power(y,2)*(-55 + 39*Power(z,2)))))/16.;
break;
case 2421 :
ex = (-3*Sqrt(15/(2.*Pi))*y*z*(195*Power(x,4) + 13*Power(y,4) + \
66*Power(z,2) - 78*Power(z,4) - Power(y,2)*(11 + 65*Power(z,2)) + \
Power(x,2)*(-165 + 208*Power(y,2) + 117*Power(z,2))))/16.;
break;
case 3421 :
ex = (-3*Sqrt(15/(2.*Pi))*(Power(x,2) - Power(y,2))*(13*Power(x,4) + \
13*Power(y,4) + 66*Power(z,2) - 78*Power(z,4) + Power(x,2)*(-11 + \
26*Power(y,2) - 65*Power(z,2)) - Power(y,2)*(11 + 65*Power(z,2))))/16.;
break;
case 1430 :
ex = (3*Sqrt(77/Pi)*(Power(y,4) - 3*Power(y,2)*Power(z,2) - \
3*Power(x,2)*(Power(y,2) - Power(z,2))))/16.;
break;
case 2430 :
ex = (3*Sqrt(77/Pi)*x*y*(3*Power(x,2) - Power(y,2) - 6*Power(z,2)))/16.;
break;
case 3430 :
ex = (-9*Sqrt(77/Pi)*x*(Power(x,2) - 3*Power(y,2))*z)/16.;
break;
case 1431 :
ex = (-3*Sqrt(105/Pi)*(39*Power(x,4)*(Power(y,2) - Power(z,2)) + \
Power(y,2)*(-13*Power(y,4) - 33*Power(z,2) + 39*Power(z,4) + Power(y,2)*(11 \
+ 26*Power(z,2))) + Power(x,2)*(26*Power(y,4) + 33*Power(z,2) - \
39*Power(z,4) + Power(y,2)*(-33 + 39*Power(z,2)))))/32.;
break;
case 2431 :
ex = (3*Sqrt(105/Pi)*x*y*(39*Power(x,4) - 13*Power(y,4) + 66*Power(z,2) - \
78*Power(z,4) + Power(y,2)*(11 - 91*Power(z,2)) + Power(x,2)*(-33 + \
26*Power(y,2) - 39*Power(z,2))))/32.;
break;
case 3431 :
ex = (-9*Sqrt(105/Pi)*x*(Power(x,2) - 3*Power(y,2))*z*(-11 + 13*Power(x,2) + \
13*Power(y,2) + 13*Power(z,2)))/32.;
break;
case 1440 :
ex = (-3*Sqrt(77/(2.*Pi))*x*(Power(x,2) - 3*Power(y,2))*z)/8.;
break;
case 2440 :
ex = (-3*Sqrt(77/(2.*Pi))*y*(-3*Power(x,2) + Power(y,2))*z)/8.;
break;
case 3440 :
ex = (3*Sqrt(77/(2.*Pi))*(Power(x,4) - 6*Power(x,2)*Power(y,2) + \
Power(y,4)))/8.;
break;
case 1441 :
ex = (-3*Sqrt(105/(2.*Pi))*x*(Power(x,2) - 3*Power(y,2))*z*(-11 + \
13*Power(x,2) + 13*Power(y,2) + 13*Power(z,2)))/16.;
break;
case 2441 :
ex = (-3*Sqrt(105/(2.*Pi))*y*(-3*Power(x,2) + Power(y,2))*z*(-11 + \
13*Power(x,2) + 13*Power(y,2) + 13*Power(z,2)))/16.;
break;
case 3441 :
ex = (3*Sqrt(105/(2.*Pi))*(Power(x,4) - 6*Power(x,2)*Power(y,2) + \
Power(y,4))*(-11 + 13*Power(x,2) + 13*Power(y,2) + 13*Power(z,2)))/16.;
break;
default:
fprintf(stderr,"not implemented");
break;
}
return ex;

}

void makeAT(int n, int m)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
           pointList.AT[i][j]=pointList.A[j][i];
        }
     }
}


void makeATA(int n, int m)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<m;j++){
           pointList.ATA[i][j]=0.;
           for(int k=0;k<n;k++){
              pointList.ATA[i][j]=pointList.ATA[i][j]+pointList.AT[i][k]*pointList.A[k][j];
           }
        }
     }
}


void makeATY(int n, int m,int wh)
{
     for(int i=0;i<m;i++){
           pointList.ATY[i]=0.;
           for(int k=0;k<n;k++){
               switch (wh) {
                  case 1 :
                     pointList.ATY[i]=pointList.ATY[i]+pointList.AT[i][k]*pointList.Y[k][0];
                     break;
				 case 2 :
                     pointList.ATY[i]=pointList.ATY[i]+pointList.AT[i][k]*pointList.Y[k][1];
                     break;
				 case 3 :
                     pointList.ATY[i]=pointList.ATY[i]+pointList.AT[i][k]*pointList.Y[k][2];
                     break;
                  
               }
           }
     }
}



void solveA(int n, int m)
{
    double faktor;
    bool ok=true;

    for(int i=1;i<m;i++){
       for(int j=i;j<m;j++){
          if(fabs(pointList.ATA[j][i-1])>0.){
             faktor=pointList.ATA[i-1][i-1]/pointList.ATA[j][i-1];
             for(int k=0;k<m;k++){
                pointList.ATA[j][k]=pointList.ATA[i-1][k]-faktor*pointList.ATA[j][k];
             }
             pointList.ATY[j]=pointList.ATY[i-1]-faktor*pointList.ATY[j];
          }
       }
    }
    for(int i=m-1;i>-1;i--){
       for(int j=i+1;j<m;j++){
          pointList.ATY[i]=pointList.ATY[i]-pointList.ATA[i][j]*pointList.X[j];
       }
       if(fabs(pointList.ATA[i][i])>0.){
          pointList.X[i]=pointList.ATY[i]/pointList.ATA[i][i];
       }
       else{
          ok=false;
       }
    }
}




void make_A_ij(double centerX,double centerY,double centerZ,double radius){

    //double dx,dy,dz,dist,u,v,w,vel;
	//int counter,howmany,step;
    //find out how many points there are
	/*howmany=0;
	for(int j=1;j<pointList.point[0][0][0];j++){
        dx=(pointList.point[0][j][2]-centerX)/radius;
        dy=(pointList.point[0][j][3]-centerY)/radius;
        dz=(pointList.point[0][j][4]-centerZ)/radius;

		u=pointList.point[0][j][5];
        v=pointList.point[0][j][6];
        w=pointList.point[0][j][7];
	    dist=pow(dx*dx+dy*dy+dz*dz,0.5);
		vel=pow(u*u+v*v+w*w,0.5);
		if(dist<1 && !(vel==0)){
			howmany=howmany+3;
		}
	}
	//find points that are within radius to center, compute \widetiled{A_{ij}}
    counter=0;
	if(howmany<500){
		step=1;
	}
	else{
        step=(int)((double)howmany/500.+0.5);
	}
	for(int j=1;j<pointList.point[0][0][0];j=j+step){
        dx=(pointList.point[0][j][2]-centerX)/radius;
        dy=(pointList.point[0][j][3]-centerY)/radius;
        dz=(pointList.point[0][j][4]-centerZ)/radius;

		u=pointList.point[0][j][5];
        v=pointList.point[0][j][6];
        w=pointList.point[0][j][7];
	    dist=pow(dx*dx+dy*dy+dz*dz,0.5);
		vel=pow(u*u+v*v+w*w,0.5);
		if(dist<1 && !(vel==0) && counter<500){
			for (int compo=1;compo<4;compo++){
                 pointList.A[counter][ 0]=Imp(1,1,0,compo,dx,dy,dz);//ip110
                 pointList.A[counter][ 1]=Imp(2,1,0,compo,dx,dy,dz);//ip210
                 pointList.A[counter][ 2]=Imp(2,2,0,compo,dx,dy,dz);//ip220

                 pointList.A[counter][ 3]=Imq(1,1,0,compo,dx,dy,dz);//iq110

                 pointList.A[counter][ 4]=Rep(1,0,0,compo,dx,dy,dz);//rp100
                 pointList.A[counter][ 5]=Rep(1,1,0,compo,dx,dy,dz);//rp110
                 pointList.A[counter][ 6]=Rep(2,0,0,compo,dx,dy,dz);//rp200
                 pointList.A[counter][ 7]=Rep(2,1,0,compo,dx,dy,dz);//rp210
                 pointList.A[counter][ 8]=Rep(2,2,0,compo,dx,dy,dz);//rp220

                 pointList.A[counter][ 9]=Req(1,0,0,compo,dx,dy,dz);//rq100
                 pointList.A[counter][10]=Req(1,1,0,compo,dx,dy,dz);//rq110

                 pointList.Y[counter]=pointList.point[0][j][4+compo]/radius;
                 counter++;
            }
		}
	}
	if(counter>35){//21 enough to get A_ij? 2 times over determined
        makeAT(counter,11);
        makeATA(counter,11);
        makeATY(counter,11,1);
        solveA(counter,11);
            
        pointList.dudx[0]=0.31539156525252005*(-1.4142135623730951*pointList.X[6]+1.7320508075688772*pointList.X[8]);
        pointList.dudx[1]=0.5462742152960396*(pointList.X[2] + 1.4142135623730951*pointList.X[9]);
        pointList.dudx[2]=0.5462742152960396*(pointList.X[3] - 1.*pointList.X[7]);
        pointList.dudx[3]=0.5462742152960396*(pointList.X[2] - 1.4142135623730951*pointList.X[9]);
        pointList.dudx[4]=-0.31539156525252005*(1.4142135623730951*pointList.X[6] + 1.7320508075688772*pointList.X[8]);
        pointList.dudx[5]=-0.5462742152960396*(pointList.X[1] + pointList.X[10]);
        pointList.dudx[6]=-0.5462742152960396*(pointList.X[3] + pointList.X[7]);
        pointList.dudx[7]=0.5462742152960396*(-1.*pointList.X[1] + pointList.X[10]);
        pointList.dudx[8]=0.8920620580763856*pointList.X[6];
	}
	else{
        pointList.dudx[0]=0;
        pointList.dudx[1]=0;
        pointList.dudx[2]=0;
        pointList.dudx[3]=0;
        pointList.dudx[4]=0;
        pointList.dudx[5]=0;
        pointList.dudx[6]=0;
        pointList.dudx[7]=0;
        pointList.dudx[8]=0;
	}*/

}



static void compute_statistics(int start, int end){
	int count=0;
	int length;
	int frame_ind,point_ind;
	pointList.num_traj=0;
	pointList.av_traj_length=0;
	pointList.max_traj_length=0;
	pointList.min_traj_length=1000;

	for(int i=start;i<end;i=i++){
        for(int j=0;j<pointList.numPointsPerFrame[i];j++){
			//check if new traj starts
			if(pointList.pointAddress[i][j][0]<0 && pointList.pointAddress[i][j][1]>-1){
				length=1;
				pointList.num_traj++; 
				frame_ind=i;
				point_ind=j;
				while(pointList.pointAddress[frame_ind][point_ind][1]>-1 && frame_ind<end){
                     point_ind=pointList.pointAddress[frame_ind][point_ind][1];
					 frame_ind++;
					 length++;
				}
				pointList.av_traj_length=pointList.av_traj_length+length;
				pointList.rms_traj_length=pointList.rms_traj_length+length*length;
				if(length>pointList.max_traj_length){
                    pointList.max_traj_length=length;
				}
				if(length<pointList.min_traj_length){
                    pointList.min_traj_length=length;
				}
			}
		}
	}
	pointList.links_per_frame=(int)((double)pointList.vel_count/(double)(end-start+1));
	pointList.av_traj_length=(int)((double)pointList.av_traj_length/(double)pointList.num_traj);
    pointList.rms_traj_length=(int)(pow((double)pointList.rms_traj_length/(double)pointList.num_traj,0.5));
    pointList.num_glues=(int)((double)pointList.num_glues/(double((end-start+1))));
}

static void getVel(int frame,int i){
    
	bool has_left=false;
	bool has_right=false;
	int im,ip;
	double u,v,w,velSq;
	
    u=0.;v=0.;w=0.;
	if(pointList.pointAddress[frame][i][0]>-1){
		has_left=true;
	    im=pointList.pointAddress[frame][i][0];
	}
    if(pointList.pointAddress[frame][i][1]>-1){
		has_right=true;
	    ip=pointList.pointAddress[frame][i][1];
	}
	if(has_left && has_right){
		u=1./2./pointList.delta_t*
			(pointList.pointPos[frame+1][ip][0]-pointList.pointPos[frame-1][im][0]);
		v=1./2./pointList.delta_t*
			(pointList.pointPos[frame+1][ip][1]-pointList.pointPos[frame-1][im][1]);
		w=1./2./pointList.delta_t*
			(pointList.pointPos[frame+1][ip][2]-pointList.pointPos[frame-1][im][2]);
	}
	else{
		if(has_left){
			u=1./pointList.delta_t*
			   (pointList.pointPos[frame][i][0]-pointList.pointPos[frame-1][im][0]);
			v=1./pointList.delta_t*
			   (pointList.pointPos[frame][i][1]-pointList.pointPos[frame-1][im][1]);
			w=1./pointList.delta_t*
			   (pointList.pointPos[frame][i][2]-pointList.pointPos[frame-1][im][2]);
		}
		if(has_right){
			u=1./pointList.delta_t*
			   (pointList.pointPos[frame+1][ip][0]-pointList.pointPos[frame][i][0]);
			v=1./pointList.delta_t*
			   (pointList.pointPos[frame+1][ip][1]-pointList.pointPos[frame][i][1]);
			w=1./pointList.delta_t*
			   (pointList.pointPos[frame+1][ip][2]-pointList.pointPos[frame][i][2]);
		}
	}

	velSq= pow(u,2.)+pow(v,2.)+pow(w,2.);
    if(velSq>0){
	    pointList.rms_u=pointList.rms_u+velSq;
	    pointList.vel_count=pointList.vel_count+1;
	}
	if(pow(velSq,0.5)>pointList.max_meas_vel){
        pointList.max_meas_vel=pow(velSq,0.5);
	}
}




double mod(double x, double y){
	
	double res;
	if(x/y>0){
	    res=(x/y-(double)((long)(x/y)))*y;
	}else{
        res=y+(x/y-(double)((long)(x/y)))*y;
	}
	
	return res;
}


static void write_traj(int start, int end){

    FILE * traj;
	char TRAJ[256];
	int trajectory[1000][2];
	int ind,point_ind,frame_ind;
	int c;

	for(int i=start;i<end;i++){
		if(pointList.output){
		    if(mod((double)i,(double)100)==0){
		        cout << "done writing ASCII trajectories of frame: ........."<<10000+i<<"\n";
		    }
		}
		if(pointList.track){
			c=sprintf (TRAJ, pointList.experiment);
	        c+=sprintf (TRAJ+c, "/new_traj.%5d", 10000+i);
		}
		else{
            c=sprintf (TRAJ, pointList.experiment);
	        c+=sprintf (TRAJ+c, "/old_traj.%5d", 10000+i);
		}
		traj  = fopen(TRAJ,"w");
		for(int j=0;j<pointList.numPointsPerFrame[i];j++){
			if(pointList.pointAddress[i][j][0]<0 && pointList.pointAddress[i][j][1]>-1){
                point_ind=j;
				frame_ind=i;
				ind=0;
				trajectory[ind][0]=frame_ind;
				trajectory[ind][1]=point_ind;
				while(pointList.pointAddress[frame_ind][point_ind][1]>-1 && frame_ind<end){
					point_ind=pointList.pointAddress[frame_ind][point_ind][1];
					frame_ind++;
					ind++;
					trajectory[ind][0]=frame_ind;
					trajectory[ind][1]=point_ind;
				}
				if(ind>3){ //e.g. if at least 5 connected points
					for(int k=0;k<ind;k++){
                        fprintf(traj, "%i\t", k);
						fprintf(traj, "%i\t", trajectory[k][0]);
						fprintf(traj, "%i\t", trajectory[k][1]);
		                fprintf(traj, "%f\t", pointList.pointPos[trajectory[k][0]][trajectory[k][1]][0]);
		                fprintf(traj, "%f\t", pointList.pointPos[trajectory[k][0]][trajectory[k][1]][1]);
		                fprintf(traj, "%f\n", pointList.pointPos[trajectory[k][0]][trajectory[k][1]][2]); 
					}
				}
			}
		}
		fclose(traj);
	}
}

static void write_ASCII(int start, int end){

    FILE * ptv_is;
	char PTV_IS[256];
	int c;

	for(int i=0;i<end;i++){
		c=sprintf (PTV_IS, pointList.experiment);
	    c+=sprintf (PTV_IS+c, "/ptv_is.%5d", 10000+i);
	    ptv_is  = fopen(PTV_IS,"w");
		fprintf(ptv_is, "%i\n", pointList.numPointsPerFrame[i]);
	    for (int j=0;j<pointList.numPointsPerFrame[i];j++){
			fprintf(ptv_is, "%i\t", pointList.pointAddress[i][j][0]);
			fprintf(ptv_is, "%i\t", pointList.pointAddress[i][j][1]);
		    fprintf(ptv_is, "%f\t", pointList.pointPos[i][j][0]);
		    fprintf(ptv_is, "%f\t", pointList.pointPos[i][j][1]);
		    fprintf(ptv_is, "%f\n", pointList.pointPos[i][j][2]);
		}
		fclose(ptv_is);
	}
	
}

static void write_binary(int start,int end){

    float x,y,z;
	short int left,right,c;

    char filename1[256];
    char filename2[256];
	FILE *fPtsOutput, *fLinkOutput;
	
	c=sprintf (filename1, pointList.experiment);
	c+=sprintf (filename1+c, "_new.pt3");
	c=sprintf (filename2, pointList.experiment);
	c+=sprintf (filename2+c, "_new.pln");

    fPtsOutput  = fopen(filename1,"wb");
    fLinkOutput = fopen(filename2,"wb");

    for(int i=start;i<end;i++){
        fwrite(&pointList.numPointsPerFrame[i],sizeof(int),1,fPtsOutput);
        for(int j=0;j<pointList.numPointsPerFrame[i];j++){
		    x=(float)(1000.*pointList.pointPos[i][j][0]);
			y=(float)(1000.*pointList.pointPos[i][j][1]);
			z=(float)(1000.*pointList.pointPos[i][j][2]);
            fwrite(&x,sizeof(float),1,fPtsOutput);
            fwrite(&y,sizeof(float),1,fPtsOutput);
            fwrite(&z,sizeof(float),1,fPtsOutput);
        }
        fwrite(&pointList.numPointsPerFrame[i],sizeof(int),1,fLinkOutput);
        for(int j=0;j<pointList.numPointsPerFrame[i];j++){
			left=(short int)pointList.pointAddress[i][j][0];
            fwrite(&left,sizeof(short int),1,fLinkOutput);
        }
        for(int j=0;j<pointList.numPointsPerFrame[i];j++){
			right=(short int)pointList.pointAddress[i][j][1];
            fwrite(&right,sizeof(short int),1,fLinkOutput);
        }
    }
    fclose(fPtsOutput);
    fclose(fLinkOutput);

}



