<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

include_once(APPPATH."core/BaseController.php");

class Depart extends BaseController {

	public function __construct()
	{
		parent::__construct();
		$this->load->helper('url');
		$this->load->model('depart_model');
		$this->load->model('user_model');
	}

	public function index()
	{
		$this->load->view('base/header');
		$this->load->view('base/depart');
		$this->load->view('base/footer');
	}

	public function default_model(){
		return $this->depart_model;
	}


	//override package data
	public function package_data($departs) {
		$data = array();
		foreach ($departs as $key => $value) {
			$data[$value['id']] = $value;
		}
		foreach ($departs as $key => $value) {
			if($value['parentId']){
				$departs[$key]['parentId_value'] = $data[$value['parentId']]['departName'];
			}else{
				$departs[$key]['parentId_value'] = '当前部门是父部门';
			}
		}

		$result = array(
			'departs'=>$departs
		);
		return $result;
	}

	public function prepare_data($record, $action){
		$params = array(
			'departName'=>$record['departName'],
			'priority'=>$record['priority'],
			'parentId'=>$record['parentId'],
			//'updated'=>time(),
		);
		return $params;	
	}

	public function remove_record($id){
			$departCount = $this->depart_model->getCount(array('status'=>0,'parentId'=>$id));
			if($departCount){
				$out_result['status'] = 'failed';
				$out_result['msg'] = "has departs";
				$this->json_out($out_result); 
				exit();
			}
			$userCount = $this->user_model->getCount(array('status'=>0,'departId'=>$id));
			if($userCount){
				$out_result['status'] = 'failed';
				$out_result['msg'] = "has users";
				$this->json_out($out_result); 
				exit();
			}
			$this->depart_model->update(array('status'=>3), $id);
			$this->json_out(array('status'=>'ok','msg'=>''));
	}

	public function action_post(){
		$req_data = $this->json_input();
		$out_result = array('status'=>'ok','msg'=>'');
		$action = $req_data['method'];
		$pid = $req_data['pid'];
		if("delete" == $action){
			$id =  $req_data['id'];
			$departCount = $this->depart_model->getCount(array('status'=>0,'parentId'=>$id));
			if($departCount){
				$out_result['status'] = 'failed';
				$out_result['msg'] = "has departs";
				$this->json_out($out_result); 
				exit();
			}
			$userCount = $this->user_model->getCount(array('status'=>0,'departId'=>$id));
			if($userCount){
				$out_result['status'] = 'failed';
				$out_result['msg'] = "has users";
				$this->json_out($out_result); 
				exit();
			}
			$result = $this->depart_model->update(array('status'=>3), $id);
			$msg = "";
		}else if('add' == $action || 'update' == $action) {
			$record = $req_data['record'];
			$params = array(
				'departName'=>$record['departName'],
				'priority'=>$record['priority'],
				'parentId'=>$record['parentId'],
				'updated'=>time(),
			);

			if('add' == $action){
				$params['created'] = time();
				$params['pid'] = $pid;
				$result = $this->depart_model->insert($params);
				if(!$result){
					$out_result['status'] = 'failed';
					$out_result['msg'] = "insert failed";
				}
			}else {
				$id = $record['id'];
				$result = $this->depart_model->update($params,$id);
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



	public function all()
	{
		$perpage = 10000;
		$departs = $this->depart_model->getList(array('status'=>0), '*', 0, $perpage);
		$data = array();
		foreach ($departs as $key => $value) {
			$data[$value['id']] = $value;
		}
		foreach ($departs as $key => $value) {
			if($value['parentId']){
				$departs[$key]['parentId_value'] = $data[$value['parentId']]['departName'];
			}else{
				$departs[$key]['parentId_value'] = '当前部门是父部门';
			}
		}
		$result = array(
			'departs'=>$departs,
		);
		echo json_encode($result);
	}

	public function del()
	{
		$id = $this->input->post('id');
		$departCount = $this->depart_model->getCount(array('status'=>0,'parentId'=>$id));
		if($departCount){
			echo "has departs";
			exit();
		}
		$userCount = $this->user_model->getCount(array('status'=>0,'departId'=>$id));
		if($userCount){
			echo "has users";
			exit();
		}
		$result = $this->depart_model->update(array('status'=>1), $id);
		if($result){
			echo 'success';
		}
	}

	public function add()
	{
		$params = array(
			'departName'=>$this->input->post('departName'),
			'priority'=>$this->input->post('priority'),
			'parentId'=>$this->input->post('parentId'),
			'created'=>time(),
			'updated'=>time()
		);
		$result = $this->depart_model->insert($params);
		if($result){
			echo 'success';
		}
	}

	public function edit()
	{
		$params = array(
			'departName'=>$this->input->post('departName'),
			'priority'=>$this->input->post('priority'),
			'parentId'=>$this->input->post('parentId'),
			'updated'=>time()
		);
		$id = $this->input->post('id');
		$result = $this->depart_model->update($params,$id);
		if($result){
			echo 'success';
		}
	}

	public function get()
	{
		$id = $this->input->post('id');
		$result = $this->depart_model->getOne(array('id'=>$id));
		if($result){
			echo json_encode($result);
		}
	}
}
