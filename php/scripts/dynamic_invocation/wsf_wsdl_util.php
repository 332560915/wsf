<?php
/*
 * Copyright (c) 2005-2008 WSO2, Inc. http://wso2.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/**
 * This function is called from call_user_function in C level.
 * Once this function is called, it will fill in the WSDL information,
 * such as payload, service endpoint, SOAP version, policies etc,
 * into an array and return that array.
 * @param array $user_parameters the details of WSDL endpoint, service address
 * and class map
 * @param array $function_parameters details of the invoked function
 * @return array $return_value array of details to be passed to C level
 */

function wsf_is_mutiple_port_types($wsdl_dom) {
    if($wsdl_dom) {
        $child_list = $wsdl_dom->childNodes;
        foreach($child_list as $child) {
            if ($child->nodeType != XML_ELEMENT_NODE) {
                continue;
            }
            if ($child->localName == WSF_DESCRIPTION) {
                return FALSE;
            }
            else if ($child->localName == WSF_DEFINITION) {
                $wsdl_11_child_list = $child->childNodes;
                $i = 0;
                foreach($wsdl_11_child_list as $wsdl_11_child) {
                    if ($child->nodeType != XML_ELEMENT_NODE) {
                        continue;
                    }
                    if ($wsdl_11_child->localName == WSF_WSDL_PORT_TYPE) {
                        $i++;
                        if($i > 1) {
                            return TRUE;
                        }
                    }
                }
            }
        }
    }
    return FALSE;
}

function wsf_process_multiple_interfaces($wsdl_dom) {
    $wsdl_2_0_child_list = $wsdl_dom->firstChild->childNodes;

    /* to store the list of interfaces */
    $interface_array = array();
    $i = 1 ;

    foreach($wsdl_2_0_child_list as $interface_child) {
        if($interface_child->nodeType != XML_ELEMENT_NODE) {
            continue;
        }
        if($interface_child->localName == 'interface') {
            $interface_array[$i] = $interface_child->attributes->getNamedItem('name')->value;
            $i++;
        }
    }
    $sig_service_array = array();
    $no_of_interfaces = count($interface_array);


    $wsdl_dom2 = new DomDocument();
    $wsdl_dom2->preserveWhiteSpace = false;
    /* copy the wsdl_dom */
    $wsdl_dom2->loadXML($wsdl_dom->saveXML());

    $wsdl_2_0_child_list2 = $wsdl_dom2->firstChild->childNodes;

    for($j = 1 ; $j <= $no_of_interfaces; $j++) {
        $wsdl_2_0_child_list1 = $wsdl_dom2->firstChild->childNodes;
        foreach($wsdl_2_0_child_list1 as $service_child) {
            if($service_child->nodeType != XML_ELEMENT_NODE) {
                continue;
            }
            if($service_child->localName == 'service') {
                $old_attr = $service_child->getAttribute('interface');
                $service_child->removeAttribute($old_attr);
                $service_child->setAttribute('interface', "tns:".$interface_array[$j]);
            }
        }

        $tmp_sig_model = wsf_get_sig_model_dom($wsdl_dom2);

        $services_node = $tmp_sig_model->firstChild;
        $service_child_list = $services_node->childNodes;
        foreach($service_child_list as $service_child) {
            if($service_child->nodeType != XML_ELEMENT_NODE) {
                continue;
            }
            if($service_child->localName == 'service' && $service_child->hasAttributes()) {
                $service_endpoint = $service_child->attributes->getNamedItem('endpoint')->value;
                $operations_child_list = $service_child->childNodes;

                foreach($operations_child_list as $operations_child) {
                    if($operations_child->nodeType != XML_ELEMENT_NODE) {
                        continue;
                    }
                    if($operations_child->localName == 'operations') {
                        $operations_name = $operations_child->attributes->getNamedItem('name')->value;
                        if(strstr($service_endpoint, $operations_name)) {
                            $sig_service_array[strstr($service_endpoint, $operations_name)] = $service_child;
                        }
                    }
                }
            }
        }
    }
    $created_sig_dom = new DOMDocument('1.0', 'iso-8859-1');
    $element = $created_sig_dom->createElement('services');
    $created_sig_dom->appendChild($element);
    foreach($sig_service_array as $value) {
        wsf_schema_appendNode($element, $value, $created_sig_dom);
    }

    return $created_sig_dom;
}

/**
 * Create WSDL DomDocument from given WSDL location.
 * If given WSDL is WSDL1.1 then it would be coverted to a WSDL2.0
 * @param string $wsdl_location
 * @return DomDocument $wsdl_dom DomDocument of WSDL2.0
 */
function wsf_get_wsdl_dom($wsdl_dom) {
    require_once('wsf_wsdl_consts.php');

    $xslt_wsdl_20_dom = new DOMDocument();
    $xslt_11_to_20_dom = new DOMDocument();

    $xslt_11_to_20_dom->preserveWhiteSpace = false;
    $xslt = new XSLTProcessor();
    global $wsdl_11_dom, $is_wsdl_11;
     
    if($wsdl_dom) {
        $child_list = $wsdl_dom->childNodes;
        foreach($child_list as $child) {
            if($child->nodeType != XML_ELEMENT_NODE) {
                continue;
            }
            if($child->localName == WSF_DEFINITION) {
                /* first element local name is definitions, so this is a
                 version 1.1 WSDL */
                $xslt_str = file_get_contents(WSF_WSDL1TO2_XSL_LOCATION, TRUE);
                if(!($xslt_wsdl_20_dom->loadXML($xslt_str)))
                  return "WSDL 1.1 to 2.0 converting stylesheet not found";
                
                $xslt->importStyleSheet($xslt_wsdl_20_dom);
                $xslt_11_to_20_dom->loadXML($xslt->transformToXML($wsdl_dom));
                $is_wsdl_11 = TRUE;
                $wsdl_11_dom = $wsdl_dom;
                return $xslt_11_to_20_dom;
            }
            else if ($child->localName == WSF_DESCRIPTION) {
                /* first element local name is description, so this is a
                  version 2.0 WSDL */
                return $wsdl_dom;
            }
            else{
                /* echo "Not a valid WSDL"; */
                return NULL;
            }
        }
    }
    else
    {
        return NULL;
    }
}


/**
 * Creates the Sig model DomDocument from WSDL2.0 DomDocument.
 * @param DomDocument $wsdl_dom WSDL2.0 DomDocument
 * @return DomDocument Sig model DomDocument
 */

function wsf_get_sig_model_dom(DomDocument $wsdl_dom) {
    require_once('wsf_wsdl_consts.php');

    $xslt_dom  = new DOMDocument();
    $xsl = new XSLTProcessor();
    
    $xslt_str = file_get_contents(WSF_SIG_XSL_LOCATION, TRUE);
    
    if($xslt_str && $xslt_dom->loadXML($xslt_str)) {
        $xsl->importStyleSheet($xslt_dom);
        return $xsl->transformToDoc($wsdl_dom);
    }
    else {
        return NULL;
    }
}

/**
 * Returns the endpoint address from Sig model DOM.
 * @param $DomDocument $sig_model_dom Sig model DomDocument
 * @return string endpoint address value
 */

function wsf_get_endpoint_address(DomDocument $sig_model_dom) {
    require_once('wsf_wsdl_consts.php');

    $services_node = $sig_model_dom->firstChild;
    $service_node = $services_node->firstChild;
    $service_attr = $service_node->attributes;
    return $service_attr->getNamedItem(WSF_ADDRESS)->value;
}

function wsf_is_multiple_endpoints(DomDocument $sig_model_dom) {
    $services_node = $sig_model_dom->firstChild;
    $no_of_ep = $services_node->childNodes->length;
    if($no_of_ep == 1)
        return FALSE;
    else
        return TRUE;
}

/**
 * Finds the operation DomNode.
 * @param DomDocument $sig_model_dom 
 * @param string $operation_name name of the param to be invoked
 * @param string $endpoint_address service endpoint address
 * @return DomNode operation DomNode of the Sig model
 */
function wsf_find_operation(DomDocument $sig_model_dom, $operation_name, $service_name, $port_name, $is_multiple) {
    require_once('wsf_wsdl_consts.php');
    require_once('wsf_wsdl_util.php');

    if ($is_multiple == FALSE) {
        $operation = NULL;
        $services_node = $sig_model_dom->firstChild;
        $services_childs_list = $services_node->childNodes;

        foreach($services_childs_list as $child) {
            if($child->tagName == WSF_SERVICE) {
                if($service_name == NULL) {
                    /* pick by port if the service name is not given*/
                    if($port_name == NULL) {
                        /* now just get the first service and port*/
                        $service_node = $child;
                        break;
                    }
                    /* now pick by the port name */
                    if($child->attributes->getNamedItem("endpoint")) {
                        $derived_port_name = $child->attributes->getNamedItem("endpoint")->value;
                    }
                    if($derived_port_name == $port_name) {
                        $service_node = $child;
                        break;
                    }
                }
                $child_service_name = NULL;
                if($child->attributes->getNamedItem(WSF_NAME)) {
                    $child_service_name = $child->attributes->getNamedItem(WSF_NAME)->value;
                }
                if($service_name == $child_service_name) {
                    /* pick by port if the service name is found*/
                    if($port_name == NULL) {
                        /* now just get the first service and port*/
                        $service_node = $child;
                        break;
                    }
                    /* now pick by the port name */
                    if($child->attributes->getNamedItem("endpoint")) {
                        $derived_port_name = $child->attributes->getNamedItem("endpoint")->value;
                    }
                    if($derived_port_name == $port_name) {
                        $service_node = $child;
                        break;
                    }
                }
            }
        }
        if(!$service_node) {
            error_log("service node not found");
            return NULL;
        }

        
        $service_child_list = $service_node->childNodes;
        /* search the operations element of the sig */
        foreach($service_child_list as $service_child) {
            if($service_child->tagName == WSF_OPERATIONS) {
                $operations_node = $service_child;
                break;
            }
        }
        
        /* search the operation element of sig */
        if($operations_node) {
            foreach($operations_node->childNodes as $operations_child) {
                if($operations_child->nodeType == XML_ELEMENT_NODE && $operations_child->tagName == WSF_OPERATION) {
                    $operation_node = $operations_child->attributes;
                    if($operation_node->getNamedItem(WSF_NAME) && $operation_node->getNamedItem(WSF_NAME)->value == $operation_name) {
                        $operation = $operations_child;
                        return $operation;
                    }
                }
            }
        }
    }
    else{
        $operation = NULL;
        $services_node = $sig_model_dom->firstChild;
        $services_childs_list = $services_node->childNodes;

        foreach($services_childs_list as $child) {
            if($child->tagName == WSF_SERVICE && $child->attributes->getNamedItem(WSF_ADDRESS) &&
                    $child->attributes->getNamedItem(WSF_ADDRESS)->value == $endpoint_address) {
                $service_node = $child;
                if(!$service_node)
                    return NULL;
                $service_child_list = $service_node->childNodes;
                /* search the operations element of the sig */
                foreach($service_child_list as $service_child) {
                    if($service_child->tagName == WSF_OPERATIONS) {
                        $operations_node = $service_child;
                        if($operations_node) {
                            foreach($operations_node->childNodes as $operations_child) {
                                if($operations_child->tagName == WSF_OPERATION) {
                                    $operation_node = $operations_child->attributes;
                                    if($operation_node->getNamedItem(WSF_NAME) && 
                                            $operation_node->getNamedItem(WSF_NAME)->value == $operation_name) {
                                        $operation = $operations_child;
                                        return $operation;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return $operation;
    }
}



/**
 * Returns the binding node corresponding to the given endpoint address.
 * @param DomDocument $wsdl_dom WSDL2.0 DomDocument
 * @param string $service_name, service name.
 * @param string $port_name, port name for wsdl1.1 / endpoint name for wsdl 2.
 *      current we use service_name + port_name 
 *      combination to figure out the binding
 * @param Bool $$is_wsdl_11 true is WSDL version 1.1, else false
 * @return DomNode binding DomNode
 */
function wsf_get_binding(DomDocument $wsdl_dom, $service_name, $port_name, $is_wsdl_11 = FALSE) {
    require_once('wsf_wsdl_consts.php');

    if($is_wsdl_11 == FALSE) {
        $root_node = $wsdl_dom->firstChild;
        $root_child_list = $root_node->childNodes;
         
        foreach($root_child_list as $childs) {
            if($childs->nodeType != XML_ELEMENT_NODE) {
                continue;
            }
            if($childs->localName == WSF_SERVICE && $childs->namespaceURI == WSF_WSDL2_NAMESPACE) {
                if($service_name == NULL) {
                    /* pick the first node as service if the service name is not given*/
                    $service_node = $childs;
                    break;
                }
                $child_service_name = NULL;
                if($childs->attributes->getNamedItem(WSF_NAME)) {
                    $child_service_name = $childs->attributes->getNamedItem(WSF_NAME)->value;
                }
                if($service_name == $child_service_name) {
                    $service_node = $childs;
                    break;
                }
            }
        }
         
        if(!$service_node) {
            error_log("Service node not found, please check service name, endpoint name");
            /* echo "serivice node not found"; */
            return;
        }
         
        $service_child_list = $service_node->childNodes;
        foreach($service_child_list as $service_childs) {
            if($service_childs->nodeType != XML_ELEMENT_NODE) {
                continue;
            }
            if($service_childs->localName == WSF_ENDPOINT && $service_childs->namespaceURI == WSF_WSDL2_NAMESPACE) {
                if($port_name == NULL) {
                    $endpoint_node = $service_childs;
                    break;
                }
                $child_endpoint_name = NULL;
                if($service_childs->attributes->getNamedItem(WSF_NAME)) {
                    $child_endpoint_name = $service_childs->attributes->getNamedItem(WSF_NAME)->value;
                }
                if($port_name == $child_endpoint_name) {
                    $endpoint_node = $service_childs;
                    break;
                }
            }
        }
        if($endpoint_node != NULL) {
            $binding_name = $endpoint_node->attributes->getNamedItem(WSF_BINDING)->value;
        }
        if(!$binding_name) {
            error_log("Binding node not found, please check service name, endpoint name");
            return NULL;
        }
         
        foreach($root_child_list as $childs) {
            if($childs->localName == WSF_BINDING && $childs->namespaceURI == WSF_WSDL2_NAMESPACE) {
                $binding_name_mod = strstr($binding_name, ":");
                if($childs->attributes->getNamedItem(WSF_NAME)->value == substr($binding_name_mod, 1))
                return $childs;
            }
        }
         
        return NULL;
    }
    else{
        $root_node = $wsdl_dom->documentElement;
        $root_child_list = $root_node->childNodes;


        foreach($root_child_list as $childs) {
            /* ignoreing comments and all the unrelated child nodes */
            if($childs->nodeType != XML_ELEMENT_NODE) {
                continue;
            }
            if($childs->localName == WSF_SERVICE && $childs->namespaceURI == WSF_WSDL_NAMESPACE) {
                if($service_name == NULL) {
                    /* pick the first node as service if the service name is not given*/
                    $service_node = $childs;
                    break;
                }
                $child_service_name = NULL;
                if($childs->attributes->getNamedItem(WSF_NAME)) {
                    $child_service_name = $childs->attributes->getNamedItem(WSF_NAME)->value;
                }
                if($service_name == $child_service_name) {
                    $service_node = $childs;
                    break;
                }
            }
        }

        if(!$service_node) {
            error_log("Service node not found, please check service name, endpoint name");
            /* echo "service node not found"; */
            return;
        }

        $service_child_list = $service_node->childNodes;
        $port_node = NULL;

        foreach($service_child_list as $service_childs) {
            /* ignoreing comments and all the unrelated child nodes */
            if($service_childs->nodeType != XML_ELEMENT_NODE) {
                continue;
            }
            if($service_childs->localName == WSF_PORT && $service_childs->namespaceURI == WSF_WSDL_NAMESPACE) {
                if($port_name == NULL) {
                    /* pick the first node as port if the port name is not given*/
                    $port_node = $service_childs;
                    break;
                }
                $child_port_name = NULL;
                if($service_childs->attributes->getNamedItem(WSF_NAME)) {
                    $child_port_name = $service_childs->attributes->getNamedItem(WSF_NAME)->value;
                }
                if($port_name == $child_port_name) {
                    $port_node = $service_childs;
                    break;
                }
            }
        }
        if($port_node != NULL) {
            $binding_name = $port_node->attributes->getNamedItem(WSF_BINDING)->value;
        }

        if(!$binding_name) {
            /* echo "binding_name not found"; */
            error_log("Binding node not found, please check service name, port name");
            return NULL;
        }

        foreach($root_child_list as $childs) {
            if($childs->nodeType != XML_ELEMENT_NODE) {
                continue;
            }
            if($childs->localName == WSF_BINDING && $childs->namespaceURI == WSF_WSDL_NAMESPACE) {
                $binding_name_mod = strstr($binding_name, ":");
                if($childs->attributes->getNamedItem(WSF_NAME)->value == substr($binding_name_mod, 1))
                    return $childs;
            }
        }
        return NULL;
    }
}

/**
 * Returns the array of policies associated with the given operation and binding.
 * @param DomDocument $wsdl_dom WSDl2.0 DomDocument
 * @param DomNode $binding_node
 * @param string $operation_name Name of the operation
 * @param Bool $$is_wsdl_11 true is WSDL version 1.1, else false
 * @return array of policies
 */
function wsf_get_all_policies(DomDocument $wsdl_dom, DomNode $binding_node, $operation_name, $is_wsdl_11 = FALSE) {
    require_once('wsf_wsdl_consts.php');

    $policy_array = array();
    if(!$binding_node)
    return NULL;

    $binding_child_list = $binding_node->childNodes;
    foreach($binding_child_list as $binding_child) {
        if($binding_child->localName == WSF_POLICY_REFERENCE) {
            $binding_attr = $binding_child->attributes;
            $policy_uri =  $binding_attr->getNamedItem(WSF_URI)->value;
            $binding_policy = get_policy_node($wsdl_dom, $policy_uri);
            $policy_array["operation_policy"] = $binding_policy;
        }

        if($binding_child->localName == WSF_POLICY) {
            $binding_policy_child = $binding_child->firstChild;
            $binding_attr = $binding_policy_child->attributes;
            $policy_uri =  $binding_attr->getNamedItem(WSF_URI)->value;
            $binding_policy = get_policy_node($wsdl_dom, $policy_uri);
            $policy_array["operation_policy"] = $binding_policy;
        }

        if($binding_child->localName == WSF_OPERATION && $is_wsdl_11 = FALSE) {
            $operation_attr = $binding_child->attributes;
            $operation_ref = $operation_attr->getNamedItem(WSF_REF)->value;
            if(substr(strstr($operation_ref, ":"), 1) == $operation_name && $binding_child->hasChildNodes()) {
                foreach($binding_child->childNodes as $input_output) {
                    if($input_output->firstChild->localName == WSF_POLICY_REFERENCE) {  /* there may be several chidren */
                        $input_output_attr = $input_output->firstChild->attributes;
                        $msg_policy_uri =  $input_output_attr->getNamedItem(WSF_URI)->value;
                        $msg_policy = get_policy_node($wsdl_dom, $msg_policy_uri);
                        $key = $input_output->localName."_policy";
                        $policy_array[$key] = $msg_policy;
                    }
                }
            }
        }

        if($binding_child->localName == WSF_OPERATION && $is_wsdl_11 = TRUE) {
            $op_name = NULL;
            $operation_attr = $binding_child->attributes;
            if($operation_attr->getNamedItem(WSF_NAME))
            $op_name = $operation_attr->getNamedItem(WSF_NAME)->value;
            if($op_name == $operation_name && $binding_child->hasChildNodes()) {
                foreach($binding_child->childNodes as $input_output) {
                    if($input_output->firstChild && $input_output->firstChild->localName == WSF_POLICY_REFERENCE) {  /* there may be several children */
                        $input_output_attr = $input_output->firstChild->attributes;
                        $msg_policy_uri =  $input_output_attr->getNamedItem(WSF_URI)->value;
                        $msg_policy = get_policy_node($wsdl_dom, $msg_policy_uri);
                        $key = $input_output->localName."_policy";
                        $policy_array[$key] = $msg_policy;
                    }
                }
            }
        }
    }
    return $policy_array;
}

/**
 * Returns policy node corresponding to given policy URI.
 * @param DomDocument $wsdl_dom WSDL2.0 DomDocument
 * @param string $policy_uri policy location defined in WSDL
 * @return string policy xml string
 */

function get_policy_node(DomDocument $wsdl_dom, $policy_uri) {
    require_once('wsf_wsdl_consts.php');

    $root_node = $wsdl_dom->firstChild;
    $root_child_list = $root_node->childNodes;
    foreach($root_child_list as $child) {
        if($child->localName == WSF_POLICY) {
            $policy_attr = $child->attributes;
            $policy_in_same_wsdl  = strpos($policy_uri, '#');
            if($policy_in_same_wsdl === 0) {
                if($policy_attr->getNamedItemNS(WSF_POLICY_ID_NAMESPACE_URI, WSF_ID)->value == substr($policy_uri, 1)) {
                    $policy_node = $child;
                    break;
                }
            } else {/* TODO- get policy from another WSDL */
            /* printf("Policy is not found in the given WSDL"); */
            return;
            }
        }
    }


    if($policy_node) {
        $clone_policy_node = $policy_node->cloneNode(TRUE);
        return $wsdl_dom->saveXML($clone_policy_node);
    } else
    return NULL;

}


/**
 * Returns the binding details as SOAP action, WSA action, etc.
 * @param DomNode $operation_node
 * @return array binding-details array
 */
function wsf_get_binding_details(DomNode $operation_node) {
    require_once('wsf_wsdl_consts.php');

    $binding_array = array();
    $soap_version = 2;
    $wsa_action = NULL;
    $soap_action = NULL;
    $http_method = NULL;

    $operation_child_list = $operation_node->childNodes;
    foreach($operation_child_list as $operation_child) {
        if($operation_child->localName == WSF_BINDINDG_DETAILS && $operation_child->hasAttributes()) {
            $binding_array[WSF_USE_SOAP] = TRUE;
            if($operation_child->attributes->getNamedItem(WSF_WSAWAACTION))
                $wsa_action = $operation_child->attributes->getNamedItem(WSF_WSAWAACTION)->value;
            if($operation_child->attributes->getNamedItem(WSF_SOAPACTION))
                $soap_action = $operation_child->attributes->getNamedItem(WSF_SOAPACTION)->value;

            if($operation_child->attributes->getNamedItem(WSF_HTTPMETHOD)) {
                $http_method = $operation_child->attributes->getNamedItem(WSF_HTTPMETHOD)->value;
            }
            if($wsa_action) {
                $binding_array[WSF_WSA] = $wsa_action;
            }
            if($soap_action) {
                $binding_array[WSF_SOAP] = $soap_action;
            }
            if($http_method) {
                $binding_array[WSF_USE_SOAP] = FALSE;
                $binding_array[WSF_HTTPMETHOD] = $http_method;
            }
        }
    }

    if($binding_array[WSF_USE_SOAP]) {
        /* goto operations parent get soap version information*/
        $operations_node = $operation_node->parentNode;
        if ($operations_node) {
            $service_node = $operations_node->parentNode;
        }
        if ($service_node) {
            $soap_type = $service_node->attributes->getNamedItem(WSF_TYPE)->value;
            if($soap_type == WSF_SOAP11) {
                $soap_version = 1;
            }
            else if($soap_type == WSF_SOAP12) {
                $soap_version = 2;
            }
            else{
                $soap_version = 2;
            }
            $binding_array[WSF_SOAP_VERSION] = $soap_version;
        }
    }


    return $binding_array;
}

/**
 * Returns schema node
 * @param DomDocument $wsdl_dom WSDL2.0 DomDocument
 * @return DomNode $schema_node Cloned schema node
 */
function wsf_get_schema_node(&$wsdl_dom, &$wsdl_dom2 = NULL) {
    require_once('wsf_wsdl_consts.php');


    static $schema_node = NULL;

    if ($schema_node != NULL)
    return $schema_node; // Asume it is a single WSDL
     
    $root_node = $wsdl_dom->firstChild;
    $root_child_list = $root_node->childNodes;
    foreach($root_child_list as $childs) {
        /* this is for inline schema */
        $tmp_node = $childs;
        $schema_node = $tmp_node->cloneNode(TRUE);
        /* end inline schema */

        if($childs->localName == WSF_TYPES) {
            $schema_list = $childs->childNodes;
            // to find import schemas
            foreach($schema_list as $schema_child) {
                $schema = array();
                $i = 0;
                $import_child_list = $schema_child->childNodes;
                foreach($import_child_list as $import_child) {
                    if($import_child->localName == "import" && $import_child->attributes->getNamedItem('schemaLocation')) {
                        $schema["schema".$i]= $import_child->attributes->getNamedItem('schemaLocation')->value;
                        $i++;
                    }
                }
            }

            if($schema) {
                $schema_dom = new DomDocument();
                $schema_dom->preserveWhiteSpace = false;
                foreach($schema as $key => $val) {
                    $schema_dom->load($val);
                    $import_schema_child_list = $schema_dom->childNodes;
                    foreach($import_schema_child_list as $import_schema_child) {
                        if($import_schema_child->localName == 'schema') {
                            $tmp_import_schema_node = $import_schema_child;
                            $cloned_import_schema_node = $tmp_import_schema_node->cloneNode(TRUE);
                            wsf_schema_appendNode($schema_node, $tmp_import_schema_node, $wsdl_dom);
                        }
                    }
                }
            }
            $wsdl_2_0_child_list2 = $wsdl_dom->firstChild->childNodes;
            foreach($wsdl_2_0_child_list2 as $types_child) {
                if($types_child->localName == 'types') {
                    $types_child_list = $types_child->childNodes;
                    foreach($types_child_list as $schema_child) {
                        if($schema_child->localName == 'schema') {
                            $schema_child_list = $schema_child->childNodes;
                            for ($i = $schema_child_list->length; $i >= 0; $i--) {
                                $import_child = $schema_child_list->item($i);
                                if($import_child && $import_child->localName == 'import' && $import_child->attributes->getNamedItem('schemaLocation')) {
                                    $schema_child->removeChild($import_child);
                                }
                            }
                        }
                    }
                }
            }

            if($wsdl_dom2) {
                $wsdl_2_0_child_list = $wsdl_dom2->firstChild->childNodes;
                foreach($wsdl_2_0_child_list as $types_child) {
                    if($types_child->localName == 'types') {
                        $types_child_list = $types_child->childNodes;
                        foreach($types_child_list as $schema_child) {
                            if($schema_child->localName == 'schema') {
                                $types_child->removeChild($schema_child);
                                foreach($schema_node->childNodes as $schema3_child)
                                wsf_schema_appendNode($types_child, $schema3_child, $wsdl_dom2);

                            }
                        }

                    }
                }
            }

            if($wsdl_dom2) {
                $wsdl_2_0_child_list = $wsdl_dom2->firstChild->childNodes;
                foreach($wsdl_2_0_child_list as $types_child) {
                    if($types_child->localName == 'types') {
                        $types_child_list = $types_child->childNodes;
                        foreach($types_child_list as $schema_child) {
                            if($schema_child->localName == 'schema') {
                                $schema_child_list = $schema_child->childNodes;
                                for ($i = $schema_child_list->length; $i >= 0; $i--) {
                                    //foreach($schema_child_list as $import_child) {
                                    $import_child = $schema_child_list->item($i);
                                    if($import_child->localName == 'import' && $import_child->attributes->getNamedItem('schemaLocation')) {
                                        $schema_child->removeChild($import_child);

                                    }
                                }
                            }
                        }
                    }
                }
            }
            return $schema_node;
        }
    }

}


/**
 * Recursive function to create schema from import schema
 * @param DomNode $parent parent dom node
 * @param DomNode $child dom node of import schema
 * @param DomDocument $doc DomDocument of parent DomNode
 */

function wsf_schema_appendNode( $parent, $child, $doc ) {
    if( $child == NULL) {
        return;
    }
    $newChild = NULL;
    if( $child-> nodeType == XML_TEXT_NODE ) {
        $newChild = $doc-> createTextNode($child->nodeValue);
        //echo $newChild->nodeValue."\n";
    }
    else if( $child-> nodeType == XML_ELEMENT_NODE) {
        $childTag = $child->tagName;

        $newChild = $doc-> createElementNS($child->namespaceURI, $childTag);

        foreach( $child->attributes as $attribute) {
            $newChild->setAttribute($attribute->name, $attribute->value);
        }

        foreach ( $child->childNodes as $childsChild) {
            wsf_schema_appendNode( $newChild, $childsChild, $doc);
        }
    }
    if( $newChild != NULL) {
        $parent-> appendChild( $newChild);
    }
}



function is_xsd_type($param_type) {

    $xsd_array = array("string", "boolean", "double", "boolean", "double",
                          "float", "int", "integer", "byte", "decimal", 
                          "base64Binary", "hexBinary", "any", "QName", "dateTime", 
                          "date", "time", "unsignedLong", "unsignedInt", "unsignedShort",
                          "unsignedByte" , "positiveInteger", "negativeInteger", "nonNegativeInteger",
                          "nonPositiveInteger", "gYearMonth", "gMonthDay", "gYear", "gMonth", 
                          "gDay", "duration", "Name", "NCName", "NMTOKEN", "NOTATION", "NMTOKENS", "ENTITY",
                          "ENTITIES", "IDREF", "IDREFS", "anyURI", "language", "ID", "normalizedString", "token", "short", "long");

    return in_array($param_type, $xsd_array);
}

function wsf_wsdl_util_xsd_to_php_type_map() {
    static $map = array(
        "string"         =>  "string",
        "boolean"        =>  "boolean",
        "double"         =>  "float",
        "float"          =>  "float",
        "int"            =>  "integer",
        "short"          =>  "integer",
        "byte"           =>  "integer",
        "long"           =>  "interger",
        "decimal"        =>  "float",
        "interger"       =>  "integer",
        "base64Binary"   =>  "string",
        "hexBinary"      =>  "string",
        "anyType"        =>  "mixed",
        "QName"          =>  "string",
        "dateTime"       =>  "string",
        "date"           =>  "string",
        "time"           =>  "string",
        "unsignedLong"   =>  "interger",
        "unsignedInt"    =>  "integer",
        "unsignedShort"  =>  "integer",
        "unsignedByte"   =>  "integer",
        "positiveInteger"=>  "integer",
        "negativeInteger"=>  "integer",
        "nonNegativeInteger"=>"integer",
        "gYearMonth"     =>  "string",
        "gMonthDate"     =>  "string",
        "gYear"          =>  "string",
        "gMonth"         =>  "string",
        "gDay"           =>  "string",
        "duration"       =>  "string",
        "Name"           =>  "string",
        "NCName"         =>  "string",
        "NMTOKEN"        =>  "string",
        "NOTATION"       =>  "string",
        "NMTOKENS"       =>  "string",
        "ENTITY"         =>  "string",
        "ENTITIES"       =>  "string",
        "IDREF"          =>  "string",
        "IDREFS"         =>  "string",
        "anyURI"         =>  "string",
        "language"       =>  "string",
        "normalizedString"=> "string",
        "token"          => "string");

    return $map;
}

function wsf_wsdl_util_serialize_php_value($xsd_type, $data_value) {
    $xsd_php_mapping_table = wsf_wsdl_util_xsd_to_php_type_map();
    $serialized_value = $data_value;

    ws_log_write(__FILE__, __LINE__, WSF_LOG_DEBUG, "serializing ".$data_value);
    
    if(array_key_exists($xsd_type, $xsd_php_mapping_table)) {
        $type = $xsd_php_mapping_table[$xsd_type];

        if($type == "boolean") {
            if($data_value == FALSE) {
                $serialized_value = "false";
            }
            else
            {
                $serialized_value = "true";
            }
        }
    }

    if($serialized_value === NULL) return "";
    return $serialized_value."";
}


function wsf_wsdl_util_convert_value($xsd_type, $data_value) {
    $xsd_php_mapping_table = wsf_wsdl_util_xsd_to_php_type_map();

    ws_log_write(__FILE__, __LINE__, WSF_LOG_DEBUG, "deserializing ".$data_value);

    $converted_value = $data_value;
    if(array_key_exists($xsd_type, $xsd_php_mapping_table)) {
        $type = $xsd_php_mapping_table[$xsd_type];
        if($type == 'integer') {
            $converted_value = (int)($data_value);
        }
        else if ($type == 'float') {
            $converted_value = (float)($data_value);
        }
        else if ($type == 'boolean') {
            $converted_value = ($data_value === "true");
        }
        else if ($type == 'string') {
            $converted_value = $data_value;
        }
        else {
            $converted_value = $data_value;
        }
    }
    
    ws_log_write(__FILE__, __LINE__, WSF_LOG_DEBUG, "deserialized to ".$converted_value);

    return $converted_value;
}

function wsf_is_rpc_enc_wsdl($wsdl_11_dom, $binding_node, $operation_name) {
    if(!$binding_node)
        return FALSE;

    $binding_child_list = $binding_node->childNodes;
    $style = NULL;
    foreach($binding_child_list as $binding_child) {
        if($binding_child->localName == "binding") {
            $binding_attr = $binding_child->attributes;
            if($binding_attr && $binding_attr->getNamedItem("style")) {
                $style =  $binding_attr->getNamedItem("style")->value;
            }
            //var_dump($style)."\n\n";
        }
        if($binding_child->localName == WSF_OPERATION && $style == "rpc") {
            $op_name = NULL;
            $operation_attr = $binding_child->attributes;
            if($operation_attr->getNamedItem(WSF_NAME))
                $op_name = $operation_attr->getNamedItem(WSF_NAME)->value;
            if($op_name == $operation_name && $binding_child->hasChildNodes()) {
                foreach($binding_child->childNodes as $input_output) {
                    if($input_output->firstChild && $input_output->firstChild->localName == "body") {
                        $body_attr = $input_output->firstChild->attributes;
                        $enc =  $body_attr->getNamedItem("use")->value;
                        $enc_style = $body_attr->getNamedItem("encodingStyle")->value;
                        if($enc =="encoded")
                            return TRUE;
                        else
                            return FALSE;
                    }
                }
            }
        }    
    }

    return FALSE;
}


/**
 * @param node to start with
 * @return the node with comments skiped
 */
function wsf_remove_next_whitespace_and_comments(&$xml) {
    while($xml != NULL && $xml->nodeType != XML_ELEMENT_NODE) {
        $xml = $xml->nextSibling;
    }
    return $xml;
}

/**
 * Easy serializing function
 * @param xml DOMDocuemnt the xml to serialize
 * @return string of serialized xml
 */
function wsf_test_serialize_node($xml) {
    $nodeText = "";

    if ($xml->nodeType == XML_TEXT_NODE) { //for text node
        $nodeText = $xml->nodeValue;
    }
    else if ( $xml->nodeType == XML_ELEMENT_NODE) // for element node
    {
        $nodeName = $xml->nodeName;

        $nodeText = "<" . $nodeName;

        // add attributes
        foreach( $xml->attributes as $attri) {
            $nodeText .= " " . $attri->name . "=" . "\"" . $attri->value . "\" ";
        }

        $nodeText .= ">";

        // add childs

        foreach ($xml->childNodes as $child) {
            $childText = wsf_test_serialize_node($child);
            $nodeText .= $childText;
        }

        $nodeText .= "</" . $nodeName . ">";

    }
    return $nodeText;
}

/** 
 * Determine whether a given variable is a map or not
 * Note that is_array returns TRUE for both general arrays and map
 * This distinguises the arrays and map
 */
function wsf_is_map($map) {
    if(!is_array($map)) {
        return FALSE;
    }
    /* array keys shouldn't be number for pure match */
    $keys = array_keys($map);
    if($keys[0] === 0) {
        return FALSE;
    }
    return TRUE;
}

/**
 * WSFUnknownSchemaConstruct
 * The class to represent unknown schema constructs in the classmap mode
 */
class WSFUnknownSchemaConstruct
{
    public $properties;
    public function __construct() {
        $this->properties = array();
    }

    public function __get($name) {
        return $this->properties[$name];
    }

    public function __set($name, $value) {
        $this->properties[$name] = $value;
    }

    public function __isset($name) {
        if($this->properties[$name] == NULL) {
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }

    public function __unset($name) {
        if($this->properties[$name] != NULL) {
            $this->properties[$name] = NULL;
        }
    }
}

?>
