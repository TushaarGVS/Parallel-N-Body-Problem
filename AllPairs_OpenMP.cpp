#include <cmath>
#include <iostream>
#include <omp.h>
#include <time.h>
#include <sys/time.h>
#define N 32
using namespace std ;

struct vec{
  float x, y ;
} ;

struct particle{
  float x, y, m ;
} ;

vec find_force(particle p, particle particles[], int size){
  vec ans ;
  ans.x = ans.y = 0 ;
  float tempx,tempy ;
  tempx = tempy = 0 ;

  #pragma omp parallel for num_threads(N) reduction(+:tempx,tempy)
  for(int i = 0 ; i < size ; i++){
    if(particles[i].x == p.x && particles[i].y == p.y)
      continue ;
    float dsq = pow(p.x - particles[i].x, 2) ;
    dsq += pow(p.y - particles[i].y, 2) ;
    float d = sqrt(dsq) ;
    tempx += ((particles[i].x - p.x) * ( particles[i].m / pow(d,3))) ;
    tempy += ((particles[i].y - p.y) * ( particles[i].m / pow(d,3))) ;

  }
  ans.x = tempx ;
  ans.y = tempy ;
  return ans ;
}

double get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1.0 + tv.tv_usec * 1.0E-6);
}

int main(){
  int n ;
  double start, end ;
  cin>>n ;

  particle particles[n] ;

  float r,vx,vy, cr,cg,cb ;
  cin>>r ;
  for(int i= 0 ; i < n ; i++){
    cin>>particles[i].x>>particles[i].y>>vx>>vy>>particles[i].m>>cr>>cg>>cb ;
  }

  vec ans[n] ;
  start = get_time() ;

  #pragma omp parallel num_threads(N)
  {
    #pragma omp for
    for(int i = 0 ; i < n ; i++){
      ans[i] = find_force(particles[i], particles, n) ;
    }
  }

  end = get_time() ;

  /*for(int i = 0 ; i < n ; i++){
    cout<<ans[i].x<<" "<<ans[i].y<<endl ;
  }*/
  cout<<(double)(end - start)<<endl ;


}
