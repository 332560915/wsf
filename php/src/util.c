/*
 * Copyright 2005,2006 WSO2, Inc. http://wso2.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "php.h"
#include "zend_exceptions.h"
#include <axutil_dll_desc.h>
#include <axis2_msg_recv.h>
#include <axutil_class_loader.h>
#include "wsf_worker.h"
#include "wsf_xml_msg_recv.h"
#include <axis2_http_transport.h>
#include <axis2_addr.h>
#include "wsf.h"
#include "wsf_util.h"
#include <axutil_error_default.h>
#include <axutil_log_default.h>
#include <axutil_uuid_gen.h>
#include <axiom_util.h>
#include <axis2_http_client.h>

xmlNodePtr wsf_util_get_xml_node(zval *node TSRMLS_DC)
{
	php_libxml_node_object *object;
	xmlNodePtr nodep;

	object = (php_libxml_node_object *)zend_object_store_get_object(node TSRMLS_CC);
	nodep = php_libxml_import_node(node TSRMLS_CC);
    if (!nodep) {
		return NULL;
	}
	if (nodep->doc == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Imported Node must have \
							associated Document");
		return NULL;
	}
	if (nodep->type == XML_DOCUMENT_NODE || nodep->type == XML_HTML_DOCUMENT_NODE) {
		nodep = xmlDocGetRootElement((xmlDocPtr) nodep);
	}
	return nodep;
}

char*
wsf_util_read_file_to_buffer(char *filename TSRMLS_DC)
{
	char *contents;
	php_stream *stream;
	int len;
	long maxlen = PHP_STREAM_COPY_ALL;
	zval *zcontext = NULL;
	php_stream_context *context = NULL;

	context = php_stream_context_from_zval(zcontext, 0);

	stream = php_stream_open_wrapper_ex(filename, "rb", 
				(USE_PATH ) | ENFORCE_SAFE_MODE | REPORT_ERRORS,
				NULL, context);
	if (!stream) {
		return NULL;
	}

	len = php_stream_copy_to_mem(stream, &contents, maxlen, 0);

	php_stream_close(stream);

	if(len <= 0 ) {
		return NULL;
	} 
	return	contents;
}

axiom_node_t* 
wsf_util_construct_header_node(const axutil_env_t *env, 
   			       zval *header TSRMLS_DC)
{
	char *ns = NULL;
	char *localname = NULL;
	char prefix[6];
	axiom_namespace_t *header_ns = NULL;
	axiom_node_t *header_node = NULL;
	axiom_element_t *header_ele = NULL;
	axiom_namespace_t *soap_ns = NULL;
	int must_understand = 0;

	zval **tmp = NULL;


	AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, " [wsf_log] construct header node ");

	if(zend_hash_find(Z_OBJPROP_P(header), WS_HEADER_LOCALNAME , 
		sizeof(WS_HEADER_LOCALNAME) ,(void**)&tmp) == SUCCESS){
			localname = Z_STRVAL_PP(tmp);
	}else{
		return NULL;
	}
	if(zend_hash_find(Z_OBJPROP_P(header), WS_HEADER_NS, sizeof(WS_HEADER_NS),
		(void **)&tmp) == SUCCESS && Z_TYPE_PP(tmp) == IS_STRING){
			ns = Z_STRVAL_PP(tmp);
	}else{
		return NULL;
	}
	
	sprintf(prefix, "ns%d", WSF_GLOBAL(curr_ns_index)++);
	header_ns = axiom_namespace_create(env, ns, prefix);
	header_ele = axiom_element_create(env, NULL , localname, header_ns, &header_node);
	
	if(zend_hash_find(Z_OBJPROP_P(header), WS_HEADER_MUST_UNDERSTAND, 
		sizeof(WS_HEADER_MUST_UNDERSTAND), (void**)&tmp) == SUCCESS && Z_TYPE_PP(tmp) == IS_BOOL){
		axiom_attribute_t *mu_attr = NULL;
		char must_val[2];
			
		AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] must understand value is set");
		
	    must_understand = Z_BVAL_PP(tmp);
		sprintf(must_val,"%d", must_understand);
		soap_ns = axiom_namespace_create(env, WSF_GLOBAL(soap_uri), "soapenv");
		mu_attr = axiom_attribute_create(env, "mustUnderstand", must_val, soap_ns);
		axiom_element_add_attribute(header_ele, env, mu_attr, header_node);
	}
	if(zend_hash_find(Z_OBJPROP_P(header), WS_HEADER_ROLE, 
		sizeof(WS_HEADER_ROLE), (void**)&tmp) == SUCCESS){
			axiom_attribute_t *role_attr = NULL;
			char *role_val = NULL;
			if(Z_TYPE_PP(tmp) == IS_LONG){
				if(Z_LVAL_PP(tmp) == WS_SOAP_ROLE_NEXT){
					role_val = WS_SOAP_ROLE_NEXT_URI;
				}else if(Z_LVAL_PP(tmp) == WS_SOAP_ROLE_NONE){
					role_val = WS_SOAP_ROLE_NONE_URI;
				}else if(Z_LVAL_PP(tmp) == WS_SOAP_ROLE_ULTIMATE_RECEIVER){
					role_val = WS_SOAP_ROLE_ULTIMATE_RECEIVER_URI;
				}		
			}else if(Z_TYPE_PP(tmp) == IS_STRING){
				role_val = Z_STRVAL_PP(tmp);
			}
			if(!soap_ns)
				soap_ns = axiom_namespace_create(env, WSF_GLOBAL(soap_uri), "soapenv"); 
			/** role is only valid for soap12, for soap11 use actor, TODO */
			if(WSF_GLOBAL(soap_version) == AXIOM_SOAP12 && role_val){
				role_attr = axiom_attribute_create(env, WS_HEADER_ROLE, 
					role_val, soap_ns);
				axiom_element_add_attribute(header_ele, env, role_attr, header_node);
			}
	}
	if(zend_hash_find(Z_OBJPROP_P(header), WS_HEADER_DATA, 
		sizeof(WS_HEADER_DATA), (void**)&tmp) == SUCCESS && Z_TYPE_PP(tmp) == IS_STRING){
			axiom_element_set_text(header_ele, env, Z_STRVAL_PP(tmp), header_node);
	}

    if(zend_hash_find(Z_OBJPROP_P(header), "payload",
        sizeof("payload"), (void**)&tmp) == SUCCESS && Z_TYPE_PP(tmp) == IS_STRING){
            char *payload = NULL;
            axiom_node_t *payload_node = NULL;
            int payload_len = 0;
            axiom_xml_reader_t *reader = NULL;
            payload = Z_STRVAL_PP(tmp);
            payload_len = Z_STRLEN_PP(tmp);
            
            reader = axiom_xml_reader_create_for_memory(env, payload, 
                    payload_len, "utf-8", AXIS2_XML_PARSER_TYPE_BUFFER);
            
            payload_node = wsf_util_read_payload(reader, env);

            axiom_node_add_child(header_node, env, payload_node);
                            
    }

	return header_node;			
}


/* {{{ efree wrapper */
static void WSF_CALL 
wsf_free_wrapper_cli(axutil_allocator_t *allocator, 
					  void *ptr)
{	
	if(ptr)
		pefree(ptr,1);
}
/* }}} end efree wrapper */

/* {{{ malloc wrapper */
static void* WSF_CALL  
wsf_malloc_wrapper_cli(axutil_allocator_t *allocator,
		      size_t size)
{
    return pemalloc(size, 1);
}
/* }}} */
/* {{{ realloc wrapper */
static void* WSF_CALL  
wsf_realloc_warpper_cli(axutil_allocator_t *allocator,void *ptr, 
		        size_t size)
{
    return perealloc(ptr, size,1);
}
/* }}} */

/* {{{ malloc wrapper */
static void* WSF_CALL  
wsf_malloc_wrapper(axutil_allocator_t *allocator,
		   size_t size)
{
    return pemalloc(size,1);
}
/* }}} */
/* {{{ realloc wrapper */
static void* WSF_CALL  
wsf_realloc_warpper(axutil_allocator_t *allocator,
		    void *ptr, 
                    size_t size)
{
    return perealloc(ptr, size, 1);
}
/* }}} */
/* {{{ free wrapper */
static  void WSF_CALL 
wsf_free_wrapper(axutil_allocator_t *allocator, 
                 void *ptr)
{
	if (ptr)
		pefree(ptr, 1);
}
/* }}} */

/* {{{ axis2_environment create function */
axutil_env_t* wsf_env_create(axis2_char_t *path_tolog)
{
    axutil_allocator_t *allocator = NULL;
    axutil_error_t *error = NULL;
    axutil_log_t *log = NULL;
    axis2_char_t log_path[250];
    axutil_env_t *env = NULL;
    axutil_thread_pool_t *thread_pool = NULL;
	const axis2_char_t *LOG_NAME = "wsf.log";
    allocator = pemalloc(sizeof(axutil_allocator_t), 1);
    
    allocator->free_fn = wsf_free_wrapper_cli;
    allocator->malloc_fn = wsf_malloc_wrapper_cli;
    allocator->realloc = wsf_realloc_warpper_cli;
    
    error = axutil_error_create(allocator);
    if (path_tolog && (
	        (0 == strcmp(path_tolog, ""))   || 
		(0 == strcmp(path_tolog, "."))  ||
		(0 == strcmp(path_tolog, "./")))) {
		snprintf(log_path, 256, "%s", LOG_NAME);
	} else {
		snprintf(log_path, 256, "%s/%s", path_tolog, LOG_NAME);
	}

    thread_pool = axutil_thread_pool_init(allocator);
    log = axutil_log_create(allocator, NULL, log_path);
    env = axutil_env_create_with_error_log_thread_pool(allocator, error, log, thread_pool);
    return env;
}
/* }}} */
/* {{{ axis2_environment create function */
axutil_env_t* wsf_env_create_svr(axis2_char_t *path_tolog)
{
    axutil_allocator_t *allocator = NULL;
    axutil_error_t *error = NULL;
    axutil_log_t *log = NULL;
    axis2_char_t log_path[250];
    axutil_env_t *env = NULL;
    axutil_thread_pool_t *thread_pool = NULL;
	const axis2_char_t *LOG_NAME = "wsf_svr.log";
    allocator = pemalloc(sizeof(axutil_allocator_t), 1);
    
    allocator->free_fn = wsf_free_wrapper;
    allocator->malloc_fn = wsf_malloc_wrapper;
    allocator->realloc = wsf_realloc_warpper;
    
    error = axutil_error_create(allocator);
    if (path_tolog && (
        	(0 == strcmp(path_tolog, "")) || 
		(0 == strcmp(path_tolog, ".")) ||
		(0 == strcmp(path_tolog, "./")))) {
		snprintf(log_path, 256, "%s", LOG_NAME);
	} else {
		snprintf(log_path, 256, "%s/%s", path_tolog, LOG_NAME);
	}

    thread_pool = axutil_thread_pool_init(allocator);
    log = axutil_log_create(allocator, NULL, log_path);
    env = axutil_env_create_with_error_log_thread_pool(allocator, error, log, thread_pool);
    return env;
}
/* }}} */

void wsf_env_free(axutil_env_t *env){

}

wsf_svc_info_t* wsf_svc_info_create()
{
    wsf_svc_info_t *svc_info = NULL;
    svc_info = emalloc(sizeof(wsf_svc_info_t));
    svc_info->svc = NULL;
    svc_info->svc_name = NULL;
    svc_info->is_class = 0;
    svc_info->msg_recv = NULL;
    svc_info->class_info = NULL;
    svc_info->modules_to_engage = NULL;
    svc_info->php_worker = NULL;
    svc_info->use_mtom = 0;  /* default is false other wise service side will send mime 
							 headers which some servers can;t handle*/
    svc_info->request_xop = 0; /* default false */
    svc_info->policy = NULL; 
    svc_info->security_token = NULL;

    svc_info->ops_to_functions = NULL;
    svc_info->ops_to_actions = NULL;
    svc_info->password_location = NULL;
    svc_info->op_name = NULL;
    svc_info->ht_opParams = NULL;
    return svc_info;
}

/* free svc info struct */
void wsf_svc_info_free(wsf_svc_info_t *svc_info, axutil_env_t *env)
{
    if(NULL != svc_info){
    	if(svc_info->svc_name){
	        AXIS2_FREE(env->allocator, svc_info->svc_name);
	        svc_info->svc_name = NULL;
        }		    
    	efree(svc_info); 
    }
}

/** create php request info struct */
wsf_req_info_t* wsf_php_req_info_create()
{
	wsf_req_info_t *req_info = NULL;
	req_info = emalloc(sizeof(wsf_req_info_t));
	req_info->svr_name = NULL;
	req_info->svr_port = -1;
	req_info->http_protocol = NULL;
	req_info->content_encoding = NULL;
	req_info->soap_action = NULL;
	req_info->request_uri = NULL;
	req_info->content_length = -1;
	req_info->content_type = NULL;
	req_info->request_method = NULL;	
	req_info->req_data = NULL;
	req_info->req_data_length = 0;
    req_info->result_length = 0;
	req_info->result_payload = NULL;
	req_info->transfer_encoding = NULL;
	req_info->query_string = NULL;
	return req_info;
}

void wsf_php_req_info_free(wsf_req_info_t *req_info)
{
    if(req_info){
	if(req_info->content_encoding){
		efree(req_info->content_encoding);
	}
	if(req_info->http_protocol){
		efree(req_info->http_protocol);
	}
	if(req_info->request_uri){
		efree(req_info->request_uri);
	}
	if(req_info->result_payload){
		efree(req_info->result_payload);
	}
	if(req_info->result_payload){
		efree(req_info->result_payload);
	}
	if(req_info->content_type){
		efree(req_info->content_type);
	}
        efree(req_info);
    }
}

/* {{{ ws_read_payload */
axiom_node_t* wsf_util_read_payload(
    axiom_xml_reader_t *reader, 
    const axutil_env_t *env)
{
	axiom_stax_builder_t *builder = NULL;
	axiom_document_t *document = NULL;
	axiom_node_t *payload = NULL;
	
	builder = axiom_stax_builder_create(env, reader);
		
	if (!builder) {
		return NULL;
	}
		
	document = axiom_stax_builder_get_document(builder, env);
		
	if (!document) {
		return NULL;
	}
		
	payload = axiom_document_get_root_element(document, env);
	
	if (!payload) {
		return NULL;
	}
	axiom_document_build_all(document, env);

	/** free stax builder and associated document only */
	axiom_stax_builder_free_self(builder, env);

	return payload;
}
/* }}} ws_read_payload */
/*
char*wsf_util_get_ttl(char *buf, axutil_env_t *env)
{
    char *tmp_string = NULL;
    char *day = NULL;
    char *hour = NULL;
    char *minute = NULL;
    char *sec    = NULL;
    char *next = NULL;
    char *index = NULL;
    char *buffer = NULL;

    int d = 0, h = 0, m = 0;
    double s = 0.0;
    double ttl = 0.0;
    int timetolive = 0;
    if(!buf)
        return NULL;    
    tmp_string = strdup(buf);
    index = strchr(tmp_string, 'd');
    if(index)
    {
        next = index+1;
        tmp_string[index - tmp_string] = '\0';    
        day = tmp_string;
        tmp_string = next;
        d = atoi(day);
    }
    index = strchr(tmp_string, 'h');
    if(index)
    {
        next = index + 1;
        tmp_string[index - tmp_string] = '\0';
        hour = tmp_string;
        tmp_string = next;
        h = atoi(hour);
    }
    index = strchr(tmp_string, 'm');
    if(index)
    {
        next = index + 1;
        tmp_string[index - tmp_string] = '\0';
        minute = tmp_string;
        tmp_string = next;
        m = atoi(minute);
    }
    index = strchr(tmp_string, 's');
    if(index)
    {
        next = index + 1;
        tmp_string[index - tmp_string] = '\0';
        sec = tmp_string;
        tmp_string = next;
        s = atof(sec);
    }
    
    ttl =     s
            + m * 60
            + h * 60 * 60
            + d * 24 * 60 * 60 ;
    
    timetolive = ttl;
    buffer = AXIS2_MALLOC(env->allocator, sizeof(char)*33);
    sprintf(buffer, "%d", timetolive);
    return buffer;
}
*/
axis2_char_t *wsf_util_get_soap_msg_from_op_client(
    axis2_op_client_t *op_client,
    axutil_env_t *env, 
	axis2_wsdl_msg_labels_t msg_label)
{
    /* if(op_client && msg_label) */
    if(op_client)
    {
        axiom_soap_envelope_t *soap_env = NULL;
        const axis2_msg_ctx_t *msg_ctx = NULL;
        axiom_node_t *node = NULL;
        
        msg_ctx = axis2_op_client_get_msg_ctx(op_client, env, msg_label);
        if(!msg_ctx)
            return NULL;
        soap_env = axis2_msg_ctx_get_soap_envelope(msg_ctx, env);
        if(!soap_env)
            return NULL;
        node = axiom_soap_envelope_get_base_node(soap_env, env);
        return axiom_node_to_string(node, env);
    }
    return NULL;    
}    

axis2_char_t *wsf_util_get_http_headers_from_op_client(
    axis2_op_client_t *op_client,
    axutil_env_t *env, 
    axis2_wsdl_msg_labels_t msg_label)
{
    if(op_client)
    {
        const axis2_msg_ctx_t *msg_ctx = NULL;
        axutil_property_t *client_property = NULL;
        axis2_http_client_t *client = NULL;
        axis2_http_simple_response_t *response = NULL;
      /*   axis2_http_simple_request_t *request = NULL; */
        axutil_array_list_t *list = NULL;
        axis2_http_header_t *header = NULL;
        int i;
        char *header_buf = NULL;
        
        msg_ctx = axis2_op_client_get_msg_ctx(op_client, env, msg_label);
        if(!msg_ctx)
            return NULL;
        client_property = (axutil_property_t *)axis2_msg_ctx_get_property(
                          msg_ctx, env,
                          AXIS2_HTTP_CLIENT);

        if (client_property)
            client = (axis2_http_client_t *)axutil_property_get_value(
                client_property,
                env);
        else
            return NULL;
        
        if (client && (msg_label == AXIS2_WSDL_MESSAGE_LABEL_OUT)){
            response = axis2_http_client_get_response(client , env);
            if(response)
                list = axis2_http_simple_response_get_headers(response, env);
            else
                return NULL;
        }

      /*   if (client && (msg_label == AXIS2_WSDL_MESSAGE_LABEL_OUT)){ */
/*             request = axis2_http_client_get_request(client , env); */
/*             if(request) */
/*                 list = axis2_http_simple_request_get_headers(request, env); */
/*             else */
/*                 return NULL; */
/*         } */
        
        if (list){
            header_buf = emalloc(500);
            if(!axutil_array_list_is_empty(list, env)){
                for(i = 0; i< axutil_array_list_size(list, env); i++){
                    header = (axis2_http_header_t *)axutil_array_list_get(list, env, i);
                    strcat(header_buf, axis2_http_header_to_external_form(header, env));
                }
                return header_buf;
            }
        }
        
    }
    return NULL;    
}    


/** load message receiver */
axis2_msg_recv_t* load_msg_recv(axutil_env_t *env, axis2_char_t *home)
{
 /* msg_receiver default location is home/lib */
	axis2_msg_recv_t *msg_recv = NULL;
	axutil_dll_desc_t *dll_desc = NULL;
	axis2_char_t *repos_name = NULL;
	axis2_char_t *dll_name = NULL;
	axis2_char_t *temp_path = NULL;
	axis2_char_t *temp_path2 = NULL;
	axis2_char_t *temp_path3 = NULL;
	axis2_char_t *msg_recv_dll_name = NULL;
	axutil_param_t *impl_info_param = NULL;
		
	dll_desc = axutil_dll_desc_create(env);
	msg_recv_dll_name = 
    axutil_dll_desc_create_platform_specific_dll_name(dll_desc, env,
                                               "ws_xml_msg_recv");
	repos_name = home;
	
	temp_path = axutil_stracat(env, repos_name, AXIS2_PATH_SEP_STR);
	temp_path2 = axutil_stracat(env, temp_path, "lib");
	temp_path3 = axutil_stracat(env, temp_path2, AXIS2_PATH_SEP_STR);
	dll_name = axutil_stracat(env, temp_path3, msg_recv_dll_name);
	axutil_dll_desc_set_name(dll_desc, env, dll_name);
	axutil_dll_desc_set_type(dll_desc, env, AXIS2_MSG_RECV_DLL);
	
	impl_info_param = axutil_param_create(env, NULL , NULL);
	if(!impl_info_param)
	{
		return NULL;
	}
	axutil_param_set_value(impl_info_param, env, dll_desc);
	axutil_class_loader_init(env);
	
	msg_recv = (axis2_msg_recv_t *) axutil_class_loader_create_dll(env, 
		impl_info_param);
	return msg_recv;
}

int wsf_util_engage_module(
    axis2_conf_t *conf, 
    axis2_char_t *module_name, 
    axutil_env_t *env, 
    axis2_svc_t *svc)
{
    axis2_module_desc_t *module = NULL;
    axutil_qname_t *mod_qname = NULL;
	axis2_phase_resolver_t *phase_resolver = NULL;
	int status = 0;
	
    mod_qname = axutil_qname_create(env, module_name, NULL, NULL);
    module = axis2_conf_get_module(conf, env, mod_qname);
    if (module){
        status = axis2_svc_engage_module(svc, env, module, conf);
		if(!status){
			phase_resolver = axis2_phase_resolver_create_with_config(env, conf);
			if (!phase_resolver){
				AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, " [wsf-log] PHASE RESLOVER NULL");
				return AXIS2_FAILURE;
			}
		    status = axis2_phase_resolver_engage_module_to_svc(phase_resolver, env, svc,
				module);
		}
	}
	return AXIS2_FAILURE;
}

/* genarate service name from uri */
char* wsf_util_generate_svc_name_from_uri(
    char *req_uri, 
    wsf_svc_info_t *svc_info,
    axutil_env_t *env)
{
	char *svc_name = NULL;
	char *temp_string = NULL;
	char *index = NULL;
	char *uri = NULL;
	
	if(strncmp(req_uri, "/", 1) == 0){
                /** start with a / */
                uri = req_uri + 1;
        }else{
                uri = req_uri;
        }
	
	temp_string = estrdup(uri);
	
	index = strstr(temp_string,".php/");
	
	if(index){
		char *op_index = NULL;
		index = index +4;
		op_index = index +1;
		temp_string[index - temp_string] ='\0';
		svc_info->op_name = axutil_strdup(env, op_index);
	}
	
	svc_name = axutil_replace(env, temp_string, '/',':');
	
	efree(temp_string);
	
	return svc_name;
}

/* create service */
void wsf_util_create_svc_from_svc_info(
    wsf_svc_info_t *svc_info, 
    axutil_env_t *env TSRMLS_DC)
{
	axutil_qname_t *svc_qname = NULL;
	axis2_svc_t *svc = NULL;
	axis2_conf_t *conf = NULL;
	axis2_conf_ctx_t *conf_ctx = NULL;
  
	if(!svc_info->php_worker){
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "error creating service");
		return;
	}
	
	conf_ctx = wsf_worker_get_conf_ctx(svc_info->php_worker , env);
	
	conf = axis2_conf_ctx_get_conf(conf_ctx, env);
	if(!conf){
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "error creating qname");
		return;
	}	
  
	svc = axis2_conf_get_svc(conf, env, svc_info->svc_name);
	
    if(NULL != svc){
	
        svc_info->svc = svc;

	}else {
    	svc_qname = axutil_qname_create(env, svc_info->svc_name, NULL, NULL);
	    svc_info->svc = axis2_svc_create_with_qname(env, svc_qname);
		axutil_qname_free(svc_qname, env);
	} 
	return;
}
/** create operation */
void wsf_util_create_op_and_add_to_svc(
    wsf_svc_info_t *svc_info, 
    char *action,
    axutil_env_t *env,
    char *op_name,
	HashTable *ht_mep TSRMLS_DC)
{
	axis2_svc_t *svc = NULL;
	axis2_op_t *op = NULL;
	axutil_qname_t *op_qname = NULL;
	zval **tmp;
	op_qname = axutil_qname_create(env, op_name, NULL, NULL);
	svc = svc_info->svc;
	
	if(NULL != svc && NULL != op_name)
	{
        op = axis2_svc_get_op_with_name(svc_info->svc, env, op_name);
        if(!op)
        {   
            axis2_conf_t *conf = NULL;
            axis2_conf_ctx_t *conf_ctx = NULL;
            axis2_phases_info_t *info = NULL;
        
            op_qname = axutil_qname_create(env, op_name, NULL, NULL);
    	
    	    op = axis2_op_create_with_qname(env, op_qname);
		
		    axis2_op_set_msg_recv(op, env, svc_info->msg_recv);

			conf_ctx = wsf_worker_get_conf_ctx(svc_info->php_worker , env);
                  
            conf = axis2_conf_ctx_get_conf(conf_ctx, env);

            info = axis2_conf_get_phases_info(conf, env);
            axis2_phases_info_set_op_phases(info, env, op);
            axis2_svc_add_op(svc_info->svc, env, op);
			
			if(ht_mep){
                char operation[300];

				AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_service] ht mep not null, %s", op_name);
                sprintf(operation, "%s", op_name);
				if(zend_hash_find(ht_mep, operation, strlen(operation)+1,
			              (void **)&tmp) == SUCCESS && Z_TYPE_PP(tmp) == IS_STRING){
					char *mep = NULL;
					mep = Z_STRVAL_PP(tmp);	
					if(mep){
						AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,"[wsf_service] op mep %s", mep);
						  if(strcmp(mep, "IN_ONLY") == 0){
								axis2_op_set_msg_exchange_pattern(op, env, AXIS2_MEP_URI_IN_ONLY);
								AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_service] AXIS2_MEP_URI_IN_ONLY");
				
						  }else if(strcmp(mep, "IN_OUT") == 0){
							  axis2_op_set_msg_exchange_pattern(op, env, AXIS2_MEP_URI_IN_OUT);
							  AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_service] AXIS2_MEP_URI_IN_OUT");
						  }									
					}
				}else{
					AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf service] message exchange pattern for %s not found", op_name);
				}
			}
            if(action){
                axis2_svc_add_mapping(svc_info->svc, env, action, op);
            }                
        }
	}		
	return;
}
void wsf_util_set_attachments_with_cids(const axutil_env_t *env,
        int enable_mtom , axiom_node_t *payload_node,
        HashTable *attach_ht, char *default_cnt_type TSRMLS_DC)
{
    axiom_node_t *node = NULL;
    axiom_node_t *tmp_node = NULL;
    axiom_element_t *payload_element = NULL;
    axiom_element_t *ele = NULL;
    if(!payload_node || !attach_ht)
        return;

	if(axiom_node_get_node_type(payload_node, env) == AXIOM_ELEMENT){
        payload_element = (axiom_element_t *)axiom_node_get_data_element(payload_node, env);     
		axiom_element_get_first_element(payload_element, env, payload_node, &node);
    
		if(node && axiom_node_get_node_type(node, env) == AXIOM_ELEMENT)
		{
			
			ele = (axiom_element_t*)axiom_node_get_data_element(node, env);        
			if(ele)
			{
				axiom_namespace_t *ns = NULL;
				axis2_char_t *ns_uri = NULL;
				axis2_char_t *ele_localname = NULL;
				ele_localname = axiom_element_get_localname(ele, env);
				if(ele_localname && axutil_strcmp(ele_localname, "Include") == 0)
				{
					ns = axiom_element_get_namespace(ele, env, node);
					if(ns && (ns_uri = axiom_namespace_get_uri(ns, env)) &&
						axutil_strcmp(ns_uri, "http://www.w3.org/2004/08/xop/include") == 0)
					{
						axis2_char_t *cnt_type = NULL;
						/* found a matching xop include element */
						axis2_char_t *cid = NULL;
						axis2_char_t *pos = NULL;
						axis2_char_t *id = NULL;                    
						zval **tmp = NULL;
						/** look for content type in parent */
						cnt_type = axiom_element_get_attribute_value_by_name(payload_element, env, 
							"xmlmime:contentType");
						if(!cnt_type)
							cnt_type = default_cnt_type;                                        
	                    
						id = axiom_element_get_attribute_value_by_name(ele, env, "href");
						if(!id)
							return;
						pos = axutil_strstr(id, "cid:");
						if(pos){
							cid = id+4;
							if(zend_hash_find(attach_ht , cid, strlen(cid)+1, (void**)&tmp) == SUCCESS &&
								Z_TYPE_PP(tmp) == IS_STRING){
	                        
								void *binary_data = NULL;
								int binary_data_len = 0;
	                            
								/* binary_data = Z_STRVAL_PP(tmp); */
								binary_data_len = Z_STRLEN_PP(tmp);
								binary_data = AXIS2_MALLOC(env->allocator, sizeof(char)*binary_data_len);
								memcpy(binary_data, Z_STRVAL_PP(tmp) , binary_data_len);
								if(binary_data)
								{
									/** detach this node */
									axiom_node_t *text_node = NULL;
									axiom_text_t *text = NULL;
									axiom_data_handler_t *data_handler = NULL;
									axiom_node_detach(node, env);
									data_handler = axiom_data_handler_create(env, NULL, cnt_type);
									axiom_data_handler_set_binary_data(data_handler, env, binary_data, binary_data_len);
									text = axiom_text_create_with_data_handler(env, payload_node, data_handler, &text_node);
	                                
									if (enable_mtom == AXIS2_FALSE){
										axiom_text_set_optimize(text, env, AXIS2_FALSE);
									}
									return;
								}
							}
						}
					}                    
				}
			}
		}
	}
    tmp_node = axiom_node_get_first_child(payload_node, env);
    while(tmp_node)
    {
        wsf_util_set_attachments_with_cids(env, enable_mtom, tmp_node, attach_ht, default_cnt_type TSRMLS_CC);
        tmp_node = axiom_node_get_next_sibling(tmp_node, env);
    }        
    return;    
}        

void wsf_util_get_attachments(const axutil_env_t *env,
        axiom_node_t *payload_node, zval *cid2str,zval *cid2contentType TSRMLS_DC)
{
    axiom_node_t *node = NULL;
    axiom_node_t *tmp_node = NULL;
    axiom_element_t *payload_element = NULL; 
    if(!payload_node || !cid2contentType || !cid2str)
        return;
    if(axiom_node_get_node_type(payload_node, env) == AXIOM_ELEMENT)
    {
        payload_element = axiom_node_get_data_element(payload_node, env);
        axiom_element_get_first_element(payload_element, env, payload_node, &node);
        
        if(node && axiom_node_get_node_type(node, env) == AXIOM_ELEMENT){
            axiom_element_t *ele = NULL;
        ele = (axiom_element_t*)axiom_node_get_data_element(node, env);        
        if(ele){
            axiom_namespace_t *ns = NULL;
            axis2_char_t *ns_uri = NULL;
            axis2_char_t *ele_localname = NULL;
            ele_localname = axiom_element_get_localname(ele, env);
            if(ele_localname && axutil_strcmp(ele_localname, "Include") == 0)
            {
                ns = axiom_element_get_namespace(ele, env, node);
                if(ns && (ns_uri = axiom_namespace_get_uri(ns, env)) &&
                    axutil_strcmp(ns_uri, "http://www.w3.org/2004/08/xop/include") == 0)
                {
                    axiom_node_t *text_node = NULL;
                    axiom_text_t *text = NULL;
                    axis2_char_t *id = NULL;
                    axis2_char_t *pos = NULL;
                    axis2_char_t *cid = NULL;
                    axiom_data_handler_t *data_handler = NULL;
                    
                    id = axiom_element_get_attribute_value_by_name(ele, env, "href");
                    if(!id)
                        return;
                    pos = axutil_strstr(id, "cid:");
                    if(pos){
                        cid = id+4;
                        text_node = axiom_node_get_first_child(node, env);
                        if(text_node && axiom_node_get_node_type(text_node, env) == AXIOM_TEXT){
                            text = (axiom_text_t*)axiom_node_get_data_element(text_node, env);
                            if(text){
                                data_handler = axiom_text_get_data_handler(text, env);
                                if(data_handler){
                                    char *cnt_type = NULL;
                                    char *data = NULL;
                                    int data_len = 0;                               
                                    axiom_data_handler_read_from(data_handler, env,
                                        &data, &data_len);
                                    cnt_type = axiom_data_handler_get_content_type(data_handler, env);
                                    add_assoc_stringl(cid2str, cid, data, data_len, 1); 
                                    
                                    if(cnt_type){
                                        add_assoc_stringl(cid2contentType, cid, cnt_type, strlen(cnt_type), 1);
                                    }                                        
                                    axiom_text_set_optimize(text, env, AXIS2_TRUE);
                                    return;
                                }
                            }
                        }
                    }
                }                    
            }
        }
    }
}
    tmp_node = axiom_node_get_first_child(payload_node, env);
    while(tmp_node)
    {
        wsf_util_get_attachments(env, tmp_node, cid2str, cid2contentType TSRMLS_CC);
        tmp_node = axiom_node_get_next_sibling(tmp_node, env);
    }        
    return;    
} 


char* wsf_util_serialize_om(const axutil_env_t *env, axiom_node_t *ret_node)
{
    axiom_xml_writer_t *writer = NULL;
    axiom_output_t *om_output = NULL;
    axis2_char_t *buffer = NULL, *new_buffer = NULL;
    unsigned int buffer_len = 0;

    writer = axiom_xml_writer_create_for_memory(env, NULL, AXIS2_TRUE, 0,
                AXIS2_XML_PARSER_TYPE_BUFFER);
    om_output = axiom_output_create (env, writer);

    axiom_node_serialize (ret_node, env, om_output);
    buffer = (axis2_char_t*)axiom_xml_writer_get_xml(writer, env);
    buffer_len = axutil_strlen(buffer);

    new_buffer = AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t)*(buffer_len + 1));
    memcpy(new_buffer, buffer, buffer_len);
    new_buffer[buffer_len] = '\0';

   /* axiom_output_free(om_output, env); */
    return new_buffer;
}

xmlDocPtr wsf_util_serialize_om_to_doc(const axutil_env_t *env, axiom_node_t *ret_node)
{
	axiom_xml_writer_t *writer = NULL;
	axiom_output_t *om_output = NULL;
	xmlDocPtr doc = NULL;
	
	writer = axiom_xml_writer_create_for_memory(env, 
	        NULL, AXIS2_TRUE, 0, AXIS2_XML_PARSER_TYPE_DOC);
	om_output = axiom_output_create (env, writer);
	axiom_node_serialize (ret_node, env, om_output);
	doc = (xmlDocPtr)axiom_xml_writer_get_xml(writer, env);
	return doc;
}

axiom_node_t* 
wsf_util_deserialize_buffer(
    const axutil_env_t *env,
    char *buffer)
{
	axiom_xml_reader_t *reader = NULL;
	axiom_stax_builder_t *builder = NULL;
	axiom_document_t *document = NULL;
	axiom_node_t *payload = NULL;

	reader = axiom_xml_reader_create_for_memory(env,buffer, axutil_strlen(buffer), "utf-8",
				AXIS2_XML_PARSER_TYPE_BUFFER);
	if (!reader) {
		return NULL;
	}

	builder = axiom_stax_builder_create(env, reader);

	if (!builder){
		return NULL;
	}
	document = axiom_stax_builder_get_document(builder, env);
	if (!document) {
		AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Document is not found");
		return NULL;
	}

	payload = axiom_document_get_root_element(document, env);

	if (!payload) {
		AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Root element of the document \
				is not found");
		return NULL;
	}
	axiom_document_build_all(document, env);
	return payload;
}

void wsf_util_get_contents_from_file(
		zval *return_value, 
		char *filename, 
		int filename_len TSRMLS_DC)
{
	char *contents;
	php_stream *stream;
	int len;
	long maxlen = PHP_STREAM_COPY_ALL;
	zval *zcontext = NULL;
	php_stream_context *context = NULL;
	char *DELIMITER = "-----";
	
	context = php_stream_context_from_zval(zcontext, 0);

	stream = php_stream_open_wrapper_ex(filename, "rb", 
				(USE_PATH ) | ENFORCE_SAFE_MODE | REPORT_ERRORS,
				NULL, context);
	if (!stream) {
		ZVAL_BOOL(return_value, 0);
	}

	if ((len = php_stream_copy_to_mem(stream, &contents, maxlen, 0)) > 0) {
		char *key = NULL;
		char *start_index = NULL;
		char *tmp_index = NULL;
	
		tmp_index = strstr(contents, DELIMITER);
		if(!tmp_index)
			return;
		if(len > (tmp_index - contents))
		{
			tmp_index += 5; 
			tmp_index = strstr(tmp_index, DELIMITER);	
			if(!tmp_index)
				return;
			start_index = tmp_index+6;
			tmp_index[contents - tmp_index] = '\0';
		}
		
		tmp_index = strstr(start_index, DELIMITER);
		if(!tmp_index)
			return;
		tmp_index = tmp_index +1;
	
		start_index[tmp_index - start_index] = '\0';
				
		key = estrdup(start_index);
		
		efree(contents);

      ZVAL_STRINGL(return_value, key, strlen(key), 0);
	} else if (len == 0) {
		ZVAL_EMPTY_STRING(return_value);
	} else {
		ZVAL_BOOL(return_value, 0);
	}
	php_stream_close(stream);

}


void
wsf_util_set_soap_fault(zval *this_ptr,
        char *fault_code_ns,
        char *fault_code,
        char *fault_reason,
        char *fault_role,
        zval *fault_detail,
        char *name TSRMLS_DC)
{
/*    if (Z_TYPE_P(obj) != IS_OBJECT) {
        object_init_ex(obj, soap_fault_class_entry);
    }
*/
    if(!this_ptr)
        return;

    if (fault_reason != NULL) {
        add_property_string(this_ptr, WS_FAULT_REASON, fault_reason, 1);
#ifdef ZEND_ENGINE_2
        zend_update_property_string(zend_exception_get_default(TSRMLS_C), this_ptr, "message", sizeof("message")-1, fault_reason TSRMLS_CC);
#endif
    }
    if (fault_code != NULL) {
        int soap_version = WSF_GLOBAL(soap_version);

        if (fault_code_ns) {
            add_property_string(this_ptr, WS_FAULT_CODE, fault_code, 1);
            add_property_string(this_ptr, WS_FAULT_CODE_NS, fault_code_ns, 1);
        } else {
            if (soap_version == AXIOM_SOAP11) {
                add_property_string(this_ptr, WS_FAULT_CODE, fault_code, 1);
                if (strcmp(fault_code,"Client") == 0 ||
                    strcmp(fault_code,"Server") == 0 ||
                    strcmp(fault_code,"VersionMismatch") == 0 ||
                  strcmp(fault_code,"MustUnderstand") == 0) {
                    add_property_string(this_ptr, WS_FAULT_CODE_NS, WS_SOAP_1_1_NAMESPACE_URI, 1);
                }
            } else if (soap_version == AXIOM_SOAP12) {
                if (strcmp(fault_code,"Client") == 0) {
                    add_property_string(this_ptr, WS_FAULT_CODE, "Sender", 1);
                    add_property_string(this_ptr, WS_FAULT_CODE_NS, WS_SOAP_1_2_NAMESPACE_URI, 1);
                } else if (strcmp(fault_code,"Server") == 0) {
                    add_property_string(this_ptr, WS_FAULT_CODE, "Receiver", 1);
                    add_property_string(this_ptr, WS_FAULT_CODE_NS, WS_SOAP_1_1_NAMESPACE_URI, 1);
                } else if (strcmp(fault_code,"VersionMismatch") == 0 ||
                           strcmp(fault_code,"MustUnderstand") == 0 ||
                           strcmp(fault_code,"DataEncodingUnknown") == 0) {
                    add_property_string(this_ptr, WS_FAULT_CODE, fault_code, 1);
                    add_property_string(this_ptr, WS_FAULT_CODE_NS, WS_SOAP_1_2_NAMESPACE_URI, 1);
                } else {
                    add_property_string(this_ptr, WS_FAULT_CODE, fault_code, 1);
                }
            }
        }
    }
    if (fault_role != NULL) {
        add_property_string(this_ptr, WS_FAULT_ROLE, fault_role, 1);
    }
    if (fault_detail != NULL) {
        add_property_zval(this_ptr, WS_FAULT_DETAIL, fault_detail);
    }
    if (name != NULL) {
        add_property_string(this_ptr, "_name", name, 1);
    }
}

static void 
wsf_util_handle_fault_code(zval *fault_obj,
                           axiom_node_t *code_node,
                           axiom_element_t *code_element,
                           axutil_env_t *env TSRMLS_DC)
{
    axiom_node_t *code_value = NULL;
    axiom_element_t *code_value_ele = NULL;
    char *code = NULL;
    if(WSF_GLOBAL(soap_version) == AXIOM_SOAP12){
        code_value_ele = axiom_element_get_first_element(code_element, env, 
                code_node, &code_value);
        code = axiom_element_get_text(code_value_ele, env, code_node);
        if(code){
            add_property_string(fault_obj, WS_FAULT_CODE, code, 1);
        }                  
    }
}

static void 
wsf_util_handle_fault_reason(zval *fault_obj,
                       axiom_node_t *reason_node,
                       axiom_element_t *reason_element,
                       axutil_env_t *env TSRMLS_DC)
{
    
    if(WSF_GLOBAL(soap_version) == AXIOM_SOAP12){
        axiom_node_t *text_node = NULL;
        axis2_char_t *text_value = NULL;
        axiom_element_t *text_element = NULL;
        text_element = axiom_element_get_first_element(reason_element, env, reason_node,
                &text_node);
        if(text_element){
            text_value = axiom_element_get_text(text_element, env, text_node);
            if(text_value){
                add_property_string(fault_obj, WS_FAULT_REASON, text_value, 1);
            }
        }
    }
}


static void
wsf_util_handle_fault_detail(zval *fault_obj,
                       axiom_node_t *detail_node,
                       axiom_element_t *reason_element,
                       axutil_env_t *env TSRMLS_DC)
{


}

void
wsf_util_set_fault_properties(zval *fault_obj, 
                        axiom_node_t *fault_node,
                        axutil_env_t *env TSRMLS_DC)
{
    axiom_element_t *fault_element = NULL;
    axiom_node_t *node = NULL;
    axiom_element_t *element = NULL;
    axiom_child_element_iterator_t *ele_iterator = NULL;

    if(!fault_node) return;
    
    fault_element = axiom_node_get_data_element(fault_node, env);

    ele_iterator = axiom_element_get_child_elements(fault_element, env, fault_node);

    if(!ele_iterator)
        return;
    while(AXIOM_CHILD_ELEMENT_ITERATOR_HAS_NEXT(ele_iterator, env)){
        node = AXIOM_CHILD_ELEMENT_ITERATOR_NEXT(ele_iterator, env);
        if(node){
            element = axiom_node_get_data_element(node, env);
            if(element){
                char *localname = NULL;
                localname = axiom_element_get_localname(element, env);
                if(localname){
                    if(strcmp(localname,"Code") == 0){
                        wsf_util_handle_fault_code(fault_obj, node, element, env TSRMLS_CC);
                    }else if((strcmp(localname, "Reason") == 0) || (strcmp(localname,"faultstring") ==0)){
                        wsf_util_handle_fault_reason(fault_obj, node, element, env TSRMLS_CC);
                    }else if((strcmp(localname, "Detail")  == 0) || (strcmp(localname, "detail") == 0)){
                        wsf_util_handle_fault_detail(fault_obj, node, element, env TSRMLS_CC); 
                    }else if((strcmp(localname, "Role") == 0) ||(strcmp(localname, "faultactor") == 0)){
                        axis2_char_t *role_uri = NULL;
                        role_uri = axiom_element_get_text(element, env, node);
                        if(role_uri)
                            add_property_string(fault_obj, WS_FAULT_ROLE, role_uri, 1);
                    }
                }
            }
        }                
    }
}
