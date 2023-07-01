#ifndef GAMENITE_CONTROLPADS_H
#define GAMENITE_CONTROLPADS_H

#include "controlpads.h"

#include <godot_cpp/classes/node.hpp>
#include <queue>

struct client_msg {
    godot::String client;
    godot::String msg;
};

namespace godot {

class GameNiteControlpads : public Node {
    GDCLASS(GameNiteControlpads, Node)

private:
    struct c_string_vec new_messages;
    std::queue<client_msg> messages;
    struct c_string_vec clients;
    void send_client_message(String client, String message);

protected:
    static void _bind_methods();

public:
    GameNiteControlpads();
    ~GameNiteControlpads();

    void _process(double delta);
};

}

#endif
