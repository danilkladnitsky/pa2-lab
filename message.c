#include "message.h"
#include <string.h>

void receive_from_all_tasks(branch_data_struct *pipe_data, Message *message) {
    for (int i = 1; i < pipe_data->pipe_len; ++i) {
        int is_receiving = i != pipe_data->id;
        if (is_receiving){
            while (1) {
                if (receive(pipe_data, i, message) == 0) break;
            }
        }
    }
}

void generate_confirm_payload(Message *message) {
    message->s_header.s_local_time = get_physical_time();

    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_payload_len = 0;
    message->s_header.s_type = ACK;
}

void generate_transfer_payload(Message *message, TransferOrder *transferOrder) {
    memcpy(message->s_payload, transferOrder, sizeof *transferOrder);
    message->s_header.s_local_time = get_physical_time();


    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_type = TRANSFER;
    message->s_header.s_payload_len = sizeof *transferOrder;
}

void add_data_to_stop_message(Message *message) {
    message->s_header.s_local_time = get_physical_time();

    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_payload_len = 0;
    message->s_header.s_type = STOP;
}

void generate_history_payload(Message *message, BalanceHistory *balanceHistory) {
    memcpy(message->s_payload, balanceHistory, sizeof *balanceHistory);

    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_type = BALANCE_HISTORY;
    message->s_header.s_payload_len = sizeof *balanceHistory;
    message->s_header.s_local_time = get_physical_time();
}


void generate_empty_payload(Message *message, char *payload, MessageType type) {
    strcpy(message->s_payload, payload);

    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_type = type;
    message->s_header.s_local_time = get_physical_time();
    message->s_header.s_payload_len = strlen(payload);
}
