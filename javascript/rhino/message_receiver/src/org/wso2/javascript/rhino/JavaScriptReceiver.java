/*
 * Copyright 2005,2006 WSO2, Inc. http://www.wso2.org
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

package org.wso2.javascript.rhino;

import org.apache.axiom.om.OMAbstractFactory;
import org.apache.axiom.om.OMElement;
import org.apache.axiom.om.OMNamespace;
import org.apache.axiom.om.OMNode;
import org.apache.axiom.om.impl.llom.OMSourcedElementImpl;
import org.apache.axiom.soap.SOAPEnvelope;
import org.apache.axiom.soap.SOAPFactory;
import org.apache.axis2.AxisFault;
import org.apache.axis2.json.JSONDataSource;
import org.apache.axis2.json.JSONBadgerfishDataSource;
import org.apache.axis2.context.MessageContext;
import org.apache.axis2.description.AxisOperation;
import org.apache.axis2.description.AxisService;
import org.apache.axis2.description.Parameter;
import org.apache.axis2.engine.MessageReceiver;
import org.apache.axis2.i18n.Messages;
import org.apache.axis2.receivers.AbstractInOutSyncMessageReceiver;

import javax.xml.stream.XMLStreamException;
import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.net.URL;

/**
 * Class JavaScriptReceiver implements the AbstractInOutSyncMessageReceiver,
 * which, is the abstract IN-OUT MEP message receiver.
 * .
 */
public class JavaScriptReceiver extends AbstractInOutSyncMessageReceiver
        implements MessageReceiver {
    public static final String LOAD_JSSCRIPTS = "loadJSScripts";

    public void invokeBusinessLogic(MessageContext inMessage, MessageContext outMessage) throws AxisFault{
        JavaScriptEngine engine = new JavaScriptEngine();
        //Get the method, arguments and the reader from the MessageContext
        String method = null;
        try {
            method = getJSMethod(inMessage);
        } catch (XMLStreamException e) {
            throw new AxisFault(e);
        }
        Reader reader = readJS(inMessage);
            Object x = inMessage.getEnvelope();
            Object args = ((SOAPEnvelope)x).getBody().getFirstElement();
            boolean json = false;
            if (args instanceof OMSourcedElementImpl) {
                Object datasource = ((OMSourcedElementImpl) args).getDataSource();
                if (datasource instanceof JSONDataSource) {
                    args = ((JSONDataSource) datasource).getCompleteJOSNString();
                    json = true;
                } else if (datasource instanceof JSONBadgerfishDataSource) {
                    throw new AxisFault("Badgerfish Convention is not supported");
                } else {
                    throw new AxisFault("Unsupported Data Format");
                }
            }


            if (reader == null) throw new AxisFault("Unable to load JavaScript file");
            if (method == null) throw new AxisFault("Unable to read the method");

            OMNode result;

            String scripts = null;

            //Get necessary JavaScripts to be loaded from services.xml
            Parameter param = inMessage.getOperationContext().getAxisOperation().getParameter(LOAD_JSSCRIPTS);
            if (param != null) {
                scripts = (String) param.getValue();
            }

            //Get necessary JavaScripts to be loaded from axis2.xml
            param = inMessage.getConfigurationContext().getAxisConfiguration().getParameter(LOAD_JSSCRIPTS);
            if (param != null) {
                if (scripts == null) {
                    scripts = (String) param.getValue();
                } else {
                    if (!scripts.equals(param.getValue())) { // Avoides loading the same set of script files twice
                        scripts += "," + param.getValue();
                    }
                }
            }

            URL repoURL = inMessage.getConfigurationContext().getAxisConfiguration().getRepository();
            if (repoURL != null) {
                JavaScriptEngine.repo = repoURL.getPath();
            }

            if (scripts != null) {
                //Get the result from executing the javascript file
                result = engine.call(method, reader, args, scripts, json);
            } else { //Parameter loadJSScripts is not set
                //Get the result from executing the javascript file
                result = engine.call(method, reader, args, json);
            }
            if (result == null) {
                throw new AxisFault(Messages.getMessage("JavaScriptNoanswer"));
            }

            //Create the out-going message
            SOAPFactory fac;
            if (inMessage.isSOAP11()) {
                fac = OMAbstractFactory.getSOAP11Factory();
            } else {
                fac = OMAbstractFactory.getSOAP12Factory();
            }
            SOAPEnvelope envelope = fac.getDefaultEnvelope();
            OMNamespace ns;
            if(json){
                ns = fac.createOMNamespace("", "");
            } else {
                String respNs = inMessage.getServiceContext().getAxisService().getSchematargetNamespace();
                ns = fac.createOMNamespace(respNs, "res");
            }
            OMElement responseElement = fac.createOMElement(method + "Response", ns);
            responseElement.addChild(result);
            envelope.getBody().addChild(responseElement);
            outMessage.setEnvelope(envelope);
    }


    /**
     * @param inMessage
     * @return the javascript operation to invoke
     * @throws AxisFault
     * @throws XMLStreamException
     */
    public String getJSMethod(MessageContext inMessage) throws AxisFault, XMLStreamException {

        //Look at the method name. if available this should be a javascript method
        AxisOperation op = inMessage.getOperationContext().getAxisOperation();
        if (op == null) {
            throw new AxisFault(Messages.getMessage("notFound", "Operation"));
        }
        return op.getName().getLocalPart();
    }

    /**
     * @param inMessage
     * @return an input stream to the javascript source file
     * @throws AxisFault
     */
    public Reader readJS(MessageContext inMessage) throws AxisFault {

        AxisService service = inMessage.getServiceContext().getAxisService();
        Parameter implInfoParam = service.getParameter("ServiceJS");
        if (implInfoParam == null) {
            throw new AxisFault("Parameter 'ServiceJS' not specified");
        }
        InputStream jsFileStream = service.getClassLoader().getResourceAsStream(implInfoParam.getValue().toString());
        if (jsFileStream == null) {
            throw new AxisFault(Messages.getMessage("javaScriptUnableToLoad", implInfoParam.getValue().toString()));
        }
        return new BufferedReader(new InputStreamReader(jsFileStream));
    }
}
