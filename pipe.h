#ifndef LAB2_PIPES
#define LAB2_PIPES

#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#include "ipc.h"
#include "logger.h"

#define DESC_COUNT 2

#define MAX_TASKS 11

typedef struct {
    int descriptors[MAX_TASKS][MAX_TASKS][2];
} branch_info_struct;

typedef struct {
    branch_info_struct *descriptors;
    int pipe_len;
    local_id id;
    balance_t balance;
} branch_data_struct;

void generate_pipe(branch_data_struct *, const balance_t[]);
void run(branch_data_struct *);

#define R_FLAG 0
#define W_FLAG 1

#endif
