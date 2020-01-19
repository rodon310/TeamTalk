<?php
/**
 * Short description for BaseController.php
 *
 * @package BaseController
 * @author xiaominfc(武汉鸣鸾信息科技有限公司) <xiaominfc@gmail.com>
 * @version 0.1
 * @copyright (C) 2020 xiaominfc(武汉鸣鸾信息科技有限公司) <xiaominfc@gmail.com>
 * @license MIT
 */

// write for same code for depart user group disconvery
//

(defined('BASEPATH')) OR exit('No direct script access allowed');

include_once(APPPATH."core/TT_Controller.php");

class BaseController extends TT_Controller {
	public function __construct(){
		parent::__construct();
		$this->load->model('project_model');
	}

	public function default_model(){

	}

	public function authIdForPid($id, $pid) {
		$count = $this->default_model()->getCount(array('id'=>$id,'pid'=>$pid));
		return $count > 0;
	}

	public function authPid($pid) {
		$cid = $this->session->userdata['account']['id'];
		$count = $this->project_model->getCount(array('id'=>$pid,'creatorId'=>$cid));
		return $count > 0;
	}

	public function package_data($data){
		return $data;
	}

	public function action_get(){
		$data = $this->page_data();
		if(!empty($data)) {
			$this->json_out($data);
		}
	}

	public function page_data(){
		$pageSize =  $this->input->get('pageSize');
		$currentPage = $this->input->get('currentPage');
		$pid = $this->input->get('pid');
		if(empty($currentPage)){
			$currentPage = 1;
		}

		if(empty($pageSize)){
			$pageSize = 10;
		}
		if(empty($pid)){
			$this->json_out(array());
			return;
		}

		$data = $this->default_model()->getList(array('status'=>0,'pid'=>$pid), '*',($currentPage-1)*$pageSize , $pageSize);
		$out_data = $this->package_data($data);
		$count = $this->default_model()->getCount(array('status'=>0,'pid'=>$pid));
		$out_data['pagination']=array(
			'current'=>intval($currentPage),
			'total'=>$count,
			'pageSize'=>intval($pageSize),
		);
		return $out_data;
	}

	public function prepare_data($record,$action) {
		return $record;
	}

	public function remove_record($id){
		$this->default_model()->update(array('status'=>3), $id);
		$this->json_out(array('status'=>'ok', 'msg'=>''));
	}

	public function add_record($params){
		$this->default_model()->insert($params);
		$this->json_out(array('status'=>'ok', 'msg'=>'')); 
	}

	public function update_record($params,$id){
		$result = $this->default_model()->update($params,$id);
		if(!$result){
			$this->json_out(array('status'=>'failed', 'msg'=>'update failed'));
			return;
		}
		$this->json_out(array('status'=>'ok', 'msg'=>''));
	}


	public function action_post(){
		$req_data = $this->json_input();
		$pid = $req_data['pid'];
		if(empty($pid) || (!$this->authPid($pid))){
			$this->json_out(array('status'=>'failed', 'msg'=>'not control the pid'));
			return;
		}
		$action = $req_data['method'];
		if("delete" == $action){
			$id =  $req_data['id'];
			if($this->authIdForPid($id, $pid)){
				$this->remove_record($id);
			}else {
				$this->json_out(array('status'=>'failed', 'msg'=>'id not in pid'));
			}
			return;
		}else if("add" == $action || "update" == $action) {
			$record = $req_data['record'];
			$params = $this->prepare_data($record,$action);
			if(empty($params)) {
				return;
			}
			$params['updated']=time();
			if("add" == $action) {
				$params['pid'] = $pid;
				$params['created']=time();
				$this->add_record($params);
			}else {
				$id = $record['id'];
				if($this->authIdForPid($id, $pid)){
					$this->update_record($params,$id);
					return;
				}else {
					$this->json_out(array('status'=>'failed', 'msg'=>'id not in pid'));  
				}
			}
			return;
		}
		$this->json_out(array('status'=>'failed', 'msg'=>'not work for that action'));
	}
} 


