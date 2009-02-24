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

#ifndef AXISOBJECT_H
#define AXISOBJECT_H
#include <Process.h>
#include <WSFDefines.h>
#include <axutil_env.h>
#include <string>
#include <AxisFault.h>


/**
 * @file AxisObject.h
 */

/**
 * @brief namespace wso2wsf Namespace of WSO2 Web Services Framework.
 */
namespace wso2wsf
{
    /**
     * @defgroup axis_object Axis Object
     * @ingroup wso2wsf
     * @{
     */

    /**
     * @brief class AxisObject This creates a base object that serves as the
     * building block for other components of the Axis Framework.
     */

    class AxisObject
    {

    protected:
        /**
         * Method to get the reference to the axutil_env struct.
         * @return reference to the axutil_env struct.
         */
        const axutil_env_t * WSF_CALL getEnv();

        /**
         * Constructor setting up the AxisObject.
         */
        WSF_CALL AxisObject();

        /**
         * Enables or Disables logging,
         * @param enable true will enable and false will disable.
         */
        void WSF_CALL enableLogging(bool enable);

    public:
        /**
         * destructor frees resources.
         */
        virtual WSF_CALL ~AxisObject();
    };
    /** @} */
}
#endif // AXISOBJECT_H