#ifdef WIN32
	#include "winsock2.h"   // hack to avoid errors winsock redefinitions
#endif
#include "ruby.h"

#include "axis2_util.h"
#include "wsf_worker.h"

#include "wsf_util.h"
#include "wsf_common.h"
#include <axutil_env.h>
#include <axis2_svc_client.h>
#include <axiom_soap.h>
#include <axis2_http_transport.h>
#include <axis2_addr.h>
#include <axiom_util.h>
#include "wsf_xml_msg_recv.h"

static axutil_env_t *ws_env_svr;
static axis2_msg_recv_t * wsf_msg_recv;
static wsf_worker_t * worker;

static int
hash_each_ops_to_funcs(VALUE key, VALUE value, VALUE arg);

static int
hash_each_action(VALUE key, VALUE value, VALUE arg);

static void
wsf_ruby_req_info_fill(wsf_req_info_t *req_info, VALUE request);

static void
wsf_ruby_res_info_fill(VALUE response, axis2_char_t *status_line);


static VALUE rb_mWSO2;
static VALUE rb_mWSF;
static VALUE rb_cWSService;

static void
wsservice_mark(wsservice_t *wsservice)
{
    rb_gc_mark(wsservice->ht_actions);
    rb_gc_mark(wsservice->ht_ops_to_funcs);
    rb_gc_mark(wsservice->ht_ops_to_mep);
    rb_gc_mark(wsservice->ht_op_params);
}

static void
wsservice_free(wsservice_t *w)
{
    free(w);
}

static VALUE
wsservice_allocate(VALUE klass)
{
    wsservice_t *w;
        
    w =(wsservice_t*)malloc(sizeof(wsservice_t));
    w->ws_env_svr = ws_env_svr;
 
    return Data_Wrap_Struct(klass, wsservice_mark, wsservice_free, w);
}


static VALUE
wsservice_initialize(VALUE self, VALUE options)
{
    VALUE ht_actions;
    VALUE ht_ops_to_funcs;
    VALUE ht_ops_to_mep;
    VALUE ht_op_params;
    VALUE request_xop;
    VALUE security_token;
    VALUE use_reliable;
    VALUE ht_classes;

    VALUE v_use_mtom;
    VALUE key = Qnil;
    VALUE v_request_uri = Qnil;

    wsservice_t *wsservice;

    wsf_svc_info_t *svc_info;

    svc_info = wsf_svc_info_create (ws_env_svr);
    svc_info->ops_to_functions = axutil_hash_make (ws_env_svr);
    svc_info->ops_to_actions = axutil_hash_make (ws_env_svr);
    svc_info->ops_to_classes = axutil_hash_make (ws_env_svr);
    svc_info->ruby_worker = worker;

    if(TYPE(options) == T_HASH)
    {
        ht_actions = rb_hash_aref(options, ID2SYM(rb_intern("actions")));
        if(ht_actions == Qnil)
        {
            ht_actions = rb_hash_aref(options, rb_str_new2("actions"));
        }
        if(ht_actions != Qnil)
        {
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                                "[wsf_service] setting actions ");
        }

        ht_ops_to_funcs = rb_hash_aref(options, ID2SYM(rb_intern("operations")));
        if(ht_ops_to_funcs == Qnil)
        {
            ht_ops_to_funcs = rb_hash_aref(options, rb_str_new2("operations"));
        }
        if(ht_ops_to_funcs != Qnil)
        {
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                                "[wsf_service] setting operation to ruby function map");
        }
        
        ht_ops_to_mep = rb_hash_aref(options, ID2SYM(rb_intern("op_mep")));
        if(ht_ops_to_mep == Qnil)
        {
            ht_ops_to_mep = rb_hash_aref(options, rb_str_new2("op_mep"));
        }
        if(ht_ops_to_mep != Qnil)
        {
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                                "[wsf_service] setting operation to mep map");
        }
        
        ht_op_params = rb_hash_aref(options, ID2SYM(rb_intern("op_params")));
        if(ht_op_params == Qnil)
        {
            ht_op_params = rb_hash_aref(options, rb_str_new2("op_params"));
        }
        if(ht_op_params != Qnil)
        {
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                                "[wsf_service] setting message operation parameters");
        }
        svc_info->ht_op_params = ht_op_params;

        v_use_mtom = rb_hash_aref(options, ID2SYM(rb_intern(WS_USE_MTOM)));
        if(v_use_mtom == Qnil)
        {
            v_use_mtom = rb_hash_aref(options, rb_str_new2(WS_USE_MTOM));
        }

        if(v_use_mtom == Qtrue)
        {
            svc_info->use_mtom  = 1;
        }
        else
        {
            svc_info->use_mtom  = 0;
        }

        AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                                  "[wsf_service] setting mtom property %d",
                                  svc_info->use_mtom);
        
        request_xop = rb_hash_aref(options, ID2SYM(rb_intern(WS_REQUEST_XOP)));
        if(request_xop == Qnil)
        {
            request_xop = rb_hash_aref(options, rb_str_new2(WS_REQUEST_XOP));
        }
        if(request_xop != Qnil)
        {
            svc_info->request_xop = FIX2INT(request_xop);
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] request xop %d", svc_info->request_xop);
        }

        security_token = rb_hash_aref(options, ID2SYM(rb_intern(WS_SECURITY_TOKEN)));
        if(security_token == Qnil)
        {
            security_token = rb_hash_aref(options, rb_str_new2(WS_SECURITY_TOKEN));
        }
        if(security_token != Qnil)
        {
            svc_info->security_token = security_token;
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] security_token object present");
        }

        use_reliable = rb_hash_aref(options, ID2SYM(rb_intern(WS_RELIABLE)));
        if(use_reliable == Qtrue)
        {
            if (!svc_info->modules_to_engage)
            {
                svc_info->modules_to_engage =  axutil_array_list_create (ws_env_svr, 3);
            }
            
            axutil_array_list_add (svc_info->modules_to_engage, 
                ws_env_svr, axutil_strdup (ws_env_svr, "sandesha2"));
        }
        AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] Use reliable is set to %b",use_reliable == Qtrue);
        
        ht_classes = rb_hash_aref(options, ID2SYM(rb_intern(WS_CLASSES)));
        if(ht_classes == Qnil)
        {
            ht_classes = rb_hash_aref(options, rb_str_new2(WS_CLASSES));
        }
        if(ht_classes != Qnil)
        {
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] ht_classes object present");
        }
    }

    Data_Get_Struct(self, wsservice_t, wsservice);

    wsservice->svc_info = svc_info;
    wsservice->ht_actions = ht_actions;
    wsservice->ht_ops_to_funcs = ht_ops_to_funcs;
    wsservice->ht_ops_to_mep = ht_ops_to_mep;
    wsservice->ht_op_params = ht_op_params;
    wsservice->ht_classes = ht_classes;

    wsservice->request_uri = NULL;
}


static void
wsf_ruby_res_info_fill(VALUE response, axis2_char_t *status_line)
{
    rb_hash_aset(response, rb_str_new2("Content-type"), rb_str_new2("text/xml"));
    rb_hash_aset(response, rb_str_new2("Status"), rb_str_new2(status_line));
}


static void
wsf_ruby_req_info_fill(wsf_req_info_t *req_info, VALUE request)
{
    VALUE temp;
    VALUE renv;
    
    char *request_envelope;
    int request_envelope_len = 0;

    VALUE v_rawpost;

    const char *rails_class = "ActionController::CgiRequest";

    if (request && strcmp(rails_class, rb_class2name(CLASS_OF(request))) == 0) 
    {
        /* we're inside Rails, wheee! */
        renv = rb_funcall(request, rb_intern("env"), 0); /* request.env */
        temp = rb_hash_aref(renv, rb_str_new2("REQUEST_URI"));
        if (temp != Qnil) 
        {
            req_info->request_uri = RSTRING(temp)->ptr;
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] request uri: %s",req_info->request_uri);
        }
        temp = rb_hash_aref(renv, rb_str_new2("QUERY_STRING"));
        if( temp != Qnil)
        {
            req_info->query_string = RSTRING(temp)->ptr;
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                             "[wsf_service] request query string: %s",req_info->query_string);
        }
        temp = rb_hash_aref(renv, rb_str_new2("HTTP_CONTENT_ENCODING"));
        if( temp != Qnil)
        {
            req_info->content_encoding = RSTRING(temp)->ptr;
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] request http content encoding: %s",req_info->content_encoding);
        }
        temp = rb_hash_aref(renv, rb_str_new2("HTTP_TRANSFER_ENCODING"));
        if( temp != Qnil)
        {
            req_info->transfer_encoding = RSTRING(temp)->ptr;
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] request transfer encoding : %s",req_info->transfer_encoding);
        }
        temp = rb_hash_aref(renv, rb_str_new2("SERVER_NAME"));
        if( temp != Qnil)
        {
            req_info->svr_name = RSTRING(temp)->ptr;
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] request server name: %s",req_info->svr_name);
        }

        temp = rb_hash_aref(renv, rb_str_new2("SERVER_PORT"));
        if( temp != Qnil)
        {
            req_info->svr_port = atoi(RSTRING(temp)->ptr);
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] request server port: %d",req_info->svr_port);
        }

        temp = rb_hash_aref(renv, rb_str_new2("SERVER_PROTOCOL"));
        if( temp != Qnil)
        {
            req_info->http_protocol = RSTRING(temp)->ptr;
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] request server protocol: %s",req_info->http_protocol);
        }

        temp = rb_hash_aref(renv, rb_str_new2("CONTENT_TYPE"));
        if( temp != Qnil)
        {
            req_info->content_type = RSTRING(temp)->ptr;
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] request content type: %s",req_info->content_type);
        }

        temp = rb_hash_aref(renv, rb_str_new2("SOAP_ACTION"));
        if( temp != Qnil)
        {
            req_info->soap_action = RSTRING(temp)->ptr;
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] request soap action: %s",req_info->soap_action);
        }

        temp = rb_hash_aref(renv, rb_str_new2("REQUEST_METHOD"));
        if( temp != Qnil)
        {
            req_info->request_method = RSTRING(temp)->ptr;
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] request method: %s",req_info->request_method);
        }

        /* get content and content length */
        temp = rb_hash_aref(renv, rb_str_new2("HTTP_CONTENT_LENGTH"));
        if( temp != Qnil)
        {
            request_envelope_len= atoi(RSTRING(temp)->ptr);
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] request htp content length: %d",request_envelope_len);
        }
        
        v_rawpost = rb_funcall(request, rb_intern("raw_post"), 0);
        if(v_rawpost != Qnil)
        {
            request_envelope = RSTRING(v_rawpost)->ptr;
            AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] request raw post data: %s",request_envelope);
            request_envelope_len = axutil_strlen(request_envelope); 
        }
        else
        {
            request_envelope = NULL;
            request_envelope_len = 0;
        }
        
        req_info->req_data = request_envelope;
        req_info->req_data_length = request_envelope_len;
        req_info->content_length = request_envelope_len;

    } 
    else 
    {
        /* not in reails */
        req_info->svr_name = strdup("localhost");
        req_info->svr_port = 9999;
        req_info->req_data = RSTRING(request)->ptr;
        req_info->req_data_length = RSTRING(request)->len;
        req_info->http_protocol = strdup("HTTP");
        req_info->request_method = strdup("POST");
        req_info->content_type = strdup("application/soap+xml;charset=UTF-8");
        req_info->content_length = req_info->req_data_length;
        req_info->request_uri = "/library/lend";
    }
         /*
    req_info->svr_name = strdup("localhost");
    req_info->svr_port = 9999;

    req_info->req_data = request_envelope;
    req_info->req_data_length = request_envelope_len;

    req_info->http_protocol = strdup("HTTP");
    req_info->request_uri = svc_info->svc_name;
    req_info->request_method = strdup("POST");
    req_info->content_type = strdup("application/soap+xml;charset=UTF-8");
    req_info->content_length = request_envelope_len;
    raw_post_null = AXIS2_TRUE;

    */

}
    

static VALUE
wsservice_reply(VALUE self, VALUE request, VALUE response)
{
    VALUE ht_actions;
    VALUE ht_ops_to_funcs;
    VALUE ht_ops_to_mep;
    VALUE ht_op_params;

    axis2_conf_t * conf = NULL;
    axis2_conf_ctx_t * conf_ctx = NULL;
    wsf_svc_info_t * svc_info = NULL;
    wsf_req_info_t  req_info; 
    wsf_worker_t *ruby_worker = NULL;
    int status = 0;

    char status_line[100];
    char *content_type = NULL;
    int in_wsdl_mode = 0;
    int raw_post_null = AXIS2_FALSE;
    
    char *arg_data = NULL;
    int arg_data_len = 0;

    wsservice_t *wsservice;

    Data_Get_Struct(self, wsservice_t, wsservice);
    svc_info = wsservice->svc_info;
    ht_actions = wsservice->ht_actions;
    ht_ops_to_funcs = wsservice->ht_ops_to_funcs;
    ht_op_params = wsservice->ht_op_params;

    wsf_ruby_req_info_init (&req_info);

    /* fill the request information */
    wsf_ruby_req_info_fill(&req_info, request);

    /* crete the service */
    if(wsservice->request_uri == NULL)
    {
        if(req_info.request_uri != NULL) 
        {
            svc_info->svc_name = wsf_util_generate_svc_name_from_uri (
                                    req_info.request_uri, svc_info, ws_env_svr);
        }
        else
        {
            svc_info->svc_name = axutil_strdup(ws_env_svr, "ruby_global_service");
        }
        svc_info->msg_recv = wsf_msg_recv;
        wsf_util_create_svc_from_svc_info (svc_info, ws_env_svr);

        wsf_util_process_ws_service_operations_and_actions(self);
    }

    ruby_worker = svc_info->ruby_worker;

    conf_ctx = wsf_worker_get_conf_ctx (ruby_worker, ws_env_svr);
    if (!conf_ctx) 
    {
        /* report error */
        return;
    }

    conf = axis2_conf_ctx_get_conf (conf_ctx, ws_env_svr); 
    if (!axis2_conf_get_svc (conf, ws_env_svr, svc_info->svc_name))
    {
        axis2_conf_add_svc (conf, ws_env_svr, svc_info->svc);
        if (NULL != svc_info->modules_to_engage)
        {
            int i = 0;
            int size =
                axutil_array_list_size (svc_info->modules_to_engage,
                ws_env_svr);
            for (i = 0; i < size; i++)
            {
                axis2_char_t * mod_name =
                    (axis2_char_t *) axutil_array_list_get (svc_info->
                    modules_to_engage, ws_env_svr, i);
                wsf_util_engage_module (conf, mod_name, ws_env_svr,
                    svc_info->svc);
            }
        }
    }

    status = wsf_worker_process_request (ruby_worker, ws_env_svr, &req_info, svc_info);

    if (status == WS_HTTP_ACCEPTED)
    {
        sprintf (status_line, "%s 202 Accepted", req_info.http_protocol);
    }
    else if (status == WS_HTTP_OK) 
    {
        sprintf (status_line, "%s 200 OK", req_info.http_protocol);
    }
    else if (status == WS_HTTP_INTERNAL_SERVER_ERROR) 
    {
        sprintf (status_line, "%s 500 Internal Server Error", req_info.http_protocol);
    }
    wsf_ruby_res_info_fill(response, status_line);

    return rb_str_new(req_info.result_payload, req_info.result_length);

    /*
    php_end_ob_buffer(0, 0 TSRMLS_CC);

    if (status == WS_HTTP_ACCEPTED){
        sprintf (status_line, "%s 202 Accepted", req_info.http_protocol);
        sapi_add_header (status_line, strlen (status_line), 1);
        sapi_add_header ("Content-Length: 0", sizeof ("Content-Length: 0") - 1, 1);
        content_type = emalloc(strlen (req_info.content_type) * sizeof (char) + 20);
        sprintf (content_type, "Content-Type: %s", req_info.content_type);
        sapi_add_header (content_type, strlen (content_type), 1);

    }else if (status == WS_HTTP_OK) {
        sprintf (status_line, "%s 200 OK", req_info.http_protocol);
        sapi_add_header (status_line, strlen (status_line), 1);
        content_type = emalloc(strlen (req_info.out_content_type) * sizeof (char) + 20);
        sprintf (content_type, "Content-Type: %s", req_info.out_content_type);
        sapi_add_header (content_type, strlen (content_type), 1);
        php_write (req_info.result_payload,
            req_info.result_length TSRMLS_CC);

    }else if (status == WS_HTTP_INTERNAL_SERVER_ERROR) {

        sprintf (status_line, "%s 500 Internal Server Error", req_info.http_protocol);
        sapi_add_header (status_line, strlen (status_line), 1);
        if (req_info.content_type){
            content_type = emalloc(strlen (req_info.content_type) * sizeof (char) + 20);
            sprintf (content_type, "Content-Type: %s", req_info.content_type);
            if (content_type){
                sapi_add_header (content_type, strlen (content_type), 1);
                php_write (req_info.result_payload, req_info.result_length TSRMLS_CC);
            }
        }
    }
    wsf_php_req_info_cleanup(&req_info, ws_env_svr);
    */
}

void
Init_wsservice()
{
    VALUE v_wsf_home;
    VALUE v_log_path;
    VALUE v_log_level;
    VALUE rm_db_dir;
    int log_level;
    axis2_char_t *wsf_home;
    axis2_char_t *log_path;

    axis2_char_t *s_log_level;


    rb_mWSO2 = rb_define_module("WSO2");
    rb_mWSF = rb_define_module_under(rb_mWSO2, "WSF");
    rb_cWSService = rb_define_class_under(rb_mWSF, "WSService", rb_cObject);
 
    rb_define_alloc_func(rb_cWSService, wsservice_allocate);
 
    rb_define_method(rb_cWSService, "initialize", wsservice_initialize, 1);
    rb_define_method(rb_cWSService, "reply", wsservice_reply, 2);

    /* then do the initialization */
   
    rb_require("rbconfig");
    
    v_wsf_home = rb_eval_string("Config::CONFIG['WSFC_HOME']");
    if(v_wsf_home == Qnil)
    {
        rb_raise(rb_eException, "Please set WSFC_HOME, I cannot continue without it");
    }
    wsf_home = RSTRING(v_wsf_home)->ptr;

    v_log_path = rb_eval_string("Config::CONFIG['WSFC_LOG_PATH']");
    if(v_log_path == Qnil)
    {
        log_path = "/tmp";
    }
    else
    {
        log_path = RSTRING(v_log_path)->ptr;
    }

    v_log_level = rb_eval_string("Config::CONFIG['WSFC_LOG_LEVEL']");
    if(v_log_level == Qnil)
    {
        log_level = 3;
    }
    else
    {
        s_log_level = RSTRING(v_log_level)->ptr;
        log_level = atoi(s_log_level);
    }

    ws_env_svr = wsf_env_create_svr(log_path, 
                                    log_level);

    if(ws_env_svr == NULL)
    {
        rb_raise(rb_eException, "Error creating ws_env_svr");
    }
    
    if(v_wsf_home == Qnil)
    {
        AXIS2_LOG_ERROR (ws_env_svr->log, AXIS2_LOG_SI, 
                          "[wsf_service] WSFC_HOME is not set");
        return;
    }
    AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] Using WSFC_HOME: %s", wsf_home);
    if(v_log_path == Qnil)
    {
        AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI, 
                          "[wsf_service] WSFC_LOG_PATH is not set, using /tmp as default log path");
    }
    AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] Using WSFC_LOG_PATH: %s", log_path);

    if(v_log_level == Qnil)
    {
        AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI, 
                          "[wsf_service] WSFC_LOG_LEVEL is not set, using 3 as the default log level");
    }
    AXIS2_LOG_DEBUG (ws_env_svr->log, AXIS2_LOG_SI,
                          "[wsf_service] Using WSFC_LOG_LEVEL: %d", log_level);
  
    wsf_msg_recv = wsf_xml_msg_recv_create(ws_env_svr);
    worker = wsf_worker_create(ws_env_svr, wsf_home, NULL);
    axiom_xml_reader_init ();
        
}


void
clean_globals()
{
    wsf_worker_free(worker, ws_env_svr);
    axis2_msg_recv_free (wsf_msg_recv, ws_env_svr);
    axutil_env_free (ws_env_svr);
    axiom_xml_reader_cleanup();
}
