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

#ifndef NEETHIEXCEPTION_H
#define NEETHIEXCEPTION_H

#include <WSFDefines.h>
#include <Exception.h>
#include <NeethiObject.h>

/**
 * @file NeethiException.h
 */

/**
 * @brief namespace wso2wsf Namespace of WSO2 Web Services Framework.
 */
namespace wso2wsf
{
    /**
     * @defgroup neethi_exception NeethiException
     * @ingroup wso2wsf
     * @{
     */

    /**
     * @brief class NeethiException This Class is a derivation of the Exception Class, and
     * is used to implement exceptions generated within the neethi objects used in WSF/C++.
     */
    class NeethiException: public Exception
    {
    private:
        /**
         * @var _sender private variable holds the reference to the
         * sender of the exception.
         */
        NeethiObject * _sender;

    public:
        /**
         * Constructor accepting a reason for the exception.
         * @param reason reason to exception.
         */
        WSF_CALL NeethiException(char const * reason);

        /**
         * Constructor accepting a reason for the exception, and the
         * object that triggered the exception.
         * @param reason reason to exception.
         * @param sender object that triggered the exception.
         */
        WSF_CALL NeethiException(char const * reason, NeethiObject * sender);

        /**
         * Method for doing the required processing, of the Neethi Exception.
         */
        WSF_EXTERN void WSF_CALL process() const;

        /**
         * destructor that can be overridden.
         */
        virtual WSF_CALL ~NeethiException();
    };
    /** @} */
}
#endif // NEETHIEXCEPTION_H
