<doctype! html>
  <html>
    <head>
      <title>Firefeed Homepage</title>
      <meta charset="UTF-8">
	  <link rel="stylesheet" type"text/css" href="style.css">
    </head>
    <body>
	<?php session_start();?>
	<h1> Welcome back <? php echo $_SESSION["username"] ?></h1>
	<? php echo $_POST["friend added string"] ?>
	<div class=sidebar>Welcome <?php echo $_SESSION["username"] ?>
	<form>
	Add a friend!<input type="text" name="Friend">
	<p><button type="submit" formmethod="GET" formaction="addfriend.php">Add a friend</button>
	<button type="submit" formmethod="GET" formaction="removefriend.php">Remove a friend</button></form>
  <br>
  Change your password!
  <br>Old password:<input type="text" name="oldpassword">
  <br>New passowrd:<input type="text" name="newpassword">
  <button type="submit" formmethod="POST" formaction="changepassword.php">Change Password!</button></form>
	</div>
	<div class = "textbar">
	<h1> Firefeed for <?php echo $_SESSION["username"]?></h1>
	<form action="update.php" method="POST">
	What's on your mind? <input type="text" name="status">
	<button type="submit">Burn your thoughts to the net!</button>
	</form>
  <?php
  for ($i = 13002; $i < 13004;$i++)
  {
  $socket = stream_socket_client("tcp://localhost:".$i, $errno, $errstr, 5);
  if (!$socket) {
     echo $errstr;
     exit(1);
  }
  fwrite($socket,"displayuserNameMessages ".$_SESSION["username"]);

  while(!feof($socket))
  {
    $reply = fread($socket,8192);
    $status = $reply."\r<br>";
    echo $status;
  }}
  ?>
	</div>
  </body>
  </html>
