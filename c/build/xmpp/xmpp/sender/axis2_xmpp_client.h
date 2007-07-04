#ifndef AXIS2_XMPP_CLIENT
#define AXIS2_XMPP_CLIENT
#include <axutil_types.h>
#include <axutil_string.h>
#include <axis2_endpoint_ref.h>
#include <axis2_addr.h>
#include <axiom.h>
#include <axiom_soap.h>
#include <axis2_svc_ctx.h>
#include <iksemel.h>
#include <stdlib.h>

#include <axis2_xmpp_transport.h>
#include <axis2_xmpp_transport_sender.h>
#include <axis2_xmpp_transport_utils.h>

int 
axis2_xmpp_client_on_data(
    void* user_data,
    int type,
    iks* node);

int 
axis2_xmpp_client_on_start_node(
    axis2_xmpp_session_data_t *session,
    iks* node);

int 
axis2_xmpp_client_on_normal_node(
    axis2_xmpp_session_data_t *session,
    iks* node);

void 
axis2_xmpp_client_on_log(
    void *user_data,
    const char* data,
    size_t size,
    int is_incoming);

void 
axis2_xmpp_client_setup_filter(
    axis2_xmpp_session_data_t* session);

int 
axis2_xmpp_client_on_message(
    void *user_data,
    ikspak *pak);

int 
axis2_xmpp_client_on_presence(
    void *user_data,
    ikspak *pak);

int 
axis2_xmpp_client_on_subscription(
    void *user_data,
    ikspak *pak);

int 
axis2_xmpp_client_on_iq(
    void *user_data,
    ikspak *pak);



#endif  /* AXIS2_XMPP_CLIENT */
