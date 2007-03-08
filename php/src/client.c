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
#include <axis2_addr.h>
#include "wsf.h"
#include "wsf_util.h"
#include <axis2_error_default.h>
#include <axis2_http_transport.h>
#include <axis2_log_default.h>
#include <axis2_uuid_gen.h>
#include <axiom_util.h>
#include "wsf_client.h"
#include "wsf_policy.h"

#ifdef USE_SANDESHA2
#include <sandesha2_client.h>
#endif

void
wsf_client_send_terminate_sequence(int is_rm_engaged, 
	int will_continue_sequence,
	int rm_spec_version,
	axis2_svc_client_t *svc_client);

xmlNodePtr 
wsf_get_xml_node(zval *node TSRMLS_DC);

axiom_xml_reader_t*
wsf_client_get_reader_from_zval(zval **param , 
			axis2_env_t *env TSRMLS_DC);

int 
wsf_client_set_addressing_options_to_options(axis2_env_t *env,
	axis2_options_t *options, 
	HashTable *ht TSRMLS_DC);

void 
wsf_client_handle_outgoing_attachments(axis2_env_t *env,
	 HashTable *msg_ht,
	 zval *msg,
	 HashTable *client_ht,
	 axiom_node_t *request_payload,
	 axis2_options_t *options TSRMLS_DC);

void
wsf_client_handle_incoming_attachments(axis2_env_t *env,
	 HashTable *client_ht,
	 zval *msg,
	 axiom_node_t *response_payload TSRMLS_DC);

void 
wsf_client_set_security_options(
		HashTable *client_ht, 
		HashTable *msg_ht,
		axis2_env_t *env, 
		axis2_options_t *client_options, 
		axis2_svc_client_t *svc_client TSRMLS_DC);

int 
wsf_client_get_rm_version(HashTable *ht TSRMLS_DC);

int 
wsf_client_set_soap_action(HashTable *client_ht, 
			   HashTable *msg_ht,
			   axis2_env_t *env,
			   axis2_options_t *client_options TSRMLS_DC);

int 
wsf_client_set_soap_action(HashTable *client_ht, 
	HashTable *msg_ht,
	axis2_env_t *env,
	axis2_options_t *client_options TSRMLS_DC)
{
	zval **tmp;
	char *action = NULL;
	if(!client_options)
	   return 0;

	if(msg_ht && zend_hash_find(msg_ht, WS_SOAP_ACTION, sizeof(WS_SOAP_ACTION), (void**)&tmp) == SUCCESS){
		action = Z_STRVAL_PP(tmp);
	}else if(client_ht && zend_hash_find(client_ht, WS_SOAP_ACTION, sizeof(WS_SOAP_ACTION), (void**)&tmp) == SUCCESS){
        	action = Z_STRVAL_PP(tmp);
	}
	if(action){
		axis2_string_t *action_string = axis2_string_create(env, action);
		AXIS2_OPTIONS_SET_SOAP_ACTION(client_options, env, action_string);
	}
	return AXIS2_SUCCESS;
}

void
wsf_client_send_terminate_sequence(int is_rm_engaged, 
	int will_continue_sequence,
	int rm_spec_version, axis2_svc_client_t *svc_client)
{
	if(is_rm_engaged && !will_continue_sequence && rm_spec_version == WS_RM_VERSION_1_1){
#ifdef USE_SANDESHA2
	{
		axis2_char_t *current_sequence_id = sandesha2_client_get_seq_id(env, svc_client);
		sandesha2_client_terminate_seq_with_svc_client_and_seq_key(env, svc_client, 
					current_sequence_id);
	}
#endif
	}
}

int 
wsf_client_get_rm_version(HashTable *ht TSRMLS_DC)
{
	zval **tmp;
	if(zend_hash_find(ht, WS_RELIABLE, sizeof(WS_RELIABLE), (void **)&tmp) == SUCCESS) {
		if(Z_TYPE_PP(tmp) == IS_BOOL && Z_BVAL_PP(tmp) == 1){
			return WS_RM_VERSION_1_0;
		}else if(Z_TYPE_PP(tmp) == IS_STRING && strcmp(Z_STRVAL_PP(tmp), "1.1") == 0){
			return WS_RM_VERSION_1_1;
		}else if(Z_TYPE_PP(tmp) == IS_STRING && strcmp(Z_STRVAL_PP(tmp), "1.0") == 0){
			return WS_RM_VERSION_1_0;
		}else if(Z_TYPE_PP(tmp) == IS_DOUBLE && Z_DVAL_PP(tmp) == 1.1){
			return WS_RM_VERSION_1_1;
		}else if(Z_TYPE_PP(tmp) == IS_DOUBLE && Z_DVAL_PP(tmp) == 1.0){
			return WS_RM_VERSION_1_0;
		}
	}
	return -1;
}


void 
wsf_client_set_security_options(
		HashTable *client_ht, 
		HashTable *msg_ht,
		axis2_env_t *env, 
		axis2_options_t *client_options, 
		axis2_svc_client_t *svc_client TSRMLS_DC)
{
	zval **tmp = NULL;
	zval *sec_token = NULL;
	zval *policy = NULL;
	

	AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] wsf_util_set_security_options");
	if(zend_hash_find(client_ht, WS_SECURITY_TOKEN, sizeof(WS_SECURITY_TOKEN), (void **)&tmp) == SUCCESS 			&& Z_TYPE_PP(tmp) == IS_OBJECT){
			sec_token = *tmp;
			AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] security token object found ");
	}
	if(zend_hash_find(client_ht, WS_POLICY_NAME, sizeof(WS_POLICY_NAME), (void **)&tmp) == SUCCESS){
			policy = *tmp;
			AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] policy object found ");
	}

	if(sec_token && policy) {
			ws_policy_handle_client_security(sec_token, policy, env, svc_client TSRMLS_CC); 
	}
}

void
wsf_client_handle_incoming_attachments(axis2_env_t *env,
				 	HashTable *client_ht,
					 zval *msg,
					 axiom_node_t *response_payload TSRMLS_DC)
{
	zval **tmp = NULL;
	int responseXOP = AXIS2_FALSE;
	if(!client_ht)
		return;

	AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] handle incomming attachments");

	if(client_ht && zend_hash_find(client_ht, WS_RESPONSE_XOP, 
			sizeof(WS_RESPONSE_XOP), (void **)&tmp) == SUCCESS && Z_TYPE_PP(tmp) == IS_BOOL){
					responseXOP = Z_BVAL_PP(tmp);
					AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] responseXOP %d", responseXOP);
    }

	if(responseXOP == 1){
		int attachments_found = AXIS2_FALSE;
		zval *cid2str = NULL;
		zval *cid2contentType = NULL;
        
		MAKE_STD_ZVAL(cid2str);
		MAKE_STD_ZVAL(cid2contentType);
    
		array_init(cid2str);
		array_init(cid2contentType);
		wsf_util_get_attachments(env, response_payload, cid2str, cid2contentType TSRMLS_CC);
    
		add_property_zval(msg, WS_ATTACHMENTS, cid2str);
		add_property_zval(msg, WS_CID2CONTENT_TYPE, cid2contentType);
	}
}


void
wsf_client_handle_outgoing_attachments(axis2_env_t *env,
								 HashTable *msg_ht,
								 zval *msg,
								 HashTable *client_ht,
								 axiom_node_t *request_payload,
								 axis2_options_t *options TSRMLS_DC)
{
	axis2_char_t *default_cnt_type = NULL;
	zval **tmp;
	HashTable *ht = NULL;
	/** default value for mtom is true */
	int enable_mtom = AXIS2_TRUE;
		
	if(!msg_ht)
		return;

	if(zend_hash_find(msg_ht , WS_DEFAULT_ATTACHEMENT_CONTENT_TYPE, 
		sizeof(WS_DEFAULT_ATTACHEMENT_CONTENT_TYPE), 
		(void **)&tmp) == SUCCESS && Z_TYPE_PP(tmp) == IS_STRING){
            default_cnt_type = Z_STRVAL_PP(tmp);   
        }else{
            default_cnt_type = "application/octet-stream";
    }
	if(client_ht && zend_hash_find(client_ht , WS_USE_MTOM, sizeof(WS_USE_MTOM), 
		(void **)&tmp) == SUCCESS){
			if(Z_TYPE_PP(tmp) == IS_BOOL && Z_BVAL_PP(tmp) == 0){
				enable_mtom = AXIS2_FALSE;
			}
    }

	AXIS2_OPTIONS_SET_ENABLE_MTOM(options, env, enable_mtom);
	AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] enable mtom %d", enable_mtom);
	
	if(zend_hash_find(msg_ht, WS_ATTACHMENTS, sizeof(WS_ATTACHMENTS), 
            (void **)&tmp) == SUCCESS && Z_TYPE_PP(tmp) == IS_ARRAY){
			ht = Z_ARRVAL_PP(tmp);
	}else if(zend_hash_find(Z_OBJPROP_P(msg), WS_ATTACHMENTS, sizeof(WS_ATTACHMENTS),
		(void**)&tmp) == SUCCESS){
			ht = Z_ARRVAL_PP(tmp);		
	}
    if(ht){
        wsf_util_set_attachments_with_cids(env, enable_mtom, request_payload,
            ht, default_cnt_type TSRMLS_CC);
    }
}

int 
wsf_client_set_addressing_options_to_options(axis2_env_t *env,
	axis2_options_t *client_options, 
	HashTable *ht TSRMLS_DC)
{
	zval **tmp = NULL;
	int addr_action_present = AXIS2_FALSE;

	if(!ht)
		return 0;

	if(zend_hash_find(ht , WS_ACTION, sizeof(WS_ACTION), (void**)&tmp) == SUCCESS) {

		AXIS2_OPTIONS_SET_ACTION(client_options, env, Z_STRVAL_PP(tmp));
       		 addr_action_present = AXIS2_TRUE;   

		AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] addressing action present :- %s", Z_STRVAL_PP(tmp));

    	}
	if(zend_hash_find(ht, WS_REPLY_TO, sizeof(WS_REPLY_TO), 
        (void**)&tmp) == SUCCESS) {
     
		axis2_endpoint_ref_t *replyto_epr = NULL;
		char *replyto = Z_STRVAL_PP(tmp);

		replyto_epr = axis2_endpoint_ref_create(env, replyto);

		AXIS2_OPTIONS_SET_REPLY_TO(client_options, env, replyto_epr);

		AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,
			"[wsf_client] replyTo present :- %s", replyto);
    	}	

	if(zend_hash_find(ht, WS_FAULT_TO, sizeof(WS_FAULT_TO), 
            (void**)&tmp) == SUCCESS) {

		axis2_endpoint_ref_t *faultto_epr = NULL;
        
		char *faultto = Z_STRVAL_PP(tmp);
        
		faultto_epr = axis2_endpoint_ref_create(env, faultto);
        
		AXIS2_OPTIONS_SET_FAULT_TO(client_options, env, faultto_epr);

		AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] faultTo present %s", faultto);

    }
	if(zend_hash_find(ht, WS_FROM, sizeof(WS_FROM), 
        (void**)&tmp) == SUCCESS) {
        axis2_endpoint_ref_t *from_epr = NULL;
        
		char *from = Z_STRVAL_PP(tmp);

        from_epr = axis2_endpoint_ref_create(env, from);
        
		AXIS2_OPTIONS_SET_REPLY_TO(client_options, env, from_epr);

		AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] from present %s", from);
    }
	return addr_action_present;
}

axiom_xml_reader_t*
wsf_client_get_reader_from_zval(zval **param , 
								axis2_env_t *env TSRMLS_DC)
{
	axis2_char_t *str_payload = NULL;
	int str_payload_len = 0;
	axiom_xml_reader_t *reader = NULL;
	xmlNodePtr nodep;

	if(Z_TYPE_PP(param) == IS_STRING){
		str_payload = Z_STRVAL_PP(param);
		str_payload_len = Z_STRLEN_PP(param);
		
		reader = axiom_xml_reader_create_for_memory(env, 
			str_payload, str_payload_len, "utf-8", AXIS2_XML_PARSER_TYPE_BUFFER);

		AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsflog] %s", str_payload);
	}else if(Z_TYPE_PP(param) == IS_OBJECT && 
		instanceof_function(Z_OBJCE_PP(param), dom_node_class_entry TSRMLS_CC)){
		
			nodep = wsf_get_xml_node(*param TSRMLS_CC);
		
		reader = axiom_xml_reader_create_for_memory(env, (void*)nodep->doc, 
							0, "utf-8", AXIS2_XML_PARSER_TYPE_DOC);

		AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] Input WSMessage - dom node ");
	}

	if (!reader) {
		zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 1 TSRMLS_CC, 
			"Xml Reader Creation Failed");
	}
	return reader;
}

/*
void 
wsf_client_add_properties(zval *this_ptr, HashTable *ht TSRMLS_DC){
	zval **tmp = NULL;
		if(zend_hash_find(ht, WS_USE_SOAP, sizeof(WS_USE_SOAP), (void **)&tmp) == SUCCESS){
            if(Z_TYPE_PP(tmp) == IS_STRING){
                add_property_stringl(this_ptr, WS_USE_SOAP, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);            
            }else if(Z_TYPE_PP(tmp) == IS_DOUBLE){
                add_property_double(this_ptr, WS_USE_SOAP, Z_DVAL_PP(tmp));
            }else if(Z_TYPE_PP(tmp) == IS_BOOL){
                add_property_bool(this_ptr, WS_USE_SOAP, Z_BVAL_PP(tmp));
            }
        }                     
		if(zend_hash_find(ht, WS_HTTP_METHOD, sizeof(WS_HTTP_METHOD), (void **)&tmp) == SUCCESS && 
            Z_TYPE_PP(tmp) == IS_STRING){
            add_property_stringl(this_ptr, WS_HTTP_METHOD, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
        }
		if(zend_hash_find(ht, WS_USE_WSA, sizeof(WS_USE_WSA), (void **)&tmp) == SUCCESS){ 
            if(Z_TYPE_PP(tmp) == IS_STRING){
                add_property_stringl(this_ptr, WS_USE_WSA, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
            }else if(Z_TYPE_PP(tmp) == IS_BOOL){
                add_property_bool(this_ptr, WS_USE_WSA, Z_BVAL_PP(tmp)); 
            }else if(Z_TYPE_PP(tmp) == IS_DOUBLE){
                add_property_double(this_ptr, WS_USE_WSA, Z_DVAL_PP(tmp));
                            
        }
        else{
            add_property_string(this_ptr, WS_USE_WSA, "1.0", 1);
        }
		if(zend_hash_find(ht, WS_TO	, sizeof(WS_TO), (void **)&tmp) == SUCCESS && 
            Z_TYPE_PP(tmp) == IS_STRING){
				add_property_stringl(this_ptr, WS_TO, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
        }
		if(zend_hash_find(ht, WS_ACTION, sizeof(WS_ACTION), (void **)&tmp) == SUCCESS && 
            Z_TYPE_PP(tmp) == IS_STRING){
				add_property_stringl(this_ptr, WS_ACTION, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
        }
		if(zend_hash_find(ht, WS_FROM, sizeof(WS_FROM), (void **)&tmp) == SUCCESS && 
            Z_TYPE_PP(tmp) == IS_STRING){
            add_property_stringl(this_ptr, WS_FROM, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
        }
		if(zend_hash_find(ht, WS_REPLY_TO, sizeof(WS_REPLY_TO), (void **)&tmp) == SUCCESS && 
            Z_TYPE_PP(tmp) == IS_STRING){
				add_property_stringl(this_ptr, WS_REPLY_TO, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
        }
		if(zend_hash_find(ht, WS_FAULT_TO, sizeof(WS_FAULT_TO), (void **)&tmp) == SUCCESS && 
            Z_TYPE_PP(tmp) == IS_STRING){
				add_property_stringl(this_ptr, WS_FAULT_TO, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
        }
	if(zend_hash_find(ht, WS_RESPONSE_XOP, sizeof(WS_RESPONSE_XOP), (void **)&tmp) == SUCCESS && 
            Z_TYPE_PP(tmp) == IS_BOOL){
				add_property_bool(this_ptr, WS_RESPONSE_XOP, Z_BVAL_PP(tmp));
        }else{
			add_property_bool(this_ptr, WS_RESPONSE_XOP, 0);        
        }
	if(zend_hash_find(ht, WS_USE_MTOM, sizeof(WS_USE_MTOM), (void **)&tmp) == SUCCESS && 
            Z_TYPE_PP(tmp) == IS_BOOL){
				add_property_bool(this_ptr, WS_USE_MTOM, Z_BVAL_PP(tmp));
        }else{
			add_property_bool(this_ptr, WS_USE_MTOM, 0);
        }
	if(zend_hash_find(ht, WS_DEFAULT_ATTACHEMENT_CONTENT_TYPE, sizeof(WS_DEFAULT_ATTACHEMENT_CONTENT_TYPE), (void **)&tmp) == SUCCESS && 
            Z_TYPE_PP(tmp) == IS_STRING){
				add_property_stringl(this_ptr, WS_DEFAULT_ATTACHEMENT_CONTENT_TYPE, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp),1);
        }

        if(zend_hash_find(ht, WS_SECURITY_TOKEN, sizeof(WS_SECURITY_TOKEN), (void **)&tmp) == SUCCESS && 
        	Z_TYPE_PP(tmp) == IS_OBJECT){
			add_property_zval(this_ptr, WS_SECURITY_TOKEN, *tmp);
        }
	if(zend_hash_find(ht, WS_POLICY_NAME, sizeof(WS_POLICY_NAME), (void **)&tmp) == SUCCESS &&
		Z_TYPE_PP(tmp) == IS_OBJECT  || Z_TYPE_PP(tmp) && IS_ARRAY ) {
			add_property_zval(this_ptr, WS_POLICY_NAME, *tmp);
        }
        
	/** RM */

	/**
		"reliable"=> TRUE | "1.1" | "1.0" | 1.1 | 1.0 

	if(zend_hash_find(ht, WS_RELIABLE, sizeof(WS_RELIABLE), (void **)&tmp) == SUCCESS) {
		if(Z_TYPE_PP(tmp) == IS_BOOL && Z_BVAL_PP(tmp) == 1){
			add_property_long(this_ptr, WS_RELIABLE, WS_RM_VERSION_1_0);
		}else if(Z_TYPE_PP(tmp) == IS_STRING && strcmp(Z_STRVAL_PP(tmp), "1.1") == 0){
			add_property_long(this_ptr, WS_RELIABLE, WS_RM_VERSION_1_1);
		}else if(Z_TYPE_PP(tmp) == IS_STRING && strcmp(Z_STRVAL_PP(tmp), "1.0") == 0){
			add_property_long(this_ptr, WS_RELIABLE, WS_RM_VERSION_1_0);
		}else if(Z_TYPE_PP(tmp) == IS_DOUBLE && Z_DVAL_PP(tmp) == 1.1){
			add_property_long(this_ptr, WS_RELIABLE, WS_RM_VERSION_1_1);
		}else if(Z_TYPE_PP(tmp) == IS_DOUBLE && Z_DVAL_PP(tmp) == 1.0){
			add_property_long(this_ptr, WS_RELIABLE, WS_RM_VERSION_1_0);
		}
    }
	if(zend_hash_find(ht, WS_SEQUENCE_EXPIRY_TIME, sizeof(WS_SEQUENCE_EXPIRY_TIME), (void **)&tmp) == SUCCESS){
		if(Z_TYPE_PP(tmp) == IS_LONG){
			add_property_long(this_ptr, WS_SEQUENCE_EXPIRY_TIME, Z_LVAL_PP(tmp));
		}
	}
	if(zend_hash_find(ht, WS_WILL_CONTINUE_SEQUENCE, sizeof(WS_WILL_CONTINUE_SEQUENCE), (void**)&tmp) == SUCCESS) {
		if(Z_TYPE_PP(tmp) == IS_BOOL){
			add_property_bool(this_ptr, WS_WILL_CONTINUE_SEQUENCE, Z_BVAL_PP(tmp));
		}
	}

	if(zend_hash_find(ht, WS_SERVER_CERT, sizeof(WS_SERVER_CERT), (void**)&tmp) == SUCCESS){
		if(Z_TYPE_PP(tmp) == IS_STRING){
			add_property_string(this_ptr, WS_SERVER_CERT, Z_STRVAL_PP(tmp), 1); 			
		}
	}

		if(zend_hash_find(ht, WS_CLIENT_CERT, sizeof(WS_CLIENT_CERT), (void**)&tmp) == SUCCESS){
			if(Z_TYPE_PP(tmp) == IS_STRING){
				add_property_string(this_ptr, WS_CLIENT_CERT, Z_STRVAL_PP(tmp), 1);
			}
		}
		if(zend_hash_find(ht, WS_PASSPHRASE, sizeof(WS_PASSPHRASE), (void**)&tmp) == SUCCESS){
			if(Z_TYPE_PP(tmp) == IS_STRING){
				add_property_string(this_ptr, WS_PASSPHRASE, Z_STRVAL_PP(tmp), 1);
			}
		}
}

*/

static int 
wsf_client_set_module_param_option(axis2_env_t *env, 
					axis2_svc_client_t *svc_client, 
					axis2_char_t *module_name,
					axis2_char_t *module_option, 
					axis2_char_t *module_option_value)
{
	axis2_conf_ctx_t *conf_ctx = NULL;
	axis2_svc_ctx_t *svc_ctx = NULL;
	axis2_module_desc_t *module_desc = NULL;
	axis2_conf_t *conf = NULL;
	axis2_hash_t *modules = NULL;
	axis2_qname_t *module_qname = NULL;
	axis2_param_t *param = NULL;

	module_qname = axis2_qname_create(env, module_name, NULL, NULL);

	svc_ctx = AXIS2_SVC_CLIENT_GET_SVC_CTX(svc_client, env);

	conf_ctx = AXIS2_SVC_CTX_GET_CONF_CTX(svc_ctx, env);

	conf = AXIS2_CONF_CTX_GET_CONF(conf_ctx, env);

	module_desc = AXIS2_CONF_GET_MODULE(conf, env, module_qname);
	if(!module_desc)
		return AXIS2_FAILURE;
	
        param = axis2_module_desc_get_param(module_desc, env, module_option);
	if(!param)
		return AXIS2_FAILURE;

	axis2_param_set_value(param, env, AXIS2_STRDUP(module_option_value, env));

	AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client setting %s module param %s to %s ",
		module_name, module_option ,module_option_value);
	axis2_qname_free(module_qname, env);

	return AXIS2_SUCCESS;
}


xmlNodePtr wsf_get_xml_node(zval *node TSRMLS_DC)
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

int wsf_client_set_headers(const axis2_env_t *env,
			   axis2_svc_client_t *svc_client, 
			   zval *msg TSRMLS_DC)
{
	HashTable *ht = NULL;	
	if(!svc_client || !msg){
		return 0;
	}else{

		zval **tmp = NULL;
		ht = Z_OBJPROP_P(msg);
		AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] setting header node ");

		if(zend_hash_find(ht, WS_HEADERS, sizeof(WS_HEADERS), (void**)&tmp) == SUCCESS){
			if(Z_TYPE_PP(tmp) == IS_ARRAY){
				HashPosition pos;
				HashTable *ht = Z_ARRVAL_PP(tmp);
				zval **val = NULL;
				zend_hash_internal_pointer_reset_ex(ht, &pos);
				
				AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] headers found");

				while(zend_hash_get_current_data_ex(ht, (void**)&val , &pos) != FAILURE){
					zval *header = *val;
					axiom_node_t *header_node = NULL;
					header_node = wsf_util_construct_header_node(env, header TSRMLS_CC);
					if(header_node)
					{
						AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] adding header block to svc_client");
						AXIS2_SVC_CLIENT_ADD_HEADER(svc_client, env, header_node);					
					}
					zend_hash_move_forward_ex(ht, &pos);
				}
			}
		}
	}
	return 1;
}





int wsf_client_set_addr_options(HashTable *client_ht, 
				HashTable *msg_ht, 
				axis2_env_t *env,
				axis2_options_t *client_options, 
				axis2_svc_client_t *svc_client, 
				int *is_addr_action_present  TSRMLS_DC)
{
		
	zval **tmp = NULL;
	int is_addressing_engaged = AXIS2_FALSE;
	int addr_action_present = AXIS2_FALSE;
	char *value = NULL;	
	
	AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] setting addressing options ");

	if(client_ht){

		if(zend_hash_find(client_ht, "useWSA", sizeof("useWSA"), 
			(void**)&tmp) == SUCCESS) {
			if(Z_TYPE_PP(tmp) == IS_BOOL){ 
				if(Z_BVAL_PP(tmp) == 1){

					value = "1.0";
					AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] useWSA true, version 1.0");

				}else{
					*is_addr_action_present = AXIS2_FALSE;
					return AXIS2_FALSE;
				}

       		} else if(Z_TYPE_PP(tmp) == IS_STRING){

					value = Z_STRVAL_PP(tmp);

					AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] useWSA is string, value is %s", value);
			}else if(Z_TYPE_PP(tmp) == IS_LONG && Z_LVAL_PP(tmp) == 1){
					
					value  = "1.0";
					
					AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] useWSA is double, value is %s", value);
			}
		}
	}
		
    if(msg_ht){
		
		AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] ws_message is present setting options using ws_message options");

		addr_action_present = wsf_client_set_addressing_options_to_options(env, 
			client_options, msg_ht TSRMLS_CC);
	} else if(client_ht){
			
		addr_action_present = wsf_client_set_addressing_options_to_options(env,
			client_options, client_ht TSRMLS_CC);
	}
	
	if(addr_action_present == AXIS2_TRUE && value){
		is_addressing_engaged = AXIS2_TRUE;
				
        AXIS2_SVC_CLIENT_ENGAGE_MODULE(svc_client, env, "addressing");
			
		AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] engage addressing");

		if(strcmp(value,"submission") == 0){
    
			axis2_property_t *prop = axis2_property_create_with_args(env, 0, 
				AXIS2_TRUE, 0, AXIS2_STRDUP(AXIS2_WSA_NAMESPACE_SUBMISSION, env));

			AXIS2_OPTIONS_SET_PROPERTY(client_options, env, AXIS2_WSA_VERSION, prop);

			AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] addressing versio is submission");
		} 
	}
	*is_addr_action_present = addr_action_present;
	return is_addressing_engaged;
}

int wsf_client_set_options(HashTable *client_ht, 
		HashTable *msg_ht, 
		axis2_env_t *env,
        axis2_options_t *client_options, 
		axis2_svc_client_t *svc_client, 
		int is_send TSRMLS_DC)
{
    zval **tmp = NULL;
    zval **msg_tmp = NULL;

	int use_soap = AXIS2_TRUE;
	int soap_version = AXIOM_SOAP12;
	
	if(client_ht){
		if(zend_hash_find(client_ht, "useSOAP", sizeof("useSOAP"), 
			(void **)&tmp) == SUCCESS){

			if(Z_TYPE_PP(tmp) == IS_STRING){
					char *value = NULL;
					value = Z_STRVAL_PP(tmp);
				if(value && strcmp(value,"1.2") == 0){

					AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] soap version SOAP12");
				}else if(value && strcmp(value,"1.1") == 0){
					
					soap_version = AXIOM_SOAP11;
					AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] soap version SOAP11");
				}
			}else if(Z_TYPE_PP(tmp) == IS_DOUBLE){
				double val = Z_DVAL_PP(tmp);
				if(val == 1.2){
					
					AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] use soap value is false ");

				}else if(val == 1.1){
					
					soap_version = AXIOM_SOAP11;
					AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] soap version soap11");

					WSF_GLOBAL(soap_version) = AXIOM_SOAP11;
					WSF_GLOBAL(soap_uri) = WS_SOAP_1_1_NAMESPACE_URI;
				}
			}else if(Z_TYPE_PP(tmp) == IS_BOOL && Z_BVAL_PP(tmp) == 0){
					use_soap = AXIS2_FALSE;
			}
		}

		if(use_soap){
			AXIS2_OPTIONS_SET_SOAP_VERSION(client_options, env, soap_version);
			AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
				"[wsf_client] useSOAP TRUE setting soap version %d", soap_version);
		}else{
		
			axis2_property_t *rest_property = NULL;

			AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
				"[wsf_client] useSOAP FALSE enabling rest ");
			
			rest_property = axis2_property_create(env);
			
			AXIS2_PROPERTY_SET_VALUE(rest_property, env, axis2_strdup(AXIS2_VALUE_TRUE, env));					
			
			AXIS2_OPTIONS_SET_PROPERTY(client_options, env, AXIS2_ENABLE_REST,
            rest_property);
		}

		/** default header type is POST, so only setting the HTTP_METHOD if GET */
		if(zend_hash_find(client_ht, WS_HTTP_METHOD, sizeof(WS_HTTP_METHOD), 
			(void **)&tmp) == SUCCESS){
			
			char *value = Z_STRVAL_PP(tmp);
			if(value && (strcmp(value, "GET") == 0 || strcmp(value, "get") == 0)){
				axis2_property_t* get_property = axis2_property_create(env);
				
				AXIS2_PROPERTY_SET_VALUE(get_property, env, axis2_strdup(AXIS2_HTTP_HEADER_GET, env));
				
				AXIS2_OPTIONS_SET_PROPERTY(client_options, env, AXIS2_HTTP_METHOD, get_property);

				AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] setting http method get property");
			}
		}
	}
   
    
	if(msg_ht && zend_hash_find(msg_ht, WS_TO, sizeof(WS_TO), 
        	(void**)&msg_tmp) == SUCCESS) {
	        axis2_endpoint_ref_t *to_epr = NULL;
        	char *to = Z_STRVAL_PP(msg_tmp);
	        to_epr = axis2_endpoint_ref_create(env, to);
	        AXIS2_OPTIONS_SET_TO(client_options, env, to_epr);
	}else if(client_ht && zend_hash_find(client_ht, WS_TO, sizeof(WS_TO), 
	        (void**)&msg_tmp) == SUCCESS) {
        	axis2_endpoint_ref_t *to_epr = NULL;
	        char *to = Z_STRVAL_PP(msg_tmp);
        	to_epr = axis2_endpoint_ref_create(env, to);
	        AXIS2_OPTIONS_SET_TO(client_options, env, to_epr);
    	}else{
		return AXIS2_FAILURE;
	}

	wsf_client_set_soap_action(client_ht, msg_ht, env, client_options TSRMLS_CC);	
			
	if(client_ht)
		wsf_client_set_security_options(client_ht, msg_ht, env, client_options, svc_client TSRMLS_CC);
	
    return AXIS2_SUCCESS;
}

/**
this_ptr -> ws_client 
param    -> payload ( ws_message, string, dom node)

*/
int wsf_client_do_request(
		zval *this_ptr, 
		zval *param, 
		zval *return_value,
		axis2_env_t *env,
		axis2_svc_client_t *svc_client,
		int is_oneway TSRMLS_DC)
{
/** for dom  */
    int status = AXIS2_SUCCESS;
    int input_type = WS_USING_INCORRECT_INPUT;

	zval **data = NULL;

	axiom_node_t *request_payload = NULL;
    axiom_node_t *response_payload = NULL;
    axiom_xml_reader_t *reader = NULL;
	axis2_options_t *client_options = NULL;

    zval **client_tmp = NULL;
    zval **msg_tmp = NULL;
	HashTable *client_ht = NULL;
	HashTable *msg_ht = NULL;

    int responseXOP = 0;

	char *str_payload = NULL;
	int str_payload_len = 0;

	/** RM OPTION VARIABLES */
	int ws_client_will_continue_sequence = AXIS2_FALSE;
	int engage_rm = AXIS2_FALSE;
	int rm_spec_version = WS_RM_VERSION_1_0;
	char *rm_spec_version_str = WS_RM_VERSION_1_0_STR;
	int is_addressing_engaged = AXIS2_FALSE;
	int is_addressing_action_present = AXIS2_FALSE;
	int is_rm_engaged = AXIS2_FALSE;


	wsf_client_set_module_param_option(env, svc_client, "sandesha2", "sandesha2_db" ,WSF_GLOBAL(rm_db_dir));

    if(Z_TYPE_P(param) == IS_OBJECT &&
        instanceof_function(Z_OBJCE_P(param), ws_message_class_entry TSRMLS_CC))
    {
        zval **tmp_val = NULL;
		if(	(zend_hash_find(Z_OBJPROP_P(param), WS_MSG_PAYLOAD_STR, 
			sizeof(WS_MSG_PAYLOAD_STR), (void**)&tmp_val) == SUCCESS) || 
			(zend_hash_find(Z_OBJPROP_P(param), WS_MSG_PAYLOAD_DOM,
				sizeof(WS_MSG_PAYLOAD_DOM), (void**)&tmp_val) == SUCCESS) )	{
				reader = wsf_client_get_reader_from_zval(tmp_val, env TSRMLS_CC);				
        }

		if(zend_hash_find(Z_OBJPROP_P(param), WS_OPTIONS, sizeof(WS_OPTIONS), (void**)&msg_tmp) == SUCCESS){
			if(Z_TYPE_PP(msg_tmp) == IS_ARRAY)
				msg_ht = Z_ARRVAL_PP(msg_tmp);
		}
		
        input_type = WS_USING_MSG;
		AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] do_request Input type is WSMessage ");

	}else{
		reader = wsf_client_get_reader_from_zval(&param, env TSRMLS_CC);
		
		AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] Input  is not WSMessage ");
		
		input_type = WS_USING_STRING;
    }       

	/** convert payload to an axiom node */
	request_payload = ws_util_read_payload(reader, env);
	
	if (!request_payload) {
		zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 1 TSRMLS_CC, 
			"request payload should not be null");
		AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "request payload node is null");
	}
	
	client_options = (axis2_options_t *)AXIS2_SVC_CLIENT_GET_OPTIONS(svc_client, env);

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), WS_OPTIONS, sizeof(WS_OPTIONS), 
		(void**)&client_tmp) == SUCCESS){

		client_ht = Z_ARRVAL_PP(client_tmp);
	}

	/** add ssl properties */
	wsf_client_enable_ssl(client_ht , env, client_options, svc_client TSRMLS_CC);

    if(input_type == WS_USING_MSG){

		/** setting soap , rest and security options */
		status = wsf_client_set_options(client_ht, msg_ht , env,
            client_options, svc_client, 0 TSRMLS_CC);

		is_addressing_engaged = wsf_client_set_addr_options(client_ht , msg_ht, env, 
			client_options, svc_client, &is_addressing_action_present TSRMLS_CC);
		/** add set headers function here */

		wsf_client_set_headers(env, svc_client, param TSRMLS_CC);

		wsf_client_handle_outgoing_attachments(env, msg_ht, param, client_ht ,request_payload, client_options TSRMLS_CC);

    }else if(input_type == WS_USING_DOM || input_type == WS_USING_STRING){

			status = wsf_client_set_options(client_ht, NULL, env,
		        client_options, svc_client, 0 TSRMLS_CC);
		
			is_addressing_engaged = wsf_client_set_addr_options(client_ht, NULL, env, 
			client_options, svc_client, &is_addressing_action_present TSRMLS_CC);
    }

	if(status == AXIS2_FAILURE){
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "service enpoint uri is needed for service invocation");
	}

	if(client_ht){/** RM OPTIONS */
		axis2_property_t *rm_prop = NULL;
		int rm_version = wsf_client_get_rm_version(client_ht TSRMLS_CC);
				
		if(rm_version > 0){
			if(rm_version == WS_RM_VERSION_1_0){
				rm_spec_version = WS_RM_VERSION_1_0;
				rm_spec_version_str = WS_RM_VERSION_1_0_STR;
				AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] rm spec version 1.0");

			}else if(Z_LVAL_PP(client_tmp) == WS_RM_VERSION_1_1){
				rm_spec_version = WS_RM_VERSION_1_1;
				rm_spec_version_str = WS_RM_VERSION_1_1_STR;
				AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] rm spec version 1.1");

			}

			rm_prop = axis2_property_create_with_args(env, 0, 0, 0, rm_spec_version_str);
			AXIS2_OPTIONS_SET_PROPERTY(client_options, env, WS_SANDESHA2_CLIENT_RM_SPEC_VERSION, rm_prop);
			engage_rm  = AXIS2_TRUE;
		}

		/**
			reliable = TRUE
			1. addressing is engaged by user specifing useWSA and Action
			2. addressing is not specified by useWSA but action presnt
					then engage addressing
			If Addressing is engaged engage RM
		*/
		if((is_addressing_engaged || 
			(!is_addressing_engaged && is_addressing_action_present )) && engage_rm){
			
				if(!is_addressing_engaged){
					AXIS2_SVC_CLIENT_ENGAGE_MODULE(svc_client, env, "addressing");
					AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] useWSA not specified, addressing engaged since rm is engaed");
				}
				AXIS2_SVC_CLIENT_ENGAGE_MODULE(svc_client, env, "sandesha2");
				is_rm_engaged = AXIS2_TRUE;
				
				/*	
				wsf_client_set_module_param_option(env, svc_client, "sandesha2", "sandesha2_db" ,WSF_GLOBAL(rm_db_dir));
				*/
				/** rm is engaged , process other rm params */
				if(zend_hash_find(Z_OBJPROP_P(this_ptr), WS_SEQUENCE_EXPIRY_TIME, sizeof(WS_SEQUENCE_EXPIRY_TIME),
					(void**)&client_tmp) == SUCCESS){
						char timeout_value[100];
						sprintf(timeout_value, "%d", Z_LVAL_PP(client_tmp));
						wsf_client_set_module_param_option(env, svc_client, 
							"sandesha2", "InactivityTimeout", timeout_value);

						AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] sequenceExpiryTime is %d", Z_LVAL_PP(client_tmp));
				}
				if(zend_hash_find(Z_OBJPROP_P(this_ptr), WS_WILL_CONTINUE_SEQUENCE, 
						sizeof(WS_WILL_CONTINUE_SEQUENCE), (void **)&client_tmp) == SUCCESS){
						if(Z_TYPE_PP(client_tmp) && Z_BVAL_PP(client_tmp) == 1){
						
							ws_client_will_continue_sequence = 1;
						
							AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] willContinueSequence true");
						}
				}
		}
		if(is_rm_engaged){
			if(ws_client_will_continue_sequence && input_type == WS_USING_MSG){
				/** if input_type is ws_message and continueSequence is true on client, we should look for 
					false value in ws_message to end the sequence */
				if(zend_hash_find(msg_ht, WS_LAST_MESSAGE, sizeof(WS_LAST_MESSAGE),
					(void**)&msg_tmp) == SUCCESS && Z_BVAL_PP(msg_tmp)== 1){
				
						ws_client_will_continue_sequence = 0;
						if(rm_spec_version == WS_RM_VERSION_1_0){
							
							axis2_property_t *last_msg_prop = axis2_property_create_with_args(env, 
													AXIS2_SCOPE_APPLICATION, 0, NULL, AXIS2_VALUE_TRUE);
							AXIS2_OPTIONS_SET_PROPERTY(client_options, env, "Sandesha2LastMessage", last_msg_prop);

							AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] seting Sandesha2LastMessage");
						}
					
					}/** END LastMessage */
				
			}else if(!ws_client_will_continue_sequence){
					AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] setting TreminateSequence property");
					
					if(rm_spec_version == WS_RM_VERSION_1_0){
						axis2_property_t *last_msg_prop = axis2_property_create_with_args(env, 
											0, 0, 0, AXIS2_VALUE_TRUE);
						AXIS2_OPTIONS_SET_PROPERTY(client_options, env, "Sandesha2LastMessage", last_msg_prop);
						AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_client] setting Sandesha2LastMessage");
					}
			}

			if(!is_oneway){

				char *timeout = NULL;
				axis2_property_t *timeout_property = NULL;
				
				{  /** set sequence offer with is mandatory for 
					single channel to work */
					char *offered_seq_id = NULL;
					axis2_property_t *sequence_property = NULL;
					
					offered_seq_id = axis2_uuid_gen(env);
					
					sequence_property = axis2_property_create(env);
					
					AXIS2_PROPERTY_SET_VALUE(sequence_property, env, 
						AXIS2_STRDUP(offered_seq_id, env));
					
					AXIS2_OPTIONS_SET_PROPERTY(client_options, env, "Sandesha2OfferedSequenceId", 
						sequence_property);
					AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
						" [wsf-log] Sandesha2OfferedSequenceId is set as property");
				}
				if(zend_hash_find(client_ht, WS_RM_RESPONSE_TIMEOUT, sizeof(WS_RM_RESPONSE_TIMEOUT),
					(void**)&client_tmp) == SUCCESS){
					char *timeout = Z_STRVAL_PP(client_tmp);
				}else{
					/** default timeout value is 5 */
					timeout = WS_RM_DEFAULT_RESPONSE_TIMEOUT;
				}
				
				timeout_property = axis2_property_create_with_args(env, 0, 0, 0, timeout);
				
				if(timeout_property){
				        AXIS2_OPTIONS_SET_PROPERTY(client_options, env, 
						AXIS2_TIMEOUT_IN_SECONDS, timeout_property);
    				}
			}
		}
	}/** END RM OPTIONS */	

	if(is_oneway)
	{
		int ret_val = 0;
		ret_val = AXIS2_SVC_CLIENT_SEND_ROBUST(svc_client, env, request_payload);
		/** if rm is engaged and spec version is 1.1 send terminate sequence */
		wsf_client_send_terminate_sequence(is_rm_engaged, ws_client_will_continue_sequence,
			rm_spec_version, svc_client);

		if(ret_val == 1)
		{
			ZVAL_TRUE(return_value);
		}else{
			ZVAL_FALSE(return_value);
		}

	}else {
		response_payload = AXIS2_SVC_CLIENT_SEND_RECEIVE(svc_client, env, request_payload);	

		/** if rm is engaged and spec version is 1.1 send terminate sequence */
		wsf_client_send_terminate_sequence(is_rm_engaged, ws_client_will_continue_sequence,
			rm_spec_version, svc_client);	
		
		if (response_payload) {
			axis2_char_t *res_text = NULL;
    		axis2_char_t *fault = NULL;
	        
			fault = axiom_util_get_localname(response_payload, env);
		    
			if( fault && 0 == strcmp(fault, "Fault"))
			{
	    		zval *rfault = NULL;
				MAKE_STD_ZVAL(rfault);
    			object_init_ex(rfault, ws_fault_class_entry);
				res_text = wsf_util_serialize_om(env, response_payload);
				add_property_stringl(rfault, "str", res_text, strlen(res_text), 1);
				ZVAL_ZVAL(return_value, rfault, NULL, NULL);
			}
			else {
	    
				zval *rmsg = NULL;
    			MAKE_STD_ZVAL(rmsg);
    			object_init_ex(rmsg, ws_message_class_entry);

				wsf_client_handle_incoming_attachments(env, client_ht, rmsg, response_payload TSRMLS_CC);
				res_text = wsf_util_serialize_om(env , response_payload);
				add_property_stringl(rmsg, WS_MSG_PAYLOAD_STR, res_text, strlen(res_text), 1); 
				ZVAL_ZVAL(return_value, rmsg, NULL, NULL);
			}
			
		} else {
			zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 1 TSRMLS_CC, 
				"response message not received");
		}
	}
	return 0;
}

void 
wsf_client_enable_ssl(HashTable *ht, 
					   axis2_env_t *env, 
					   axis2_options_t *options,
					   axis2_svc_client_t *svc_client TSRMLS_DC)
{
	axis2_property_t *ssl_server_key_prop = NULL;
	axis2_property_t *ssl_client_key_prop = NULL;
	axis2_property_t *passphrase_prop = NULL;
	zval **tmp = NULL;
	char *ssl_server_key_filename = NULL;
	char *ssl_client_key_filename = NULL;
	char *passphrase = NULL;

	if(!ht)
		return;
	
	if(zend_hash_find(ht, WS_SERVER_CERT, sizeof(WS_SERVER_CERT), (void **)&tmp) == SUCCESS){
		ssl_server_key_filename = Z_STRVAL_PP(tmp);		
	}
	if(zend_hash_find(ht, WS_CLIENT_CERT, sizeof(WS_CLIENT_CERT), (void **)&tmp) == SUCCESS){
		ssl_client_key_filename = Z_STRVAL_PP(tmp);	
	}
	if(zend_hash_find(ht, WS_PASSPHRASE, sizeof(WS_PASSPHRASE), (void **)&tmp) == SUCCESS){
		passphrase = Z_STRVAL_PP(tmp);		
	}

	ssl_server_key_prop = axis2_property_create_with_args(env, 0, AXIS2_TRUE, 0, AXIS2_STRDUP(ssl_server_key_filename, env));
	AXIS2_OPTIONS_SET_PROPERTY(options, env, "SERVER_CERT", ssl_server_key_prop);

	ssl_client_key_prop = axis2_property_create_with_args(env, 0, AXIS2_TRUE, 0, AXIS2_STRDUP(ssl_client_key_filename, env));
	AXIS2_OPTIONS_SET_PROPERTY(options, env, "KEY_FILE", ssl_client_key_prop);

	passphrase_prop = axis2_property_create_with_args(env, 0, AXIS2_TRUE, 0, AXIS2_STRDUP(passphrase, env));
	AXIS2_OPTIONS_SET_PROPERTY(options, env, "SSL_PASSPHRASE", passphrase_prop);

	AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf-client] setting ssh options %s -- %s -- %s ", ssl_server_key_filename, ssl_client_key_filename, passphrase);
}


