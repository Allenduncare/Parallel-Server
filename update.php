<!doctype html>
<html>
  <head>
	<title>Update</title>
	<meta charset="UTF-8">
  <META HTTP-EQUIV="refresh" CONTENT="5,home.php">
  </head>
  <body>

   <?php
   session_start();
   for ($i= 13002; $i < 13011;$i++)
   {
   $socket = stream_socket_client("tcp://localhost:".$i, $errno, $errstr, 5);
   if (!$socket) {
      echo $errstr;
      exit(1);
  }
  echo $_POST["status"];
  fwrite($socket,"updateStatus ".$_SESSION["username"]." ".$_POST["status"]);
  $reply= fread($socket,256);
  echo $reply;
}
  ?>

  </body>
</html>
