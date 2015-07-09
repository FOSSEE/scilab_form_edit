<?php
// $Id$

function _completed_proposal_all()
{
  function _tbc_ext($status, $preference_id) {                                  
    if($status == "Approved") {                                                 
       return " | " . l("ER", "tbc_external_review/add_book/" . $preference_id);
    }                                                                           
    else {                                                                      
       return "";                                                              
    }                                                                           
  }
  /* get pending proposals to be approved */
  $proposal_rows = array();
  
  $proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE proposal_status = 3 ORDER BY id ASC");
 
  while ($proposal_data = db_fetch_object($proposal_q))
  {
    /* get preference */
    $preference_q = db_query("SELECT * FROM textbook_companion_preference WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);   
    $preference_data = db_fetch_object($preference_q);
    if(!$preference_data){
          $preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = 1 LIMIT 1", $proposal_data->id);  
          $preference_data = db_fetch_object($preference_q);
    }

    $proposal_status = '';
    switch ($proposal_data->proposal_status)
    {
    case 0: $proposal_status = 'Pending'; break;
    case 1: $proposal_status = 'Approved'; break;
    case 2: $proposal_status = 'Dis-approved'; break;
    case 3: $proposal_status = 'Completed'; break;
    case 4: $proposal_status = 'External'; break;
    default: $proposal_status = 'Unknown'; break;
    }

    $proposal_rows[] = array(
	date('d-m-Y', $proposal_data->creation_date),
    "{$preference_data->book} <br> <em>by {$preference_data->author}</em>",
	l($proposal_data->full_name, 'user/' . $proposal_data->uid),
	$proposal_data->city,
	$proposal_data->state,
    date('d-m-Y', $proposal_data->completion_date),
	$proposal_status,
   // l('Status', 'form_edit/status/' . $proposal_data->id) . ' | ' .;
     l('Edit', 'form_edit/tbc/edit/' . $proposal_data->id) . _tbc_ext($proposal_status, $preference_data->id)
    );
  }

  /* check if there are any pending proposals */
  if (!$proposal_rows)
  {
    drupal_set_message(t('There are no proposals.'), 'status');
    return '';
  }

  $proposal_header = array('Date of Submission', 'Title of the Book', 'Contributor Name', 'City' , 'State', 'Expected Date of Completion', 'Status', 'Action');
  $output = theme_table($proposal_header, $proposal_rows);
  return $output; 
}



/******************************************************************************/
/**************************** PROPOSAL EDIT FORM ******************************/
/******************************************************************************/

function proposal_edit_form($form_state,$nonaicte_book)
{
  global $user;

  /* get current proposal */
  $proposal_id = arg(3);
  $proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d AND proposal_status = 3", $proposal_id);
  if ($proposal_q)
  {
    $proposal_data = db_fetch_object($proposal_q);
    if (!$proposal_data)
    {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('form_edit');
      return;
    }
  } else {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('form_edit');
    return;
  }

  $user_data = user_load($proposal_data->uid);

  $preference1_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 1);
  $preference1_data = db_fetch_object($preference1_q);
  $preference2_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 2);
  $preference2_data = db_fetch_object($preference2_q);
  $preference3_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 3);
  $preference3_data = db_fetch_object($preference3_q);

  $form['full_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Full Name'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $proposal_data->full_name,
  );
  $form['email_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#size' => 30,
    '#value' => $user_data->mail,
    '#disabled' => TRUE,
  );
  $form['mobile'] = array(
    '#type' => 'textfield',
    '#title' => t('Mobile No.'),
    '#size' => 30,
    '#maxlength' => 15,
    '#required' => TRUE,
    '#default_value' => $proposal_data->mobile,
  );
  $form['how_project'] = array(
    '#type' => 'select',
    '#title' => t('How did you come to know about this project'),
    '#options' => array('Scilab Website' => 'Scilab Website',
                        'Friend' => 'Friend',
                        'Professor/Teacher' => 'Professor/Teacher',
                        'Mailing List' => 'Mailing List',
                        'Poster in my/other college' => 'Poster in my/other college',
                        'Others' => 'Others'),
    '#required' => TRUE,
    '#default_value' => $proposal_data->how_project,
  );
  $form['course'] = array(
    '#type' => 'textfield',
    '#title' => t('Course'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $proposal_data->course,
  );
  $form['branch'] = array(
    '#type' => 'select',
    '#title' => t('Department/Branch'),
    '#options' => array('Electrical Engineering' => 'Electrical Engineering',
                        'Electronics Engineering' => 'Electronics Engineering',
                        'Computer Engineering' => 'Computer Engineering',
                        'Chemical Engineering' => 'Chemical Engineering',
                        'Instrumentation Engineering' => 'Instrumentation Engineering',
                        'Mechanical Engineering' => 'Mechanical Engineering',
                        'Civil Engineering' => 'Civil Engineering',
                        'Physics' => 'Physics',
                        'Applied Physics' =>'Applied Physics',
                        ' Nuclear Technology and Biotechnology' => ' Nuclear Technology and Biotechnology',
                        'Mathematics' => 'Mathematics',
                        'Others' => 'Others'),
    '#required' => TRUE,
    '#default_value' => $proposal_data->branch,
  );
  $form['university'] = array(
    '#type' => 'textfield',
    '#title' => t('University/Institute'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $proposal_data->university,
  );
  $form['city'] = array(
    '#type' => 'textfield',
    '#title' => t('City/Village'),
    '#size' => 50,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $proposal_data->city,
  );
 
  $form['all_state'] = array(
    '#type' => 'select',    
    '#title' => t('States'),
    '#options' => array(
    				 '0' => 'Please select...',
					 'Andhra Pradesh' => t('Andhra Pradesh'),
					 'Arunachal Pradesh' => 'Arunachal Pradesh',
					 'Assam' => 'Assam',
					 'Bihar' => 'Bihar',
					 'Chhattisgarh' => 'Chhattisgarh',
					 'Goa' => 'Goa',
					 'Gujarat' => 'Gujarat',
					 'Haryana' => 'Haryana',
					 'Himachal Pradesh' => 'Himachal Pradesh',
					 'Jammu & Kashmir' => 'Jammu & Kashmir',
					 'Jharkhand' => 'Jharkhand',
					 'Karnataka' => 'Karnataka',
					 'Kerala' => 'Kerala',
					 'Madhya Pradesh' => 'Madhya Pradesh',
					 'Maharashtra' => 'Maharashtra',
					 'Manipur' => 'Manipur',
					 'Meghalaya' => 'Meghalaya',
					 'Mizoram' => 'Mizoram',
					 'Nagaland' => 'Nagaland',
					 'Odisha' => 'Odisha',
					 'Punjab' => 'Punjab',
					 'Rajasthan' => 'Rajasthan',
					 'Sikkim' => 'Sikkim',
					 'Tamil Nadu' => 'Tamil Nadu',
					 'Telangana' => 'Telangana',
					 'Tripura' => 'Tripura',
					 'Uttarakhand' => 'Uttarakhand',
					 'Uttar Pradesh'=> 'Uttar Pradesh',
					 'West Bengal' => 'West Bengal',
					 'Andaman & Nicobar' => 'Andaman & Nicobar',
					 'Chandigarh' => 'Chandigarh',
					 'Dadra and Nagar Haveli' => 'Dadra and Nagar Haveli',
					 'Daman & Diu' => 'Daman & Diu',
					 'Delhi' => 'Delhi',
					 'Lakshadweep' => 'Lakshadweep',
					 'Puducherry' => 'Puducherry',
					 'Others' => 'Others',
                ),
    '#required' => TRUE,
    '#tree' => TRUE,
    '#validated' => TRUE,
    '#default_value' => $proposal_data->state,
  );
  $form['faculty'] = array(
    '#type' => 'textfield',
    '#title' => t('College Teacher/Professor'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $proposal_data->faculty,
  );
  $form['reviewer'] = array(
    '#type' => 'textfield',
    '#title' => t('Reviewer'),
    '#size' => 30,
    '#maxlength' => 100,
    '#default_value' => $proposal_data->reviewer,
  );
  $form['completion_date'] = array(
    '#type' => 'textfield',
    '#title' => t('Expected Date of Completion'),
    '#description' => t('Input date format should be DD-MM-YYYY. Eg: 23-03-2011'),
    '#size' => 10,
    '#maxlength' => 10,
    '#default_value' => date('d-m-Y', $proposal_data->completion_date),
  );
  $form['scilab_version'] = array(
    '#type' => 'textfield',
    '#title' => t('Scilab Version'),
    '#size' => 10,
    '#maxlength' => 20,
    '#default_value' => $proposal_data->scilab_version,
  );
  $form['operating_system'] = array(
    '#type' => 'textfield',
    '#title' => t('Operating System'),
    '#size' => 30,
    '#maxlength' => 50,
    '#default_value' => $proposal_data->operating_system,
  );
  $form['preference1'] = array(
    '#type' => 'fieldset',
    '#title' => t('Book Preference 1'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['preference1']['book1'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the book'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $preference1_data->book,
  );
  $form['preference1']['author1'] = array(
    '#type' => 'textfield',
    '#title' => t('Author Name'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $preference1_data->author,
  );
  $form['preference1']['isbn1'] = array(
    '#type' => 'textfield',
    '#title' => t('ISBN No'),
    '#size' => 30,
    '#maxlength' => 25,
    '#required' => TRUE,
    '#default_value' => $preference1_data->isbn,
  );
  $form['preference1']['publisher1'] = array(
    '#type' => 'textfield',
    '#title' => t('Publisher & Place'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $preference1_data->publisher,
  );
  $form['preference1']['edition1'] = array(
    '#type' => 'textfield',
    '#title' => t('Edition'),
    '#size' => 4,
    '#maxlength' => 2,
    '#required' => TRUE,
    '#default_value' => $preference1_data->edition,
  );
  $form['preference1']['year1'] = array(
    '#type' => 'textfield',
    '#title' => t('Year of pulication'),
    '#size' => 4,
    '#maxlength' => 4,
    '#required' => TRUE,
    '#default_value' => $preference1_data->year,
  );
  if($preference2_data){
  $form['preference2'] = array(
    '#type' => 'fieldset',
    '#title' => t('Book Preference 2'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['preference2']['book2'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the book'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $preference2_data->book,
  );
  $form['preference2']['author2'] = array(
    '#type' => 'textfield',
    '#title' => t('Author Name'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $preference2_data->author,
  );
  $form['preference2']['isbn2'] = array(
    '#type' => 'textfield',
    '#title' => t('ISBN No'),
    '#size' => 30,
    '#maxlength' => 25,
    '#required' => TRUE,
    '#default_value' => $preference2_data->isbn,
  );
  $form['preference2']['publisher2'] = array(
    '#type' => 'textfield',
    '#title' => t('Publisher & Place'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $preference2_data->publisher,
  );
  $form['preference2']['edition2'] = array(
    '#type' => 'textfield',
    '#title' => t('Edition'),
    '#size' => 4,
    '#maxlength' => 2,
    '#required' => TRUE,
    '#default_value' => $preference2_data->edition,
  );
  $form['preference2']['year2'] = array(
    '#type' => 'textfield',
    '#title' => t('Year of pulication'),
    '#size' => 4,
    '#maxlength' => 4,
    '#required' => TRUE,
    '#default_value' => $preference2_data->year,
  );
}
if($preference3_data){
  $form['preference3'] = array(
    '#type' => 'fieldset',
    '#title' => t('Book Preference 3'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['preference3']['book3'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the book'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $preference3_data->book,
  );
  $form['preference3']['author3'] = array(
    '#type' => 'textfield',
    '#title' => t('Author Name'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $preference3_data->author,
  );
  $form['preference3']['isbn3'] = array(
    '#type' => 'textfield',
    '#title' => t('ISBN No'),
    '#size' => 30,
    '#maxlength' => 25,
    '#required' => TRUE,
    '#default_value' => $preference3_data->isbn,
  );
  $form['preference3']['publisher3'] = array(
    '#type' => 'textfield',
    '#title' => t('Publisher & Place'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $preference3_data->publisher,
  );
  $form['preference3']['edition3'] = array(
    '#type' => 'textfield',
    '#title' => t('Edition'),
    '#size' => 4,
    '#maxlength' => 2,
    '#required' => TRUE,
    '#default_value' => $preference3_data->edition,
  );
  $form['preference3']['year3'] = array(
    '#type' => 'textfield',
    '#title' => t('Year of pulication'),
    '#size' => 4,
    '#maxlength' => 4,
    '#required' => TRUE,
    '#default_value' => $preference3_data->year,
  );
}

  /* hidden fields */
  $form['hidden_proposal_id'] = array(
    '#type' => 'hidden',
    '#value' => $proposal_id,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'form_edit'),
  );
  return $form;
}

function proposal_edit_form_validate($form, &$form_state, &$nonaicte_book)

{

  if (($form_state['values']['all_state'] === '') || ($form_state['values']['all_state'] === 0)) {
    form_set_error('all_state', t('You must select a state.'));
  }

  
   

  /* mobile */
  if (!preg_match('/^[0-9\ \+]{0,15}$/', $form_state['values']['mobile']))
    form_set_error('mobile', t('Invalid mobile number'));

  /* date of completion */
  if (!preg_match('/^[0-9]{1,2}-[0-9]{1,2}-[0-9]{4}$/', $form_state['values']['completion_date']))
    form_set_error('completion_date', t('Invalid expected date of completion'));

  list($d, $m, $y) = explode('-', $form_state['values']['completion_date']);
  $d = (int)$d; $m = (int)$m; $y = (int)$y;
  if (!checkdate($m, $d, $y))
    form_set_error('completion_date', t('Invalid expected date of completion'));
  //if (mktime(0, 0, 0, $m, $d, $y) <= time())
  //form_set_error('completion_date', t('Expected date of completion should be in future'));  

  /* edition */
  if (!preg_match('/^[1-9][0-9]{0,1}$/', $form_state['values']['edition1']))
    form_set_error('edition1', t('Invalid edition for Book Preference 1'));
  if($form_state['values']['edition2']){
  if (!preg_match('/^[1-9][0-9]{0,1}$/', $form_state['values']['edition2']))
    form_set_error('edition2', t('Invalid edition for Book Preference 2'));
  if (!preg_match('/^[1-9][0-9]{0,1}$/', $form_state['values']['edition3']))
    form_set_error('edition3', t('Invalid edition for Book Preference 3'));
}


  /* year of publication */
  if (!preg_match('/^[1-3][0-9][0-9][0-9]$/', $form_state['values']['year1']))
    form_set_error('year1', t('Invalid year of pulication for Book Preference 1'));
  if($form_state['values']['edition2']){
  if (!preg_match('/^[1-3][0-9][0-9][0-9]$/', $form_state['values']['year2']))
    form_set_error('year2', t('Invalid year of pulication for Book Preference 2'));
  if (!preg_match('/^[1-3][0-9][0-9][0-9]$/', $form_state['values']['year3']))
    form_set_error('year3', t('Invalid year of pulication for Book Preference 3'));
}

  /* year of publication */
  $cur_year = date('Y');
  if ((int)$form_state['values']['year1'] > $cur_year)
    form_set_error('year1', t('Year of pulication should be not in the future for Book Preference 1'));
  if ((int)$form_state['values']['year2'] > $cur_year)
    form_set_error('year2', t('Year of pulication should be not in the future for Book Preference 2'));
  if ((int)$form_state['values']['year3'] > $cur_year)
    form_set_error('year3', t('Year of pulication should be not in the future for Book Preference 3'));
  
  /* isbn */
  if (!preg_match('/^[0-9\-xX]+$/', $form_state['values']['isbn1']))
    form_set_error('isbn1', t('Invalid ISBN for Book Preference 1'));
  if($form_state['values']['edition2']){
  if (!preg_match('/^[0-9\-xX]+$/', $form_state['values']['isbn2']))
    form_set_error('isbn2', t('Invalid ISBN for Book Preference 2'));
  if (!preg_match('/^[0-9\-xX]+$/', $form_state['values']['isbn3']))
    form_set_error('isbn3', t('Invalid ISBN for Book Preference 3'));
}
  return;
}

function proposal_edit_form_submit($form, &$form_state)
{
  /* completion date to timestamp */
  list($d, $m, $y) = explode('-', $form_state['values']['completion_date']);
  $completion_date_timestamp = mktime(0, 0, 0, $m, $d, $y);
 
  $proposal_id = $form_state['values']['hidden_proposal_id'];

  
  $query = "UPDATE {textbook_companion_proposal} SET full_name = '".$form_state['values']['full_name']."', mobile = '".$form_state['values']['mobile']."', how_project = '".$form_state['values']['how_project']."', course = '".$form_state['values']['course']."', branch = '".$form_state['values']['branch']."', university = '".$form_state['values']['university']."', city = '".$form_state['values']['city']."', state = '". $form_state['values']['all_state'] ."', faculty = '".$form_state['values']['faculty']."', reviewer = '".$form_state['values']['reviewer']."', completion_date = $completion_date_timestamp, operating_system= '".$form_state['values']['operating_system']."', scilab_version= '".$form_state['values']['scilab_version']."' WHERE id =".$proposal_id;


  db_query($query);


/*db_query("UPDATE {textbook_companion_proposal} SET full_name = '%s', mobile = '%s', how_project = '%s', course = '%s', branch = '%s', university = '%s', faculty = '%s', reviewer = '%s', completion_date = %d, operating_system= '%s', scilab_version= '%s' WHERE id = %d",
    $form_state['values']['full_name'],
    $form_state['values']['mobile'],
    $form_state['values']['how_project'],
    $form_state['values']['course'],
    $form_state['values']['branch'],
    $form_state['values']['university'],
    $form_state['values']['faculty'],
    $form_state['values']['reviewer'],
    $completion_date_timestamp,
    $form_state['values']['operating_system'],
    $form_state['values']['scilab_version'],
    $proposal_id); */

  $preference1_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 1);
  $preference1_data = db_fetch_object($preference1_q);
  if ($preference1_data)
  	$preference1_id = $preference1_data->id;
  $preference2_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 2);
  $preference2_data = db_fetch_object($preference2_q);
  if ($preference2_data)
  	$preference2_id = $preference2_data->id;
  $preference3_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 3);
  $preference3_data = db_fetch_object($preference3_q);
  if ($preference3_data)
  	$preference3_id = $preference3_data->id;

  if ($preference1_data)
  {
    del_book_pdf($preference1_data->id);
		db_query("UPDATE {textbook_companion_preference} SET book = '%s', author = '%s', isbn = '%s', publisher = '%s', edition = %d, year = %d WHERE id = %d",
			$form_state['values']['book1'],
			$form_state['values']['author1'],
			$form_state['values']['isbn1'],
			$form_state['values']['publisher1'],
			$form_state['values']['edition1'],
			$form_state['values']['year1'],
			$preference1_id);
	}
  if ($preference2_data)
  {
    del_book_pdf($preference2_data->id);
		db_query("UPDATE {textbook_companion_preference} SET book = '%s', author = '%s', isbn = '%s', publisher = '%s', edition = %d, year = %d WHERE id = %d",
			$form_state['values']['book2'],
			$form_state['values']['author2'],
			$form_state['values']['isbn2'],
			$form_state['values']['publisher2'],
			$form_state['values']['edition2'],
			$form_state['values']['year2'],
			$preference2_id);
	}
  if ($preference3_data)
  {
    del_book_pdf($preference3_data->id);
		db_query("UPDATE {textbook_companion_preference} SET book = '%s', author = '%s', isbn = '%s', publisher = '%s', edition = %d, year = %d WHERE id = %d",
			$form_state['values']['book3'],
			$form_state['values']['author3'],
			$form_state['values']['isbn3'],
			$form_state['values']['publisher3'],
			$form_state['values']['edition3'],
			$form_state['values']['year3'],
			$preference3_id);
	}
  drupal_set_message(t('Proposal Updated'), 'status');
}


function textbook_companion_path() {
  return $_SERVER['DOCUMENT_ROOT'] . base_path() . 'uploads/';
}

function del_book_pdf($book_id)
{
  $root_path = textbook_companion_path();
  $dir_path = $root_path . "latex/";
  $pdf_filename = "book_" . $book_id . ".pdf";
  if (file_exists($dir_path . $pdf_filename))
    unlink($dir_path . $pdf_filename);
}

/******************************************************************************************************************/
function _completed_lab_proposal_all()
{
  /* get pending proposals to be approved */
  $proposal_rows = array();
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE approval_status = 3 ORDER BY id DESC");
  while ($proposal_data = db_fetch_object($proposal_q))
  {
    $approval_status = '';
    switch ($proposal_data->approval_status)
    {
    case 0: $approval_status = 'Pending'; break;
    case 1: $approval_status = 'Approved'; break;
    case 2: $approval_status = 'Dis-approved'; break;
    case 3: $approval_status = 'Solved'; break;
    default: $approval_status = 'Unknown'; break;
    }
    $proposal_rows[] = array(date('d-m-Y', $proposal_data->creation_date), l($proposal_data->name, 'user/' . $proposal_data->uid), $proposal_data->lab_title, $proposal_data->department, $proposal_data->city,
	$proposal_data->state, $approval_status, l('Edit', 'form_edit/lm/edit/' . $proposal_data->id));
  }

  /* check if there are any pending proposals */
  if (!$proposal_rows)
  {
    drupal_set_message(t('There are no proposals.'), 'status');
    return '';
  }

  $proposal_header = array('Date of Submission', 'Name', 'Title of the Lab', 'Department', 'City', 'State', 'Status');
  $output = theme_table($proposal_header, $proposal_rows);
  return $output; 
}

function _completed_lab_proposal_edit_form($form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = (int)arg(3);
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  if ($proposal_q)
  {
    if ($proposal_data = db_fetch_object($proposal_q))
    {
      /* everything ok */
    } else {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('form_edit/lm/all');
      return;
    }
  } else {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('form_edit/lm/all');
    return;
  }

  $user_data = user_load($proposal_data->uid);

  $form['name_title'] = array(
    '#type' => 'select',
    '#title' => t('Title'),
    '#options' => array('Mr' => 'Mr', 'Ms' => 'Ms', 'Mrs' => 'Mrs', 'Dr' => 'Dr', 'Prof' => 'Prof'),
    '#required' => TRUE,
    '#default_value' => $proposal_data->name_title,
  );
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name of the Proposer'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $proposal_data->name,
  );
  $form['email_id'] = array(
    '#type' => 'item',
    '#title' => t('Email'),
    '#value' => $user_data->mail,
  );
  $form['contact_ph'] = array(
    '#type' => 'textfield',
    '#title' => t('Contact No.'),
    '#size' => 30,
    '#maxlength' => 15,
    '#required' => TRUE,
    '#default_value' => $proposal_data->contact_ph,
  );
  $form['department'] = array(
    '#type' => 'select',
    '#title' => t('Department/Branch'),
    '#options' => array('' => 'Please select...',
                        'Computer Engineering' => 'Computer Engineering',
                        'Electrical Engineering' => 'Electrical Engineering',
                        'Electronics Engineering' => 'Electronics Engineering',
                        'Chemical Engineering' => 'Chemical Engineering',
                        'Instrumentation Engineering' => 'Instrumentation Engineering',
                        'Mechanical Engineering' => 'Mechanical Engineering',
                        'Civil Engineering' => 'Civil Engineering',
                        'Physics' => 'Physics',
                        'Mathematics' => 'Mathematics',
                        'Others' => 'Others'),
    '#required' => TRUE,
    '#default_value' => $proposal_data->department,
  );
  $form['university'] = array(
    '#type' => 'textfield',
    '#title' => t('University/Institute'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $proposal_data->university,
  );
  $form['city'] = array(
    '#type' => 'textfield',
    '#title' => t('City/Village'),
    '#size' => 50,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $proposal_data->city,
  );
 
  $form['all_state'] = array(
    '#type' => 'select',    
    '#title' => t('States'),
    '#options' => array(
    				 '0' => 'Please select...',
					 'Andhra Pradesh' => t('Andhra Pradesh'),
					 'Arunachal Pradesh' => 'Arunachal Pradesh',
					 'Assam' => 'Assam',
					 'Bihar' => 'Bihar',
					 'Chhattisgarh' => 'Chhattisgarh',
					 'Goa' => 'Goa',
					 'Gujarat' => 'Gujarat',
					 'Haryana' => 'Haryana',
					 'Himachal Pradesh' => 'Himachal Pradesh',
					 'Jammu & Kashmir' => 'Jammu & Kashmir',
					 'Jharkhand' => 'Jharkhand',
					 'Karnataka' => 'Karnataka',
					 'Kerala' => 'Kerala',
					 'Madhya Pradesh' => 'Madhya Pradesh',
					 'Maharashtra' => 'Maharashtra',
					 'Manipur' => 'Manipur',
					 'Meghalaya' => 'Meghalaya',
					 'Mizoram' => 'Mizoram',
					 'Nagaland' => 'Nagaland',
					 'Odisha' => 'Odisha',
					 'Punjab' => 'Punjab',
					 'Rajasthan' => 'Rajasthan',
					 'Sikkim' => 'Sikkim',
					 'Tamil Nadu' => 'Tamil Nadu',
					 'Telangana' => 'Telangana',
					 'Tripura' => 'Tripura',
					 'Uttarakhand' => 'Uttarakhand',
					 'Uttar Pradesh'=> 'Uttar Pradesh',
					 'West Bengal' => 'West Bengal',
					 'Andaman & Nicobar' => 'Andaman & Nicobar',
					 'Chandigarh' => 'Chandigarh',
					 'Dadra and Nagar Haveli' => 'Dadra and Nagar Haveli',
					 'Daman & Diu' => 'Daman & Diu',
					 'Delhi' => 'Delhi',
					 'Lakshadweep' => 'Lakshadweep',
					 'Puducherry' => 'Puducherry',
					 'Others' => 'Others',
                ),
    '#required' => TRUE,
    '#tree' => TRUE,
    '#validated' => TRUE,
    '#default_value' => $proposal_data->state,
  );
  $form['lab_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the Lab'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $proposal_data->lab_title,
  );

  /* get experiment details */
  $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d ORDER BY id ASC", $proposal_id);
    $form['lab_experiment'] = array(
      '#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#tree' => TRUE,
    );
  for ($counter = 1; $counter <= 15; $counter++) {
  	$experiment_title = '';
  	$experiment_data = db_fetch_object($experiment_q);
  	if ($experiment_data) {
  		$experiment_title = $experiment_data->title;
			$form['lab_experiment']['update'][$experiment_data->id] = array(
				'#type' => 'textfield',
				'#title' => t('Title of the Experiment ') . $counter,
				'#size' => 50,
				'#required' => FALSE,
				'#tree' => TRUE,
				'#default_value' => $experiment_title,
			);
  	} else {
			$form['lab_experiment']['insert'][$counter] = array(
				'#type' => 'textfield',
				'#title' => t('Title of the Experiment ') . $counter,
				'#size' => 50,
				'#required' => FALSE,
				'#default_value' => $experiment_title,
			);
		}
	}

	if ($proposal_data->solution_provider_uid == 0) {
		$solution_provider_user = 'Open';
	} else if ($proposal_data->solution_provider_uid == $proposal_data->uid) {
		$solution_provider_user = 'Proposer';
	} else {
		$user_data = user_load($proposal_data->solution_provider_uid);
		if (!$user_data) {
			$solution_provider_user = 1;
			drupal_set_message('Solution provider user name is invalid', 'error');
		}
		$solution_provider_user = $user_data->name;
	}
  $form['solution_provider_uid'] = array(
    '#type' => 'item',
    '#title' => t('Who will provide the solution'),
    '#value' => $solution_provider_user,
  );

  $form['open_solution'] = array(
    '#type' => 'checkbox',
    '#title' => t('Open the solution for everyone'),
  );

  $form['solution_display'] = array(
    '#type' => 'radios',
    '#title' => t('Do you want to display the solution on the www.scilab.in website'),
    '#options' => array('1' => 'Yes', '2' => 'No'),
    '#required' => TRUE,
    '#default_value' => ($proposal_data->solution_display == 1) ? "1" : "2",
  );

  $form['delete_proposal'] = array(
    '#type' => 'checkbox',
    '#title' => t('Delete Proposal')
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'form_edit/lm/all'),
  );
  return $form;
}

function _completed_lab_proposal_edit_form_validate($form, &$form_state)
{
  $proposal_id = (int)arg(3);
  if (($form_state['values']['all_state'] === '') || ($form_state['values']['all_state'] === 0)) {
    form_set_error('all_state', t('You must select a state.'));
  }

  /* check before delete proposal */
  if ($form_state['values']['delete_proposal'] == 1) {
    $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d", $proposal_id);
    while ($experiment_data = db_fetch_object($experiment_q)) {
      $solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE experiment_id = %d", $experiment_data->id);
      if (db_fetch_object($solution_q)) {
        form_set_error('', t('Cannot delete proposal since there are solutions already uploaded. Use the "Bulk Manage" interface to delete this proposal'));
      }
    }
  }
  return;
}

function _completed_lab_proposal_edit_form_submit($form, &$form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = (int)arg(3);
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  if ($proposal_q)
  {
    if ($proposal_data = db_fetch_object($proposal_q))
    {
      /* everything ok */
    } else {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('form_edit/lm/all');
      return;
    }
  } else {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('form_edit/lm/all');
    return;
  }

  /* delete proposal */
  if ($form_state['values']['delete_proposal'] == 1) {
    db_query("DELETE FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
    db_query("DELETE FROM {lab_migration_experiment} WHERE proposal_id = %d", $proposal_id);
    drupal_set_message(t('Proposal Delete'), 'status');
    drupal_goto('form_edit/lm/all');
    return;
  }

  if ($form_state['values']['open_solution'] == 1) {
    db_query("UPDATE {lab_migration_proposal} SET solution_provider_uid = %d, solution_status = %d, solution_provider_name_title = '', 	solution_provider_name = '', solution_provider_contact_ph = '', solution_provider_department 	= '', solution_provider_university = '' WHERE id = %d", 0, 0, $proposal_id);
  }

  $solution_display = 0;
  if ($form_state['values']['solution_display'] == 1) {
  	$solution_display = 1;
  } else {
  	$solution_display = 0;
  }

  /* update proposal */
  $result = db_query("UPDATE {lab_migration_proposal} SET name_title = '%s', name = '%s', contact_ph = '%s', department = '%s', university = '%s', city = '%s', state = '%s', lab_title = '%s', solution_display = %d WHERE id = %d",
    $form_state['values']['name_title'],
    $form_state['values']['name'],
    $form_state['values']['contact_ph'],
    $form_state['values']['department'],
    $form_state['values']['university'],
    $form_state['values']['city'],
    $form_state['values']['all_state'],    
    $form_state['values']['lab_title'],
    $solution_display,
    $proposal_id);
  if (!$result)
  {
    drupal_set_message(t('Error receiving your proposal. Please try again.'), 'error');
    return;
  }

  /* updating existing experiments */
  foreach ($form_state['values']['lab_experiment']['update'] as $update_id => $update_value) {
    if (strlen(trim($update_value)) >= 1) {
      $result = db_query("UPDATE {lab_migration_experiment} SET title = '%s' WHERE id = %d", trim($update_value), $update_id);
      if (!$result)
      {
        drupal_set_message(t('Could not update Title of the Experiment : ') . trim($update_value), 'error');
      }
    } else {
      db_query("DELETE FROM {lab_migration_experiment} WHERE id = %d LIMIT 1", $update_id);
    }
  }

  /* inserting new experiments */
  $number_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d ORDER BY number DESC LIMIT 1", $proposal_id);
  if ($number_data = db_fetch_object($number_q)) {
    $number = (int)$number_data->number;
  	$number++;
  } else {
  	$number = 1;
  }
  foreach ($form_state['values']['lab_experiment']['insert'] as $insert_id => $insert_value) {
  	if (strlen(trim($insert_value)) >= 1) {
  		$result = db_query("INSERT INTO {lab_migration_experiment} (proposal_id, number, title) VALUES (%d, %d, '%s')", $proposal_id, $number, trim($insert_value));
			if (!$result)
			{
				drupal_set_message(t('Could not insert Title of the Experiment : ') . trim($insert_value), 'error');
			} else {
				$number++;
			}
		}
	}

  drupal_set_message(t('Proposal Updated'), 'status');
}

