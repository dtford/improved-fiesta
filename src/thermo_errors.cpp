#include <string>

#include "thermo_errors.h"

const std::string thermo_error_to_msg( thermo_error_t err )
{
    std::string msg;

    switch( err )
    {
        case OK:
        {
            msg = "Everything is just fine.";
            break;
        }
        case NO_OPEN:
        {
            msg = "You tried to open a file but things did not go well.";
            break;
        }
        case NO_FORK:
        {
            msg = "Unable to fork a child process.";
            break;
        }
        case NO_SETSID:
        {
            msg = "Unable to set the session id.";
            break;
        }
        case RECV_SIGTERM:
        {
            msg = "Received a termination signal; exiting.";
            break;
        }
        case RECV_SIGKILL:
        {
            msg = "Received a kill signal; exiting.";
            break;
        }
        case WEIRD_EXIT:
        {
            msg = "An unexptected condition has come up, exiting.";
            break;
        }
        case OPT_ERROR:
        {
            msg = "There was a problem with the options provided, exiting.";
            break;
        }
        default:
        {
            msg = "You submitted some kind of wackadoodle error code. What's up with you?";
        }
    }
    return msg;
}
