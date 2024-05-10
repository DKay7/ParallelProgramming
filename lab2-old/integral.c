#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

enum errors
{
    E_SUCCESS  = 0,
    E_ERROR    = -1
};

const double START = 0.000001; // Start of the integration interval
// const double END   = 1000000.0; // End of the integration interval
const double END   = 1.0; // End of the integration interval
const double DOUBLE_PREC = 0.00000000001; // Precision used to compare doubles
const size_t SCALE_PARAM = 100; // Scaling parameter used to determine the number of tasks


typedef struct task
{
    double start_diap; // Start of the integration interval for this task
    double end_diap; // End of the integration interval for this task
    double result; // Result of the integration for this task
    double eps; // Desired level of precision for this task
} task;

typedef struct thread_info
{
    task* calc_tasks; // array of tasks
    pthread_mutex_t* mutex; // mutex used for synchronization
    size_t* next; // index of the next task to be processed
    size_t num_tasks; // Total number of tasks
} thread_info;

void func(double* res, double x);
int integral(double* ans, double start, double end, double start_val, double end_val, double prec);
int cmp_double(double a, double b);
void* thread_routine(void* data);

void output_results(int size, double t) {
    // printf("%d %lg\n", size, t);
    FILE *fp;
    fp = fopen("results.csv", "a");
    // fprintf(fp, "n,time\n");
    fprintf(fp, "%d,%lg\n", size, t);
    fclose(fp);
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("Bad number input arguments. Try ./integral.out [num_thread] [precision]\n");
        return 0;
    }

    errno = 0;
    unsigned long num_threads = strtol(argv[1], NULL, 10);
    errno = 0;
    double prec = strtod(argv[2], NULL);
    if (errno < 0)
    {
        perror("[main] Getting precision returned error\n");
        exit(EXIT_FAILURE);
    }

    size_t num_tasks = num_threads * SCALE_PARAM;
    errno = 0;
    task* task_arr = (task*) malloc(sizeof(*task_arr) * num_tasks); // array of tasks
    assert(task_arr);

    double step = (END - START) / num_tasks;
    //step - diapasone per task, task = n_thr * SCALE_PARAM
    for (size_t i = 0; i < num_tasks; i++)
    {
        task_arr[i].start_diap = START + i * step; // Start of the integration interval for this task
        task_arr[i].end_diap = task_arr[i].start_diap + step; // End of the integration interval for this task
        task_arr[i].result = 0; // Result of the integration for this task
        task_arr[i].eps = prec; // Desired level of precision for this task
    }

    size_t next_task = 0; // Index of the next task to be processed

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex used for synchronization

    errno = 0;
    thread_info* thread_info_arr = (thread_info*) malloc(sizeof(*thread_info_arr) * num_threads);
    assert(task_arr);

    errno = 0;
    pthread_t* thread_arr = (pthread_t*) malloc(sizeof(*thread_arr) * num_threads); // array of threads
    assert(task_arr);


    clock_t start_time = clock(); // Start time of the program
    for (size_t i = 0; i < num_threads; i++)
    {
        thread_info_arr[i].mutex = &mutex; // mutex used for synchronization
        thread_info_arr[i].calc_tasks = task_arr; // array of tasks
        thread_info_arr[i].next = &next_task; // Index of the next task to be processed
        thread_info_arr[i].num_tasks = num_tasks; // Total number of tasks

        errno = 0;
        int ret = pthread_create(&thread_arr[i], NULL, thread_routine, &thread_info_arr[i]);
        if (ret < 0)
        {
            perror("[main] can't start thread routine\n");
            exit(0);
        }
    }

    for (size_t i = 0; i < num_threads; i++)
    {
        pthread_join(thread_arr[i], NULL);
    }

    double answer = 0.0;
    for (size_t i = 0; i < num_tasks; i++) {
        answer += task_arr[i].result;
    }
    clock_t end_time = clock();
    double total_t = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("answer = %lg\n", answer);
    // printf("Total time = %lg seconds\n", total_t);
    output_results(num_threads, total_t);
    return 0;
}

int cmp_double(double a, double b)
{
    double diff = a - b;
    if (diff < 0)
    {
        if (diff >= -DOUBLE_PREC)
            return 0;
        else
            return -1;
    }
    else
    {
        if (diff <= DOUBLE_PREC)
            return 0;
        else
            return 1;
    }
}

void func(double* res, double x)
{
    assert(res && "func");
    if (x != x) // NaN check
    {
        printf("NaN check failed");
        exit(0);
    }
    if (x == 0) // NaN check
    {
        printf("Division by zero check failed");
        exit(0);
    }

    // *res = sin(1 / x / x);
    *res = sin(1 / x);

}

void* thread_routine(void* data)
{
    assert(data);

    thread_info* info = (thread_info*) data;
    task* task_arr = info->calc_tasks; // array of tasks
    pthread_mutex_t* mutex = info->mutex; // mutex used for synchronization
    size_t* next_task = info->next; // index of the next task to be processed
    size_t  max_task_num = info->num_tasks; // Total number of tasks

    size_t curr_task = 0;
    while (*next_task < max_task_num)
    {
        pthread_mutex_lock(mutex);
        curr_task = *next_task;
        if (curr_task >= max_task_num)
        {
            pthread_mutex_unlock(mutex);
            break;
        }
        (*next_task)++;
        pthread_mutex_unlock(mutex);

        double start = task_arr[curr_task].start_diap;
        double end   = task_arr[curr_task].end_diap;

        double start_val = 0.0;
        double end_val   = 0.0;
        func(&start_val, start);
        func(&end_val, end);

        double result = 0.0;
        int ret = integral(&result, start, end, start_val, end_val, task_arr[curr_task].eps);
        if (ret != E_SUCCESS)
        {
            printf("Integral func returned error");
            exit(EXIT_FAILURE);
        }

        task_arr[curr_task].result = result;
    }

    return 0;
}

int integral(double* ans, double start, double end, double start_val, double end_val, double prec)
{
    assert(ans);

    double mid = (start + end) / 2;
    double mid_val = 0.0;
    func(&mid_val, mid);

    double integ = (start_val + end_val) / 2 * (end - start); // Simpson

    double left_integ  = (start_val + mid_val) / 2 * (mid - start);
    double right_integ = (mid_val + end_val) / 2 * (end - mid);
    double prec_integ  = left_integ + right_integ;

    double diff = prec_integ - integ;
    if (diff < 0)
        diff = -diff;

    double check_val = prec * integ;
    if (check_val < 0)
        check_val = -check_val;

    if (cmp_double(diff, check_val) <= 0)
    {
        *ans = integ;
        return E_SUCCESS;
    }

    int ret = integral(&left_integ, start, mid, start_val, mid_val, prec);
    if (ret != E_SUCCESS)
    {
        printf("Integral func returned error");
        exit(EXIT_FAILURE);
    }

    ret = integral(&right_integ, mid, end, mid_val, end_val, prec);
    if (ret != E_SUCCESS)
    {
        printf("Integral func returned error");
        exit(EXIT_FAILURE);
    }

    *ans = left_integ + right_integ;

    return E_SUCCESS;
}