#include "stdio.h"
#include "unistd.h"
#include <fcntl.h>
#include "pipe.h"

FILE *pipes_log_file;

void close_all_pipes(branch_info_struct *, int, local_id);
void close_pipe_for(branch_info_struct *, local_id id, int);
void exit_from_parent_pipe(branch_info_struct *, int);
