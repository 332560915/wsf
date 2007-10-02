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

try {

    $msg = new WSMessage($reqPayloadString,
			array(
			   "to"=>"http://localhost/samples/reliable/ping_service_rm.php",
              "action" => "http://php.axis2.org/samples/echoString"));

    $client = new WSClient(array("reliable"=>TRUE,"useWSA" => TRUE));
    
	
    $client->send($msg);

    echo "Message Sent";

    sleep(10);

    
} catch (Exception $e) {

	if ($e instanceof WSFault) {
		printf("Soap Fault: %s\n", $e->Reason);
	} else {
		printf("Message = %s\n",$e->getMessage());
	}
}
?>
