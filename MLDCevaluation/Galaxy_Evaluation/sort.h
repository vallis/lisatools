#define SWAP(a,b) temp=(a);(a)=(b);(b)=temp;
#define MM 7
#define NSTACK 50

void indexx(unsigned long n, double* arr, long* I)
{
  unsigned long i, indxt, ir=n, temp, j, k, l=1;
  long nl, nh;
  int jstack=0, *istack;
  double a;


  nl=1;
  nh=n;

  istack=ivector(1,NSTACK);
  
  for(j=1; j<=n; j++)
    {
      I[j]=j;
    }

  for(;;)
    { 
    if(ir-l < MM)
      {
	for(j=l+1;j<=ir;j++)
	  {
	    indxt=I[j];
	    a=arr[indxt];
	    for(i=j-1;i>=l;i--)
	      { 
		if(arr[I[i]]<=a) break;

		I[i+1]=I[i];
	      }
	    
	    I[i+1]=indxt;
	  }

	if(jstack==0) break;
	
	ir=istack[(jstack--)];
	l=istack[(jstack--)];

      }
    else
      {

	k=(l+ir) >> 1;

	SWAP(I[k],I[l+1]);
	
	if(arr[I[l]]>arr[I[ir]])
	  {
	    SWAP(I[l], I[ir])
	  }
	
	if(arr[I[l+1]]>arr[I[ir]])
	  {
	    SWAP(I[l+1],I[ir])
	  }
	
	if(arr[I[l]]>arr[I[l+1]])
	  {
	    SWAP(I[l],I[l+1])
	  }

	i=l+1;
	j=ir;
	indxt=I[l+1];
	a=arr[indxt];

	for(;;)
	  {
	    
	    do i++; while(arr[I[i]]<a);
	    do j--; while(arr[I[j]]>a);
	    
	    if(j<i) break;
	    
	    SWAP(I[i], I[j]);
	  }

	I[l+1]=I[j];
	I[j]=indxt;
	jstack+=2;
		
	if(ir-i+1 >= j-l)
	  {
	    istack[jstack]=ir;
	    istack[jstack-1]=i;
	    ir=j-1;

	  }
	else
	  {
	    istack[jstack]=j-1;
	    istack[jstack-1]=l;
	    l=i;
	  }
	  
      }
    }
	  
   free_ivector(istack,1,NSTACK);
}
