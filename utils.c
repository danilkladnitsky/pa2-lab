#include "utils.h"

void exit_from_parent_pipe(branch_info_struct *pipe_info, int tasks_list_size) {
    for (int i = 0; i < tasks_list_size; ++i) {
        for (int j = 0; j < tasks_list_size; ++j) {
            if (j != i){
                int not_parent = j != PARENT_ID && i != PARENT_ID;

                int is_parent = j == PARENT_ID;
                if (not_parent){
                    close(pipe_info->descriptors[i][j][W_FLAG]);

                    close(pipe_info->descriptors[i][j][R_FLAG]);
                }
                else{
                    if (is_parent) {
                        close(pipe_info->descriptors[i][j][W_FLAG]);
                        return;
                    }
                        close(pipe_info->descriptors[i][j][R_FLAG]);
                }
            }
        }
    }
}

void close_all_pipes(branch_info_struct *pipe_info, int tasks_list_size, local_id id) {
    for (int i = 0; i < tasks_list_size; ++i) {
        for (int j = 0; j < tasks_list_size; ++j) {
            if(i == j){
                continue;
            }
                close(pipe_info->descriptors[i][j][R_FLAG]);

                close(pipe_info->descriptors[i][j][W_FLAG]);
        }
    }
}



void close_pipe_for(branch_info_struct *pipe_info, local_id id, int tasks_list_size) {
    for (int i = 0; i < tasks_list_size; ++i) {
        for (int j = 0; j < tasks_list_size; ++j) {
            if(i == j){
                continue;
            }

                if (i == id && i >= 1) {
                    close(pipe_info->descriptors[i][j][R_FLAG]);
                    return;
                } else {
                    if (j == id) {
                        close(pipe_info->descriptors[i][j][W_FLAG]);
                        return;
                    }
                        close(pipe_info->descriptors[i][j][R_FLAG]);
                        close(pipe_info->descriptors[i][j][W_FLAG]);
                }
        }
    }
}
