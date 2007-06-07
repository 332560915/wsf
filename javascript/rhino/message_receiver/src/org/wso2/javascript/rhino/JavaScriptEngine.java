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

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.Reader;
import java.net.URL;

import org.apache.axiom.om.OMNode;
import org.apache.axiom.om.impl.llom.OMSourcedElementImpl;
import org.apache.axis2.AxisFault;
import org.apache.axis2.json.JSONBadgerfishDataSource;
import org.apache.axis2.json.JSONDataSource;
import org.apache.axis2.json.JSONOMBuilder;
import org.mozilla.javascript.Context;
import org.mozilla.javascript.Function;
import org.mozilla.javascript.ImporterTopLevel;
import org.mozilla.javascript.JavaScriptException;
import org.mozilla.javascript.Scriptable;
import org.mozilla.javascript.ScriptableObject;
import org.mozilla.javascript.WrappedException;
import org.mozilla.javascript.xmlimpl.XML;
import org.mozilla.javascript.xmlimpl.XMLList;

/**
 * Class JavaScriptEngine implements a simple Javascript evaluator using Rhino.
 * Two of the shell functionalities, print() and load(), are implemented as well.
 * Extending the ImporterTopLevel class gives access to the top level methods,
 * importClass() and importPackage().
 */
public class JavaScriptEngine extends ImporterTopLevel {
    //TODO Do we really need this?? (thilina)
    public static String axis2RepositoryLocation;

    private Context cx;

    /**
     * Constructs a new instance of the JavaScriptEngine class
     */
    public JavaScriptEngine() {
        super(Context.enter());
        cx = Context.getCurrentContext();
        String[] names = { "load", "print" };
        defineFunctionProperties(names, JavaScriptEngine.class, ScriptableObject.DONTENUM);
    }

    /**
     * Evaluates a Reader instance associated to a Javascript source.
     *
     * @param reader a Reader instance associated to a Javascript source
     * @throws IOException if the Reader instance generates an IOException
     */
    public void evaluate(Reader reader) throws IOException {
        cx.evaluateReader(this, reader, "<cmd>", 1, null);
    }

    /**
     * Loads and executes a set of Javascript source files. The source files
     * are searched relative to the service archive. If not found then the
     * search will assume absolute path is given. If fails again then it will
     * search under classes folder in Axis2 repository.
     *
     * <strong>We load this method to the JS Engine to be used internally by the java scripts.</strong>
     * @throws FileNotFoundException if the specified source cannot be found
     * @throws IOException           if evaluating the source produces an IOException
     */
    public static void load(Context cx, Scriptable thisObj, Object[] args, Function funObj)
            throws FileNotFoundException, IOException {

        JavaScriptEngine engine = (JavaScriptEngine) getTopLevelScope(thisObj);

        for (int i = 0; i < args.length; i++) {
            // Assumes resource's path is given relative to the service archive
            URL url = engine.getClass().getClassLoader().getResource(Context.toString(args[i]));
            File f = new File(url.getPath());
            if (!f.exists()) {
                // Assumes resource's path is given as absolute
                f = new File(Context.toString(args[i]));
                if (!f.exists() && axis2RepositoryLocation != null) {
                    // Assumes resource's path is given relative to the classes folder in Axis2 repository
                    f = new File(axis2RepositoryLocation + File.separator + "classes"
                            + File.separator + Context.toString(args[i]));
                }
            }
            FileReader fReader = new FileReader(f);
            engine.evaluate(fReader);
        }
    }

    /**
     * Prints the value of each element in the args array.
     * This is a similar implementation to the Rhino's print()
     * functionality in the shell.
     * <strong>We load this method to the JS Engine to be used internally by the java scripts.</strong>
     */
    public static void print(Context cx, Scriptable thisObj, Object[] args, Function funObj) {
        for (int i = 0; i < args.length; i++) {
            if (i > 0) {
                System.out.print(" ");
            }
            String s = Context.toString(args[i]);
            System.out.print(s);
        }
        System.out.println();
    }

    /**
     * Evaluates the requested operation in the Javascript service implementation.
     *
     * @param method Javascript operation name
     * @param reader a Reader instance associated with the Javascript service
     * @param args   an Object representing the input to the operation
     * @return an OMNode containing the result from executing the operation
     * @throws AxisFault
     */
    public OMNode call(String method, Reader reader, Object args) throws AxisFault {
        boolean json = false;
        try {
            // Handle JSON messages
            if (args instanceof OMSourcedElementImpl) {
                Object datasource = ((OMSourcedElementImpl) args).getDataSource();
                if (datasource instanceof JSONDataSource) {
                    args = ((JSONDataSource) datasource).getCompleteJOSNString();
                    args = "var x = " + args + ";";
                    cx.evaluateString(this, (String) args, "Get JSON", 0, null);
                    args = this.get("x", this);
                    json = true;
                } else if (datasource instanceof JSONBadgerfishDataSource) {
                    throw new AxisFault("Badgerfish Convention is not supported");
                } else {
                    throw new AxisFault("Unsupported Data Format");
                }

            } else if (args !=null) {
                // Rhino E4X XMLLibImpl object can be instantiated only from within a script
                // So we instantiate it in here, so that we can use it outside of the script later
                cx.evaluateString(this, "new XML();", "Instantiate E4X", 0, null);
                
                Object[] objects = {args};
                args = cx.newObject(this, "XML",objects);
            }
            
            // Evaluates the javascript file
            evaluate(reader);

            // Get the function from the scope the javascript object is in
            Object fObj = this.get(method, this);
            if (!(fObj instanceof Function) || (fObj == Scriptable.NOT_FOUND)) {
                throw new AxisFault("Method " + method + " is undefined or not a function");
            }
            // Invokes the java script function
            Object functionArgs[] = { args };
            Function f = (Function) fObj;
            Object result = f.call(cx, this, this, functionArgs);

            if (json) {
                result = ((String) result).substring(1, ((String) result).length() - 1);
                InputStream in = new ByteArrayInputStream(((String) result).getBytes());
                JSONOMBuilder builder = new JSONOMBuilder();
                result = builder.processDocument(in, null, null);
            } else {
                // Get the OMNode inside the resulting object
                if (result instanceof XML) {
                    result = ((XML) result).getAxiomFromXML();
                } else if (result instanceof XMLList) {
                    XMLList list = (XMLList) result;
                    if (list.length() == 1) {
                        result = list.getAxiomFromXML();
                    } else if (list.length() == 0) {
                        throw new AxisFault("Function returns an XMLList containing zero node");
                    } else {
                        throw new AxisFault(
                                "Function returns an XMLList containing more than one node");
                    }
                }
            }
            return (OMNode) result;
        } catch (WrappedException exception) {
            throw AxisFault.makeFault(exception.getCause());
        } catch (JavaScriptException exception) {
            throw new AxisFault(exception.getValue().toString(), exception);
        } catch (Throwable throwable) {
            throw AxisFault.makeFault(throwable);
        }
    }

    /**
     * Evaluates the requested operation in the Javascript service
     * implementation. Any Javascript source defined under loadJSScripts
     * parameter is evaluated before evaluating the operation.
     * 
     * @param method
     *            Javascript operation name
     * @param reader
     *            a Reader instance associated with the Javascript service
     * @param args
     *            an Object representing the input to the operation
     * @param scripts
     *            a string represnting a set of Javascript files to be evaluated
     *            before evaluating the service
     * @param json
     *            a boolean parameter indicating whether the service is
     *            accepting a JSON input or an XML input
     * @return an OMNode containing the result from executing the operation
     * @throws AxisFault
     */
    public OMNode call(String method, Reader reader, Object args, String scripts) throws AxisFault {

        if (scripts != null) {
            // Generate load command out of the parameter scripts
            scripts = "load(" + ("[\"" + scripts + "\"]").replaceAll(",", "\"],[\"") + ")";
            cx.evaluateString(this, scripts, "Load JavaScripts", 0, null);
        }
        return call(method, reader, args);
    }

    public Context getCx() {
        return cx;
    }
}