
#ifndef handle_messages_h
#define handle_messages_h

void join_node(t_node_info *my_node);
void joining_node(t_node_info *my_node, int newfd, char buffer[128]);
void update_sucessor(t_node_info *my_node);
void receive_from_succ(t_node_info *my_node);
void new_succ(t_node_info *my_node, char buffer[128]);
void new_sec_succ(t_node_info *my_node, char buffer[128]);
void warn_pred(t_node_info *my_node);
void warn_sec_succ(t_node_info *my_node);
void receive_message(t_node_info *my_node);
void new_pred(t_node_info *my_node, int newfd, char buffer[128]);

#endif