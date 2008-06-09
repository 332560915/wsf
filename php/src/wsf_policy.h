/*
 * Copyright 2005,2008 WSO2, Inc. http://wso2.com
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


#ifndef WSF_POLICY_H
#define WSF_POLICY_H

#include <axutil_env.h>
#include <axiom.h>
#include <axis2_svc_client.h>
#include <php.h>
#include "wsf_common.h"
#include <axiom_soap_body.h>
#include <TSRM.h>
#include "ext/libxml/php_libxml.h"
#include <rampart_context.h>


#define WS_POLICY "Policy"

#define WS_IN_POLICY "inpolicy"
#define WS_OUT_POLICY "outpolicy"

#define WS_SIGN "sign"
#define WS_ENCRYPT "encrypt"
#define WS_ALGORITHM "algorithmSuite"
#define WS_LAYOUT "layOut"
#define WS_TIMESTAMP "includeTimeStamp"
#define WS_PROTECTION_ORDER "protectionOrder"
#define WS_UT "useUsernameToken"
#define WS_TOKEN_REFERENCE "securityTokenReference"
#define WS_ENCRYPT_SIGNATURE "encryptSignature"
#define WS_CUSTOM_TOKENS "customTokens"

#define WS_DEFAULT_TOKEN_VAL "X509"
#define WS_X509_TOKEN_VAL "X509Token"
#define WS_INITIATOR_VAL "InitiatorToken"
#define WS_INCLUDE_TOKEN_URI "http://schemas.xmlsoap.org/ws/2005/07/securitypolicy/IncludeToken/Always"
#define WS_INCLUDE_TOKEN "IncludeToken"
#define WS_RECIPIENT_TOKEN "RecipientToken"
#define WS_USERNAMETOKEN "UsernameToken"
#define WS_SIGNSUPPORTING_TOKEN "SignedSupportingTokens"
#define WS_ALGO_SUITE "AlgorithmSuite"
#define WS_ONLY_SIGN_HEADER_AND_BODY "OnlySignEntireHeadersAndBody"
#define WS_ENCRYPTPARTS "EncryptedParts"
#define WS_BODY "Body"
#define WS_LAYOUT_VAL "Layout"
#define WS_STRICT "Strict"
#define WS_WSS10 "Wss10"


/** ws_security_token options */
#define WS_USER "user"
#define WS_CERTIFICATE "certificate"
#define WS_PASSWORD_TYPE "passwordType"
#define WS_PASSWORD "password"
#define WS_PRIVATE_KEY "privateKey"
#define WS_TTL "ttl"
#define WS_RECEIVER_CERTIFICATE "receiverCertificate"
#define WS_RAMPART_CONFIGURATION "RampartConfiguration"
#define WS_DIGEST "Digest"
#define WS_PLAINTEXT "plainText"
#define WS_PASSWORD_CALL_BACK "passwordCallback"

#define BASIC256 "Basic256"
#define BASIC192 "Basic192"
#define BASIC128 "Basic128"
#define TRIPLEDES "TripleDes"
#define BASIC256_RSA15 "Basic256Rsa15"
#define BASIC192_RSA15 "Basic192Rsa15"
#define BASIC128_RSA15 "Basic128Rsa15"
#define TRIPLEDES_RSA15 "TripleDesRsa15"

#define ISSUER_SERIAL "IssuerSerial"
#define KEYIDENTIFIER "KeyIdentifier"
#define EMBEDDEDTOKEN "EmbeddedToken"
#define THUMBPRINT "Thumbprint"
#define DIRECT "Direct"
#define ISSUER_SERIAL_VAL "MustSupportRefIssuerSerial"
#define KEYIDENTIFIER_VAL "MustSupportRefKeyIdentifier"
#define EMBEDDEDTOKEN_VAL "MustSupportRefEmbeddedToken"
#define THUMBPRINT_VAL "MustSupportRefThumbprint"

#define SIGN_BEFORE "SignBeforeEncrypt"
#define ENCRYPT_BEFORE "EncryptBeforeSigning"

/**
* 
* @param this_ptr, 
* @param property, 
* @param TSRMLS_DC, 
* @returns AXIS2_SUCCESS on success, AXIS2_FAILURE Otherwise
*/
int wsf_policy_set_policy_options (
    zval * this_ptr,
    zval * property,
    const axutil_env_t * env TSRMLS_DC);

/**
* 
* @param sec_token, 
* @param policy, 
* @param env, 
* @param TSRMLS_DC, 
* @returns AXIS2_SUCCESS on success, AXIS2_FAILURE Otherwise
*/
int wsf_policy_handle_client_security (
    zval * sec_token,
    zval * policy,
    axutil_env_t * env,
    axis2_svc_client_t * svc_client TSRMLS_DC);

/**
* 
* @param sec_token, 
* @param policy, 
* @param env, 
* @param svc, 
* @param TSRMLS_DC, 
* @returns AXIS2_SUCCESS on success, AXIS2_FAILURE Otherwise
*/
int wsf_policy_handle_server_security (
    zval * sec_token,
    zval * policy,
    axutil_env_t * env,
    axis2_svc_t * svc,
    axis2_conf_t * conf TSRMLS_DC);

/**
* 
* @param rampart_ctx, 
* @param sec_token, 
* @param policy, 
* @param TSRMLS_DC, 
* @returns AXIS2_SUCCESS on success, AXIS2_FAILURE Otherwise
*/
int
wsf_set_rampart_options (
    rampart_context_t* rampart_ctx,
    zval * sec_token,
    zval * policy,
    axutil_env_t * env TSRMLS_DC);


/**
* 
* @param sec_token, 
* @param policy, 
* @param is_server_side, 
* @param TSRMLS_DC, 
* @returns AXIS2_SUCCESS on success, AXIS2_FAILURE Otherwise
*/
axiom_node_t *wsf_do_create_policy (
    zval * sec_token,
    zval * policy,
	axis2_bool_t is_server_side,
    axutil_env_t * env TSRMLS_DC);



#endif /* WSF_POLICY_H */
