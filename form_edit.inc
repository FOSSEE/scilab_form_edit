<?php
// $Id$

function _completed_proposal_all()
{
		function _tbc_ext($status, $preference_id)
		{
				if ($status == "Approved") {
						return " | " . l("ER", "tbc_external_review/add_book/" . $preference_id);
				} //$status == "Approved"
				else {
						return "";
				}
		}
		/* get pending proposals to be approved */
		$proposal_rows = array();

		$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE proposal_status = 3 ORDER BY id ASC");

		while ($proposal_data = $proposal_q->fetchObject()) {
				/* get preference */
				$preference_q = db_query("SELECT pe.*, po.approval_date as approval_date
  FROM textbook_companion_preference pe LEFT JOIN textbook_companion_proposal po ON pe.proposal_id = po.id
  WHERE po.proposal_status = 3 AND pe.proposal_id = :proposal_id AND pe.approval_status = 1 AND pe.category > 0  ORDER BY pe.book ASC", array(
						":proposal_id" => $proposal_data->id
				));
				/* $preference_q = db_query("SELECT * FROM textbook_companion_preference WHERE proposal_id = :proposal_id AND approval_status = 1 LIMIT 1",array(":proposal_id" =>$proposal_data->id));  */
				$preference_data = $preference_q->fetchObject();
				/* if(!$preference_data){
				$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = :proposal_id AND pref_number = 1 LIMIT 1", array(":proposal_id" => $proposal_data->id));  
				$preference_data = $preference_q->fetchObject();
				}*/
				$proposal_status = '';
				switch ($proposal_data->proposal_status) {
						case 0:
								$proposal_status = 'Pending';
								break;
						case 1:
								$proposal_status = 'Approved';
								break;
						case 2:
								$proposal_status = 'Dis-approved';
								break;
						case 3:
								$proposal_status = 'Completed';
								break;
						case 4:
								$proposal_status = 'External';
								break;
						default:
								$proposal_status = 'Unknown';
								break;
				} //$proposal_data->proposal_status
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
		} //$proposal_data = $proposal_q->fetchObject()

		/* check if there are any pending proposals */
		if (!$proposal_rows) {
				drupal_set_message(t('There are no proposals.'), 'status');
				return '';
		} //!$proposal_rows
		$prop_count_query = db_query("SELECT COUNT( pe.book ) AS book_count FROM textbook_companion_preference pe LEFT JOIN textbook_companion_proposal po ON pe.proposal_id = po.id WHERE po.proposal_status =3 AND pe.approval_status =1 AND pe.category>0");
		$prop_count_data = $prop_count_query->fetchObject();

		$proposal_header = array(
				'Date of Submission',
				'Title of the Book',
				'Contributor Name',
				'City',
				'State',
				'Expected Date of Completion',
				'Status',
				'Action'
		);
		$output .= "Completed Books: " . $prop_count_data->book_count;
		$output .= theme('table', array(
				'header' => $proposal_header,
				'rows' => $proposal_rows,
				'attributes' => array(
						'style' => 'overflow: scroll'
				)
		));

		return $output;
}

/******************************************************************************/
/**************************** PROPOSAL EDIT FORM ******************************/
/******************************************************************************/

function proposal_edit_form($form, &$form_state, $nonaicte_book)
{
		global $user;

		/* get current proposal */
		$proposal_id = arg(3);
		$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = :prop_id AND proposal_status = 3", array(
				":prop_id" => $proposal_id
		));
		if ($proposal_q) {
				$proposal_data = $proposal_q->fetchObject();
				if (!$proposal_data) {
						drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
						drupal_goto('form_edit');
						return;
				} //!$proposal_data
		} //$proposal_q
		else {
				drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
				drupal_goto('form_edit');
				return;
		}

		$user_data = user_load($proposal_data->uid);

		$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = :proposal_id AND approval_status = 1 LIMIT 1", array(
				":proposal_id" => $proposal_id
		));
		$preference_data = $preference_q->fetchObject();
		/*$preference2_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = :proposal_id AND pref_number = :pref_number LIMIT 1",array(":proposal_id"=>$proposal_id, ":pref_number"=> 2));
		$preference2_data = $preference2_q->fetchObject();
		$preference3_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = :proposal_id AND pref_number = :pref_number LIMIT 1", array(":proposal_id"=>$proposal_id, ":pref_number"=>3));
		$preference3_data = $preference3_q->fetchObject();
		*/
		$form['full_name'] = array(
				'#type' => 'textfield',
				'#title' => t('Full Name'),
				'#size' => 30,
				'#maxlength' => 50,
				'#required' => TRUE,
				'#default_value' => $proposal_data->full_name
		);
		$form['email_id'] = array(
				'#type' => 'textfield',
				'#title' => t('Email'),
				'#size' => 30,
				'#value' => $user_data->mail,
				'#disabled' => TRUE
		);
		$form['mobile'] = array(
				'#type' => 'textfield',
				'#title' => t('Mobile No.'),
				'#size' => 30,
				'#maxlength' => 15,
				'#required' => TRUE,
				'#default_value' => $proposal_data->mobile
		);
		$form['how_project'] = array(
				'#type' => 'select',
				'#title' => t('How did you come to know about this project'),
				'#options' => array(
						'Scilab Website' => 'Scilab Website',
						'Friend' => 'Friend',
						'Professor/Teacher' => 'Professor/Teacher',
						'Mailing List' => 'Mailing List',
						'Poster in my/other college' => 'Poster in my/other college',
						'Others' => 'Others'
				),
				'#required' => TRUE,
				'#default_value' => $proposal_data->how_project
		);
		$form['course'] = array(
				'#type' => 'textfield',
				'#title' => t('Course'),
				'#size' => 30,
				'#maxlength' => 50,
				'#required' => TRUE,
				'#default_value' => $proposal_data->course
		);
		$form['branch'] = array(
				'#type' => 'select',
				'#title' => t('Department/Branch'),
				'#options' => _form_edit_list_of_departments($proposal_data->branch),
				'#required' => TRUE,
				'#default_value' => $proposal_data->branch
		);
		$form['university'] = array(
				'#type' => 'textfield',
				'#title' => t('University/Institute'),
				'#size' => 30,
				'#maxlength' => 100,
				'#required' => TRUE,
				'#default_value' => $proposal_data->university
		);
		$form['country'] = array(
				'#type' => 'select',
				'#title' => t('Country'),
				'#options' => array(
						'India' => 'India',
						'Others' => 'Others'
				),
				'#required' => TRUE,
				'#tree' => TRUE,
				'#validated' => TRUE,
				'#default_value' => $proposal_data->country
		);
		$form['other_country'] = array(
				'#type' => 'textfield',
				'#title' => t('Other than India'),
				'#size' => 100,
				'#attributes' => array(
						'placeholder' => t('Enter your country name')
				),
				'#default_value' => $proposal_data->country,
				'#states' => array(
						'visible' => array(
								':input[name="country"]' => array(
										'value' => 'Others'
								)
						)
				)
		);
		$form['other_state'] = array(
				'#type' => 'textfield',
				'#title' => t('State other than India'),
				'#size' => 100,
				'#attributes' => array(
						'placeholder' => t('Enter your state/region name')
				),
				'#default_value' => $proposal_data->state,
				'#states' => array(
						'visible' => array(
								':input[name="country"]' => array(
										'value' => 'Others'
								)
						)
				)
		);
		$form['other_city'] = array(
				'#type' => 'textfield',
				'#title' => t('City other than India'),
				'#size' => 100,
				'#attributes' => array(
						'placeholder' => t('Enter your city name')
				),
				'#default_value' => $proposal_data->city,
				'#states' => array(
						'visible' => array(
								':input[name="country"]' => array(
										'value' => 'Others'
								)
						)
				)
		);
		$form['all_state'] = array(
				'#type' => 'select',
				'#title' => t('State'),
				'#options' => _tbc_list_of_states(),
				'#validated' => TRUE,
				'#default_value' => $proposal_data->state,
				'#states' => array(
						'visible' => array(
								':input[name="country"]' => array(
										'value' => 'India'
								)
						)
				)
		);
		$form['city'] = array(
				'#type' => 'select',
				'#title' => t('City'),
				'#options' => _tbc_list_of_cities(),
				'#default_value' => $proposal_data->city,
				'#states' => array(
						'visible' => array(
								':input[name="country"]' => array(
										'value' => 'India'
								)
						)
				)
		);
		$form['pincode'] = array(
				'#type' => 'textfield',
				'#title' => t('Pincode'),
				'#size' => 30,
				'#maxlength' => 6,
				'#default_value' => $proposal_data->pincode,
				'#required' => True,
				'#attributes' => array(
						'placeholder' => 'Enter pincode....'
				)
		);
		$form['faculty'] = array(
				'#type' => 'textfield',
				'#title' => t('College Teacher/Professor'),
				'#size' => 30,
				'#maxlength' => 100,
				'#required' => TRUE,
				'#default_value' => $proposal_data->faculty
		);
		$form['reviewer'] = array(
				'#type' => 'textfield',
				'#title' => t('Reviewer'),
				'#size' => 30,
				'#maxlength' => 100,
				'#default_value' => $proposal_data->reviewer
		);
		$form['completion_date'] = array(
				'#type' => 'textfield',
				'#title' => t('Expected Date of Completion'),
				'#description' => t('Input date format should be DD-MM-YYYY. Eg: 23-03-2011'),
				'#size' => 10,
				'#maxlength' => 10,
				'#default_value' => date('d-m-Y', $proposal_data->completion_date)
		);
		list($scilab, $version) = explode(' ', $proposal_data->scilab_version);
		$scilab_version = $version;
		$form['version'] = array(
				'#type' => 'textfield',
				'#title' => t('Scilab Version'),
				'#size' => 10,
				'#maxlength' => 20,
				'#default_value' => $scilab_version
		);
		$form['operating_system'] = array(
				'#type' => 'textfield',
				'#title' => t('Operating System'),
				'#size' => 30,
				'#maxlength' => 50,
				'#default_value' => $proposal_data->operating_system
		);
		$form['preference'] = array(
				'#type' => 'fieldset',
				'#title' => t('Book Preference'),
				'#collapsible' => TRUE,
				'#collapsed' => FALSE
		);
		$form['preference']['book'] = array(
				'#type' => 'textfield',
				'#title' => t('Title of the book'),
				'#size' => 30,
				'#maxlength' => 100,
				'#required' => TRUE,
				'#default_value' => $preference_data->book
		);
		$form['preference']['author'] = array(
				'#type' => 'textfield',
				'#title' => t('Author Name'),
				'#size' => 30,
				'#maxlength' => 100,
				'#required' => TRUE,
				'#default_value' => $preference_data->author
		);
		$form['preference']['isbn'] = array(
				'#type' => 'textfield',
				'#title' => t('ISBN No'),
				'#size' => 30,
				'#maxlength' => 25,
				'#required' => TRUE,
				'#default_value' => $preference_data->isbn
		);
		$form['preference']['publisher'] = array(
				'#type' => 'textfield',
				'#title' => t('Publisher & Place'),
				'#size' => 30,
				'#maxlength' => 50,
				'#required' => TRUE,
				'#default_value' => $preference_data->publisher
		);
		$form['preference']['edition'] = array(
				'#type' => 'textfield',
				'#title' => t('Edition'),
				'#size' => 4,
				'#maxlength' => 2,
				'#required' => TRUE,
				'#default_value' => $preference_data->edition
		);
		$form['preference']['year'] = array(
				'#type' => 'textfield',
				'#title' => t('Year of pulication'),
				'#size' => 4,
				'#maxlength' => 4,
				'#required' => TRUE,
				'#default_value' => $preference_data->year
		);
		$form['preference']['book_category_'] = array(
				'#type' => 'select',
				'#title' => t('Category'),
				'#options' => _form_edit_list_of_category($preference_data->category)
		);

		/* hidden fields */
		$form['hidden_proposal_id'] = array(
				'#type' => 'hidden',
				'#value' => $proposal_id
		);

		$form['submit'] = array(
				'#type' => 'submit',
				'#value' => t('Submit')
		);
		$form['cancel'] = array(
				'#markup' => l(t('Cancel'), 'form_edit/tbc/all')
		);
		return $form;
}

function proposal_edit_form_validate($form, &$form_state, &$nonaicte_book)
{

		if (($form_state['values']['all_state'] === '') || ($form_state['values']['all_state'] === 0)) {
				form_set_error('all_state', t('You must select a state.'));
		} //($form_state['values']['all_state'] === '') || ($form_state['values']['all_state'] === 0)

		if (!preg_match("/^[1-9]{1}[0-9]{5}$/", $form_state['values']['pincode']))
				form_set_error('pincode', t('Invalid pincode'));

		/* mobile */
		if (!preg_match('/^[0-9\ \+]{0,15}$/', $form_state['values']['mobile']))
				form_set_error('mobile', t('Invalid mobile number'));

		/* date of completion */
		if (!preg_match('/^[0-9]{1,2}-[0-9]{1,2}-[0-9]{4}$/', $form_state['values']['completion_date']))
				form_set_error('completion_date', t('Invalid expected date of completion'));

		list($d, $m, $y) = explode('-', $form_state['values']['completion_date']);
		$d = (int) $d;
		$m = (int) $m;
		$y = (int) $y;
		if (!checkdate($m, $d, $y))
				form_set_error('completion_date', t('Invalid expected date of completion'));
		//if (mktime(0, 0, 0, $m, $d, $y) <= time())
		//form_set_error('completion_date', t('Expected date of completion should be in future'));

		/* edition */
		if (!preg_match('/^[1-9][0-9]{0,1}$/', $form_state['values']['edition']))
				form_set_error('edition', t('Invalid edition for Book Preference '));

		/* year of publication */
		if (!preg_match('/^[1-3][0-9][0-9][0-9]$/', $form_state['values']['year']))
				form_set_error('year', t('Invalid year of pulication for Book Preference '));

		/* year of publication */
		$cur_year = date('Y');
		if ((int) $form_state['values']['year'] > $cur_year)
				form_set_error('year', t('Year of pulication should be not in the future for Book Preference '));

		/* isbn */
		if (!preg_match('/^[0-9\-xX]+$/', $form_state['values']['isbn']))
				form_set_error('isbn', t('Invalid ISBN for Book Preference'));

		/* country */
		if ($form_state['values']['country'] == 'Others') {
				if ($form_state['values']['other_country'] == '') {
						form_set_error('other_country', t('Enter country name'));
				} //$form_state['values']['other_country'] == ''
				else {
						$form_state['values']['country'] = $form_state['values']['other_country'];
				}
				if ($form_state['values']['other_state'] == '') {
						form_set_error('other_state', t('Enter state name'));
				} //$form_state['values']['other_state'] == ''
				else {
						$form_state['values']['all_state'] = $form_state['values']['other_state'];
				}
				if ($form_state['values']['other_city'] == '') {
						form_set_error('other_city', t('Enter city name'));
				} //$form_state['values']['other_city'] == ''
				else {
						$form_state['values']['city'] = $form_state['values']['other_city'];
				}
		} //$form_state['values']['country'] == 'Others'
		else {
				if ($form_state['values']['country'] == '') {
						form_set_error('country', t('Select country name'));
				} //$form_state['values']['country'] == ''
				if ($form_state['values']['all_state'] == '') {
						form_set_error('all_state', t('Select state name'));
				} //$form_state['values']['all_state'] == ''
				if ($form_state['values']['city'] == '') {
						form_set_error('city', t('Select city name'));
				} //$form_state['values']['city'] == ''
		}

		return;
}

function proposal_edit_form_submit($form, &$form_state)
{
		/* completion date to timestamp */
		list($d, $m, $y) = explode('-', $form_state['values']['completion_date']);
		$completion_date_timestamp = mktime(0, 0, 0, $m, $d, $y);

		$proposal_id = $form_state['values']['hidden_proposal_id'];

		db_query("UPDATE {textbook_companion_proposal} SET 
full_name = :full_name,
mobile = :mobile,
how_project = :how_project,
course = :course,
branch = :branch,
university = :university,
country= :country,
city = :city,
pincode =:pincode,
state = :state,
faculty = :faculty,
reviewer = :reviewer,
completion_date = :completion_date,
operating_system = :operating_system,
scilab_version= :scilab_version
WHERE 
id = :id", array(
				":full_name" => $form_state['values']['full_name'],
				":mobile" => $form_state['values']['mobile'],
				":how_project" => $form_state['values']['how_project'],
				":course" => $form_state['values']['course'],
				":branch" => $form_state['values']['branch'],
				":university" => $form_state['values']['university'],
				":country" => $form_state['values']['country'],
				":city" => $form_state['values']['city'],
				":pincode" => $form_state['values']['pincode'],
				":state" => $form_state['values']['all_state'],
				":faculty" => $form_state['values']['faculty'],
				":reviewer" => $form_state['values']['reviewer'],
				":completion_date" => $completion_date_timestamp,
				":operating_system" => $form_state['values']['operating_system'],
				":scilab_version" => $form_state['values']['version'],
				":id" => $proposal_id
		));

		$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = :proposal_id AND approval_status = 1 LIMIT 1", array(
				":proposal_id" => $proposal_id
		));
		$preference_data = $preference_q->fetchObject();

		if ($preference_data) {
				del_book_pdf($preference_data->id);
				db_query("UPDATE {textbook_companion_preference} SET book = :book, author = :author, isbn = :isbn, publisher = :publisher , edition = :edition, year = :year WHERE id = :id", array(
						":book" => $form_state['values']['book'],
						":author" => $form_state['values']['author'],
						":isbn" => $form_state['values']['isbn'],
						":publisher" => $form_state['values']['publisher'],
						":edition" => $form_state['values']['edition'],
						":year" => $form_state['values']['year'],
						":id" => $preference_id
				));
		} //$preference_data

		drupal_set_message(t('Proposal Updated'), 'status');
}


/******************************************************************************************************************/
function _completed_lab_proposal_all()
{
		/* get pending proposals to be approved */
		$proposal_rows = array();
		$proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE approval_status = 3 ORDER BY id DESC");
		while ($proposal_data = $proposal_q->fetchObject()) {
				$approval_status = '';
				switch ($proposal_data->approval_status) {
						case 0:
								$approval_status = 'Pending';
								break;
						case 1:
								$approval_status = 'Approved';
								break;
						case 2:
								$approval_status = 'Dis-approved';
								break;
						case 3:
								$approval_status = 'Solved';
								break;
						default:
								$approval_status = 'Unknown';
								break;
				} //$proposal_data->approval_status
				$proposal_rows[] = array(
						date('d-m-Y', $proposal_data->creation_date),
						l($proposal_data->name, 'user/' . $proposal_data->uid),
						$proposal_data->lab_title,
						$proposal_data->department,
						$proposal_data->city,
						$proposal_data->state,
						$approval_status,
						l('Edit', 'form_edit/lm/edit/' . $proposal_data->id)
				);
		} //$proposal_data = $proposal_q->fetchObject()

		/* check if there are any pending proposals */
		if (!$proposal_rows) {
				drupal_set_message(t('There are no proposals.'), 'status');
				return '';
		} //!$proposal_rows

		$proposal_header = array(
				'Date of Submission',
				'Name',
				'Title of the Lab',
				'Department',
				'City',
				'State',
				'Status'
		);

		$output = theme('table', array(
				'header' => $proposal_header,
				'rows' => $proposal_rows,
				'attributes' => array(
						'style' => 'overflow: scroll'
				)
		));
		return $output;
}

function _completed_lab_proposal_edit_form($form, &$form_state)
{
		global $user;

		/* get current proposal */
		$proposal_id = (int) arg(3);
		$proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = :id", array(
				":id" => $proposal_id
		));
		if ($proposal_q) {
				if ($proposal_data = $proposal_q->fetchObject()) {
						/* everything ok */
				} //$proposal_data = $proposal_q->fetchObject()
				else {
						drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
						drupal_goto('form_edit/lm/all');
						return;
				}
		} //$proposal_q
		else {
				drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
				drupal_goto('form_edit/lm/all');
				return;
		}

		$user_data = user_load($proposal_data->uid);

		$form['name_title'] = array(
				'#type' => 'select',
				'#title' => t('Title'),
				'#options' => array(
						'Mr' => 'Mr',
						'Ms' => 'Ms',
						'Mrs' => 'Mrs',
						'Dr' => 'Dr',
						'Prof' => 'Prof'
				),
				'#required' => TRUE,
				'#default_value' => $proposal_data->name_title
		);
		$form['name'] = array(
				'#type' => 'textfield',
				'#title' => t('Name of the Proposer'),
				'#size' => 90,
				'#maxlength' => 50,
				'#required' => TRUE,
				'#default_value' => $proposal_data->name
		);
		$form['email_id'] = array(
				'#type' => 'item',
				'#title' => t('Email'),
				'#value' => $user_data->mail
		);
		$form['contact_ph'] = array(
				'#type' => 'textfield',
				'#title' => t('Contact No.'),
				'#size' => 30,
				'#maxlength' => 15,
				'#required' => TRUE,
				'#default_value' => $proposal_data->contact_ph
		);
		$form['department'] = array(
				'#type' => 'select',
				'#title' => t('Department/Branch'),
				'#options' => _form_edit_list_of_departments(),
				'#required' => TRUE,
				'#default_value' => $proposal_data->department
		);
		$form['university'] = array(
				'#type' => 'textfield',
				'#title' => t('University/Institute'),
				'#size' => 90,
				'#maxlength' => 50,
				'#required' => TRUE,
				'#default_value' => $proposal_data->university
		);
		$form['country'] = array(
				'#type' => 'select',
				'#title' => t('Country'),
				'#options' => array(
						'India' => 'India',
						'Others' => 'Others'
				),
				'#default_value' => $proposal_data->country,
				'#required' => TRUE,
				'#tree' => TRUE,
				'#validated' => TRUE
		);
		$form['other_country'] = array(
				'#type' => 'textfield',
				'#title' => t('Other than India'),
				'#size' => 100,
				'#default_value' => $proposal_data->country,
				'#attributes' => array(
						'placeholder' => t('Enter your country name')
				),
				'#states' => array(
						'visible' => array(
								':input[name="country"]' => array(
										'value' => 'Others'
								)
						)
				)
		);
		$form['other_state'] = array(
				'#type' => 'textfield',
				'#title' => t('State other than India'),
				'#size' => 100,
				'#default_value' => $proposal_data->state,
				'#attributes' => array(
						'placeholder' => t('Enter your state/region name')
				),
				'#states' => array(
						'visible' => array(
								':input[name="country"]' => array(
										'value' => 'Others'
								)
						)
				)
		);
		$form['other_city'] = array(
				'#type' => 'textfield',
				'#title' => t('City other than India'),
				'#size' => 100,
				'#default_value' => $proposal_data->city,
				'#attributes' => array(
						'placeholder' => t('Enter your city name')
				),
				'#states' => array(
						'visible' => array(
								':input[name="country"]' => array(
										'value' => 'Others'
								)
						)
				)
		);
		$form['all_state'] = array(
				'#type' => 'select',
				'#title' => t('State'),
				'#options' => _tbc_list_of_states(),
				'#default_value' => $proposal_data->state,
				'#validated' => TRUE,
				'#states' => array(
						'visible' => array(
								':input[name="country"]' => array(
										'value' => 'India'
								)
						)
				)
		);
		$form['city'] = array(
				'#type' => 'select',
				'#title' => t('City'),
				'#options' => _tbc_list_of_cities(),
				'#default_value' => $proposal_data->city,
				'#states' => array(
						'visible' => array(
								':input[name="country"]' => array(
										'value' => 'India'
								)
						)
				)
		);
		$form['pincode'] = array(
				'#type' => 'textfield',
				'#title' => t('Pincode'),
				'#size' => 30,
				'#maxlength' => 6,
				'#required' => True,
				'#attributes' => array(
						'placeholder' => 'Enter pincode....'
				),
				'#default_value' => $proposal_data->pincode
		);
		$form['lab_title'] = array(
				'#type' => 'textfield',
				'#title' => t('Title of the Lab'),
				'#size' => 90,
				'#maxlength' => 50,
				'#required' => TRUE,
				'#default_value' => $proposal_data->lab_title
		);

		/* get experiment details */
		$experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = :proposal_id ORDER BY id ASC", array(
				":proposal_id" => $proposal_id
		));
		$form['lab_experiment'] = array(
				'#type' => 'fieldset',
				'#collapsible' => FALSE,
				'#tree' => TRUE
		);
		for ($counter = 1; $counter <= 15; $counter++) {
				$experiment_title = '';
				$experiment_data = $experiment_q->fetchObject();
				if ($experiment_data) {
						$experiment_title = $experiment_data->title;
						$form['lab_experiment']['update'][$experiment_data->id] = array(
								'#type' => 'textfield',
								'#title' => t('Title of the Experiment ') . $counter,
								'#size' => 90,
								'#required' => FALSE,
								'#tree' => TRUE,
								'#default_value' => $experiment_title
						);
				} //$experiment_data
				else {
						$form['lab_experiment']['insert'][$counter] = array(
								'#type' => 'textfield',
								'#title' => t('Title of the Experiment ') . $counter,
								'#size' => 90,
								'#required' => FALSE,
								'#default_value' => $experiment_title
						);
				}
		} //$counter = 1; $counter <= 15; $counter++

		if ($proposal_data->solution_provider_uid == 0) {
				$solution_provider_user = 'Open';
		} //$proposal_data->solution_provider_uid == 0
		else if ($proposal_data->solution_provider_uid == $proposal_data->uid) {
				$solution_provider_user = 'Proposer';
		} //$proposal_data->solution_provider_uid == $proposal_data->uid
		else {
				$user_data = user_load($proposal_data->solution_provider_uid);
				if (!$user_data) {
						$solution_provider_user = 1;
						drupal_set_message('Solution provider user name is invalid', 'error');
				} //!$user_data
				$solution_provider_user = $user_data->name;
		}

		$form['solution_display'] = array(
				'#type' => 'radios',
				'#title' => t('Do you want to display the solution on the www.scilab.in website'),
				'#options' => array(
						'1' => 'Yes',
						'2' => 'No'
				),
				'#required' => TRUE,
				'#default_value' => ($proposal_data->solution_display == 1) ? "1" : "2"
		);
		$form['submit'] = array(
				'#type' => 'submit',
				'#value' => t('Submit')
		);
		$form['cancel'] = array(
				'#markup' => l(t('Cancel'), 'form_edit/lm/all')
		);
		return $form;
}

function _completed_lab_proposal_edit_form_validate($form, &$form_state)
{
		$proposal_id = (int) arg(3);
		if (($form_state['values']['all_state'] === '') || ($form_state['values']['all_state'] === 0)) {
				form_set_error('all_state', t('You must select a state.'));
		} //($form_state['values']['all_state'] === '') || ($form_state['values']['all_state'] === 0)
		if (!preg_match("/^[1-9]{1}[0-9]{5}$/", $form_state['values']['pincode']))
				form_set_error('pincode', t('Invalid pincode'));

		/* check before delete proposal */
		if ($form_state['values']['delete_proposal'] == 1) {
				$experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = :proposal_id", array(
						":proposal_id" => $proposal_id
				));
				while ($experiment_data = $experiment_q->fetchObject()) {
						$solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE experiment_id = :experiment_id", array(
								":experiment_id" => $experiment_data->id
						));
						if ($solution_q->fetchObject()) {
								form_set_error('', t('Cannot delete proposal since there are solutions already uploaded. Use the "Bulk Manage" interface to delete this proposal'));
						} //$solution_q->fetchObject()
				} //$experiment_data = $experiment_q->fetchObject()
		} //$form_state['values']['delete_proposal'] == 1
		return;
}

function _completed_lab_proposal_edit_form_submit($form, &$form_state)
{
		global $user;

		/* get current proposal */
		$proposal_id = (int) arg(3);
		$proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = :id", array(
				":id" => $proposal_id
		));
		if ($proposal_q) {
				if ($proposal_data = $proposal_q->fetchObject()) {
						/* everything ok */
				} //$proposal_data = $proposal_q->fetchObject()
				else {
						drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
						drupal_goto('form_edit/lm/all');
						return;
				}
		} //$proposal_q
		else {
				drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
				drupal_goto('form_edit/lm/all');
				return;
		}

		$solution_display = 0;
		if ($form_state['values']['solution_display'] == 1) {
				$solution_display = 1;
		} //$form_state['values']['solution_display'] == 1
		else {
				$solution_display = 0;
		}

		/* update proposal */
		$result = db_query("UPDATE {lab_migration_proposal} SET 
  name_title = :name_title, 
  name = :name, 
  contact_ph = :contact_ph, 
  department = :department, 
  university = :university,
  city = :city, 
  state = :state, 
  pincode = :pincode, 
  lab_title = :lab_title, 
  solution_display = :solution_display 
  WHERE 
  id = :id", array(
				":name_title" => $form_state['values']['name_title'],
				":name" => $form_state['values']['name'],
				":contact_ph" => $form_state['values']['contact_ph'],
				":department" => $form_state['values']['department'],
				":university" => $form_state['values']['university'],
				":city" => $form_state['values']['city'],
				":state" => $form_state['values']['all_state'],
				":pincode" => $form_state['values']['pincode'],
				":lab_title" => $form_state['values']['lab_title'],
				":solution_display" => $solution_display,
				":id" => $proposal_id
		));
		if (!$result) {
				drupal_set_message(t('Error receiving your proposal. Please try again.'), 'error');
				return;
		} //!$result

		/* updating existing experiments */
		foreach ($form_state['values']['lab_experiment']['update'] as $update_id => $update_value) {
				if (strlen(trim($update_value)) >= 1) {
						$result = db_query("UPDATE {lab_migration_experiment} SET title = :title WHERE id = :id", array(
								":title" => trim($update_value),
								":id" => $update_id
						));
						if (!$result) {
								drupal_set_message(t('Could not update Title of the Experiment : ') . trim($update_value), 'error');
						} //!$result
				} //strlen(trim($update_value)) >= 1
				else {
						db_query("DELETE FROM {lab_migration_experiment} WHERE id = :id LIMIT 1", array(
								":id" => $update_id
						));
				}
		} //$form_state['values']['lab_experiment']['update'] as $update_id => $update_value

		/* inserting new experiments */
		$number_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = :proposal_id ORDER BY number DESC LIMIT 1", array(
				":proposal_id" => $proposal_id
		));
		if ($number_data = $number_q->fetchObject()) {
				$number = (int) $number_data->number;
				$number++;
		} //$number_data = $number_q->fetchObject()
		else {
				$number = 1;
		}
		foreach ($form_state['values']['lab_experiment']['insert'] as $insert_id => $insert_value) {
				if (strlen(trim($insert_value)) >= 1) {
						$result = db_query("INSERT INTO {lab_migration_experiment} (proposal_id, number, title) VALUES (:proposal_id, :number, :title)", array(
								":proposal_id" => $proposal_id,
								":number" => $number,
								":title" => trim($insert_value)
						));
						if (!$result) {
								drupal_set_message(t('Could not insert Title of the Experiment : ') . trim($insert_value), 'error');
						} //!$result
						else {
								$number++;
						}
				} //strlen(trim($insert_value)) >= 1
		} //$form_state['values']['lab_experiment']['insert'] as $insert_id => $insert_value
		
		drupal_set_message(t('Proposal Updated'), 'status');
}
