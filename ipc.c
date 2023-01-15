#include "stdio.h"
#include <unistd.h>

#include "ipc.h"
#include "main.h"

int send(void *self, local_id destination, const Message *message) {
    branch_data_struct *pipe_data = (branch_data_struct *) self;
    return write(pipe_data->descriptors->descriptors[pipe_data->id][destination][W_FLAG], message,
                          sizeof(message->s_header) + message->s_header.s_payload_len);
}

int receive(void *self, local_id sender, Message *message) {
    size_t size = sizeof(message->s_header);
    branch_data_struct *pipe_data = (branch_data_struct *) self;
    local_id to = pipe_data->id;

    if (read(pipe_data->descriptors->descriptors[sender][to][R_FLAG], &message->s_header, size) > 0) {
        read(pipe_data->descriptors->descriptors[sender][to][R_FLAG], &message->s_payload, message->s_header.s_payload_len);
        fflush(stdout);
        return 0;
    } else {
        fflush(stdout);
        return -1;
    }
}

int receive_any(void *self, Message *message) {
    branch_data_struct *pipe_data = (branch_data_struct *) self;
    while (1) {
        for (int i = 0; i < pipe_data->pipe_len; ++i) {
            if (i != pipe_data->id) {
                int result = receive(self, i, message);
                if (result == 0) {
                    return 0;
                }
            }
        }
    }
}



int send_multicast(void *self, const Message *message) {
    branch_data_struct *pipe_data = (branch_data_struct *) self;

    uint16_t size = pipe_data->pipe_len;
    for (int i = 0; i < size; ++i){
        if (i != pipe_data->id) {
            int result = send(self, i, message);
            if (result == -1) return result;
        }
    }

    return 0;
}
