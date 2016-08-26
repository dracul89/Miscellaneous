<?php
  class crudeParser{
  public function crudeParser(){}
  public function parse($string){
  $word = NULL;
  $line = NULL;
  $block = false;
  $next = false;
  $breakIntoLines = explode("\n",$string);//makes us an array of strings

     
  
  foreach($breakIntoLines as $key => $value ) {
      if($block){
	if(strstr($value,'*/'))$block = false;
      }
      if(strstr($value,'/*')){
       $block = true;
      }elseif(strstr($value,'//')){
	  $line = explode("//",$value);
	  $count = count($line);
	  if($count == 2){
	    $trimmed = trim($line[0]," \t"); //remove tab and white space
	    if(strlen($trimmed) >0){
	    //here we can parse the rest :) 
	      if(strstr($trimmed,'class')){
		$temp = sscanf($trimmed,"%s %s %s %s");
		  foreach($temp as $key => $value) {
		    if($next){
		      $word = $value;
		      $next = false;
		    }
		    if($value == "class") $next = true;    
		}
		$unreplaced = $word;
		$word = str_replace("{","",$unreplaced);
		return $word;
	      }
	    }
	  }
     
      }else{ // no comments involved
	if(strstr($value,'class') && !$block){
	  $temp = sscanf($value,"%s %s %s %s");
	  foreach($temp as $key => $value) {
	      if($next){
		$word = $value;
		$next = false;
	      }
	      if($value == "class") $next = true;    
	  }
	  $unreplaced = $word;
	  $word = str_replace("{","",$unreplaced);
	  return $word;// because I only want the first one 
	}
      }
      
      
  }
  }// end function
  }
?>
