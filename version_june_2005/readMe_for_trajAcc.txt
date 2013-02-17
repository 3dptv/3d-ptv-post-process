Readme for trajAcc format from June 2005

Each file trajAcc.xxx contains all trajectories that begin at frame number xxx.


                  for(int ii=0;ii<numInTraj-2*pm;ii++){
                     fprintf(fpp, "%lf\t", xp[ii]);//1
                     fprintf(fpp, "%lf\t", yp[ii]);//2
                     fprintf(fpp, "%lf\t", zp[ii]);//3
                     fprintf(fpp, "%lf\t", up[ii]);//4
                     fprintf(fpp, "%lf\t", vp[ii]);//5
                     fprintf(fpp, "%lf\t", wp[ii]);//6
                     fprintf(fpp, "%lf\t", axp[ii]);//7
                     fprintf(fpp, "%lf\t", ayp[ii]);//8
                     fprintf(fpp, "%lf\t", azp[ii]);//9
                     
                     fprintf(fpp, "%lf\t", dudxp[ii]);//10
                     fprintf(fpp, "%lf\t", dudyp[ii]);//11
                     fprintf(fpp, "%lf\t", dudzp[ii]);//12
                     fprintf(fpp, "%lf\t", dvdxp[ii]);//13
                     fprintf(fpp, "%lf\t", dvdyp[ii]);//14
                     fprintf(fpp, "%lf\t", dvdzp[ii]);//15
                     fprintf(fpp, "%lf\t", dwdxp[ii]);//16
                     fprintf(fpp, "%lf\t", dwdyp[ii]);//17
                     fprintf(fpp, "%lf\t", dwdzp[ii]);//18
                     fprintf(fpp, "%lf\t", dudtp[ii]);//19
                     fprintf(fpp, "%lf\t", dvdtp[ii]);//20
                     fprintf(fpp, "%lf\t", dwdtp[ii]);//21

                     fprintf(fpp, "%lf\t", daxdxp[ii]);//22
                     fprintf(fpp, "%lf\t", daxdyp[ii]);//23
                     fprintf(fpp, "%lf\t", daxdzp[ii]);//24
                     fprintf(fpp, "%lf\t", daydxp[ii]);//25
                     fprintf(fpp, "%lf\t", daydyp[ii]);//26
                     fprintf(fpp, "%lf\t", daydzp[ii]);//27
                     fprintf(fpp, "%lf\t", dazdxp[ii]);//28
                     fprintf(fpp, "%lf\t", dazdyp[ii]);//29
                     fprintf(fpp, "%lf\t", dazdzp[ii]);//30

                     fprintf(fpp, "%lf\t", NXp[ii]);//31
                     fprintf(fpp, "%lf\t", NYp[ii]);//32
                     fprintf(fpp, "%lf\t", NZp[ii]);//33

                     fprintf(fpp, "%lf\n", (double)(ii));//34
  