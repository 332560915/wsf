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

$reqPayloadString = <<<XML
    <ns1:echoString xmlns:ns1="http://php.axis2.org/samples">
        <text>Hello World!</text>
    </ns1:echoString>
XML;

$sec_arry = array("useUsernameToken"=> TRUE,
                  "includeTimeStamp"=>TRUE );

$policy_obj = array("outpolicy"=>$sec_arry);

$policy = new WSPolicy(array("security"=>$policy_obj));

$sec_token = new WSSecurityToken(array("user" => "buddhika",
                                       "ttl" => 300,
                                       "password" => "buddhika123",
                                       "passwordType" => "Digest"));

try{
$client = new WSClient(array("to"=>"http://localhost/samples/secpolicy/call_back_scenario/sec_service_scenario2_array.php",
                             "policy"=>$policy,
			     "securityToken" => $sec_token));
 
 $response = $client->request($reqPayloadString);
 echo  $response->str;
}
catch (Exception $e){
    if ($e instanceof WSFault) {
        printf("Soap Fault: %s\n", $e->code);
    } else {
        printf("Message = %s\n",$e->getMessage());
    }

}



?>