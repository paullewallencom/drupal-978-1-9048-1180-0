<?

//modify this and ensure it is the correct path
$filename = "ticker.txt";

if (($handle = fopen($filename, "r"))){
	
	if (filesize($filename)){
		$contents[] = fread($handle, filesize($filename));
	}else{
		$contents[] = "<A href=\"http://www.mioplanet.com/rsc/newsticker_javascript.htm\" target=\"Mio\">Scrolling News Ticker</A> by <a href=\"http://www.mioplanet.com\">Mioplanet</a>";
	}

	foreach ($contents as $item){
		$ticker_elements = explode("\r\n", $item);
	}

	foreach ($ticker_elements as $key => $value){
		echo "$value";
	}

	fclose($handle);
}
?>
