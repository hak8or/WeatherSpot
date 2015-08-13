
<?php

$start = microtime(TRUE);
usleep(250);
$end = microtime(TRUE);

$diffy = $end - $start;

echo "Start: " . $start . " to End: " . $end . " Diffy: " . $diffy;
echo "<br>";

$dt = new DateTime("@$dt");
echo $dt->format('u');
?>
