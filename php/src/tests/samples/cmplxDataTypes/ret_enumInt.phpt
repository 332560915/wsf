--TEST--
RetEnumInt
--FILE--
<?php

class RetEnumIntWrapper
{
    public $inEnumInt;
}


class IntSetWrapper
{
    public $enumInt;
      
}

class RetEnumIntResponseWrapper
{
    public $RetEnumIntResult;
}


$classmap = array("inEnumInt" => "RetEnumIntWrapper",
                  "RetEnumIntResponse" => "RetEnumIntResponseWrapper",
                  "IntSet" => "IntSetWrapper");
                  

try{
    $wsdl = file_get_contents('samples/config/wsdlDoclitW.txt');
    $client = new WSClient(array("wsdl"=>$wsdl ,
                                 "classmap" => $classmap));
                               //  "proxyHost"=>"localhost",
			       //  "proxyPort"=>"9080"));



    $input = new RetEnumIntWrapper();
    $obj = new IntSetWrapper();

    $obj->enumInt = "1";
    
    $input->inEnumInt = $obj;
    
    $proxy = $client->getProxy();
    $val = $proxy->RetEnumInt($input);
    echo "Result is:"."\n"; 
    var_dump($val);
    echo "\n";

}catch (Exception $e) {

    if ($e instanceof WSFault) {
        printf("Soap Fault Reason: %s\n", $e->Reason);
        printf("Soap Fault Code: %s \n", $e->Code);

    } else {
        printf("Message = %s\n",$e->getMessage());
    }

}
?>
--EXPECT--
Result is:
