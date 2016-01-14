<!doctype html>
<html>
<head>
<title>Firefeed Signup</title>
<meta charset="UTF-8">
<META HTTP-EQUIV="refresh" CONTENT="10,home.php">
</head>
   <body>
	<?php
		session_start();
    for ($i = 13002; $i < 13004;$i++)
    {$socket = stream_socket_client("tcp://localhost:".$i, $errno, $errstr, 5);
  if (!$socket) {
       echo $errstr;
       exit(1);
   }
   fwrite($socket,"addUser ".$_POST["username"]." ".$_POST["password"]);
   $reply= fread($socket,256);
   if ($reply == "Success")
   {
    $_SESSION["username"]=$_POST["username"];
  echo "Welcome to Firefeed. You will be redirected shortly.";}
  else
  { echo "Username is taken";
  header("signup.html");
  }
}
  ?>
  </body>
</html>
