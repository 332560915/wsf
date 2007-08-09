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
import org.apache.axiom.om.OMText;
import org.apache.axiom.om.OMFactory;
import org.apache.axiom.soap.SOAP11Constants;
import org.apache.axiom.soap.SOAP12Constants;
import org.apache.axiom.soap.SOAPBody;
import org.apache.axiom.soap.SOAPEnvelope;
import org.apache.axiom.soap.SOAPFactory;
import org.apache.axis2.AxisFault;
import org.apache.axis2.json.JSONOMBuilder;
import org.apache.axis2.context.MessageContext;
import org.apache.axis2.databinding.utils.ConverterUtil;
import org.apache.axis2.description.AxisMessage;
import org.apache.axis2.description.AxisOperation;
import org.apache.axis2.description.AxisService;
import org.apache.axis2.description.Parameter;
import org.apache.axis2.engine.MessageReceiver;
import org.apache.axis2.receivers.AbstractInOutMessageReceiver;
import org.apache.axis2.wsdl.WSDLConstants;
import org.apache.ws.commons.schema.XmlSchemaComplexType;
import org.apache.ws.commons.schema.XmlSchemaElement;
import org.apache.ws.commons.schema.XmlSchemaParticle;
import org.apache.ws.commons.schema.XmlSchemaSequence;
import org.apache.ws.commons.schema.XmlSchemaType;
import org.apache.ws.commons.schema.constants.Constants;
import org.mozilla.javascript.Context;
import org.mozilla.javascript.NativeArray;
import org.mozilla.javascript.NativeObject;
import org.mozilla.javascript.xmlimpl.XML;
import org.mozilla.javascript.xmlimpl.XMLList;

import javax.xml.namespace.QName;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.ByteArrayInputStream;
import java.net.URL;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Iterator;
import java.util.Date;

/**
 * Class JavaScriptReceiver implements the AbstractInOutSyncMessageReceiver,
 * which, is the abstract IN-OUT MEP message receiver.
 */
public class JavaScriptReceiver extends AbstractInOutMessageReceiver implements MessageReceiver,
        JavaScriptEngineConstants {

    /**
     * Invokes the Javascript service with the parameters from the inMessage
     * and sets the outMessage with the response from the service.
     *
     * @param inMessage MessageContext object with information about the incoming message
     * @param outMessage MessageContext object with information about the outgoing message
     * @throws AxisFault
     */
    public void invokeBusinessLogic(MessageContext inMessage, MessageContext outMessage)
            throws AxisFault {
        SOAPEnvelope soapEnvelope = inMessage.getEnvelope();
        try {
            // Create JS Engine, Inject HostObjects
            JavaScriptEngine engine = new JavaScriptEngine();

            // Rhino E4X XMLLibImpl object can be instantiated only from within a script
            // So we instantiate it in here, so that we can use it outside of the script later
            engine.getCx().evaluateString(engine, "new XML();", "Instantiate E4X", 0, null);

            JavaScriptEngineUtils.loadHostObjects(engine, inMessage.getConfigurationContext()
                    .getAxisConfiguration());

            // Inject the incoming MessageContext to the Rhino Context. Some
            // host objects need access to the MessageContext. Eg: FileSystem,
            // WSRequest
            Context context = engine.getCx();
            context.putThreadLocal(AXIS2_MESSAGECONTEXT, inMessage);

            /*
             * Some host objects depend on the data we obtain from the
             * AxisService & ConfigurationContext.. It is possible to get these
             * data through the MessageContext. But we face problems at the
             * deployer, where we need to instantiate host objects in order for
             * the annotations framework to work and the MessageContext is not
             * available at that time. For the consistency we inject them in
             * here too..
             */
            context.putThreadLocal(AXIS2_SERVICE, inMessage.getAxisService());
            context.putThreadLocal(AXIS2_CONFIGURATION_CONTEXT, inMessage
                            .getConfigurationContext());

            JavaScriptEngineUtils.loadGlobalPropertyObjects(engine, inMessage
                    .getConfigurationContext().getAxisConfiguration());
            // JS Engine seems to need the Axis2 repository location to load the
            // imported scripts. TODO: Do we really need this (thilina)
            URL repoURL = inMessage.getConfigurationContext().getAxisConfiguration()
                    .getRepository();
            if (repoURL != null) {
                JavaScriptEngine.axis2RepositoryLocation = repoURL.getPath();
            }

            Reader reader = readJS(inMessage);
            String jsFunctionName = inferJavaScriptFunctionName(inMessage);

            //support for importing javaScript files using services.xml or the axis2.xml
            String scripts = getImportScriptsList(inMessage);

            ArrayList params = new ArrayList();
            OMElement result = null;
            OMElement payload = soapEnvelope.getBody().getFirstElement();
            Object args = payload;
            if (payload != null) {
                AxisMessage axisMessage = inMessage.getAxisOperation().getMessage(
                        WSDLConstants.MESSAGE_LABEL_IN_VALUE);
                XmlSchemaElement xmlSchemaElement = axisMessage.getSchemaElement();
                if (xmlSchemaElement != null) {
                    XmlSchemaType schemaType = xmlSchemaElement.getSchemaType();
                    if (schemaType instanceof XmlSchemaComplexType) {
                        XmlSchemaComplexType complexType = ((XmlSchemaComplexType) schemaType);
                        XmlSchemaParticle particle = complexType.getParticle();
                        if (particle instanceof XmlSchemaSequence) {
                            XmlSchemaSequence xmlSchemaSequence = (XmlSchemaSequence) particle;
                            Iterator iterator = xmlSchemaSequence.getItems().getIterator();
                            // now we need to know some information from the
                            // binding operation.
                            while (iterator.hasNext()) {
                                XmlSchemaElement innerElement = (XmlSchemaElement) iterator.next();
                                long maxOccurs = innerElement.getMaxOccurs();
                                // Check whether the schema advertises this element as an array
                                if (maxOccurs > 1) {
                                    // If its an array get all elements with that name and create a sinple parameter out of it
                                    Iterator iterator1 = payload.getChildrenWithName(new QName(
                                            innerElement.getName()));
                                    params.add(handleArray(iterator1, innerElement.getSchemaTypeName(), engine));
                                } else {
                                    OMElement omElement = payload.getFirstChildWithName(new QName(
                                            innerElement.getName()));
                                    if (omElement == null) {
                                        // There was no such element in the payload. Therefore we check for minoccurs
                                        // and if its 0 add null as a parameter (If not we might mess up the parameters
                                        // we pass into the function).
                                        if (innerElement.getMinOccurs() == 0) {
                                            params.add(null);
                                            continue;
                                        } else {
                                            // If minoccurs is not zero throw an exception.
                                            // Do we need to di strict schema validation?
                                            throw new AxisFault(
                                                    "Required element " + innerElement.getName()
                                                            + " defined in the schema can not be found in the request");
                                        }
                                    }
                                    params.add(createParam(omElement, innerElement.getSchemaTypeName(),
                                            engine));
                                }
                            }
                            args = params.toArray();
                        } else {
                            throw new AxisFault("Unsupported schema type in request");
                        }
                    } else if (xmlSchemaElement.getSchemaTypeName() == Constants.XSD_ANYTYPE) {
                        // TODO : Keith > Don't we need to handle the 'else' in here..
                        args = payload;
                    }
                }
            } else {
                // This validates whether the user has sent a bad SOAP message
                // with a non-XML payload.
                if (soapEnvelope.getBody().getFirstOMChild() != null) {
                    OMText textPayLoad = (OMText) soapEnvelope.getBody().getFirstOMChild();
                    //we allow only a sequence of spaces
                    if (textPayLoad.getText().trim().length() > 0) {
                        throw new AxisFault(
                                "Non-XML payload is not allowed. PayLoad inside the SOAP body needs to be an XML element.");
                    }
                }
            }
            AxisMessage outAxisMessage = inMessage.getAxisOperation().getMessage(
                    WSDLConstants.MESSAGE_LABEL_OUT_VALUE);
            // Get the result by executing the javascript file
            Boolean annotated = Boolean.FALSE;
            Parameter parameter = outAxisMessage.getParameter(JavaScriptEngineConstants.ANNOTATED);
            if (parameter != null) {
                annotated = (Boolean) parameter.getValue();
            }
            Object response = engine.call(jsFunctionName, reader, args, scripts, annotated);

            // Create the outgoing message
            SOAPFactory fac;
            if (inMessage.isSOAP11()) {
                fac = OMAbstractFactory.getSOAP11Factory();
            } else {
                fac = OMAbstractFactory.getSOAP12Factory();
            }
            SOAPEnvelope envelope = fac.getDefaultEnvelope();
            SOAPBody body = envelope.getBody();
            XmlSchemaElement xmlSchemaElement = outAxisMessage.getSchemaElement();
            OMElement outElement;
            String prefix = "ws";
            if (xmlSchemaElement != null) {
                QName elementQName = xmlSchemaElement.getSchemaTypeName();
                OMNamespace namespace = fac.createOMNamespace(elementQName.getNamespaceURI(),
                        prefix);
                outElement = fac.createOMElement(xmlSchemaElement.getName(), namespace);
                XmlSchemaType schemaType = xmlSchemaElement.getSchemaType();
                if (schemaType instanceof XmlSchemaComplexType) {
                    XmlSchemaComplexType complexType = ((XmlSchemaComplexType) schemaType);
                    XmlSchemaParticle particle = complexType.getParticle();
                    if (particle instanceof XmlSchemaSequence) {
                        XmlSchemaSequence xmlSchemaSequence = (XmlSchemaSequence) particle;
                        Iterator iterator = xmlSchemaSequence.getItems().getIterator();
                        // now we need to know some information from the binding operation.
                        while (iterator.hasNext()) {
                            XmlSchemaElement innerElement = (XmlSchemaElement) iterator.next();
                            // The name of the element returned should match the schema hence set that name.
                            result = buildResponse(annotated, engine.isJson(), response, innerElement);
                            outElement.addChild(result);
                        }
                        body.addChild(outElement);
                    } else {
                        throw new AxisFault("Unsupported schema type in response.");
                    }
                } else if (xmlSchemaElement.getSchemaTypeName() == Constants.XSD_ANYTYPE) {
                    if (response != null) {
                        body.addChild(buildResponse(annotated, engine.isJson(), response, xmlSchemaElement));
                    }
                }
            } else if (response != null) {
                body.addChild(buildResponse(annotated, engine.isJson(), response, xmlSchemaElement));
            }
            outMessage.setEnvelope(envelope);
        } catch (Throwable throwable) {
            AxisFault fault = AxisFault.makeFault(throwable);
            // This is a workaround to avoid Axis2 sending the SOAPFault with a
            // http-400 code when sending using SOAP1. We explicitly set the
            // FualtCode to 'Receiver'.
            fault.setFaultCode(SOAP12Constants.SOAP_ENVELOPE_NAMESPACE_URI.equals(soapEnvelope
                   .getNamespace().getNamespaceURI()) 
                       ?SOAP12Constants.SOAP_DEFAULT_NAMESPACE_PREFIX+ ":" + SOAP12Constants.FAULT_CODE_RECEIVER
                       :SOAP12Constants.SOAP_DEFAULT_NAMESPACE_PREFIX + ":"+ SOAP11Constants.FAULT_CODE_RECEIVER);
            throw fault;
        }
    }

    /**
     * Provides support for importing JavaScript files specified in the
     * Services.xml or the Axis2.xml using the "loadJSScripts" parameter.
     * @param inMessage - The incoming message Context
     * @return
     */
    private String getImportScriptsList(MessageContext inMessage) {
        String scripts = null;

        // Get necessary JavaScripts to be loaded from services.xml
        Parameter param = inMessage.getOperationContext().getAxisOperation().getParameter(
                JavaScriptEngineConstants.LOAD_JSSCRIPTS);
        if (param != null) {
            scripts = (String) param.getValue();
        }

        /**** TODO We might not need the following code since getting a parameter
        from an Operation covers this(thilina) ****/
        // Get necessary JavaScripts to be loaded from axis2.xml
        param = inMessage.getConfigurationContext().getAxisConfiguration().getParameter(
                JavaScriptEngineConstants.LOAD_JSSCRIPTS);
        if (param != null) {
            if (scripts == null) {
                scripts = (String) param.getValue();
            } else {
                // Avoids loading the same set of script files twice
                if (!scripts.equals(param.getValue())) {
                    scripts += "," + param.getValue();
                }
            }
        }
        return scripts;
    }

    /**
     * Creates an object that can be passed into a javascript function from an OMElement.
     * @param omElement - The OMElement that the parameter should be created for
     * @param type - The schemaType of the incoming message element
     * @param engine - Reference to the javascript engine
     * @return - An Object that can be passed into a JS function
     */
    private Object createParam(OMElement omElement, QName type, JavaScriptEngine engine) {
        // TODO we may need to handle arrays here and also do some type conversion in some cases.

        if (Constants.XSD_ANYTYPE.equals(type)) {
            Context context = engine.getCx();
            OMElement element = omElement.getFirstElement();
            Object[] objects = { element };
            return context.newObject(engine, "XML", objects);
        }
        if (Constants.XSD_BOOLEAN.equals(type)) {
            String value = omElement.getText();
            return Boolean.valueOf(value);
        }
        if (Constants.XSD_DOUBLE.equals(type)) {
            String value = omElement.getText();
            return new Double(ConverterUtil.convertToDouble(value));
        }
        if (Constants.XSD_FLOAT.equals(type)) {
            String value = omElement.getText();
            return new Float(ConverterUtil.convertToFloat(value));
        }
        if (Constants.XSD_DATETIME.equals(type)) {
            String value = omElement.getText();
            Calendar calendar = ConverterUtil.convertToDateTime(value);
            return calendar.getTime();
        }
        return omElement.getText();
    }

    /**
     * Creates an array object that can be passed into a JS function
     * @param iterator - Iterator to the omelements that belong to the array
     * @param type - The schematype of the omelement
     * @param engine Reference to the javascript engine
     * @return - An array Object that can be passed into a JS function
     */
    private Object handleArray(Iterator iterator, QName type, JavaScriptEngine engine) {
        ArrayList objectList = new ArrayList();
        while (iterator.hasNext()) {
            OMElement omElement = (OMElement) iterator.next();
            objectList.add(createParam(omElement, type, engine));
        }
        return objectList.toArray();
    }

    /**
     * Extracts and returns the name of the JS function associated for the
     * currently dispatched operation. First we try to retrieve the function
     * name vis the JS_FUNCTION_NAME parameter of the AxisOperation. If not we
     * assume the localpart of the operation name to be the function name.
     * 
     * @param inMessage
     *            MessageContext object with information about the incoming
     *            message
     * @return the name of the requested JS function
     * @throws AxisFault
     *             if the function name cannot be inferred.
     */
    private String inferJavaScriptFunctionName(MessageContext inMessage) throws AxisFault {
        //Look at the method name. if available this should be a javascript method
        AxisOperation op = inMessage.getOperationContext().getAxisOperation();
        if (op == null) {
            throw new AxisFault("Operation notFound");
        }
        Parameter parameter;
        String jsFunctionName;
        if ((parameter = op.getParameter(JS_FUNCTION_NAME)) != null) {
            jsFunctionName = (String) parameter.getValue();
        } else {
            jsFunctionName = op.getName().getLocalPart();
        }
        if (jsFunctionName == null)
            throw new AxisFault(
                    "Unable to infer the JavaScript function  corresponding to this message.");
        return jsFunctionName;
    }

    /**
     * Locates the service Javascript file associated with ServiceJS parameter and returns
     * a Reader for it.
     *
     * @param inMessage MessageContext object with information about the incoming message
     * @return an input stream to the javascript source file
     * @throws AxisFault if the parameter ServiceJS is not specified or if the service
     * implementation is not available
     */
    private Reader readJS(MessageContext inMessage) throws AxisFault {
        InputStream jsFileStream;
        AxisService service = inMessage.getServiceContext().getAxisService();
        Parameter implInfoParam = service.getParameter(JavaScriptEngineConstants.SERVICE_JS);
        if (implInfoParam == null) {
            throw new AxisFault("Parameter 'ServiceJS' not specified");
        }
        if (implInfoParam.getValue() instanceof File) {
            try {
                jsFileStream = new FileInputStream((File) (implInfoParam.getValue()));
            } catch (FileNotFoundException e) {
                throw new AxisFault("Unable to load the javaScript, File not Found", e);
            }
        } else {
            jsFileStream = service.getClassLoader().getResourceAsStream(
                    implInfoParam.getValue().toString());
        }
        if (jsFileStream == null) {
            throw new AxisFault("Unable to load the javaScript");
        }
        return new BufferedReader(new InputStreamReader(jsFileStream));
    }

    private OMElement buildResponse(Boolean annotated, boolean json, Object result, XmlSchemaElement innerElement) throws AxisFault {
        if (json) {
            result = ((String) result).substring(1, ((String) result).length() - 1);
            InputStream in = new ByteArrayInputStream(((String) result).getBytes());
            JSONOMBuilder builder = new JSONOMBuilder();
            result = builder.processDocument(in, null, null);
        }
        // Convert the JS return to XML
        boolean addTypeInfo = !annotated.booleanValue();
        return createResponseElement(result, innerElement.getName(), addTypeInfo);
    }

    /**
     * Given a jsObject converts it to corresponding XML
     * @param jsObject  - The object that needs to be converted
     * @param elementName - The element name of the wrapper
     * @param addTypeInfo - Whether type information should be added into the element as an attribute
     * @return - OMelement which represents the JSObject
     * @throws AxisFault - Thrown in case an exception occurs during the conversion
     */
    private OMElement createResponseElement(Object jsObject, String elementName, boolean addTypeInfo) throws AxisFault {
        String className = jsObject.getClass().getName();
        OMFactory fac = OMAbstractFactory.getOMFactory();
        OMNamespace namespace = fac.createOMNamespace("http://www.wso2.org/ns/jstype", "js");
        OMElement element = fac.createOMElement(elementName, null);
        // Get the OMNode inside the jsObjecting object
        if (jsObject instanceof XML) {
            element.addChild((((XML) jsObject).getAxiomFromXML()));
            if (addTypeInfo) {
                element.addAttribute("type", "xml", namespace);
            }
        } else if (jsObject instanceof XMLList) {
            XMLList list = (XMLList) jsObject;
            if (list.length() == 1) {
                element.addChild(list.getAxiomFromXML());
                if (addTypeInfo) {
                element.addAttribute("type", "xmlList", namespace);
                }
            } else if (list.length() == 0) {
                throw new AxisFault("Function returns an XMLList containing zero node");
            } else {
                throw new AxisFault(
                        "Function returns an XMLList containing more than one node");
            }
        } else {

            if (jsObject instanceof String) {
                element.setText((String) jsObject);
                if (addTypeInfo) {
                    element.addAttribute("type", "string", namespace);
                }
            } else if (jsObject instanceof Boolean) {
                Boolean booljsObject = (Boolean) jsObject;
                element.setText(booljsObject.toString());
                if (addTypeInfo) {
                    element.addAttribute("type", "boolean", namespace);
                }
            } else if (jsObject instanceof Number) {
                Number numjsObject = (Number) jsObject;
                String str = numjsObject.toString();
                if (str.indexOf("Infinity") >= 0) {
                    str = str.replace("Infinity", "INF");
                }
                element.setText(str);
                if (addTypeInfo) {
                    element.addAttribute("type", "number", namespace);
                }
            }  else if (jsObject instanceof Date || "org.mozilla.javascript.NativeDate".equals(className)) {
                Date date = (Date) Context.jsToJava(jsObject, Date.class);
                Calendar calendar = Calendar.getInstance();
                calendar.setTime(date);
                String dateTime = ConverterUtil.convertToString(calendar);
                element.setText(dateTime);
                if (addTypeInfo) {
                    element.addAttribute("type", "date", namespace);
                }
            } else if (jsObject instanceof NativeArray) {
                element.addAttribute("type", "array", namespace);
                NativeArray nativeArray = (NativeArray) jsObject;
                Object[] objects = nativeArray.getAllIds();
                for (int i = 0; i < objects.length; i++) {
                    Object object = objects[i];
                    Object o;
                    String propertyElementName;
                    if (object instanceof String) {
                        String property = (String) object;
                        if ("length".equals(property)) {
                            continue;
                        }
                        o = nativeArray.get(property, nativeArray);
                        propertyElementName = property;
                    } else {
                        Integer property = (Integer) object;
                        o = nativeArray.get(property.intValue(), nativeArray);
                        propertyElementName = "item";
                    }
                    OMElement paramElement = createResponseElement(o, propertyElementName, true);
                    element.addChild(paramElement);
                }
            } else if (jsObject instanceof Object[]) {
                element.addAttribute("type", "array", namespace);
                Object[] objects = (Object[]) jsObject;
                for (int i = 0; i < objects.length; i++) {
                    Object object = objects[i];
                    OMElement paramElement = createResponseElement(object, "item", true);
                    element.addChild(paramElement);
                }
            } else if (jsObject instanceof NativeObject) {
                element.addAttribute("type", "object", namespace);
                NativeObject nativeObject = (NativeObject) jsObject;
                Object[] objects = NativeObject.getPropertyIds(nativeObject);
                for (int i = 0; i < objects.length; i++) {
                    Object object = objects[i];
                    Object o;
                    if (object instanceof String) {
                        String property = (String) object;
                        o = nativeObject.get(property, nativeObject);
                        OMElement paramElement = createResponseElement(o, property, true);
                        element.addChild(paramElement);
                    }
                }
            }
        }
        return element;
    }
}
