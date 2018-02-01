#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

/**
 *integer powers of 10
 */
int EE( int Y ){
  int ret = 1;
  while( Y-- ) ret*=10;
  return ret;
}

int main( int argc, char** argv){

  if (argc < 2 ){
    cerr << "Usage: generate_input N Y" << endl
	 << "N = number of input trials" << endl
	 << "10^Y = Max trial value" << endl;
    exit(0);
  }
  int N = atoi(argv[1]);
  int Y = atoi(argv[2]);
  int x;
  cout << N << endl;
  for ( int i = 0; i < N; ++i ){
    if( Y > 8 )
      cout << rand()%100000000;
    do {
      x =  rand()%EE(Y%8);
    } while (!(x%2 && x%5 ));  //reallistically should output leading zeros
    cout << x << endl;
  }
}
