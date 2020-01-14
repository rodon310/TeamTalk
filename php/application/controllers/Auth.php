<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

include_once(APPPATH."core/REST_Controller.php");

class Auth extends REST_Controller {

	public function __construct()
	{
		parent::__construct();
		$this->load->helper('url');
		$this->load->model('admin_model');
	}

	public function index(){
		$this->login();
	}


	public function login_get(){
		if(isset($this->session->userdata['account'])){
			redirect('/home');
			exit();
		}
		$this->load->view('auth/login');
	}



	public function currentuser(){
		$result = array('name'=>'guest');
		if(isset($this->session->userdata['account'])){
			$result['avatar'] = 'https://gw.alipayobjects.com/zos/antfincdn/XAosXuNZyF/BiazfanxmamNRoxxVxka.png';
			$account = $this->session->userdata['account'];
			if(is_array($account)) {
				$result['name'] = $account['uname']; 
				if($account['type'] == "1"){
					$result['authority'] = "admin";
				}else {
					$result['authority'] = "user";
				}   
			}else {
				$result['name'] = $account;    
				$result['authority'] = $account;
			}

		}
		$this->json_out($result);
	}

	public function userregister_post() {
		$req_data = $this->json_input();
		$out_result = array('status'=>'ok','currentAuthority'=>'user');	

		if(!isset($req_data['mail'])) {
			$out_result['status'] = 'error';
			$out_result['msg'] = 'lost mail';
			$this->json_out($out_result);
			return;
		}

		if((!isset($req_data['password'])) || (!isset($req_data['confirm']))) {
			$out_result['status'] = 'error'; 
			$out_result['msg'] = 'lost password or confirm';
			$this->json_out($out_result);
			return;
		}else if($req_data['password'] != $req_data['confirm']) {
			$out_result['status'] = 'error'; 
			$out_result['msg'] = 'password not equal confirm'; 
			$this->json_out($out_result); 
			return;
		}

		$params = array(
			'uname'=>$req_data['mail'],
			'pwd'=>md5($req_data['password']),
			'type'=>2,
			'updated'=>time(),
			'created'=>time()
		);
		$result = $this->admin_model->insert($params);
		
		if(!$result) {
			$out_result['status'] = 'error';
			$out_result['currentAuthority'] = '';	
		}
		$this->json_out($out_result);
	}

	public function userlogin_post(){
		$submit = $this->json_input('submit');
		$account = $this->json_input('account');
		$password = $this->json_input('password');
		$result = array('status'=>'ok','msg'=>'wrong');
		if($submit){
			$admin = $this->admin_model->getOne(array('uname'=>$account));
			if(md5($password) == $admin['pwd']){
				$result['msg'] = 'right';
				if(isset($admin['type'])) {
					$session = array(
						'account' => $admin
					);
					$this->session->set_userdata($session);
					if($admin['type'] == "1"){
						$result['currentAuthority'] = 'admin';
					}else {
						$result['currentAuthority'] = 'user';
					}
				}else {
					$session = array(
						'account' => $account
					);
					$this->session->set_userdata($session);
					$result['currentAuthority'] = 'admin';
				}

			}
		}
		$this->json_out($result);
	}


	public function userlogout(){
		$this->session->sess_destroy();
		$result = array('status'=>'ok','msg'=>'logout');
		$this->json_out($result);
	}

	public function login_post(){
		$submit = $this->input->post('submit');
		$account = $this->input->post('account');
		$password = $this->input->post('password');
		$result = array('status'=>0,'msg'=>'wrong');
		if($submit){
			$admin = $this->admin_model->getOne(array('uname'=>$account));
			if(md5($password) == $admin['pwd']){
				$session = array(
					'account' => $account
				);
				$this->session->set_userdata($session);
				$result['msg'] = 'right';
			}
		}
		$this->json_out($result);
	}

	public function logout(){
		$this->session->sess_destroy();
		redirect('/auth/login');
	}
}
