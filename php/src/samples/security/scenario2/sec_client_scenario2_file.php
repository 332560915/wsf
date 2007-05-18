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

$out_policy_xml = file_get_contents("client-outgoing-secpolicy.xml");
$in_policy_xml = file_get_contents("client-incoming-secpolicy.xml");


$sec_file = array("outpolicy" =>$out_policy_xml,
                  "inpolicy" =>$in_policy_xml);

$policy = new WSPolicy(array("security" => $sec_file));

$sec_token = new WSSecurityToken(array("user" => "Raigama",
                                       "ttl" => 4,
                                      "password" => "RaigamaPW",
                                       "passwordType" => "Digest"));
				       
try{
$client = new WSClient(array("to"=>"http://localhost/samples/security/scenario2/sec_service_scenario2_file.php",
                             "policy"=>$policy,
			     "securityToken" => $sec_token));
 
$response = $client->request($reqPayloadString);
echo $response->str;
}
catch (Exception $e){
    if ($e instanceof WSFault) {
        printf("Soap Fault: %s\n", $e->code);
    } else {
        printf("Message = %s\n",$e->getMessage());
    }

}



?>
