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


        /**
         * adb_getUserRoles.c
         *
         * This file was auto-generated from WSDL
         * by the Apache Axis2/C version: SNAPSHOT  Built on : Mar 10, 2008 (08:35:52 GMT+00:00)
         */

        #include "adb_getUserRoles.h"
        
               /*
                * implmentation of the getUserRoles|http://service.mgt.user.carbon.wso2.org element
                */
           


        struct adb_getUserRoles
        {
            axis2_char_t *property_Type;

            
                axutil_qname_t* qname;
            axis2_char_t* property_userName;

                
                axis2_bool_t is_valid_userName;
            
        };


       /************************* Private Function prototypes ********************************/
        


       /************************* Function Implmentations ********************************/
        adb_getUserRoles_t* AXIS2_CALL
        adb_getUserRoles_create(
            const axutil_env_t *env)
        {
            adb_getUserRoles_t *_getUserRoles = NULL;
            
                axutil_qname_t* qname = NULL;
            
            AXIS2_ENV_CHECK(env, NULL);

            _getUserRoles = (adb_getUserRoles_t *) AXIS2_MALLOC(env->
                allocator, sizeof(adb_getUserRoles_t));

            if(NULL == _getUserRoles)
            {
                AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
                return NULL;
            }

            memset(_getUserRoles, 0, sizeof(adb_getUserRoles_t));

            _getUserRoles->property_Type = axutil_strdup(env, "adb_getUserRoles");
            _getUserRoles->property_userName  = NULL;
                  _getUserRoles->is_valid_userName  = AXIS2_FALSE;
            
                  qname =  axutil_qname_create (env,
                        "getUserRoles",
                        "http://service.mgt.user.carbon.wso2.org",
                        NULL);
                _getUserRoles->qname = qname;
            

            return _getUserRoles;
        }

        adb_getUserRoles_t* AXIS2_CALL
        adb_getUserRoles_create_with_values(
            const axutil_env_t *env,
                axis2_char_t* _userName)
        {
            adb_getUserRoles_t* adb_obj = NULL;
            axis2_status_t status = AXIS2_SUCCESS;

            adb_obj = adb_getUserRoles_create(env);

            
              status = adb_getUserRoles_set_userName(
                                     adb_obj,
                                     env,
                                     _userName);
              if(status == AXIS2_FAILURE) {
                  adb_getUserRoles_free (adb_obj, env);
                  return NULL;
              }
            

            return adb_obj;
        }
      
        axis2_char_t* AXIS2_CALL
                adb_getUserRoles_free_popping_value(
                        adb_getUserRoles_t* _getUserRoles,
                        const axutil_env_t *env)
                {
                    axis2_char_t* value;

                    
                    
                    value = _getUserRoles->property_userName;

                    _getUserRoles->property_userName = (axis2_char_t*)NULL;
                    adb_getUserRoles_free(_getUserRoles, env);

                    return value;
                }
            

        axis2_status_t AXIS2_CALL
        adb_getUserRoles_free(
                adb_getUserRoles_t* _getUserRoles,
                const axutil_env_t *env)
        {
            
            
            return adb_getUserRoles_free_obj(
                _getUserRoles,
                env);
            
        }

        axis2_status_t AXIS2_CALL
        adb_getUserRoles_free_obj(
                adb_getUserRoles_t* _getUserRoles,
                const axutil_env_t *env)
        {
            

            AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
            AXIS2_PARAM_CHECK(env->error, _getUserRoles, AXIS2_FAILURE);

            if (_getUserRoles->property_Type != NULL)
            {
              AXIS2_FREE(env->allocator, _getUserRoles->property_Type);
            }

            adb_getUserRoles_reset_userName(_getUserRoles, env);
            
              if(_getUserRoles->qname)
              {
                  axutil_qname_free (_getUserRoles->qname, env);
                  _getUserRoles->qname = NULL;
              }
            

            if(_getUserRoles)
            {
                AXIS2_FREE(env->allocator, _getUserRoles);
                _getUserRoles = NULL;
            }

            return AXIS2_SUCCESS;
        }


        

        axis2_status_t AXIS2_CALL
        adb_getUserRoles_deserialize(
                adb_getUserRoles_t* _getUserRoles,
                const axutil_env_t *env,
                axiom_node_t **dp_parent,
                axis2_bool_t *dp_is_early_node_valid,
                axis2_bool_t dont_care_minoccurs)
        {
            
            
            return adb_getUserRoles_deserialize_obj(
                _getUserRoles,
                env,
                dp_parent,
                dp_is_early_node_valid,
                dont_care_minoccurs);
            
        }

        axis2_status_t AXIS2_CALL
        adb_getUserRoles_deserialize_obj(
                adb_getUserRoles_t* _getUserRoles,
                const axutil_env_t *env,
                axiom_node_t **dp_parent,
                axis2_bool_t *dp_is_early_node_valid,
                axis2_bool_t dont_care_minoccurs)
        {
          axiom_node_t *parent = *dp_parent;
          
          axis2_status_t status = AXIS2_SUCCESS;
           
             const axis2_char_t* text_value = NULL;
             axutil_qname_t *qname = NULL;
          
            axutil_qname_t *element_qname = NULL; 
            
               axiom_node_t *first_node = NULL;
               axis2_bool_t is_early_node_valid = AXIS2_TRUE;
               axiom_node_t *current_node = NULL;
               axiom_element_t *current_element = NULL;
            
            AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
            AXIS2_PARAM_CHECK(env->error, _getUserRoles, AXIS2_FAILURE);

            
              
              while(parent && axiom_node_get_node_type(parent, env) != AXIOM_ELEMENT)
              {
                  parent = axiom_node_get_next_sibling(parent, env);
              }
              if (NULL == parent)
              {
                /* This should be checked before everything */
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                            "Failed in building adb object for getUserRoles : "
                            "NULL element can not be passed to deserialize");
                return AXIS2_FAILURE;
              }
              

                    current_element = (axiom_element_t *)axiom_node_get_data_element(parent, env);
                    qname = axiom_element_get_qname(current_element, env, parent);
                    if (axutil_qname_equals(qname, env, _getUserRoles-> qname))
                    {
                        
                          first_node = axiom_node_get_first_child(parent, env);
                          
                    }
                    else
                    {
                        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                              "Failed in building adb object for getUserRoles : "
                              "Expected %s but returned %s",
                              axutil_qname_to_string(_getUserRoles-> qname, env),
                              axutil_qname_to_string(qname, env));
                        
                        return AXIS2_FAILURE;
                    }
                    

                     
                     /*
                      * building userName element
                      */
                     
                     
                     
                                   current_node = first_node;
                                   is_early_node_valid = AXIS2_FALSE;
                                   
                                   
                                    while(current_node && axiom_node_get_node_type(current_node, env) != AXIOM_ELEMENT)
                                    {
                                        current_node = axiom_node_get_next_sibling(current_node, env);
                                    }
                                    if(current_node != NULL)
                                    {
                                        current_element = (axiom_element_t *)axiom_node_get_data_element(current_node, env);
                                        qname = axiom_element_get_qname(current_element, env, current_node);
                                    }
                                   
                                 element_qname = axutil_qname_create(env, "userName", "http://service.mgt.user.carbon.wso2.org", NULL);
                                 

                           if ( 
                                (current_node   && current_element && (axutil_qname_equals(element_qname, env, qname))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, env, qname)))
                              {
                                is_early_node_valid = AXIS2_TRUE;
                              }
                              
                                 
                                      text_value = axiom_element_get_text(current_element, env, current_node);
                                      if(text_value != NULL)
                                      {
                                            status = adb_getUserRoles_set_userName(_getUserRoles, env,
                                                               text_value);
                                      }
                                      
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in setting the value for userName ");
                                     if(element_qname)
                                     {
                                         axutil_qname_free(element_qname, env);
                                     }
                                     return AXIS2_FAILURE;
                                 }
                              }
                           
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, env);
                     element_qname = NULL;
                  }
                 
          return status;
       }

          axis2_bool_t AXIS2_CALL
          adb_getUserRoles_is_particle()
          {
            
                 return AXIS2_FALSE;
              
          }


          void AXIS2_CALL
          adb_getUserRoles_declare_parent_namespaces(
                    adb_getUserRoles_t* _getUserRoles,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index)
          {
            
                  /* Here this is an empty function, Nothing to declare */
                 
          }

        
        
        axiom_node_t* AXIS2_CALL
        adb_getUserRoles_serialize(
                adb_getUserRoles_t* _getUserRoles,
                const axutil_env_t *env, axiom_node_t *parent, axiom_element_t *parent_element, int parent_tag_closed, axutil_hash_t *namespaces, int *next_ns_index)
        {
            
            
                return adb_getUserRoles_serialize_obj(
                    _getUserRoles, env, parent, parent_element, parent_tag_closed, namespaces, next_ns_index);
            
        }

        axiom_node_t* AXIS2_CALL
        adb_getUserRoles_serialize_obj(
                adb_getUserRoles_t* _getUserRoles,
                const axutil_env_t *env, axiom_node_t *parent, axiom_element_t *parent_element, int parent_tag_closed, axutil_hash_t *namespaces, int *next_ns_index)
        {
            
            
         
         axiom_node_t* current_node = NULL;
         int tag_closed = 0;
         
                axiom_namespace_t *ns1 = NULL;

                axis2_char_t *qname_uri = NULL;
                axis2_char_t *qname_prefix = NULL;
                axis2_char_t *p_prefix = NULL;
                axis2_bool_t ns_already_defined;
            
                    axis2_char_t *text_value_1;
                    axis2_char_t *text_value_1_temp;
                    
               axis2_char_t *start_input_str = NULL;
               axis2_char_t *end_input_str = NULL;
               unsigned int start_input_str_len = 0;
               unsigned int end_input_str_len = 0;
            
            
               axiom_data_source_t *data_source = NULL;
               axutil_stream_t *stream = NULL;

             
                int next_ns_index_value = 0;
            

            AXIS2_ENV_CHECK(env, NULL);
            AXIS2_PARAM_CHECK(env->error, _getUserRoles, NULL);
            
             
                    namespaces = axutil_hash_make(env);
                    next_ns_index = &next_ns_index_value;
                     
                           ns1 = axiom_namespace_create (env,
                                             "http://service.mgt.user.carbon.wso2.org",
                                             "n"); 
                           axutil_hash_set(namespaces, "http://service.mgt.user.carbon.wso2.org", AXIS2_HASH_KEY_STRING, axutil_strdup(env, "n"));
                       
                     
                    parent_element = axiom_element_create (env, NULL, "getUserRoles", ns1 , &parent);
                    
                    
                    axiom_element_set_namespace(parent_element, env, ns1, parent);


            
                    data_source = axiom_data_source_create(env, parent, &current_node);
                    stream = axiom_data_source_get_stream(data_source, env);
                  
                       if(!(p_prefix = (axis2_char_t*)axutil_hash_get(namespaces, "http://service.mgt.user.carbon.wso2.org", AXIS2_HASH_KEY_STRING)))
                       {
                           p_prefix = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof (axis2_char_t) * ADB_DEFAULT_NAMESPACE_PREFIX_LIMIT);
                           sprintf(p_prefix, "n%d", (*next_ns_index)++);
                           axutil_hash_set(namespaces, "http://service.mgt.user.carbon.wso2.org", AXIS2_HASH_KEY_STRING, p_prefix);
                           
                           axiom_element_declare_namespace_assume_param_ownership(parent_element, env, axiom_namespace_create (env,
                                            "http://service.mgt.user.carbon.wso2.org",
                                            p_prefix));
                       }
                      

                   if (!_getUserRoles->is_valid_userName)
                   {
                      
                           /* no need to complain for minoccurs=0 element */
                            
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("userName"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("userName")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing userName element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%suserName>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%suserName>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                    
                           text_value_1 = _getUserRoles->property_userName;
                           
                           axutil_stream_write(stream, env, start_input_str, start_input_str_len);
                           
                            
                           text_value_1_temp = axutil_xml_quote_string(env, text_value_1, AXIS2_TRUE);
                           if (text_value_1_temp)
                           {
                               axutil_stream_write(stream, env, text_value_1_temp, axutil_strlen(text_value_1_temp));
                               AXIS2_FREE(env->allocator, text_value_1_temp);
                           }
                           else
                           {
                               axutil_stream_write(stream, env, text_value_1, axutil_strlen(text_value_1));
                           }
                           
                           axutil_stream_write(stream, env, end_input_str, end_input_str_len);
                           
                     
                     AXIS2_FREE(env->allocator,start_input_str);
                     AXIS2_FREE(env->allocator,end_input_str);
                 } 

                 
                   if(namespaces)
                   {
                       axutil_hash_index_t *hi;
                       void *val;
                       for (hi = axutil_hash_first(namespaces, env); hi; hi = axutil_hash_next(env, hi)) 
                       {
                           axutil_hash_this(hi, NULL, NULL, &val);
                           AXIS2_FREE(env->allocator, val);
                       }
                       axutil_hash_free(namespaces, env);
                   }
                

            return parent;
        }


        

            /**
             * Getter for userName by  Property Number 1
             */
            axis2_char_t* AXIS2_CALL
            adb_getUserRoles_get_property1(
                adb_getUserRoles_t* _getUserRoles,
                const axutil_env_t *env)
            {
                return adb_getUserRoles_get_userName(_getUserRoles,
                                             env);
            }

            /**
             * getter for userName.
             */
            axis2_char_t* AXIS2_CALL
            adb_getUserRoles_get_userName(
                    adb_getUserRoles_t* _getUserRoles,
                    const axutil_env_t *env)
             {
                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _getUserRoles, NULL);
                  

                return _getUserRoles->property_userName;
             }

            /**
             * setter for userName
             */
            axis2_status_t AXIS2_CALL
            adb_getUserRoles_set_userName(
                    adb_getUserRoles_t* _getUserRoles,
                    const axutil_env_t *env,
                    const axis2_char_t*  arg_userName)
             {
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _getUserRoles, AXIS2_FAILURE);
                
                if(_getUserRoles->is_valid_userName &&
                        arg_userName == _getUserRoles->property_userName)
                {
                    
                    return AXIS2_SUCCESS; 
                }

                adb_getUserRoles_reset_userName(_getUserRoles, env);

                
                if(NULL == arg_userName)
                {
                    /* We are already done */
                    return AXIS2_SUCCESS;
                }
                _getUserRoles->property_userName = (axis2_char_t *)axutil_strdup(env, arg_userName);
                        if(NULL == _getUserRoles->property_userName)
                        {
                            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Error allocating memeory for userName");
                            return AXIS2_FAILURE;
                        }
                        _getUserRoles->is_valid_userName = AXIS2_TRUE;
                    
                return AXIS2_SUCCESS;
             }

             

           /**
            * resetter for userName
            */
           axis2_status_t AXIS2_CALL
           adb_getUserRoles_reset_userName(
                   adb_getUserRoles_t* _getUserRoles,
                   const axutil_env_t *env)
           {
               int i = 0;
               int count = 0;
               void *element = NULL;

               AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
               AXIS2_PARAM_CHECK(env->error, _getUserRoles, AXIS2_FAILURE);
               

               
            
                
                if(_getUserRoles->property_userName != NULL)
                {
                   
                   
                        AXIS2_FREE(env-> allocator, _getUserRoles->property_userName);
                     _getUserRoles->property_userName = NULL;
                }
            
                
                
                _getUserRoles->is_valid_userName = AXIS2_FALSE; 
               return AXIS2_SUCCESS;
           }

           /**
            * Check whether userName is nill
            */
           axis2_bool_t AXIS2_CALL
           adb_getUserRoles_is_userName_nil(
                   adb_getUserRoles_t* _getUserRoles,
                   const axutil_env_t *env)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _getUserRoles, AXIS2_TRUE);
               
               return !_getUserRoles->is_valid_userName;
           }

           /**
            * Set userName to nill (currently the same as reset)
            */
           axis2_status_t AXIS2_CALL
           adb_getUserRoles_set_userName_nil(
                   adb_getUserRoles_t* _getUserRoles,
                   const axutil_env_t *env)
           {
               return adb_getUserRoles_reset_userName(_getUserRoles, env);
           }

           

