#include "pipe.h"
#include "utils.h"
#include "message.h"

void confirm_balance_value(BalanceState *state, BalanceHistory *history, timestamp_t updated_at, timestamp_t time) {
    for (int i = updated_at + 1; i <= time; ++i) {
        history->s_history[i].s_time = i;
        history->s_history[i].s_balance
            = ( i!= time) ? history->s_history[updated_at].s_balance :  state->s_balance;
    }
}

void get_balance(BalanceState *state, balance_t balance) {
    state->s_balance = balance;
    state->s_balance_pending_in = 0;
    state->s_time = get_physical_time();
}

void generate_pipe(branch_data_struct *pipe_data, const balance_t balances[]) {
    for (int i = 1; i < pipe_data->pipe_len; ++i) {
        pipe_data->id = i;
        pipe_data->balance = balances[i];
        fflush(stdout);
        if (fork() == 0) {
            close_pipe_for(pipe_data->descriptors, i, pipe_data->pipe_len);
            run(pipe_data);
        }
    }
}


void run(branch_data_struct *pipe_data) {
    char payload[MAX_PAYLOAD_LEN];
    timestamp_t confirm_date = 0;

    BalanceHistory history;
    history.s_id = pipe_data->id;
    BalanceState initial_balance;
    get_balance(&initial_balance, pipe_data->balance);
    history.s_history[0] = initial_balance;

    Message START_MESSAGE;
    generate_empty_payload(&START_MESSAGE, payload, STARTED);
    send_multicast(pipe_data, &START_MESSAGE);
    log_start_event(pipe_data->id, payload, get_physical_time(), pipe_data->balance);

    Message START_RECEIVE_MESSAGE;
    receive_from_all_tasks(pipe_data, &START_RECEIVE_MESSAGE);
    log_message_start_receiving(pipe_data->id, payload, get_physical_time());

    int do_loop = 1;

    while (do_loop) {
        Message IN_TRANSFER_MESSAGE;
        Message CONFIRM_MESSAGE;
        receive_any(pipe_data, &IN_TRANSFER_MESSAGE);

        int was_stopped = IN_TRANSFER_MESSAGE.s_header.s_type == STOP;
        int is_transfer = IN_TRANSFER_MESSAGE.s_header.s_type == TRANSFER;

        if (was_stopped) {
            do_loop = 0;
            history.s_history_len = get_physical_time() + 1;
            int date_is_valid = history.s_history_len > (confirm_date + 1);
            if (date_is_valid){
                BalanceState  state;
                get_balance(&state, pipe_data->balance);
                confirm_balance_value(&state, &history, confirm_date, state.s_time);
            }

            continue;
        }

        if (is_transfer) {
            TransferOrder order;
             
            memcpy(&order, IN_TRANSFER_MESSAGE.s_payload, IN_TRANSFER_MESSAGE.s_header.s_payload_len);


            if (order.s_src == pipe_data->id) {
                BalanceState current_balance;

                pipe_data->balance -= order.s_amount;
                get_balance(&current_balance, pipe_data->balance);
                confirm_balance_value(&current_balance, &history, confirm_date, current_balance.s_time);
                
                send(pipe_data, order.s_dst, &IN_TRANSFER_MESSAGE);

                log_transfer_outcoming_event(get_physical_time(), pipe_data->id, order.s_amount, order.s_dst);
                confirm_date = current_balance.s_time;
                continue;
            }
            
                pipe_data->balance += order.s_amount;
                BalanceState current_balance;
                
                get_balance(&current_balance, pipe_data->balance);
                confirm_balance_value(&current_balance, &history, confirm_date, current_balance.s_time);
                confirm_date = current_balance.s_time;
                log_transfer_incoming_event(get_physical_time(), order.s_src, order.s_amount, pipe_data->id);

                generate_confirm_payload(&CONFIRM_MESSAGE);
                send(pipe_data, PARENT_ID, &CONFIRM_MESSAGE);
        }
    }

    Message DONE_MESSAGE;
    Message DONE_RECEIVING_MESSAGE;
    Message HISTORY_RECORD_MESSAGE;


    log_done_event(pipe_data->id, payload, get_physical_time(), pipe_data->balance);
    generate_empty_payload(&DONE_MESSAGE, payload, DONE);
    send_multicast(pipe_data, &DONE_MESSAGE);

    receive_from_all_tasks(pipe_data, &DONE_RECEIVING_MESSAGE);
    log_end_receive_event(pipe_data->id, payload, get_physical_time());

    generate_history_payload(&HISTORY_RECORD_MESSAGE, &history);
    send(pipe_data, PARENT_ID, &HISTORY_RECORD_MESSAGE);

    close_all_pipes(pipe_data->descriptors, pipe_data->pipe_len, pipe_data->id);

    exit(0);
}

