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

#ifndef NEETHIPOLICY_H
#define NEETHIPOLICY_H

#include <AxisObject.h>
#include <OMElement.h>
#include <neethi_policy.h>
#include <neethi_util.h>

/**
 * @file NeethiPolicy.h
 */

/**
 * @brief namespace wso2wsf Namespace of WSO2 Web Services Framework.
 */
namespace wso2wsf
{
    /**
     * @defgroup neethi_policy Neethi Policy
     * @ingroup wso2wsf
     * @{
     */

    /**
     * @brief class NeethiPolicy Defines a policy object that can be used to
     * provide WS-Policy information. This is a wrapper to the XML policy
     * advertisement used in security, QoS etc.
     */
    class NeethiPolicy : public AxisObject
    {
    private:
        /**
         * @var _wsf_neethi_policy protected variable holds reference for
         * neethi_policy struct.
         */
        neethi_policy_t * _wsf_neethi_policy;

    public:
        /**
         * Method to get the reference to the neethi_policy struct.
         * @return reference to the neethi_policy struct.
         */
        neethi_policy_t * getNeethiPolicy();

        /**
         * Method to set the reference to the neethi_policy struct.
         * @param policy reference to the neethi_policy struct.
         */
        void setNeethiPolicy(neethi_policy_t * policy);

        /**
         * Creates a neethi_policy struct.
         * @param file_name file name.
         */
        NeethiPolicy(std::string file_name);

        /**
         * Creates a neethi_policy struct.
         * @param element OM element which is the root of WS-Policy XML.
         */
        NeethiPolicy(OMElement * element);

        /**
         * desctructor that frees resources.
         */
        virtual ~NeethiPolicy();

        /**
         * Sets policy name.
         * @param name name to set.
         * @return true if successful, false otherwise.
         */
        bool setName(std::string name);

        /**
         * Gets policy name.
         * @return policy name as string.
         */
        std::string getName();

        /**
         * Sets policy id.
         * @param id id to set.
         * @return true if successful, false otherwise.
         */
        bool setId(std::string id);

        /**
         * Gets policy id.
         * @return policy id as string.
         */
        std::string getId();

    };
    /** @} */
}
#endif // NEETHIPOLICY_H
