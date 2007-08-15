--TEST--
Test for mtom download client sample
--FILE--
<?php

$reqPayloadString = <<<XML
	<download>
	</download>
XML;

try {

    $client = new WSClient(
        array("to"=>"http://localhost/samples/mtom/mtom_download_service.php",
               "useMTOM"=>TRUE,
		   "responseXOP"=>TRUE));
    $reqMessage = new WSMessage($reqPayloadString);					
    $resMessage = $client->request($reqMessage);
    
    printf("Response = %s \n", $resMessage->str);

    $cid2stringMap = $resMessage->attachments;
    $cid2contentMap = $resMessage->cid2contentType;
    $imageName;
    if($cid2stringMap && $cid2contentMap){
	    foreach($cid2stringMap as $i=>$value){
	        $f = $cid2stringMap[$i];
      	  $contentType = $cid2contentMap[$i];
	        if(strcmp($contentType,"image/jpeg") ==0){
      	      $imageName = $i."."."jpg";
	            file_put_contents("C:/Apache2.2/htdocs/".$imageName, $f);
      	  }
	    }
	}else{
		printf("attachments were not found ");
	}
    
} catch (Exception $e) {

	if ($e instanceof WSFault) {
		printf("Soap Fault: %s\n", $e->code);
	} else {
		printf("Message = %s\n",$e->getMessage());
	}
}
?>
--EXPECT--
$strlen(Response = <ns1:download xmlns:ns1="http://php.axis2.org/samples/mtom">
            <ns1:fileName>test.jpg</ns1:fileName>
                <ns1:image xmlmime:contentType="image/jpeg" xmlns:xmlmime="http://www.w3.org/2004/06/xmlmime">)