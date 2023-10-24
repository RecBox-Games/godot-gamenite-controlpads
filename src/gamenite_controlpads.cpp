#include <iostream>
#include "controlpads.h"
#include "gamenite_controlpads.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

char* createNewCString(const char* original) {
    // Determine the length of the original C-string
    size_t length = std::strlen(original);
    
    // Allocate new memory for the new C-string
    char* newCString = new char[length + 1];
    
    // Copy content from the original C-string to the new C-string
    std::strcpy(newCString, original);
    
    return newCString;
}

void GameNiteControlpads::send_client_message(String client, String message) {
    CharString client_buf = client.ascii();
    const char * c_client = client_buf.get_data();
    CharString message_buf = message.ascii();
    const char * c_message = message_buf.get_data();
    printf("sending <%s> to <%s>\n", c_message, c_client);
    fflush(stdout);
    Err ret = send_message(c_client, c_message);
    CP_CHECK(ret);
}


void GameNiteControlpads::_bind_methods() {
    ADD_SIGNAL(MethodInfo("message_received",
                          PropertyInfo(Variant::STRING, "client"),
                          PropertyInfo(Variant::STRING, "message")));
    ClassDB::bind_method(D_METHOD("send_message", "client", "message"), &GameNiteControlpads::send_client_message);
}

GameNiteControlpads::GameNiteControlpads() {
    CP_CHECK(get_client_handles(&this->clients));
}

GameNiteControlpads::~GameNiteControlpads() {
    // Add your cleanup here.
}

void GameNiteControlpads::_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    // ---- clients ----
    bool changed;
    CP_CHECK(clients_changed(&changed));
    if (changed) {
        printf("clients changed\n");
        free_strvec(this->clients);
        CP_CHECK(get_client_handles(&this->clients));
    }

    // ---- messages ----
    // loop through each client
    for (int i = 0; i < this->clients.len; i++) {
        // get the messages meant for client i
        CP_CHECK(get_messages(clients.ptr[i], &this->new_messages));
        // copy message contents into strings owned by this C++ code
        int j;
        for (j = 0; j < this->new_messages.len; j++) {
            printf("received <%s> from <%s>\n", new_messages.ptr[j], clients.ptr[i]);
            // Warning: potential memory leak
            char* persistent_c_client = createNewCString(clients.ptr[i]);
            String client = String(persistent_c_client);
            // Warning: potential memory leak
            char* persistent_c_msg = createNewCString(new_messages.ptr[j]);
            String msg = String(persistent_c_msg);
            struct client_msg cm = client_msg { client, msg };
            this->messages.push(cm);
        }

        /*
        if (j != 0) {
            printf("client<%s>\n", clients.ptr[i]);
            for (j = 0; j < this->new_messages.len; j++) {
                printf("    msg<%s>\n", new_messages.ptr[j]);
            }
        }
        */

        // now that we have copies, tell the library (Rust code) to free the
        // original messages
        free_strvec(this->new_messages);
    }

    // ---- emit ----
    // we only output one message at a time even if we got multiple messages
    // this frame (TODO: we may need to change this later to improve
    // performance)
    if (this->messages.size() > 0) {
        struct client_msg cm = this->messages.front();
        this->messages.pop();
        emit_signal("message_received", cm.client, cm.msg);
    }
}
