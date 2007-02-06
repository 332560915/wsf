<?php
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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


  class WS_WsdlBinding
   {


       const WS_WSDL2_BINDING_VERSION_ATTR_VAL = "1.2";

       private $svr_name;
       public  $operations;
       private $wsdl_location;

       function __construct($service_name, $wsdl_ep, $operations = false)
       {
	   $this->svr_name = $service_name;
	   $this->operations = $operations;
	   $this->wsdl_location = $wsdl_ep;
       }


      public function createDocLitBinding(DomDocument $binding_doc, DomElement $binding_root)
      {
          $binding_ele = $binding_doc->createElementNS(WS_WsdlConst::WS_SCHEMA_WSDL_NAMESPACE,
				      WS_WsdlConst::WS_WSDL_BINDING_ATTR_NAME);
	  $binding_ele->setAttribute(WS_WsdlConst::WS_WSDL_NAME_ATTR_NAME, $this->svr_name);
	  $binding_ele->setAttribute(WS_WsdlConst::WS_WSDL_TYPE_ATTR_NAME, 
				     $this->svr_name);
	  
	  $s_binding = $binding_doc->createElementNS(WS_WsdlConst::WS_SCHEMA_SOAP_NAMESPACE,
					     WS_WsdlConst::WS_WSDL_BINDING_ATTR_NAME);
	  $s_binding->setAttribute(WS_WsdlConst::WS_WSDL_TRANSPORT_ATTR_NAME, 
				   WS_WsdlConst::WS_SCHEMA_SOAP_HTTP_NAMESPACE);
	  $s_binding->setAttribute(WS_WsdlConst::WS_WSDL_STYLE_ATTR_NAME,
				   WS_WsdlConst::WS_WSDL_DOCUMENT_ATTR_NAME);
	  $binding_ele->appendChild($s_binding);
	  
	  foreach($this->operations as $name => $params) 
	  {
	      $op = $binding_doc->createElementNS(WS_WsdlConst::WS_SCHEMA_WSDL_NAMESPACE,
					  WS_WsdlConst::WS_WSDL_OPERATION_ATTR_NAME);
	      $op->setAttribute(WS_WsdlConst::WS_WSDL_NAME_ATTR_NAME, $name);
	      $action_ele = $binding_doc->createElementNS(WS_WsdlConst::WS_SCHEMA_SOAP_NAMESPACE,
							  WS_WsdlConst::WS_WSDL_OPERATION_ATTR_NAME);
	      $action_ele->setAttribute(WS_WsdlConst::WS_WSDL_SOAP_ACTION_ATTR_NAME,
					WS_WsdlConst::WS_WSDL_HTTP_ATTR_NAME.$this->wsdl_location.
					"/".$name);
	      $action_ele->setAttribute(WS_WsdlConst::WS_WSDL_STYLE_ATTR_NAME,
					WS_WsdlConst::WS_WSDL_DOCUMENT_ATTR_NAME);
	      $op->appendChild($action_ele);
	      

	      foreach(array(WS_WsdlConst::WS_WSDL_INPUT_ATTR_NAME,
			    WS_WsdlConst::WS_WSDL_OUTPUT_ATTR_NAME) as $type) 
	      {
		  $sbinding_ele = $binding_doc->createElementNS(WS_WsdlConst::WS_SCHEMA_WSDL_NAMESPACE, $type);
		  $s_body = $binding_doc->createElementNS(WS_WsdlConst::WS_SCHEMA_SOAP_NAMESPACE,
						  WS_WsdlConst::WS_WSDL_BODY_ATTR_NAME);
		  $s_body->setAttribute(WS_WsdlConst::WS_WSDL_USE_ATTR_NAME, 
					WS_WsdlConst::WS_WSDL_LITERAL_ATTR_NAME);
		  $sbinding_ele->appendChild($s_body);
		  $op->appendChild($sbinding_ele);
	      }
	      $binding_ele->appendChild($op);
	  }
	  $binding_root->appendChild($binding_ele);
      }




       public function createRPCBinding(DomDocument $binding_doc, DomElement $binding_root)
       {
	   $binding_ele = $binding_doc->createElementNS(WS_WsdlConst::WS_SCHEMA_WSDL_NAMESPACE,
							WS_WsdlConst::WS_WSDL_BINDING_ATTR_NAME);
	   $binding_ele->setAttribute(WS_WsdlConst::WS_WSDL_NAME_ATTR_NAME, $this->svr_name);
	   $binding_ele->setAttribute(WS_WsdlConst::WS_WSDL_TYPE_ATTR_NAME,
				      $this->svr_name);
	   
	   $s_binding = $binding_doc->createElementNS(WS_WsdlConst::WS_SCHEMA_SOAP_NAMESPACE,
						      WS_WsdlConst::WS_WSDL_BINDING_ATTR_NAME);
	   $s_binding->setAttribute(WS_WsdlConst::WS_WSDL_STYLE_ATTR_NAME, 
				    WS_WsdlConst::WS_WSDL_RPC_ATTR_NAME);
	   $s_binding->setAttribute(WS_WsdlConst::WS_WSDL_TRANSPORT_ATTR_NAME,
				    WS_WsdlConst::WS_SCHEMA_SOAP_HTTP_NAMESPACE);
	   $binding_ele->appendChild($s_binding);

	   foreach($this->operations as $name => $params)
	   {
	       $op = $binding_doc->createElementNS(WS_WsdlConst::WS_SCHEMA_WSDL_NAMESPACE,
						   WS_WsdlConst::WS_WSDL_OPERATION_ATTR_NAME);
	       $op->setAttribute(WS_WsdlConst::WS_WSDL_NAME_ATTR_NAME, $name);
	       foreach(array(WS_WsdlConst::WS_WSDL_INPUT_ATTR_NAME,
			     WS_WsdlConst::WS_WSDL_OUTPUT_ATTR_NAME) as $type)
	       {
		   $sbinding_ele = $binding_doc->createElementNS(WS_WsdlConst::WS_SCHEMA_WSDL_NAMESPACE, $type);
		   $s_body = $binding_doc->createElementNS(WS_WsdlConst::WS_SCHEMA_SOAP_NAMESPACE,
							   WS_WsdlConst::WS_WSDL_BODY_ATTR_NAME);
		   $s_body->setAttribute(WS_WsdlConst::WS_WSDL_USE_ATTR_NAME,
					 WS_WsdlConst::WS_WSDL_ENCODED_ATTR_NAME);
		   $s_body->setAttribute(WS_WsdlConst::WS_WSDL_ENCOD_STYLE_ATTR_NAME,
					 WS_WsdlConst::WS_SOAP_SCHEMA_ENCODING_NAMESPACE);
		   $sbinding_ele->appendChild($s_body);
		   $op->appendChild($sbinding_ele);
	       }
	       $binding_ele->appendChild($op);
	   }
	   $binding_root->appendChild($binding_ele);
       }


       public function createWsdl2Binding(DomDocument $binding_doc, DomElement $binding_root)
      {
          $binding_ele = $binding_doc->createElementNS(WS_WsdlConst::WS_WSDL2_NAMESPACE,
				      WS_WsdlConst::WS_WSDL_BINDING_ATTR_NAME);
	  $binding_ele->setAttribute(WS_WsdlConst::WS_WSDL_NAME_ATTR_NAME, 
				     $this->svr_name.strtoupper(WS_WsdlConst::WS_WSDL_SOAP_ATTR_NAME).
				     ucfirst(WS_WsdlConst::WS_WSDL_BINDING_ATTR_NAME));
	  $binding_ele->setAttribute(WS_WsdlConst::WS_WSDL_TYPE_ATTR_NAME, 
				     WS_WsdlConst:: WS_WSDL2_WSOAP_ATTR_VAL);
	  
	  $binding_ele->setAttribute(WS_WsdlConst::WS_WSDL_INTERFACE_ATTR_NAME,
				    $this->svr_name.ucfirst(WS_WsdlConst::WS_WSDL_INTERFACE_ATTR_NAME));
	  $binding_ele->setAttribute(WS_WsdlConst::WS_WSDL_VERSION_ATTR_NAME,
				     self::WS_WSDL2_BINDING_VERSION_ATTR_VAL);
	  $binding_ele->setAttribute(WS_WsdlConst::WS_WSDL_PROTOCAL_ATTR_NAME,
				     WS_WsdlConst:: WS_WSDL2_BINDING_ATTR_VAL);

	  $binding_root->appendChild($binding_ele);
      }



       
}

?>