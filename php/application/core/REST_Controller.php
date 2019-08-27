<?php
/**
 * Short description for REST_Controller.php
 *
 * @package REST_Controller
 * @author xiaominfc(武汉鸣鸾信息科技有限公司) <xiaominfc@gmail.com>
 * @version 0.1
 * @copyright (C) 2019 xiaominfc(武汉鸣鸾信息科技有限公司) <xiaominfc@gmail.com>
 * @license MIT
 */

(defined('BASEPATH')) OR exit('No direct script access allowed');


class REST_Controller extends CI_Controller {

	protected $allowed_http_methods = ['get', 'delete', 'post', 'put', 'options', 'patch', 'head'];


    protected $req_body_data = null;

    function __construct()
	{
		parent::__construct();
	}
	
	public function _remap($method, $params = array()){
		$req_method = $this->_requset_method();
		$call_func = $method.'_'.$req_method;
		try {
			if (method_exists($this, $call_func))
        	{
            	return call_user_func_array(array($this, $call_func), $params);
			}else if(method_exists($this, $method)){
				return call_user_func_array(array($this, $method), $params);
			}
		} catch (Exception $th) {
			//throw $th;
			$call_func_error = $call_func.'_error';
			$method_error = $method.'_error';
			if (method_exists($this, $call_func_error))
        	{
				log_message('info',json_encode($th));
				return call_user_func_array(array($this, $call_func_error), $th);
			}else if(method_exists($this, $method_error)){
				log_message('info',json_encode($th));
				return call_user_func_array(array($this, $method_error), $th);
			}else{
				log_message('info',"no method for error");
				throw $th;
			}
		}
		show_404();
	}

	private function _requset_method(){
		$method = $this->input->method();
		if(empty($method)){
			$method = 'get';
		}
		$method = strtolower($method);
		return in_array($method, $this->allowed_http_methods) ? $method : 'get';
	}

	public function json_out($result){
        header('Content-Type: application/json; charset=utf-8');
        echo json_encode($result);
    }

    public function json_input($index = null){       
        if(empty($req_body_data)){
            $req_body_data = json_decode(file_get_contents("php://input"),true);
            if(empty($req_body_data)){
              $req_body_data = array();           
            }
		}
        if(empty($index)){
            return $req_body_data;
        }
        return $req_body_data[$index];
    }

}
