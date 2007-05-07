<?php
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

function echoFunction($inMessage) {

    $returnMessage = new WSMessage($inMessage->str);

    return $returnMessage;
}

function password($username)
{
    $password = "buddhika123";
    return $password;
}


$operations = array("echoString" => "echoFunction");

$sec_array = array("includeTimeStamp" => TRUE,
                    "useUsernameToken" => TRUE);

$policy = new WSPolicy(array("security"=>$sec_array));

$sec_token = new WSSecurityToken(array("password_callback" => "password",
                                     "passwordType" => "Digest",
                                       "ttl" => 100));

$svr = new WSService(array("operations" => $operations,
                           "policy" => $policy,
                           "securityToken" => $sec_token));
        
$svr->reply();

?>
