
<?php
//starting session
session_start();
if($_SESSION["member_id"])
{
$user_id=$_SESSION["member_id"];
echo"welcome &nbsp<b>$user_id<b>";
}
echo"<br/>";
?>

<html>
<title></title>
<head>
<link href="./assets/style/style3.css" rel="stylesheet" type="text/css" />
</head>
<body>
<!-- Content  -->
<a href="logout.php"><b><i>logout</i></b></a>
<fieldset>
      <h1>You are here to add events</h1>
      <h2></h2>
      <div class="form-container">
      <form action="#" method="post">
      <p class="legend"><strong>Note:</strong> Required fields are marked with an asterisk (<em>*</em>)</p>

	<fieldset>

		<legend>Event Information</legend>

                 <div><label for="year">year<em>*</em></label> <input id="year" name="year" size="100" type="text" value="<?php echo $year; ?>"/></div>
                 <div><label for="week">Week<em>*</em></label> <input id="week" name="week" size="100" type="text" value="<?php echo $week; ?>"/></div>
                 <div><label for="event">Event Time<em>*</em></label> <input id="time" name="time" size="45" type="text" value="<?php echo $time; ?>"/></div>
                 <div><label for="country">Event Name<em>*</em></label> <input id="name" name="event" size="60" value="<?php echo $name; ?>" /></div>	
                 <div><label for="code">Day of the week</label> <input id="date" name="date" size="10" type="text" value="<?php echo $date; ?>"/></div>
	</fieldset>
	<div class="buttonrow">
		<input value="Save" class="button" type="submit" name="Submit"/>
	</div>

</fieldset>

	</form>

	
	</div>
	
	
	  
<!-- End of Content  -->

<?php

/*get the values from the form and send them to the service to add values.*/

if(isset($_POST['Submit']))
{ 
 $week=$_POST['week'];
 $time = $_POST['time'];
 $event=$_POST['event'];
 $date=$_POST['date'];
 $year=$_POST['year'];

$reqPayloadString = <<<XML
<ns1:add xmlns:ns1="http://ws.apache.org/axis2/php/math">
      <param1>$time</param1>
      <param2>$event</param2>
      <param3>$date</param3>
      <param4>$user_id</param4> 
      <param5>$week</param5>
      <param6>$year</param6>
 </ns1:add>
XML;

}

try { 

    $client = new WSClient(
        array("to"=>"http://localhost/samples/solutions/calendar/calendar_service.php"));
          
    $resMessage = $client->request($reqPayloadString);
        

     printf($resMessage->str);
      


} catch (Exception $e) { 
    
    

        if ($e instanceof WSFault) {
    
                printf("Soap Fault: %s\n", $e->Reason);
    
        } else {

                printf("Message = %s\n",$e->getMessage());

        }



}

?>
<br>
<br>
<a href="calendar_client.php">Back to the calender</a>
</body>
</html>
