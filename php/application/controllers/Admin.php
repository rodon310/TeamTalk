<?php
/**
 * Short description for Admin.php
 *
 * @package Admin
 * @author xiaominfc(武汉鸣鸾信息科技有限公司) <xiaominfc@gmail.com>
 * @version 0.1
 * @copyright (C) 2019 xiaominfc(武汉鸣鸾信息科技有限公司) <xiaominfc@gmail.com>
 * @license MIT
 */

if (!defined('BASEPATH')) exit('No direct script access allowed');

include_once(APPPATH."core/TT_Controller.php");

class Admin extends TT_Controller {

	public function __construct()
	{
		parent::__construct();
		$this->load->helper('url');
		$this->load->model('admin_model');
	}

	public function action_get() {
		$pageSize =  $this->input->get('pageSize');
		$currentPage = $this->input->get('currentPage');
		if(empty($currentPage)){
			$currentPage = 1;
		}

		if(empty($pageSize)){
			$pageSize = 10;
		}
		$admin_users = $this->admin_model->getList(array('status'=>0),'*', ($currentPage-1)*$pageSize , $pageSize);
		$count = $this->admin_model->getCount(array('status'=>0));
		$result = array(
			'pagination'=> array(
				'current'=>intval($currentPage),
				'total'=>$count,
				'pageSize'=>intval($pageSize),
			),
			'admin_users'=>$admin_users
		);
		$this->json_out($result);     
	}

	public function action_post() {

		$req_data = $this->json_input();
		$out_result = array('status'=>'ok','msg'=>'');
		$action = $req_data['method'];
		if("delete" == $action){
			$id =  $req_data['id'];
			$result = $this->admin_model->update(array('status'=>3), $id);
			$msg = "";
		}else if('add' == $action || 'update' == $action) {
			$record = $req_data['record'];

			$params = array(
				'uname'=>$record['uname'],
				'type'=>$record['type'],
				'updated'=>time()
			);

			$password = '';
			if(isset($record['pwd'])) {
				$password = $record['pwd'];
			}


			if(!empty($password)){
				$params['pwd'] = md5($password);
			}else if("add" == $action) {
				$out_result['status'] = 'failed';
				$out_result['msg'] = "lost password";
				$this->json_out($out_result);  
				return;
			}

			if('add' == $action){
				$params['created'] = time();
				$result = $this->admin_model->insert($params);
				if(!$result){
					$out_result['status'] = 'failed';
					$out_result['msg'] = "insert failed";
				}
			}else {
				$id = $record['id'];
				$result = $this->admin_model->update($params,$id);
				if(!$result){
					$out_result['status'] = 'failed';
					$out_result['msg'] = "update failed";
				}
			}

		}else {
			$out_result['msg'] = "no such ".$action;
		}
		$this->json_out($out_result);   
	}

}

