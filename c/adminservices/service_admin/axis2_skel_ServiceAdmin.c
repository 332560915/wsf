

/**
* axis2_skel_ServiceAdmin.c
*
* This file was auto-generated from WSDL for "ServiceAdmin|http://mgt.service.carbon.wso2.org" service
* by the Apache Axis2/C version: #axisVersion# #today#
* axis2_skel_ServiceAdmin Axis2/C skeleton for the axisService
*/

#include "codegen/axis2_skel_ServiceAdmin.h"
#include "service_admin_constants.h"

static axis2_char_t *axis2_skel_ServiceAdmin_get_service_type(
    const axutil_env_t *env,
    axis2_svc_t *svc);

/**
* auto generated function definition signature
* for "startService|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _startService of the adb_startService_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_startService(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
													 adb_startService_t* _startService,
													 axis2_skel_ServiceAdmin_startService_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "getExposedTransports|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _getExposedTransports of the adb_getExposedTransports_t*
*
* @return adb_getExposedTransportsResponse_t*
*/
adb_getExposedTransportsResponse_t* axis2_skel_ServiceAdmin_getExposedTransports(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																				 adb_getExposedTransports_t* _getExposedTransports )
{
	/* TODO fill this with the necessary business logic */
	return (adb_getExposedTransportsResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "listServiceGroup|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _listServiceGroup of the adb_listServiceGroup_t*
*
* @return adb_listServiceGroupResponse_t*
*/
adb_listServiceGroupResponse_t* axis2_skel_ServiceAdmin_listServiceGroup(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																		 adb_listServiceGroup_t* _listServiceGroup )
{
	/* TODO fill this with the necessary business logic */
	return (adb_listServiceGroupResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "addTransportBinding|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _addTransportBinding of the adb_addTransportBinding_t*
*
* @return adb_addTransportBindingResponse_t*
*/
adb_addTransportBindingResponse_t* axis2_skel_ServiceAdmin_addTransportBinding(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																			   adb_addTransportBinding_t* _addTransportBinding,
																			   axis2_skel_ServiceAdmin_addTransportBinding_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return (adb_addTransportBindingResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "getWSDL|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _getWSDL of the adb_getWSDL_t*
*
* @return adb_getWSDLResponse_t*
*/
adb_getWSDLResponse_t* axis2_skel_ServiceAdmin_getWSDL(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
													   adb_getWSDL_t* _getWSDL )
{
	/* TODO fill this with the necessary business logic */
	return (adb_getWSDLResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "getNumberOfFaultyServices|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
*
* @return adb_getNumberOfFaultyServicesResponse_t*
*/
adb_getNumberOfFaultyServicesResponse_t* axis2_skel_ServiceAdmin_getNumberOfFaultyServices(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx )
{
	/* TODO fill this with the necessary business logic */
	return (adb_getNumberOfFaultyServicesResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "configureMTOM|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _configureMTOM of the adb_configureMTOM_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_configureMTOM(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
													  adb_configureMTOM_t* _configureMTOM )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "setServicePolicy|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _setServicePolicy of the adb_setServicePolicy_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_setServicePolicy(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
														 adb_setServicePolicy_t* _setServicePolicy,
														 axis2_skel_ServiceAdmin_setServicePolicy_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "getPolicy|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _getPolicy of the adb_getPolicy_t*
*
* @return adb_getPolicyResponse_t*
*/
adb_getPolicyResponse_t* axis2_skel_ServiceAdmin_getPolicy(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
														   adb_getPolicy_t* _getPolicy )
{
	/* TODO fill this with the necessary business logic */
	return (adb_getPolicyResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "getFaultyServiceArchives|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _getFaultyServiceArchives of the adb_getFaultyServiceArchives_t*
*
* @return adb_getFaultyServiceArchivesResponse_t*
*/
adb_getFaultyServiceArchivesResponse_t* axis2_skel_ServiceAdmin_getFaultyServiceArchives(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																						 adb_getFaultyServiceArchives_t* _getFaultyServiceArchives )
{
	/* TODO fill this with the necessary business logic */
	return (adb_getFaultyServiceArchivesResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "addPoliciesToService|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _addPoliciesToService of the adb_addPoliciesToService_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_addPoliciesToService(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
															 adb_addPoliciesToService_t* _addPoliciesToService,
															 axis2_skel_ServiceAdmin_addPoliciesToService_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "getServiceParameters|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _getServiceParameters of the adb_getServiceParameters_t*
*
* @return adb_getServiceParametersResponse_t*
*/
adb_getServiceParametersResponse_t* axis2_skel_ServiceAdmin_getServiceParameters(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																				 adb_getServiceParameters_t* _getServiceParameters,
																				 axis2_skel_ServiceAdmin_getServiceParameters_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return (adb_getServiceParametersResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "setBindingOperationMessagePolicy|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _setBindingOperationMessagePolicy of the adb_setBindingOperationMessagePolicy_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_setBindingOperationMessagePolicy(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																		 adb_setBindingOperationMessagePolicy_t* _setBindingOperationMessagePolicy,
																		 axis2_skel_ServiceAdmin_setBindingOperationMessagePolicy_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "setBindingPolicy|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _setBindingPolicy of the adb_setBindingPolicy_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_setBindingPolicy(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
														 adb_setBindingPolicy_t* _setBindingPolicy,
														 axis2_skel_ServiceAdmin_setBindingPolicy_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "removeServicePoliciesByNamespace|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _removeServicePoliciesByNamespace of the adb_removeServicePoliciesByNamespace_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_removeServicePoliciesByNamespace(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																		 adb_removeServicePoliciesByNamespace_t* _removeServicePoliciesByNamespace,
																		 axis2_skel_ServiceAdmin_removeServicePoliciesByNamespace_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "setBindingOperationPolicy|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _setBindingOperationPolicy of the adb_setBindingOperationPolicy_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_setBindingOperationPolicy(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																  adb_setBindingOperationPolicy_t* _setBindingOperationPolicy,
																  axis2_skel_ServiceAdmin_setBindingOperationPolicy_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "setModulePolicy|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _setModulePolicy of the adb_setModulePolicy_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_setModulePolicy(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
														adb_setModulePolicy_t* _setModulePolicy,
														axis2_skel_ServiceAdmin_setModulePolicy_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "getBindingOperationMessagePolicy|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _getBindingOperationMessagePolicy of the adb_getBindingOperationMessagePolicy_t*
*
* @return adb_getBindingOperationMessagePolicyResponse_t*
*/
adb_getBindingOperationMessagePolicyResponse_t* axis2_skel_ServiceAdmin_getBindingOperationMessagePolicy(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																										 adb_getBindingOperationMessagePolicy_t* _getBindingOperationMessagePolicy )
{
	/* TODO fill this with the necessary business logic */
	return (adb_getBindingOperationMessagePolicyResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "changeServiceState|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _changeServiceState of the adb_changeServiceState_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_changeServiceState(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
														   adb_changeServiceState_t* _changeServiceState )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "setServiceParameters|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _setServiceParameters of the adb_setServiceParameters_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_setServiceParameters(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
															 adb_setServiceParameters_t* _setServiceParameters )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "getOperationMessagePolicy|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _getOperationMessagePolicy of the adb_getOperationMessagePolicy_t*
*
* @return adb_getOperationMessagePolicyResponse_t*
*/
adb_getOperationMessagePolicyResponse_t* axis2_skel_ServiceAdmin_getOperationMessagePolicy(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																						   adb_getOperationMessagePolicy_t* _getOperationMessagePolicy )
{
	/* TODO fill this with the necessary business logic */
	return (adb_getOperationMessagePolicyResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "getNumberOfActiveServices|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
*
* @return adb_getNumberOfActiveServicesResponse_t*
*/
adb_getNumberOfActiveServicesResponse_t* axis2_skel_ServiceAdmin_getNumberOfActiveServices(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																						   axis2_skel_ServiceAdmin_getNumberOfActiveServices_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return (adb_getNumberOfActiveServicesResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "getServiceData|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _getServiceData of the adb_getServiceData_t*
*
* @return adb_getServiceDataResponse_t*
*/
adb_getServiceDataResponse_t* 
axis2_skel_ServiceAdmin_getServiceData(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
										adb_getServiceData_t* _getServiceData )
{
	    adb_getServiceDataResponse_t *response = NULL;
    axis2_char_t *service_name = NULL;
    adb_ServiceMetaData_t *adb_svc = NULL;


    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_conf_t *axis2_conf = NULL;
    axis2_svc_t *service = NULL;

    service_name = adb_getServiceData_get_serviceName(_getServiceData, env);
    if(!service_name)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Service name is not given.");
        AXIS2_ERROR_SET(env->error,
            AXIS2_ERROR_SVC_SKEL_INVALID_OPERATION_PARAMETERS_IN_SOAP_REQUEST, AXIS2_FAILURE);
        return NULL;
    }

    conf_ctx = axis2_msg_ctx_get_conf_ctx(msg_ctx, env);
    axis2_conf = axis2_conf_ctx_get_conf(conf_ctx, env);
    service = axis2_conf_get_svc(axis2_conf, env, service_name);

    if(!service)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Cannot find service %s", service_name);
        AXIS2_ERROR_SET(env->error,
            AXIS2_ERROR_SVC_SKEL_INVALID_OPERATION_PARAMETERS_IN_SOAP_REQUEST, AXIS2_FAILURE);
        return NULL;
    }

    adb_svc = adb_ServiceMetaData_create(env);
    adb_ServiceMetaData_set_active(adb_svc, env, AXIS2_TRUE); /* TODO get this from somewhere */
    adb_ServiceMetaData_set_description(adb_svc, env, axis2_svc_get_svc_desc(service, env));
    adb_ServiceMetaData_set_disableTryit(adb_svc, env, AXIS2_TRUE);
    adb_ServiceMetaData_add_eprs(adb_svc, env, "http://localhost:9090/aaa"); /* TODO fix the epr, get it from somewhere */
    adb_ServiceMetaData_set_foundWebResources(adb_svc, env, AXIS2_FALSE);
    adb_ServiceMetaData_set_mtomStatus(adb_svc, env, "optional");
    adb_ServiceMetaData_set_name(adb_svc, env, service_name);
    adb_ServiceMetaData_add_operations(adb_svc, env, "test 0");
    adb_ServiceMetaData_set_scope(adb_svc, env, "application");
    adb_ServiceMetaData_set_serviceGroupName(adb_svc, env, "ServieGroup");
    adb_ServiceMetaData_set_serviceId(adb_svc, env, service_name);
    adb_ServiceMetaData_set_serviceVersion(adb_svc, env, "");
    adb_ServiceMetaData_set_serviceType(adb_svc, env,
        axis2_skel_ServiceAdmin_get_service_type(env, service));
    adb_ServiceMetaData_set_tryitURL(adb_svc, env, "http://localhost:9090/tryit");
    /* TODO fix the wsdl endpoints */
    adb_ServiceMetaData_add_wsdlURLs(adb_svc, env, "http://localhost:9090/test?wsdl");
    adb_ServiceMetaData_add_wsdlURLs(adb_svc, env, "http://localhost:9090/test?wsdl2");

    response = adb_getServiceDataResponse_create_with_values(env, adb_svc);
    return response;
}



/**
* auto generated function definition signature
* for "removeTransportBinding|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _removeTransportBinding of the adb_removeTransportBinding_t*
*
* @return adb_removeTransportBindingResponse_t*
*/
adb_removeTransportBindingResponse_t* axis2_skel_ServiceAdmin_removeTransportBinding(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																					 adb_removeTransportBinding_t* _removeTransportBinding,
																					 axis2_skel_ServiceAdmin_removeTransportBinding_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return (adb_removeTransportBindingResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "getPolicies|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _getPolicies of the adb_getPolicies_t*
*
* @return adb_getPoliciesResponse_t*
*/
adb_getPoliciesResponse_t* axis2_skel_ServiceAdmin_getPolicies(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
															   adb_getPolicies_t* _getPolicies )
{
	/* TODO fill this with the necessary business logic */
	return (adb_getPoliciesResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "removeServiceParameter|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _removeServiceParameter of the adb_removeServiceParameter_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_removeServiceParameter(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
															   adb_removeServiceParameter_t* _removeServiceParameter )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "getBindingOperationPolicy|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _getBindingOperationPolicy of the adb_getBindingOperationPolicy_t*
*
* @return adb_getBindingOperationPolicyResponse_t*
*/
adb_getBindingOperationPolicyResponse_t* axis2_skel_ServiceAdmin_getBindingOperationPolicy(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																						   adb_getBindingOperationPolicy_t* _getBindingOperationPolicy )
{
	/* TODO fill this with the necessary business logic */
	return (adb_getBindingOperationPolicyResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "deleteServiceGroups|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _deleteServiceGroups of the adb_deleteServiceGroups_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_deleteServiceGroups(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
															adb_deleteServiceGroups_t* _deleteServiceGroups )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "getNumberOfInactiveServices|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
*
* @return adb_getNumberOfInactiveServicesResponse_t*
*/
adb_getNumberOfInactiveServicesResponse_t* axis2_skel_ServiceAdmin_getNumberOfInactiveServices(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																							   axis2_skel_ServiceAdmin_getNumberOfInactiveServices_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return (adb_getNumberOfInactiveServicesResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "setServiceOperationPolicy|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _setServiceOperationPolicy of the adb_setServiceOperationPolicy_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_setServiceOperationPolicy(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																  adb_setServiceOperationPolicy_t* _setServiceOperationPolicy,
																  axis2_skel_ServiceAdmin_setServiceOperationPolicy_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "deleteFaultyServiceGroup|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _deleteFaultyServiceGroup of the adb_deleteFaultyServiceGroup_t*
*
* @return adb_deleteFaultyServiceGroupResponse_t*
*/
adb_deleteFaultyServiceGroupResponse_t* axis2_skel_ServiceAdmin_deleteFaultyServiceGroup(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																						 adb_deleteFaultyServiceGroup_t* _deleteFaultyServiceGroup )
{
	/* TODO fill this with the necessary business logic */
	return (adb_deleteFaultyServiceGroupResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "engageModuleToService|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _engageModuleToService of the adb_engageModuleToService_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_engageModuleToService(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
															  adb_engageModuleToService_t* _engageModuleToService,
															  axis2_skel_ServiceAdmin_engageModuleToService_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "deleteFaultyServiceGroups|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _deleteFaultyServiceGroups of the adb_deleteFaultyServiceGroups_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_deleteFaultyServiceGroups(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																  adb_deleteFaultyServiceGroups_t* _deleteFaultyServiceGroups )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "removeBindingPolicy|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _removeBindingPolicy of the adb_removeBindingPolicy_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_removeBindingPolicy(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
															adb_removeBindingPolicy_t* _removeBindingPolicy,
															axis2_skel_ServiceAdmin_removeBindingPolicy_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "deleteAllFaultyServiceGroups|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_deleteAllFaultyServiceGroups(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "deleteAllNonAdminServiceGroups|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_deleteAllNonAdminServiceGroups(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "getModulePolicy|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _getModulePolicy of the adb_getModulePolicy_t*
*
* @return adb_getModulePolicyResponse_t*
*/
adb_getModulePolicyResponse_t* axis2_skel_ServiceAdmin_getModulePolicy(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																	   adb_getModulePolicy_t* _getModulePolicy )
{
	/* TODO fill this with the necessary business logic */
	return (adb_getModulePolicyResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "setPolicy|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _setPolicy of the adb_setPolicy_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_setPolicy(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
												  adb_setPolicy_t* _setPolicy,
												  axis2_skel_ServiceAdmin_setPolicy_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "listServiceGroups|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _listServiceGroups of the adb_listServiceGroups_t*
*
* @return adb_listServiceGroupsResponse_t*
*/
adb_listServiceGroupsResponse_t* 
axis2_skel_ServiceAdmin_listServiceGroups(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
										adb_listServiceGroups_t* _listServiceGroups )
{
adb_listServiceGroupsResponse_t *response = NULL;
    adb_ServiceGroupMetaDataWrapper_t *metadata = NULL;
    axutil_array_list_t *adb_svc_grps = NULL; /* contains list of adb_ServiceGroupMetaData_t */
    axutil_array_list_t *adb_svc_types = NULL; /* contains list of service types(char*) */
    axutil_hash_t *service_types = NULL; /* to get the unique set of service types */
    int no_of_correct_service_grp = 0;
    int no_of_faulty_service_grp = 0;
    int no_of_pages = 1;

    axis2_char_t *service_type_filter = NULL;
    axis2_char_t *svc_grp_filter = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_conf_t *axis2_conf = NULL;
    axutil_hash_t *svc_grp_map = NULL;
    axutil_hash_index_t *hi;
    adb_svc_grps = axutil_array_list_create(env, 0);
    service_types = axutil_hash_make(env);

    /* if the service type filter is not set, set it to default value */
    service_type_filter = adb_listServiceGroups_get_serviceTypeFilter(_listServiceGroups, env);
    if((!service_type_filter) || (!axutil_strcmp(service_type_filter, "")))
    {
        service_type_filter = DEFAULT_SERVICE_TYPE_FILTER;
    }

    svc_grp_filter = adb_listServiceGroups_get_serviceGroupSearchString(_listServiceGroups,env);

    /* get the service groups and populate it */
    conf_ctx = axis2_msg_ctx_get_conf_ctx(msg_ctx, env);
    axis2_conf = axis2_conf_ctx_get_conf(conf_ctx, env);
    svc_grp_map = axis2_conf_get_all_svc_grps(axis2_conf, env);

    for(hi = axutil_hash_first(svc_grp_map, env); hi; hi = axutil_hash_next(env, hi))
    {
        axis2_svc_grp_t *svc_grp = NULL;
        axis2_char_t *svc_grp_name = NULL;
        axutil_hash_t *svc_map = NULL;
        adb_ServiceGroupMetaData_t *adb_svc_grp = NULL;
        axutil_array_list_t *adb_svc_list = NULL; /* contains list of adb_ServiceMetaData_t */
        axutil_hash_index_t *hi_svc;
        axutil_array_list_t *engaged_module  = NULL;

        axutil_hash_this(hi, &svc_grp_name, NULL, &svc_grp);

        /* Check whether service group filter is satisfied */
        if(svc_grp_filter && axutil_strcmp(svc_grp_filter, ""))
        {
            if(!axutil_strstr(svc_grp_name, svc_grp_filter))
            {
                /* filter is not satisfied */
                continue;
            }
        }

        svc_map = axis2_svc_grp_get_all_svcs(svc_grp, env);
        adb_svc_list = axutil_array_list_create(env, 0);
        for(hi_svc = axutil_hash_first(svc_map, env); hi_svc; hi_svc = axutil_hash_next(env, hi_svc))
        {
            axis2_svc_t *svc = NULL;
            axis2_char_t *svc_name = NULL;
            adb_ServiceMetaData_t *adb_svc = NULL;
            axis2_char_t *service_type = NULL;

            axutil_hash_this(hi_svc, &svc_name, NULL, &svc);
            /* Get the service type and filter using the given service type */
            service_type = axis2_skel_ServiceAdmin_get_service_type(env, svc);
            if(axutil_strcmp(service_type_filter, DEFAULT_SERVICE_TYPE_FILTER) &&
                axutil_strcmp(service_type, service_type_filter))
            {
                /* filter is not satisfied */
                continue;
            }

            /* check whether service type is already added, if not add it */
            if(!axutil_hash_get(service_types, service_type, AXIS2_HASH_KEY_STRING))
            {
                axutil_hash_set(service_types, service_type, AXIS2_HASH_KEY_STRING, service_type);
            }

            adb_svc = adb_ServiceMetaData_create(env);
            adb_ServiceMetaData_set_active(adb_svc, env, AXIS2_TRUE); /* TODO get this from somewhere */
            adb_ServiceMetaData_set_disableTryit(adb_svc, env, AXIS2_TRUE);
            /*adb_ServiceMetaData_set_tryitURL(adb_svc, env, "http://localhost:9090/test/tryit");*/
            adb_ServiceMetaData_set_name(adb_svc, env, svc_name);
            adb_ServiceMetaData_set_serviceType(adb_svc, env, service_type);

            /* TODO fix the wsdl endpoints */
            adb_ServiceMetaData_add_wsdlURLs(adb_svc, env, "http://localhost:9090/test?wsdl");
            adb_ServiceMetaData_add_wsdlURLs(adb_svc, env, "http://localhost:9090/test?wsdl2");

            axutil_array_list_add(adb_svc_list, env, adb_svc);
        }

        /* check whether there are atleast one service for this group. There might be empty groups
         * due to the filters
         */
        if(axutil_array_list_size(adb_svc_list, env))
        {
            adb_svc_grp = adb_ServiceGroupMetaData_create(env);
            adb_ServiceGroupMetaData_set_serviceGroupName(adb_svc_grp, env, svc_grp_name);
            adb_ServiceGroupMetaData_set_services(adb_svc_grp, env, adb_svc_list);
            axutil_array_list_add(adb_svc_grps, env, adb_svc_grp);
        }
        else
        {
            axutil_array_list_free(adb_svc_list, env);
        }
    }

    no_of_correct_service_grp = axutil_array_list_size(adb_svc_grps, env);
    adb_svc_types = axutil_array_list_create(env, 0);
    for(hi = axutil_hash_first(service_types, env); hi; hi = axutil_hash_next(env, hi))
    {
        axis2_char_t *service_type = NULL;
        axutil_hash_this(hi, &service_type, NULL, NULL);
        axutil_array_list_add(adb_svc_types, env, axutil_strdup(env, service_type));
    }
    axutil_hash_free(service_types, env);

    metadata = adb_ServiceGroupMetaDataWrapper_create_with_values(env, adb_svc_grps,
        no_of_correct_service_grp, no_of_faulty_service_grp, no_of_pages, adb_svc_types);
    response = adb_listServiceGroupsResponse_create_with_values(env, metadata);
    return response;
}



/**
* auto generated function definition signature
* for "setServiceOperationMessagePolicy|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _setServiceOperationMessagePolicy of the adb_setServiceOperationMessagePolicy_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_setServiceOperationMessagePolicy(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																		 adb_setServiceOperationMessagePolicy_t* _setServiceOperationMessagePolicy,
																		 axis2_skel_ServiceAdmin_setServiceOperationMessagePolicy_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "getBindingPolicy|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _getBindingPolicy of the adb_getBindingPolicy_t*
*
* @return adb_getBindingPolicyResponse_t*
*/
adb_getBindingPolicyResponse_t* axis2_skel_ServiceAdmin_getBindingPolicy(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																		 adb_getBindingPolicy_t* _getBindingPolicy )
{
	/* TODO fill this with the necessary business logic */
	return (adb_getBindingPolicyResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "getServiceBindings|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _getServiceBindings of the adb_getServiceBindings_t*
*
* @return adb_getServiceBindingsResponse_t*
*/
adb_getServiceBindingsResponse_t* axis2_skel_ServiceAdmin_getServiceBindings(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																			 adb_getServiceBindings_t* _getServiceBindings,
																			 axis2_skel_ServiceAdmin_getServiceBindings_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return (adb_getServiceBindingsResponse_t*)NULL;
}



/**
* auto generated function definition signature
* for "stopService|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _stopService of the adb_stopService_t*
*
* @return 
*/
axis2_status_t  axis2_skel_ServiceAdmin_stopService(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
													adb_stopService_t* _stopService,
													axis2_skel_ServiceAdmin_stopService_fault *fault )
{
	/* TODO fill this with the necessary business logic */
	return AXIS2_SUCCESS;
}



/**
* auto generated function definition signature
* for "getOperationPolicy|http://mgt.service.carbon.wso2.org" operation.
* @param env environment ( mandatory)* @param MessageContext the outmessage context
* @param _getOperationPolicy of the adb_getOperationPolicy_t*
*
* @return adb_getOperationPolicyResponse_t*
*/
adb_getOperationPolicyResponse_t* axis2_skel_ServiceAdmin_getOperationPolicy(const axutil_env_t *env , axis2_msg_ctx_t *msg_ctx,
																			 adb_getOperationPolicy_t* _getOperationPolicy )
{
	/* TODO fill this with the necessary business logic */
	return (adb_getOperationPolicyResponse_t*)NULL;
}


static axis2_char_t *axis2_skel_ServiceAdmin_get_service_type(
    const axutil_env_t *env,
    axis2_svc_t *svc)
{
    axutil_param_t* param = NULL;

    param = axis2_svc_get_param(svc, env, AXIS2_SERVICE_TYPE);
    if(!param)
    {
        /* Send default service type */
        return AXIS2_DEFAULT_SERVICE_TYPE;
    }

    return (axis2_char_t*)axutil_param_get_value(param, env);
}

