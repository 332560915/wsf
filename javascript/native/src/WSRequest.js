/*
 * Copyright 2007 WSO2, Inc. http://www.wso2.org
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

var WSRequest;

WSRequest = function() {
    this.readyState = 0;
    this.responseText = null;
    this.responseXML = null;
    this.error = null;
    this.onreadystatechange = null;
    this._xmlhttp = WSRequest.util._createXMLHttpRequestObject();
    this._soapVer = null;
};

WSError = function() {
    this.code = null;
    this.reason = null;
    this.detail = null;
};

WSRequest.prototype._async = true;
WSRequest.prototype._optionSet = null;
WSRequest.prototype._uri = null;
WSRequest.prototype._username = null;
WSRequest.prototype._password = null;

WSRequest.prototype.open = function(options, URL, asnycFlag, userName, passWord) {
    if (arguments.length < 2 || arguments.length > 6)
    {
        throw new Error("Invalid input argument");
    }

    if (typeof(options) == "string") {
        this._optionSet = new Array();
        this._optionSet["HTTPMethod"] = options;
    } else {
        this._optionSet = options;
    }

    this._uri = URL;
    this._async = asnycFlag;
    if (userName != null && passWord == null)
        throw new Error("User name should have a password");
    else
    {
        this._username = userName;
        this._password = passWord;
    }

    this.readyState = 1;
    if (this.onreadystatechange != null)
        this.onreadystatechange();
    this.responseText = null;
    this.responseXML = null;
    this.error = null;
};

WSRequest.prototype.send = function(payload) {
    if (arguments.length > 1) {
        return new Error("Invalid input argument");
    }

    var req = null;        // string to be sent
    
    if (this._optionSet["HTTPMethod"] != null)
        var method = this._optionSet["HTTPMethod"];
    else
        var method = "POST";
    
    this._soapVer = WSRequest.util._bindingVersion(this._optionSet);
   
    if (payload != null)
    {
        // seralize the dom to string
        var content = WSRequest.util._serializeToString(payload);
        if (content == false) {
            throw new Error("Invalid input argument");
        }

        // formulate the message envelope
        if (this._soapVer == 0) {
            req = WSRequest.util._buildHTTPpayload(this._optionSet, this._uri, content);
        } else {
            req = WSRequest.util._buildSOAPEnvelope(this._soapVer, this._optionSet, this._uri, content);
        }
    }

    // Note that we infer soapAction from the "action" parameter - also used for wsa:Action.
    //  WS-A recommends keeping these two items in sync.
    var soapAction = this._optionSet["action"];
    
    this._xmlhttp.open(method, this._uri, this._async);
    
    switch (this._soapVer) {
        case 1.1:
            this._xmlhttp.setRequestHeader("SOAPAction", soapAction == undefined ? '""' : '"' + soapAction + '"');
            this._xmlhttp.setRequestHeader("Content-Type","text/xml; charset=UTF-8");
            break;
        case 1.2:
            this._xmlhttp.setRequestHeader("Content-Type","application/soap+xml;charset=UTF-8" + (soapAction == undefined ? "" : ";action=" + soapAction));
            break;
        case 0:
            if (this._optionSet["HTTPInputSerialization"] != null) {
                contentType = this._optionSet["HTTPInputSerialization"]
            } else {
                contentType = "application/xml";
            }
            this._xmlhttp.setRequestHeader("Content-Type", contentType);
            break;
    }

    if (this._async) {
        this._xmlhttp.onreadystatechange = WSRequest.util._bind(this._handleReadyState, this);
        this._xmlhttp.send(req);
    } else {
        // sync call
        this.readyState = 2;
        if (this.onreadystatechange != null)
            this.onreadystatechange();
        this._xmlhttp.send(req);

        this._processResult();
        
        this.readyState = 4;
        if (this.onreadystatechange != null)
            this.onreadystatechange();
    }
}

/**
 * @description Set responseText, responseXML, and error of WSRequest.
 * @method _processResult
 * @private
 * @static
 */
WSRequest.prototype._processResult = function () {
    if (this._soapVer == 0) {
        this.responseText = this._xmlhttp.responseText;
        this.responseXML = this._xmlhttp.responseXML;
        this.error = null;  // How would I tell?
    } else {
        var browser = WSRequest.util._getBrowser();
            
        if (this._xmlhttp.responseText != "") {
            if ((browser == "ie" || browser == "ie7") && this._xmlhttp.responseXML.documentElement == null) {
                // unrecognized media type (probably application/soap+xml)
                var responseXMLdoc = new ActiveXObject("Microsoft.XMLDOM");
                responseXMLdoc.loadXML(this._xmlhttp.responseText);
                var response = responseXMLdoc.documentElement;
            } else {
                var response = this._xmlhttp.responseXML.documentElement;
            }
            if (this._soapVer == 1.1)
                var soapNamespace = "http://schemas.xmlsoap.org/soap/envelope/";
            else 
                var soapNamespace = "http://www.w3.org/2003/05/soap-envelope";
            
            var soapBody = WSRequest.util._firstElement(response, soapNamespace, "Body");
            if (soapBody != null && soapBody.hasChildNodes()) {
                var newDoc; 
                if (browser == "gecko")
                {
                    try {
                        netscape.security.PrivilegeManager.enablePrivilege("UniversalBrowserRead");
                    } catch(e) {
                    }
                    var newDoc = document.implementation.createDocument("","",null);
                    newDoc.appendChild(soapBody.firstChild);
                }
    
                if (browser == "ie" || browser == "ie7") {
                    var newDoc = new ActiveXObject("Microsoft.XMLDOM");
                    newDoc.appendChild(soapBody.firstChild);
                }
    
                this.responseXML = newDoc;
                this.responseText = WSRequest.util._serializeToString(newDoc);
                
                fault = WSRequest.util._firstElement(newDoc, soapNamespace, "Fault");
                if (fault != undefined) {
                    this.error = new WSError();
                    if (this._soapVer == 1.2) {
                        this.error.code = WSRequest.util._stringValue(WSRequest.util._firstElement(fault, soapNamespace, "Value"));
                        this.error.reason = WSRequest.util._stringValue(WSRequest.util._firstElement(fault, soapNamespace, "Text"));
                        this.error.detail =
                            WSRequest.util._serializeToString(WSRequest.util._firstElement(fault, soapNamespace, "Detail"));
                    } else {
                        this.error.code = WSRequest.util._stringValue(fault.getElementsByTagName("faultcode")[0]);
                        this.error.reason = WSRequest.util._stringValue(fault.getElementsByTagName("faultstring")[0]);
                        this.error.detail =
                            WSRequest.util._serializeToString(fault.getElementsByTagName("detail")[0]);
                    }
                }
            } else {
                // empty SOAP body - not necessarily an error
                this.responseXML = null;
                this.responseText = "";
                this.error = null;
            }
        } else {
            this.responseXML = null;
            this.responseText = "";
            this.error = new WSError();
            this.error.code = "HTTP" + this._xmlhttp.status;
            this.error.reason = "No SOAP Body.";
            this.error.detail = this._xmlhttp.statusText;
        }
    }
}


WSRequest.prototype._handleReadyState = function() {
    if (this._xmlhttp.readyState == 2) {
        this.readyState = 2;
        if (this.onreadystatechange != null)
            this.onreadystatechange();
    }

    if (this._xmlhttp.readyState == 3) {
        this.readyState = 3;
        if (this.onreadystatechange != null)
            this.onreadystatechange();
    }

    if (this._xmlhttp.readyState == 4) {
        this.readyState = 4;

        this._processResult();

        if (this.onreadystatechange != null)
            this.onreadystatechange();
    }
};


WSRequest.util = {

    _msxml : [
            'MSXML2.XMLHTTP.3.0',
            'MSXML2.XMLHTTP',
            'Microsoft.XMLHTTP'
            ],

/**
 * @description Instantiates a XMLHttpRequest object and returns it.
 * @method _createXMLHttpRequestObject
 * @private
 * @static
 * @return object
 */
    _createXMLHttpRequestObject : function() {
        var xhrObject;

        try {
            xhrObject = new XMLHttpRequest();
        } catch(e) {
            for (var i = 0; i < this._msxml.length; ++i) {
                try
                {
                    // Instantiates XMLHttpRequest for IE and assign to http.
                    xhrObject = new ActiveXObject(this._msxml[i]);
                    break;
                }
                catch(e) {
                    // do nothing
                }
            }
        } finally {
            return xhrObject;
        }
    },

/**
 * @description Serialize payload to string.
 * @method _serializeToString
 * @private
 * @static
 * @param {dom} payload   xml payload
 * @return string
 */
    _serializeToString : function(payload) {
        if (typeof(payload) == "string") {
            return payload;
        } else if (typeof(payload) == "object") {
            var browser = WSRequest.util._getBrowser();
            switch (browser) {

                case "gecko":
                    var serializer = new XMLSerializer();
                    return serializer.serializeToString(payload);
                    break;
                case "ie":
                    return payload.xml;
                    break;
                case "ie7":
                    return payload.xml;
                    break;
                case "opera":
                    var xmlSerializer = document.implementation.createLSSerializer();
                    return xmlSerializer.writeToString(payload);
                    break;
                case "safari":
                // use the safari method
                    throw new Error("Not implemented");
                case "undefined":
                    throw new Error("XMLHttp object could not be created");
            }
        } else {
            return false;
        }
    },


/**
 * @description get the character element children in a browser-independent way.
 * @method _stringValue
 * @private
 * @static
 * @param {dom element} node
 * @return string
 */
    _stringValue : function(node) {
        var browser = WSRequest.util._getBrowser();
        switch (browser) {
            case "ie":
            case "ie7":
                return node.text;
                break;
            case "gecko":
            case "opera":
            case "safari":
            case "undefined":
                value = "";
                if (node.nodeType == 3) value = node.nodeValue;
                else for (c in node.childNodes) value += WSRequest.util._stringValue(node.childNodes[c]);
                return value;
                break;
        }
    },


/**
 * @description Determines which binding to use (SOAP 1.1, SOAP 1.2, or HTTP) from the various options.
 * @method _bindingVersion
 * @private
 * @static
 * @param {Array} options   Options given by user
 * @return string
 */
    _bindingVersion : function(options) {
        switch (options["useBindng"]) {
            case "SOAP 1.2":
                soapVer = 1.2;
                break;
            case "SOAP 1.1":
                soapVer = 1.1;
                break;
            case "HTTP":
                soapVer = 0;
                break;
            case undefined:
                useSOAP = options["useSOAP"];
                switch (useSOAP) {
                    case 1.2:
                        soapVer = 1.2;
                        break;
                    case "1.2":
                        soapVer = 1.2;
                        break;
                    case 1.1:
                        soapVer = 1.1;
                        break;
                    case "1.1":
                        soapVer = 1.1;
                        break;
                    case true:
                        soapVer = 1.2;
                        break;
                    case false:
                        soapVer = 0;
                        break;
                    case undefined:
                        throw("Unspecified binding type: set useBinding = 'SOAP 1.1' | 'SOAP 1.2' | 'HTTP'.");
                        break;
                    default:
                        throw("Unsupported useSOAP value '" + useSOAP + "'; set 'useBinding' option instead.");
                }
                break;
            default:
                throw("Unsupported useBinding value '" + options["useBinding"] + "': must be 'SOAP 1.2' | 'SOAP 1.1' | 'HTTP'.");
        }
        return soapVer;
    },

/**
 * @description Determine which browser we're running.
 * @method _getBrowser
 * @private
 * @static
 * @return string
 */
    _getBrowser : function() {
        var ua = navigator.userAgent.toLowerCase();
        if (ua.indexOf('opera') != -1) { // Opera (check first in case of spoof)
            return 'opera';
        } else if (ua.indexOf('msie 7') != -1) { // IE7
            return 'ie7';
        } else if (ua.indexOf('msie') != -1) { // IE
            return 'ie';
        } else if (ua.indexOf('safari') != -1) { // Safari (check before Gecko because it includes "like Gecko")
            return 'safari';
        } else if (ua.indexOf('gecko') != -1) { // Gecko
            return 'gecko';
        } else {
            return false;
        }
    },


/**
 * @description Build HTTP payload using given parameters.
 * @method _buildHTTPpayload
 * @private
 * @static
 * @param {Array} options   Options given by user
 * @param {string} url Address the request will be sent to.
 * @param {string} content SOAP payload
 * @return string
 */
    _buildHTTPpayload : function(options, url, content) {
        // For now, we only support stuffing the XML into the body.  
        //    Eventually, we can support splitting the payload between
        //    body and url here.
        return content;
    },

/**
 * @description Build soap message using given parameters.
 * @method _buildSoapEnvelope
 * @private
 * @static
 * @param {string} soapVer SOAP version (1.1 or 1.2)
 * @param {Array} options   Options given by user
 * @param {string} url Address the request will be sent to.
 * @param {string} content SOAP payload
 * @return string
 */
    _buildSOAPEnvelope : function(soapVer, options, url, content) {
        if (soapVer == 1.1)
            ns = "http://schemas.xmlsoap.org/soap/envelope/";
        else 
            ns = "http://www.w3.org/2003/05/soap-envelope";
        
        var headers = "";

        // addressing version/namespace
        var useWSA = options["useWSA"];
        var wsaNs = "";
        var wsaNsDecl = "";
        if (useWSA != undefined && useWSA != false) {
            if (useWSA == "1.0" || useWSA == true) {
                wsaNs = "http://www.w3.org/2005/08/addressing";
                standardversion = true;
            } else if (useWSA == "submission") {
                wsaNs = "http://schemas.xmlsoap.org/ws/2004/08/addressing";
                standardversion = false;
            } else throw ("Unknown WS-Addressing version '" + useWSA + "': must be '1.0' | 'submission' | true | false.");
            wsaNsDecl = ' xmlns:wsa="' + wsaNs + '"';
            headers = this._buildWSAHeaders(standardversion, options, url);
        }
        
        request = '<?xml version="1.0" encoding="UTF-8"?>\n' +
                  '<s:Envelope xmlns:s="' + ns + '"' +
                  wsaNsDecl + '>\n' +
                  '<s:Header>' + headers + '</s:Header>\n' +
                  '<s:Body>' + content + '</s:Body>\n' +
                  '</s:Envelope>';
        return request;
    },

/**
 * @description Build WS-Addressing headers using given parameters.
 * @method _buildWSAHeaders
 * @private
 * @static
 * @param {boolean} standardversion true for 1.0, false for submission
 * @param {Array} options   Options given by user
 * @param {string} address Address the request will be sent to.
 * @return string
 */
    _buildWSAHeaders : function(standardversion, options, address) {
        if (options['action'] == null)
            throw("'Action' option must be specified when WS-Addressing is engaged.");
        
        // wsa:To (required)
        var headers = "<wsa:To>" + address + "</wsa:To>\n";

        // wsa:From (optional)
        // Note: reference parameters and metadata aren't supported.
        if (options['from'] != null)
            headers += "<wsa:From><wsa:Address>" + options['From'] + "</wsa:Address></wsa:From>\n";
        
        // wsa:ReplyTo (optional)
        // Note: reference parameters and metadata aren't supported.
        // Note: No way to specify that wsa:ReplyTo should be omitted (e.g., only in-out MEPs are supported).
        if (options['replyto'] != null) {
            headers += "<wsa:ReplyTo><wsa:Address>" + options['ReplyTo'] + "</wsa:Address></wsa:ReplyTo>\n";
        } else {
            // Note: although wsa:ReplyTo is optional on in-out MEPs in the standard version, we put it in
            //  explicitly for convenience.
            headers += "<wsa:ReplyTo><wsa:Address>" + 
            ( standardversion ? 
                "http://www.w3.org/2005/08/addressing/anonymous" : 
                "http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous"
            ) + 
            "</wsa:Address></wsa:ReplyTo>\n";
        }
        
        // wsa:MessageID (required if a response is expected, e.g. wsa:ReplyTo is specified, which is always for us.)
        // If user doesn't supply an identifier, we'll make one up.
        if (options['messageid'] != null) {
            id = options['messageid'];
        } else {
            // coin a unique identifier based on the time (in milliseconds) and a 10-digit random number.
            now = (new Date()).valueOf();
            randomToken = Math.floor(Math.random()*10000000000);
            id = "http://identifiers.wso2.com/messageid/" + now + "/" + randomToken;
        }
        headers += "<wsa:MessageID>" + id + "</wsa:MessageID>\n";

        // wsa:FaultTo (optional)
        // Note: reference parameters and metadata aren't supported.
        if (options['faultto'] != null)
            headers += "<wsa:FaultTo><wsa:Address>" + options['FaultTo'] + "</wsa:Address></wsa:FaultTo>\n";

        // wsa:Action (required)
        headers += "<wsa:Action>" + options['action'] + "</wsa:Action>\n"
        
        return headers;
    },

    _getRealScope : function(fn) {
        var scope = window;
        if (fn._cscope) scope = fn._cscope;
        return function() {
            return fn.apply(scope, arguments);
        }
    },

    _bind : function(fn, obj) {
        fn._cscope = obj;
        return this._getRealScope(fn);

    },
    
    // workaround for the browser-specific differences in getElementsByTagName
    _firstElement : function (node, namespace, localName) {
        if (node == null) return null;
        var browser = WSRequest.util._getBrowser();
        if (browser == "ie" || browser == "ie7") {
            doc = (node.nodeType == 9 ? node : node.ownerDocument);
            doc.setProperty("SelectionNamespaces", "xmlns:soap='" + namespace + "'");
            el = node.selectSingleNode(".//soap:" + localName);
        } else {
            // Some Firefox DOMs recognize namespaces ...
            el = node.getElementsByTagNameNS(namespace, localName)[0];
            if (el == undefined)
                // ... and some don't.
                el = node.getElementsByTagName(localName)[0];
        }
        return el;
    }
    
    
};



