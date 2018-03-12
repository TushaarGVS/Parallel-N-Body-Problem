#include <math.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>
#define THREADS_PER_BLOCK 2048


struct vec{
  float x, y ;
} ;

struct particle{
  float x, y, m ;
} ;

__global__ void find_force(int index, struct particle *particles, struct vec *ans, int size){
  
  struct particle p = particles[index] ;
  int i = threadIdx.x + blockIdx.x * blockDim.x;
  
  if(i <  size && !(particles[i].x == p.x && particles[i].y == p.y)){ 
    float dsq = pow(p.x - particles[i].x, 2) ;
    dsq += pow(p.y - particles[i].y, 2) ;
    float d = sqrt(dsq) ;

    __syncthreads() ;
    ans[index].x += ((particles[i].x - p.x) * ( particles[i].m / pow(d,3))) ;
    ans[index].y += ((particles[i].y - p.y) * ( particles[i].m / pow(d,3))) ;
    __syncthreads() ;
  }
}

double get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1.0 + tv.tv_usec * 1.0E-6);
}

int main(){
  int n ;
  double start, end ;
  scanf("%d" ,&n) ;

  struct particle particles[n] ;

  float r,vx,vy, cr,cg,cb ;
  scanf("%f" ,&r) ;
  for(int i= 0 ; i < n ; i++)
    scanf("%f%f%f%f%f%f%f%f", &particles[i].x, &particles[i].y, &vx, &vy, &particles[i].m, &cr, &cg, &cb) ;
  

  struct vec ans[n] ;

  struct particle *d_particles ;
  struct vec *d_ans ;
  int d_particles_size = n * sizeof(struct particle) ;
  int d_ans_size = n * sizeof(struct vec) ;
    
  cudaMalloc((void **)&d_particles, d_particles_size) ;
  cudaMalloc((void **)&d_ans, d_ans_size) ;

  cudaMemcpy(d_particles, particles, d_particles_size, cudaMemcpyHostToDevice) ;
	    
  start = get_time() ;

  for(int i = 0 ; i < n ; i++)
    find_force<<<(n+THREADS_PER_BLOCK-1)/THREADS_PER_BLOCK, THREADS_PER_BLOCK>>>(i, d_particles, d_ans, n) ;
    

  cudaMemcpy(ans, d_ans, d_ans_size, cudaMemcpyDeviceToHost) ;

  end = get_time() ;

  printf("%lf\n", (double)(end-start)) ;
}
