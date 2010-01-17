

    /**
     * axis2_skel_AuthenticationAdminService.c
     *
     * This file was auto-generated from WSDL for "AuthenticationAdminService|http://authentication.services.core.carbon.wso2.org" service
     * by the Apache Axis2/C version: #axisVersion# #today#
     * axis2_skel_AuthenticationAdminService Axis2/C skeleton for the axisService
     */

     #include "codegen/axis2_skel_AuthenticationAdminService.h"

     

		 
        /**
         * auto generated function definition signature
         * for "logout|http://authentication.services.core.carbon.wso2.org" operation.
         * @param env environment ( mandatory)* @param MessageContext the outmessage context
         *
         * @return 
         */
        axis2_status_t  axis2_skel_AuthenticationAdminService_logout(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
                                          axis2_skel_AuthenticationAdminService_logout_fault *fault )
        {
          /* TODO fill this with the necessary business logic */
          return AXIS2_SUCCESS;
        }
     

		 
        /**
         * auto generated function definition signature
         * for "login|http://authentication.services.core.carbon.wso2.org" operation.
         * @param env environment ( mandatory)* @param MessageContext the outmessage context
         * @param _login of the adb_login_t*
         *
         * @return adb_loginResponse_t*
         */
        adb_loginResponse_t* axis2_skel_AuthenticationAdminService_login(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
                                              adb_login_t* _login,
                                          axis2_skel_AuthenticationAdminService_login_fault *fault )
        {
          /* TODO fill this with the necessary business logic */
			adb_loginResponse_t *loginResponse = adb_loginResponse_create(env);
			adb_loginResponse_set_return(loginResponse, env, AXIS2_TRUE);
          return loginResponse;
        }
     

		 
        /**
         * auto generated function definition signature
         * for "getSystemPermissionOfUser|http://authentication.services.core.carbon.wso2.org" operation.
         * @param env environment ( mandatory)* @param MessageContext the outmessage context
         * @param _getSystemPermissionOfUser of the adb_getSystemPermissionOfUser_t*
         *
         * @return adb_getSystemPermissionOfUserResponse_t*
         */
        adb_getSystemPermissionOfUserResponse_t* axis2_skel_AuthenticationAdminService_getSystemPermissionOfUser(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
                                              adb_getSystemPermissionOfUser_t* _getSystemPermissionOfUser,
                                          axis2_skel_AuthenticationAdminService_getSystemPermissionOfUser_fault *fault )
        {
			adb_getSystemPermissionOfUserResponse_t* response = NULL;
			axutil_array_list_t *list = NULL;

			/* TODO fill this with the necessary business logic */
			list = axutil_array_list_create(env, 0);
			axutil_array_list_add(list, env, axutil_strdup(env, "login"));
			axutil_array_list_add(list, env, axutil_strdup(env, "manage-configuration"));
			axutil_array_list_add(list, env, axutil_strdup(env, "manage-security"));
			axutil_array_list_add(list, env, axutil_strdup(env, "upload-services"));
			axutil_array_list_add(list, env, axutil_strdup(env, "manage-services"));
			axutil_array_list_add(list, env, axutil_strdup(env, "manage-lc-configuration"));
			axutil_array_list_add(list, env, axutil_strdup(env, "manage-mediation"));
			axutil_array_list_add(list, env, axutil_strdup(env, "monitor-system"));
			axutil_array_list_add(list, env, axutil_strdup(env, "delegate-identity"));

			response = adb_getSystemPermissionOfUserResponse_create_with_values(env, list);
			return response;
        }
     

		 
        /**
         * auto generated function definition signature
         * for "loginWithDelegation|http://authentication.services.core.carbon.wso2.org" operation.
         * @param env environment ( mandatory)* @param MessageContext the outmessage context
         * @param _loginWithDelegation of the adb_loginWithDelegation_t*
         *
         * @return adb_loginWithDelegationResponse_t*
         */
        adb_loginWithDelegationResponse_t* axis2_skel_AuthenticationAdminService_loginWithDelegation(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
                                              adb_loginWithDelegation_t* _loginWithDelegation,
                                          axis2_skel_AuthenticationAdminService_loginWithDelegation_fault *fault )
        {
			adb_loginWithDelegationResponse_t* loginWithDelegationRes = adb_loginWithDelegationResponse_create(env);
			adb_loginWithDelegationResponse_set_return(loginWithDelegationRes, env, AXIS2_TRUE);
			return loginWithDelegationRes;
        }
     

		 
        /**
         * auto generated function definition signature
         * for "getUserPermissions|http://authentication.services.core.carbon.wso2.org" operation.
         * @param env environment ( mandatory)* @param MessageContext the outmessage context
         * @param _getUserPermissions of the adb_getUserPermissions_t*
         *
         * @return adb_getUserPermissionsResponse_t*
         */
        adb_getUserPermissionsResponse_t* axis2_skel_AuthenticationAdminService_getUserPermissions(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
                                              adb_getUserPermissions_t* _getUserPermissions,
                                          axis2_skel_AuthenticationAdminService_getUserPermissions_fault *fault )
        {
			adb_getUserPermissionsResponse_t* response = NULL;
    axutil_array_list_t *list = NULL;

    /* TODO fill this with the necessary business logic */
    list = axutil_array_list_create(env, 0);
    axutil_array_list_add(list, env, axutil_strdup(env, "login"));
    axutil_array_list_add(list, env, axutil_strdup(env, "manage-configuration"));
    axutil_array_list_add(list, env, axutil_strdup(env, "manage-security"));
    axutil_array_list_add(list, env, axutil_strdup(env, "upload-services"));
    axutil_array_list_add(list, env, axutil_strdup(env, "manage-services"));
    axutil_array_list_add(list, env, axutil_strdup(env, "manage-lc-configuration"));
    axutil_array_list_add(list, env, axutil_strdup(env, "manage-mediation"));
    axutil_array_list_add(list, env, axutil_strdup(env, "monitor-system"));
    axutil_array_list_add(list, env, axutil_strdup(env, "delegate-identity"));

	response = adb_getUserPermissionsResponse_create_with_values(env, list);
    return response;
        }
     

