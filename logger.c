#include "utils.h"

void log_transfer_outcoming_event(timestamp_t date, local_id from_id, balance_t money, local_id to_id) {
    fprintf(events_log_file, log_transfer_out_fmt, date, from_id, money, to_id);
}

static const char *const log_open_pipe_descr =
        "pipe #%d was open %s -> %d -> %d\n";
static const char *const log_close_pipe_descr =
        "pipe #%d was closed in #%d\n";

void log_transfer_incoming_event(timestamp_t date, local_id from_id, balance_t money, local_id to_id) {
    fprintf(events_log_file, log_transfer_in_fmt, date, to_id, money, from_id);
    fflush(events_log_file);
    printf(log_transfer_in_fmt, date, from_id, money, to_id);
    fflush(stdout);
}

void log_done_event(local_id id, char *doneMessage, timestamp_t date, balance_t balance) {
    sprintf(doneMessage, log_done_fmt, date, id, balance);
    fprintf(events_log_file, "%s", doneMessage);
}

void log_message_start_receiving(local_id id, char *payload, timestamp_t date) {
    sprintf(payload, log_received_all_started_fmt, date, id);
    fprintf(events_log_file, "%s", payload);
}



void log_start_event(local_id id, char *payload, timestamp_t date, balance_t balance ) {
    sprintf(payload, log_started_fmt, date, id, getpid(), getppid(), balance);
    fprintf(events_log_file, "%s", payload);
}

void log_end_receive_event(local_id id, char *payload, timestamp_t date) {
    sprintf(payload, log_received_all_done_fmt, id, date);
    fprintf(events_log_file, "%s", payload);
}

void log_open_pipe_event(int fd, char *type, int from, int to) {
    fprintf(pipes_log_file, log_open_pipe_descr, fd, type, from, to);
}

void log_close_pipe_event(int fd, local_id id) {
    fprintf(pipes_log_file, log_close_pipe_descr, fd, id);
}

