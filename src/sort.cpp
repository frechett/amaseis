#include "as1.h"
/*********************************************************************
** This is a collection of sort routines all using C's qsort()
  10/3/88.  Change integer sort to "straight insertion" since
            sort should be stable.
  10/12/88: Threw out insertion sort and made qsort stable by
            adding an index array to break ties.
  01/18/90: Add merge sort and adapt for RS and RSX (w/ heap expander)
            Define HX is for use with Heap Expander
            Since merge sort is stable, we don't need tie breaker stuff.
            However, we do need it with HX since we order things first
            to sort items a page at a time.
  02/03/90: Re-do sort when using HX to sort all items on each page
            first and then merge.
  03/17/91: Attempt to have HX sorts and non-HX as similar as possible
            in anticipation of making HX selectable with option (/X)
  04/14/91: Fixed bug in merge() with Ivan's help.  Was not merging if there
            was nothing to move in first segment (n0 == 0)
            Also speeded up by replacing dest++ = p0++ with memcpy()
            and shorted code with better organization.
  09/10/91: Adding routine sortli() to sort long integers.
  12/30/91: Remove condition compiles on "ifdef HX" and replace with
            conditional executions based on global variable hx_active
  06/11/00: Convert to Windows 32-bit.  References to "integers" means
  					to "short" integers (16-bit) and long to 32-bit integers
*********************************************************************/
/********************************************************************
** Global variables which are set in the sorting function and used
** in the compare functions
********************************************************************/

#if 0
unsigned short *keyi;	/* Global pointer to short integers */
int *keyl; 			/* Global pointer to long integers */
int sorta1_length;  	/* length of each element */
int global_sort_dir; 	/* direction of sort */
SDEFPTR keya;        	/* Global variable for our compare function */
SDEFPTR keya1;       	/* Points to array of chars */
#endif
int global_sort_dir; 	/* direction of sort */
/**********************************
** select an element.  Taken from Numerical Recipes in C, Second edition.
**
**********************************/
#define SWAP(a,b) temp = (a);(a)=(b);(b)=temp;

float select(int k, int n, float arr[])
// Returns the kth smallest value in the array arr[1..n].  The input array
// will be rearranged to have this value in location arr[k], with all smaller
// elements moved to arr[1..k-1] (in arbitrary order) and all larger elements
// in arr[k+1..n] (also in arbitrary order)
{
  int i,ir,j,l,mid;
  float a,temp;

  l=1;
  ir=n;
  for(;;) {
  	if(ir <= l+1) {		// Active partition contains 1 or 2 elements
    	if(ir == l+1 && arr[ir] < arr[l]) {	// case of 2 elements
      	SWAP(arr[l],arr[ir])
      }
      return arr[k];
    }	else {
    	mid=(l+ir) >> 1;					// Choose median of left, center, and right
      SWAP(arr[mid],arr[l+1])		// elements as partitioning element a.  Also
      if (arr[l] > arr[ir]) {		// rearrange so that arr[l] <= arr[l+1],
      	SWAP(arr[l],arr[ir])	// arr[ir] >= arr[l+1]
      }
      if(arr[l+1] > arr[ir]) {
      	SWAP(arr[l+1],arr[ir])
      }
      if(arr[l] > arr[l+1]) {
      	SWAP(arr[l],arr[l+1])
      }
      i=l+1;                    // Initialize pointers for partitioning
      j=ir;
      a=arr[l+1];								// Partitioning element
      for(;;) {													// Beginning of innermost loop
      	do i++; while (arr[i] < a);				// Scan up to find element > a
        do j--; while (arr[j] > a);				// Scan down to find element < a
        if (j < i) break;						 		// Pointers crossed.  Partitioning complete
        SWAP(arr[i],arr[j])
      }                  								// End of innermost loop
      arr[l+1]=arr[j];					// Insert partitioning element
      arr[j]=a;
      if (j >= k) ir=j-1;				// Keep active the partition that contains
      if (j <= k) l=i;					// kth element
    }
  }
}


/**********************************
** merge(dest,source,n0,n1,elwidth,cmpf)
**
** Merge two pre-sorted strings into one
** dest: pointer to destination
** source: pointer to source
** n0:     length of string 1
** n1:     length of string 2
** elwidth: Width of one element (char = 1, int = 2, far ptr = 4)
** cmpf:   Compare function
*************************************/
//typedef int (*cmpf)(char *, char *);
static void merge(
  char *dest,
  char *p0,
  int n0,
  int n1,
  int elwidth,
  int (*cmpf)(void *,void *))          /* key comparison function              */
{
  char *p1;     /* Points to second string.  */
  p1 = p0 + n0*elwidth;
  do {
    if(n0 == 0) {
      memcpy(dest,p1,n1*elwidth);
      n1 = 0;
    }
    else if(n1 == 0) {       /* Just move n0 points in to dest */
      memcpy(dest,p0,n0*elwidth);
      n0 = 0;
    }
    else if(0 < (*cmpf)(p0,p1)) {
      memcpy(dest,p1,elwidth);
      dest += elwidth;
      p1 += elwidth;
      n1--;
    } else {
      memcpy(dest,p0,elwidth);
      dest += elwidth;
      p0 += elwidth;
      n0--;
    }
  } while(n0 || n1);
}
/******************************************
** msort(p,n,elwidth,cmp)
**
** Merge sort
*******************************************/
void msort(
	void *p,           /* Index to be sorted */
	int n,             /* Number of items */
	int elwidth,       /* Width of an element (char = 1, int = 2, etc. */
	int (*cmpf)(void *, void *))  /* key comparison function              */
{
  char *array[2];
  int n0, n1, i, t;
  int step = 1;
  int s = 1;               /* equals 0 or 1 depending on target of merge */
  array[1] = (char *)calloc(n,elwidth); /* place to hold temporary index */
  array[0] = (char *)p;
  while(step<n) {
    s = !s;
    n0 = step;
    n1 = step;
    for(i=0;i<n;i+=step+step) {
      if(i + n0 + n1 >n) {
        if(i + n0 > n) {
          n1 = 0;
          n0 = n - i;
        } else {
          n1 = n - i - n0;
        }
      }
      t = elwidth*i;
      merge(array[!s]+t,array[s]+t,n0,n1,elwidth,cmpf);
    }
    step <<= 1;
  }
  /* If ended up in wrong array, move back */
  if(!s)
  {
    memcpy(array[0],array[1],n*elwidth);
  }
  free(array[1]);
}

#if 0
/********************************************/
/* Compare function for strings             */
/********************************************/
static int cmps(void *p1, void *p2)
{
//int cmps(
//	char **i1_ptr,
//	char **i2_ptr)
//{
	char **i1_ptr = (char**)p1;
  char **i2_ptr = (char**)p2;
    if(*i1_ptr == NULL) return -1;
    if(*i2_ptr == NULL) return  1;
    return(strcmp(*i1_ptr,*i2_ptr));
}
/*****************************************
** Compare function for single alpha array
******************************************/
static int cmpa1(void *p1, void *p2)
{
	int *i1_ptr = (int *)p1;
	int *i2_ptr = (int *)p2;
  int i;
  {
    char *q1 = getsptr(keya1,*i1_ptr);
    char *q2 = getsptr(keya1,*i2_ptr);
    i = strncmp(q1,q2,sorta1_length);
  }
  if(global_sort_dir) i = -i;
  return i;
}
/******************************************
** Compare function for indices pointing to short integers
*******************************************/
static int cmpi(void *p1, void *p2)
{
	unsigned short *i1_ptr = (unsigned short *)p1;
	unsigned short *i2_ptr = (unsigned short *)p2;
  unsigned k1,k2;
  int i;
  k1 = keyi[*i1_ptr];
  k2 = keyi[*i2_ptr];
  if(k1==k2) i = 0;
  else i = (k1<k2) ? -1 : 1;
  if (global_sort_dir) i = -i;
  return(i);
}
/******************************************
** Compare function for indices pointing to long integers
** The indices are, themselves, unsigned integers
*******************************************/
static int cmpl(void *p1, void *p2)
{
	int *i1_ptr = (int *)p1;
	int *i2_ptr = (int *)p2;
  int i;
  int k1 = keyl[*i1_ptr];
  int k2 = keyl[*i2_ptr];
  if(k1==k2) i = 0;
  else i = (k1<k2) ? -1 : 1;
  if (global_sort_dir) i = -i;
  return(i);
}
/***********************************************/
/* Compare Function for linked list of strings */
/***********************************************/
static int cmpa(void *p1, void *p2)
{
	int *i1_ptr = (int *)p1;
	int *i2_ptr = (int *)p2;
  SDEFPTR sptr;
  char *q1, *q2;
  int i;
  sptr = keya;
  while (sptr) {
    q1 = (char *)getsptr(sptr,*i1_ptr);
    q2 = (char *)getsptr(sptr,*i2_ptr);
    i = strncmp(q1,q2,sptr->s_length);
  	// 6/1/95: What is this for?  It says to reverse the sort
  	// if this is not the first field in the database.  Messes
  	// things up if the first search field is the second field
  	// 6/13/95: Found it.  "->off" is just used as a flag for
  	// direction.  When using this, don't use original search
  	// structure since it uses off for distance into record.
  	// Instead use new structure derived from it with off
  	// set according to direction of sort
  	if(sptr->off)
    	i = -i;
  	if (i) return (i);
  		sptr=sptr->nextsdef;
   	}
    // Only get here if fields are identical.  Now check times for tie breaking
    if(keyi)
    	return(cmpl(i1_ptr,i2_ptr));      /* Compare function for ints */
    else
    	return(0);
}
/****************************************
** Compare function for sorting short integers
*****************************************/
static int cmp(void *p1, void *p2)
{
 unsigned short *i1 = (unsigned short *)p1;
 unsigned short *i2 = (unsigned short *)p2;
  int i = 0;
  if(*i1>*i2)
  	i = 1;
  else if(*i1<*i2)
  	i = -1;
  if (global_sort_dir)
  	i = -i;
  return i;
}
#endif
/****************************************
** Compare function for sorting long integers
*****************************************/
static int cmplong(void *p1, void *p2)
{
  long *i1 = (long *)p1;
  long *i2 = (long *)p2;
  int i = 0;
  if(*i1>*i2) i = 1;
  else if(*i1<*i2) i = -1;
  if (global_sort_dir) i = -i;
  return i;
}
#if 0
/********************************************/
/* Sort an array of pointers.  Pointers     */
/* point to strings.  Strings are key.      */
/* Don't worry about tie breaking since     */
/* just use to sort files which are unique. */
/********************************************/
void sorts(
  char **p,   /* array of pointers to strings */
  int n) /* number of items to sort      */
{
    /* call: msort(void *base, int nelem, int width, int(*fcmp)()); */
    msort(p,n,sizeof(*p),cmps);
}
/********************************************************************
/* Sort an array of long integers which are used as indices into
/* another array of short integers (the keys)
/* Use msort()
/********************************************************************/

void sorti(
  unsigned short *key,   	/* points to array of integers */
  unsigned *p,     				/* array of integers to be re-arranged */
  unsigned n)      				/* number of items to sort         */
{
    keyi = key; /* point to vector of keys */
    msort(p,n,sizeof(unsigned *),cmpi);
}

// Sort integers based on keys which are long integers
void sortl(
	int *key, 	/* points to array of long integers */
	int *p,     /* array of integers to be re-arranged */
	int n)  		/* number of items to sort         */
{
    keyl = key; /* point to vector of keys */
    msort(p,n,sizeof(int),cmpl);
}

/*******************************************************************
/* Sort an array of short integers
/******************************************************************/
void sortint(
	unsigned short *p,	/* array of integers to be re-arranged */
	unsigned n,      		/* number of items to sort         */
	int dir)
{
    global_sort_dir = (dir <0) ? 1 : 0;
    msort(p,n,sizeof(unsigned short),cmp);
}
#endif
/********************************************************************/
/* Sort an array of long integers                                        */
/********************************************************************/
void sortlongint(
	int *p,     /* array of integers to be re-arranged */
	int n,      /* number of items to sort         */
	int dir)
{
    global_sort_dir = (dir <0) ? 1 : 0;
    msort(p,n,sizeof(int),cmplong);
}
#if 0

/********************************************************************/
/* Sort an array of pointers.  Pointers point to in-core array      */
/* of linked list of characters (not strings)                       */
/* If the alpha fields are identical, then check times.             */
/********************************************************************/
void sorta(
	SDEFPTR key,    		/* points to beginning of array of search fields */
	int *p,   			/* array of integers to be re-arranged */
	int n,     		/* number of items to sort      */
	int *stimes,  	/* pointer to times to be sorted on if alpha data the same */
	int dir)          	/* direction.  1 means sort up, -1 means sort down */
                  		/* for numeric sort */
{
  SDEFPTR sptr;
  int jump=0;
  /* find out if there is only one string in chain.  If so use sorta1 */
  sptr = key;
  global_sort_dir = (dir <0) ? 1 : 0;
  while(sptr) {
    jump++;
    sptr=sptr->nextsdef;
  }
  if(jump==0) {                    /* No alpha data */
  	if(stimes == NULL)
  		return;   /* Nothing to sort */
  	sortl(stimes,p,n);           /* Just numeric data */
    	return;
  }
  if(jump==1 && stimes == NULL) {  /* One field of alpha data and no numeric data */
  	global_sort_dir = key->off;
    sorta1(key,p,n);
    return;
  }
  keya = key;         /* Set up global variable */
  keyl = stimes;      /* points to times */
  msort(p,n,sizeof(int),cmpa);
}

/********************************************************************/
/* Sort one element of a linked list of characters                  */
/* Uses merge sort msort()                                       */
/********************************************************************/
void sorta1(
	SDEFPTR key,/* points to beginning of array of search fields */
	int *p,     /* array of integers to be re-arranged */
	int n)      /* number of items to sort      */
{
  /* Set up our global variables */
  sorta1_length = key->s_length;
  keya1 = key; /* point to vector of keys */
  msort(p,n,sizeof(int),cmpa1);
}

#endif
