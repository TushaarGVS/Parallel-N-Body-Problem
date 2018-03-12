/* 
 * gcc BarnesHut.c -o barnes -lm
 * ./barnes filename
 */
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

#define THRESHOLD 0.2

const double G = 6.67408 ;
double space_x1, space_x2, space_y1, space_y2 ;

struct Force {
  double x, y ;
} ;

struct Particle {
  double x, y ;    // 2d co-ordinates
  double mass ;
  
  struct Force netForce ;
} ;

struct Quadrant {
  double x1, x2, y1, y2 ;
  int numChildParticles ;
  double centerOfMass_x, centerOfMass_y, mass ;
  
  struct Quadrant* NO ;
  struct Quadrant* NW ;
  struct Quadrant* SW ;
  struct Quadrant* SO ;
  
  struct Particle existingParticle ;
} ;

struct Quadrant* root = NULL ;

double get_time() {
  struct timeval tv ;
  gettimeofday(&tv, NULL) ;
  return (tv.tv_sec * 1.0 + tv.tv_usec * 1.0E-6) ;
}

int getQuadrant(struct Particle particle, struct Quadrant* quadrant) {
  int subQuadrantNumber ;
  
  double x = particle.x ;
  double y = particle.y ;
  
  double x1 = quadrant -> x1 ;
  double x2 = quadrant -> x2 ;
  double y1 = quadrant -> y1 ;
  double y2 = quadrant -> y2 ;
  
  double x_mid = (x2 + x1)/ 2 ;
  double y_mid = (y2 + y1)/ 2 ;
  
  // if particle is not in the quadrant
  if(x < x1 || x > x2 || y < y1 || y > y2)
    return 0 ;
  
  if(x < x_mid) {
    if(y < y_mid)
      subQuadrantNumber = 3 ;
    else
      subQuadrantNumber = 2 ;
  } else {
    if(y < y_mid)
      subQuadrantNumber = 4 ;
    else
      subQuadrantNumber = 1 ;
  }
  return subQuadrantNumber ;
}

struct Quadrant* subNode(int subQuadrantNumber, struct Quadrant* quadrant) {
  if (subQuadrantNumber == 1)
    return quadrant -> NO ;
  else if(subQuadrantNumber == 2)
    return quadrant -> NW ;
  else if(subQuadrantNumber == 3)
    return quadrant -> SW ;
  else
    return quadrant -> SO ;
}

struct Quadrant* createSubNode(int subQuadrantNumber, struct Quadrant* quadrant) {
  struct Quadrant* newQuadrant = (struct Quadrant*)malloc(sizeof(struct Quadrant)) ;
  newQuadrant -> numChildParticles = 0 ;
  
  double x1 = quadrant -> x1 ;
  double x2 = quadrant -> x2 ;
  double y1 = quadrant -> y1 ;
  double y2 = quadrant -> y2 ;
  
  double x_mid = (x2 + x1)/ 2 ;
  double y_mid = (y2 + y1)/ 2 ;
  
  if(subQuadrantNumber == 1) {
    newQuadrant -> x1 = x_mid ;
    newQuadrant -> x2 = x2 ;
    newQuadrant -> y1 = y_mid ;
    newQuadrant -> y2 = y2 ;
    
    quadrant -> NO = newQuadrant ;
  } else if(subQuadrantNumber == 2) {
    newQuadrant -> x1 = x1 ;
    newQuadrant -> x2 = x_mid ;
    newQuadrant -> y1 = y_mid ;
    newQuadrant -> y2 = y2 ;
    
    quadrant -> NW = newQuadrant ;
  } else if(subQuadrantNumber == 3) {
    newQuadrant -> x1 = x1 ;
    newQuadrant -> x2 = x_mid ;
    newQuadrant -> y1 = y1 ;
    newQuadrant -> y2 = y_mid ;
    
    quadrant -> SW = newQuadrant ;
  } else if(subQuadrantNumber == 4) {
    newQuadrant -> x1 = x_mid ;
    newQuadrant -> x2 = x2 ;
    newQuadrant -> y1 = y1 ;
    newQuadrant -> y2 = y_mid ;
    
    quadrant -> SO = newQuadrant ;
  }
  return newQuadrant ;
}

void insertToNode(struct Particle newParticle, struct Quadrant* quadrant) {
  int subQuadrantNumber, numChildParticles = quadrant -> numChildParticles ;
  
  if(numChildParticles > 1) {
    subQuadrantNumber = getQuadrant(newParticle, quadrant) ;
    struct Quadrant* subQuadrant = subNode(subQuadrantNumber, quadrant);
    if (subQuadrant == NULL)
      subQuadrant = createSubNode(subQuadrantNumber, quadrant) ;
    insertToNode(newParticle, subQuadrant) ;
  } else if(numChildParticles == 1) {
    subQuadrantNumber = getQuadrant(quadrant -> existingParticle, quadrant) ;
    struct Quadrant* subQuadrant = subNode(subQuadrantNumber, quadrant) ;
    if (subQuadrant == NULL)
      subQuadrant = createSubNode(subQuadrantNumber, quadrant) ;
    insertToNode(quadrant -> existingParticle, subQuadrant);
    
    subQuadrantNumber = getQuadrant(newParticle,quadrant) ;
    subQuadrant = subNode(subQuadrantNumber, quadrant) ;
    if (subQuadrant == NULL)
      subQuadrant = createSubNode(subQuadrantNumber, quadrant) ; 
    insertToNode(newParticle, subQuadrant) ;
  } else
    quadrant -> existingParticle = newParticle ;
  quadrant -> numChildParticles++ ;
}

void buildQuadTree(struct Particle particles[], struct Quadrant* root, int numParticles) {
  int i ;
  for(i = 0; i < numParticles; i++)
    insertToNode(particles[i], root) ;
}

struct Quadrant* initQuadTree(double space_x1, double space_x2, double space_y1, double space_y2) {
  struct Quadrant* root = (struct Quadrant*)malloc(sizeof(struct Quadrant)) ;
  
  root -> x1 = space_x1 ;
  root -> x2 = space_x2 ;
  root -> y1 = space_y1 ;
  root -> y2 = space_y2 ;
  
  root -> numChildParticles = 0 ;
  return root ;
}

void printQuadTree(struct Quadrant* root) {
  if(root == NULL)
    return ;
  
  if(root -> numChildParticles == 1) {
    printf("%lf, %lf; %lf, %lf :\t", root -> x1, root -> x2, root -> y1, root -> y2) ;
    struct Particle particle = root -> existingParticle ;
    printf("%lf, %lf, %lf\n", particle.x, particle.y, particle.mass) ;
  }
  
  printQuadTree(root -> NO) ;
  printQuadTree(root -> NW) ;
  printQuadTree(root -> SW) ;
  printQuadTree(root -> SO) ;
}

void computeMassDistribution(struct Quadrant* root) { 
  if(root == NULL)
    return ;
  if(root -> numChildParticles == 1) {
    struct Particle existingParticle = root -> existingParticle ;
    root -> centerOfMass_x = existingParticle.x ;
    root -> centerOfMass_y = existingParticle.y ;
    root -> mass = existingParticle.mass ;
  } else {
    int subQuadrantNumber ;
    struct Quadrant* subQuadrant ;
    for(subQuadrantNumber = 1; subQuadrantNumber <= 4; subQuadrantNumber++) {
      subQuadrant = subNode(subQuadrantNumber, root) ;
      if(!subQuadrant)
	continue ;
      computeMassDistribution(subQuadrant) ;
      root -> mass += subQuadrant -> mass;
      root -> centerOfMass_x += (subQuadrant -> centerOfMass_x) * (subQuadrant -> mass);
      root -> centerOfMass_y += (subQuadrant -> centerOfMass_y) * (subQuadrant -> mass);
    }
  }
  root -> centerOfMass_x /= root -> mass ;
  root -> centerOfMass_y /= root -> mass ;
}

struct Force gravitationalForce(struct Particle particle_1, struct Particle particle_2) {
  struct Force force ;
  double d_sq = pow(particle_1.x - particle_2.x, 2) ;
  d_sq += pow(particle_1.y - particle_2.y, 2) ;
  double d = sqrt(d_sq) ;
  
  force.x = G * (particle_2.x - particle_1.x) * ( particle_2.mass / pow(d, 3)) ;
  force.y = G * (particle_2.y - particle_1.y) * ( particle_2.mass / pow(d, 3)) ;
  return force ;
}

int checkSame(struct Particle particle_1, struct Particle particle_2) {
  if(particle_1.x == particle_2.x && particle_1.y == particle_2.y)
    return 1 ;
  return 0 ;
}

double distance = 0 ;

int heightAndDistance(struct Quadrant* root, struct Quadrant* head, struct Particle targetParticle, int height) {
  if(root == NULL)
    return 0 ;
  
  if(root == head) {
    double x1 = root -> centerOfMass_x ;
    double y1 = root -> centerOfMass_y ;
    double x2 = targetParticle.x ;
    double y2 = targetParticle.y ;
    
    double d_sq = pow(x2 - x1, 2) ;
    d_sq += pow(y2 - y1, 2) ;
    distance = sqrt(d_sq) ;
    return height ;
  }
  
  int level_NO = heightAndDistance(root -> NO, head, targetParticle, height + 1) ;
  if(level_NO != 0)
    return level_NO ;
  int level_NW = heightAndDistance(root -> NW, head, targetParticle, height + 1) ;
  if(level_NW != 0)
    return level_NW ;
  int level_SW = heightAndDistance(root -> SW, head, targetParticle, height + 1) ;
  if(level_SW != 0)
    return level_SW ;
  return heightAndDistance(root -> SO, head, targetParticle, height + 1) ;
}

struct Force computeForce(struct Quadrant* head, struct Particle targetParticle) {
  struct Force force ;
  force.x = force.y = 0 ;
  if(root == NULL)
    return force ;
  
  if(head -> numChildParticles == 1 && checkSame(head -> existingParticle, targetParticle)) {
    force.x = 0 ;
    force.y = 0 ;
  } else if(head -> numChildParticles == 1)
    force = gravitationalForce(targetParticle, head -> existingParticle) ;
  else {
    int d = heightAndDistance(root, head, targetParticle, 1) ;
    double r = distance ;
    distance = 0 ;
    if(d/r < THRESHOLD) {
      struct Particle body ;
      body.x = head -> centerOfMass_x ;
      body.y = head -> centerOfMass_y ;
      body.mass = head -> mass ;
      force = gravitationalForce(targetParticle, body) ;
    } else {
      int subQuadrantNumber ;
      struct Quadrant* subQuadrant ;
      struct Force forceIntermediate ;
      for(subQuadrantNumber = 1; subQuadrantNumber <= 4; subQuadrantNumber++) {
	subQuadrant = subNode(subQuadrantNumber, head) ;
	if(!subQuadrant)
	  continue ;
	forceIntermediate = computeForce(subQuadrant, targetParticle) ;
	force.x += forceIntermediate.x ;
	force.y += forceIntermediate.y ;
      }
    }
  }
  return force ;
}

void calculateForce(struct Particle particles[], int numParticles) {
  int i ;
  for(i = 0; i < numParticles; i++)
    particles[i].netForce = computeForce(root, particles[i]) ;
}

void printForce(struct Particle particles[], int numParticles) {
  int i ;
  for(i = 0; i < numParticles; i++)
    printf("%lf, %lf, %lf :\t %lf %lf\n", particles[i].x, particles[i].y, particles[i].mass, particles[i].netForce.x, particles[i].netForce.y) ;
}

int main(int argc, char *argv[]) {
  int numParticles, i = 0, count = 0 ;
  double radius ;
  size_t buffer_size = 100 ;
  char* line = NULL ;
  FILE *file ;
  char read ;
  file = fopen(argv[1], "r") ;
  if(file)
    while((read = getline(&line, &buffer_size, file)) != -1) {
      if(count == 0)
	numParticles = atoi(line) ;
      else if(count == 1) {
	radius = atof(line) ;
	break ;
      }
      count++ ;
    }
  
  /*
   * input file format:
   * number_of_bodies
   * radius
   * x y velocity_x velocity_y mass color_r color_g color_b
   */
  
  struct Particle particles[numParticles] ;
  double space_x1, space_x2, space_y1, space_y2 ;
  space_x1 = space_y1 = -1.0 * radius ;
  space_x2 = space_y2 = 1.0 * radius ;

  count = 0 ;
  file = fopen(argv[1], "r") ;
  if(file)
    while((read = getline(&line, &buffer_size, file)) != -1) {
      if(count == 0 || count == 1)
	count++ ;
      else {
	char *token = strtok(line, " ") ;
	particles[i].x = atof(token) ;

	// next token
	token = strtok(NULL, " ") ;
	particles[i].y = atof(token) ;

	// mass token
	token = strtok(NULL, " ") ;
	token = strtok(NULL, " ") ;
	token = strtok(NULL, " ") ;
	particles[i++].mass = atof(token) ;
      }
    }

  double start = get_time() ;
  root = initQuadTree(space_x1, space_x2, space_y1, space_y2) ;
  buildQuadTree(particles, root, numParticles) ;
  double end = get_time() ;
  double treeConstructionTime = end - start ;
  printf("Quad Tree (Quadrant (x1, x2, y1, y2) : Particle (x, y, mass)): \n") ;
  printQuadTree(root) ;
  printf("\n") ;
  
  start = get_time() ;
  computeMassDistribution(root) ;
  calculateForce(particles, numParticles) ;
  end = get_time() ;
  double forceCalculationTime = end - start ;
  printf("Force (Particle (x, y, mass) : Force(e-11) (x, y)): \n") ;
  printForce(particles, numParticles) ;
  
  printf("\nTime for:\n1. Tree Construction: %lf\n2. Force Calculation: %lf\n\n", treeConstructionTime, forceCalculationTime) ;
  printf("Total time for serial Barnes Hut: %lf\n", treeConstructionTime + forceCalculationTime) ;
  
  fclose(file) ;
  return 0 ;
}
