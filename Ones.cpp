#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

#define lint __uint128_t

int intlog2 ( lint val) {
    if (val == 0)
      return 0;
    if (val == 1)
      return 0;
    int ret = 0;
    while (val > 1) {
        val >>= 1;
        ret++;
    }
    return ret;
}

lint intpow2 ( int val ){
  lint ret = 1;
  while( val-- )
    ret*=2;
  return ret;
}

/****
 *Calulates a vector of all possible whole divisors of the totient using the array of all primes, p.
 */
void allFactors ( lint t, int* p, vector<lint>& trials ){
  vector<lint> each;
  vector<lint>::iterator it;
  lint eachProduct;
  double maxfactor = sqrt((double)t);
  for (int i = 0; p[i]<= maxfactor; ++i ){ //iterate over all primes
    eachProduct = 1;
    while ( t%p[i] == 0 ){
      t=t/p[i];
      eachProduct *= p[i];
      vector<lint> temp;
      temp.resize(trials.size());
      transform(trials.begin(),trials.end(),temp.begin(),bind1st(multiplies<lint>(),eachProduct));
      int size1 = each.size();
      each.insert(each.end(), temp.begin(), temp.end());
      if( size1 )
	inplace_merge(each.begin(), each.begin()+size1, each.end());
    }
    int size2 = trials.size();
    trials.insert(trials.end(), each.begin(), each.end());
    inplace_merge(trials.begin(), trials.begin()+size2, trials.end());
    each.clear();
  }
  if( t != 1 ){
    vector<lint> temp;
    temp.resize(trials.size());
    transform(trials.begin(),trials.end(),temp.begin(),bind1st(multiplies<lint>(),t));
    int size2 = trials.size();
    trials.insert(trials.end(), temp.begin(), temp.end());
    inplace_merge(trials.begin(), trials.begin()+size2, trials.end());
  }
}



/****
 * Uses the Euler Product Formula to calculate the totient function of n, using the array of all primes, P.
 */
lint ePF( lint n, int* p){
  lint totient = n;
  double maxfactor = sqrt((double)n);
  //cout << "Debug: " << maxfactor << endl;
  for (int i = 0; p[i]<= maxfactor; ++i ){ //iterate over all primes
    //cout << "Debug: " << i << " " <<  p[i] << endl;
    if ( n%p[i] == 0 ){
      totient = totient/p[i]*(p[i]-1);
    }
    while ( n%p[i] == 0 ){
      n=n/p[i];
    }
  }
  if( n != 1 ){
    totient = totient/n*(n-1);
  }
  return totient;
}

/****
* Uses the sieve of Eratosthenes algorithm to calculate all primes less than m
*/
void primes(int m, int* p, bool* sieve ){

  for( int i = 2; i < m; ++i )
    sieve[i-2]=1;
  int x=0;
  for( int i = 2; i < m; ++i ){
    if (sieve[i-2]){
      p[x]=i;
      x++;
      for( lint j = (lint)i*i; j < m; j=j+i )
	sieve[j-2] = 0;
    }
  }
  //cout << x << endl; //used to determine value of x
}

/****
* Calculates the smallest integer comprised of repeating 1s, R(k), which is a multiple of input n
*/
int main( int argc, char** argv ){
  //Calculate Primes
  int m = int(sqrt(9e13)); //looking for all primes less than m
  int num = 632759; //1.3*m/(log(m)-1); //number of primes less than m.
  int *p  = new int[num];
  bool *sieve = new bool[m-2];
  primes(m, p, sieve);
  
  delete sieve;

  //Input and iterate over all records
  char in[40]; lint input;
  cin >> in;
  int records = atoi(in);
  int total = 0;
  for (int r = 0; r < records; ++r){
    cin >> in;
    input = atol(in); //n
    if ( input < 1 || input %5 == 0 || input % 2 == 0) { //Catch invalid inputs
        cout <<  0 << endl;
    } else {
        
    //Calculate the totient of n*9 and create a vector containing all whole divisors of the totient
    vector<lint> trials;
    trials.push_back(1);
    lint totient = ePF(input*9, p);
    allFactors ( totient, p,  trials );
    
    lint k = 0;  // R(k)%n
      
    //Pre-calculate the values needed to calculate R(k) in modulo n
    int numPre = 80;  //number of powers of 2 to pre-calculate in modulo
    lint mul[numPre]; lint add[numPre];
    mul[0] = 10; add[0] = 1;
    for( int i = 1; i < numPre; ++i){
      if( i > 3 ){ //after (10^(2^3))%n is calculated, increment only in multiples of 10^(2^2) to prevent overflow errors 
	mul[i] = (  (((mul[i-1]*(mul[i-1]/100000000))%input*mul[2])%input*mul[2])%input + (mul[i-1]*((mul[i-1]%100000000)/10000))%input*mul[2]+(mul[i-1]*(mul[i-1]%10000))%input  )%input;
	add[i] = (  (((add[i-1]*(mul[i-1]/100000000))%input*mul[2])%input*mul[2])%input + (add[i-1]*((mul[i-1]%100000000)/10000))%input*mul[2]+(add[i-1]*(mul[i-1]%10000))%input + add[i-1]  )%input;
      }
      else{
	mul[i] = (mul[i-1]*mul[i-1])%input;
	add[i] = (add[i-1]*mul[i-1]+add[i-1])%input;
      }
    }
      
    //iterate over all divisors of the tortient, and for each calculate R(k)%n from the previous and check if it is 0
    lint tinput = 0;
    lint last = 0;
    lint compare;
    for ( vector<lint>::iterator it = trials.begin()+1; it != trials.end(); ++it){ //iterate over all trials
      if( input == 1 ){
	cout << 1 << endl;
	break;
      }
      tinput = *it-last;
      last = *it;
      lint copInput = tinput;
      for( int i = 0; copInput > 0; ++i ){ //increase R(k) in increments of 10^2^i
	if( copInput % 2 ){
	  if( intlog2(k)+intlog2(mul[i]) > 56 ){  //increase R(k) using slower algorithm if the product would otherwise excede the size of a 63 bit int
	    k = (  (((k*(mul[i]/100000000))%input*mul[2])%input*mul[2])%input + (k*((mul[i]%100000000)/10000))%input*mul[2]+(k*(mul[i]%10000))%input  )%input;
	      k=(k+add[i])%input;
	  }    
	  else
	    k=(k*mul[i]+add[i])%input;
	}
	copInput /= 2;
    if (i >= numPre ){
	  cerr << "Error: Increase NumPre" << endl;
	  break;
	}
    }
     
      if( k == 0 ){
	cout <<  (long)*it << endl; //if the solution is found, output the value of the successful trial
	break;
      }
      
    }
    }
  }
  delete p;
}
