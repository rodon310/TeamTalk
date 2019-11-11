<?php
/**
 * Short description for Apiv2.php
 *
 * @package Apiv2
 * @author xiaominfc(武汉鸣鸾信息科技有限公司) <xiaominfc@gmail.com>
 * @version 0.1
 * @copyright (C) 2019 xiaominfc(武汉鸣鸾信息科技有限公司) <xiaominfc@gmail.com>
 * @license MIT
 */

if ( ! defined('BASEPATH')) exit('No direct script access allowed');

include_once(APPPATH."core/REST_Controller.php");
include_once(APPPATH."libraries/Utils.php");


class Apiv2 extends REST_Controller {
	public function __construct()
	{
		parent::__construct();
		$this->load->model('user_model');
	}

	public function register_post(){
		$req_data = $this->json_input();
		$params = array('status'=>0, 'updated'=>time(), 'created'=>time(),'sex'=>0,'departId'=>0);
		if(isset($req_data['name'])){
			$params['name'] = $req_data['name'];
      //check user exist
			$count = $this->user_model->getCount(array('name'=>$params['name']));
			if($count > 0){
				$this->_fail_msg_out('用户已存在!');
				return;
			}
			$parans['domain'] = Utils::getStrDomain($params['name']);
		}else {
			$this->_fail_msg_out('lost name');
			return;
		}

		if(isset($req_data['password'])){
			$salt = rand()%10000;
			$password = $req_data['password'];
			$params['password'] = md5(md5($password).$salt);
			$params['salt'] = $salt;
		}else {
			$this->_fail_msg_out('lost password');
			return;
		}

		if(isset($req_data['nick'])){
			$params['nick'] = $req_data['nick'];
		}else {
			$params['nick'] = $params['name'];
		}

		foreach(array("sex","departId","phone","email","avatar") as $value){	
			if(isset($req_data[$value])) {
				$params[$value] = $req_data[$value];
			}
		}


		$re = $this->user_model->insert($params);
		if(!$re) {
			$this->_fail_msg_out('insert failed');
			return;
		}
		$this->_ok_msg_out('add ok!');
	}

	public function _fail_msg_out($msg = '') {
		$this->json_out(array('status'=>'failed','msg'=>$msg));
	}

	public function _ok_msg_out($msg = '') {
		$this->json_out(array('status'=>'ok', 'msg'=>$msg));
	}

}
