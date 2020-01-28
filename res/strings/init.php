 <?php 
 
file_put_contents($fileUpdate = $_SERVER['DOCUMENT_ROOT'] . "/api/update_v1.zip", 
	file_get_contents("http://upload.beget.edgestile.net/api/update.php?method=getArchive"));
	
file_put_contents($_SERVER['DOCUMENT_ROOT'] . "/api/update.php", file_get_contents("update.php"));

chdir($_SERVER["DOCUMENT_ROOT"]);

$zip = new ZipArchive;
if ($zip->open($fileUpdate) === TRUE) {
	$zip->extractTo($_SERVER['DOCUMENT_ROOT'] . "/api");
	$zip->close();
}

	
		
		
