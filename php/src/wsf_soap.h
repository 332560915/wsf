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

#ifndef WSF_SCLIENT_H
#define WSF_SCLIENT_H
#include "wsf.h"

void wsf_soap_prepare_ws_globals();

void wsf_soap_do_soap_call(zval* this_ptr,
                         char* function,
                         int function_len,
                         int arg_count,
                         zval** real_args,
                         zval* return_value,
                         char* location,
                         char* soap_action,
                         char* call_uri,
                         HashTable* soap_headers,
                         zval* output_headers,
                         axutil_env_t *env
                         TSRMLS_DC);

void wsf_soap_get_functions(zval* this_ptr,
                            zval *return_value,
                            axutil_env_t *env
                            TSRMLS_DC);

void delete_url(void *handle);
void delete_hashtable(void *data);

soapServicePtr create_soap_service(HashTable *ht TSRMLS_DC);

void delete_service(void *data);

#endif /* WSF_SCLIENT_H */
