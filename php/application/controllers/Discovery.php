<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

include_once(APPPATH."core/BaseController.php");

class Discovery extends BaseController {

	public function __construct()
	{
		parent::__construct();
		$this->load->helper('url');
		$this->load->model('discovery_model');
	}

	public function index()
	{
		$this->load->view('base/header');
		$this->load->view('base/discovery');
		$this->load->view('base/footer');
	}

	public function default_model(){
		return $this->discovery_model;
	}

	public function package_data($discoverys) {
		$result = array(
			'discoverys'=>$discoverys
		);
		return $result;
	}

	public function prepare_data($record,$action){
		$params = array(
				'itemName'=>$record['itemName'],
				'itemUrl'=>$record['itemUrl'],
				'itemPriority'=>$record['itemPriority'],
		);
		return $params;

	}

	public function action_post_error($error = array()) {
		$out_result = array('status'=>'failed','msg'=>json_encode($error));
		$this->json_out($out_result); 
	}

	public function all()
	{
		$discoverys = $this->discovery_model->getList(array('status'=>0));
		$result = array(
			'discoverys'=>$discoverys
		);
		echo json_encode($result);
	}

	public function add()
	{
		$params = array(
			'itemName'=>$this->input->post('itemName'),
			'itemUrl'=>$this->input->post('itemUrl'),
			'itemPriority'=>$this->input->post('itemPriority'),
			'created'=>time(),
			'updated'=>time()
		);
		$result = $this->discovery_model->insert($params);
		if($result){
			echo 'success';
		}
	}

	public function edit()
	{
		$params = array(
			'itemName' => $this->input->post('itemName'),
			'itemUrl' => $this->input->post('itemUrl'),
			'itemPriority' => $this->input->post('itemPriority')
		);
		$result = $this->discovery_model->update($params, $this->input->post('id'));
		if($result){
			echo 'success';
		}
	}

	public function get()
	{
		$id = $this->input->post('id');
		$result = $this->discovery_model->getOne(array('id'=>$id));
		if($result){
			echo json_encode($result);
		}
	}

	public function json()
	{
		$data = $this->discovery_model->getList(array('status'=>0));
		echo json_encode($data);
	}

	public function del()
	{
		$id = $this->input->post('id');
		$result = $this->discovery_model->update(array('status'=>1), $id);
		if($result){
			echo 'success';
		}
	}
}
