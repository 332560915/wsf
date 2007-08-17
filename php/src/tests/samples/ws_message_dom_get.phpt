--TEST--
Test for ws_message_dom_get_echo_client.phpt sample
--FILE--
<?php
$reqPayloadString = <<<XML
	<ns1:echo xmlns:ns1="http://php.axis2.org/samples">
		<text>Hello World!</text>
	</ns1:echo>
XML;

try {

    $reqMessage = new WSMessage($reqPayloadString,
        array("to"=>"http://localhost/samples/echo_service_addr.php",
              "action"=>"http://php.axis2.org/samples/echoString"));
              
    $client = new WSClient(array("useWSA" => TRUE));
    
    $resMessage = $client->request($reqMessage);
    
    printf("Response = %s \n", htmlspecialchars($resMessage->str));
    
    /*printf("Response = $reqMessage->str"); */
           
} catch (Exception $e) {

	if ($e instanceof WSFault) {
		printf("Soap Fault: %s\n", $e->code);
	} else {
		printf("Message = %s\n",$e->getMessage());
	}

}
?>
--EXPECT--
Response = &lt;ns1:echo xmlns:ns1=&quot;http://php.axis2.org/samples&quot;&gt;
		&lt;text&gt;Hello World!&lt;/text&gt;
	&lt;/ns1:echo&gt;