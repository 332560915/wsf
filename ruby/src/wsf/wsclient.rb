# Copyright 2005,2006,2007 WSO2, Inc. http://wso2.com
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

require 'WSFC'
require 'wsf/wsmessage'
require 'wsf/wsfault'
require 'rexml/document'

class WSClient

  # Create a new WSClient instance.
  # All instance level initialization is done here.
  
  def initialize(options, axis2c_home = WSFC::AXIS2C_HOME, log_file_name = WSFC::DEFAULT_LOG_FILE)
	@svc_client = nil
	@options = nil

	# Create Environment
    @env = WSFC::axutil_env_create_all(log_file_name, WSFC::AXIS2_LOG_LEVEL_TRACE)
    if @env.nil? then
      puts "[critical][wsf-ruby] Failed to create WSF/C environment..."
      return
    end

    # Check if axis2c_home is valid
    if !axis2c_home.kind_of?(String) or axis2c_home.empty? then
      WSFC::axis2_log_critical(@env, "[wsf-ruby] 'axis2c_home' is invalid")
      return
    end

    # Create service client
    @svc_client = WSFC::axis2_svc_client_create(@env, axis2c_home)
    if @svc_client.nil? then
      WSFC::axis2_log_critical(@env, "[wsf-ruby] Failed to create service client")
      return
    end
   
	@options = Hash.new
    options.each_pair {|k,v| @options.store(k,v)} if options.kind_of? Hash

	# Set client level options
	client_options = WSFC::axis2_svc_client_get_options(@svc_client, @env)
	
	if client_options.nil? then
      client_options = WSFC::axis2_options_create(@env)
	  WSFC::axis2_svc_client_set_options(@svc_client, @env, client_options)
	end

	set_client_options(client_options)
  end
  
  # This method is used to set client level settings according to 
  # the options specified when the client is created.
  
  def set_client_options(client_options)
	# Proxy settings
    WSFC::axis2_svc_client_set_proxy(@svc_client,
                                     @env,
                                     @options["proxy_host"].to_s,
                                     @options["proxy_port"].to_s) if @options.has_key?("proxy_host") and @options.has_key?("proxy_port")

    # SOAP settings
    use_soap = @options.has_key?("use_soap") ? @options["use_soap"].to_s.upcase : "TRUE"

    if use_soap.eql? "FALSE" then # REST style
      WSFC::axis2_options_set_enable_rest(client_options, @env, WSFC::AXIS2_TRUE)
    else # SOAP style
      soap_version = use_soap.eql?("1.1") ? WSFC::AXIOM_SOAP11 : WSFC::AXIOM_SOAP12
      WSFC::axis2_options_set_soap_version(client_options, @env, soap_version)
    end
    
    # HTTP method
    http_method = @options.has_key?("http_method") ? @options["http_method"].to_s.upcase : "POST"

    if http_method.eql? "GET" then
      WSFC::axis2_options_set_http_method(client_options, @env, WSFC::AXIS2_HTTP_GET)
    end
    
    # SSL settings
    ca_cert = @options.has_key?("ca_cert") ? @options["ca_cert"].to_s : ""
    client_cert = @options.has_key?("client_cert") ? @options["client_cert"].to_s : ""
    pass_phrase = @options.has_key?("pass_phrase") ? @options["pass_phrase"].to_s : ""

    ca_cert_property = WSFC::ruby_axutil_property_create_with_args(@env,
																   WSFC::AXIS2_SCOPE_APPLICATION,
																   WSFC::AXIS2_TRUE,
                                                                   WSFC::ruby_axutil_strdup(@env, ca_cert))
    
    client_cert_property = WSFC::ruby_axutil_property_create_with_args(@env,
																	   WSFC::AXIS2_SCOPE_APPLICATION,
																	   WSFC::AXIS2_TRUE,
                                                                       WSFC::ruby_axutil_strdup(@env, client_cert))
    
    pass_phrase_property = WSFC::ruby_axutil_property_create_with_args(@env,
																	   WSFC::AXIS2_SCOPE_APPLICATION,
																	   WSFC::AXIS2_TRUE,
                                                                       WSFC::ruby_axutil_strdup(@env, pass_phrase))
    
    WSFC::ruby_axis2_options_set_property(client_options,
                                          @env,
                                          "SERVER_CERT",
                                          ca_cert_property)

    WSFC::ruby_axis2_options_set_property(client_options,
                                          @env,
                                          "KEY_FILE",
                                          client_cert_property)

    WSFC::ruby_axis2_options_set_property(client_options,
                                          @env,
                                          "SSL_PASSPHRASE",
                                          pass_phrase_property)
  end

  # This method is used to set transaction level settings.
  # This is called for every request/send call.
  
  def set_transaction_options(message, client_options)
    WSFC::axis2_options_set_xml_parser_reset(client_options, @env, WSFC::AXIS2_FALSE)
	
	# SOAP settings
    use_soap = @options.has_key?("use_soap") ? @options["use_soap"].to_s.upcase : "TRUE"

    if !use_soap.eql?("FALSE") then # SOAP style
      action = message_property("action", message).to_s
      begin
        soap_action = WSFC::axutil_string_create(@env, action)
        WSFC::axis2_options_set_soap_action(client_options, @env, soap_action)
      end unless action.empty?
    end
  end

  # This method is used to do a blocking request call.
  # message can be an XML string, a REXML object or a WSMessage.
  # A WSFault is thown if an error occurs while a message is being sent.

  def request(message)
    if @svc_client.nil? then
      WSFC::axis2_log_error(@env, "[wsf-ruby] Service client not created")
      return nil
    end
   
    client_options = WSFC::axis2_svc_client_get_options(@svc_client, @env)
    
	# Set end point 
    to = message_property("to", message).to_s
    if to.empty? then
      WSFC::axis2_log_error(@env, "[wsf-ruby] End point not specified")
      return nil
    end
    
    to_end_point_ref = WSFC::axis2_endpoint_ref_create(@env, to)
    WSFC::axis2_options_set_to(client_options, @env, to_end_point_ref)
   
    # Create request payload 
    request_axiom_payload = message_to_axiom_node(message)
    if request_axiom_payload.nil? then
      WSFC::axis2_log_error(@env, "[wsf-ruby] Failed to create a valid AXIOM payload for request")
      return nil
    end
   
	# Set transaction level options
    set_transaction_options(message, client_options)

    # Handle Security
    handle_security

    # Handle Addressing options
    addressing_engaged = handle_addressing(message, client_options)

	# Handle RM options
    handle_reliable_messaging(message, client_options, addressing_engaged, false)

    # Handle outgoing attachments
    handle_outgoing_attachments(message, client_options, request_axiom_payload)
 
    response_axiom_payload = WSFC::axis2_svc_client_send_receive(@svc_client, @env, request_axiom_payload)

    if WSFC::axis2_svc_client_get_last_response_has_fault(@svc_client, @env) == WSFC::AXIS2_TRUE then # SOAP fault occurred
      last_soap_fault_e = last_soap_fault_exception
      
      if last_soap_fault_e.nil? then
        raise WSFault.new("SOAP-FAULT-ERROR", "Malformatted SOAP fault message")
      else
        raise last_soap_fault_e
      end
    else
      if response_axiom_payload.nil? then # No response from the server
        raise WSFault.new("NULL-REPLY", "No response from the server")
      else
        return axiom_node_to_message(response_axiom_payload)
      end
    end
  end

  # This method is used to do a send call.
  # message can be an XML string, a REXML object or a WSMessage.
  # A WSFault is thown if an error occurs while a message is being sent.
 
  def send(message)
    if @svc_client.nil? then
      WSFC::axis2_log_error(@env, "[wsf-ruby] Service client not created")
      return
    end
  
	client_options = WSFC::axis2_svc_client_get_options(@svc_client, @env)

    # Set end point 
    to = message_property("to", message).to_s
    if to.empty? then
      WSFC::axis2_log_error(@env, "[wsf-ruby] End point not specified")
      return
    end
    
    to_end_point_ref = WSFC::axis2_endpoint_ref_create(@env, to)
    WSFC::axis2_options_set_to(client_options, @env, to_end_point_ref)
    
    # Create request payload 
    request_axiom_payload = message_to_axiom_node(message)
    if request_axiom_payload.nil? then
      WSFC::axis2_log_error(@env, "[wsf-ruby] Failed to create a valid AXIOM node for request")
      return
    end
   
	# Set transaction level options
    set_transaction_options(message, client_options)

    # Handle Security
    handle_security

    # Hadle Addressing options
    addressing_engaged = handle_addressing(message, client_options)

	# Handle RM options
	handle_reliable_messaging(message, client_options, addressing_engaged, true)

    # Handle outgoing attachments
    handle_outgoing_attachments(message, client_options, request_axiom_payload)

    status = WSFC::axis2_svc_client_send_robust(@svc_client, @env, request_axiom_payload)

    if WSFC::axis2_svc_client_get_last_response_has_fault(@svc_client, @env) == WSFC::AXIS2_TRUE then # SOAP fault occurred
      last_soap_fault_e = last_soap_fault_exception
      
      if last_soap_fault_e.nil? then
        raise WSFault.new("SOAP-FAULT-ERROR", "Malformatted SOAP fault message")
      else
        raise last_soap_fault_e
      end
    else
      if status == WSFC::AXIS2_FALSE then
        raise WSFault.new("SEND-ERROR", "Error occurred while sending message")
      end
    end
  end

  # This method is used to create an AXIOM node with respect to a message.
  # message can be an XML string, a REXML object or a WSMessage.
  
  def message_to_axiom_node(message)
    str_payload = ""
    
    if message.kind_of? String then
      str_payload = message
    elsif message.kind_of? REXML::Document then
      str_payload = message.to_s
    elsif message.kind_of? WSMessage then
      str_payload = message.payload_to_s
    end

    if str_payload.empty? then
      WSFC::axis2_log_error(@env, "[wsf-ruby] Payload not found")
      return nil
    end

    return WSFC::ruby_str_to_axiom_node(@env, str_payload, str_payload.length)
  end

  # This method is used to create a WSMessage with respect to a given AXIOM node
  # This is used to create the response message in a request call
  
  def axiom_node_to_message(axiom_node)
    str_payload = WSFC::ruby_axiom_node_to_str(@env, axiom_node)
	
	if str_payload.empty? then
      WSFC::axis2_log_error(@env, "[wsf-ruby] Failed to generate payload string from axiom node")
	  return nil
	end
    
    message = WSMessage.new(str_payload)

    handle_incoming_attachments(message, axiom_node)

    return message
  end

  # This method is used to get the value of the property with the given name
  # which is spcified when the client is created

  def client_property(property_name)
    return @options.has_key?(property_name) ? @options[property_name] : nil
  end

  # This method is used to get the value of property with the given name.
  # The given property can be something spcified when the client is created
  # If the message is of type WSMessage, the propery may be specified in the message
  # and in which case, that value gets the priority

  def message_property(property_name, message)
    if message.kind_of? WSMessage then
      msg_property = message.property(property_name)
      return msg_property unless msg_property.nil?
    end
    
    return @options.has_key?(property_name) ? @options[property_name] : nil
  end

  # This method is used to create a WSFault object when a SOAP fault occurs
  # It is required to throw a WSFault instance when a SOAP fault happens while
  # sending or requesting
 
  def last_soap_fault_exception
	soap_fault_base_node = WSFC::ruby_get_last_soap_fault_base_node(@svc_client, @env)
	return nil if soap_fault_base_node.nil?
   
    fault_element = WSFC::ruby_axiom_node_get_data_element(soap_fault_base_node, @env)
	child_element_ite = WSFC::axiom_element_get_child_elements(fault_element, @env, soap_fault_base_node)

	code, reason, role, detail = "", "", "", ""

	begin
      child_node = WSFC::axiom_child_element_iterator_next(child_element_ite, @env)
	  while !child_node.nil?
	    child_element = WSFC::ruby_axiom_node_get_data_element(child_node, @env)

		begin
		  localname = WSFC::axiom_element_get_localname(child_element, @env)
		  if localname.eql?("Code") then #code
		  
		    code_node = WSFC::ruby_axiom_element_get_first_node(child_element, child_node, @env)
			if !code_node.nil? then
			  
              code_element = WSFC::ruby_axiom_node_get_data_element(code_node, @env)
              code = WSFC::axiom_element_get_text(code_element, @env, code_node) unless code_element.nil?

			end

		  elsif localname.eql?("faultcode") then

		    code = WSFC::axiom_element_get_text(child_element, @env, child_node)

		  elsif localname.eql?("Reason") then #reason

		    reason_node = WSFC::ruby_axiom_element_get_first_node(child_element, child_node, @env)
			if !reason_node.nil? then

			  reason_element = WSFC::ruby_axiom_node_get_data_element(reason_node, @env)
			  reason = WSFC::axiom_element_get_text(reason_element, @env, reason_node) unless reason_element.nil?

			end

		  elsif localname.eql?("faultstring") then

		    reason = WSFC::axiom_element_get_text(child_element, @env, child_node)

		  elsif localname.eql?("Detail") or localname.eql?("detail") then #detail

            detail_node = WSFC::axiom_node_get_first_element(child_node, @env)
			detail = WSFC::axiom_node_to_string(detail_node, @env) unless detail_node.nil?

		  elsif localname.eql?("Role") or localname.eql?("faultactor") then #role

            role = WSFC::axiom_element_get_text(child_element, @env, child_node)

		  end
		end unless child_element.nil?

	    child_node = WSFC::axiom_child_element_iterator_next(child_element_ite, @env)
	  end
    end unless child_element_ite.nil?

    # xml
    xml = WSFC::axiom_node_to_string(soap_fault_base_node, @env)
    
    return WSFault.new(code, reason, role, detail, xml)
  end

  # This method is used to handle security
  
  def handle_security
     policy = client_property("policy")
     sec_token = client_property("security_token")

     return unless !policy.nil? # and !sec_token.nil?
     
     incoming_policy_node = policy.get_policy_as_axiom_node(@env)

     if not incoming_policy_node.nil?
        if WSFC::axiom_node_get_node_type(incoming_policy_node, @env) == WSFC::AXIOM_ELEMENT then
     	   root = WSFC::ruby_axiom_node_get_data_element(incoming_policy_node, @env)
           if not root.nil?
              neethi_policy = WSFC::neethi_engine_get_policy(@env, incoming_policy_node, root)
              svc = WSFC::axis2_svc_client_get_svc(@svc_client, @env)
              desc = WSFC::axis2_svc_get_base(svc, @env)
              policy_include = WSFC::axis2_desc_get_policy_include(desc, @env)
              WSFC::axis2_policy_include_add_policy_element(policy_include, @env, WSFC::AXIS2_SERVICE_POLICY, neethi_policy)
              svc_ctx = WSFC::axis2_svc_client_get_svc_ctx(@svc_client, @env)
              conf_ctx = WSFC::axis2_svc_ctx_get_conf_ctx(svc_ctx, @env)
              conf = WSFC::axis2_conf_ctx_get_conf(conf_ctx, @env)
              rampart_ctx = WSFC::rampart_context_create(@env)
              set_security_token_data_to_rampart_context(rampart_ctx, sec_token)
     	      security_param = WSFC::ruby_axutil_security_param_create(@env, WSFC::WS_RAMPART_CONFIGURATION, rampart_ctx)
 	      WSFC::axis2_conf_add_param(conf, @env, security_param)
     	   end
     	end
     end
     
     WSFC::axis2_svc_client_engage_module(@svc_client, @env, "rampart")

  end

  def set_security_token_data_to_rampart_context(rampart_context, sec_token)
    return if sec_token.nil?
    
    option = sec_token.option("private_key") 
    if not option.nil?
       if (WSFC::ruby_rampart_context_set_prv_key(rampart_context, @env, option) == WSFC::AXIS2_SUCCESS) then
          WSFC::axis2_log_debug(@env, "[wsf_sec_policy] setting pvt key  ")
       end
       if (WSFC::rampart_context_set_prv_key_type(rampart_context, @env, WSFC::AXIS2_KEY_TYPE_PEM) == WSFC::AXIS2_SUCCESS) then 
          WSFC::axis2_log_debug(@env, "[wsf_sec_policy] setting pvt key format ")
       end
    end
   
    option = sec_token.option("certificate")
    if not option.nil?
       if (WSFC::ruby_rampart_context_set_certificate(rampart_context, @env, option) == WSFC::AXIS2_SUCCESS) then
          WSFC::axis2_log_debug(@env, "[wsf_sec_policy] setting pub key  ")
       end
       if (WSFC::rampart_context_set_certificate_type(rampart_context, @env, WSFC::AXIS2_KEY_TYPE_PEM) == WSFC::AXIS2_SUCCESS) then
          WSFC::axis2_log_debug(@env, "[wsf_sec_policy] setting pub key type ")
       end
    end

    option = sec_token.option("receiver_certificate")
    if not option.nil?
       if (WSFC::ruby_rampart_context_set_receiver_certificate(rampart_context, @env, option) == WSFC::AXIS2_SUCCESS) then
          WSFC::axis2_log_debug(@env, "[wsf_sec_policy] setting receiver pub key")
       end 
       if (WSFC::rampart_context_set_receiver_certificate_type(rampart_context, @env, WSFC::AXIS2_KEY_TYPE_PEM) == WSFC::AXIS2_SUCCESS) then
          WSFC::axis2_log_debug(@env, "[wsf_sec_policy] setting receiver pub key format")
       end
    end

    option = sec_token.option("user")
    if not option.nil?
       if (WSFC::rampart_context_set_user(rampart_context, @env, option) == WSFC::AXIS2_SUCCESS) then
          WSFC::axis2_log_debug(@env, "[wsf_sec_policy] setting username ")
       end 
    end

    option = sec_token.option("password")
    if not option.nil?
       if (WSFC::rampart_context_set_password(rampart_context, @env, option) == WSFC::AXIS2_SUCCESS) then
          WSFC::axis2_log_debug(@env, "[wsf_sec_policy] setting password ")
       end
    end

    option = sec_token.option("password_type")
    if not option.nil?
       if (WSFC::rampart_context_set_password_type(rampart_context, @env, option) == WSFC::AXIS2_SUCCESS) then
          WSFC::axis2_log_debug(@env, "[wsf_sec_policy] setting password type ")
       end
    end

    option = sec_token.option("ttl")
    if not option.nil?
       if (WSFC::rampart_context_set_ttl(rampart_context, @env, option) == WSFC::AXIS2_SUCCESS) then
          WSFC::axis2_log_debug(@env, "[wsf_sec_policy) setting ttl")
       end
    end

    #option = sec_token.option("password_callback")
    #if not option.nil?
    #   if (WSFC::ruby_rampart_context_set_pwcb_function(rampart_context, @env, nil, option) == WSFC::AXIS2_SUCCESS) then
    #      WSFC::axis2_log_debug(@env, "[wsf_sec_policy] setting callback function")
    #   end
    #end
  end  

  # This method is used to engage WS-Addressing specifications
  # All addressing specific manipulations have to be done inside this method
  
  def handle_addressing(message, client_options)
    return false unless message.kind_of? WSMessage

    use_wsa = client_property("use_wsa").to_s.upcase
    action = message_property("action", message).to_s

	status = false

    if (use_wsa.eql? "1.0" or use_wsa.eql? "SUBMISSION" or use_wsa.eql? "TRUE") and (!action.empty?) then
      # Action
      WSFC::axis2_options_set_action(client_options, @env, action)
      
      # From
      from = message_property("from", message).to_s
      begin
        from_end_point_ref = WSFC::axis2_endpoint_ref_create(@env, from)
        WSFC::axis2_options_set_from(client_options, @env, from_end_point_ref)
      end unless from.empty?
     
      # Reply_to
      reply_to = message_property("reply_to", message).to_s
      begin
        reply_to_end_point_ref = WSFC::axis2_endpoint_ref_create(@env, reply_to)
        WSFC::axis2_options_set_reply_to(client_options, @env, reply_to_end_point_ref)
      end unless reply_to.empty?
      
      # Fault_to
      fault_to = message_property("fault_to", message).to_s
      begin
        fault_to_end_point_ref = WSFC::axis2_endpoint_ref_create(@env, fault_to)
        WSFC::axis2_options_set_fault_to(client_options, @env, fault_to_end_point_ref)
      end unless fault_to.empty?
      
      WSFC::axis2_svc_client_engage_module(@svc_client, @env, WSFC::AXIS2_MODULE_ADDRESSING)

      if use_wsa.eql? "SUBMISSION" then
        property = WSFC::ruby_axutil_property_create_with_args(@env,
															   WSFC::AXIS2_SCOPE_REQUEST,
															   WSFC::AXIS2_TRUE,
                                                               WSFC::ruby_axutil_strdup(@env, WSFC::AXIS2_WSA_NAMESPACE_SUBMISSION))
        WSFC::ruby_axis2_options_set_property(client_options,
                                              @env,
                                              WSFC::AXIS2_WSA_VERSION,
                                              property)
      end

	  status = true
    end

	return status
  end

  # This method is used to handle attachments sent with a message
  # All MTOM/XOP specific manipulations have to be done inside this method

  def handle_outgoing_attachments(message, client_options, axiom_payload)
    return unless message.kind_of? WSMessage

    attachments = message_property("attachments", message)
    return if attachments.nil?
    return unless attachments.kind_of? Hash
    
    enable_mtom = client_property("use_mtom").to_s.upcase.eql?("FALSE") ? WSFC::AXIS2_FALSE : WSFC::AXIS2_TRUE
    
    default_content_type_ref = message_property("default_attachment_content_type", message)
    default_content_type = default_content_type_ref.nil? ? WSFC::DEFAULT_CONTENT_TYPE : default_content_type_ref.to_s

    WSFC::axis2_options_set_enable_mtom(client_options, @env, enable_mtom)

    pack_attachments(axiom_payload, attachments, enable_mtom, default_content_type)  
  end  

  # This method is used to pack attachments specified using the "attachments" property
  # in the outgoing payload according to cid information specified in "Include" tags

  def pack_attachments(node, attachments, enable_mtom, default_content_type)
    return if node.nil?

    if WSFC::axiom_node_get_node_type(node, @env) == WSFC::AXIOM_ELEMENT then
      
      node_element = WSFC::ruby_axiom_node_get_data_element(node, @env)
      if !node_element.nil? then

        child_element_ite = WSFC::axiom_element_get_child_elements(node_element, @env, node)
        if !child_element_ite.nil? then

          child_node = WSFC::axiom_child_element_iterator_next(child_element_ite, @env)
          attachment_done = false

          while !child_node.nil? and !attachment_done do
            child_element = WSFC::ruby_axiom_node_get_data_element(child_node, @env)

            element_localname = WSFC::axiom_element_get_localname(child_element, @env)
            if !element_localname.nil? and  WSFC::ruby_axutil_strcmp(element_localname, WSFC::AXIS2_ELEMENT_LN_INCLUDE) == WSFC::AXIS2_TRUE then
        
              element_namespace = WSFC::axiom_element_get_namespace(child_element, @env, child_node)
              if !element_namespace.nil? then
          
                namespace_uri = WSFC::axiom_namespace_get_uri(element_namespace, @env)
                if !namespace_uri.nil? and WSFC::ruby_axutil_strcmp(namespace_uri, WSFC::AXIS2_NAMESPACE_URI_INCLUDE) == WSFC::AXIS2_TRUE then
            
                  cnt_type = WSFC::axiom_element_get_attribute_value_by_name(node_element, @env, WSFC::AXIS2_ELEMENT_ATTR_NAME_CONTENT_TYPE)
                  content_type = cnt_type.nil? ? default_content_type : cnt_type

                  href = WSFC::axiom_element_get_attribute_value_by_name(child_element, @env, WSFC::AXIS2_ELEMENT_ATTR_NAME_HREF)
                  href.lstrip!
                  href.rstrip!

                  if href.length > 4 then
            
                    cid = href[4..href.length - 1]

                    content = attachments[cid]
                    if !content.nil? then
                
                      WSFC::ruby_axiom_attach_content(@env,
                                                      child_node,
                                                      node,
                                                      enable_mtom,
                                                      content_type,
                                                      content,
                                                      content.length)

                      attachment_done = true
                
                    end

                  end

                end

              end

            end

            child_node = WSFC::axiom_child_element_iterator_next(child_element_ite, @env)
          end

        end

      end
      
    end

    # Process child nodes
    child_node = WSFC::axiom_node_get_first_child(node, @env)
    while !child_node.nil? do
      pack_attachments(child_node, attachments, enable_mtom, default_content_type)

      child_node = WSFC::axiom_node_get_next_sibling(child_node, @env)
    end

  end

  # This method is used to handle attachments received with a message
  # All MTOM/XOP specific manipulations have to be done inside this method

  def handle_incoming_attachments(message, axiom_payload)
    return unless message.kind_of? WSMessage
    
    response_xop = client_property("response_xop").to_s.upcase.eql?("TRUE") ? WSFC::AXIS2_TRUE : WSFC::AXIS2_FALSE
    
    unpack_attachments(axiom_payload, message) if response_xop == WSFC::AXIS2_TRUE 
  end

  # This method is used to unpack attachments received with the payload
  # The received attachments are saved as strings against the content id's
  # The content type is saved against the content id as well

  def unpack_attachments(node, message)
    return if node.nil?

    # Process current node
    if WSFC::axiom_node_get_node_type(node, @env) == WSFC::AXIOM_TEXT then
     
      text_element = WSFC::ruby_axiom_node_get_text_element(node, @env)
      if !text_element.nil? then
        
        data_handler = WSFC::axiom_text_get_data_handler(text_element, @env)
        if !data_handler.nil? then
          
          base64_content = WSFC::ruby_axiom_data_handler_get_base64_content(data_handler, @env)
          content_type = WSFC::axiom_data_handler_get_content_type(data_handler, @env)
          cid = WSFC::axiom_text_get_content_id(text_element, @env)

	      message.add_attachment_content(cid, base64_content)
          message.add_content_type(cid, content_type)

        end
        
      end
      
    end
    
    # Process child nodes
    child_node = WSFC::axiom_node_get_first_child(node, @env)
    while !child_node.nil? do
      unpack_attachments(child_node, message)

      child_node = WSFC::axiom_node_get_next_sibling(child_node, @env)
    end
  end

  # This method is used to engage WS-ReliableMessaging specifications
  # All RM specific manipulations have to be done inside this method
  
  def handle_reliable_messaging(message, client_options, addressing_engaged, one_way)
    reliable = client_property("reliable").to_s.upcase

    if reliable.eql?("TRUE") or reliable.eql?("1.0") or reliable.eql?("1.1") then
	  
	  # Set RM version
	  rm_version = reliable.eql?("1.1") ? WSFC::WS_RM_VERSION_1_1 : WSFC::WS_RM_VERSION_1_0
	  rm_version_str = reliable.eql?("1.1") ? WSFC::WS_RM_VERSION_1_1_STR : WSFC::WS_RM_VERSION_1_0_STR

      rm_property = WSFC::ruby_axutil_property_create_with_args(@env,
																WSFC::AXIS2_SCOPE_REQUEST,
																WSFC::AXIS2_FALSE,
															    WSFC::ruby_axutil_strdup(@env, rm_version_str))

	  WSFC::ruby_axis2_options_set_property(client_options,
											@env,
											WSFC::WS_SANDESHA2_CLIENT_RM_SPEC_VERSION,
											rm_property)

	  # Engage Addressing if not engaged already
	  action = message_property("action", message).to_s
	  need_to_engage_addressing = (!addressing_engaged) and (!action.empty?)

	  WSFC::axis2_svc_client_engage_module(@svc_client, @env, WSFC::AXIS2_MODULE_ADDRESSING) if need_to_engage_addressing

	  # Engage Sandesha2
	  WSFC::axis2_svc_client_engage_module(@svc_client, @env, WSFC::AXIS2_MODULE_SANDESHA2)

	  # Set Sequence Expiry Time
	  sequence_expiry_time = client_property("sequence_expiry_time").to_s
      WSFC::ruby_set_module_param_value(@env,
										@svc_client,
										"sandesha2",
										WSFC::WS_SANDESHA2_CLIENT_INACT_TIMEOUT,
										sequence_expiry_time) unless sequence_expiry_time.empty?

	  # Set Sequence Key
	  sequence_key = client_property("sequence_key").to_s
	  sequence_key_property = WSFC::ruby_axutil_property_create_with_args(@env,
												                          WSFC::AXIS2_SCOPE_REQUEST,
												                          WSFC::AXIS2_TRUE,
												                          WSFC::ruby_axutil_strdup(@env, sequence_key))

	  WSFC::ruby_axis2_options_set_property(client_options,
											@env,
											WSFC::WS_SANDESHA2_CLIENT_SEQ_KEY,
										    sequence_key_property)

	  # Mark last message
	  last_msg = true
	  will_continue_sequence = client_property("will_continue_sequence").to_s.upcase

	  if will_continue_sequence.eql?("TRUE") and message.kind_of?(WSMessage) then
	    
          last_message = message_property("last_message", message).to_s.upcase
		  last_msg = false unless last_message.eql? "TRUE"
	  
	  end

	  if (last_msg) and (rm_version == 1) then
		  
	    last_msg_property =  WSFC::ruby_axutil_property_create_with_args(@env,
																	     WSFC::AXIS2_SCOPE_REQUEST,
																		 WSFC::AXIS2_FALSE,
																		 WSFC::ruby_axutil_strdup(@env, WSFC::AXIS2_VALUE_TRUE))

		WSFC::ruby_axis2_options_set_property(client_options,
											  @env,
											  WSFC::WS_SANDESHA2_CLIENT_LAST_MESSAGE,
											  last_msg_property)

	  end

	  if (!one_way) then

		# Set offered sequence id
        offered_sequence_id = WSFC::axutil_uuid_gen(@env)

		sequence_property = WSFC::axutil_property_create(@env)
        WSFC::ruby_axutil_property_set_value(sequence_property,
											 @env,
											 WSFC::ruby_axutil_strdup(@env, offered_sequence_id))

		WSFC::ruby_axis2_options_set_property(client_options,
											  @env,
											  WSFC::WS_SANDESHA2_CLIENT_OFFERED_SEQ_ID,
											  sequence_property)
		
		# Set time out
		response_time_out = client_property("response_time_out").to_s
		time_out = response_time_out.empty? ? WSFC::WS_SANDESHA2_CLIENT_DEFAULT_TIME_OUT : response_time_out

        time_out_property = WSFC::ruby_axutil_property_create_with_args(@env,
																        WSFC::AXIS2_SCOPE_REQUEST,
																        WSFC::AXIS2_FALSE,
                                                                        WSFC::ruby_axutil_strdup(@env, time_out))
		
		WSFC::ruby_axis2_options_set_property(client_options,
											  @env,
											  WSFC::WS_SANDESHA2_CLIENT_TIME_OUT,
											  time_out_property)

	  end
	  
	end

  end

  private :set_client_options
  private :set_transaction_options
  private :message_to_axiom_node
  private :axiom_node_to_message
  private :client_property
  private :message_property
  private :last_soap_fault_exception
  private :handle_addressing
  private :handle_outgoing_attachments
  private :pack_attachments
  private :handle_incoming_attachments
  private :unpack_attachments
  private :handle_reliable_messaging

end
