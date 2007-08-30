--TEST--
Test for ws_service_reply_invalid_param sample
--FILE--
<?php

function echoFunction($inMessage) {

    $returnMessage = new WSMessage($inMessage->str);

    return $returnMessage;
}

$operations = array("echoString" => "echoFunction");
$actions = array("http://php.axis2.org/samples/echoString" => "echoString");

$svr = new WSService(array("operations" => $operations, 
                              "actions" => $actions));
        
$svr->reply(array("echoString" => "echoFunction"));

?>
--EXPECT--
Response = <ns1:echo xmlns:ns1="http://php.axis2.org/samples">
		<text>Hello World!</text>
	</ns1:echo>
