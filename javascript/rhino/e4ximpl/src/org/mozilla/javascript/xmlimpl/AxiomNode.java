/*
* Copyright 2005,2006 WSO2, Inc. http://wso2.com
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

package org.mozilla.javascript.xmlimpl;

import org.apache.axiom.om.*;
import org.mozilla.javascript.ScriptRuntime;
import org.mozilla.javascript.ObjArray;

import java.util.Iterator;
import java.util.HashMap;
import java.util.ArrayList;

/**
 * value of type XML represents an XML element, attribute, comment, processing-instruction or
 * text node.
 * EcmaScript - 357 specification.
 * But in axiom element,comment,processing-instruction and text node implement the OMNode interface.
 * attribute implements OMAttribute interface.
 * AxiomNode is written to wrap those two types into a one class.
 */

public class AxiomNode {
    private OMNode omNode;
    private OMAttribute omAttribute;

    private boolean isOMNode = false;
    private boolean isOMAttribute = false;
    private boolean isOMElement = false;
    private boolean isOMComment = false;
    private boolean isOMText = false;
    private boolean isOMPI = false;

    private AxiomNode parentNode;
    XML xmlObject;
    private OMFactory omFactory = null;
    private AxiomNodeMatcher nodeMatcher = null;

    public static final int APPEND_CHILD = 1;
    public static final int PREPEND_CHILD = 2;

    public static final String UNSUPPORTED_OP = "Operation is supported only for XML Elements";
    public static final String INVALID_CHILD_INDEX = "Invalid child index";
    public static final String NODE_MATCHER = "NodeMatcher";
    public static final String DESCENDENT_CHILDREN_MATCHER = "DescendantChildrenMatcher";
    public static final String DESCENDEN_ATTRIBUTE_MATCHER = "DescendantAttributeMatcher";

    public static final HashMap matcherMap = new HashMap(3);


    private AxiomNode(Object value) {
        if (value instanceof OMNode) {
            omNode = (OMNode) value;
            isOMNode = true;

            if (omNode instanceof OMElement) {
                isOMElement = true;

            } else if (omNode instanceof OMText) {
                isOMText = true;

            } else if (omNode instanceof OMComment) {
                isOMComment = true;

            } else if (omNode instanceof OMProcessingInstruction) {
                isOMPI = true;

            }

        } else if (value instanceof OMAttribute) {
            omAttribute = (OMAttribute) value;
            isOMAttribute = true;

        }
    }

    public void addInScopeNamespace(Namespace value) {
        if (!isElement())
            throw ScriptRuntime.typeError(UNSUPPORTED_OP);

        OMElement element = this.getOMElement();
        if (value != null) {
            OMNamespace ns = this.getOMFactory().createOMNamespace(value.uri(), value.prefix());
            element.declareNamespace(ns);
        }
    }

    private void addToList(AxiomNode tmpAxiomNode, XMLList xmlList) {
        if (tmpAxiomNode.isText() && (tmpAxiomNode.getOMText().getTextCharacters())[0] == '\n') {
        } else {
            xmlList.addAxiomNode(tmpAxiomNode);
        }
    }

    /**
     * Retrieve the (index)th child of the current element and covered in an AxiomNode
     *
     * @param index (zero-based) of the child to retrieved
     * @return the child element at index
     */
    public AxiomNode childAt(int index) {
        if (!isOMElement)
            throw ScriptRuntime.typeError(UNSUPPORTED_OP);
        if (index < 0)
            throw ScriptRuntime.typeError(INVALID_CHILD_INDEX);

        int length = 0;
        OMNode result;
        Iterator iterator = this.getOMElement().getChildren();
        while (iterator.hasNext()) {
            result = (OMNode) iterator.next();
            if (length == index) {
                return buildAxiomNode(result, this);
            }
            length++;
        }
        return null;
    }

    /**
     * Clone this OMNode.
     * Cloning depends on the type of the OMNode
     *
     * @return Cloned OMNode
     */
    public OMNode cloneOMNode() {
        if (isOMElement) {
            return getOMElement().cloneOMElement();

        } else if (isOMText) {
            return getOMFactory().createOMText(getOMText().getText());

        } else if (isOMComment) {
            System.out.println("clonning comments is not implemented;");
            //return factory.createOMComment(null,((OMComment)obj).getValue());

        } else if (isOMPI) {
            System.out.println("Clonning PI is not implemented");
            //return factory.createOMComment(null,((OMComment)obj).getValue());
        }
        return null;
    }

    /**
     * Detach this AxiomNode form the its Parent Elment
     *
     * @return whether this AxiomNode is detached from the parent Element
     */
    public boolean detach() {
        try {
            if (isOMNode) {
                if (omNode.getParent() != null)
                    omNode.detach();

            } else if (isOMAttribute) {
                //parent Element should be an OMElement
                OMElement parentElement = getParentNode().getOMElement();
                parentElement.removeAttribute(omAttribute);

            } else {
                return false;
            }
        } catch (Exception e) {
            return false;
        }
        return true;
    }

    public XMLList getChildElements(XMLName xmlName, XMLList xmlList) {
        if (!isElement()) {
            //throw ScriptRuntime.typeError(UNSUPPORTED_OP);
            return null;
        }

        javax.xml.namespace.QName qname = new javax.xml.namespace.QName(xmlName.uri(),
                xmlName.localName());
        Iterator iterator;

        if (this.isElement()) {

            if (xmlName.localName().equals("*")) {
                iterator = this.getOMElement().getChildElements();

                while (iterator.hasNext())
                    xmlList.addAxiomNode(AxiomNode.buildAxiomNode(iterator.next(), this));

            } else {

                iterator = this.getOMElement().getChildElements();
                while (iterator.hasNext()) {
                    AxiomNode tmpAxiomNode = AxiomNode.buildAxiomNode(iterator.next(), this);

                    if (qname.equals(((OMElement) tmpAxiomNode.getOMNode()).getQName()))
                        xmlList.addAxiomNode(tmpAxiomNode);
                }
            }
        }
        return xmlList;
    }

    public int getChildIndex() {
        int childIndex = -1;
        AxiomNode parentNode = getParentNode();

        if (parentNode == null || isAttribute())
            return childIndex;

        else if (parentNode.isElement()) {
            OMElement omElement = parentNode.getOMElement();
            Iterator iterator = omElement.getChildren();

            while (iterator.hasNext()) {
                OMNode omNode = (OMNode) iterator.next();
                childIndex++;
                if (omNode == this.getOMNode()) {
                    break;
                }
            }
            return childIndex;
        }
        return childIndex;
    }

    public XML getElemetFromText(XMLLibImpl lib, XMLName xmlName, String value) {
        XML xml;
        OMElement omElement = getOMFactory().createOMElement(xmlName.localName(),
                xmlName.uri(), "");
        omElement.setText(value);
        xml = XML.toXML(lib, omElement.toString());
        return xml;
    }

    public Object[] getInScopeNamespace(XMLLibImpl lib) {
        OMNamespace ns;
        ObjArray nsDeclarations = new ObjArray();
        OMElement omElement = this.getOMElement();
        HashMap nsMap = new HashMap();

        while (omElement != null) {
            Iterator iterator = omElement.getAllDeclaredNamespaces();

            while (iterator.hasNext()) {
                ns = (OMNamespace) iterator.next();
                Namespace namespace = new Namespace(lib, ns.getPrefix(), ns.getNamespaceURI());
                if (nsMap.get(ns.getPrefix()) == null) {
                    nsDeclarations.add(namespace);
                    nsMap.put(ns.getPrefix(), "IN");
                }
            }

            if (!(omElement.getParent() instanceof OMDocument)) {
                omElement = (OMElement) omElement.getParent();

            } else {
                omElement = null;
            }
        }
        nsMap = null;
        return nsDeclarations.toArray();
    }

    public OMAttribute getOMAttribute() {
        return (isAttribute()) ? (OMAttribute) omAttribute : null;
    }

    public OMComment getOMComment() {
        return (isOMComment) ? (OMComment) omNode : null;
    }

    public OMElement getOMElement() {
        return (isOMElement) ? (OMElement) omNode : null;
    }

    public OMFactory getOMFactory() {
        if (isOMAttribute) {
            omFactory = omAttribute.getOMFactory();

        } else if (omNode != null) {
            omFactory = omNode.getOMFactory();

        } else {
            omFactory = OMAbstractFactory.getOMFactory();

        }
        return omFactory;
    }

    public OMNode getOMNode() {
        return omNode;
    }

    public OMProcessingInstruction getOMProcessingInstruction() {
        return (isProcessingInstruction()) ? (OMProcessingInstruction) omNode : null;
    }

    public OMText getOMText() {
        return (isOMText) ? (OMText) omNode : null;
    }

    public AxiomNode getParentNode() {
        return parentNode;
    }

    /**
     * If this AxiomNode represents a Comment or a Text Node QName will be null;
     *
     * @return QName of this AxiomNode
     */
    public javax.xml.namespace.QName getQName() {
        javax.xml.namespace.QName qname = null;

        if (isText() || isComment()) {
            return null;

        } else if (isElement()) {
            qname = this.getOMElement().getQName();

        } else if (isAttribute()) {
            qname = this.getOMAttribute().getQName();

        } else if (isProcessingInstruction()) {
            //TODO handle PI in the name method
            //OMProcessingInstruction omPI = (OMProcessingInstruction) axiomNode.getNode();
            //qname = new QName(lib, omPI.getQName().getNamespaceURI(),
            // omPI.getQName().getLocalPart(), "");
            qname = null;
        }
        return qname;
    }

    public XML getXMLObject(XMLLibImpl lib) {
        if (xmlObject == null) {
            //TODO CHANGE -- lib
            xmlObject = new XML(lib, this);

        }
        return xmlObject;
    }

    private void insertChild(AxiomNode nodeToInsert, int mode) {
        if (!isElement())
            throw ScriptRuntime.typeError(UNSUPPORTED_OP);

        if (mode == PREPEND_CHILD) {
            AxiomNode refNode = childAt(0);
            if (refNode != null) {
                insertChild(refNode, nodeToInsert, mode);
                return;
            }
        }
        //TODO Check with  the defaultSettings.
        this.getOMElement().addChild(nodeToInsert.cloneOMNode());
    }

    public void insertChild(AxiomNode refNode, AxiomNode nodeToInsert, int mode) {
        if (refNode == null) {
            insertChild(nodeToInsert, mode);

        } else if (refNode.getOMNode() != null && nodeToInsert.getOMNode() != null) {

            //TODO Check with  the defaultSettings.
            switch (mode) {
                case PREPEND_CHILD: {
                    OMNode prevSibling = refNode.getOMNode().getPreviousOMSibling();

                    if (prevSibling == null) {
                        refNode.getParentNode().getOMElement().setFirstChild(nodeToInsert.cloneOMNode());
                        nodeToInsert.getOMNode().insertSiblingAfter(refNode.getOMNode());

                    } else {
                        prevSibling.insertSiblingAfter(nodeToInsert.cloneOMNode());

                    }

                    break;
                }
                default: {
                    refNode.getOMNode().insertSiblingAfter(nodeToInsert.cloneOMNode());
                }
            }
        }
    }

    void insertAttribute(XMLName xmlName, Object value) {
        String uri = xmlName.uri();
        String localName = xmlName.localName();
        OMNamespace omNamespace;
        OMElement omElement;

        if (uri == null || localName.equals("*")) {
            throw ScriptRuntime.typeError("@* assignment not supported.");
        }

        String strValue = ScriptRuntime.toString(value);
        if (isElement()) {
            omElement = this.getOMElement();
            omNamespace = omElement.getOMFactory().createOMNamespace(uri, "");
            OMAttribute at = omElement.getOMFactory().createOMAttribute(localName, omNamespace, strValue);
            omElement.addAttribute(at);
        }
    }

    public boolean isAttribute() {
        return isOMAttribute;
    }

    public boolean isComment() {
        return isOMComment;
    }

    public boolean isElement() {
        return isOMElement;
    }

    public boolean isNull() {
        return isOMAttribute || isOMNode;
    }

    public boolean isOMNode() {
        return isOMNode;
    }

    public boolean isProcessingInstruction() {
        return isOMPI;
    }

    public boolean isText() {
        return isOMText;
    }

    public XMLList matchNodes(XMLName xmlName, XMLList xmlList, int type) {
        if (!isElement())
            throw ScriptRuntime.typeError(UNSUPPORTED_OP);

        if (xmlName.localName().equals(XMLName.ANY_NAME) && xmlName.uri() == null) {
            xmlList = nodeMatcher.matchAnyQName(this, xmlList, type);
            //System.out.println("case 1");
        } else if (xmlName.localName().equals(XMLName.ANY_NAME)) {
            xmlList = nodeMatcher.matchAnyLocalName(this, xmlList, type, xmlName.uri());
            //System.out.println("case 2");
        } else if (xmlName.uri() == null || xmlName.uri().equals("")) {
            xmlList = nodeMatcher.matchAnyNamespace(this, xmlList, type, xmlName.localName(), xmlName.uri());
            //System.out.println("case 3");
        } else {
            xmlList = nodeMatcher.matchQName(this, xmlList, type, new javax.xml.namespace.QName(xmlName.uri(), xmlName.localName()));
            //System.out.println("case 4");                          ,
        }
        return xmlList;
    }

    public void removeChild(AxiomNode axiomNode) {
        if (!isElement())
            throw ScriptRuntime.typeError(UNSUPPORTED_OP);

        if (axiomNode.getOMNode() == null) {
            System.out.println(" remove Child null Omnode");
            return;
        }

        if (axiomNode.isAttribute()) {
            this.getOMElement().removeAttribute(axiomNode.getOMAttribute());

        } else if (axiomNode.getOMNode() != null) {
            axiomNode.detach();
        }
    }

    /**
     * Set the corresponding matcher inner class. Instances of the inners classes are stored in a
     * HashMap,
     *
     * @param matcherName
     */
    public void setNodeMatcher(String matcherName) {

        AxiomNodeMatcher nodeMatcher = (AxiomNodeMatcher) matcherMap.get(matcherName);
        if (nodeMatcher == null) {
            if (matcherName.equals(NODE_MATCHER)) {
                nodeMatcher = new NodeMatcher();

            } else if (matcherName.equals(DESCENDENT_CHILDREN_MATCHER)) {
                nodeMatcher = new DescendantChildrenMatcher();

            } else if (matcherName.equals(DESCENDEN_ATTRIBUTE_MATCHER)) {
                nodeMatcher = new DescendantAttributeMatcher();

            }
            matcherMap.put(matcherName, nodeMatcher);
        }
        this.nodeMatcher = nodeMatcher;
    }

    public void setParentNode(AxiomNode parentNode) {
        this.parentNode = parentNode;
    }

    //Static methods
    static AxiomNode buildAxiomNode(Object value, AxiomNode parent) {
        AxiomNode axiomNode = new AxiomNode(value);
        if (parent != null && parent.isOMElement)
            axiomNode.setParentNode(parent);
        return axiomNode;
    }

    /**
     * Inner class NodeMatcher
     * Match x.y, x.ns::y, x.@, x.@ns::y expressions
     */
    private static class NodeMatcher implements AxiomNodeMatcher {

        //match any localName and any namespaceURI
        public synchronized XMLList matchAnyQName(AxiomNode thisAxiomNode, XMLList xmlList, int type) {
            Iterator iterator = null;
            if (type == AxiomNodeMatcher.CHILDREN) {
                iterator = thisAxiomNode.getOMElement().getChildren();

            } else if (type == AxiomNodeMatcher.ATTRIBUTES) {
                iterator = thisAxiomNode.getOMElement().getAllAttributes();
            }
            
            AxiomNode tmpAxiomNode;
            while (iterator.hasNext()) {
                tmpAxiomNode = AxiomNode.buildAxiomNode(iterator.next(), thisAxiomNode);
                thisAxiomNode.addToList(tmpAxiomNode, xmlList);
            }

            return xmlList;
        }

        //match any localName with a given namespaceURI
        public synchronized XMLList matchAnyLocalName(AxiomNode thisAxiomNode, XMLList xmlList, int type, String namespaceURI) {
            Iterator iterator = null;
            if (type == AxiomNodeMatcher.CHILDREN) {
                iterator = thisAxiomNode.getOMElement().getChildren();

            } else if (type == AxiomNodeMatcher.ATTRIBUTES) {
                iterator = thisAxiomNode.getOMElement().getAllAttributes();

            }

            AxiomNode tmpAxiomNode;
            while (iterator.hasNext()) {
                tmpAxiomNode = AxiomNode.buildAxiomNode(iterator.next(), thisAxiomNode);
                javax.xml.namespace.QName qname = tmpAxiomNode.getQName();

                if ((namespaceURI.equals("")) || (qname != null &&
                        qname.getNamespaceURI().equals(namespaceURI)) || (tmpAxiomNode.isText())) {
                    thisAxiomNode.addToList(tmpAxiomNode, xmlList);

                }
            }
            return xmlList;
        }

        //match any namespace with a given localName
        public synchronized XMLList matchAnyNamespace(AxiomNode thisAxiomNode, XMLList xmlList, int type, String localName, String namespaceURI) {
            Iterator iterator = null;
            if (type == AxiomNodeMatcher.CHILDREN) {
                iterator = thisAxiomNode.getOMElement().getChildren();

            } else if (type == AxiomNodeMatcher.ATTRIBUTES) {
                iterator = thisAxiomNode.getOMElement().getAllAttributes();

            }

            AxiomNode tmpAxiomNode;
            while (iterator.hasNext()) {
                tmpAxiomNode = AxiomNode.buildAxiomNode(iterator.next(), thisAxiomNode);
                javax.xml.namespace.QName qname = tmpAxiomNode.getQName();

                if (qname != null && qname.getLocalPart().equals(localName)) {

                    if ((namespaceURI == null) || (qname.getNamespaceURI().equals(namespaceURI))) {
                        thisAxiomNode.addToList(tmpAxiomNode, xmlList);
                    }
                }
            }
            return xmlList;
        }

        //match given QName;
        public synchronized XMLList matchQName(AxiomNode thisAxiomNode, XMLList xmlList, int type, javax.xml.namespace.QName qname) {
            AxiomNode tmpAxiomNode;
            Iterator iterator;
            if (type == AxiomNodeMatcher.CHILDREN) {
                iterator = thisAxiomNode.getOMElement().getChildrenWithName(qname);

                while (iterator.hasNext()) {
                    tmpAxiomNode = AxiomNode.buildAxiomNode(iterator.next(), thisAxiomNode);
                    thisAxiomNode.addToList(tmpAxiomNode, xmlList);
                }

            } else if (type == AxiomNodeMatcher.ATTRIBUTES) {
                iterator = thisAxiomNode.getOMElement().getAllAttributes();

                while (iterator.hasNext()) {
                    tmpAxiomNode = AxiomNode.buildAxiomNode(iterator.next(), thisAxiomNode);

                    if (qname.equals(tmpAxiomNode.getOMAttribute().getQName())) {
                        thisAxiomNode.addToList(tmpAxiomNode, xmlList);
                    }
                }
            }
            return xmlList;
        }
    }

    /**
     * Inner class DescendantChildrenMatcher
     * Match x..y, x..ns::y, x..* expressions
     */
    private static class DescendantChildrenMatcher implements AxiomNodeMatcher {
        //match any localName and any namespaceURI
        public synchronized XMLList matchAnyQName(AxiomNode thisAxiomNode, XMLList xmlList, int type) {
            matchAnyQNameRec(thisAxiomNode, xmlList, thisAxiomNode);
            return xmlList;
        }

        //match any localName with a given namespaceURI
        public synchronized XMLList matchAnyLocalName(AxiomNode thisAxiomNode, XMLList xmlList, int type, String namespaceURI) {
            matchAnyLocalNameRec(thisAxiomNode, xmlList, namespaceURI, thisAxiomNode);
            return xmlList;
        }

        //match any namespace with a given localName
        public synchronized XMLList matchAnyNamespace(AxiomNode thisAxiomNode, XMLList xmlList, int type, String localName, String namespaceURI) {
            matchAnyNamespaceRec(thisAxiomNode, xmlList, localName, namespaceURI, thisAxiomNode);
            return xmlList;
        }

        //match given QName;
        public synchronized XMLList matchQName(AxiomNode thisAxiomNode, XMLList xmlList, int type, javax.xml.namespace.QName qname) {
            matchQNameRec(thisAxiomNode, xmlList, qname, thisAxiomNode);
            return xmlList;
        }

        private void matchAnyQNameRec(AxiomNode thisAxiomNode, XMLList xmlList, AxiomNode axiomNode) {
            AxiomNode tmpAxiomNode;
            Iterator iterator = axiomNode.getOMElement().getChildren();
            while (iterator.hasNext()) {

                tmpAxiomNode = AxiomNode.buildAxiomNode(iterator.next(), axiomNode);
                thisAxiomNode.addToList(tmpAxiomNode, xmlList);

                if (tmpAxiomNode.isOMElement) {
                    matchAnyQNameRec(thisAxiomNode, xmlList, tmpAxiomNode);
                }
            }
        }

        private void matchAnyLocalNameRec(AxiomNode thisAxiomNode, XMLList xmlList, String namespaceURI, AxiomNode axiomNode) {
            AxiomNode tmpAxiomNode;

            Iterator iterator = axiomNode.getOMElement().getChildren();
            while (iterator.hasNext()) {
                tmpAxiomNode = AxiomNode.buildAxiomNode(iterator.next(), axiomNode);
                javax.xml.namespace.QName qname = tmpAxiomNode.getQName();

                if ((namespaceURI.equals("")) || (qname != null &&
                        qname.getNamespaceURI().equals(namespaceURI)) || (tmpAxiomNode.isText())) {
                    thisAxiomNode.addToList(tmpAxiomNode, xmlList);
                }

                if (tmpAxiomNode.isElement()) {
                    matchAnyLocalNameRec(thisAxiomNode, xmlList, namespaceURI, tmpAxiomNode);
                }
            }
        }

        private void matchAnyNamespaceRec(AxiomNode thisAxiomNode, XMLList xmlList, String localName, String namespaceURI, AxiomNode axiomNode) {
            AxiomNode tmpAxiomNode;
            Iterator iterator = axiomNode.getOMElement().getChildren();
            while (iterator.hasNext()) {

                tmpAxiomNode = AxiomNode.buildAxiomNode(iterator.next(), axiomNode);
                javax.xml.namespace.QName qname = tmpAxiomNode.getQName();

                if (qname != null && localName.equals(tmpAxiomNode.getQName().getLocalPart()))
                    if ((namespaceURI == null) || (qname.getNamespaceURI().equals(namespaceURI))) {
                        thisAxiomNode.addToList(tmpAxiomNode, xmlList);
                    }

                if (tmpAxiomNode.isElement())
                    matchAnyNamespaceRec(thisAxiomNode, xmlList, localName, namespaceURI, tmpAxiomNode);
            }
        }

        private void matchQNameRec(AxiomNode thisAxiomNode, XMLList xmlList, javax.xml.namespace.QName qname, AxiomNode axiomNode) {
            AxiomNode tmpAxiomNode;
            Iterator iterator = axiomNode.getOMElement().getChildren();
            while (iterator.hasNext()) {

                tmpAxiomNode = AxiomNode.buildAxiomNode(iterator.next(), axiomNode);
                javax.xml.namespace.QName tmpQName = tmpAxiomNode.getQName();

                if (tmpQName != null && qname.equals(tmpQName)) {
                    thisAxiomNode.addToList(tmpAxiomNode, xmlList);
                }

                if (tmpAxiomNode.isElement())
                    matchQNameRec(thisAxiomNode, xmlList, qname, tmpAxiomNode);
            }
        }
    }

    /**
     * Inner class DescendantAttributeMatcher
     * Match x..@y, x..@ns::y, x..@* expressions
     */
    private static class DescendantAttributeMatcher implements AxiomNodeMatcher {

        //match any localName and any namespaceURI
        public XMLList matchAnyQName(AxiomNode thisAxiomNode, XMLList xmlList, int type) {
            matchAnyQNameRec(xmlList, thisAxiomNode);
            return xmlList;
        }

        //match any localName with a given namespaceURI
        public XMLList matchAnyLocalName(AxiomNode thisAxiomNode, XMLList xmlList, int type, String namespaceURI) {
            matchAnyLocalNameRec(xmlList, namespaceURI, thisAxiomNode);
            return xmlList;
        }

        //match any namespace with a given localName
        public XMLList matchAnyNamespace(AxiomNode thisAxiomNode, XMLList xmlList, int type, String localName, String namespaceURI) {
            matchAnyNamespaceRec(xmlList, localName, namespaceURI, thisAxiomNode);
            return xmlList;
        }

        //match given QName;
        public XMLList matchQName(AxiomNode thisAxiomNode, XMLList xmlList, int type, javax.xml.namespace.QName qname) {
            matchQNameRec(xmlList, qname, thisAxiomNode);
            return xmlList;
        }


        private void matchAnyQNameRec(XMLList xmlList, AxiomNode axiomNode) {

            AxiomNode tmpAxiomNode;
            Iterator iteratoAtr = axiomNode.getOMElement().getAllAttributes();

            while (iteratoAtr.hasNext()) {
                tmpAxiomNode = AxiomNode.buildAxiomNode(iteratoAtr.next(), axiomNode);
                xmlList.addAxiomNode(tmpAxiomNode);
            }

            Iterator iteratorEle = axiomNode.getOMElement().getChildElements();
            while (iteratorEle.hasNext()) {
                tmpAxiomNode = AxiomNode.buildAxiomNode(iteratorEle.next(), axiomNode);
                matchAnyQNameRec(xmlList, tmpAxiomNode);
            }
        }

        private void matchAnyLocalNameRec(XMLList xmlList, String namespaceURI, AxiomNode axiomNode) {
            AxiomNode tmpAxiomNode;

            Iterator iterator = axiomNode.getOMElement().getAllAttributes();
            while (iterator.hasNext()) {
                tmpAxiomNode = AxiomNode.buildAxiomNode(iterator.next(), axiomNode);
                javax.xml.namespace.QName qname = tmpAxiomNode.getQName();

                if (qname != null && qname.getNamespaceURI().equals(namespaceURI)) {
                    xmlList.addAxiomNode(tmpAxiomNode);
                }
            }

            Iterator iteratorEle = axiomNode.getOMElement().getChildElements();
            while (iteratorEle.hasNext()) {
                tmpAxiomNode = AxiomNode.buildAxiomNode(iteratorEle.next(), axiomNode);
                matchAnyLocalNameRec(xmlList, namespaceURI, tmpAxiomNode);
            }
        }

        private void matchAnyNamespaceRec(XMLList xmlList, String localName, String namespaceURI, AxiomNode axiomNode) {
            AxiomNode tmpAxiomNode;
            Iterator iterator = axiomNode.getOMElement().getAllAttributes();
            while (iterator.hasNext()) {

                tmpAxiomNode = AxiomNode.buildAxiomNode(iterator.next(), axiomNode);
                javax.xml.namespace.QName qname = tmpAxiomNode.getQName();

                if (qname != null && localName.equals(qname.getLocalPart()))
                    if (namespaceURI == null) {
                        xmlList.addAxiomNode(tmpAxiomNode);
                    } else if (qname.getNamespaceURI().equals(namespaceURI)) {
                        xmlList.addAxiomNode(tmpAxiomNode);
                    }
            }

            Iterator iteratorEle = axiomNode.getOMElement().getChildElements();
            while (iteratorEle.hasNext()) {
                tmpAxiomNode = AxiomNode.buildAxiomNode(iteratorEle.next(), axiomNode);
                matchAnyNamespaceRec(xmlList, localName, namespaceURI, tmpAxiomNode);
            }

        }

        private void matchQNameRec(XMLList xmlList, javax.xml.namespace.QName qname, AxiomNode axiomNode) {
            AxiomNode tmpAxiomNode;
            OMAttribute omAttribute = axiomNode.getOMElement().getAttribute(qname);

            if (omAttribute != null) {
                tmpAxiomNode = AxiomNode.buildAxiomNode(omAttribute, axiomNode);
                xmlList.addAxiomNode(tmpAxiomNode);
            }

            Iterator iterator = axiomNode.getOMElement().getChildElements();
            while (iterator.hasNext()) {
                tmpAxiomNode = AxiomNode.buildAxiomNode(iterator.next(), axiomNode);
                matchQNameRec(xmlList, qname, tmpAxiomNode);
            }
        }
    }

    private static class AxiomNodeEquality {

        public static boolean isOMNodesEqual(AxiomNode refNode, AxiomNode nodeToCompare) {
            boolean result = false;

            if (refNode.isOMAttribute) {

            } else if (refNode.isOMElement) {
                result = isOMElementsEqual(refNode, nodeToCompare);

            } else if (refNode.isOMText) {

            } else if (refNode.isOMPI) {

            } else if (refNode.isOMComment) {

            }
            return result;
        }

        private static boolean isOMAttributesEqual(AxiomNode refNode, AxiomNode nodeToCompare) {
            return false;

        }

        private static boolean isOMElementsEqual(AxiomNode refNode, AxiomNode nodeToCompare) {
            boolean result = false;

            if (!nodeToCompare.isOMElement)
                return result;

            OMElement refEle = refNode.getOMElement();
            OMElement eletoCompare = nodeToCompare.getOMElement();

            if (!isQNamesEqual(refEle.getQName(), eletoCompare.getQName()))
                return result;

            return false;

        }

        private static boolean isOMTextsEqual(AxiomNode refNode, AxiomNode nodeToCompare) {
            return false;
        }

        private static boolean isOMPIsEqual(AxiomNode refNode, AxiomNode nodeToCompare) {
            return false;
        }

        private static boolean isOMCommentsEqual(AxiomNode refNode, AxiomNode nodeToCompare) {
            return false;
        }

        private static boolean isQNamesEqual(javax.xml.namespace.QName refQName,
                                             javax.xml.namespace.QName QNameToCompare) {
            boolean result = false;

            if (refQName.getNamespaceURI().equals(QNameToCompare.getNamespaceURI()) &&
                    (refQName.getLocalPart().equals(QNameToCompare.getLocalPart())))
                return true;

            return result;
        }
    }
}
