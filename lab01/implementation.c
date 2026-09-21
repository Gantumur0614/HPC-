#include <stdio.h> 
#include <time.h>
#include <stdlib.h> 
#include <omp.h>

typedef struct {
    double x, y, z;
    double mass;
} Aos;

typedef struct {
    double* x;
    double* y;
    double* z;
    double* mass;
} Soa;

double speed_up(double duration_1, double duration_n) {
    return duration_1 / duration_n;
}

double parallel_section(double speed_up_val, int n_processor) {
    return (n_processor * (speed_up_val - 1.0)) / (speed_up_val * (n_processor - 1.0)); 
}

double amdahl_law(int n_processor, double p) {
    double s = 1.0 - p;
    return 1.0 / (s + (p / n_processor));
}

int main() {
    int N[] = {1000000, 10000000, 50000000};
    int threads_list[] = {1, 2, 4, 8, 16};
    int num_threads_cases = 5;

    FILE *fp = fopen("benchmark_results.csv", "w");
    if (fp != NULL) {
        fprintf(fp, "N,Type,Access,Threads,Time,Speedup,Parallel_P\n");
    }

    for (int t = 0; t < 3; t++) {
        int n = N[t];

        Aos *aos = (Aos *)malloc(n * sizeof(Aos));
        for (int i = 0; i < n; i++) {
            aos[i].x = 1.0; aos[i].y = 1.0; aos[i].z = 1.0; aos[i].mass = 1.0;
        }
        
        Soa soa;
        soa.x = (double *)malloc(n * sizeof(double));
        soa.y = (double *)malloc(n * sizeof(double));
        soa.z = (double *)malloc(n * sizeof(double));
        soa.mass = (double *)malloc(n * sizeof(double));
        for (int i = 0; i < n; i++) {
            soa.x[i] = 1.0; soa.y[i] = 1.0; soa.z[i] = 1.0; soa.mass[i] = 1.0;
        }

        int *index = (int *)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            index[i] = rand() % n;
        }

        double t1_aos_seq = 0.0, t1_soa_seq = 0.0;
        double t1_aos_rand = 0.0, t1_soa_rand = 0.0;

        for (int idx = 0; idx < num_threads_cases; idx++) {
            int th = threads_list[idx];

            double start = omp_get_wtime();
            double sum_aos = 0;
            #pragma omp parallel for reduction(+:sum_aos) num_threads(th)
            for (int i = 0; i < n; i++) {
                sum_aos += aos[i].mass;
            } 
            double end = omp_get_wtime(); 
            double time_aos_seq = end - start;
            if (th == 1) t1_aos_seq = time_aos_seq;
            double su_aos_seq = speed_up(t1_aos_seq, time_aos_seq);
            double p_aos_seq = (th > 1) ? parallel_section(su_aos_seq, th) : 0.0;

            printf("sequential_aos_%d_th%d: %f\n", n, th, time_aos_seq);
            if (fp) fprintf(fp, "%d,AoS,Sequential,%d,%f,%f,%f\n", n, th, time_aos_seq, su_aos_seq, p_aos_seq);

            start = omp_get_wtime();
            double sum_soa = 0;
            #pragma omp parallel for reduction(+:sum_soa) num_threads(th)
            for (int i = 0; i < n; i++) {
                sum_soa += soa.mass[i];
            }
            end = omp_get_wtime();
            double time_soa_seq = end - start;
            if (th == 1) t1_soa_seq = time_soa_seq;
            double su_soa_seq = speed_up(t1_soa_seq, time_soa_seq);
            double p_soa_seq = (th > 1) ? parallel_section(su_soa_seq, th) : 0.0;

            printf("sequential_soa_%d_th%d: %f\n", n, th, time_soa_seq);
            if (fp) fprintf(fp, "%d,SoA,Sequential,%d,%f,%f,%f\n", n, th, time_soa_seq, su_soa_seq, p_soa_seq);

            start = omp_get_wtime();
            sum_aos = 0;
            #pragma omp parallel for reduction(+:sum_aos) num_threads(th)
            for (int i = 0; i < n; i++) {
                sum_aos += aos[index[i]].mass;
            }
            end = omp_get_wtime();
            double time_aos_rand = end - start;
            if (th == 1) t1_aos_rand = time_aos_rand;
            double su_aos_rand = speed_up(t1_aos_rand, time_aos_rand);
            double p_aos_rand = (th > 1) ? parallel_section(su_aos_rand, th) : 0.0;

            printf("random_aos_%d_th%d: %f\n", n, th, time_aos_rand);
            if (fp) fprintf(fp, "%d,AoS,Random,%d,%f,%f,%f\n", n, th, time_aos_rand, su_aos_rand, p_aos_rand);

            start = omp_get_wtime();
            sum_soa = 0;
            #pragma omp parallel for reduction(+:sum_aos) num_threads(th)
            for (int i = 0; i < n; i++) {
                sum_soa += soa.mass[index[i]];
            }
            end = omp_get_wtime();
            double time_soa_rand = end - start;
            if (th == 1) t1_soa_rand = time_soa_rand;
            double su_soa_rand = speed_up(t1_soa_rand, time_soa_rand);
            double p_soa_rand = (th > 1) ? parallel_section(su_soa_rand, th) : 0.0;

            printf("random_soa_%d_th%d: %f\n", n, th, time_soa_rand);
            if (fp) fprintf(fp, "%d,SoA,Random,%d,%f,%f,%f\n", n, th, time_soa_rand, su_soa_rand, p_soa_rand);
        }

        free(aos);
        free(soa.x); 
        free(soa.y);
        free(soa.z);
        free(soa.mass);
        free(index);
    }

    if (fp) fclose(fp);
    return 0;
}