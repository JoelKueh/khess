
#include <pthread.h>

#include "cibyl.h"

/* Command strings for matching. */
const char STR_UCI[] = "uci";
const char STR_DEBUG[] = "debug";
const char STR_SETOPTION[] = "setoption";
const char STR_REGISTER[] = "register";
const char STR_UCINEWGAME[] = "ucinewgame";
const char STR_POSITION[] = "position";
const char STR_GO[] = "go";
const char STR_STOP[] = "stop";
const char STR_PONDERHIT[] = "ponderhit";
const char STR_QUIT[] = "quit";

const char *STR_MAPPING[] = {
    STR_UCI,
    STR_DEBUG,
    STR_SETOPTION,
    STR_REGISTER,
    STR_UCINEWGAME,
    STR_POSITION,
    STR_GO,
    STR_STOP,
    STR_PONDERHIT,
    STR_QUIT
};

const char ENGINE_NAME[] = "Cibyl";
const char ENGINE_AUTHOR[] = "Joel Kuehne";

cibyl_errno_t handle_uci()
{
    /* Send ID. */
    printf("id name %s\n", ENGINE_NAME);
    printf("id author %s\n", ENGINE_AUTHOR);

    /* No need to send options. */

    /* Send UCIOK. */
    printf("uciok");

    return CIBYL_OK;
}

cibyl_errno_t handle_debug()
{
    /* TODO: Implement debug mode. */
    return CIBYL_OK;
}

cibyl_errno_t handle_isready()
{
    printf("readyok\n");
    return CIBYL_OK;
}

cibyl_errno_t handle_setoption()
{
    return CIBYL_OK;
}

cibyl_errno_t handle_register()
{
    return CIBYL_OK;
}

cibyl_errno_t handle_ucinewgame()
{

}

cibyl_errno_t handle_position()
{

}

cibyl_errno_t handle_go()
{

}

cibyl_errno_t handle_stop()
{

}

cibyl_errno_t handle_ponderhit()
{

}

cibyl_errno_t handle_quit()
{

}

cibyl_errno_t handle_command()
{

}

