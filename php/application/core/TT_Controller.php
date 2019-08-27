<?php (defined('BASEPATH')) OR exit('No direct script access allowed');

include_once(APPPATH."core/REST_Controller.php");

class TT_Controller extends REST_Controller {

	function __construct()
	{
		parent::__construct();
		if(!isset($this->session->userdata['account'])){
			$this->load->helper('url');
			redirect('/auth/login');
		}
	}
}	


