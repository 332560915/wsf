#include <stdlib.h>
#include <strings.h>
#include <axutil_stomp.h>


axutil_stomp_t *
axutil_stomp_create (const axutil_env_t *env,
    char *host,
    int port)
{
    AXIS2_ENV_CHECK (env->error, NULL);
    axutil_stomp_t *stomp;
    stomp = AXIS2_MALLOC (env->allocator, sizeof (axutil_stomp_t));
    stomp->socket = axutil_network_handler_open_socket (env, host, port);

    if (stomp->socket == -1)
    {
        AXIS2_LOG_ERROR (env->log, AXIS2_LOG_SI, 
                     "stomp client is unable to open socket for %s host and %d port", 
                     host, port);
        return stomp;
    }

    stomp->stream = axutil_stream_create_socket (env, stomp->socket);

    if (!stomp->stream)
    {
        AXIS2_LOG_ERROR (env->log, AXIS2_LOG_SI, 
                         "stomp client is unable to create stream for",
                         "socket with %s host and %d port", 
                         host, port);

        return stomp;
    }

    stomp->frame = axutil_stomp_frame_create (env);

    if (!stomp->frame)
    {
        AXIS2_LOG_ERROR (env->log, AXIS2_LOG_SI, 
                         "stomp client is unable to create frame for",
                         "socket with %s host and %d port", 
                         host, port);
        return stomp;
    }

    return stomp;
}


axis2_status_t
axutil_stomp_set_command (axutil_stomp_t *stomp,
    const axutil_env_t *env,
    axis2_char_t *command)
{
    AXIS2_PARAM_CHECK (env->error, stomp, AXIS2_FAILURE);
    axutil_stomp_frame_set_command (stomp->frame, env, command);
    return AXIS2_SUCCESS;
}


axis2_status_t
axutil_stomp_set_body (axutil_stomp_t *stomp,
    const axutil_env_t *env,
    axis2_char_t *body)
{
    AXIS2_PARAM_CHECK (env->error, stomp, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK (env->error, stomp->frame, AXIS2_FAILURE);
    axutil_stomp_frame_set_body (stomp->frame, env, body);
    return AXIS2_SUCCESS;
}


axis2_status_t
axutil_stomp_set_header (axutil_stomp_t *stomp,
    const axutil_env_t *env,
    axis2_char_t *header,
    axis2_char_t *value)
{
    AXIS2_PARAM_CHECK (env->error, stomp, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK (env->error, stomp->frame, AXIS2_FAILURE);
    axutil_stomp_frame_set_header (stomp->frame, env, header, value);
    return AXIS2_SUCCESS;
}


int 
axutil_stomp_write (axutil_stomp_t *stomp,
    const axutil_env_t *env)
{
    AXIS2_PARAM_CHECK (env->error, stomp, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK (env->error, stomp->frame, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK (env->error, stomp->stream, AXIS2_FAILURE);

    return axutil_stomp_frame_write (stomp->frame, env, stomp->stream);
}


axutil_stomp_frame_t *
axutil_stomp_read (axutil_stomp_t *stomp,
    const axutil_env_t *env)
{
    axutil_stomp_frame_t *frame = NULL;
    frame = axutil_stomp_frame_read (env, stomp->stream);
    if (frame)
    {
/*         axutil_stomp_frame_free (stomp->frame, env); */
        stomp->frame = frame;
    }
    return stomp->frame;
}







