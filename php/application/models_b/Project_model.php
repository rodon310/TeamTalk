<?php
/**
 * Short description for Project_model.php
 *
 * @package Project_model
 * @author xiaominfc(武汉鸣鸾信息科技有限公司) <xiaominfc@gmail.com>
 * @version 0.1
 * @copyright (C) 2019 xiaominfc(武汉鸣鸾信息科技有限公司) <xiaominfc@gmail.com>
 * @license MIT
 */

include_once(APPPATH."core/TT_Model.php");
class Project_model extends TT_Model {
	function __construct(){
		parent::__construct();
		$this->table_name = "IMProject";
	}
}
