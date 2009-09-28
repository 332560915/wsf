/*
* Copyright 2005-2009 WSO2, Inc. http://wso2.com
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


#include <ServiceSkeleton.h>
#include <stdio.h>
#include <axutil_env.h>
#include <Environment.h>
#include "Echo.h"

using namespace wso2wsf;
/** Load the service into axis2 engine */
WSF_SERVICE_INIT(Echo)

OMElement* Echo::echoString(OMElement *msg)
{
	OMElement *echoEle = new OMElement(msg->getLocalname(), new OMNamespace(msg->getNamespace()->getURI(),msg->getNamespace()->getPrefix()));
	OMElement *text = new OMElement("text");
	echoEle->addChild(text);
	text->setText("Hello World");
	return echoEle;
}

OMElement* Echo::invoke(OMElement *ele, MessageContext *msgCtx)
{
	/** Since this is the only method in the service, it can be directly invoked. Please refer to the math
	 * sample to check how to selectively invoke a mehtod */
	return echoString(ele);
}

OMElement* Echo::onFault(OMElement *ele)
{
	OMElement *responseEle = new OMElement("EchoServiceErrorResponse");
	responseEle->setText("Echo Service Failed");
	return responseEle;
}

