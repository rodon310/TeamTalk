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
            $result['name'] = $this->session->userdata['account'];    
        }
        $this->json_out($result);
    }

    public function userlogin_post(){
        $submit = $this->json_input('submit');
		$account = $this->json_input('account');
		$password = $this->json_input('password');
        $result = array('status'=>'ok','msg'=>'wrong');
        if($submit){
			$admin = $this->admin_model->getOne(array('uname'=>$account));
			if(md5($password) == $admin['pwd']){
				$session = array(
					'account' => $account
				);
                $this->session->set_userdata($session);
                $result['msg'] = 'right';
                $result['currentAuthority'] = 'admin';
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
