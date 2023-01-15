#include "pipe.h"

void generate_empty_payload(Message *, char *, MessageType);

void generate_transfer_payload(Message *, TransferOrder *);

void generate_confirm_payload(Message *);

void add_data_to_stop_message(Message *);
void receive_from_all_tasks(branch_data_struct *, Message *);
void generate_history_payload(Message *, BalanceHistory *);

