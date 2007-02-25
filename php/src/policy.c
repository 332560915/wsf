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
#include <axis2_addr.h>
#include "wsf.h"
#include "wsf_util.h"
#include <axis2_error_default.h>
#include <axis2_log_default.h>
#include <axis2_uuid_gen.h>
#include <axiom_util.h>
#include "wsf_policy.h"
#include <axiom.h>

#define ArrySize 8

int set_security_policy_options(zval *policy_obj,
                                 zval **tmp,
                                 const axis2_env_t *env TSRMLS_DC);

axiom_node_t *
create_policy_node(const axis2_env_t *env,
                   axiom_node_t *node );

axiom_node_t *
create_initiator_token(const axis2_env_t *env,
                       axiom_node_t *node,
                       zval **optionVal TSRMLS_DC);

axiom_node_t *
create_recipient_token(const axis2_env_t *env,
                       axiom_node_t *node,
                       zval **optionVal TSRMLS_DC);

axiom_node_t*
create_algorithm_suite(const axis2_env_t *env,
                       axiom_node_t *node,
                       zval **optionVal TSRMLS_DC);

axiom_node_t *
create_sign_parts(const axis2_env_t *env,
		  axiom_node_t *parent_node,
		  zval **tmp TSRMLS_DC);

axiom_node_t *
create_encrypt_parts(const axis2_env_t *env,
		     axiom_node_t *parent_node,
		     zval **tmp TSRMLS_DC);

axiom_node_t *
create_layout(const axis2_env_t *env,
              axiom_node_t *node,
              zval **optionVal TSRMLS_DC);

axiom_node_t *
create_default_sign(const axis2_env_t *env,
					axiom_node_t *node TSRMLS_DC);

axiom_node_t *
create_default_encrypt(const axis2_env_t *env,
		       axiom_node_t *node);

axiom_node_t *
create_username_token(const axis2_env_t *env,
		      axiom_node_t *parent_node,
		      zval **tmp TSRMLS_DC);

axiom_node_t *
create_token_reference(const axis2_env_t *env,
		       axiom_node_t *parent_node,
		       zval **tmp TSRMLS_DC);


char * algorithmArry[ArrySize]={
    BASIC256, BASIC192, BASIC128,
    TRIPLEDES, BASIC256_RSA15, BASIC192_RSA15,
    BASIC128_RSA15, TRIPLEDES_RSA15
};


axiom_node_t *do_create_client_policy(zval *sec_token,
				      zval *policy,
				      axis2_env_t *env TSRMLS_DC);


int ws_policy_handle_client_security(zval *sec_token,
                                     zval *policy,
                                     axis2_env_t *env,
                                     axis2_svc_client_t *svc_client,
                                     axis2_options_t *options TSRMLS_DC)
{
    axiom_node_t *outgoing_policy_node = NULL;
    axiom_node_t *incoming_policy_node = NULL;
    HashTable *ht = NULL;
    zval *policy_type = NULL;

    if (!sec_token && !policy)
        return AXIS2_FAILURE;

    /* if incoming policy and outgoing policy are diffrenet from each
       other */
    if ( Z_TYPE_P(policy) == IS_ARRAY)
    {

	ht = Z_ARRVAL_P(policy);
	if (zend_hash_find(ht, WS_IN_POLICY, sizeof(WS_IN_POLICY), (void *)&policy_type) == SUCCESS &&
	    (Z_TYPE_P(policy_type) == IS_OBJECT ))
	{
		incoming_policy_node = do_create_client_policy(sec_token, policy_type, env TSRMLS_CC);
	}
	if (zend_hash_find(ht, WS_OUT_POLICY, sizeof(WS_OUT_POLICY), (void *)&policy_type) == SUCCESS &&
	    (Z_TYPE_P(policy_type) == IS_OBJECT ))
	{
		outgoing_policy_node = do_create_client_policy(sec_token, policy_type, env TSRMLS_CC);
	}
	
    }
    /* since creating policy xml is the same procedure use one
       function */
    if ( Z_TYPE_P(policy) == IS_OBJECT)
    {
		outgoing_policy_node = do_create_client_policy(sec_token, policy, env TSRMLS_CC);
	incoming_policy_node = outgoing_policy_node;
    }
    
    /* for testing only ,should be remove later */ 
    if (outgoing_policy_node)
    {
	FILE *fp = NULL;
	axis2_char_t *om_str = NULL;
	om_str = AXIOM_NODE_TO_STRING(outgoing_policy_node, env);
	if (om_str)
	{
	    fp = fopen("/tmp/outgoing_policy.xml", "w");
	    fprintf(fp, "%s", om_str );
	    om_str = NULL;
	}
    }
    return AXIS2_SUCCESS;
}

axiom_node_t *do_create_client_policy(zval *sec_token,
        zval *policy,
		axis2_env_t *env TSRMLS_DC)
{
    axiom_node_t *root_om_node = NULL;
    axiom_node_t* exact_om_node = NULL;
    axiom_node_t *all_om_node = NULL;
    axiom_node_t *asymmetric_om_node = NULL;
    axiom_node_t *policy_om_node = NULL;
    axiom_node_t *init_om_node = NULL;
    axiom_node_t *rec_om_node = NULL;
    axiom_node_t *timestamp_om_node = NULL;
    axiom_node_t *enc_parts_om_node = NULL;

    axiom_element_t* root_om_ele = NULL;
    axiom_element_t * exact_om_ele = NULL;
    axiom_element_t *all_om_ele = NULL;
    axiom_element_t *asymmetric_om_ele = NULL;

    axiom_namespace_t *wsp_ns = NULL;
    axiom_namespace_t *exactly_ns = NULL;
    axiom_namespace_t *sp_ns = NULL;
    
    HashTable *ht_policy = NULL;
    HashTable *ht_token = NULL;

    zval **policy_val = NULL;
    zval **token_val = NULL;
    zval **tmp ;
    zval **tmp1;

    zval **sign_tmp = NULL;
    zval **encrypt_tmp = NULL;

    int is_encrypt = AXIS2_FALSE;
    int is_sign = AXIS2_FALSE;
    int is_default = AXIS2_TRUE; /* for the case when only
				  * usernametoken or timestamp enable */

    if (policy == NULL || sec_token == NULL)
        return NULL;

    ht_policy = Z_OBJPROP_P(policy);
    ht_token = Z_OBJPROP_P(sec_token);

    if (!ht_policy || !ht_token)
        return NULL;

    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_policy] creating client out going policy node ");

    /* first it has to detertermine whether either sigining or
     * encryption is included in the policy object.If so proceed
     */

    if (zend_hash_find(ht_policy, WS_SIGN, sizeof(WS_SIGN), (void **)&tmp) == SUCCESS)
    {
	if (Z_TYPE_PP(tmp) == IS_STRING || Z_TYPE_PP(tmp) == IS_BOOL)
	{
	    is_sign = AXIS2_TRUE;
	    sign_tmp = tmp;
	}
    }

    if (zend_hash_find(ht_policy, WS_ENCRYPT, sizeof(WS_ENCRYPT), (void **)&tmp) == SUCCESS)
    {
	if (Z_TYPE_PP(tmp) == IS_STRING || Z_TYPE_PP(tmp) == IS_BOOL)
	{
	    is_encrypt = AXIS2_TRUE;
	    encrypt_tmp = tmp;
	}
    }

	wsp_ns = axiom_namespace_create(env, WS_POLICY_NAMESPACE_URI, WS_POLICY_NAMESPACE);
	root_om_ele = axiom_element_create(env, NULL, WS_POLICY, wsp_ns, &root_om_node);
	
	exact_om_ele = axiom_element_create(env, root_om_node, WS_POLICY_EXACTLYONE, wsp_ns, &exact_om_node);
	all_om_ele = axiom_element_create(env, exact_om_node, WS_POLICY_ALL, wsp_ns, &all_om_node);
	
	sp_ns = axiom_namespace_create(env, WS_SEC_POLICY_NAMESPACE_URI, WS_SEC_POLICY_NAMESPACE);
	asymmetric_om_ele = axiom_element_create(env, all_om_node, WS_POLICY_ASYMMETRIC_BINDING, sp_ns, &asymmetric_om_node);
	
	policy_om_node = create_policy_node(env, asymmetric_om_node );


/*     if (zend_hash_find(ht_policy, WS_SIGN, sizeof(WS_SIGN), (void **)&tmp) == SUCCESS || */
/* 	 zend_hash_find(ht_policy, WS_ENCRYPT, sizeof(WS_ENCRYPT), (void **)&tmp) == SUCCESS) */
    if ((is_sign == AXIS2_TRUE && sign_tmp != NULL) ||
	(is_encrypt == AXIS2_TRUE && encrypt_tmp != NULL))
    {
	AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_sec_policy] encrypt or signing is enable");

	/*  if the sigining part is included in the user options */
        if (sign_tmp)
        {
	    create_initiator_token(env, policy_om_node, sign_tmp TSRMLS_CC);
	    create_sign_parts(env, policy_om_node, sign_tmp TSRMLS_CC);
        }
	else
	{
	    /* Since initiator token is needed for the default case */
		create_default_sign(env, policy_om_node TSRMLS_CC);
	    
	}
        /* if the encryption is included */
        if (encrypt_tmp)
        {
	    create_recipient_token(env, policy_om_node, encrypt_tmp TSRMLS_CC);
	    create_encrypt_parts(env, policy_om_node, encrypt_tmp TSRMLS_CC);
        }
	else
	{
	    /* Since recipient token is needed for the default case */
	    create_default_encrypt(env, policy_om_node);
	}
        /* if algorithm suite is presence in the options */
        if (zend_hash_find(ht_policy, WS_ALGORITHM, sizeof(WS_ALGORITHM), (void **)&tmp) == SUCCESS)
        {
	    create_algorithm_suite(env, policy_om_node, tmp TSRMLS_CC);
        }
	else
	{
	    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_sec_policy] No algorithm found so quit");
/* 	    return NULL; */
	}
        /*if the Layout is presence call this function for the default
	 * case layout is strict */
        if (zend_hash_find(ht_policy, WS_LAYOUT, sizeof(WS_LAYOUT), (void **)&tmp) == SUCCESS)
        {
	    create_layout(env, policy_om_node, tmp TSRMLS_CC);
        }
	else
	    create_layout(env, policy_om_node, tmp TSRMLS_CC);

	is_default = AXIS2_FALSE;
    }

    /* for timestamp */
    if (zend_hash_find(ht_policy, WS_TIMESTAMP, sizeof(WS_TIMESTAMP), (void **)&tmp) == SUCCESS &&
	zend_hash_find(ht_token, WS_TTL, sizeof(WS_TTL), (void **)&tmp) == SUCCESS )
    {
	if ( is_default == AXIS2_TRUE)
	{
		create_default_sign(env, policy_om_node TSRMLS_CC);
	    create_default_encrypt(env, policy_om_node);
	    is_default = AXIS2_FALSE;
	}

	axiom_element_create(env, policy_om_node, "IncludeTimestamp", sp_ns, &timestamp_om_node);
    }
    
    /* for usernameToken */
    if (zend_hash_find(ht_policy, WS_UT, sizeof(WS_UT), (void **)&tmp) == SUCCESS &&
	zend_hash_find(ht_token, WS_USER, sizeof(WS_USER), (void **)&tmp1) == SUCCESS )
    {
	if ( is_default == AXIS2_TRUE)
	{
		create_default_sign(env, policy_om_node TSRMLS_CC);
	    create_default_encrypt(env, policy_om_node);
	    is_default = AXIS2_FALSE;
	}
	create_username_token(env, policy_om_node, tmp TSRMLS_CC);
    }
    

    return root_om_node;
}

axiom_node_t *
create_policy_node(const axis2_env_t *env,
                   axiom_node_t *parent_om_node)
{
    axiom_node_t *policy_om_node = NULL;
    axiom_element_t *policy_om_ele = NULL;
    axiom_namespace_t *wsp_ns = NULL;

    wsp_ns = axiom_namespace_create(env, WS_POLICY_NAMESPACE_URI, WS_POLICY_NAMESPACE);
    policy_om_ele = axiom_element_create(env, parent_om_node, WS_POLICY, wsp_ns, &policy_om_node);

    return policy_om_node;


}


axiom_node_t *
create_initiator_token(const axis2_env_t *env,
                       axiom_node_t *parent_om_node,
                       zval **tmp TSRMLS_DC)
{
    axiom_node_t *in_token_om_node = NULL;
    axiom_node_t *policy_om_node1 = NULL;
    axiom_node_t *x509_om_node = NULL;
    axiom_node_t *policy_om_node2 = NULL;
    axiom_node_t *incl_token_om_node = NULL;
    axiom_node_t *token_id_om_node = NULL;
    axiom_node_t *tmp_node = NULL; /* if wrong option is found earlier
				    * node should be given back */

    axiom_element_t *in_token_om_ele = NULL;
    axiom_element_t *x509_om_ele = NULL;
    axiom_element_t *incl_token_om_ele = NULL;

    axiom_attribute_t *attr = NULL;

    axiom_namespace_t *sp_ns = NULL;
    char *token_name = NULL;

    tmp_node = parent_om_node;

    /* for x509 token and kerberose tokens.. can be extend later */
    if( tmp != NULL && Z_TYPE_PP(tmp) == IS_STRING)
    {
        token_name = Z_STRVAL_PP(tmp);
        if (stricmp(token_name, "x509") == 0)
        {
	    sp_ns = axiom_namespace_create(env,"http://schemas.xmlsoap.org/ws/2005/07/securitypolicy", "sp");

	    in_token_om_ele = axiom_element_create(env, parent_om_node, "InitiatorToken", sp_ns, &in_token_om_node);
		policy_om_node1 = create_policy_node(env, in_token_om_node);

            x509_om_ele = axiom_element_create(env, policy_om_node1, "X509Token", sp_ns, &x509_om_node);
            attr = axiom_attribute_create(env, "IncludeToken", "http://schemas.xmlsoap.org/ws/2005/07/securitypolicy/IncludeToken/Always",
                                          sp_ns);
            AXIOM_ELEMENT_ADD_ATTRIBUTE(x509_om_ele, env, attr, x509_om_node);
            policy_om_node2 = create_policy_node(env, x509_om_node);
            axiom_element_create(env, policy_om_node2, "WssX509V3Token10", sp_ns, &token_id_om_node);
	    return token_id_om_node;
        }
        /** implement for othertoken types */

    }

    if (tmp != NULL && Z_TYPE_PP(tmp) == IS_BOOL)
    {
	/* for the default case also it is treated as X509 */
	if (Z_BVAL_PP(tmp))
	{
	    sp_ns = axiom_namespace_create(env,"http://schemas.xmlsoap.org/ws/2005/07/securitypolicy", "sp");
	    in_token_om_ele = axiom_element_create(env, parent_om_node, "InitiatorToken", sp_ns, &in_token_om_node);
	    policy_om_node1 = create_policy_node(env, in_token_om_node);

	    x509_om_ele = axiom_element_create(env, policy_om_node1, "X509Token", sp_ns, &x509_om_node);

	    attr = axiom_attribute_create(env, "IncludeToken", "http://schemas.xmlsoap.org/ws/2005/07/securitypolicy/IncludeToken/Always",
					  sp_ns);
	    AXIOM_ELEMENT_ADD_ATTRIBUTE(x509_om_ele, env, attr, x509_om_node);
	    policy_om_node2 = create_policy_node(env, x509_om_node);
	    axiom_element_create(env, policy_om_node2, "WssX509V3Token10", sp_ns, &token_id_om_node);
	    return token_id_om_node;
	}
	else
	{
/* 	   php_error_docref(NULL TSRMLS_CC, E_WARNING, "sigining is not specified"); */
	   return tmp_node;
	}
    }

    return tmp_node;
}


/** for encryption part of the policy file */

axiom_node_t *
create_recipient_token(const axis2_env_t *env,
                       axiom_node_t *parent_om_node,
                       zval **tmp TSRMLS_DC)
{
    axiom_node_t *rec_token_om_node = NULL;
    axiom_node_t *policy_om_node1 = NULL;
    axiom_node_t *x509_om_node = NULL;
    axiom_node_t *policy_om_node2 = NULL;
    axiom_node_t *incl_token_om_node = NULL;
    axiom_node_t *token_id_om_node = NULL;
    axiom_node_t *tmp_node = NULL;

    axiom_element_t *rec_token_om_ele = NULL;
    axiom_element_t *x509_om_ele = NULL;
    axiom_element_t *incl_token_om_ele = NULL;

    axiom_attribute_t *attr = NULL;

    axiom_namespace_t *sp_ns = NULL;
    char *token_name = NULL;

    tmp_node = parent_om_node;

    if( tmp != NULL && Z_TYPE_PP(tmp) == IS_STRING)
    {
        token_name = Z_STRVAL_PP(tmp);
        if (stricmp(token_name, "x509") == 0)
        {

	    sp_ns = axiom_namespace_create(env,"http://schemas.xmlsoap.org/ws/2005/07/securitypolicy", "sp");

	    rec_token_om_ele = axiom_element_create(env, parent_om_node, "RecipientToken", sp_ns, &rec_token_om_node);
	    policy_om_node1 = create_policy_node(env, rec_token_om_node);
	    x509_om_ele = axiom_element_create(env, policy_om_node1, "X509Token", sp_ns, &x509_om_node);

	    /* Here uri may change according to the options difined in policy spec
	     * e.g - 'Always' may be changed as 'Never'*/
	    attr = axiom_attribute_create(env, "IncludeToken", "http://schemas.xmlsoap.org/ws/2005/07/securitypolicy/IncludeToken/Always",
					  sp_ns);
	    AXIOM_ELEMENT_ADD_ATTRIBUTE(x509_om_ele, env, attr, x509_om_node);
	    policy_om_node2 = create_policy_node(env, x509_om_node);
	    axiom_element_create(env, policy_om_node2, "WssX509V3Token10", sp_ns, &token_id_om_node);
	    return token_id_om_node;
	}
    }
    
    if ( tmp != NULL && Z_TYPE_PP(tmp) == IS_BOOL)
    {
	if ( Z_BVAL_PP(tmp))
	{
	    sp_ns = axiom_namespace_create(env,"http://schemas.xmlsoap.org/ws/2005/07/securitypolicy", "sp");

	    rec_token_om_ele = axiom_element_create(env, parent_om_node, "RecipientToken", sp_ns, &rec_token_om_node);
	    policy_om_node1 = create_policy_node(env, rec_token_om_node);
	    x509_om_ele = axiom_element_create(env, policy_om_node1, "X509Token", sp_ns, &x509_om_node);

	    /* Here uri may change according to the options difined in policy spec
	     * e.g - 'Always' may be changed as 'Never'*/
	    attr = axiom_attribute_create(env, "IncludeToken", "http://schemas.xmlsoap.org/ws/2005/07/securitypolicy/IncludeToken/Always",
					  sp_ns);
	    AXIOM_ELEMENT_ADD_ATTRIBUTE(x509_om_ele, env, attr, x509_om_node);
	    policy_om_node2 = create_policy_node(env, x509_om_node);
	    axiom_element_create(env, policy_om_node2, "WssX509V3Token10", sp_ns, &token_id_om_node);
	    return token_id_om_node;

	}
	else 
	    tmp_node;
    }
    return tmp_node;
}




axiom_node_t *
create_algorithm_suite(const axis2_env_t *env,
                       axiom_node_t *parent_node,
                       zval **tmp TSRMLS_DC)
{
    axiom_node_t *alg_om_node = NULL;
    axiom_node_t *alg_name_om_node = NULL;
    axiom_node_t *policy_om_node = NULL;
    axiom_node_t *tmp_node = NULL;

    axiom_namespace_t *sp_ns = NULL;
    char *alg_name = NULL;

    tmp_node = parent_node;

/*     sp_ns = axiom_namespace_create(env, "http://schemas.xmlsoap.org/ws/2005/07/securitypolicy", "sp"); */
/*     axiom_element_create(env, parent_node, "AlgorithmSuite", sp_ns, &alg_om_node); */
    if ( tmp != NULL && Z_TYPE_PP(tmp) == IS_STRING)
    {
	int i ;
	alg_name = Z_STRVAL_PP(tmp);
/* 	php_printf("\n Alg_name is %s", alg_name); */

	for (i = 0; i < ArrySize ; i++)
	{
	    if ((strcmp(alg_name, algorithmArry[i]) == 0))
	    {
		sp_ns = axiom_namespace_create(env, "http://schemas.xmlsoap.org/ws/2005/07/securitypolicy", "sp");
		axiom_element_create(env, parent_node, "AlgorithmSuite", sp_ns, &alg_om_node);
		policy_om_node = create_policy_node(env, alg_om_node);
		axiom_element_create(env, policy_om_node, algorithmArry[i], sp_ns, &alg_name_om_node);

		return alg_name_om_node;
	    }
	}
    }

    return tmp_node;

}
/* this function is called if only user want to sign the xml
 * message.Now only we have only default case "OnlySignEntireHeadersAndBody */
axiom_node_t *
create_sign_parts(const axis2_env_t *env,
              axiom_node_t *parent_node,
              zval **tmp TSRMLS_DC)
{
    axiom_node_t *sign_om_node = NULL;
    axiom_namespace_t *sp_ns = NULL;

    sp_ns = axiom_namespace_create(env, WS_SEC_POLICY_NAMESPACE_URI, WS_SEC_POLICY_NAMESPACE);
    axiom_element_create(env, parent_node, "OnlySignEntireHeadersAndBody", sp_ns, &sign_om_node);
    return sign_om_node;
}

axiom_node_t *
create_encrypt_parts(const axis2_env_t *env,
              axiom_node_t *parent_node,
              zval **tmp TSRMLS_DC)
{
    axiom_node_t *encrypt_om_node = NULL;
    axiom_node_t *part_om_node = NULL;
    axiom_namespace_t *sp_ns = NULL;

    sp_ns = axiom_namespace_create(env, WS_SEC_POLICY_NAMESPACE_URI, WS_SEC_POLICY_NAMESPACE);
    axiom_element_create(env, parent_node, "EncryptedParts", sp_ns, &encrypt_om_node);
    axiom_element_create(env, encrypt_om_node, "Body", sp_ns, &part_om_node);
    return part_om_node;
}


axiom_node_t *
create_layout(const axis2_env_t *env,
              axiom_node_t *parent_node,
              zval **tmp TSRMLS_DC)
{
    axiom_node_t *layout_om_node = NULL;
    axiom_node_t *policy_om_node = NULL;
    axiom_node_t *strict_om_node = NULL;

    axiom_namespace_t *sp_ns = NULL;

    sp_ns = axiom_namespace_create(env, "http://schemas.xmlsoap.org/ws/2005/07/securitypolicy", "sp");
    axiom_element_create(env, parent_node, "Layout", sp_ns, &layout_om_node);
    policy_om_node = create_policy_node(env, layout_om_node);
    axiom_element_create(env, policy_om_node, "Strict", sp_ns, &strict_om_node);
    return strict_om_node;
}

axiom_node_t *
create_default_sign(const axis2_env_t *env,
					axiom_node_t *parent_node TSRMLS_DC)
{
    zval *test;
    MAKE_STD_ZVAL(test);
    ZVAL_STRING(test, "X509", 1);
    create_initiator_token(env, parent_node, &test TSRMLS_CC);

}

axiom_node_t *
create_default_encrypt(const axis2_env_t *env,
					   axiom_node_t *parent_node TSRMLS_DC)
{
    zval *test;
    MAKE_STD_ZVAL(test);
    ZVAL_STRING(test, "X509", 1);
    create_recipient_token(env, parent_node, &test TSRMLS_CC);

}

axiom_node_t *
create_username_token(const axis2_env_t *env,
		      axiom_node_t *parent_node,
		      zval **tmp TSRMLS_DC)
{
    axiom_node_t *signsupport_om_node = NULL;
    axiom_node_t *policy_om_node = NULL;
    axiom_node_t *ut_om_node = NULL;

    axiom_element_t *ele = NULL;
    axiom_attribute_t *attr = NULL;
    axiom_namespace_t *sp_ns = NULL;

    sp_ns = axiom_namespace_create(env, WS_SEC_POLICY_NAMESPACE_URI, WS_SEC_POLICY_NAMESPACE);
    axiom_element_create(env, parent_node, "SignedSupportingTokens", sp_ns, &signsupport_om_node);
    policy_om_node = create_policy_node(env, signsupport_om_node);
    ele = axiom_element_create(env, policy_om_node, "UsernameToken", sp_ns, &ut_om_node);
    attr = axiom_attribute_create(env, "IncludeToken", "http://schemas.xmlsoap.org/ws/2005/07/securitypolicy/IncludeToken/Always",
				  sp_ns);
    AXIOM_ELEMENT_ADD_ATTRIBUTE(ele, env, attr, ut_om_node);
            
    return ut_om_node;

}

axiom_node_t *
create_token_reference(const axis2_env_t *env,
		       axiom_node_t *parent_node,
		       zval **tmp TSRMLS_DC)
{
    return parent_node;
}




int ws_policy_set_policy_options(zval *this_ptr,
                                 HashTable *ht,
                                 const axis2_env_t *env TSRMLS_DC)
{
    zval **tmp = NULL;

    if (!ht)
        return AXIS2_FAILURE;


    /* for security policy related things */
    if(zend_hash_find(ht, "security", sizeof("security"), (void **)&tmp) == SUCCESS &&
       Z_TYPE_PP(tmp) == IS_ARRAY)
    {
        set_security_policy_options(this_ptr, tmp , env TSRMLS_CC);
    }
    
    return AXIS2_SUCCESS;

}

int set_security_policy_options(zval *policy_obj,
                                 zval **sec_options,
                                 const axis2_env_t *env TSRMLS_DC)
{
    HashTable *ht_sec = NULL;

    zval **sec_prop = NULL;

    if (sec_options == NULL)
        return AXIS2_FAILURE ;

    ht_sec = Z_ARRVAL_PP(sec_options);
    if (!ht_sec)
        return AXIS2_FAILURE;


    if (zend_hash_find(ht_sec, WS_SIGN, sizeof(WS_SIGN), (void **)&sec_prop) == SUCCESS &&
            (Z_TYPE_PP(sec_prop) == IS_STRING || Z_TYPE_PP(sec_prop) == IS_BOOL ))
    {
        add_property_zval(policy_obj, WS_SIGN, *sec_prop);
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_policy] signing is enable ");
    }

    if (zend_hash_find(ht_sec, WS_ENCRYPT, sizeof(WS_ENCRYPT), (void **)&sec_prop) == SUCCESS &&
            (Z_TYPE_PP(sec_prop) == IS_STRING || Z_TYPE_PP(sec_prop) == IS_BOOL))
    {
        add_property_zval(policy_obj, WS_ENCRYPT, *sec_prop);
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_policy]  encryption is enable ");

    }

    if (zend_hash_find(ht_sec, WS_ALGORITHM, sizeof(WS_ALGORITHM), (void **)&sec_prop) == SUCCESS &&
            (Z_TYPE_PP(sec_prop) == IS_STRING ))
    {
        add_property_stringl(policy_obj, WS_ALGORITHM, Z_STRVAL_PP(sec_prop), Z_STRLEN_PP(sec_prop), 1 );
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_policy] algorithmSuite is enable ");

    }

    if (zend_hash_find(ht_sec, WS_LAYOUT, sizeof(WS_LAYOUT), (void **)&sec_prop) == SUCCESS &&
            (Z_TYPE_PP(sec_prop) == IS_STRING))
    {
        add_property_stringl(policy_obj, WS_LAYOUT, Z_STRVAL_PP(sec_prop), Z_STRLEN_PP(sec_prop), 1 );
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_policy] layout is enable ");

    }

    if (zend_hash_find(ht_sec, WS_TIMESTAMP, sizeof(WS_TIMESTAMP), (void **)&sec_prop) == SUCCESS &&
            (Z_TYPE_PP(sec_prop) == IS_BOOL))
    {
        add_property_bool(policy_obj, WS_TIMESTAMP, Z_BVAL_PP(sec_prop));
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_policy] timestamp is enable ");

    }

    if (zend_hash_find(ht_sec, WS_PROTECTION_ORDER, sizeof(WS_PROTECTION_ORDER), (void **)&sec_prop) == SUCCESS &&
            (Z_TYPE_PP(sec_prop) == IS_STRING))
    {
        add_property_stringl(policy_obj, WS_PROTECTION_ORDER, Z_STRVAL_PP(sec_prop), Z_STRLEN_PP(sec_prop), 1 );
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_policy] protection order is enable ");

    }

    if (zend_hash_find(ht_sec, WS_UT, sizeof(WS_UT), (void **)&sec_prop) == SUCCESS &&
            (Z_TYPE_PP(sec_prop) == IS_BOOL))
    {
        add_property_bool(policy_obj, WS_UT, Z_BVAL_PP(sec_prop));
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_policy] UsernameToken is enable ");

    }

    if (zend_hash_find(ht_sec, WS_TOKEN_REFERENCE, sizeof(WS_TOKEN_REFERENCE), (void **)&sec_prop) == SUCCESS &&
            (Z_TYPE_PP(sec_prop) == IS_STRING))
    {
        add_property_stringl(policy_obj, WS_TOKEN_REFERENCE, Z_STRVAL_PP(sec_prop), Z_STRLEN_PP(sec_prop), 1 );
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[wsf_policy] token reference  is enable ");

    }


    return AXIS2_SUCCESS;
}





















