--TEST--
Test for RetArrayAnyType1D_one_string sample
--FILE--

<?php

try {

    $client = new WSClient(
        array("wsdl"=>"http://10.10.34.26/interop/sc-wsdl/cmplx/DocLitB/DocLitB_Service.php?wsdl" ,
              "useSOAP" => "1.1",
              "to" => $argv[1]));
    $proxy = $client->getProxy();	
    
    $string1 = array(NULL);
    $val =  $proxy->RetArrayString1D($string1);
    echo "\n";
    var_dump($val);
    echo "\n";

/*     $string11 = array(NULL); */
    $val =  $proxy->RetArrayString1D();
    echo "\n";
    var_dump($val);
    echo "\n";

    $string2 = "One Element";
    $val =  $proxy->RetArrayString1D(array($string2));    
    
    echo "\n One Element \n ============\n";
    echo $val->string."\n";
    echo "\n";

    $string3 = array();
    for($i = 0; $i<10; $i++){
        $string3[$i] = "string Array Data".$i;
    }
    $val =  $proxy->RetArrayString1D($string3);
    echo "\n Ten Elements \n ============\n";
    foreach($val->string as $i=> $value){
        if($value)
            echo $value."\n";
        
    }
    echo "\n";

    $string3 = array();
    for($i = 0; $i<10; $i++){
        $string3[$i] =  NULL;
    }
    $val =  $proxy->RetArrayString1D($string3);
    echo "\n All ten NULL Elements \n =============\n";
    foreach($val->string as $i=> $value){
        if(!$value)
            echo $value."NULL value\n";
        
    }
    echo "\n";

    $string4 = array();
    for($i = 0; $i<10; $i++){
        if($odd = $i%2)
            $string4[$i] = "string Array Data".$i;
        else
            $string4[$i] = NULL;

    }
    $val =  $proxy->RetArrayString1D($string4);
    echo "\n Ten with NULL Elements \n =============\n";
    foreach($val->string as $i=> $value){
        if(!$value)
            echo $value."NULL value\n";
        else
            echo $value."\n";
    }
    echo "\n";

    $string5 = array();
    for($i = 0; $i<1000; $i++){
        $string5[$i] =  "string Array Data".$i;;
    }
    $val =  $proxy->RetArrayString1D($string5);
    echo "\n All ten NULL Elements \n =============\n";
    foreach($val->string as $i=> $value){
        if($value)
            echo $value."\n";
        
    }
    echo "\n";
        

} catch (Exception $e) {

		printf("Message = %s\n",$e->getMessage());
}
?>

--EXPECT--
Response = <ns1:echo xmlns:ns1="http://php.axis2.org/samples">
		<text>Hello World!</text>
	</ns1:echo>









