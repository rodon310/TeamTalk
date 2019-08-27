<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

include_once(APPPATH."core/TT_Controller.php");

class Discovery extends TT_Controller {

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


	public function action_get(){
        $pageSize =  $this->input->get('pageSize');
		$currentPage = $this->input->get('currentPage');
		if(empty($currentPage)){
			$currentPage = 1;
        }

        if(empty($pageSize)){
            $pageSize = 10;
        }
		$discoverys = $this->discovery_model->getList(array('status'=>0),'*', ($currentPage-1)*$pageSize , $pageSize);
		$count = $this->discovery_model->getCount(array('status'=>0));
		$result = array(
            'pagination'=> array(
			   'current'=>intval($currentPage),
               'total'=>$count,
               'pageSize'=>intval($pageSize),
             ),
			 'discoverys'=>$discoverys
        );
        $this->json_out($result);
	}


	public function action_post() {
		$req_data = $this->json_input();
		$out_result = array('status'=>'ok','msg'=>'');
		$action = $req_data['method'];
		if("delete" == $action){
			$id =  $req_data['id'];
			$result = $this->discovery_model->update(array('status'=>3), $id);
			$msg = "";
		}else if('add' == $action || 'update' == $action) {
			$record = $req_data['record'];
			$params = array(
				'itemName'=>$record['itemName'],
				'itemUrl'=>$record['itemUrl'],
				'itemPriority'=>$record['itemPriority'],
				'updated'=>time()
			);

			if('add' == $action){
				$params['created'] = time();
				$result = $this->discovery_model->insert($params);
				if(!$result){
					$out_result['status'] = 'failed';
					$out_result['msg'] = "insert failed";
				}
			}else {
				$id = $record['id'];
				$result = $this->discovery_model->update($params,$id);
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