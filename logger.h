#ifndef LAB1_LOGS_H
#define LAB1_LOGS_H
#include "stdio.h"

#include "ipc.h"
#include "pa2345.h"
#include "banking.h"

FILE *events_log_file;

void log_transfer_incoming_event(timestamp_t, local_id src, balance_t, local_id dst);
void log_transfer_outcoming_event(timestamp_t, local_id src, balance_t, local_id dst);

void log_open_pipe_event(int, char *, int, int);
void log_close_pipe_event(int, local_id);

void log_start_event(local_id, char *, timestamp_t, balance_t);
void log_done_event(local_id, char *, timestamp_t, balance_t);
void log_message_start_receiving(local_id, char *, timestamp_t);
void log_end_receive_event(local_id, char *, timestamp_t);


#endif //LAB1_LOGS_H
