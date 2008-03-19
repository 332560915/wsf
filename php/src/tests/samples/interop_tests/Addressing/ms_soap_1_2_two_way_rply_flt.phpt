--TEST--
SOAP 1.2 two-way message with fault. FaultTo is anonymous, ReplyTo is non-anonymous.
--FILE--
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
<echoIn xmlns="http://example.org/echo">Hello World</echoIn>
XML;

try {

    $end_point = file_get_contents('epSoap12.txt');   
	$reqMessage = new WSMessage($reqPayloadString, 
        array("to" => $end_point,
              "action" => "http://example.org/action/echoIn", 
              "replyTo" => "http://example.org/node/A",
			  "faultTo" => "http://www.w3.org/2005/08/addressing/anonymous"));
	
	$client = new WSClient(array("useSOAP" => "1.2",
                                     "useWSA" => TRUE /*,
                                     "proxyHost" => "localhost",
                                     "proxyPort" => "9090"*/));

	$resMessage = $client->request($reqMessage);
        echo $resMessage->str."\n";
	
   } catch (Exception $e) {
	if ($e instanceof WSFault) {
		printf("Soap Fault: %s\n", $e->Code);
	} else {
		printf("Message = %s\n",$e->getMessage());
	}
}
?>
--EXPECT--
<echoOut xmlns="http://example.org/echo">Hello World</echoOut>

