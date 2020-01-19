<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

include_once(APPPATH."core/BaseController.php");

class Group extends BaseController {

	public function __construct()
	{
		parent::__construct();
		$this->load->helper('url');
		$this->load->model('group_model');
		$this->load->model('user_model');
		$this->load->model('grouprelation_model');
	}

	public function index()
	{
		$this->load->view('base/header');
		$this->load->view('base/group');
		$this->load->view('base/footer');
	}


	public function default_model() {
		return $this->group_model;
	}

	public function package_data($groups){
		foreach ($groups as $key => $value) {
			if($groups[$key]['avatar']){
				if(substr($groups[$key]['avatar'], 0, 4) === "http") {
					$groups[$key]['avatar_value'] = $groups[$key]['avatar'];
				}else {
					$groups[$key]['avatar_value'] = $this->config->config['msfs_url'].$groups[$key]['avatar'];	
				}
			}
		}
		$result = array(
			'groups'=>$groups
		);
		return $result;
	}

	//init base info for add or update
	public function prepare_data($record,$action){
			return $record;
	}

	//add new record
	public function add_record($record) {
		$avatar ='';
		if(isset($record['avatar'])) {
			$avatar = $record['avatar'];
		}
		$array = array(
			'req_user_id' 	=> 0,
			'app_key'		=> 'asdfasdf',
			'group_name'	=> $record['name'],
			'group_type'	=> intval($record['type']),
			'group_avatar'	=> $avatar,
			'user_id_list'	=> array()
		);
		$res = $this->httpRequest($this->config->config['http_url'].'/query/CreateGroup','post',json_encode($array));
		log_message('info','result:'.$res);
		$res = json_decode($res,1);
		if(!$res || $res['error_code'] != 0){
			$this->json_out(array('status'=>'failed', 'msg'=>'insert failed'));
			return;
		}
		$this->json_out(array('status'=>'ok', 'msg'=>''));
	}

	public function update_record($record){
		$avatar ='';
		if(isset($record['avatar'])){
			$avatar = $record['avatar'];
		}
		$id = $record['id'];
		$params = array(
			'name'	=> $record['name'],
			'type'	=> $record['type'],
			'avatar'	=> $avatar,
		);
		$result = $this->default_model()->update($params,$id);
		if(!$result){
			$this->json_out(array('status'=>'failed', 'msg'=>'update failed')); 
			return;
		}else {
			$this->default_model()->autoInc($id,'version');
		}
		$this->json_out(array('status'=>'ok', 'msg'=>''));
	}

	public function editmember_post() {
		$req_data = $this->json_input();
		log_message('info',json_encode($req_data));
		$args = array(
			'req_user_id'   => 0,
			'app_key'       => 'asdfasdf',
			'group_id'      => intval($req_data['id']),
			'modify_type'   => intval($req_data['change']),
			'user_id_list'  => array(intval($req_data['userId']))
		);                  
		$res = $this->httpRequest($this->config->config['http_url'].'/query/ChangeMembers','post',json_encode($args));
		$this->json_out($res);   
	}

	public function all()
	{
		$perpage = 10000;
		$groups = $this->group_model->getList(array('status'=>0), '*', 0, $perpage);
		foreach ($groups as $key => $value) {
			if($groups[$key]['avatar']){

				if(substr($groups[$key]['avatar'], 0, 4) === "http") {
					$groups[$key]['avatar_value'] = $groups[$key]['avatar'];
				}else {
					$groups[$key]['avatar_value'] = $this->config->config['msfs_url'].$groups[$key]['avatar'];	
				}
			}
		}
		$result = array(
			'groups'=>$groups,
		);
		echo json_encode($result);
	}

	public function del()
	{
		$id = $this->input->post('id');
		$result = $this->group_model->updateByWhere(array('status'=>1), 'id', $id);
		if($result){
			echo 'success';
		}
	}

	public function add()
	{
		$array = array(
			'req_user_id' 	=> 0,
			'app_key'		=> 'asdfasdf',
			'group_name'	=> $this->input->post('name'),
			'group_type'	=> 1,
			'group_avatar'	=> '',
			'user_id_list'	=> array(1)
		);
		$res = $this->httpRequest($this->config->config['http_url'].'/query/CreateGroup','post',json_encode($array));
		$res = json_decode($res,1);
		if($res['error_code'] == 0){
			echo 'success';
		}else{
			echo 'fail';
		}
	}

	public function edit()
	{
		$params = array(
			'name'=>$this->input->post('name'),
			'avatar'=>$this->input->post('avatar'),
			'adesc'=>$this->input->post('adesc'),
			'createUserId'=>1,
			'type'=>1,
			'userCnt'=>0,
			'status'=>0,
			'updated'=>time()
		);
		$id = $this->input->post('id');
		$result = $this->group_model->updateByWhere($params, 'id', $id);
		if($result){
			echo 'success';
		}
	}

	public function get()
	{
		$id = $this->input->post('id');
		$result = $this->group_model->getOne(array('id'=>$id));
		if($result){
			echo json_encode($result);
		}
	}

	public function getMember()
	{
		$id = $this->input->get('id');
		if(empty($id)) {
			$id = $this->input->post('id');
		}
		$perpage = 10000;
		$users = $this->grouprelation_model->getList(array('status'=>0,'groupId'=>$id), '*', 0, $perpage);
		foreach ($users as $key => $value) {
			$_data = $this->user_model->getOne(array('id'=>$value['userId']));
			$users[$key]['name'] = $_data['name'];
		}
		$data = array();
		$result = array(
			'users'=>$users,
		);
		echo json_encode($result);
	}

	public function changeMember()
	{
		$add = array(
			'req_user_id'   => 0,
			'app_key'       => 'asdfasdf',
			'group_id'      => intval($this->input->post('id')),
			'modify_type'   => intval($this->input->post('change')),
			'user_id_list'  => array(intval($this->input->post('userId')))
		);                  
		$res = $this->httpRequest($this->config->config['http_url'].'/query/ChangeMembers','post',json_encode($add));
		$this->json_out($res);
	}

	public function httpRequest($url,$method,$params=array()){
		$curl=curl_init();
		curl_setopt($curl,CURLOPT_RETURNTRANSFER,1);
		curl_setopt($curl,CURLOPT_HEADER,0 ) ;
		curl_setopt($curl,CURLOPT_URL,$url);
		curl_setopt($curl,CURLOPT_POST,1 );
		curl_setopt($curl, CURLOPT_POSTFIELDS,$params);
		$result=curl_exec($curl);
		curl_close($curl);
		return $result;
	}

}
