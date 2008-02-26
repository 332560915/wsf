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

function wsf_is_mutiple_port_types($wsdl_dom)
{
	if($wsdl_dom){
		$child_list = $wsdl_dom->childNodes;
		foreach($child_list as $child){
			if ($child->localName == WSF_DESCRIPTION)
			return FALSE;
			else if ($child->localName == WSF_DEFINITION){
				$wsdl_11_child_list = $child->childNodes;
				$i = 0;
				foreach($wsdl_11_child_list as $wsdl_11_child){
					if ($wsdl_11_child->localName == 'portType')
					$i++;
				}
				if($i > 1)
				return TRUE;
				return FALSE;
			}
		}
	}
}

function wsf_process_multiple_interfaces($wsdl_dom, $sig_model_dom, $xslt_location)
{

	$wsdl_2_0_child_list = $wsdl_dom->firstChild->childNodes;
	$interface_array = array();
	$i = 1 ;

	foreach($wsdl_2_0_child_list as $interface_child){
		if($interface_child->localName == 'interface'){
			$interface_array[$i] = $interface_child->attributes->getNamedItem('name')->value;
			$i++;
		}
	}
	$sig_service_array = array();
	$no_of_interfaces = count($interface_array);


	$wsdl_dom2 = new DomDocument();
	$wsdl_dom2->preserveWhiteSpace = false;
	$wsdl_dom2->loadXML($wsdl_dom->saveXML());

	$wsdl_2_0_child_list2 = $wsdl_dom2->firstChild->childNodes;

	for($j = 1 ; $j <= $no_of_interfaces; $j++){
		$wsdl_2_0_child_list1 = $wsdl_dom2->firstChild->childNodes;
		foreach($wsdl_2_0_child_list1 as $service_child){
			if($service_child->localName == 'service'){
				$old_attr = $service_child->getAttribute('interface');
				$service_child->removeAttribute($old_attr);
				$service_child->setAttribute('interface', "tns:".$interface_array[$j]);
			}
		}

		$tmp_sig_model = wsf_get_sig_model_dom($wsdl_dom2, $xslt_location);


		$services_node = $tmp_sig_model->firstChild;
		$service_child_list = $services_node->childNodes;
		foreach($service_child_list as $service_child){
			if($service_child->localName == 'service' && $service_child->hasAttributes()){
				$service_endpoint = $service_child->attributes->getNamedItem('endpoint')->value;
				$operations_child_list = $service_child->childNodes;

				foreach($operations_child_list as $operations_child){
					if($operations_child->localName == 'operations'){
						$operations_name = $operations_child->attributes->getNamedItem('name')->value;
						if(strstr($service_endpoint, $operations_name)){
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
	foreach($sig_service_array as $value){
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
function wsf_get_wsdl_dom($wsdl_dom, $xslt_location)
{
	require_once('wsf_wsdl_consts.php');
//	require_once('dynamic_invocation/xslt/wsdl11to20.xsl10.xsl');

	$xslt_wsdl_20_dom = new DOMDocument();
	$xslt_11_to_20_dom = new DOMDocument();
	$xslt_11_to_20_dom->preserveWhiteSpace = false;
	$xslt = new XSLTProcessor();
	global $wsdl_11_dom, $is_wsdl_11;
	 
	if($wsdl_dom){
		$child_list = $wsdl_dom->childNodes;
		foreach($child_list as $child){
		  if($child->nodeType == XML_COMMENT_NODE)
		    continue;
		  if($child->localName == WSF_DEFINITION){
		    /* first element local name is definitions, so this is a
		     version 1.1 WSDL */
		    $xslt_str = file_get_contents("dynamic_invocation/xslt/wsdl11to20.xsl10.xsl", TRUE);
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
	  return NULL;
}


/**
 * Creates the Sig model DomDocument from WSDL2.0 DomDocument.
 * @param DomDocument $wsdl_dom WSDL2.0 DomDocument
 * @return DomDocument Sig model DomDocument
 */

function wsf_get_sig_model_dom(DomDocument $wsdl_dom, $xslt_location)
{
	require_once('wsf_wsdl_consts.php');

	$xslt_dom  = new DOMDocument();
	$xsl = new XSLTProcessor();
	
	$xslt_str = file_get_contents("dynamic_invocation/xslt/wsdl2sig.xslt", TRUE);
	
	if($xslt_str && $xslt_dom->loadXML($xslt_str)){
		$xsl->importStyleSheet($xslt_dom);
		return $xsl->transformToDoc($wsdl_dom);
	}
	else
	return NULL;
}

/**
 * Returns the endpoint address from Sig model DOM.
 * @param $DomDocument $sig_model_dom Sig model DomDocument
 * @return string endpoint address value
 */

function wsf_get_endpoint_address(DomDocument $sig_model_dom)
{
	require_once('wsf_wsdl_consts.php');

	$services_node = $sig_model_dom->firstChild;
	$service_node = $services_node->firstChild;
	$service_attr = $service_node->attributes;
	return $service_attr->getNamedItem(WSF_ADDRESS)->value;
}

function wsf_is_multiple_endpoints(DomDocument $sig_model_dom)
{
	$services_node = $sig_model_dom->firstChild;
	$no_of_ep = $services_node->childNodes->length;
	if($no_of_ep == 1)
	return FALSE;
	else
	return TRUE;
}


/**
 * Returns the binding node corresponding to the given endpoint address.
 * @param DomDocument $wsdl_dom WSDL2.0 DomDocument
 * @param string $endpoint_address
 * @param Bool $$is_wsdl_11 true is WSDL version 1.1, else false
 * @return DomNode binding DomNode
 */
function wsf_get_binding(DomDocument $wsdl_dom, $endpoint_address, $is_wsdl_11 = FALSE)
{
	require_once('wsf_wsdl_consts.php');

	if($is_wsdl_11 == FALSE){
		$root_node = $wsdl_dom->firstChild;
		$root_child_list = $root_node->childNodes;
		 
		 
		foreach($root_child_list as $childs){
			if($childs->localName == WSF_SERVICE && $childs->namespaceURI == WSF_WSDL2_NAMESPACE){
				$service_node = $childs;
				break;
			}
		}
		 
		if(!$service_node){
		 /* echo "serivice node not found"; */
			return;
		}
		 
		$service_child_list = $service_node->childNodes;
		foreach($service_child_list as $service_childs){
			if($service_childs->localName == WSF_ENDPOINT && $service_childs->namespaceURI == WSF_WSDL2_NAMESPACE){
				if($service_childs->attributes->getNamedItem(WSF_ADDRESS)->value == $endpoint_address){
					$binding_name = $service_childs->attributes->getNamedItem(WSF_BINDING)->value;
					break;
				}
			}
		}
		 
		if(!$binding_name){
		 /* echo "binding_name not found"; */
			return;
		}
		 
		foreach($root_child_list as $childs){
			if($childs->localName == WSF_BINDING && $childs->namespaceURI == WSF_WSDL2_NAMESPACE){
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


		foreach($root_child_list as $childs){
		  
		  if($childs->localName == WSF_SERVICE && $childs->namespaceURI == WSF_WSDL_NAMESPACE){
		    $service_node = $childs;
		    break;
		  }
		}

		if(!$service_node){
			/* echo "service node not found"; */
			return;
		}

		$service_child_list = $service_node->childNodes;
		foreach($service_child_list as $service_childs){
			if($service_childs->localName == WSF_PORT && $service_childs->namespaceURI == WSF_WSDL_NAMESPACE){
				foreach($service_childs->childNodes as $port_child){
					if($port_child->localName == WSF_ADDRESS && $port_child->attributes->getNamedItem(WSF_LOCATION)->value == $endpoint_address){
						$binding_name = $port_child->parentNode->attributes->getNamedItem(WSF_BINDING)->value;
						break;
					}
				}

			}

		}

		if(!$binding_name){
			/* echo "binding_name not found"; */
			return;
		}

		foreach($root_child_list as $childs){
			if($childs->localName == WSF_BINDING && $childs->namespaceURI == WSF_WSDL_NAMESPACE){
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
function wsf_get_all_policies(DomDocument $wsdl_dom, DomNode $binding_node, $operation_name, $is_wsdl_11 = FALSE)
{
	require_once('wsf_wsdl_consts.php');

	$policy_array = array();
	if(!$binding_node)
	return NULL;

	$binding_child_list = $binding_node->childNodes;
	foreach($binding_child_list as $binding_child){
		if($binding_child->localName == WSF_POLICY_REFERENCE){
			$binding_attr = $binding_child->attributes;
			$policy_uri =  $binding_attr->getNamedItem(WSF_URI)->value;
			$binding_policy = get_policy_node($wsdl_dom, $policy_uri);
			$policy_array["operation_policy"] = $binding_policy;
		}

		if($binding_child->localName == WSF_POLICY){
			$binding_policy_child = $binding_child->firstChild;
			$binding_attr = $binding_policy_child->attributes;
			$policy_uri =  $binding_attr->getNamedItem(WSF_URI)->value;
			$binding_policy = get_policy_node($wsdl_dom, $policy_uri);
			$policy_array["operation_policy"] = $binding_policy;
		}

		if($binding_child->localName == WSF_OPERATION && $is_wsdl_11 = FALSE){
			$operation_attr = $binding_child->attributes;
			$operation_ref = $operation_attr->getNamedItem(WSF_REF)->value;
			if(substr(strstr($operation_ref, ":"), 1) == $operation_name && $binding_child->hasChildNodes()){
				foreach($binding_child->childNodes as $input_output){
					if($input_output->firstChild->localName == WSF_POLICY_REFERENCE){  /* there may be several chidren */
						$input_output_attr = $input_output->firstChild->attributes;
						$msg_policy_uri =  $input_output_attr->getNamedItem(WSF_URI)->value;
						$msg_policy = get_policy_node($wsdl_dom, $msg_policy_uri);
						$key = $input_output->localName."_policy";
						$policy_array[$key] = $msg_policy;
					}
				}
			}
		}

		if($binding_child->localName == WSF_OPERATION && $is_wsdl_11 = TRUE){
			$op_name = NULL;
			$operation_attr = $binding_child->attributes;
			if($operation_attr->getNamedItem(WSF_NAME))
			$op_name = $operation_attr->getNamedItem(WSF_NAME)->value;
			if($op_name == $operation_name && $binding_child->hasChildNodes()){
				foreach($binding_child->childNodes as $input_output){
					if($input_output->firstChild && $input_output->firstChild->localName == WSF_POLICY_REFERENCE){  /* there may be several children */
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

function get_policy_node(DomDocument $wsdl_dom, $policy_uri)
{
	require_once('wsf_wsdl_consts.php');

	$root_node = $wsdl_dom->firstChild;
	$root_child_list = $root_node->childNodes;
	foreach($root_child_list as $child){
		if($child->localName == WSF_POLICY){
			$policy_attr = $child->attributes;
			$policy_in_same_wsdl  = strpos($policy_uri, '#');
			if($policy_in_same_wsdl === 0){
				if($policy_attr->getNamedItemNS(WSF_POLICY_ID_NAMESPACE_URI, WSF_ID)->value == substr($policy_uri, 1)){
					$policy_node = $child;
					break;
				}
			} else {/* TODO- get policy from another WSDL */
			/* printf("Policy is not found in the given WSDL"); */
			return;
			}
		}
	}


	if($policy_node){
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
function wsf_get_binding_details(DomNode $operation_node)
{
	require_once('wsf_wsdl_consts.php');

	$binding_array = array();
	$soap_version = 2;
	$wsa_action = NULL;
	$soap_action = NULL;

	$operation_child_list = $operation_node->childNodes;
	foreach($operation_child_list as $operation_child){
		if($operation_child->localName == WSF_BINDINDG_DETAILS && $operation_child->hasAttributes()){
			if($operation_child->attributes->getNamedItem(WSF_WSAWAACTION))
			$wsa_action = $operation_child->attributes->getNamedItem(WSF_WSAWAACTION)->value;
			if($operation_child->attributes->getNamedItem(WSF_SOAPACTION))
			$soap_action = $operation_child->attributes->getNamedItem(WSF_SOAPACTION)->value;
			if($wsa_action)
			$binding_array[WSF_WSA] = $wsa_action;
			if($soap_action)
			$binding_array[WSF_SOAP] = $soap_action;
		}
	}


	$operations_node = $operation_node->parentNode;
	if ($operations_node)
	$service_node = $operations_node->parentNode;
	if ($service_node){
		$soap_type = $service_node->attributes->getNamedItem(WSF_TYPE)->value;
		if($soap_type == WSF_SOAP11)
		$soap_version = 1;
		else if($soap_type == WSF_SOAP12)
		$soap_version = 2;
		else
		$soap_version = 2;
		$binding_array[WSF_SOAP_VERSION] = $soap_version;
	}


	return $binding_array;
}

/**
 * Returns schema node
 * @param DomDocument $wsdl_dom WSDL2.0 DomDocument
 * @return DomNode $schema_node Cloned schema node
 */
function wsf_get_schema_node(&$wsdl_dom, &$wsdl_dom2 = NULL)
{
	require_once('wsf_wsdl_consts.php');


	static $schema_node = NULL;

	if ($schema_node != NULL)
	return $schema_node; // Asume it is a single WSDL
	 
	$root_node = $wsdl_dom->firstChild;
	$root_child_list = $root_node->childNodes;
	foreach($root_child_list as $childs){
		/* this is for inline schema */
		$tmp_node = $childs;
		$schema_node = $tmp_node->cloneNode(TRUE);
		/* end inline schema */

		if($childs->localName == WSF_TYPES){
			$schema_list = $childs->childNodes;
			// to find import schemas
			foreach($schema_list as $schema_child){
				$schema = array();
				$i = 0;
				$import_child_list = $schema_child->childNodes;
				foreach($import_child_list as $import_child){
					if($import_child->localName == "import" && $import_child->attributes->getNamedItem('schemaLocation')){
						$schema["schema".$i]= $import_child->attributes->getNamedItem('schemaLocation')->value;
						$i++;
					}
				}
			}

			if($schema){
				$schema_dom = new DomDocument();
				$schema_dom->preserveWhiteSpace = false;
				foreach($schema as $key => $val){
					$schema_dom->load($val);
					$import_schema_child_list = $schema_dom->childNodes;
					foreach($import_schema_child_list as $import_schema_child){
						if($import_schema_child->localName == 'schema'){
							$tmp_import_schema_node = $import_schema_child;
							$cloned_import_schema_node = $tmp_import_schema_node->cloneNode(TRUE);
							wsf_schema_appendNode($schema_node, $tmp_import_schema_node, $wsdl_dom);
						}
					}
				}
			}
			$wsdl_2_0_child_list2 = $wsdl_dom->firstChild->childNodes;
			foreach($wsdl_2_0_child_list2 as $types_child){
				if($types_child->localName == 'types'){
					$types_child_list = $types_child->childNodes;
					foreach($types_child_list as $schema_child){
						if($schema_child->localName == 'schema'){
							$schema_child_list = $schema_child->childNodes;
							for ($i = $schema_child_list->length; $i >= 0; $i--) {
								$import_child = $schema_child_list->item($i);
								if($import_child && $import_child->localName == 'import' && $import_child->attributes->getNamedItem('schemaLocation')){
									$schema_child->removeChild($import_child);
								}
							}
						}
					}
				}
			}

			if($wsdl_dom2){
				$wsdl_2_0_child_list = $wsdl_dom2->firstChild->childNodes;
				foreach($wsdl_2_0_child_list as $types_child){
					if($types_child->localName == 'types'){
						$types_child_list = $types_child->childNodes;
						foreach($types_child_list as $schema_child){
							if($schema_child->localName == 'schema'){
								$types_child->removeChild($schema_child);
								foreach($schema_node->childNodes as $schema3_child)
								wsf_schema_appendNode($types_child, $schema3_child, $wsdl_dom2);

							}
						}

					}
				}
			}

			if($wsdl_dom2){
				$wsdl_2_0_child_list = $wsdl_dom2->firstChild->childNodes;
				foreach($wsdl_2_0_child_list as $types_child){
					if($types_child->localName == 'types'){
						$types_child_list = $types_child->childNodes;
						foreach($types_child_list as $schema_child){
							if($schema_child->localName == 'schema'){
								$schema_child_list = $schema_child->childNodes;
								for ($i = $schema_child_list->length; $i >= 0; $i--) {
									//foreach($schema_child_list as $import_child){
									$import_child = $schema_child_list->item($i);
									if($import_child->localName == 'import' && $import_child->attributes->getNamedItem('schemaLocation')){
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

function wsf_schema_appendNode( $parent, $child, $doc )
{
	if( $child == NULL)
	{
		return;
	}
	$newChild = NULL;
	if( $child-> nodeType == XML_TEXT_NODE )
	{
		$newChild = $doc-> createTextNode($child->nodeValue);
		//echo $newChild->nodeValue."\n";
	}
	else if( $child-> nodeType == XML_ELEMENT_NODE)
	{
		$childTag = $child->tagName;

		$newChild = $doc-> createElementNS($child->namespaceURI, $childTag);

		foreach( $child->attributes as $attribute)
		{
			$newChild->setAttribute($attribute->name, $attribute->value);
		}

		foreach ( $child->childNodes as $childsChild)
		{
			wsf_schema_appendNode( $newChild, $childsChild, $doc);
		}
	}
	if( $newChild != NULL)
	{
		$parent-> appendChild( $newChild);
	}
}



function is_xsd_type($param_type)
{

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

function wsf_wsdl_util_xsd_to_php_type_map()
{
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

function wsf_wsdl_util_serialize_php_value($xsd_type, $data_value)
{
	$xsd_php_mapping_table = wsf_wsdl_util_xsd_to_php_type_map();
    $serialized_value = $data_value;
    
    if(array_key_exists($xsd_type, $xsd_php_mapping_table))
    {
        $type = $xsd_php_mapping_table[$xsd_type];

        if($type == "boolean")
        {
            if($data_value == FALSE)
            {
                $serialized_value = "false";
            }
            else
            {
                $serialized_value = "true";
            }
        }
    }

    if($serialized_value === NULL) return "";
    return $serialized_value;
}


function wsf_wsdl_util_convert_value($xsd_type, $data_value)
{
	$xsd_php_mapping_table = wsf_wsdl_util_xsd_to_php_type_map();

    $converted_value = $data_value;
    if(array_key_exists($xsd_type, $xsd_php_mapping_table))
    {
        $type = $xsd_php_mapping_table[$xsd_type];
        if($type == 'integer')
            $converted_type = (int)($data_value);
        else if ($type == 'float')
            $converted_type = (float)($data_value);
        else if ($type == 'boolean')
            $converted_type = (boolean)($data_value);
        else if ($type == 'string')
            $converted_type = $data_value;
        else
            $converted_type = $data_value;
    }

	return $converted_value;
}

function wsf_is_rpc_enc_wsdl($wsdl_11_dom, $binding_node, $operation_name)
{
    if(!$binding_node)
        return FALSE;

    $binding_child_list = $binding_node->childNodes;
    $style = NULL;
    foreach($binding_child_list as $binding_child)
    {
        if($binding_child->localName == "binding"){
            $binding_attr = $binding_child->attributes;
            if($binding_attr && $binding_attr->getNamedItem("style")){
                $style =  $binding_attr->getNamedItem("style")->value;
            }
            //var_dump($style)."\n\n";
        }
        if($binding_child->localName == WSF_OPERATION && $style == "rpc"){
            $op_name = NULL;
            $operation_attr = $binding_child->attributes;
            if($operation_attr->getNamedItem(WSF_NAME))
                $op_name = $operation_attr->getNamedItem(WSF_NAME)->value;
            if($op_name == $operation_name && $binding_child->hasChildNodes()){
                foreach($binding_child->childNodes as $input_output){
                    if($input_output->firstChild && $input_output->firstChild->localName == "body"){
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
 * Recursive function to create payload 
 * @param DomDocument $payload_dom 
 * @param array $parameter_struct $sig_model as an array
 * @param DomNode $parent_ele - The parent node to add the content 
 * @param DomNode $root_ele - The top most of parent
 * @param mixed $class_obj - class object to pass
 * @param $namespace_map - Just make sure the unique namespace is used. Newly added
    (passed by reference)
 */


function wsf_create_payload_for_class_map(DomDocument $payload_dom,
                                            array $parameter_struct, 
                                            DomNode $parent_ele, 
                                            DomNode $root_ele, 
                                            $class_obj, 
                                            array &$namespace_map = array())
{
    static $i = 2;

    if($parameter_struct[WSF_HAS_SIG_CHILDS] !== TRUE)
    {
        $values = array_values($namespace_map);
        $prefix = $values[0];
        wsf_create_payload_for_unknown_class_map($payload_dom, $parent_ele, $class_obj, $prefix);
        return;
    }

    foreach($parameter_struct as $key => $value){
        if(is_array($value)){
            if($value["class_map_name"] == "anyType")
            {
                  $values = array_values($namespace_map);
                  $prefix = $values[0];
                  {
                        wsf_create_payload_for_unknown_class_map($payload_dom, $parent_ele, $class_obj, $prefix);
                  }
                  continue;
            }
            if (isset($value[WSF_TYPE_REP]) && $value[WSF_TYPE_REP]){
                if($class_obj->$key)
                {
                    $arg_val = $class_obj->$key;
                    /* type conversion is needed */
                    if(!array_key_exists(WSF_NS, $value) || empty($value[WSF_NS]) || $value[WSF_NS] == "NULL"){
                        $value[WSF_NS] = NULL;
                    }

                    if(!isset($value[WSF_NS]) || $value[WSF_NS] == NULL){
                        $node_name = $key;
                    }
                    else{
                        if(array_key_exists($value[WSF_NS], $namespace_map) && $namespace_map[$value[WSF_NS]] != NULL){
                            $prefix = $namespace_map[$value[WSF_NS]];
                        }
                        else{
                            $prefix = "ns".$i;
                            $i ++;
                            $root_ele->setAttribute("xmlns:".$prefix, $value[WSF_NS]);
                            $namespace_map[$value[WSF_NS]] = $prefix;
                        }
                        $node_name = $prefix.":".$key;
                    }

                    if(array_key_exists("maxOccurs", $value) && ($value["maxOccurs"] == "unbounded" || $value["maxOccurs"] > 1) && is_array($arg_val)){
                        foreach($arg_val as $arg_val_item){
                            $serialized_value = wsf_wsdl_util_serialize_php_value($value[WSF_TYPE_REP], $arg_val_item);
                            $element_2 = $payload_dom->createElement($node_name, $serialized_value);
                            $parent_ele->appendChild($element_2);
                        }
                    }
                    else if(!is_array($arg_val)){
                        /* in a case this is not an array */
                        $serialized_value = wsf_wsdl_util_serialize_php_value($value[WSF_TYPE_REP], $arg_val);
                        $element_2 = $payload_dom->createElement($node_name, $serialized_value);
                        $parent_ele->appendChild($element_2);
                    }
                    else{
                        error_log("Array is given for ". $key ." which should be a non array. \n");
                    }
                }
            }
            else {
                if($class_obj->$key)
                {
                    $arg_val = $class_obj->$key;
                    /* type conversion is needed */
                    if($value[WSF_NS] == "NULL"){
                        $value[WSF_NS] = NULL;
                    }

                    if($value[WSF_NS] == NULL){
                        $node_name = $key;
                    }
                    else{
                        if($namespace_map[$value[WSF_NS]] != NULL){
                            $prefix = $namespace_map[$value[WSF_NS]];
                        }
                        else{
                            $prefix = "ns".$i;
                            $i ++;
                            $root_ele->setAttribute("xmlns:".$prefix, $value[WSF_NS]);
                            $namespace_map[$value[WSF_NS]] = $prefix;
                        }
                        $node_name = $prefix.":".$key;
                    }

                    if(array_key_exists("maxOccurs", $value) && ($value["maxOccurs"] == "unbounded" || $value["maxOccurs"] > 1) && is_array($arg_val)){
                        foreach($arg_val as $arg_val_item){
                            $element_2 = $payload_dom->createElement($node_name);
                            if(is_object($arg_val_item))
                            {
                                if($value[WSF_HAS_SIG_CHILDS] === TRUE)
                                {
                                    wsf_create_payload_for_class_map($payload_dom, $value, $element_2, $root_ele, $arg_val_item, $namespace_map);
                                }
                                else
                                {
                                    wsf_create_payload_for_unknown_class_map($payload_dom, $element_2, $arg_val_item, $prefix);
                                }
                            }
                            else
                            {
                                if($value[WSF_HAS_SIG_CHILDS] === TRUE)
                                {
                                    wsf_create_payload_for_array($payload_dom, $value, $element_2, $root_ele, $arg_val_item, $namespace_map);
                                }
                                else
                                {
                                    wsf_create_payload_for_unknown_array($payload_dom, $element_2, $arg_val_item, $prefix);
                                }
                            }
                            $parent_ele->appendChild($element_2);
                        }
                    }
                    else{
                        /* in a case this is not an array */
                        $element_2 = $payload_dom->createElement($node_name);
                        if(is_object($arg_val))
                        {
                            if($value[WSF_HAS_SIG_CHILDS] === TRUE)
                            {
                                wsf_create_payload_for_class_map($payload_dom, $value, $element_2, $root_ele, $arg_val, $namespace_map);
                            }
                            else
                            {
                                wsf_create_payload_for_unknown_class_map($payload_dom, $element_2, $arg_val, $prefix);
                            }
                        }
                        else
                        {
                            if($value[WSF_HAS_SIG_CHILDS] === TRUE)
                            {
                                wsf_create_payload_for_array($payload_dom, $value, $element_2, $root_ele, $arg_val, $namespace_map);
                            }
                            else
                            {
                                wsf_create_payload_for_unknown_array($payload_dom, $element_2, $arg_val, $prefix);
                            }
                        }
                        $parent_ele->appendChild($element_2);
                    }
                }
            }
        }
    }
}

/** create payload for arrays
 * @param $payload_dom - DomDocument for the payload building
 * @param $parameter_struct - Map of parameters extracted from sig model
 * @param $parent_ele - The parent node to add the content
 * @param $root_ele - The always parent. Just to add all the namespace declaration here..
 * @param $user_arguments - The user given arguments
 * @param $namespace_map - Just make sure the unique namespace is used. Newly added
    (passed by reference)
 */
function wsf_create_payload_for_array(DomDocument $payload_dom,
                                     array $parameter_struct,
                                     DomNode $parent_ele,
                                     DomNode $root_ele,
                                     $user_arguments,
                                     array &$namespace_map = array())
{
    static $i = 2;

    $unknown_schema_found = FALSE;

    if($parameter_struct[WSF_HAS_SIG_CHILDS] !== TRUE){
        $values = array_values($namespace_map);
        $prefix = $values[0];
        if(is_array($user_arguments))
        {
            wsf_create_payload_for_unknown_array($payload_dom, $parent_ele, $user_arguments, $prefix);
        }
        $unknown_schema_found = TRUE;
    }


    foreach($parameter_struct as $key => $value){
        if(is_array($value)){
            if($unknown_schema_found)
            {
                continue;
            }
            if($value["class_map_name"] == "anyType")
            {
                  $values = array_values($namespace_map);
                  $prefix = $values[0];
                  if(is_array($user_arguments))
                  {
                      wsf_create_payload_for_unknown_array($payload_dom, $parent_ele, $user_arguments, $prefix);
                  }
                  continue;
            }
            if (isset($value[WSF_TYPE_REP]) && $value[WSF_TYPE_REP]){
                foreach($user_arguments as $arg_key => $arg_val){
                    if($key == $arg_key){
                        /* type conversion is needed */
                        if($value[WSF_NS] == "NULL"){
                            $value[WSF_NS] = NULL;
                        }

                        if($value[WSF_NS] == NULL){
                            $node_name = $key;
                        }
                        else{
                            if($namespace_map[$value[WSF_NS]] != NULL){
                                $prefix = $namespace_map[$value[WSF_NS]];
                            }
                            else{
                                $prefix = "ns".$i;
                                $i ++;
                                $root_ele->setAttribute("xmlns:".$prefix, $value[WSF_NS]);
                                $namespace_map[$value[WSF_NS]] = $prefix;
                            }
                            $node_name = $prefix.":".$key;
                        }

                        if(array_key_exists("maxOccurs", $value) && ($value["maxOccurs"] == "unbounded" || $value["maxOccurs"] > 1) && is_array($arg_val)){
                            foreach($arg_val as $arg_val_item){
                                $serialized_value = wsf_wsdl_util_serialize_php_value($value[WSF_TYPE_REP], $arg_val_item);
                                $element_2 = $payload_dom->createElement($node_name, $serialized_value);
                                $parent_ele->appendChild($element_2);
                            }
                        }
                        else if(!is_array($arg_val)){
                            /* in a case this is not an array */
                            $serialized_value = wsf_wsdl_util_serialize_php_value($value[WSF_TYPE_REP], $arg_val);
                            $element_2 = $payload_dom->createElement($node_name, $serialized_value);
                            $parent_ele->appendChild($element_2);
                        }
                        else{
                            error_log("Array is given for ". $key ." which should be a non array. \n");
                        }
                    }
                }
            }
            else {
                foreach($user_arguments as $arg_key => $arg_val){
                    if($key == $arg_key){
                        /* type conversion is needed */
                        if($value[WSF_NS] == "NULL"){
                            $value[WSF_NS] = NULL;
                        }

                        if($value[WSF_NS] == NULL){
                            $node_name = $key;
                        }
                        else{
                            if($namespace_map[$value[WSF_NS]] != NULL){
                                $prefix = $namespace_map[$value[WSF_NS]];
                            }
                            else{
                                $prefix = "ns".$i;
                                $i ++;
                                $root_ele->setAttribute("xmlns:".$prefix, $value[WSF_NS]);
                                $namespace_map[$value[WSF_NS]] = $prefix;
                            }
                            $node_name = $prefix.":".$key;
                        }

                        if(array_key_exists("maxOccurs", $value) && ($value["maxOccurs"] == "unbounded" || $value["maxOccurs"] > 1) && is_array($arg_val)){
                            foreach($arg_val as $arg_val_item){
                                $element_2 = $payload_dom->createElement($node_name);
                                if(is_object($arg_val_item))
                                {
                                    if($value[WSF_HAS_SIG_CHILDS] === TRUE)
                                    {
                                        wsf_create_payload_for_class_map($payload_dom, $value, $element_2, $root_ele, $arg_val_item, $namespace_map);
                                    }
                                    else
                                    {
                                        wsf_create_payload_for_unknown_class_map($payload_dom, $element_2, $arg_val_item, $prefix);
                                    }
                                }
                                else
                                {
                                    if($value[WSF_HAS_SIG_CHILDS] === TRUE)
                                    {
                                        wsf_create_payload_for_array($payload_dom, $value, $element_2, $root_ele, $arg_val_item, $namespace_map);
                                    }
                                    else
                                    {
                                        wsf_create_payload_for_unknown_array($payload_dom, $element_2, $arg_val_item, $prefix);
                                    }
                                }
                                $parent_ele->appendChild($element_2);
                            }
                        }
                        else{
                            /* in a case this is not an array */
                            #$element_2 = $payload_dom->createElementNS($value[WSF_NS], $node_name);
                            $element_2 = $payload_dom->createElement($node_name);
                            if(is_object($arg_val))
                            {
                                if($value[WSF_HAS_SIG_CHILDS] === TRUE)
                                {
                                    wsf_create_payload_for_class_map($payload_dom, $value, $element_2, $root_ele, $arg_val, $namespace_map);
                                }
                                else
                                {
                                    wsf_create_payload_for_unknown_class_map($payload_dom, $element_2, $arg_val, $prefix);
                                }
                            }
                            else
                            {
                                if($value[WSF_HAS_SIG_CHILDS] === TRUE)
                                {
                                    wsf_create_payload_for_array($payload_dom, $value, $element_2, $root_ele, $arg_val, $namespace_map);
                                }
                                else
                                {
                                    wsf_create_payload_for_unknown_array($payload_dom, $element_2, $arg_val, $prefix);
                                }
                            }
                            $parent_ele->appendChild($element_2);
                        }
                    }
                }
            }
        }else if($key == WSF_TYPE_REP && is_xsd_type($value)){

            /* TODO multiple values */
            if($user_arguments == NULL || !is_array($user_arguments))
            {
                $serialized_value = wsf_wsdl_util_serialize_php_value($value, $user_arguments);
                $element_2 = $payload_dom->createTextNode($serialized_value);
                $parent_ele->appendChild($element_2);

            }
            else if(is_array($user_arguments))
            {
                error_log("Array is specified for when non-array is expected\n");
            }
        }
    }
}

/** create payload for unknown class maps 
 * @param $payload_dom - DomDocument for the payload building
 * @param $parent_ele - The parent node to add the content
 * @param $user_arguments - The user given argument array 
 * @param $class_obj - object with user data..
 * @param $ns_prefix
 */
function wsf_create_payload_for_unknown_class_map(DomDocument $payload_dom,
                                     DomNode $parent_ele,
                                     $class_obj,
                                     $ns_prefix)
{
    $reflex_obj = new ReflectionObject($class_obj);
    $reflex_properties = $reflex_obj->getProperties();


    foreach($reflex_properties as $reflex_property)
    {
        $key = $reflex_property->getName();
        $value = $reflex_property->getValue($class_obj);

        if($ns_prefix != NULL && !empty($ns_prefix))
        {
            $node_name = $ns_prefix.":".$key;
        }
        else
        {
            $node_name = $key;
        }
        if(is_object($value))
        {
            $current_node = $payload_dom->createElement($node_name);
            wsf_create_payload_for_unknown_class_map($payload_dom, $current_node, $value, $ns_prefix);
            $parent_ele->appendChild($current_node);
        }
        else if(wsf_is_map($value))
        {
            $current_node = $payload_dom->createElement($node_name);
            wsf_create_payload_for_unknown_array($payload_dom, $current_node, $value, $ns_prefix);
            $parent_ele->appendChild($current_node);
        }
        else if(is_array($value))
        {
            foreach($value as $child_value)
            {
                if(is_object($child_value))
                {
                    $current_node = $payload_dom->createElement($node_name);
                    wsf_create_payload_for_unknown_class_map($payload_dom, $current_node, $child_value, $ns_prefix);
                    $parent_ele->appendChild($current_node);
                }
                else if(wsf_is_map($child_value))
                {
                    $current_node = $payload_dom->createElement($node_name);
                    wsf_create_payload_for_unknown_array($payload_dom, $current_node, $child_value, $ns_prefix);
                    $parent_ele->appendChild($current_node);
                }
                else if(is_array($child_value))
                {
                    error_log("Invalid array (inside an array) supplied around ".print_r($child_value, TRUE)." \n");
                }
                else
                {
                    $current_node = $payload_dom->createElement($node_name, $child_value);
                    $parent_ele->appendChild($current_node);
                }
            }
        }
        else
        {
            $current_node = $payload_dom->createElement($node_name, $value);
            $parent_ele->appendChild($current_node);
        }
    }
}



/** create payload for arrays
 * @param $payload_dom - DomDocument for the payload building
 * @param $parent_ele - The parent node to add the content
 * @param $user_arguments - The user given argument array 
 * @param $ns_prefix
 */
function wsf_create_payload_for_unknown_array(DomDocument $payload_dom,
                                     DomNode $parent_ele,
                                     $user_arguments,
                                     $ns_prefix)
{
    foreach($user_arguments as $key=> $value)
    {
        if($ns_prefix != NULL && !empty($ns_prefix))
        {
            $node_name = $ns_prefix.":".$key;
        }
        else
        {
            $node_name = $key;
        }
        if(is_object($value))
        {
            $current_node = $payload_dom->createElement($node_name);
            wsf_create_payload_for_unknown_class_map($payload_dom, $current_node, $value, $ns_prefix);
            $parent_ele->appendChild($current_node);
        }
        else if(wsf_is_map($value))
        {
            $current_node = $payload_dom->createElement($node_name);
            wsf_create_payload_for_unknown_array($payload_dom, $current_node, $value, $ns_prefix);
            $parent_ele->appendChild($current_node);
        }
        else if(is_array($value))
        {
            foreach($value as $child_value)
            {
                if(is_object($child_value))
                {
                    $current_node = $payload_dom->createElement($node_name);
                    wsf_create_payload_for_unknown_class_map($payload_dom, $current_node, $child_value, $ns_prefix);
                    $parent_ele->appendChild($current_node);
                }
                else if(wsf_is_map($child_value))
                {
                    $current_node = $payload_dom->createElement($node_name);
                    wsf_create_payload_for_unknown_array($payload_dom, $current_node, $child_value, $ns_prefix);
                    $parent_ele->appendChild($current_node);
                }
                else if(is_array($child_value))
                {
                    error_log("Invalid array (inside an array) supplied around ".print_r($child_value, TRUE)." \n");
                }
                else
                {
                    $current_node = $payload_dom->createElement($node_name, $child_value);
                    $parent_ele->appendChild($current_node);
                }
            }
        }
        else
        {
            $current_node = $payload_dom->createElement($node_name, $value);
            $parent_ele->appendChild($current_node);
        }
    }
}

/**
 * Recursive function to create payload 
 * @param DomDocument $payload_dom 
 * @param array $parameter_struct $sig_model as an array
 * @param DomNode $parent_ele - The parent node to add the content 
 * @param DomNode $root_ele - The top most of parent
 * @param mixed $class_obj - class object to pass
 * @param $namespace_map - Just make sure the unique namespace is used. Newly added
    (passed by reference)
 */


function wsf_create_rpc_payload_for_class_map(DomDocument $payload_dom,
                                            array $parameter_struct, 
                                            DomNode $parent_ele, 
                                            DomNode $root_ele, 
                                            $class_obj, 
                                            array &$namespace_map = array())
{
    static $i = 2;

    foreach($parameter_struct as $key => $value){
        if(is_array($value)){
            if($value["class_map_name"] == "anyType")
            {
                  $values = array_values($namespace_map);
                  $prefix = $values[0];
                  {
                        wsf_create_payload_for_unknown_class_map($payload_dom, $parent_ele, $class_obj, $prefix);
                  }
                  continue;
            }
                if (isset($value[WSF_TYPE_REP]) && $value[WSF_TYPE_REP]){
                    if($class_obj->$key)
                    {
                        $arg_val = $class_obj->$key;
                        /* type conversion is needed */
                        if($value[WSF_NS] == "NULL"){
                            $value[WSF_NS] = NULL;
                        }

                        if($value[WSF_NS] == NULL){
                            $node_name = $key;
                        }
                        else{
                            if($namespace_map[$value[WSF_NS]] != NULL){
                                $prefix = $namespace_map[$value[WSF_NS]];
                            }
                            else{
                                $prefix = "ns".$i;
                                $i ++;
                                $root_ele->setAttribute("xmlns:".$prefix, $value[WSF_NS]);
                                $namespace_map[$value[WSF_NS]] = $prefix;
                            }
                            $node_name = $prefix.":".$key;
                        }

                        if(array_key_exists("maxOccurs", $value) && ($value["maxOccurs"] == "unbounded" || $value["maxOccurs"] > 1) && is_array($arg_val)){
                            foreach($arg_val as $arg_val_item){
                                $element_2 = $payload_dom->createElement($node_name, $arg_val_item);
                                $element_2->setAttribute("xsi:type", "xsd:".$value[WSF_TYPE_REP]);
                                $parent_ele->appendChild($element_2);
                            }
                        }
                        else{
                            /* in a case this is not an array */
                            $element_2 = $payload_dom->createElement($node_name, $arg_val);
                            $element_2->setAttribute("xsi:type", "xsd:".$value[WSF_TYPE_REP]);
                            $parent_ele->appendChild($element_2);
                        }
                    }
                }
                else {
                    if($class_obj->$key)
                    {
                        $arg_val = $class_obj->$key;
                        /* type conversion is needed */
                        if($value[WSF_NS] == "NULL"){
                            $value[WSF_NS] = NULL;
                        }

                        if($value[WSF_NS] == NULL){
                            $node_name = $key;
                        }
                        else{
                            if($namespace_map[$value[WSF_NS]] != NULL){
                                $prefix = $namespace_map[$value[WSF_NS]];
                            }
                            else{
                                $prefix = "ns".$i;
                                $i ++;
                                $root_ele->setAttribute("xmlns:".$prefix, $value[WSF_NS]);
                                $namespace_map[$value[WSF_NS]] = $prefix;
                            }
                            $node_name = $prefix.":".$key;
                        }

                        if(array_key_exists("maxOccurs", $value) && ($value["maxOccurs"] == "unbounded" || $value["maxOccurs"] > 1) && is_array($arg_val)){
                            foreach($arg_val as $arg_val_item){
                                $element_2 = $payload_dom->createElement($node_name);
                                if(is_object($arg_val_item))
                                {
                                    wsf_create_rpc_payload_for_class_map($payload_dom, $value, $element_2, $root_ele, $arg_val_item, $namespace_map);
                                }
                                else
                                {
                                    wsf_create_rpc_payload_for_array($payload_dom, $value, $element_2, $root_ele, $arg_val_item, $namespace_map);
                                }
                                $parent_ele->appendChild($element_2);
                            }
                        }
                        else{
                            /* in a case this is not an array */
                            $element_2 = $payload_dom->createElement($node_name);
                            if(is_object($arg_val))
                            {
                                wsf_create_rpc_payload_for_class_map($payload_dom, $value, $element_2, $root_ele, $arg_val, $namespace_map);
                            }
                            else
                            {
                                wsf_create_rpc_payload_for_array($payload_dom, $value, $element_2, $root_ele, $arg_val, $namespace_map);
                            }
                            $parent_ele->appendChild($element_2);
                        }
                    }
                }
        }else if($key == WSF_TYPE_REP && is_xsd_type($value)){
            /* TODO multiple values */
            if(!$user_arguments[0])
                $element_2 = $payload_dom->createTextNode(0);
            else
                $element_2 = $payload_dom->createTextNode($user_arguments[0]);
            $root_ele->appendChild($element_2);
        }
    }
}

/** create payload for arrays
 * @param $payload_dom - DomDocument for the payload building
 * @param $parameter_struct - Map of parameters extracted from sig model
 * @param $parent_ele - The parent node to add the content
 * @param $root_ele - The always parent. Just to add all the namespace declaration here..
 * @param $user_arguments - The user given argument array 
 * @param $namespace_map - Just make sure the unique namespace is used. Newly added
    (passed by reference)
 */
function wsf_create_rpc_payload_for_array(DomDocument $payload_dom,
                                     array $parameter_struct,
                                     DomNode $parent_ele,
                                     DomNode $root_ele,
                                     array $user_arguments,
                                     array &$namespace_map = array())
{
    static $i = 2;

    foreach($parameter_struct as $key => $value){
        if(is_array($value)){
            if($value["class_map_name"] == "anyType")
            {
                  $values = array_values($namespace_map);
                  $prefix = $values[0];
                  if(is_array($user_arguments))
                  {
                      wsf_create_payload_for_unknown_array($payload_dom, $parent_ele, $user_arguments, $prefix);
                  }
                  continue;
            }
            if (isset($value[WSF_TYPE_REP]) && $value[WSF_TYPE_REP]){
                foreach($user_arguments as $arg_key => $arg_val){
                    if($key == $arg_key){
                        /* type conversion is needed */
                        if($value[WSF_NS] == "NULL"){
                            $value[WSF_NS] = NULL;
                        }

                        if($value[WSF_NS] == NULL){
                            $node_name = $key;
                        }
                        else{
                            if($namespace_map[$value[WSF_NS]] != NULL){
                                $prefix = $namespace_map[$value[WSF_NS]];
                            }
                            else{
                                $prefix = "ns".$i;
                                $i ++;
                                $root_ele->setAttribute("xmlns:".$prefix, $value[WSF_NS]);
                                $namespace_map[$value[WSF_NS]] = $prefix;
                            }
                            $node_name = $prefix.":".$key;
                        }

                        if(array_key_exists("maxOccurs", $value) && ($value["maxOccurs"] == "unbounded" || $value["maxOccurs"] > 1) && is_array($arg_val)){
                            foreach($arg_val as $arg_val_item){
                                $element_2 = $payload_dom->createElement($node_name, $arg_val_item);
                                $element_2->setAttribute("xsi:type", "xsd:".$value[WSF_TYPE_REP]);
                                $parent_ele->appendChild($element_2);
                            }
                        }
                        else{
                            /* in a case this is not an array */
                            $element_2 = $payload_dom->createElement($node_name, $arg_val);
                            $element_2->setAttribute("xsi:type", "xsd:".$value[WSF_TYPE_REP]);
                            $parent_ele->appendChild($element_2);
                        }
                    }
                }
            }
            else {
                foreach($user_arguments as $arg_key => $arg_val){
                    if($key == $arg_key){
                        /* type conversion is needed */
                        if($value[WSF_NS] == "NULL"){
                            $value[WSF_NS] = NULL;
                        }

                        if($value[WSF_NS] == NULL){
                            $node_name = $key;
                        }
                        else{
                            if($namespace_map[$value[WSF_NS]] != NULL){
                                $prefix = $namespace_map[$value[WSF_NS]];
                            }
                            else{
                                $prefix = "ns".$i;
                                $i ++;
                                $root_ele->setAttribute("xmlns:".$prefix, $value[WSF_NS]);
                                $namespace_map[$value[WSF_NS]] = $prefix;
                            }
                            $node_name = $prefix.":".$key;
                        }

                        if(array_key_exists("maxOccurs", $value) && ($value["maxOccurs"] == "unbounded" || $value["maxOccurs"] > 1) && is_array($arg_val)){
                            foreach($arg_val as $arg_val_item){
                                $element_2 = $payload_dom->createElement($node_name);
                                if(is_object($arg_val_item))
                                {
                                    wsf_create_rpc_payload_for_array($payload_dom, $value, $element_2, $root_ele, $arg_val_item, $namespace_map);
                                }
                                else
                                {
                                    wsf_create_rpc_payload_for_class_map($payload_dom, $value, $element_2, $root_ele, $arg_val_item, $namespace_map);
                                }
                                $parent_ele->appendChild($element_2);
                            }
                        }
                        else{
                            /* in a case this is not an array */
                            #$element_2 = $payload_dom->createElementNS($value[WSF_NS], $node_name);
                            $element_2 = $payload_dom->createElement($node_name);
                            wsf_create_rpc_payload_for_array($payload_dom, $value, $element_2, $root_ele, $arg_val, $namespace_map);
                            $parent_ele->appendChild($element_2);
                        }
                    }
                }
            }
        }else if($key == WSF_TYPE_REP && is_xsd_type($value)){
            /* TODO multiple values */
            if(!$user_arguments[0])
                $element_2 = $payload_dom->createTextNode(0);
            else
                $element_2 = $payload_dom->createTextNode($user_arguments[0]);
            $root_ele->appendChild($element_2);
        }
    }
}

/**
 * Recursive function to create temperaly structure
 * @param DomNode $types_node schema node of the WSDL
 * @param string $param_type Type of the parameter
 */


function wsf_create_temp_struct(DomNode $param_child, $wrapper_ns)
{
    $rec_array = array();
    $param_nil = NULL;
    $param_min = NULL;
    $param_max = NULL;
    $param_ns = NULL;
    $param_has_sig_childs = FALSE;
   
    $param_attr = $param_child->attributes;
    if($param_attr->getNamedItem(WSF_WSDL_SIMPLE))
        $wrap_type = $param_attr->getNamedItem(WSF_WSDL_SIMPLE)->value;
    if($param_attr->getNamedItem(WSF_TARGETNAMESPACE))
        $param_ns = $param_attr->getNamedItem(WSF_TARGETNAMESPACE)->value;
    if($param_attr->getNamedItem(WSF_TYPE))
        $param_type = $param_attr->getNamedItem(WSF_TYPE)->value;
    if($param_attr->getNamedItem(WSF_TYPE_NAMESPACE))
        $param_type_ns = $param_attr->getNamedItem(WSF_TYPE_NAMESPACE)->value;
    if($param_attr->getNamedItem('minOccurs'))
        $param_min = $param_attr->getNamedItem('minOccurs')->value;
    if($param_attr->getNamedItem('maxOccurs'))
        $param_max = $param_attr->getNamedItem('maxOccurs')->value;
    if($param_attr->getNamedItem('nillable'))
        $param_nil = $param_attr->getNamedItem('nillable')->value;

    if($param_child->hasChildNodes()){
        $param_has_sig_childs = TRUE;
    }
    $rec_array[WSF_HAS_SIG_CHILDS] = $param_has_sig_childs;

    if($wrap_type == 'yes'){ /* this is for simple types */
        if($param_ns == NULL)
            $rec_array[WSF_NS] = "NULL";//$wrapper_ns;
        else
            $rec_array[WSF_NS] = $param_ns;

        $rec_array[WSF_TYPE_REP] = $param_type;
        if($param_min != 1)
            $rec_array['minOccurs'] = $param_min; 
        if($param_max != 1)
            $rec_array['maxOccurs'] = $param_max; 
        if($param_nil)
            $rec_array['nillable'] = $param_nil; 
    }
    else{
        
        /* for some WSDL sig model gives simple type no even there are
         * xsd types in the parameter. So this check is needed */
        if (is_xsd_type($param_type)){
            if($param_ns == NULL)
                $rec_array[WSF_NS] = $wrapper_ns;
            else
                $rec_array[WSF_NS] = $param_ns;

            $rec_array[WSF_TYPE_REP] = $param_type;
            if($param_min)
                $rec_array['minOccurs'] = $param_min;
            if($param_max != 1)
                $rec_array['maxOccurs'] = $param_max;
            if($param_nil)
                $rec_array['nillable'] = $param_nil;
        }
        else{
            if($param_ns == NULL)
                $rec_array[WSF_NS] = $wrapper_ns;
            else
                $rec_array[WSF_NS] = $param_ns;

            $rec_array[WSF_NS] = $param_ns;
            $rec_array['class_map_name'] = $param_type;
            if($param_min)
                $rec_array['minOccurs'] = $param_min;
            if($param_max != 1)
                $rec_array['maxOccurs'] = $param_max;
            if($param_nil)
                $rec_array['nillable'] = $param_nil;

            $param_child_list_level2 = $param_child->childNodes;
            foreach($param_child_list_level2 as $param_child_level2){
                $param_child_level2_attr = $param_child_level2->attributes;
                $param_level2_name = $param_child_level2_attr->getNamedItem(WSF_NAME)->value;
                $rec_array[$param_level2_name] = wsf_create_temp_struct($param_child_level2, $wrapper_ns);
            }
        }
    }

    return $rec_array;

}

/**
 * parse payload and return an array of paramerters
 * @param $payload 
 * @param $parameter_struct
 * @return
 */
function wsf_parse_payload_for_array(DomNode $payload, array $parameter_struct)
{
    $parse_tree = array();

    $is_just_text = FALSE;
    
    /* for now, we only support complex type sequences */
    //$child_nodes = $payload->childNodes;

    $original_payload = $payload;
    while($payload != NULL && $payload->nodeType != XML_ELEMENT_NODE)
    {
        $payload = $payload->nextSibling;
    }
    if($payload == NULL) {
        $is_just_text = TRUE;
    }


    if($payload != NULL)
    {
        /* go for the childs */
        $current_child = $payload->firstChild;
        while($current_child != NULL && $current_child->nodeType != XML_ELEMENT_NODE)
        {
            $current_child = $current_child->nextSibling;
        }
        if($current_child == NULL)
        {
            $is_just_text = TRUE;
        }
    }

    foreach($parameter_struct as $key => $value)
    {   
        if(is_array($value))
        {
            if($is_just_text)
            {
                continue;
            }
            if($value["class_map_name"] == "anyType")
            {
                $tag_name = $current_child->localname;
                if($key == $tag_name)
                {
                  $converted_value = wsf_parse_payload_for_unknown_array($current_child);
                  $parse_tree[$key] = $converted_value;
                  continue;
                }
            }
            if(isset($value[WSF_TYPE_REP]) && $value[WSF_TYPE_REP]){
                if($key == $current_child->localName) {
                    if(array_key_exists("maxOccurs", $value) && ($value["maxOccurs"] == "unbounded" || $value["maxOccurs"] > 1)){
                        $i = 0;
                        $parse_tree[$key] = array();
                        while($current_child !== NULL && $current_child->localName == $key)
                        {
                            if($current_child->firstChild)
                            {
                                $converted_value =  wsf_wsdl_util_convert_value($value[WSF_TYPE_REP], $current_child->firstChild->wholeText);
                            }
                            else
                            {
                                if(!isset($value["nillable"]))
                                {
                                    error_log("Non nillable element". $key ."is nil. \n");
                                }
                                $converted_value = "";
                            }
                            $parse_tree[$key][$i++] = $converted_value;
                            $current_child = $current_child->nextSibling;
                            while($current_child != NULL && $current_child->nodeType != XML_ELEMENT_NODE)
                            {
                                $current_child = $current_child->nextSibling;
                            }
                        }
                    }
                    else
                    {
                        if($current_child->firstChild)
                        {
                            $converted_value =  wsf_wsdl_util_convert_value($value[WSF_TYPE_REP], $current_child->firstChild->wholeText);
                        }
                        else
                        {
                            if(!isset($value["nillable"]))
                            {
                                error_log("Non nillable element". $key ."is nil. \n");
                            }
                            $converted_value = "";
                        }
                        $parse_tree[$key] = $converted_value;
                        $current_child = $current_child->nextSibling;
                    }
                }
                else
                {
                    if($value["minOccurs"] && $value["miOccurs"] == 0){
                        // so this if fine
                    }
                    else{
                       error_log("minOccurs!=0 element". $key ."doesn't exist.\n");
                    }
                }
            }
            else
            {
                if($key == $current_child->localName) {
                    if(array_key_exists("maxOccurs", $value) && ($value["maxOccurs"] == "unbounded" || $value["maxOccurs"] > 1)){
                        $i = 0;
                        $parse_tree[$key] = array();
                        while($current_child !== NULL && $current_child->localName == $key)
                        {
                            if($current_child->firstChild)
                            {
                                if($value[WSF_HAS_SIG_CHILDS] === TRUE)
                                {
                                    $converted_value = wsf_parse_payload_for_array($current_child, $value);
                                }
                                else
                                {
                                    $converted_value = wsf_parse_payload_for_unknown_array($current_child);
                                }
                            }
                            else
                            {
                                if(!isset($value["nillable"]))
                                {
                                    error_log("Non nillable element". $key ."is nil. \n");
                                }
                                $converted_value = "";
                            }
                            $parse_tree[$key][$i++] = $converted_value;
                            $current_child = $current_child->nextSibling;
                        }
                    }
                    else
                    {
                        if($current_child->firstChild)
                        {
                            if($value[WSF_HAS_SIG_CHILDS] === TRUE)
                            {
                                $converted_value = wsf_parse_payload_for_array($current_child, $value);
                            }
                            else
                            {
                                $converted_value = wsf_parse_payload_for_unknown_array($current_child);
                            }
                        }
                        else
                        {
                            if(!isset($value["nillable"]))
                            {
                                error_log("Non nillable element". $key ."is nil. \n");
                            }
                            $converted_value = "";
                        }
                        $parse_tree[$key] = $converted_value;
                        $current_child = $current_child->nextSibling;
                    }
                }
                else
                {
                    if($value["minOccurs"] && $value["miOccurs"] == 0){
                        // so this if fine
                    }
                    else{
                       error_log("minOccurs!=0 element". $key ."doesn't exist.\n");
                    }
                }
            }
        }else if($key == WSF_TYPE_REP && is_xsd_type($value)){
            if ($payload != NULL && $payload->nodeType != XML_TEXT_NODE)
            {
                $original_value = $payload->nodeValue;
                $converted_value = wsf_wsdl_util_convert_value($value, $original_value);

                return $converted_value;
            }
        }
    }

    return $parse_tree;
}


/**
 * parse payload and return an object hierarchy
 * @param $payload 
 * @param $parameter_struct
 * @return
 */
function wsf_parse_payload_for_class_map(DomNode $payload, array $parameter_struct, $element_name, $class_map)
{
    if(is_array($class_map))
    {
        $class_name = $class_map[$element_name];
    }
    if(!isset($class_name) || $class_name == NULL)
    {
        $class_name = $element_name;
    }
    
    while($payload != NULL && $payload->nodeType != XML_ELEMENT_NODE)
    {
        $payload = $payload->nextSibling;
    }
    if($payload == NULL) {
        return NULL;
    }

    try
    {
        $ref_class = new ReflectionClass($class_name);
        if ($ref_class->isInstantiable()) {
            $object = $ref_class->newInstance();

        }
    } catch(Exception $e) {
        $object = new WSFUnknownSchemaConstruct();
    }

    if($object == NULL){
        return NULL;
    }

    /* for now, we only support complex type sequences */
    //$child_nodes = $payload->childNodes;
    $current_child = $payload->firstChild;
    while($current_child != NULL && $current_child->nodeType != XML_ELEMENT_NODE)
    {
        $current_child = $current_child->nextSibling;
    }
    if($current_child == NULL)
    {
        return $parse_tree;
    }

    foreach($parameter_struct as $key => $value)
    {   
        if(is_array($value))
        {
            if($value["class_map_name"] == "anyType")
            {
                $tag_name = $current_child->localname;
                if($key == $tag_name)
                {
                  $converted_value = wsf_parse_payload_for_unknown_class_map($current_child, "anyType_for".$key, $class_map);
                  $parse_tree[$key] = $converted_value;
                  continue;
                }
            }
            if(isset($value[WSF_TYPE_REP]) && $value[WSF_TYPE_REP]){
                if($key == $current_child->localName) {
                    if(array_key_exists("maxOccurs", $value) && ($value["maxOccurs"] == "unbounded" || $value["maxOccurs"] > 1)){
                        $i = 0;
                        $tmp_array = array();
                        while($current_child !== NULL && $current_child->localName == $key)
                        {
                            if($current_child->firstChild)
                            {
                                $converted_value =  wsf_wsdl_util_convert_value($value[WSF_TYPE_REP], $current_child->firstChild->wholeText);
                            }
                            else
                            {
                                if(!isset($value["nillable"]))
                                {
                                    error_log("Non nillable element". $key ."is nil. \n");
                                }
                                $converted_value = "";
                            }
                            $tmp_array[$i++] = $converted_value;
                            $current_child = $current_child->nextSibling;
                        }
                        $object->$key = $tmp_array;
                    }
                    else
                    {
                        if($current_child->firstChild)
                        {
                            $converted_value =  wsf_wsdl_util_convert_value($value[WSF_TYPE_REP], $current_child->firstChild->wholeText);
                        }
                        else
                        {
                            if(!isset($value["nillable"]))
                            {
                                error_log("Non nillable element". $key ."is nil. \n");
                            }
                            $converted_value = "";
                        }
                        $object->$key = $converted_value;
                        $current_child = $current_child->nextSibling;
                        while($current_child != NULL && $current_child->nodeType != XML_ELEMENT_NODE)
                        {
                            $current_child = $current_child->nextSibling;
                        }
                    }
                }
                else
                {
                    if($value["minOccurs"] && $value["miOccurs"] == 0){
                        // so this if fine
                    }
                    else{
                       error_log("minOccurs!=0 element". $key ."doesn't exist.\n");
                    }
                }
            }
            else
            {
                $class_map_name = $value["class_map_name"];

                if($key == $current_child->localName) {
                    if(array_key_exists("maxOccurs", $value) && ($value["maxOccurs"] == "unbounded" || $value["maxOccurs"] > 1)){
                        $i = 0;
                        $tmp_array = array();
                        while($current_child !== NULL && $current_child->localName == $key)
                        {
                            if($current_child->firstChild)
                            {
                                if($value[WSF_HAS_SIG_CHILDS] === TRUE)
                                {
                                    $converted_value = wsf_parse_payload_for_class_map($current_child, $value, $class_map_name, $class_map);
                                }
                                else
                                {
                                    $converted_value = wsf_parse_payload_for_unknown_class_map($current_child, $class_map_name, $class_map);
                                }
                            }
                            else
                            {
                                if(!isset($value["nillable"]))
                                {
                                    error_log("Non nillable element". $key ."is nil. \n");
                                }
                                $converted_value = "";
                            }
                            $tmp_array[$i++] = $converted_value;
                            $current_child = $current_child->nextSibling;
                        }
                        $object->$key = $tmp_array;
                    }
                    else
                    {
                        if($current_child->firstChild)
                        {
                            if($value[WSF_HAS_SIG_CHILDS] === TRUE)
                            {
                                $converted_value = wsf_parse_payload_for_class_map($current_child, $value, $class_map_name, $class_map);
                            }
                            else
                            {
                                $converted_value = wsf_parse_payload_for_unknown_class_map($current_child, $class_map_name, $class_map);
                            }
                        }
                        else
                        {
                            if(!isset($value["nillable"]))
                            {
                                error_log("Non nillable element". $key ."is nil. \n");
                            }
                            $converted_value = "";
                        }
                        $object->$key = $converted_value;
                        $current_child = $current_child->nextSibling;
                    }
                }
                else
                {
                    if($value["minOccurs"] && $value["miOccurs"] == 0){
                        // so this if fine
                    }
                    else{
                       error_log("minOccurs!=0 element". $key ."doesn't exist.\n");
                    }
                }
            }
        }
    }

    return $object;
}

/**
 * parse payload and return an object of paramerters for used in serverside
 * @param $payload 
 * @param $parameter_struct
 * @return
 */
function wsf_parse_payload_for_service_class_map(DomNode $payload, array $parameter_struct, $element_name, $class_map)
{
    $parse_tree = array();
    
    /* for now, we only support complex type sequences */
    //$child_nodes = $payload->childNodes;
    $current_child = $payload->firstChild;
    foreach($parameter_struct as $key => $value)
    {   
        if(is_array($value))
        {
            if(isset($value[WSF_TYPE_REP]) && $value[WSF_TYPE_REP]){
                if($key == $current_child->localName) {
                    if(array_key_exists("maxOccurs", $value) && ($value["maxOccurs"] == "unbounded" || $value["maxOccurs"] > 1)){
                        $i = 0;
                        $parse_tree[$key] = array();
                        while($current_child->localName == $key)
                        {
                            if($current_child->firstChild)
                            {
                                $converted_value =  wsf_wsdl_util_convert_value($value[WSF_TYPE_REP], $current_child->firstChild->wholeText);
                            }
                            else
                            {
                                if(!isset($value["nillable"]))
                                {
                                    error_log("Non nillable element". $key ."is nil. \n");
                                }
                                $converted_value = "";
                            }
                            $parse_tree[$key][$i++] = $converted_value;
                            $current_child = $current_child->nextSibling;
                        }
                    }
                    else
                    {
                        if($current_child->firstChild)
                        {
                            $converted_value =  wsf_wsdl_util_convert_value($value[WSF_TYPE_REP], $current_child->firstChild->wholeText);
                        }
                        else
                        {
                            if(!isset($value["nillable"]))
                            {
                                error_log("Non nillable element". $key ."is nil. \n");
                            }
                            $converted_value = "";
                        }
                        $parse_tree[$key] = $converted_value;
                        $current_child = $current_child->nextSibling;
                    }
                }
                else
                {
                    if($value["minOccurs"] && $value["miOccurs"] == 0){
                        // so this if fine
                    }
                    else{
                       error_log("minOccurs!=0 element". $key ."doesn't exist.\n");
                    }
                }
            }
            else
            {
                $class_map_name = $value["class_map_name"];

                if($key == $current_child->localName) {
                    if(array_key_exists("maxOccurs", $value) && ($value["maxOccurs"] == "unbounded" || $value["maxOccurs"] > 1)){
                        $i = 0;
                        $parse_tree[$key] = array();
                        while($current_child->localName == $key)
                        {
                            if($current_child->firstChild)
                            {
                                if($value[WSF_HAS_SIG_CHILDS] === TRUE)
                                {
                                    $converted_value = wsf_parse_payload_for_class_map($current_child, $value, $class_map_name, $class_map);
                                }
                                else
                                {
                                    $converted_value = wsf_parse_payload_for_unknown_class_map($current_child, $class_map_name, $class_map);
                                }
                            }
                            else
                            {
                                if(!isset($value["nillable"]))
                                {
                                    error_log("Non nillable element". $key ."is nil. \n");
                                }
                                $converted_value = "";
                            }
                            $parse_tree[$key][$i++] = $converted_value;
                            $current_child = $current_child->nextSibling;
                        }
                    }
                    else
                    {
                        if($current_child->firstChild)
                        {
                            if($value[WSF_HAS_SIG_CHILDS] === TRUE)
                            {
                                $converted_value = wsf_parse_payload_for_class_map($current_child, $value, $class_map_name, $class_map);
                            }
                            else
                            {
                                $converted_value = wsf_parse_payload_for_unknown_class_map($current_child, $class_map_name, $class_map);
                            }
                        }
                        else
                        {
                            if(!isset($value["nillable"]))
                            {
                                error_log("Non nillable element". $key ."is nil. \n");
                            }
                            $converted_value = "";
                        }
                        $parse_tree[$key] = $converted_value;
                        $current_child = $current_child->nextSibling;
                    }
                }
                else
                {
                    if($value["minOccurs"] && $value["miOccurs"] == 0){
                        // so this if fine
                    }
                    else{
                       error_log("minOccurs!=0 element". $key ."doesn't exist.\n");
                    }
                }
            }
        }
    }

    return $parse_tree;
}

/**
 * wsf_parse_payload_for_unknown_array
 * @param $current_node node to parse
 * @returns array of parsed data
 */
function wsf_parse_payload_for_unknown_array($current_node)
{
    $param_child = array();

    foreach($current_node->childNodes as $child)
    {
        if($child->nodeType == XML_TEXT_NODE)
        {
            $node_value = $child->nodeValue;
            return $node_value;
        }


        $name = $child->tagName;
        $needle = strpos($name, ':');

        if($needle !== FALSE)
        {
            $name = substr($name, $needle + 1);
        }

        if($child->nodeType ==  XML_ELEMENT_NODE)
        {
            $node_value = wsf_parse_payload_for_unknown_array($child);
        }
        else
        {
            $node_value = NULL;
        }

        if($node_value !== NULL)
        {
            if($param_child[$name] === NULL)
            {
                $param_child[$name] = $node_value;
            }
            else
            {
                if(is_array($param_child[$name]) && !wsf_is_map($param_child[$name]))
                {
                    $i = count($param_child[$name]);
                    $param_child[$name][$i] = $node_value;
                }
                else
                {
                    $tmp = $param_child[$name];
                    $param_child[$name] = array($tmp, $node_value);
                }
            }
        }
    }

    return $param_child;
}

/**
 * wsf_parse_payload_for_unknown_class_map
 * @param $current_node node to parse
 * @returns array of parsed data
 */
function wsf_parse_payload_for_unknown_class_map($current_node, $element_name, $class_map)
{
    if(is_array($class_map))
    {
        $class_name = $class_map[$element_name];
    }
    if(!isset($class_name) || $class_name == NULL)
    {
        $class_name = $element_name;
    }
   
    try
    {
        $ref_class = new ReflectionClass($class_name);
        if ($ref_class->isInstantiable()) {
            $object = $ref_class->newInstance();

        }
    } catch(Exception $e) {
        $object = new WSFUnknownSchemaConstruct();
    }

    if($object == NULL){
        return NULL;
    }


    foreach($current_node->childNodes as $child)
    {
        if($child->nodeType == XML_TEXT_NODE)
        {
            $node_value = $child->nodeValue;
            return $node_value;
        }


        $name = $child->tagName;
        $needle = strpos($name, ':');

        if($needle !== FALSE)
        {
            $name = substr($name, $needle + 1);
        }

        if($child->nodeType ==  XML_ELEMENT_NODE)
        {
            $node_value = wsf_parse_payload_for_unknown_class_map($child, $name, $class_map);
        }
        else
        {
            $node_value = NULL;
        }

        if($node_value !== NULL)
        {
            if($object->$name === NULL)
            {
                $object->$name = $node_value;
            }
            else
            {
                if(is_array($object->$name) && !wsf_is_map($object->$name))
                {
                    $i = count($object->$name);
                    $tmp = $object->$name;
                    $tmp[$i] = $node_value;
                    $object->$name = $tmp;
                }
                else
                {
                    $tmp = $object->$name;
                    $object->$name = array($tmp, $node_value);
                }
            }
        }
    }

    return $object;
}


/**
 * @param node to start with
 * @return the node with comments skiped
 */
function wsf_remove_next_whitespace_and_comments(&$xml)
{
    while($xml != NULL && $xml->nodeType != XML_ELEMENT_NODE)
    {
        $xml = $xml->nextSibling;
    }
    return $xml;
}

function wsf_test_serialize_node($xml)
{
    $nodeText = "";

    if ($xml->nodeType == XML_TEXT_NODE) { //for text node
        $nodeText = $xml->nodeValue;
    }
    else if ( $xml->nodeType == XML_ELEMENT_NODE) // for element node
    {
        $nodeName = $xml->nodeName;

        $nodeText = "<" . $nodeName;

        // add attributes
        foreach( $xml->attributes as $attri)
        {
            $nodeText .= " " . $attri->name . "=" . "\"" . $attri->value . "\" ";
        }

        $nodeText .= ">";

        // add childs

        foreach ($xml->childNodes as $child)
        {
            $childText = wsf_test_serialize_node($child);
            $nodeText .= $childText;
        }

        $nodeText .= "</" . $nodeName . ">";

    }
    return $nodeText;
}

function wsf_is_map($map)
{
    if(!is_array($map))
    {
        return FALSE;
    }
    $keys = array_keys($map);
    if($keys[0] === 0)
    {
        return FALSE;
    }
    return TRUE;
}

class WSFUnknownSchemaConstruct
{
    public $properties;
    public function __construct()
    {
        $this->properties = array();
    }

    public function __get($name)
    {
        return $this->properties[$name];
    }

    public function __set($name, $value)
    {
        $this->properties[$name] = $value;
    }

    public function __isset($name)
    {
        if($this->properties[$name] == NULL)
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }

    public function __unset($name)
    {
        if($this->properties[$name] != NULL)
        {
            $this->properties[$name] = NULL;
        }
    }
}

?>
