--TEST--
Test for ws_client_request_invalid_param sample
--FILE--
<?php
$reqPayloadString = <<<XML
	<ns1:echo xmlns:ns1="http://php.axis2.org/samples">
		<text>Hello World!</text>
	</ns1:echo>
XML;

try {

    $reqMessage = new WSMessage($reqPayloadString,
        array("to"=>"http://localhost/samples/ws_service_actions.php",
              "action" => "http://php.axis2.org/samples/echoString"));
              
    $client = new WSClient(array("useWSA" => TRUE));
				
    $resMessage = $client->request();
    
    printf("Response = %s \n", $resMessage->str);

} catch (Exception $e) {

	if ($e instanceof WSFault) {
		printf("Soap Fault: %s\n", $e->code);
	} else {
		printf("Message = %s\n",$e->getMessage());
	}

}
?>
--EXPECT--
Warning: WSClient::request() expects exactly 1 parameter, 0 given in C:\wsfphp\src\tests\samples\ws_client_request_invalid_param.php on line 16

Fatal error: WSClient::request(): Invalid parameters in C:\wsfphp\src\tests\samples\ws_client_request_invalid_param.php on line 16
