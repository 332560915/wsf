--TEST--
Echo Bianry As String- Aug04 -soap1.2-MTOM
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
<EchoBinaryAsString xmlns="http://xmlsoap.org/Ping"><array><xop:Include href="cid:myid1" xmlns:xop="http://www.w3.org/2004/08/xop/include"/></array>
</EchoBinaryAsString>
XML;

try {
	$f = file_get_contents("string.txt");
/* 	$f = file_get_contents("axis2.jpg"); */
    
	$reqMessage = new WSMessage($reqPayloadString, 
        array("to" => "http://131.107.72.15/MTOM_Service_Indigo/Soap11MtomUtf8.svc/MtomTest",
              "action" => "http://xmlsoap.org/echoBinaryAsString",
              "attachments" => array("myid1" => $f))); 
	
	$client = new WSClient(array("useMTOM" => TRUE,
                                     "useSOAP" => "1.2",
                                     "useWSA" => TRUE/*,//));
				     "proxyHost" => "localhost",
                                   "proxyPort" => "9090"*/));

	$resMessage = $client->request($reqMessage);
	
        echo $resMessage->str;

} catch (Exception $e) {
	if ($e instanceof WSFault) {
		printf("Soap Fault: %s\n", $e->Code);
	} else {
		printf("Message = %s\n",$e->getMessage());
	}
}
?>
--EXPECT--
<EchoBinaryAsStringResponse xmlns="http://xmlsoap.org/Ping"><EchoBinaryAsStringResult>H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! H e l l o   W o r l d ! &#13;
</EchoBinaryAsStringResult></EchoBinaryAsStringResponse>
