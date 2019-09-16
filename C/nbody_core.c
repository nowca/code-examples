/**
 * @file nbody_core.c
 * @author Jonathan Nowca
 * @date 1 Aug 2018
 * @brief NBody Core file for processing nbody algorithm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include "particles.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <omp.h>


void alloc_mem(int particle_count);
void generate_random_data(ParticleCluster* particle_cluster);
void load_source_data(ParticleCluster* particle_cluster);
char* concat(const char *s1, const char *s2);
char* double_to_char(double d);
void json_add_value(char** buf, double value, int state);

double *BODY_MASSES; /**< Initialization body masses */
double **BODY_POSITIONS; /**< Initialization body positions */

ParticleCluster particle_cluster; /**< Particle cluster for n body processing */

/** @brief Random double number function
 *
 *  Generates a random double number.
 *
 *  @param seed seed for srand function
 *  @return random double number
 */
double get_random(int seed) {
	srand(seed);
	return -1+2*((float)rand())/RAND_MAX;
};

/** @brief Particle memory allocation function
 *
 *  Allocates memory for n particles.
 *
 *  @param sparticle_count number of particles
 */
void alloc_mem(int particle_count) {
    int n = particle_count;

	BODY_MASSES    = (double *)malloc(sizeof(double) * n);
    BODY_POSITIONS = (double**)malloc(sizeof(double*) * n);
};

/** @brief source data loading function
 *
 *  Loads generated source data from BODY_POSITIONS and 
    BODY_MASSES into the given particle cluster.
 *
 *  @param particle_cluster particle cluster to load
 */
void load_source_data(ParticleCluster* particle_cluster) {
	int i;
    for (i = 0; i < particle_cluster->particle_count; i++) {
        Particle* particle = &particle_cluster->particles[i];

        particle->rx = BODY_POSITIONS[i][0];
        particle->ry = BODY_POSITIONS[i][1];
        particle->rz = BODY_POSITIONS[i][2];
        particle->vx = BODY_POSITIONS[i][3];
        particle->vy = BODY_POSITIONS[i][4];
        particle->vz = BODY_POSITIONS[i][5];
        particle->ax = 0.0;
        particle->ay = 0.0;
        particle->az = 0.0;
        particle->m = BODY_MASSES[i];
    }
};

/** @brief Random integer number function
 *
 *  Generates a random integer number out
 *  of the unix /dev/urandom file.
 *
 *  @return random integer number
 */
int getUnixRandomInt() {
    int byte_count = 64;
    int random_numnum;
    char data[64];
    FILE *fp = fopen("/dev/urandom", "r");
    fread(&data, 1, byte_count, fp);
    fclose(fp);

    for (int i = 0; i < strlen(data); i++) {
        random_numnum += data[i] << (i*8);
    }

    return random_numnum > 0 ? random_numnum : -random_numnum;
}

/** @brief Random data generation function
 *
 *  Generates a random data for a given particle cluster.
 *
 *  @see alloc_mem()
 *  @see get_random()
    @see getUnixRandomInt()
 *  @param particle_cluster particle cluster
 */
void generate_random_data(ParticleCluster* particle_cluster) {
    alloc_mem(particle_cluster->particle_count);

    // center coordinates of sphere
    double x0 = 0.0;
    double y0 = 0.0;
    double z0 = 0.0;

    int i;
    for (i = 0; i < particle_cluster->particle_count; i++) {
        double u      = get_random(getUnixRandomInt());
        double v      = get_random(getUnixRandomInt());
        double radius = 1.0;
        double theta  = 2 * M_PI * u;
        double phi    = acos(v);

        double random_x      = x0 + (radius * sin(phi) * cos(theta));
        double random_y      = y0 + (radius * sin(phi) * sin(theta));
        double random_z      = z0 + (radius * cos(phi));
        double random_body[] = {random_x, random_y, random_z, 0.0, 0.0, 0.0};

        BODY_MASSES[i]    = 1.0;
        BODY_POSITIONS[i] = (double *)malloc(sizeof(double) * 6);
        memcpy(BODY_POSITIONS[i], random_body, sizeof(random_body[0]) * 6);
    }
}

int divisor = 1; /**< particle count divisor */

/** @brief NBody initialization function
 *
 *  Generates random data and loads particles.
 *
 *  @param particle_count number of particles
 *  @return particle count divisor
 */
int nbody_init(int particle_count) {
    divisor = (int)((double)particle_count / (double)100);
    particle_cluster = New_ParticleCluster(particle_count);
    generate_random_data(&particle_cluster);
	load_source_data(&particle_cluster);
    return divisor;
};

double G   = 6.673e-11; /**< Gravity constant: 6.673 * Math.pow(10.0,-11.0) */
double eps = 0.1;       /**< Epsilon: softening factor = avoids division through zero on collision */

int c_steps = 0; /**< calculaton step counter */

/** @brief NBody computation function
 *
 *  Computates n body algroithm with OpenMP multiprocessing
 *
 *  @param dt timestep
 *  @param p_max_threads number of used threads
 *  @return calculation step number
 */
int nbody_compute(double dt, int p_max_threads) {
    omp_set_num_threads(p_max_threads);

    #pragma omp parallel
    {
        int thread_num = omp_get_thread_num();
        int steps_per_thread = (int)((double)particle_cluster.particle_count / (double)p_max_threads);
        int i_start = steps_per_thread * thread_num;
        int i_end = i_start + steps_per_thread;

        int i, j = 0;
        for (i = 0; i < particle_cluster.particle_count; i++) {
            Particle* particle_i = &particle_cluster.particles[i];

            particle_i->ax = 0.0;
            particle_i->ay = 0.0;
            particle_i->az = 0.0;

            for (j = i_start; j < i_end; j++) {
                if (j != i) {
                    Particle* particle_j = &particle_cluster.particles[j];

                    double dx = particle_j->rx - particle_i->rx;
                    double dy = particle_j->ry - particle_i->ry;
                    double dz = particle_j->rz - particle_i->rz;

                    double R_ij  = sqrt(dx * dx + dy * dy + dz * dz + eps);
                    double R_ij3 = (R_ij * R_ij * R_ij);

                    double f = G * particle_j->m / R_ij3;

                    // set acceleration
                    particle_i->ax += f * dx;
                    particle_i->ay += f * dy;
                    particle_i->az += f * dz;
                }
            }

            // set velocity
            particle_i->vx += particle_i->ax * dt;
            particle_i->vy += particle_i->ay * dt;
            particle_i->vz += particle_i->az * dt;

            // set position
            particle_i->rx += particle_i->vx * dt;
            particle_i->ry += particle_i->vy * dt;
            particle_i->rz += particle_i->vz * dt;
        }
    }

    c_steps++;

    return c_steps;
}

/** @brief String concatińation function
 *
 *  Concatinates two strings int one.
 *
 *  @param s1 first string
 *  @param s2 second string
 *  @return result ads
 */
char* concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

/** @brief Double to Character conversation function
 *
 *  Convertes a double number into an character array.
 *
 *  @param d double number
 *  @return converted string
 */
char* double_to_char(double d) {
    int len = snprintf(NULL, 0, "%f", d);
    char *str = malloc(len+1);
    snprintf(str, len, "%f", d);
    return str;
}

/** @brief JSON build value addition function
 *
 *  Adds a double value to json building string
 *
 *  @see nbody_get_json_data()
 *  @param buf json build string
 *  @param value double value to add
 *  @param state state to add: 1=open with "[", 0,1=add ",", 2="]"
 */
void json_add_value(char** buf, double value, int state) {
    char* tmp;
    if (state == 0) { tmp = concat(*buf, "["); }
    tmp = concat(tmp, double_to_char(value));
    if (state == 0 || state == 1) { tmp = concat(tmp, ","); }
    if (state == 2) { tmp = concat(tmp, "]"); }
    *buf = tmp;
}

int chunk = 0;

/** @brief JSON string building function
 *
 *  Builds a JSON string out of the processed particle_cluster.
 *
 *  @see json_add_value()
 *  @return buf Converted JSON string
 */
char* nbody_get_json_data() {
    char* buf = "[";
    
    int segment = particle_cluster.particle_count / divisor;

    int i = segment * (chunk);
    int n = segment * (chunk + 1);
    chunk = (chunk + 1) % divisor;

    for (; i < n; i++) {
        Particle* particle = &particle_cluster.particles[i];

        json_add_value(&buf, particle->rx, 0);
        json_add_value(&buf, particle->ry, 1);
        json_add_value(&buf, particle->rz, 2);

        if (i < n-1) { buf = concat(buf, ","); }
        else { buf = concat(buf, "]\0"); }
    }

    return buf;
}