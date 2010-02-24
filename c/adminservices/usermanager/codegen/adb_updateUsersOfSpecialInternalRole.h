/*
* Copyright 2009-2010 WSO2, Inc. http://wso2.com
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


        #ifndef ADB_UPDATEUSERSOFSPECIALINTERNALROLE_H
        #define ADB_UPDATEUSERSOFSPECIALINTERNALROLE_H

       /**
        * adb_updateUsersOfSpecialInternalRole.h
        *
        * This file was auto-generated from WSDL
        * by the Apache Axis2/Java version: #axisVersion# #today#
        */

       /**
        *  adb_updateUsersOfSpecialInternalRole class
        */

        

        #include <stdio.h>
        #include <axiom.h>
        #include <axis2_util.h>
        #include <axiom_soap.h>
        #include <axis2_client.h>

        #include "axis2_extension_mapper.h"

        #ifdef __cplusplus
        extern "C"
        {
        #endif

        #define ADB_DEFAULT_DIGIT_LIMIT 1024
        #define ADB_DEFAULT_NAMESPACE_PREFIX_LIMIT 64
        

        typedef struct adb_updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t;

        
        

        /******************************* Create and Free functions *********************************/

        /**
         * Constructor for creating adb_updateUsersOfSpecialInternalRole_t
         * @param env pointer to environment struct
         * @return newly created adb_updateUsersOfSpecialInternalRole_t object
         */
        adb_updateUsersOfSpecialInternalRole_t* AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_create(
            const axutil_env_t *env );

        /**
         * Wrapper for the "free" function, will invoke the extension mapper instead
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object to free
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_free (
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env);

        /**
         * Free adb_updateUsersOfSpecialInternalRole_t object
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object to free
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_free_obj (
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env);



        /********************************** Getters and Setters **************************************/
        /******** Deprecated for array types, Use 'Getters and Setters for Arrays' instead ***********/
        

        /**
         * Getter for roleName. 
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @return axis2_char_t*
         */
        axis2_char_t* AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_get_roleName(
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env);

        /**
         * Setter for roleName.
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @param arg_roleName axis2_char_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_set_roleName(
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env,
            const axis2_char_t*  arg_roleName);

        /**
         * Resetter for roleName
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_reset_roleName(
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env);

        
        

        /**
         * Getter for usersShown. Deprecated for array types, Use adb_updateUsersOfSpecialInternalRole_get_usersShown_at instead
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @return Array of axis2_char_t*s.
         */
        axutil_array_list_t* AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_get_usersShown(
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env);

        /**
         * Setter for usersShown.Deprecated for array types, Use adb_updateUsersOfSpecialInternalRole_set_usersShown_at
         * or adb_updateUsersOfSpecialInternalRole_add_usersShown instead.
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @param arg_usersShown Array of axis2_char_t*s.
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_set_usersShown(
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env,
            axutil_array_list_t*  arg_usersShown);

        /**
         * Resetter for usersShown
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_reset_usersShown(
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env);

        
        

        /**
         * Getter for usersSelected. Deprecated for array types, Use adb_updateUsersOfSpecialInternalRole_get_usersSelected_at instead
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @return Array of axis2_char_t*s.
         */
        axutil_array_list_t* AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_get_usersSelected(
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env);

        /**
         * Setter for usersSelected.Deprecated for array types, Use adb_updateUsersOfSpecialInternalRole_set_usersSelected_at
         * or adb_updateUsersOfSpecialInternalRole_add_usersSelected instead.
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @param arg_usersSelected Array of axis2_char_t*s.
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_set_usersSelected(
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env,
            axutil_array_list_t*  arg_usersSelected);

        /**
         * Resetter for usersSelected
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_reset_usersSelected(
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env);

        
        /****************************** Getters and Setters For Arrays **********************************/
        /************ Array Specific Operations: get_at, set_at, add, remove_at, sizeof *****************/

        /**
         * E.g. use of get_at, set_at, add and sizeof
         *
         * for(i = 0; i < adb_element_sizeof_property(adb_object, env); i ++ )
         * {
         *     // Getting ith value to property_object variable
         *     property_object = adb_element_get_property_at(adb_object, env, i);
         *
         *     // Setting ith value from property_object variable
         *     adb_element_set_property_at(adb_object, env, i, property_object);
         *
         *     // Appending the value to the end of the array from property_object variable
         *     adb_element_add_property(adb_object, env, property_object);
         *
         *     // Removing the ith value from an array
         *     adb_element_remove_property_at(adb_object, env, i);
         *     
         * }
         *
         */

        
        
        /**
         * Get the ith element of usersShown.
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @param i index of the item to return
         * @return ith axis2_char_t* of the array
         */
        axis2_char_t* AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_get_usersShown_at(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                const axutil_env_t *env, int i);

        /**
         * Set the ith element of usersShown. (If the ith already exist, it will be replaced)
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @param i index of the item to return
         * @param arg_usersShown element to set axis2_char_t* to the array
         * @return ith axis2_char_t* of the array
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_set_usersShown_at(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                const axutil_env_t *env, int i,
                const axis2_char_t* arg_usersShown);


        /**
         * Add to usersShown.
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @param arg_usersShown element to add axis2_char_t* to the array
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_add_usersShown(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                const axutil_env_t *env,
                const axis2_char_t* arg_usersShown);

        /**
         * Get the size of the usersShown array.
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct.
         * @return the size of the usersShown array.
         */
        int AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_sizeof_usersShown(
                    adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                    const axutil_env_t *env);

        /**
         * Remove the ith element of usersShown.
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @param i index of the item to remove
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_remove_usersShown_at(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                const axutil_env_t *env, int i);

        
        
        /**
         * Get the ith element of usersSelected.
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @param i index of the item to return
         * @return ith axis2_char_t* of the array
         */
        axis2_char_t* AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_get_usersSelected_at(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                const axutil_env_t *env, int i);

        /**
         * Set the ith element of usersSelected. (If the ith already exist, it will be replaced)
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @param i index of the item to return
         * @param arg_usersSelected element to set axis2_char_t* to the array
         * @return ith axis2_char_t* of the array
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_set_usersSelected_at(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                const axutil_env_t *env, int i,
                const axis2_char_t* arg_usersSelected);


        /**
         * Add to usersSelected.
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @param arg_usersSelected element to add axis2_char_t* to the array
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_add_usersSelected(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                const axutil_env_t *env,
                const axis2_char_t* arg_usersSelected);

        /**
         * Get the size of the usersSelected array.
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct.
         * @return the size of the usersSelected array.
         */
        int AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_sizeof_usersSelected(
                    adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                    const axutil_env_t *env);

        /**
         * Remove the ith element of usersSelected.
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @param i index of the item to remove
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_remove_usersSelected_at(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                const axutil_env_t *env, int i);

        


        /******************************* Checking and Setting NIL values *********************************/
        /* Use 'Checking and Setting NIL values for Arrays' to check and set nil for individual elements */

        /**
         * NOTE: set_nil is only available for nillable properties
         */

        

        /**
         * Check whether roleName is nill
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_is_roleName_nil(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                const axutil_env_t *env);


        
        /**
         * Set roleName to nill (currently the same as reset)
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_set_roleName_nil(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                const axutil_env_t *env);
        

        /**
         * Check whether usersShown is nill
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_is_usersShown_nil(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                const axutil_env_t *env);


        
        /**
         * Set usersShown to nill (currently the same as reset)
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_set_usersShown_nil(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                const axutil_env_t *env);
        

        /**
         * Check whether usersSelected is nill
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_is_usersSelected_nil(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                const axutil_env_t *env);


        
        /**
         * Set usersSelected to nill (currently the same as reset)
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_set_usersSelected_nil(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                const axutil_env_t *env);
        
        /*************************** Checking and Setting 'NIL' values in Arrays *****************************/

        /**
         * NOTE: You may set this to remove specific elements in the array
         *       But you can not remove elements, if the specific property is declared to be non-nillable or sizeof(array) < minOccurs
         */
        
        /**
         * Check whether usersShown is nill at i
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct.
         * @param i index of the item to return.
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_is_usersShown_nil_at(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                const axutil_env_t *env, int i);
 
       
        /**
         * Set usersShown to nill at i
         * @param  _updateUsersOfSpecialInternalRole _ adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct.
         * @param i index of the item to set.
         * @return AXIS2_SUCCESS on success, or AXIS2_FAILURE otherwise.
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_set_usersShown_nil_at(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole, 
                const axutil_env_t *env, int i);

        
        /**
         * Check whether usersSelected is nill at i
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct.
         * @param i index of the item to return.
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_is_usersSelected_nil_at(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                const axutil_env_t *env, int i);
 
       
        /**
         * Set usersSelected to nill at i
         * @param  _updateUsersOfSpecialInternalRole _ adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct.
         * @param i index of the item to set.
         * @return AXIS2_SUCCESS on success, or AXIS2_FAILURE otherwise.
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_set_usersSelected_nil_at(
                adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole, 
                const axutil_env_t *env, int i);

        

        /**************************** Serialize and Deserialize functions ***************************/
        /*********** These functions are for use only inside the generated code *********************/

        
        /**
         * Wrapper for the deserialization function, will invoke the extension mapper instead
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @param dp_parent double pointer to the parent node to deserialize
         * @param dp_is_early_node_valid double pointer to a flag (is_early_node_valid?)
         * @param dont_care_minoccurs Dont set errors on validating minoccurs, 
         *              (Parent will order this in a case of choice)
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_deserialize(
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env,
            axiom_node_t** dp_parent,
            axis2_bool_t *dp_is_early_node_valid,
            axis2_bool_t dont_care_minoccurs);

        /**
         * Deserialize an XML to adb objects
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @param dp_parent double pointer to the parent node to deserialize
         * @param dp_is_early_node_valid double pointer to a flag (is_early_node_valid?)
         * @param dont_care_minoccurs Dont set errors on validating minoccurs,
         *              (Parent will order this in a case of choice)
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_deserialize_obj(
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env,
            axiom_node_t** dp_parent,
            axis2_bool_t *dp_is_early_node_valid,
            axis2_bool_t dont_care_minoccurs);
                            
            

       /**
         * Declare namespace in the most parent node 
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @param parent_element parent element
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index pointer to an int which contain the next namespace index
         */
       void AXIS2_CALL
       adb_updateUsersOfSpecialInternalRole_declare_parent_namespaces(
                    adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index);

        

        /**
         * Wrapper for the serialization function, will invoke the extension mapper instead
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @param updateUsersOfSpecialInternalRole_om_node node to serialize from
         * @param updateUsersOfSpecialInternalRole_om_element parent element to serialize from
         * @param tag_closed whether the parent tag is closed or not
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index an int which contain the next namespace index
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axiom_node_t* AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_serialize(
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env,
            axiom_node_t* updateUsersOfSpecialInternalRole_om_node, axiom_element_t *updateUsersOfSpecialInternalRole_om_element, int tag_closed, axutil_hash_t *namespaces, int *next_ns_index);

        /**
         * Serialize to an XML from the adb objects
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @param updateUsersOfSpecialInternalRole_om_node node to serialize from
         * @param updateUsersOfSpecialInternalRole_om_element parent element to serialize from
         * @param tag_closed whether the parent tag is closed or not
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index an int which contain the next namespace index
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axiom_node_t* AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_serialize_obj(
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env,
            axiom_node_t* updateUsersOfSpecialInternalRole_om_node, axiom_element_t *updateUsersOfSpecialInternalRole_om_element, int tag_closed, axutil_hash_t *namespaces, int *next_ns_index);

        /**
         * Check whether the adb_updateUsersOfSpecialInternalRole is a particle class (E.g. group, inner sequence)
         * @return whether this is a particle class.
         */
        axis2_bool_t AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_is_particle();

        /******************************* Alternatives for Create and Free functions *********************************/

        

        /**
         * Constructor for creating adb_updateUsersOfSpecialInternalRole_t
         * @param env pointer to environment struct
         * @param _roleName axis2_char_t*
         * @param _usersShown axutil_array_list_t*
         * @param _usersSelected axutil_array_list_t*
         * @return newly created adb_updateUsersOfSpecialInternalRole_t object
         */
        adb_updateUsersOfSpecialInternalRole_t* AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_create_with_values(
            const axutil_env_t *env,
                axis2_char_t* _roleName,
                axutil_array_list_t* _usersShown,
                axutil_array_list_t* _usersSelected);

        


                /**
                 * Free adb_updateUsersOfSpecialInternalRole_t object and return the property value.
                 * You can use this to free the adb object as returning the property value. If there are
                 * many properties, it will only return the first property. Other properties will get freed with the adb object.
                 * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object to free
                 * @param env pointer to environment struct
                 * @return the property value holded by the ADB object, if there are many properties only returns the first.
                 */
                axis2_char_t* AXIS2_CALL
                adb_updateUsersOfSpecialInternalRole_free_popping_value(
                        adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
                        const axutil_env_t *env);
            

        /******************************* get the value by the property number  *********************************/
        /************NOTE: This method is introduced to resolve a problem in unwrapping mode *******************/

        
        

        /**
         * Getter for roleName by property number (1)
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @return axis2_char_t*
         */
        axis2_char_t* AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_get_property1(
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env);

    
        

        /**
         * Getter for usersShown by property number (2)
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @return Array of axis2_char_t*s.
         */
        axutil_array_list_t* AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_get_property2(
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env);

    
        

        /**
         * Getter for usersSelected by property number (3)
         * @param  _updateUsersOfSpecialInternalRole adb_updateUsersOfSpecialInternalRole_t object
         * @param env pointer to environment struct
         * @return Array of axis2_char_t*s.
         */
        axutil_array_list_t* AXIS2_CALL
        adb_updateUsersOfSpecialInternalRole_get_property3(
            adb_updateUsersOfSpecialInternalRole_t* _updateUsersOfSpecialInternalRole,
            const axutil_env_t *env);

    
     #ifdef __cplusplus
     }
     #endif

     #endif /* ADB_UPDATEUSERSOFSPECIALINTERNALROLE_H */
    

