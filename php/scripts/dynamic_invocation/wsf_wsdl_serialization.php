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
 * Recursive function to create payload 
 * @param DomDocument $payload_dom 
 * @param array $sig_model as a DomNode
 *      example of sig..
 *
 *      <params wrapper-element="myDemo"
 *             wrapper-element-ns="http://wso2.org/dyn/codegen/demo"
 *             simple="no" contentModel="all">
 *          <param token="#in"
 *               targetNamespace="http://wso2.org/dyn/codegen/demo"
 *               minOccurs="1" maxOccurs="1" name="demo1" type="int"
 *               type-namespace="http://www.w3.org/2001/XMLSchema" 
 *               type-prefix="xs" simple="yes"/>
 *          <param token="#in" 
 *               targetNamespace="http://wso2.org/dyn/codegen/demo"
 *               minOccurs="1" maxOccurs="1" name="demo2" type="string"
 *               type-namespace="http://www.w3.org/2001/XMLSchema" 
 *               type-prx="xs" simple="yes"/>
 *      </params>
 *
 *
 * @param DomNode $parent_node - The parent node to add the content 
 * @param DomNode $root_node - The top most of parent
 * @param mixed $user_obj - class object to pass
 * @param $prefix_i - next available prefix index 
 * @param $namespace_map - Just make sure the unique namespace is used.
    Newly added (passed by reference)
 */


function wsf_create_payload_for_class_map(DomDocument $payload_dom,
                                            DomNode $sig_node, 
                                            DomNode $parent_node, 
                                            DomNode $root_node, 
                                            $user_obj, 
                                            &$prefix_i, 
                                            array &$namespace_map) {

    ws_log_write(__FILE__, __LINE__, WSF_LOG_DEBUG, "Loading in to creating payload from classmap");
    ws_log_write(__FILE__, __LINE__, WSF_LOG_DEBUG, wsf_test_serialize_node($sig_node));

    // This is not expected in the class map mode to have params with no childs
    // So mark it as an unknown schema

    // here we always expect structures with childs
    if(!$sig_node->hasChildNodes()) {
        
        // Just take the first namespace in the map to create the xml elements in 
        // the unknown structure..
        $values = array_values($namespace_map);
        $prefix = $values[0];
        wsf_create_payload_for_unknown_class_map($payload_dom, $parent_node, $user_obj, $prefix);
        return;
    }

    // filling user class information to the array..
    $user_arguments = wsf_convert_classobj_to_array($sig_node, $user_obj);
    
    wsf_build_content_model($sig_node, $user_arguments, $parent_node, $payload_dom, $root_node, $prefix_i, $namespace_map);
}

/** create payload for arrays
 * @param $payload_dom - DomDocument for the payload building
 * @param $sig_node - sig model
 * @param $parent_node - The parent node to add the content
 * @param $root_node - The always parent. Just to add all the namespace declaration here..
 * @param $user_arguments - The user given arguments
 * @param $prefix_i - next available prefix index 
 * @param $namespace_map - Just make sure the unique namespace is used. Newly added
    (passed by reference)
 */
function wsf_create_payload_for_array(DomDocument $payload_dom,
                                     DOMNode $sig_node,
                                     DomNode $parent_node,
                                     DomNode $root_node,
                                     $user_arguments,
                                     &$prefix_i, 
                                     array &$namespace_map) {

    ws_log_write(__FILE__, __LINE__, WSF_LOG_DEBUG, "Loading in to creating payload from arrays");
    ws_log_write(__FILE__, __LINE__, WSF_LOG_DEBUG, wsf_test_serialize_node($sig_node));

    // here we always expect structures with childs
    if(!$sig_node->hasChildNodes()) {
        
        // Just take the first namespace in the map to create the xml elements in 
        // the unknown structure..
        $values = array_values($namespace_map);
        $prefix = $values[0];
        wsf_create_payload_for_unknown_class_map($payload_dom, $parent_node, $user_obj, $prefix);
        return;
    }


    if($sig_node->hasAttributes()) {
        wsf_build_content_model($sig_node, $user_arguments, $parent_node, $payload_dom,  $root_node, $prefix_i, $namespace_map);
    }
    else {
        // this situation meets only for non-wrapped mode as doclit-bare wsdls
        $the_only_node = $sig_node->firstChild;

        $is_simple = FALSE;
        if($the_only_node->attributes->getNamedItem(WSF_WSDL_SIMPLE) &&
                $the_only_node->attributes->getNamedItem(WSF_WSDL_SIMPLE)->value == "yes") {
            $is_simple = TRUE;
        }
        $param_type = NULL;
        if($the_only_node->attributes->getNamedItem(WSF_TYPE)) {
            $param_type = $the_only_node->attributes->getNamedItem(WSF_TYPE)->value;
        }
        if($is_simple) {
            if($user_arguments == NULL || !is_array($user_arguments)) {
                $serialized_value = wsf_wsdl_util_serialize_php_value($param_type, $user_arguments);
                $text_node = $payload_dom->createTextNode($serialized_value);
                $parent_node->appendChild($text_node);

            }
            else if(is_array($user_arguments)) {
                ws_log_write(__FILE__, __LINE__, WSF_LOG_ERROR, "Array is specified when non-array is expected for the root node\n");
            }
        }
    }
}

/** create payload for unknown class maps 
 * @param $payload_dom - DomDocument for the payload building
 * @param $parent_node - The parent node to add the content
 * @param $user_arguments - The user given argument array 
 * @param $user_obj - object with user data..
 * @param $ns_prefix
 */
function wsf_create_payload_for_unknown_class_map(DomDocument $payload_dom,
                                     DomNode $parent_node,
                                     $user_obj,
                                     $ns_prefix) {

    $reflex_obj = new ReflectionObject($user_obj);
    $reflex_properties = $reflex_obj->getProperties();


    foreach($reflex_properties as $reflex_property) {
        $key = $reflex_property->getName();
        $value = $reflex_property->getValue($user_obj);

        if($ns_prefix != NULL && !empty($ns_prefix)) {
            $node_name = $ns_prefix.":".$key;
        }
        else
        {
            $node_name = $key;
        }
        if(is_object($value)) {
            $current_node = $payload_dom->createElement($node_name);
            wsf_create_payload_for_unknown_class_map($payload_dom, $current_node, $value, $ns_prefix);
            $parent_node->appendChild($current_node);
        }
        else if(wsf_is_map($value)) {
            $current_node = $payload_dom->createElement($node_name);
            wsf_create_payload_for_unknown_array($payload_dom, $current_node, $value, $ns_prefix);
            $parent_node->appendChild($current_node);
        }
        else if(is_array($value)) {
            foreach($value as $child_value) {
                if(is_object($child_value)) {
                    $current_node = $payload_dom->createElement($node_name);
                    wsf_create_payload_for_unknown_class_map($payload_dom, $current_node, $child_value, $ns_prefix);
                    $parent_node->appendChild($current_node);
                }
                else if(wsf_is_map($child_value)) {
                    $current_node = $payload_dom->createElement($node_name);
                    wsf_create_payload_for_unknown_array($payload_dom, $current_node, $child_value, $ns_prefix);
                    $parent_node->appendChild($current_node);
                }
                else if(is_array($child_value)) {
                    error_log("Invalid array (inside an array) supplied around ".print_r($child_value, TRUE)." \n");
                }
                else
                {
                    $current_node = $payload_dom->createElement($node_name, $child_value);
                    $parent_node->appendChild($current_node);
                }
            }
        }
        else
        {
            $current_node = $payload_dom->createElement($node_name, $value);
            $parent_node->appendChild($current_node);
        }
    }
}



/** create payload for arrays
 * @param $payload_dom - DomDocument for the payload building
 * @param $parent_node - The parent node to add the content
 * @param $user_arguments - The user given argument array 
 * @param $ns_prefix
 */
function wsf_create_payload_for_unknown_array(DomDocument $payload_dom,
                                     DomNode $parent_node,
                                     $user_arguments,
                                     $ns_prefix) {
    if(is_array($user_arguments)) {
        foreach($user_arguments as $key=> $value) {
            if($ns_prefix != NULL && !empty($ns_prefix)) {
                $node_name = $ns_prefix.":".$key;
            }
            else
            {
                $node_name = $key;
            }
            if(is_object($value)) {
                $current_node = $payload_dom->createElement($node_name);
                wsf_create_payload_for_unknown_class_map($payload_dom, $current_node, $value, $ns_prefix);
                $parent_node->appendChild($current_node);
            }
            else if(wsf_is_map($value)) {
                $current_node = $payload_dom->createElement($node_name);
                wsf_create_payload_for_unknown_array($payload_dom, $current_node, $value, $ns_prefix);
                $parent_node->appendChild($current_node);
            }
            else if(is_array($value)) {
                foreach($value as $child_value) {
                    if(is_object($child_value)) {
                        $current_node = $payload_dom->createElement($node_name);
                        wsf_create_payload_for_unknown_class_map($payload_dom, $current_node, $child_value, $ns_prefix);
                        $parent_node->appendChild($current_node);
                    }
                    else if(wsf_is_map($child_value)) {
                        $current_node = $payload_dom->createElement($node_name);
                        wsf_create_payload_for_unknown_array($payload_dom, $current_node, $child_value, $ns_prefix);
                        $parent_node->appendChild($current_node);
                    }
                    else if(is_array($child_value)) {
                        error_log("Invalid array (inside an array) supplied around ".print_r($child_value, TRUE)." \n");
                    }
                    else
                    {
                        $current_node = $payload_dom->createElement($node_name, $child_value);
                        $parent_node->appendChild($current_node);
                    }
                }
            }
            else
            {
                $current_node = $payload_dom->createElement($node_name, $value);
                $parent_node->appendChild($current_node);
            }
        }
    }
    else {
        return $user_arguments; // just return the current value;
    }
}

/** 
 * Do serialization over simple schema types
 * @param $sig_param_node sig model for the param
 * @param $user_val the user value given to the type
 * @param $parant_ele just root element of the element
 * @param $payload_dom  payload dom document
 * @param $root_node root element of the document, in order to add namespace 
 * @param $prefix_i - next available prefix index 
 * @param namespace_map, array, map to the namespace to prefix
 */
function wsf_serialize_simple_types(DomNode $sig_param_node, $user_val,
                    $parent_node, $payload_dom, $root_node, &$prefix_i, &$namespace_map) {

    $target_namespace = NULL;
    $min_occurs = 1;
    $max_occurs = 1;
    $sig_param_attris = $sig_param_node->attributes;
    $param_type = NULL;
    $param_name = NULL;

    if($sig_param_attris->getNamedItem(WSF_NAME)) {
        $param_name = 
            $sig_param_attris->getNamedItem(WSF_NAME)->value;
    }

    if($sig_param_attris->getNamedItem(WSF_TARGETNAMESPACE)) {
         $target_namespace = 
            $sig_param_attris->getNamedItem(WSF_TARGETNAMESPACE)->value;
    }

    if($sig_param_attris->getNamedItem(WSF_MIN_OCCURS)) {
         $min_occurs = 
            $sig_param_attris->getNamedItem(WSF_MIN_OCCURS)->value;
    }

    if($sig_param_attris->getNamedItem(WSF_MAX_OCCURS)) {
         $max_occurs = 
            $sig_param_attris->getNamedItem(WSF_MAX_OCCURS)->value;
    }
    
    if($sig_param_attris->getNamedItem(WSF_TYPE)) {
         $param_type = 
            $sig_param_attris->getNamedItem(WSF_TYPE)->value;
    }
    
    if($target_namespace == NULL) {
        $node_name = $param_name;
    }
    else{
        if(array_key_exists($target_namespace, $namespace_map)
                     && $namespace_map[$target_namespace] != NULL) {
            $prefix = $namespace_map[$target_namespace];
        }
        else{
            $prefix = "ns".$prefix_i;
            $prefix_i ++;
            $root_node->setAttribute("xmlns:".$prefix, $target_namespace);
            $namespace_map[$target_namespace] = $prefix;
        }
        $node_name = $prefix.":".$param_name;
    }
    
    if($max_occurs > 1 || $max_occurs == "unbounded") {
        if(is_array($user_val)) {
            foreach($user_val as $user_val_item) {
                /* type conversion is needed */
                $serialized_value = wsf_wsdl_util_serialize_php_value(
                             $param_type, $user_val_item);
                $ele = $payload_dom->createElement($node_name, $serialized_value);
                $parent_node->appendChild($ele);
            }
        }
        else {
            /* in a case this is not an array */
            $serialized_value = wsf_wsdl_util_serialize_php_value(
                             $param_type, $user_val);
            $ele = $payload_dom->createElement($node_name, $serialized_value);
            $parent_node->appendChild($ele);
        }
    }
    else {
        if(!is_array($user_val)) {
            /* in a case this is not an array */
            $serialized_value = wsf_wsdl_util_serialize_php_value(
                             $param_type, $user_val);
            $ele = $payload_dom->createElement($node_name, $serialized_value);
            $parent_node->appendChild($ele);
        }
        else {
            error_log("Array is given for ". $param_name ." which should be a non array. \n");
            ws_log_write(__FILE__, __LINE__, WSF_LOG_ERROR,
                "Array is given for ". $param_name ." which should be a non array.");
        }
    }
}

/** 
 * Do serialization over complex schema types
 * @param $sig_param_node sig model for the param
 * @param $user_val the user value given to the type
 * @param $parant_ele just root element of the element
 * @param $payload_dom  payload dom document
 * @param $root_node root element of the document, in order to add namespace 
 * @param $prefix_i - next available prefix index 
 * @param namespace_map, array, map to the namespace to prefix
 */
function wsf_serialize_complex_types(DomNode $sig_param_node, $user_val,
                                $parent_node, $payload_dom, $root_node,
                                &$prefix_i, &$namespace_map) {

    $target_namespace = NULL;
    $min_occurs = 1;
    $max_occurs = 1;
    $sig_param_attris = $sig_param_node->attributes;
    $param_type = NULL;
    $param_name = NULL;

    if($sig_param_attris->getNamedItem(WSF_NAME)) {
        $param_name = 
            $sig_param_attris->getNamedItem(WSF_NAME)->value;
    }

    if($sig_param_attris->getNamedItem(WSF_TARGETNAMESPACE)) {
        $target_namespace = 
            $sig_param_attris->getNamedItem(WSF_TARGETNAMESPACE)->value;
    }

    if($sig_param_attris->getNamedItem(WSF_MIN_OCCURS)) {
        $min_occurs = 
            $sig_param_attris->getNamedItem(WSF_MIN_OCCURS)->value;
    }

    if($sig_param_attris->getNamedItem(WSF_MAX_OCCURS)) {

        $max_occurs = 
            $sig_param_attris->getNamedItem(WSF_MAX_OCCURS)->value;
    }
    
    if($sig_param_attris->getNamedItem(WSF_TYPE)) {
        $param_type = 
            $sig_param_attris->getNamedItem(WSF_TYPE)->value;
    }
    
    if($target_namespace == NULL) {
        $node_name = $param_name;
    }
    else {
        if(array_key_exists($target_namespace, $namespace_map)
                     && $namespace_map[$target_namespace] != NULL) {
            $prefix = $namespace_map[$target_namespace];
        }
        else{
            $prefix = "ns".$prefix_i;
            $prefix_i ++;
            $root_node->setAttribute("xmlns:".$prefix, $target_namespace);
            $namespace_map[$target_namespace] = $prefix;
        }
        $node_name = $prefix.":".$param_name;
    }

    if($max_occurs > 1 || $max_occurs == "unbounded") {
        if(is_array($user_val)) {
            foreach($user_val as $user_val_item) {
                /* type conversion is needed */
                $param_node = $payload_dom->createElement($node_name);
                if(is_object($user_val_item)) {
                    if($sig_param_node->hasChildNodes()) {
                        wsf_create_payload_for_class_map($payload_dom, 
                                    $sig_param_node, $param_node, $root_node,
                                    $user_val_item, $prefix_i, $namespace_map);
                    }
                    else
                    {
                        wsf_create_payload_for_unknown_class_map($payload_dom,
                                    $param_node, $user_val_item, $prefix);
                    }
                }
                else
                {
                    if($sig_param_node->hasChildNodes()) {
                        wsf_create_payload_for_array($payload_dom,
                                    $sig_param_node, $param_node, $root_node,
                                    $user_val_item, $prefix_i, $namespace_map);
                    }
                    else
                    {
                        wsf_create_payload_for_unknown_array($payload_dom,
                                    $param_node, $user_val_item, $prefix);
                    }
                }
                $parent_node->appendChild($param_node);
            }
        }
        else {
            /* in a case this is not an array */

            $param_node = $payload_dom->createElement($node_name);
            if(is_object($user_val)) {
                if($sig_param_node->hasChildNodes()) {
                    wsf_create_payload_for_class_map($payload_dom, 
                                $sig_param_node, $param_node, $root_node,
                                $user_val, $prefix_i, $namespace_map);
                }
                else
                {
                    wsf_create_payload_for_unknown_class_map($payload_dom,
                                $param_node, $user_val, $prefix);
                }
            }
            else
            {
                if($sig_param_node->hasChildNodes()) {
                    wsf_create_payload_for_array($payload_dom,
                                $sig_param_node, $param_node, $root_node,
                                $user_val, $prefix_i, $namespace_map);
                }
                else
                {
                    wsf_create_payload_for_unknown_array($payload_dom,
                                $param_node, $user_val, $prefix);
                }
            }
            $parent_node->appendChild($param_node);
        }
    }
    else {
        /* in a case this is not an array */

        $param_node = $payload_dom->createElement($node_name);
        if(is_object($user_val)) {
            if($sig_param_node->hasChildNodes()) {
                wsf_create_payload_for_class_map($payload_dom, 
                            $sig_param_node, $param_node, $root_node,
                            $user_val, $prefix_i, $namespace_map);
            }
            else
            {
                wsf_create_payload_for_unknown_class_map($payload_dom,
                            $param_node, $user_val, $prefix);
            }
        }
        else
        {
            if($sig_param_node->hasChildNodes()) {
                wsf_create_payload_for_array($payload_dom,
                            $sig_param_node, $param_node, $root_node,
                            $user_val, $prefix_i, $namespace_map);
            }
            else
            {
                wsf_create_payload_for_unknown_array($payload_dom,
                            $param_node, $user_val, $prefix);
            }
        }
        $parent_node->appendChild($param_node);
    }
}

/**
 * build the content model correctly
 * @param $sig_node sig model to be processed..
 * @param $user_arguments, user arguments as an array
 * @param $parent_node the immediate parent element
 * @param $payload_dom the dom document of the building xml
 * @param $prefix_i - next available prefix index 
 * @param $root_node the root element of the dom
 */
function wsf_build_content_model(DomNode $sig_node, array $user_arguments,
            DomNode $parent_node, DomNode $payload_dom, $root_node, &$prefix_i, &$namespace_map) {

    $content_model = WSF_WSDL_SEQUENCE;
    if($sig_node->attributes->getNamedItem(WSF_CONTENT_MODEL)) {
        $content_model = $sig_node->attributes->getNamedItem(WSF_CONTENT_MODEL)->value;
    }

    $just_found_once = FALSE;
    
    $sig_param_nodes = $sig_node->childNodes;
    foreach($sig_param_nodes as $sig_param_node) {
        if($sig_param_node->nodeName == WSF_PARAM) {
            $param_name = $sig_param_node->attributes->getNamedItem(WSF_NAME)->value;

            // users are not expected to provide it in exact sequence..
            // for both all and sequences we build the xml same order as in the schema
            // for choice we pick the first non-null value in the group, when it is in the order defined in the schema
            foreach($user_arguments as $user_key => $user_val) {
                if($param_name == $user_key) {
                    if ($sig_param_node->attributes->getNamedItem(WSF_WSDL_SIMPLE)->value == "yes") {
                        wsf_serialize_simple_types($sig_param_node,
                                        $user_val, $parent_node, $payload_dom, 
                                        $root_node, $prefix_i, $namespace_map);
                    }
                    else {
                        wsf_serialize_complex_types($sig_param_node, 
                                        $user_val, $parent_node, $payload_dom,
                                        $root_node, $prefix_i, $namespace_map);
                    }
                    if($user_val !== NULL) {
                        $just_found_once = TRUE;
                    }
                    break;
                }
            }
            if($just_found_once && $content_model == "choice"){
                break;
            }
        }
        else if($sig_param_node->nodeName == WSF_INNER_CONTENT) {
            wsf_build_content_model($sig_param_node, $user_arguments, $parent_node,
                    $payload_dom, $root_node, $prefix_i, $namespace_map);
        }
    }
}

/**
 * convert user object to user arguments array, right now just for one level
 * @param $sig_node DomNode sig model
 * @param $user_obj
 * @return resulting argument array
 */
function wsf_convert_classobj_to_array($sig_node, $user_obj) {

    $user_arguments = array();

    $sig_param_nodes = $sig_node->childNodes;

    foreach($sig_param_nodes as $sig_param_node) {
        if($sig_param_node->nodeName == WSF_PARAM) {
            $param_attrs = $sig_param_node->attributes;
            if($param_attrs->getNamedItem(WSF_NAME)) {
                $param_name = $param_attrs->getNamedItem(WSF_NAME)->value;
            }
            if($user_obj->$param_name !== NULL) {
                $user_arguments[$param_name] = $user_obj->$param_name;
            }
        }
        else if($sig_param_node->nodeName == WSF_INNER_CONTENT) {
            $tmp_array = wsf_convert_classobj_to_array($sig_param_node, $user_obj);
            $user_arguments = array_merge($user_arguments, $tmp_array);
        }
    }

    return $user_arguments;
}

//-------------------------------------------------------------------------------------------
//currently we don't have diffent serialization for rpc-style in contrast with doc-lit

?>
