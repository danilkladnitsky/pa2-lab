#include "main.h"

local_id runner_amount;
int task_count = 0;

branch_info_struct pipe_info;
AllHistory history;
branch_data_struct pipe_data;


int close_bank(){
    return fclose(pipes_log_file);
}

void open_log_files() {
    events_log_file = fopen(events_log, "w");
    pipes_log_file = fopen(pipes_log, "w");
}

void open_pipes(branch_info_struct *pipe_info, int task_count) {
    for (int i = 0; i < task_count; ++i) {
        for (int j = 0; j < task_count; ++j) {
            if (i != j) {
                pipe(pipe_info->descriptors[i][j]);
                log_open_pipe_event(pipe_info->descriptors[i][j][R_FLAG], "reading", i, j);
                log_open_pipe_event(pipe_info->descriptors[i][j][W_FLAG], "writing", i, j);
                fcntl(pipe_info->descriptors[i][j][R_FLAG], F_SETFL, O_NONBLOCK);
                fcntl(pipe_info->descriptors[i][j][W_FLAG], F_SETFL, O_NONBLOCK);
            }
        }
    }
}

void transfer(void * parent_data, local_id src, local_id dst, balance_t amount) {
    Message transfer;
    Message CONFIRM_MESSAGE;
    TransferOrder transferOrder;

    transferOrder.s_dst = dst;
    transferOrder.s_src = src;
    transferOrder.s_amount = amount;
    generate_transfer_payload(&transfer, &transferOrder);

    send(parent_data, src, &transfer);

    int loop_condition = 1;
    while (loop_condition) {
        if (receive(parent_data, dst, &CONFIRM_MESSAGE) == 0) break;
    }
}

void wait_for(int runner_amount) {
    for (int i = 0; i < runner_amount; i++) wait(NULL);
}


void setup(){ 
    open_log_files();
    open_pipes(&pipe_info, task_count);

    pipe_data.descriptors = &pipe_info;
    pipe_data.pipe_len = task_count;

    
}

int main(int argc, char *argv[]) {
    Message stop_msg;
    Message payload;

    runner_amount = (int) strtol(argv[2], NULL, 10);
    task_count = runner_amount + 1;
    Message balance_value_message[task_count];

    history.s_history_len = runner_amount;
    balance_t total_balance[runner_amount];

    for (int i = 1; i <= runner_amount; ++i) total_balance[i] = strtol(argv[i + 2], NULL, 10);
    setup();

    generate_pipe(&pipe_data, total_balance);
    exit_from_parent_pipe(pipe_data.descriptors, task_count);

    pipe_data.id = PARENT_ID;
    pipe_data.descriptors = &pipe_info;

    receive_from_all_tasks(&pipe_data, &payload);
    log_message_start_receiving(PARENT_ID, payload.s_payload, get_physical_time());
    bank_robbery(&pipe_data, runner_amount);
    add_data_to_stop_message(&stop_msg);
    send_multicast(&pipe_data, &stop_msg);
    receive_from_all_tasks(&pipe_data, &payload);
    int task_size = task_count - 1;

     int loop_condition = 1;
    for (int i = 1; i <= task_size; ++i){
        while (loop_condition) {
            int do_break = receive(&pipe_data, i, &balance_value_message[i]) == 0;
            if (do_break)
                break;
        }

    }

    int history_size = runner_amount - 1;
    for (int i = 0; i <= history_size; ++i){
        BalanceHistory balance;
        memcpy(&balance, &balance_value_message[i + 1].s_payload, sizeof(BalanceHistory));
        history.s_history[i] = balance;
    }

    print_history(&history);
    wait_for(runner_amount);
    close_all_pipes(&pipe_info, task_count, PARENT_ID);
    return close_bank();
}
