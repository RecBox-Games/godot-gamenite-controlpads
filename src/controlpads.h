#pragma once

#include <cstdint>

struct c_string_vec {
  char **ptr;
  uint64_t len;
  uint64_t cap;
};

typedef uint64_t Err;

extern "C" {
    extern Err clients_changed(bool * did_change);
    extern Err get_client_handles(c_string_vec * client_handles);
    extern Err send_message(const char * client, const char * msg);
    extern Err get_messages(char * client, c_string_vec * messages);
    extern void free_strvec(c_string_vec vec);
}

// control pad check
#define CP_CHECK(x) do {                                                   \
        uint64_t retval = (x);                                             \
        if (retval != 0) {                                                 \
            fprintf(stderr, "controlpads error: %s returned %ld as %s:%d", \
                    #x, retval, __FILE__, __LINE__);                       \
        }                                                                  \
    } while (0)

