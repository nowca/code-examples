remove_filter('the_content', 'wpautop');
session_start();

function verifyFormToken( $form ) {
  if( !isset( $_SESSION[$form.'_token'] ) ) { 
  	return false;
  }

  if( !isset( $_POST['token'] ) ) {
  	return false;
  }

  if ( $_SESSION[$form.'_token'] !== $_POST['token'] ) {
  	return false;
  }

  return true;
}

function generateFormToken( $form ) {
	$token = md5( uniqid( microtime(), true ) );  
	$_SESSION[ $form.'_token' ] = $token;

	return $token;
}

function cleanMail( $email ) {
	$pattern = "/^[_a-z0-9-]+(\.[_a-z0-9-]+)*@[a-z0-9-]+(\.[a-z0-9-]+)*(\.[a-z]{2,3})$/i"; 
    if ( preg_match( $pattern, trim( strip_tags( $email ) ) ) ) { 
    	return trim( strip_tags( $email ) ); 
    } else {
    	return false; 
    }
}

if ( verifyFormToken('form1') ) {
	$whitelist = array(
					'token',
					'req-name',
					'opt-address',
					'opt-tel',
					'req-mail',
					'req-msg'
				 );

	foreach ( $_POST as $key=>$item ) {
		if ( !in_array( $key, $whitelist ) ) {
			die( "Ah ah ah..." );
		}
 	}

	$error = false;
	$errorMsg = '';

	if ( !($cleanedMail = cleanMail( $_POST['req-mail'] )) ) {
		$errorMsg .= '<label>- ungültige E-Mail Adresse</label>'; 
		$error = true;
	}


    if ( !$error ) {
		$message = '<html><body>';
	    $message .= '<table rules="all" style="border-color: #555;" cellpadding="10">';
 		$message .= "<tr style='background: #eee;'><td><strong>Name:</strong> </td><td>" . strip_tags( $_POST['req-name'] ) . "</td></tr>";
 		$message .= "<tr><td><strong>Anschrift:</strong> </td><td>" . strip_tags( $_POST['opt-address'] ) . "</td></tr>";
 		$message .= "<tr><td><strong>Telefon:</strong> </td><td>" . strip_tags( $_POST['opt-tel'] ) . "</td></tr>";
  	  	$message .= "<tr><td><strong>E-Mail:</strong> </td><td>" . $cleanedMail . "</td></tr>";
		$curText = htmlentities( strip_tags( $_POST['req-msg'] ) );           
		if ( ($curText) != '' ) {
			$message .= "<tr><td><strong>Nachricht:</strong> </td><td>" . $curText . "</td></tr>";
		}
		$message .= "</table>";
		$message .= "</body></html>";

		$to = 'info@mail.com';
		$subject = 'Nachricht über Webseite';
		$headers = "From: " . $cleanedMail . "\r\n";
		$headers .= "Reply-To: ". $cleanedMail . "\r\n";
		$headers .= "MIME-Version: 1.0\r\n";
		$headers .= "Content-Type: text/html; charset=ISO-8859-1\r\n";

		if ( mail( $to, $subject, $message, $headers ) ) {
			$responseText = '<label>Vielen Dank für Ihre Nachricht.</label>';
		} else {
			$responseText = '<label>Fehler beim Senden der Nachricht.</label>';
		}
	} else {
		$responseText = '<label>Fehler:</label><br>' . $errorMsg;
	}


    $formSource = '
	<div class="item dark">
		<center>
			' . $responseText . '                                                            
        </center>
    </div>';
} else {
	$newToken = generateFormToken( 'form1' ); 

	$formSource = '
	<form action="../kontakt/" method="post" id="change-form">
		<input type="hidden" name="token" value="' . $newToken . '">
 		<div class="item dark">
 			<label>Name*:
 				<input type="text" placeholder="Name" name="req-name">
 			</label>
 			<label>
 				Anschrift:
 				<textarea placeholder="Anschrift" rows="3" name="opt-address"></textarea>
 			</label>
 			<label>Telefon:
 				<input type="text" placeholder="Telefon" name="opt-tel">
 			</label>
 			<label>E-Mail*:
  				<input type="text" placeholder="E-Mail" name="req-mail">
 			</label>
 			<label>
 				Nachricht*:
 				<textarea placeholder="Nachricht" rows="5" name="req-msg"></textarea>
 			</label>
 			<input class="success button" type="submit" value="Abschicken" />
 		</div>
	</form>';
}