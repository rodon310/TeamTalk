<?php
/**
 * Short description for Project.php
 *
 * @package Project
 * @author xiaominfc(武汉鸣鸾信息科技有限公司) <xiaominfc@gmail.com>
 * @version 0.1
 * @copyright (C) 2020 xiaominfc(武汉鸣鸾信息科技有限公司) <xiaominfc@gmail.com>
 * @license MIT
 */

if ( ! defined('BASEPATH')) exit('No direct script access allowed');

include_once(APPPATH."core/TT_Controller.php");

class Project extends TT_Controller {

	public function __construct()
	{
		parent::__construct();
		$this->load->model('project_model');
	}

	public function action_get(){
		$pageSize =  $this->input->get('pageSize');
		$currentPage = $this->input->get('currentPage');
		if(empty($currentPage)){
			$currentPage = 1;
		}

		if(empty($pageSize)){
			$pageSize = 10;
		}
		//creatorid
		$cid = $this->session->userdata['account']['id'];
		$projects = $this->project_model->getList(array('status'=>0,'creatorId'=>$cid), '*',($currentPage-1)*$pageSize , $pageSize);	
		$count = $this->project_model->getCount(array('status'=>0));
		$result = array(
			'pagination'=> array(
				'current'=>intval($currentPage),
				'total'=>$count,
				'pageSize'=>intval($pageSize),
			),
			'projects'=>$projects
		);
		$this->json_out($result);
	}


	public function action_post() {

		$req_data = $this->json_input();
		$out_result = array('status'=>'ok','msg'=>'');
		$action = $req_data['method'];
		if("delete" == $action){
			$id =  $req_data['id'];
			$result = $this->project_model->update(array('status'=>3), $id);
			$msg = "";
		}else if('add' == $action || 'update' == $action) {
			$record = $req_data['record'];
			$params = array(
				'name'=>$record['name'],
				'type'=>$record['type'],
				//'status'=>0,
				'updated'=>time()
			);		
			if('add' == $action){
				$cid = $this->session->userdata['account']['id'];
				$params['created'] = time();
				$params['creatorId'] = $cid;
				$result = $this->project_model->insert($params);
				if(!$result){
					$out_result['status'] = 'failed';
					$out_result['msg'] = "insert failed";
				}
			}else {
				$id = $record['id'];
				$result = $this->project_model->update($params,$id);
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
