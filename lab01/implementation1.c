#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 

typedef struct {
    double x, y, z;
    double mass;
} ParticlesAos;

typedef struct {
    double* x;
    double* y;
    double* z;
    double* mass;
} ParticlesSoa;

typedef struct {
    double aos_seq_all;
    double aos_seq_mass;
    double aos_rand_all;
    double aos_rand_mass;
    
    double soa_seq_all;
    double soa_seq_mass;
    double soa_rand_all;
    double soa_rand_mass;
} Time_table;

int *make_random_indices(int n) {
    if (n <= 0) {
        return NULL;
    }

    static int seeded = 0; 
    if (!seeded) {
        srand(time(NULL));
        seeded = 1;
    }

    int *arr = (int *)malloc(n * sizeof(int));
    if (arr == NULL) {
        return NULL;
    }

    for (int i = 0;i < n;i++) {
        arr[i] = i;
    }

    for (int i = n - 1;i > 0;i--) {
        int j = rand() % (i + 1);
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    };
    return arr;
}

double calcualte_avg_time(double summed_val, int total_run_num) {
    return summed_val / total_run_num;
}

double sum_aos_seq_all(ParticlesAos *p, int n) {
    double sum = 0.0;
    for (int i = 0;i < n;i++) {
        sum += p[i].x + p[i].y + p[i].z + p[i].mass;
    }
    return sum;
}

double sum_aos_seq_mass(ParticlesAos *p, int n) {
    double sum = 0.0;
    for (int i = 0;i < n;i++) {
        sum += p[i].mass;
    }
    return sum;
}

double sum_aos_rand_all(ParticlesAos *p, const int *idx, int n) {
    double sum = 0.0;
    for (int i = 0;i < n;i++) {
        sum += p[idx[i]].x + p[idx[i]].y + p[idx[i]].z + p[idx[i]].mass;
    }
    return sum;
}

double sum_aos_rand_mass(ParticlesAos *p, const int *idx, int n) {
    double sum = 0.0;
    for (int i = 0;i < n;i++) {
        sum += p[idx[i]].mass;
    }
    return sum;
}


double sum_soa_seq_all(ParticlesSoa p, int n) {
    double sum = 0.0;
    for (int i = 0;i < n;i++) {
        sum += p.x[i] + p.y[i] + p.z[i] + p.mass[i];
    }
    return sum;
}

double sum_soa_seq_mass(ParticlesSoa p, int n) {
    double sum = 0.0;
    for (int i = 0;i < n;i++) {
        sum += p.mass[i];
    }
    return sum;
}

double sum_soa_rand_all(ParticlesSoa p, const int *idx, int n) {
    double sum = 0.0;
    for (int i = 0;i < n;i++) {
        sum += p.x[idx[i]] + p.y[idx[i]] + p.z[idx[i]] + p.mass[idx[i]];
    }
    return sum;
}

double sum_soa_rand_mass(ParticlesSoa p, const int *idx, int n) {
    double sum = 0.0;
    for (int i = 0;i < n;i++) {
        sum += p.mass[idx[i]];
    }
    return sum;
}


int main() {
    int total_run_num = 5;
    int N_values[] = {1000000, 10000000, 50000000, 100000000};
    int len_n = sizeof(N_values) / sizeof(N_values[0]);

    int **random_indices = malloc(len_n * sizeof(int *));
    Time_table *table = (Time_table *)malloc(len_n * sizeof(Time_table));
    
    for (int t = 0;t < len_n;t++) {
        table[t].aos_seq_all = 0.0;
        table[t].aos_rand_all = 0.0;
        table[t].aos_seq_mass = 0.0;
        table[t].aos_rand_mass = 0.0;

        table[t].soa_seq_all = 0.0;
        table[t].soa_rand_all = 0.0;
        table[t].soa_seq_mass = 0.0;
        table[t].soa_rand_mass = 0.0;

        random_indices[t] = make_random_indices(N_values[t]);
    }

    for (int t = 0; t < len_n;t++) {
        int n = N_values[t];

        for (int run = 0;run < total_run_num;run++) {
            ParticlesAos *aos = (ParticlesAos *)malloc(n * sizeof(ParticlesAos));
            for (int i = 0;i < n;i++) {
                aos[i].x = 1.0; 
                aos[i].y = 2.0;
                aos[i].z = 3.0;
                aos[i].mass = 1.5;
            }

            ParticlesSoa soa;
            soa.x = (double *)malloc(n * sizeof(double));
            soa.y = (double *)malloc(n * sizeof(double));
            soa.z = (double *)malloc(n * sizeof(double));
            soa.mass = (double *)malloc(n * sizeof(double));
            
            for (int i = 0;i < n;i++) {
                soa.x[i] = 1.0;
                soa.y[i] = 2.0;
                soa.z[i] = 3.0;
                soa.mass[i] = 1.5;
            }
            
            // seqial accessing 
            double sum = 0.0;
            clock_t start, end;

            start = clock();
            sum_aos_seq_all(aos, n);
            end = clock();
            table[t].aos_seq_all += (double)(end - start) / CLOCKS_PER_SEC;

            sum = 0.0;
            start = clock();
            sum_aos_seq_mass(aos, n);
            end = clock();
            table[t].aos_seq_mass += (double)(end - start) / CLOCKS_PER_SEC;

            sum = 0.0;
            start = clock();
            sum_soa_seq_all(soa, n);
            end = clock();
            table[t].soa_seq_all += (double)(end - start) / CLOCKS_PER_SEC; 

            sum = 0.0;
            start = clock();
            sum_soa_seq_mass(soa, n);
            end = clock();
            table[t].soa_seq_mass += (double)(end - start) / CLOCKS_PER_SEC;

            // Random accessing 
            sum = 0.0;
            start = clock();
            sum_aos_rand_all(aos, random_indices[t], n);
            end = clock();
            table[t].aos_rand_all += (double)(end - start) / CLOCKS_PER_SEC;

            sum = 0.0;
            start = clock();
            sum_aos_rand_mass(aos, random_indices[t], n);
            end = clock();
            table[t].aos_rand_mass += (double)(end - start) / CLOCKS_PER_SEC;

            sum = 0.0;
            start = clock();
            sum_soa_rand_all(soa, random_indices[t], n);
            end = clock();
            table[t].soa_rand_all += (double)(end - start) / CLOCKS_PER_SEC;

            sum = 0.0;
            start = clock();
            sum_soa_rand_mass(soa, random_indices[t], n);
            end = clock();
            table[t].soa_rand_mass += (double)(end - start) / CLOCKS_PER_SEC;

            free(aos);
            free(soa.x);
            free(soa.y);
            free(soa.z);
            free(soa.mass);
        }

        table[t].aos_seq_all = calcualte_avg_time(table[t].aos_seq_all, total_run_num);
        table[t].aos_rand_all = calcualte_avg_time(table[t].aos_rand_all, total_run_num);
        table[t].aos_seq_mass = calcualte_avg_time(table[t].aos_seq_mass, total_run_num);
        table[t].aos_rand_mass = calcualte_avg_time(table[t].aos_rand_mass, total_run_num);

        table[t].soa_seq_all = calcualte_avg_time(table[t].soa_seq_all, total_run_num);
        table[t].soa_rand_all = calcualte_avg_time(table[t].soa_rand_all, total_run_num);
        table[t].soa_seq_mass = calcualte_avg_time(table[t].soa_seq_mass, total_run_num);
        table[t].soa_rand_mass = calcualte_avg_time(table[t].soa_rand_mass, total_run_num);
    }   

    FILE *result = fopen("result.csv", "w");
    if (result) {
        fprintf(result, "Арга,N=1M (s),N=10M (s),N=50M (s),N=100M\n");

        const char *row_titles[] = {
            "aos_seq_all",
            "aos_seq_mass",
            "aos_rand_all",
            "aos_rand_mass",
            "soa_seq_all",
            "soa_seq_mass",
            "soa_rand_all",
            "soa_rand_mass"
        };

        for (int row = 0;row < 8;row++) {
            double col1 = 0, col2 = 0, col3 = 0, col4 = 0;

            switch (row) {
                case 0: col1 = table[0].aos_seq_all; col2 = table[1].aos_seq_all; col3 = table[2].aos_seq_all; col4 = table[3].aos_seq_all; break; 
                case 1: col1 = table[0].aos_seq_mass; col2 = table[1].aos_seq_mass; col3 = table[2].aos_seq_mass; col4 = table[3].aos_seq_mass; break; 
                case 2: col1 = table[0].aos_rand_all; col2 = table[1].aos_rand_all; col3 = table[2].aos_rand_all; col4 = table[3].aos_rand_all; break; 
                case 3: col1 = table[0].aos_rand_mass; col2 = table[1].aos_rand_mass; col3 = table[2].aos_rand_mass; col4 = table[3].aos_rand_mass; break;
                case 4: col1 = table[0].soa_seq_all; col2 = table[1].soa_seq_all; col3 = table[2].soa_seq_all; col4 = table[3].soa_seq_all; break; 
                case 5: col1 = table[0].soa_seq_mass; col2 = table[1].soa_seq_mass; col3 = table[2].soa_seq_mass; col4 = table[3].soa_seq_mass; break; 
                case 6: col1 = table[0].soa_rand_all; col2 = table[1].soa_rand_all; col3 = table[2].soa_rand_all; col4 = table[3].soa_rand_all; break; 
                case 7: col1 = table[0].soa_rand_mass; col2 = table[1].soa_rand_mass; col3 = table[2].soa_rand_mass; col4 = table[3].soa_rand_mass; break;
            }
            fprintf(result, "\"%s\",%.6f,%.6f,%.6f,%.6f\n", row_titles[row], col1, col2, col3, col4);
        }

        fclose(result);
    }
    
    for (int t = 0;t < len_n;t++) {
        free(random_indices[t]);
    }
    free(random_indices);
    free(table);
    
    return 0;
}