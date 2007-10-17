<?php

include_once '../../soap_ext.php';

try {
    $client = new WSSoapClient(null, 
        array('location' => 'http://localhost:9091/ws_hello_service_nonwsdl.php',
              'uri' => 'http://localhost/axis/helloService' ));
    $client->__setLocation('http://localhost:9909/ws_hello_service_nonwsdl.php');
    $a = 1; $b = 2; $c = 3;
    $result =  $client->__soapCall('SomeFunction', array($a, $b, $c), NULL, NULL);

    printf("Result = %s\n", $result);

} catch (Exception $e) {
    printf("Message = %s\n",$e->__toString());
}
?> 

