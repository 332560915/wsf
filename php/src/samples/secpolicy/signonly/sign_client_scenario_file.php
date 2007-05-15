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
<ns1:echoString xmlns:ns1="http://php.axis2.org/samples"><text>
Hello World!</text></ns1:echoString>
XML;

$pubkey = ws_get_cert_from_file("a_cert.cert");
$prvkey = ws_get_key_from_file("a_key.pem");

$out_policy = file_get_contents("client-policy-out.xml");
$in_policy = file_get_contents("client-policy-in.xml");

$policy_files = array("inpolicy" => $in_policy,
                      "outpolicy" => $out_policy);

$policy = new WSPolicy(array("security" => $policy_files));

$sec_token = new WSSecurityToken(array("ttl" => 60,
                                       "certificate" => $pubkey,
                                       "privateKey" => $prvkey ));
				       
try{
$client = new WSClient(array("to"=>"http://localhost/samples/secpolicy/signonly/sign_service_scenario_file.php",
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
