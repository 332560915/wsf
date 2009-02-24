/*
* Copyright 2005,2009 WSO2, Inc. http://wso2.com
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

#ifndef MESSAGECONTEXT_H
#define MESSAGECONTEXT_H

#include <WSFDefines.h>
#include <OMElement.h>
#include <string>
#include <axis2_msg_ctx.h>
#include <axis2_conf_ctx.h>
#include <Parameter.h>
#include <Property.h>
#include <Options.h>

/**
* @file MessageContext.h
*/

/**
* @brief namespace wso2wsf Namespace of WSO2 Web Services Framework.
*/
namespace wso2wsf
{
	/**
	* @defgroup MessageContext MessageContext
	* @ingroup wso2wsf
	* @{
	*/

	/**
	* message context captures all state information related to a message
	* invocation. It holds information on the service and operation to be invoked
	* as well as context hierarchy information related to the service and operation.
	* It also has information on transports, that are to be used in invocation. The
	* phase information is kept, along with the phase at which message invocation was
	* paused as well as the handler in the phase from which the invocation is to be
	* resumed. message context would hold the request SOAP message along the out
	* path and would capture response along the in path.
	* message context also has information on various engine specific information,
	* such as if it should be doing MTOM, REST.
	* As message context is inherited form context, it has the capability of
	* storing user defined properties.
	* @{
	*/
	class MessageContext: public AxisObject
	{
	private:
		/** pointer to the underlying axis2_msg_context */
		axis2_msg_ctx_t *_msg_ctx;
		
	public:
		 
		/**
		 * Create message Context using the axis2_msg_ctx object                                                                     
		 */
		 WSF_EXTERN MessageContext(axis2_msg_ctx_t *_msg_ctx);

		 /**
		* This method returns the WS-Addressing fault to address. Fault to address tells where to 
		* send the fault in case there is an error.
		* @return returns an string of the fault to endpoint 
		*/
		WSF_EXTERN std::string WSF_CALL getFaultTo();

		/**
		* Gets WS-Addressing from endpoint. From address tells where the request came from.
		* @return string of the from endpoint reference
		*/
		WSF_EXTERN std::string WSF_CALL getFrom();

		/**
		* Checks if there is a SOAP fault on in flow.
		* @return true if there is a fault, false otherwise
		*/
		WSF_EXTERN bool WSF_CALL getInFaultFlow();
		
		/**
		* Gets the SOAP envelope associated with the MessageContex. If the MessageContex is an In Message
		* Context, the SOAP Envelope will be the Request SOAP envelope. If the MessageContext is an out MessageContext,
		* the returned SOAP Envelope will be a response message
		* @return Pointer to an OMElement representing the SOAP Envelope
		*/
		WSF_EXTERN OMElement *WSF_CALL getSoapEnvelope();

		/**
		* Gets the SOAP envelope of the response.
		* @return Pointer to an OMElement representing the SOAP Envelope
		*/
		WSF_EXTERN OMElement* WSF_CALL getResponseSoapEnvelope();

		/**
		* Gets the fault soap envelope 
		* @return A pointer to an OMElement representing the SOAP Fault Envelope
		*/
		WSF_EXTERN OMElement* WSF_CALL getFaultSoapEnvelope();
		/**
		* Gets message ID.
		* @return message ID string corresponding to the message the message 
		* context is related to
		*/

		WSF_EXTERN std::string WSF_CALL getMessageId();

		/**
		* Gets WS-Addressing reply to endpoint as a string. Reply to address tells where 
		* the the response should be sent to.    
		* @return return a string corresponding to the reply to endpoint uri
		*/
		WSF_EXTERN std::string WSF_CALL getReplyTo();

		/**
		* Gets process fault status.
		* @return true if process fault is on, false otherwise
		*/
		WSF_EXTERN bool WSF_CALL getProcessFault();
				
		/**
		* Checks if it is on the server side that the message is being dealt 
		* with, or on the client side.
		* @return true if it is server side, false otherwise
		*/
		WSF_EXTERN bool WSF_CALL getServerSide();
		
		/**
		* Gets WS-Addressing to endpoint URL. To address tells where message should 
		* be sent to.
		* @return Returns the to Endpoint URI
		*/
		WSF_EXTERN std::string WSF_CALL getTo();

	   /**
		* Sets WS-Addressing fault to endpoint. Fault to address tells where 
		* the fault message should be sent when there is an error.
		* @param toUri To uri
		* @return true if successful, false otherwise.
		*/
		WSF_EXTERN bool WSF_CALL setFaultTo(std::string toUri);

		/**
		* Sets WS-Addressing from endpoint. From address tells where 
		* the message came from.
		* @return true on success, false otherwise
		*/
		WSF_EXTERN bool WSF_CALL setFrom(std::string toUri);

		/**
		* Sets in fault flow status. 
		* @param in_fault_flow true if there is a fault on in path, false otherwise
		* @return true if success false otherwise
		*/
		WSF_EXTERN bool WSF_CALL setInFaultFlow(bool infault);

		/**
		* Sets SOAP envelope. The fact that if it is the request SOAP envelope
		* or that of response depends on the current status represented by 
		* message context.
		* @param 
		* assumes ownership of SOAP envelope
		* @return true on success, else false
		WSF_EXTERN axis2_status_t WSF_CALL
			axis2_msg_ctx_set_soap_envelope(
			axis2_msg_ctx_t * msg_ctx,
			const axutil_env_t * env,
		struct axiom_soap_envelope *soap_envelope);
		*/

		/**
		* Sets response SOAP envelope.
		*
		
		* @param soap_envelope pointer to SOAP envelope, message context
		* assumes ownership of SOAP envelope
		* @return true on success, else false
		WSF_EXTERN axis2_status_t WSF_CALL
			axis2_msg_ctx_set_response_soap_envelope(
			axis2_msg_ctx_t * msg_ctx,
			const axutil_env_t * env,
		struct axiom_soap_envelope *soap_envelope);
		*/

		/**
		* Sets fault SOAP envelope.
		*
		
		* @param soap_envelope pointer to SOAP envelope, message context
		* assumes ownership of SOAP envelope
		* @return true on success, else false
		
		WSF_EXTERN axis2_status_t WSF_CALL
			axis2_msg_ctx_set_fault_soap_envelope(
			axis2_msg_ctx_t * msg_ctx,
			const axutil_env_t * env,
		struct axiom_soap_envelope *soap_envelope);
		*/
	
    	/**
		* Sets WS-Addressing reply to address indicating the location to which
		* the reply would be sent.
		* @param ReplyTo endpoint reference uri
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setReplyTo(std::string replyToUri);

		/**
		* Sets the bool value indicating if it is the server side or the
		* client side.
		* @param server_side true if it is server side, false if it
		* is client side
		* @return true on success, else false
		*/
		
		WSF_EXTERN bool WSF_CALL setServerSide(bool serverSide);

		/**
		* Sets WS-Addressing to address.
		* @param reference pointer to endpoint reference struct representing
		* the address where the request should be sent to. message context
		* assumes ownership of endpoint struct
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setTo(std::string toUri);

		/**
		* Gets the bool value indicating if it is required to have a new thread
		* for the invocation, or if the same thread of execution could be used.
		*
		
		* @return true if new thread is required, else false
		*/
		WSF_EXTERN bool WSF_CALL getNewThread_required();

		/**
		* Sets the bool value indicating if it is required to have a new thread
		* for the invocation, or if the same thread of execution could be used.    
		*
		
		* @param new_thread_required true if a new thread is required, 
		* else false
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setNewThreadRequired(bool newThreadRequired);
			
		/**
		* Sets WS-Addressing action. 
		*
		
		* @param action_uri WSA action URI string
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setWSAAction(std::string actionUri);

		/**
		* Gets WS-Addressing action.
		*
		
		* @return pointer to WSA action URI string
		*/
		WSF_EXTERN std::string WSF_CALL getWSAAction();

		/**
		* Sets WS-Addressing message ID.
		*
		
		* @param message_id pointer to message ID string
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setWSAMessageId(std::string messageId);

		/**
		* Gets WS-Addressing message ID. 
		*
		
		* @return WSA message ID string
		*/
		WSF_EXTERN std::string WSF_CALL getWSAMessageId();

		/**
		* Gets the bool value indicating the paused status. It is possible 
		* to pause the engine invocation by any handler. By calling this method
		* one can find out if some handler has paused the invocation.
		*
		* @return true if message context is paused, else false
		*/
		WSF_EXTERN bool WSF_CALL getPaused();

		/**
		* Sets the bool value indicating the paused status of invocation.
		*
		* @param paused paused
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setPaused(bool paused);

		/**
		* Gets the bool value indicating the keep alive status. It is possible 
		* to keep alive the message context by any handler. By calling this method
		* one can see whether it is possible to clean the message context.
		*
		
		* @return true if message context is keep alive, else false
		*/

		WSF_EXTERN bool WSF_CALL isKeepAlive();

		/**
		* Sets the bool value indicating the keep alive status of invocation.
		* By setting this one can indicate the engine not to clean the message 
		* context.
		* @param keep_alive keep alive
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setKeepAlive(bool keepAlive);

		/**
		* Gets the bool value indicating the output written status. 
		*
		* @return true if output is written, else false
		*/
		WSF_EXTERN bool WSF_CALL getOutputWritten();

		/**
		* Sets the bool value indicating the output written status. 
		*
		* @param output_written true if output is written, else false
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setOutputWritten(bool outputWritten);

		/**
		* Gets the HTTP Method that relates to the service that is 
		* related to the message context.
		*
		
		* @return HTTP Method string, returns a reference,
		* not a cloned copy
		*/
		WSF_EXTERN std::string WSF_CALL getRestHTTPMethod();

		/**
		* Sets the HTTP Method that relates to the service that is 
		* related to the message context.
		*
		* @param rest_http_method HTTP Method string, msg_ctx does not assume
		* ownership of rest_http_method.
		* @return true on success, else false
		*/
		WSF_EXTERN axis2_status_t WSF_CALL setRESTHTTPMethod(std::string HTTPMethod);

		/**
		* Gets configuration descriptor parameter with given key. This method 
		* recursively search the related description hierarchy for the parameter 
		* with given key until it is found or the parent of the description 
		* hierarchy is reached. The order of search is as follows:
		* \n
		* 1. search in operation description, if its there return
		* \n
		* 2. if the parameter is not found in operation or operation is NULL, 
		* search in service
		* \n
		* 3. if the parameter is not found in service or service is NULL search 
		* in configuration
		*
		
		* @param key parameter key  
		* @return pointer to parameter struct corresponding to the given key
		*/
		WSF_EXTERN Parameter* WSF_CALL getParameter(std::string key);

		/**
		* Gets parameters related to a named module and a given handler 
		* description. The order of searching for parameter is as follows:
		* \n
		* 1. search in module configuration stored inside corresponding operation 
		* description if its there
		* \n
		* 2. search in corresponding operation if its there
		* \n
		* 3. search in module configurations stored inside corresponding 
		* service description if its there
		* \n
		* 4. search in corresponding service description if its there
		* \n
		* 5. search in module configurations stored inside configuration
		* \n
		* 6. search in configuration for parameters
		* \n
		* 7. get the corresponding module and search for the parameters
		* \n
		* 8. search in handler description for the parameter
		
		
		* @param key parameter key
		* @param module_name name of the module
		* @param handler_desc pointer to handler description
		* @return pointer to parameter 
		
		WSF_EXTERN axutil_param_t *WSF_CALL
			axis2_msg_ctx_get_module_parameter(
			const axis2_msg_ctx_t * msg_ctx,
			const axutil_env_t * env,
			const axis2_char_t * key,
			const axis2_char_t * module_name,
			axis2_handler_desc_t * handler_desc);
		*/

		/**
		* Gets property corresponding to the given key.
		* @param key key string with which the property is stored
		* @return pointer to property struct
		*/
		WSF_EXTERN Property * WSF_CALL getProperty(std::string key);

		/**
		* Gets property value corresponding to the property given key.
		* @param property_str key string with which the property is stored
		* @return pointer to property struct
		*/
		WSF_EXTERN void* WSF_CALL getPropertyValue(std::string propStr);

		/**
		* Sets property with given key.
		*
		
		* @param key key string
		* @param value property to be stored
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setProperty(std::string key, std::string value);

		/**
		* Gets the QName of the handler at which invocation was paused.
		* @return pointer to QName of the paused handler
		*/
		WSF_EXTERN std::string WSF_CALL getPausedHandlerName();

		/**
		* Gets the name of the phase at which the invocation was paused.
		* @return name string of the paused phase.
		*/
		WSF_EXTERN std::string WSF_CALL getPausedPhaseName();

		/**
		* Sets the name of the phase at which the invocation was paused.
		* @param paused_phase_name paused phase name string
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setPausedPhaseName(std::string pausedPhaseName);

		/**
		* Gets SOAP action.
		* @return SOAP action string
		*/
		WSF_EXTERN std::string WSF_CALL getSOAPAction();

		/**
		* Sets SOAP action.
		* @param soap_action SOAP action string
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setSOAPAction(std::string action);

		/**
		* Gets the boolean value indicating if MTOM is enabled or not.
		* @return true if MTOM is enabled, else false
		*/
		WSF_EXTERN bool WSF_CALL getDoingMTOM();

		/**
		* Sets the boolean value indicating if MTOM is enabled or not.
		* @param doing_mtom true if MTOM is enabled, else false
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setDoingMTOM(bool doingMTOM);

		/**
		* Gets the boolean value indicating if REST is enabled or not.
		* @return true if REST is enabled, else false
		*/
		WSF_EXTERN bool WSF_CALL getDoingREST();

		/**
		* Sets the boolean value indicating if REST is enabled or not.
		* @param doing_rest true if REST is enabled, else false
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setDoingREST(bool doingREST);

		/**
		* Sets the boolean value indicating if REST should be done through 
		* HTTP POST or not.
		* @param do_rest_through_post true if REST is to be done with 
		* HTTP POST, else false if REST is not to be done with HTTP POST
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setDoRESTThroughPOST(bool doREST);

		/**
		* Sets the boolean value indicating if REST should be done through 
		* HTTP POST or not.
		* @return true if REST is to be done with HTTP POST, else 
		* false if REST is not to be done with HTTP POST
		*/
		WSF_EXTERN bool WSF_CALL getDoRESTThroughPOST();

		/**
		* Gets manage session bool value.
		* @return true if session is managed, else false
		*/
		WSF_EXTERN bool WSF_CALL getManageSession();

		/**
		* Sets manage session bool value.
		* @param manage_session manage session bool value
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setManageSession(bool manageSession);

		/**
		* Gets the bool value indicating the SOAP version being used either
		* SOAP 1.1 or SOAP 1.2
		* @return true if SOAP 1.1 is being used, else false if 
		* SOAP 1.2 is being used
		*/

		WSF_EXTERN bool WSF_CALL getIsSOAP11();

		/**
		* Sets the bool value indicating the SOAP version being used either
		* SOAP 1.1 or SOAP 1.2
		* @param is_soap11 true if SOAP 1.1 is being used, else 
		* false if SOAP 1.2 is being used
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setIsSOAP11(bool isSOAP11);

		/**
		* Gets the options to be used in invocation.
		* @return options pointer to options struct, message context does not 
		* assume the ownership of the struct
		*/
		WSF_EXTERN Options* WSF_CALL getOptions();

		/**
		* Gets the bool value indicating the paused status.
		* @return true if invocation is paused, else false
		*/
		WSF_EXTERN bool WSF_CALL isPaused();

		/**
		* Sets the options to be used in invocation.
		* @param options pointer to options struct, message context does not 
		* assume the ownership of the struct
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setOptions(Options options);

		/**
		* Sets the flow to be invoked.
		* @param flow int value indicating the flow
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setFlow(int flow);

		/**
		* Gets the flow to be invoked.
		* @return int value indicating the flow
		*/
		WSF_EXTERN int WSF_CALL getFlow();


		/**
		* Sets the list of supported REST HTTP Methods
		* @param supported_rest_http_methods pointer array list containing
		* the list of HTTP Methods supported. Message context does
		* assumes the ownership of the array list. Anything added to this
		* array list will be freed by the msg_ctx
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setSupportedRESTHTTPMethods(std::vector<std::string> *methods);

		/**
		* Gets the list of supported REST HTTP Methods 
		* @return pointer array list containing
		* the list of HTTP Methods supported. Message context does
		* assumes the ownership of the array list
		*/
		WSF_EXTERN  std::vector<std::string>* WSF_CALL getSupportedRESTHTTPMethods();

		/**
		* Sets current handler index, indicating which handler is currently 
		* being invoked in the execution chain
		* @param index index of currently executed handler
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setCurrentHandlerIndex(int index);

		/**
		* Gets current handler index, indicating which handler is currently 
		* being invoked in the execution chain    
		* @return index of currently executed handler
		*/
		WSF_EXTERN int WSF_CALL getCurrentHandlerIndex();

		/**
		* Gets paused handler index, indicating at which handler the execution 
		* chain was paused.
		* @return index of handler at which invocation was paused
		*/
		WSF_EXTERN int WSF_CALL getPausedHandlerIndex();

		/**
		* Sets index of the current phase being invoked.    
		* @param index index of current phase
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setCurrentPhaseIndex(int index);

		/**
		* Gets index of the current phase being invoked.    
		* @return index of current phase
		*/
		WSF_EXTERN int WSF_CALL getCurrentPhaseIndex();

		/**
		* Gets the phase at which the invocation was paused.
		* @return index of paused phase
		*/
		WSF_EXTERN int WSF_CALL getPausedPhaseIndex();

		/**
		* Gets character set encoding to be used.
		* @return true on success, else false
		*/
		WSF_EXTERN std::string WSF_CALL getCharsetEncoding();

		/**
		* Sets character set encoding to be used.
		* @param str pointer to string struct representing encoding
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setCharsetEncodingStng(std::string str);


		/**
		* Gets the integer value indicating http status_code.
		* @return status value
		*/

		WSF_EXTERN int WSF_CALL getStatusCode();


		/**
		* Sets the int value indicating http status code
		* @param status_code of the http response
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setStatusCode(int statusCode);


		/**
		* Retrieves HTTP Accept-Charset records.
		* @return HTTP Accept-Charset records associated.
		*/
		WSF_EXTERN std::vector<std::string>* WSF_CALL getHTTPAcceptCharsetRecordList();

		/**
		* Retrieves HTTP Accept-Charset records, and removes them
		* from the message context
		* @return HTTP Accept-Charset records associated.
		*/
		WSF_EXTERN std::vector<std::string>* WSF_CALL extractHTTPAcceptCharsetRecordList();

		/**
		* Sets the HTTP Accept-Charset records
		* @param accept_charset_record_list an Array List containing the
		* HTTP Accept-Charset records
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setHTTPAcceptCharsetRecordList(std::vector<std::string> *recordList);

		/**
		* Retrieves HTTP Accept-Language records.
		* @return HTTP Accept-Language records associated.
		*/
		WSF_EXTERN std::vector<std::string>* WSF_CALL getHTTPAcceptLanguageRecordList();

		/**
		* Retrieves HTTP Accept-Language records, and removes them
		* from the message context
		* @return HTTP Accept-Language records associated.
		*/
		WSF_EXTERN std::vector<std::string>* WSF_CALL extractHTTPAcceptLanguageRecordList();

		/**
		* Sets the HTTP Accept-Language records
		* @param accept_language_record_list an Array List containing the
		* HTTP Accept-Language records
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setHTTPAcceptLanguageRecordList(std::vector<std::string> *acceptLanguageRecordList);

		/**
		* Gets the Content Language used
		* @return Content Language string
		*/
		WSF_EXTERN std::string WSF_CALL getContentLanguage();

		/**
		* Sets the Content Language used
		* @param str Content Language string
		* @return true on success, else false
		*/

		WSF_EXTERN bool WSF_CALL setContentLanguage(std::string langauge);

		/**
		* Retrieves HTTP Accept records.
		* @return HTTP Accept records associated.
		*/
		WSF_EXTERN std::vector<std::string>* WSF_CALL getHTTPAcceptRecordList();

		/**
		* Retrieves HTTP Accept records, and removes them
		* from the message context
		* @return HTTP Accept records associated.
		*/
		WSF_EXTERN std::vector<std::string>* WSF_CALL extractHTTPAcceptRecordList();

		/**
		* Sets the HTTP Accept records
		* @param accept_record_list an Array List containing the
		* HTTP Accept records
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setHTTPAcceptRecordList(std::vector<std::string>* recordList);

		/**
		* Gets the transfer encoding used
		* @return Transfer encoding string
		*/
		WSF_EXTERN std::string WSF_CALL getTransferEncoding();

		/**
		* Sets the transfer encoding used
		* @param str Transfer encoding string
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setTransferEncoding(std::string encoding);

		/**
		* Gets the Transport URL
		* @return Transport URL string
		*/
		WSF_EXTERN std::string WSF_CALL getTransportURL();

		/**
		* Sets the Transport URL
		* @param str Transport URL string
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setTransportURL(std::string transportURL);

		/**
		* Gets whether there was no content in the response.
		* This will cater for a situation where the invoke
		* method in a service returns NULL when no fault has
		* occurred.
		* @return returns true if there was no content
		* occurred or false otherwise
		*/
		WSF_EXTERN bool WSF_CALL getNoContent();

		/**
		* Sets that there was no content in the response.
		* @param no_content expects true if there was no
		* content in the response or false otherwise
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setNoContent(bool noContent);

		/**
		* Gets whether an authentication failure occurred
		* @return returns true if an authentication failure
		* occurred or false if not
		*/
		WSF_EXTERN bool WSF_CALL getAuthFailed();


		/**
		* Sets whether an authentication failure occurred
		*
		
		* @param status expects true if an authentication failure
		* occurred or false if not
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setAuthFailed(bool status);

		/**
		* Gets whether HTTP Authentication is required or
		* whether Proxy Authentication is required
		*
		
		* @return returns true for HTTP Authentication
		* and false for Proxy Authentication
		*/
		WSF_EXTERN bool WSF_CALL getRequiredAuthIsHTTP();

		/**
		* Sets whether HTTP Authentication is required or
		* whether Proxy Authentication is required
		* @param is_http use true for HTTP Authentication
		* and false for Proxy Authentication
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setRequiredAuthIsHTTP(bool isHTTP);

		/**
		* Sets the authentication type
		* @param auth_type Authentication type string
		* @return true on success, else false
		*/
		WSF_EXTERN bool WSF_CALL setAuthType(std::string authType);

		/**
		* Gets the authentication type
		* @return Authentication type string
		*/
		WSF_EXTERN std::string WSF_CALL getAuthType();
		
		/*
		WSF_EXTERN axutil_array_list_t *WSF_CALL
			axis2_msg_ctx_get_mime_parts(
			axis2_msg_ctx_t * msg_ctx,
			const axutil_env_t * env);


		WSF_EXTERN void WSF_CALL
			axis2_msg_ctx_set_mime_parts(
			axis2_msg_ctx_t * msg_ctx,
			const axutil_env_t * env,
			axutil_array_list_t *mime_parts);
		*/

		/**
		* Incrementing the msg_ctx ref count. This is necessary when 
		* prevent freeing msg_ctx through op_client when it is in use 
		* as in sandesha2.
		* @return true if still in use, else false 
		*/
		WSF_EXTERN bool WSF_CALL incrementRef();
		
	};
	/** @} */

}
#endif // MESSAGECONTEXT_H