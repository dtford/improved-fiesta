#ifndef __THERMO_ERRORS_H__
#define __THERMO_ERRORS_H__

typedef enum {
    OK,
    NO_OPEN,
    NO_FORK,
    NO_SETSID,
    RECV_SIGTERM,
    RECV_SIGKILL,
    WEIRD_EXIT,
    OPT_ERROR
} thermo_error_t;

const std::string thermo_error_to_msg(const thermo_error_t err);

#endif
