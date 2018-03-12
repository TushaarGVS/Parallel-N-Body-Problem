#include <cmath>
#include <iostream>
using namespace std ;

struct vec{
  float x, y, z ;
} ;

struct particle{
  float x, y, z, m ;
} ;

vec find_force(particle p, particle particles[], int size){
  vec ans ;
  ans.x = ans.y = ans.z  = 0 ;
  for(int i = 0 ; i < size ; i++){
    if(particles[i].x == p.x && particles[i].y == p.y && particles[i].z == p.z)
      continue ;
    float dsq = pow(p.x - particles[i].x, 2) ;
    dsq += pow(p.y - particles[i].y, 2) ;
    dsq += pow(p.z - particles[i].z, 2) ;
    float d = sqrt(dsq) ;
    ans.x += ((particles[i].x - p.x) * ( particles[i].m / pow(d,3))) ;
    ans.y += ((particles[i].y - p.y) * ( particles[i].m / pow(d,3))) ;
    ans.z += ((particles[i].z - p.z) * ( particles[i].m / pow(d,3))) ;
  }
  return ans ;
}

int main(){
  particle particles[100] ;
  int n ;
  cout<<"Enter number of particles:"<<endl ;
  cin>>n ;
  cout<<"Enter the particles as x y z m:"<<endl ;
  for(int i= 0 ; i < n ; i++){
    cin>>particles[i].x>>particles[i].y>>particles[i].z>>particles[i].m ;
  }

  vec ans[100] ;
  for(int i = 0 ; i < n ; i++){
    ans[i] = find_force(particles[i], particles, n) ;
  }

  cout<<"The force vector for each particle is:"<<endl ;

  for(int i = 0 ; i < n ; i++){
    cout<<ans[i].x<<" "<<ans[i].y<<" "<<ans[i].z<<endl ;
  }
}
