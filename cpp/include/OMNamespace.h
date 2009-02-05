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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OMNAMESPACE_H
#define OMNAMESPACE_H

#include <WSFDefines.h>
#include <stdio.h>
#include <string>
#include <OMObject.h>
#include <axiom_namespace.h>
#include <AxisFault.h>
#include <OMException.h>

/**
 * @file OMNamespace.h
 */

/**
 * @brief namespace wso2wsf Namespace of WSO2 Web Services Framework.
 */
namespace wso2wsf
{
    /**
     * @defgroup om_namespace OM Namespace
     * @ingroup wso2wsf
     * @{
     */

    /**
     * @brief class OMNamespace Defines the base class used to represent namespaces in
     * the Axis Object Model. Namespaces are typically composed of a prefix and URI pair
     * which identifies the particular occurence. This class provides an abstraction to
     * namespace associated with the XML model.
     */
    class OMNamespace : public OMObject
    {
        /**
         * Overloaded insertion operator, is used for printing the namespace onto a console.
         * @param str existing stream.
         * @param ns OMNamespace object.
         * @return a modified output stream is returned.
         */
        friend WSF_EXTERN std::ostream & WSF_CALL operator <<(std::ostream & str, OMNamespace const & ns);

        /**
         * Overloaded insertion operator, is used for printing the namespace onto a console.
         * @param str existing stream.
         * @param ns OMNamespace object.
         * @return a modified output stream is returned.
         */
        friend WSF_EXTERN std::ostream & WSF_CALL operator <<(std::ostream & str, OMNamespace const * ns);

    private:
        /**
         * @var _wsf_axiom_namespace protected variable holds reference for
         * axiom_namespace struct.
         */
        axiom_namespace_t * _wsf_axiom_namespace;

    public:
        /**
         * Method to get the reference to the axiom_namespace struct.
         * @return reference to the axiom_namespace struct.
         */
        axiom_namespace_t * WSF_CALL getAxiomNamespace();

        /**
         * Method to set the reference to the axiom_namespace struct.
         * @param ns reference to the axiom_namespace struct.
         */
        void WSF_CALL setAxiomNamespace(axiom_namespace_t * ns);

        /**
         * Creates a namespace struct.
         * @param uri namespace URI.
         * @param prefix namespace prefix.
         * @exception OMException an exception is thrown if something goes wrong
         * while creating the object.
         */
        WSF_EXTERN WSF_CALL OMNamespace(std::string uri, std::string prefix) throw (OMException);

        /**
         * Copy Constructor.
         * @param om_namespace namespace to be cloned.
         * @exception OMException an exception is thrown if something goes wrong
         * while creating the object.
         */
        WSF_EXTERN WSF_CALL OMNamespace(OMNamespace & om_namespace) throw (OMException);

        /**
         * destructor that frees resources.
         */
        virtual WSF_CALL ~OMNamespace();

        /**
         * Compares two namespaces.
         * @param ns namespace to be compared.
         * @return true if the two namespaces are equal, false otherwise.
         */
        WSF_EXTERN bool WSF_CALL equals(OMNamespace * ns);

        /**
         * Method to obtain URI.
         * @return namespace uri, NULL on error.
         */
        WSF_EXTERN std::string WSF_CALL getURI();

        /**
         * Method to obtain prefix.
         * @return prefix, NULL on error.
         */
        WSF_EXTERN std::string WSF_CALL getPrefix();

        /**
         * Converts this namespace to a string.
         * @return converted namespace as string.
         */
        WSF_EXTERN virtual std::string WSF_CALL toString();
    };
    /** @} */

    inline std::ostream & WSF_CALL operator <<(std::ostream & str, OMNamespace const & ns)
    {
        return str << const_cast<OMNamespace &>(ns).toString();
    }

    inline std::ostream & WSF_CALL operator <<(std::ostream & str, OMNamespace const * ns)
    {
        return str << const_cast<OMNamespace *>(ns)->toString();
    }
}
#endif // OMNAMESPACE_H
