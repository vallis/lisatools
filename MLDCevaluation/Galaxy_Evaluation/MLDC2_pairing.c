/*
 gcc MLDC2_pairing.c template.c AEnoise.c dfour1.c nrutil2.c -lm -o MLDC2_pairing
 */
/***************************  REQUIRED LIBRARIES  ***************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>


/*********************  DEFINITION OF GLOBAL VARIABLES  *********************/

#include "Constants.h"

/*************  PROTOTYPE DECLARATIONS FOR INTERNAL FUNCTIONS  **************/

#include "nrutil2.h"

void template(double *params, long NP, double *hA, double *hE, long NMAX, long *hM, long *hp, double Sn);

void KILL(char*);

double cross_correlation(double *h1A, double *h1E, double *h2A, double *h2E, long N, long M, int bs);
double nwip(double *a, double *b, long N, double Sn);
double AEnoise(double f);

/* ============================  MAIN PROGRAM  ============================ */

int main(int argc, char* argv[])
{
  /*   Variable declaration   */
  int i, j, k, m, n, J;
  int NP, NMAX;
  int *count, **bin_pop;
  int source, source_match;
  int i_match, j_match, k_match;
  int cc5, cc6, cc7, cc8, cc9;
  int *bc5, *bc6, *bc7, *bc8, *bc9;
  int binshift;
  
  long NI, NR;
  long q, ip, rp, M, Bi;
  long MT, MK;
  int npbin, binw;
  
  double f, theta, phi, Amp, psi, iota, phase, SNR;
  double d_f, d_theta, d_phi, d_Amp, d_psi, d_iota, d_phase;
  double Sn;
  double *params;
  double **rparams, **iparams;
  double *rA, *rE, *iA, *iE, *dA, *dE;
  double **nwr, **corr, **fmatch;
  double *ranges, *del;
  double min_nwr;
  double **histos;
  double **phist;
  int bin, bins, bcnt;
  int flag;
  long qmax;
  long tmax;
  double bnw;
  char Gfile[100];
    
  FILE *ifile, *sfile, *outfile, *hfile;
  FILE *bfile1, *bfile2, *bfile3, *bfile4;
    
  if(argc!=4) KILL("to run:  ./MLDC2_pairing keyfile sourcefile nametag\n");

  /*  open key file  */
  ifile = fopen(argv[1], "r");
  
  /* open source parameter file   */
  sfile = fopen(argv[2], "r");


  sprintf(Gfile, "%s_report_1.txt", argv[3]);
  bfile1 = fopen(Gfile,"w");
  sprintf(Gfile, "%s_report_2.txt", argv[3]);
  bfile2 = fopen(Gfile,"w");
  sprintf(Gfile, "%s_report_3.txt", argv[3]);
  bfile3 = fopen(Gfile,"w");
  sprintf(Gfile, "%s_report_4.txt", argv[3]);
  bfile4 = fopen(Gfile,"w");
  
  printf("read in data files\n");
  
  binw = 6;
  npbin = 8;
  NP = 7;  
  NMAX = 8192;
  bins = 400;
  bnw = 2.0/(double)bins;
  qmax = 0;
  tmax = 0;

  histos = dmatrix(1,2,1,bins);
  phist = dmatrix(0,NP,1,bins);
  ranges = dvector(0,NP);
  del = dvector(1,NP);
  bc5 = ivector(1,4);
  bc6 = ivector(1,4);
  bc7 = ivector(1,4);
  bc8 = ivector(1,4);
  bc9 = ivector(1,4);

  for(i=1; i<=bins; i++)
   {
    histos[1][i] = 0.0;
    histos[2][i] = 0.0;
    for(j=0; j<=NP; j++) phist[j][i] = 0.0;
    }


  for(i=1; i<=4; i++)
  {
    bc5[i] = bc6[i] = bc7[i] = bc8[i] = bc9[i] = 0;
  }

  /*  determine number of sources injected  */ 
  NI=0;
  while(!feof(ifile)) 
  {
    NI++;
    fscanf(ifile,"%lf %lf %lf %lf %lf %lf %lf",&f, &theta, &phi, &Amp, &iota, &psi, &phase);
    q = (long)floor(f*T);
    if(q > tmax) tmax = q;
  }
  rewind(ifile);
  NI--;    
  printf("number of sources injected:  %i\n",NI);  
  printf("highest populated injected bin:  %i\n",tmax); 

  tmax -= binw;
  
  /*  determine number of sources recovered   */
  NR=0;
  while(!feof(sfile)) 
  {
    NR++;
    fscanf(sfile,"%ld %lf %lf %lf %lf %lf %lf %lf",&i, &f, &theta, &phi, &Amp, &psi, &iota, &phase);
    q = (long)floor(f*T);
    if(q > qmax) qmax = q;
  }
  rewind(sfile);
  NR--;    
  printf("number of sources recovered:  %i\n",NR);  
  printf("highest populated recovered bin:  %i\n",qmax); 

  if(qmax > tmax) qmax = tmax;

  qmax += 2*binw;
  
  /*  allocate memory  */
  count   = ivector(1,qmax);
  iA      = dvector(1,2*NMAX);
  iE      = dvector(1,2*NMAX);
  rA      = dvector(1,2*NMAX);
  rE      = dvector(1,2*NMAX);
  dA      = dvector(1,2*NMAX);
  dE      = dvector(1,2*NMAX);
  params  = dvector(1,NP);
  bin_pop = imatrix(1,qmax,1,npbin);
  iparams = dmatrix(1,NI,0,NP);
  rparams = dmatrix(1,NR,0,NP);
  nwr     = dmatrix(1,2*binw+1,1,npbin);
  corr    = dmatrix(1,2*binw+1,1,npbin);
  fmatch  = dmatrix(1,2*binw+1,1,npbin);
  

  /*  store injected sources in array  */
  printf("storing injected source parameters\n");
  for(i=1; i<=NI; i++)
  {
    fscanf(ifile,"%lf %lf %lf %lf %lf %lf %lf",&f, &theta, &phi, &Amp, &iota, &psi, &phase);
    iparams[i][1] = f;
    iparams[i][2] = pi/2.0-theta;
    iparams[i][3] = phi;
    iparams[i][4] = Amp;
    iparams[i][5] = iota;
    iparams[i][6] = psi;
    iparams[i][7] = phase;
    
    // remove degeneracy in phase and polarization
    if(iparams[i][7] >= pi)
    {
      iparams[i][7] -= pi;
      iparams[i][6] += pi/2.0;
    }
    while(iparams[i][6] > pi) iparams[i][6] -= pi;
  }
  rewind(ifile);
  
  printf("storing recovered source parameters\n");
  /* store recovered sources in array  */
  for(i=1; i<=NR; i++)
  {
    fscanf(sfile,"%ld %lf %lf %lf %lf %lf %lf %lf",&j, &f, &theta, &phi, &Amp, &psi, &iota, &phase);
    rparams[i][1] = f;
    rparams[i][2] = pi/2.0-theta;
    rparams[i][3] = phi;
    rparams[i][4] = Amp;
    rparams[i][5] = iota;
    rparams[i][6] = psi;
    rparams[i][7] = phase;
    
    // remove degeneracy in phase and polarization
    if(rparams[i][7] >= pi)
    {
      rparams[i][7] -= pi;
      rparams[i][6] += pi/2.0;
    }
    while(rparams[i][6] > pi) rparams[i][6] -= pi;

  }
  
  rewind(sfile);

  ranges[0] = 4.0;
  ranges[1] = 2.0;
  ranges[2] = pi/2.0;
  ranges[3] = pi/4.0;
  ranges[4] = 3.0;
  ranges[5] = pi/2.0;
  ranges[6] = pi/2.0;
  ranges[7] = pi/2.0;


  /*  find the bin number for each injected source  */
  for(i=1; i<=qmax; i++)
  {
    for(j=1; j<=npbin; j++) bin_pop[i][j] = 0;
    count[i] = 0;
  }
  
  /*  tally the number of injected sources which populate each frequency bin  */
  n=0;
  for(i=1; i<=NI; i++)
  {
    q = (long)floor(iparams[i][1]*T);
    if(q <= qmax)
      {
    count[q]++;
    if(count[q] > npbin) 
    {
      printf("bin %i is overpopulated\n", q);
      n++;
    }
  else 
    {
     bin_pop[q][count[q]] = i;
    }
      }
  }
  printf("total number of overpopulated bins: %i \n", n);

  bcnt = 0;
  
  //open "report card" file
  sprintf(Gfile, "%s_report.dat", argv[3]);
  outfile = fopen(Gfile,"w");

  sprintf(Gfile, "%s_false_positives.dat", argv[3]);
  sfile = fopen(Gfile,"w");

  fprintf(outfile, "| # rec | # inj | nwr | correlation | SNR rec | SNR inj | d_f * T | d_theta | d_phi | d_A/A | d_iota | d_psi | d_phase |\n");
  //printf("| # rec | # inj | nwr | correlation | SNR rec | SNR inj | d_f * T | d_theta | d_phi | d_A | d_iota | d_psi | d_phase |\n");
  
  cc5 = cc6 = cc7 = cc8 = cc9 = 0;
  
  /*  Iterate through list of recovered sources and find best match to injected sources. 
      Recovered sources are matched to whichever source within +/- 2 frequency bins
      yields the lowest noise-weighted residual (h_rec - h_inj | h_rec - h_inj)   */

  // hfile = fopen("h.dat","w");

  for(n=1; n<=NR; n++)
  {
    if(n%1000 == 0) printf("%d sources done\n", n);
    //  select recovered source
    f = rparams[n][1];
    Sn= AEnoise(f);
    q = (long)floor(f*T);
    
    //  form template for recovered source
    for(i=1; i<=NP; i++) params[i] = rparams[n][i];

    /* for(i=1; i<=NP; i++) printf("%e ", params[i]);
       printf("\n"); */

    template(params, NP, rA, rE, 2*NMAX, &MT, &rp, Sn);
    
    //  calculate recovered SNR
    rparams[n][0] = sqrt( nwip(rA, rA, MT, Sn) + nwip(rE, rE, MT, Sn) );

    
    //  look in bins either side of recovered source bin for injected sources  
    m = 0;
    for(k=q-binw; k<=q+binw; k++)
    {
      //  keep track of which of the eligable bins we are looking in
      m++;
      
      //  select the injected sources which live in the current bin
      for(j=1; j<=npbin; j++)
      {
        if(bin_pop[k][j] != 0)
        {                  
          source = bin_pop[k][j];
          f = iparams[source][1];
          Sn= AEnoise(f);

          //  form template for injected source
          for(i=1; i<=NP; i++) params[i] = iparams[source][i];
          template(params, NP, iA, iE, 2*NMAX, &MK, &ip, Sn);

          /* for(i=1; i<=NP; i++) printf("%e ", params[i]);
	     printf("\n"); */

          // printf("%d %d %d\n", n, MT, MK);
          
          //  calculate injected sources SNR
          iparams[source][0] = sqrt( nwip(iA, iA, MK, Sn) + nwip(iE, iE, MK, Sn) );

          //  store offset between injected and recovered templates  
          binshift = ip - rp - MK/2 + MT/2;

          // printf("%d %d %d\n", binshift, ip, rp);
        
          //  calculate noise weighted residual (nwr) by first lining up templates        
          for(i=1; i<=MK; i++)
          {
            if((i+binshift >= 1) && (i+binshift <= MT))
            {
              dA[2*i-1] = rA[2*(i+binshift)-1] - iA[2*i-1];
              dA[2*i]   = rA[2*(i+binshift)]   - iA[2*i];
              dE[2*i-1] = rE[2*(i+binshift)-1] - iE[2*i-1];
              dE[2*i]   = rE[2*(i+binshift)]   - iE[2*i];
              // fprintf(hfile,"%d %e %e %e %e\n", i, iE[2*i-1], iE[2*i], rE[2*(i+binshift)-1], rE[2*(i+binshift)]);
            }else
            {              
              dA[2*i-1] = iA[2*i-1];
              dA[2*i]   = iA[2*i];
              dE[2*i-1] = iE[2*i-1];
              dE[2*i]   = iE[2*i];
            }
            
          }
          nwr[m][j] = nwip(dA, dA, MK, Sn) + nwip(dE, dE, MK, Sn);
          
          //  calculate cross correlation (h1 | h2)/sqrt(h1^2 * h2^2)
          corr[m][j] = cross_correlation(rA, rE, iA, iE, MT, MK, binshift);
          
          //  calculate absolute difference in frequency
          fmatch[m][j] = fabs(rparams[n][1] - iparams[source][1]);
          
        // artificially assign huge nwr value for unmatched sources  
        }else nwr[m][j] = 1.0e22;  
        
      }//end j loop (finding injected sources within a bin)
      
    }// end k loop (moving from bin to bin)
    
    // find inj/rec pair with lowest nwr
    min_nwr = 1.0e21;
    flag = 0;
    for(i=1; i<=(2*binw+1); i++)
    {
      for(j=1; j<=npbin; j++)
      {
        if(nwr[i][j] < min_nwr)
        {
          min_nwr = nwr[i][j];
          i_match = i;
          j_match = j;
          flag = 1;
        }
      }
    }

    if(flag == 1)
      {
    //  determine bin which contained best matched injected source
      k_match = q - binw + (i_match-1);
    //  determine which source within bin
      source_match = bin_pop[k_match][j_match];
      }
    else
      {
	source_match = 0;
        i_match = 1;
        j_match = 1;
        corr[i_match][j_match] = -0.999;
      }

      if(corr[i_match][j_match] < 0.5)
	{
         fprintf(sfile,"%d ", n);
         fprintf(sfile,"%.12lg ",rparams[n][1]);
         fprintf(sfile,"%.12lg ",rparams[n][2]);
         fprintf(sfile,"%.12lg ",rparams[n][3]);
         fprintf(sfile,"%.12lg ",rparams[n][4]);
         fprintf(sfile,"%.12lg ",rparams[n][6]);
         fprintf(sfile,"%.12lg ",rparams[n][5]);
         fprintf(sfile,"%.12lg ",rparams[n][7]);
         fprintf(sfile,"\n");
	}
      
    //  write report card  
    if(source_match > 0)
    {
      fprintf(outfile,"%i %i %lg %lg %lg %lg ",n,source_match,min_nwr,corr[i_match][j_match],rparams[n][0],iparams[source_match][0]);

      del[1] = -(iparams[source_match][1] - rparams[n][1])*T;
      del[2] = -(iparams[source_match][2] - rparams[n][2]);
      del[3] = -(iparams[source_match][3] - rparams[n][3]);
      del[4] = -(iparams[source_match][4] - rparams[n][4])/iparams[source_match][4];
      del[6] = -(iparams[source_match][5] - rparams[n][5]);
      del[5] = -(iparams[source_match][6] - rparams[n][6]);
      del[7] = -(iparams[source_match][7] - rparams[n][7]);

      for(i=1; i<=NP; i++) fprintf(outfile,"%lg ",del[i]);
      fprintf(outfile,"\n");

      for(i=1; i<=NP; i++)
	{
         bin = (int)((double)(bins)*(ranges[i] + del[i])/(2.0*ranges[i]))+1;
         if((bin >0) && (bin <=bins) && (i != 2)) phist[i][bin] += 1.0;
        }

      bin = (int)((double)(bins)*log10(rparams[n][0])/ranges[0]);
      if((bin >0) && (bin <=bins)) phist[0][bin] += 1.0;

      if(fabs(iparams[source_match][3] - rparams[n][3]) > pi/2.0)
	{
	  /* sky flip */
	  bin = (int)((double)(bins)*(ranges[2] + iparams[source_match][2] + rparams[n][2])/(2.0*ranges[2]))+1;
        }
      else
        {
	 bin = (int)((double)(bins)*(ranges[2] + iparams[source_match][2] - rparams[n][2])/(2.0*ranges[2]))+1;
        }
       if((bin >0) && (bin <=bins)) phist[2][bin] += 1.0;

       f = rparams[n][1];

      if(f>=0.2985e-3 && f<=0.3015e-3)
      {  
      fprintf(bfile1,"%i %i %lg %lg %lg %lg ",n,source_match,min_nwr,corr[i_match][j_match],rparams[n][0],iparams[source_match][0]);
      for(i=1; i<=NP; i++) fprintf(bfile1,"%lg ",del[i]);
      fprintf(bfile1,"\n");
      }
      if(f>=0.9985e-3 && f<=1.0015e-3)
      {  
      fprintf(bfile2,"%i %i %lg %lg %lg %lg ",n,source_match,min_nwr,corr[i_match][j_match],rparams[n][0],iparams[source_match][0]);
      for(i=1; i<=NP; i++) fprintf(bfile2,"%lg ",del[i]);
      fprintf(bfile2,"\n");
      }
      if(f>=2.9985e-3 && f<=3.0015e-3)
      {  
      fprintf(bfile3,"%i %i %lg %lg %lg %lg ",n,source_match,min_nwr,corr[i_match][j_match],rparams[n][0],iparams[source_match][0]);
      for(i=1; i<=NP; i++) fprintf(bfile3,"%lg ",del[i]);
      fprintf(bfile3,"\n");
      }
      if(f>=5.9985e-3 && f<=6.0015e-3)
      {  
      fprintf(bfile4,"%i %i %lg %lg %lg %lg ",n,source_match,min_nwr,corr[i_match][j_match],rparams[n][0],iparams[source_match][0]);
      for(i=1; i<=NP; i++) fprintf(bfile4,"%lg ",del[i]);
      fprintf(bfile4,"\n");
      }    

    
      // printf("%i %i %lg %lg %lg %lg ",n,source_match,min_nwr,corr[i_match][j_match],rparams[n][0],iparams[source][0]);
      //for(i=1; i<=NP; i++) printf("%lg ",iparams[source_match][i] - rparams[n][i]);
      //printf("\n");
    }
    //  if there is no injected source in adjacent bins write "no match" on report card
    else
    {

      f = rparams[n][1];
      if(f>=0.2985e-3 && f<=0.3015e-3)
      {  
      fprintf(bfile1,"%i  no match\n",n);
      }
      if(f>=0.9985e-3 && f<=1.0015e-3)
      {  
      fprintf(bfile2,"%i  no match\n",n);
      }
      if(f>=2.9985e-3 && f<=3.0015e-3)
      {  
      fprintf(bfile3,"%i  no match\n",n);
      }
      if(f>=5.9985e-3 && f<=6.0015e-3)
      {  
      fprintf(bfile4,"%i  no match\n",n);
      }    
      printf("%i  no match  %e\n",n, f);
      fprintf(outfile,"%i  no match\n",n);
      //printf("%i  no match\n",n);
      corr[i_match][j_match] = -0.999;
    }

    /* tally number of paired sources with correlations below various thresholds
       this gives a notion of a 'false positive' level  */
    if(corr[i_match][j_match] < 0.5) cc5++;
    if(corr[i_match][j_match] < 0.6) cc6++;
    if(corr[i_match][j_match] < 0.7) cc7++;
    if(corr[i_match][j_match] < 0.8) cc8++;
    if(corr[i_match][j_match] < 0.9) cc9++;

      f = rparams[n][1];
      if(f>=0.2985e-3 && f<=0.3015e-3)
      {  
       if(corr[i_match][j_match] < 0.5) bc5[1]++;
       if(corr[i_match][j_match] < 0.6) bc6[1]++;
       if(corr[i_match][j_match] < 0.7) bc7[1]++;
       if(corr[i_match][j_match] < 0.8) bc8[1]++;
       if(corr[i_match][j_match] < 0.9) bc9[1]++;
      }
      if(f>=0.9985e-3 && f<=1.0015e-3)
      {  
       if(corr[i_match][j_match] < 0.5) bc5[2]++;
       if(corr[i_match][j_match] < 0.6) bc6[2]++;
       if(corr[i_match][j_match] < 0.7) bc7[2]++;
       if(corr[i_match][j_match] < 0.8) bc8[2]++;
       if(corr[i_match][j_match] < 0.9) bc9[2]++;
      }
      if(f>=2.9985e-3 && f<=3.0015e-3)
      {  
       if(corr[i_match][j_match] < 0.5) bc5[3]++;
       if(corr[i_match][j_match] < 0.6) bc6[3]++;
       if(corr[i_match][j_match] < 0.7) bc7[3]++;
       if(corr[i_match][j_match] < 0.8) bc8[3]++;
       if(corr[i_match][j_match] < 0.9) bc9[3]++;
      }
      if(f>=5.9985e-3 && f<=6.0015e-3)
      {  
       if(corr[i_match][j_match] < 0.5) bc5[4]++;
       if(corr[i_match][j_match] < 0.6) bc6[4]++;
       if(corr[i_match][j_match] < 0.7) bc7[4]++;
       if(corr[i_match][j_match] < 0.8) bc8[4]++;
       if(corr[i_match][j_match] < 0.9) bc9[4]++;
      }       

    bin = (int)((1.0+corr[i_match][j_match])/bnw)+1;

    histos[1][bin] += 1.0;

    if(rparams[n][0] > 10.0) 
      {
	bcnt++;
        histos[2][bin] += 1.0;
      }

    
  }// end n loop (iterating through recovered sources)

  fclose(sfile);
  
  /*  update terminal on "false positives"  */

  printf("number of sources w/ cc < 0.5:  %li  w/ cc > 0.5:  %li\n", cc5, NR-cc5);
  printf("number of sources w/ cc < 0.6:  %li  w/ cc > 0.6:  %li\n", cc6, NR-cc6);
  printf("number of sources w/ cc < 0.7:  %li  w/ cc > 0.7:  %li\n", cc7, NR-cc7);
  printf("number of sources w/ cc < 0.8:  %li  w/ cc > 0.8:  %li\n", cc8, NR-cc8);
  printf("number of sources w/ cc < 0.9:  %li  w/ cc > 0.9:  %li\n", cc9, NR-cc9);

  
  /*  write "false positive" numbers to report card  */
  fprintf(outfile, "\n\n**********************************************************************************\n\n");
  fprintf(outfile, "number of sources w/ cc < 0.5:  %li  w/ cc > 0.5:  %li\n", cc5, NR-cc5);
  fprintf(outfile, "number of sources w/ cc < 0.6:  %li  w/ cc > 0.6:  %li\n", cc6, NR-cc6);
  fprintf(outfile, "number of sources w/ cc < 0.7:  %li  w/ cc > 0.7:  %li\n", cc7, NR-cc7);
  fprintf(outfile, "number of sources w/ cc < 0.8:  %li  w/ cc > 0.8:  %li\n", cc8, NR-cc8);
  fprintf(outfile, "number of sources w/ cc < 0.9:  %li  w/ cc > 0.9:  %li\n", cc9, NR-cc9);


  fprintf(bfile1, "\n\n**********************************************************************************\n\n");
  fprintf(bfile1, "number of sources w/ cc < 0.5:  %li \n", bc5[1]);
  fprintf(bfile1, "number of sources w/ cc < 0.6:  %li \n", bc6[1]);
  fprintf(bfile1, "number of sources w/ cc < 0.7:  %li \n", bc7[1]);
  fprintf(bfile1, "number of sources w/ cc < 0.8:  %li \n", bc8[1]);
  fprintf(bfile1, "number of sources w/ cc < 0.9:  %li \n", bc9[1]);

  fprintf(bfile2, "\n\n**********************************************************************************\n\n");
  fprintf(bfile2, "number of sources w/ cc < 0.5:  %li \n", bc5[2]);
  fprintf(bfile2, "number of sources w/ cc < 0.6:  %li \n", bc6[2]);
  fprintf(bfile2, "number of sources w/ cc < 0.7:  %li \n", bc7[2]);
  fprintf(bfile2, "number of sources w/ cc < 0.8:  %li \n", bc8[2]);
  fprintf(bfile2, "number of sources w/ cc < 0.9:  %li \n", bc9[2]);

  fprintf(bfile3, "\n\n**********************************************************************************\n\n");

  fprintf(bfile3, "number of sources w/ cc < 0.5:  %li \n", bc5[3]);
  fprintf(bfile3, "number of sources w/ cc < 0.6:  %li \n", bc6[3]);
  fprintf(bfile3, "number of sources w/ cc < 0.7:  %li \n", bc7[3]);
  fprintf(bfile3, "number of sources w/ cc < 0.8:  %li \n", bc8[3]);
  fprintf(bfile3, "number of sources w/ cc < 0.9:  %li \n", bc9[3]);

  fprintf(bfile4, "\n\n**********************************************************************************\n\n");

  fprintf(bfile4, "number of sources w/ cc < 0.5:  %li \n", bc5[4]);
  fprintf(bfile4, "number of sources w/ cc < 0.6:  %li \n", bc6[4]);
  fprintf(bfile4, "number of sources w/ cc < 0.7:  %li \n", bc7[4]);
  fprintf(bfile4, "number of sources w/ cc < 0.8:  %li \n", bc8[4]);
  fprintf(bfile4, "number of sources w/ cc < 0.9:  %li \n", bc9[4]);

  /*  close files  */
  fclose(sfile);
  fclose(ifile);
  fclose(outfile);

  fclose(bfile1);
  fclose(bfile2);
  fclose(bfile3);
  fclose(bfile4);

  sprintf(Gfile, "%s_correlation_histograms.dat", argv[3]);
  hfile = fopen(Gfile,"w");

  iota = 0.0;
  phase = 0.0;

  for(n=1; n<=bins; n++)
  {
    iota += histos[1][n]/(double)NR;
    phase +=  histos[2][n]/(double)bcnt;
    fprintf(hfile,"%f %f %f\n", bnw/2.0-1.0+(double)(n-1)*bnw, histos[1][n], histos[1][n]/(double)NR);
  }

  fclose(hfile);

  sprintf(Gfile, "%s_parameter_histograms.dat", argv[3]);
  hfile = fopen(Gfile,"w");

  for(n=1; n<=bins; n++)
  {
    fprintf(hfile,"%f %f ", (double)(n)/(double)(bins)*(ranges[0]), phist[0][n]/(double)NR);
    for(j=1; j<= NP; j++)
      {
    fprintf(hfile,"%f %f ", -ranges[j]+(double)(n)/(double)(bins)*(2.0*ranges[j]), phist[j][n]/(double)NR);
      }
   fprintf(hfile,"\n");
  }


  fclose(hfile);

  
  /*  free memory  */  
  free_ivector(count,1,NFFT);
  free_dvector(iA,1,2*NMAX);
  free_dvector(iE,1,2*NMAX);
  free_dvector(rA,1,2*NMAX);
  free_dvector(rE,1,2*NMAX);
  free_dvector(dA,1,2*NMAX);
  free_dvector(dE,1,2*NMAX);
  free_dvector(params,1,NP);
  free_imatrix(bin_pop,1,NFFT,1,npbin);
  free_dmatrix(iparams,1,NI,0,NP);
  free_dmatrix(rparams,1,NR,0,NP);
  free_dmatrix(nwr,1,7,1,npbin);
  free_dmatrix(corr,1,7,1,npbin);
  free_dmatrix(fmatch,1,7,1,npbin);
  free_dmatrix(histos,1,2,1,bins);
}

/* ==========================  END MAIN PROGRAM  ========================== */


void KILL(char* Message)
{
  printf("\a\n");
  printf(Message);
  printf("Terminating the program.\n\n\n");
  exit(1);
 
  return;
}


double nwip(double *a, double *b, long N, double Sn)
{

  int i;
  double ReA, ImA, ReB, ImB;
  double product;
  
  product = 0.0;
 
  for(i=1; i<=N; i++)
    {
      ReA = a[2*i-1];  
      ImA = a[2*i]; 
      ReB = b[2*i-1];  
      ImB = b[2*i];    
      product += ReA*ReB + ImA*ImB;
    }
 
  return 4.0*product/Sn;
  
}

double cross_correlation(double *h1A, double *h1E, double *h2A, double *h2E, long N, long M, int bs)
{
  int i,j;
  double h11, h22, h12;
  double *h1A_shift,*h1E_shift;
  
  h1A_shift = dvector(1,2*M);
  h1E_shift = dvector(1,2*M);

  for(i=1; i<=M; i++)
  {
    j = i + bs;
    if(j>=1 && j<=N)
    {
      h1A_shift[2*i-1] = h1A[2*j-1];
      h1A_shift[2*i]   = h1A[2*j];
      h1E_shift[2*i-1] = h1E[2*j-1];
      h1E_shift[2*i]   = h1E[2*j];  
    }else
    {
      h1A_shift[2*i-1] = 0.0;
      h1A_shift[2*i]   = 0.0;
      h1E_shift[2*i-1] = 0.0;
      h1E_shift[2*i]   = 0.0;  
    }
  }
  
  h11 = nwip(h1A, h1A, N, 1.0) + nwip(h1E, h1E, N, 1.0);
  h12 = nwip(h1A_shift, h2A, M, 1.0) + nwip(h1E_shift, h2E, M, 1.0);
  h22 = nwip(h2A, h2A, M, 1.0) + nwip(h2E, h2E, M, 1.0);
  
  free_dvector(h1A_shift,1,2*N);
  free_dvector(h1E_shift,1,2*N);
  
  return h12/sqrt(h11*h22);
}
